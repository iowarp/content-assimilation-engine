#include <errno.h>
#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif


#ifdef USE_HERMES
#include <hermes/hermes.h>
#include <hermes/data_stager/stager_factory.h>
#endif

#ifdef USE_POCO
#include <chrono>
#include <iomanip>
#include <thread>
#include "Poco/DigestEngine.h"
#include "Poco/Exception.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/Net/Context.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/NullStream.h"
#include "Poco/Path.h"
#include "Poco/Pipe.h"
#include "Poco/PipeStream.h"
#include "Poco/Process.h"
#include "Poco/SHA2Engine.h"
#include "Poco/SharedMemory.h"
#include "Poco/StreamCopier.h"
#include "Poco/TemporaryFile.h"
#include "Poco/URI.h"
#endif

#ifdef USE_AWS
#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/utils/memory/stl/AWSStringStream.h>
#include <aws/core/utils/StringUtils.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#endif

#include "OMNI.h"

int read_exact_bytes_from_offset(const char *filename, off_t offset,
                                 size_t num_bytes, unsigned char *buffer);

#ifdef USE_POCO
std::string sha256_file(const std::string& filePath) {
    try {
      
      Poco::FileInputStream fis(filePath);
        
      Poco::SHA2Engine sha256(Poco::SHA2Engine::SHA_256);
        
      const size_t bufferSize = 8192;
      char buffer[bufferSize];
        
      while (!fis.eof()) {
	fis.read(buffer, bufferSize);
	std::streamsize bytesRead = fis.gcount();
	if (bytesRead > 0) {
	  sha256.update(buffer, static_cast<unsigned>(bytesRead));
	}
      }
        
      const Poco::DigestEngine::Digest& digest = sha256.digest();
        
      std::stringstream ss;
      for (unsigned char b : digest) {
	ss << std::hex << std::setfill('0') << std::setw(2)
	   << static_cast<int>(b);
      }
        
      return ss.str();
    }
    catch (const Poco::Exception& ex) {
      throw std::runtime_error("Error: calculating SHA256 - "
			       + ex.displayText());
    }
}
#endif

int write_meta(std::string name, std::string tags) {
  std::filesystem::path file_path = ".blackhole/ls";
  std::ofstream outfile(file_path, std::ios::out | std::ios::app);
  if (outfile.is_open()) {
    outfile << name << "|";
    outfile << tags << std::endl;
    outfile.close();  
  }
  return 0;
}

#ifdef USE_HERMES
int put_hermes_tags(hermes::Context* ctx, hermes::Bucket* bkt, std::string tags) {
  hermes::Blob blob(tags.size());
  memcpy(blob.data(), tags.c_str(), blob.size());
  hermes::BlobId blob_id = bkt->Put("metadata", blob, *ctx);
  if (blob_id.IsNull()) {
    std::cerr << "Error: putting tags into metadata BLOB failed"
	      << std::endl;    
    return -1;
  }
  return 0;
}

int put_hermes(std::string name, std::string tags, std::string path,
	       unsigned char* buffer, int nbyte) {

  hermes::Context ctx;
  hermes::Bucket bkt(name);
  hermes::Blob blob(nbyte);
  memcpy(blob.data(), buffer, blob.size());
  hermes::BlobId blob_id = bkt.Put(path, blob, ctx);
  std::cout << "wrote '" << buffer << "' to '" << name << "' buffer."
	      << std::endl;
  return put_hermes_tags(&ctx, &bkt, tags);
}

int get_hermes(std::string name, std::string path) {

  auto bkt = HERMES->GetBucket(name);
  hermes::BlobId blob_id = bkt.GetBlobId(path);
  hermes::Blob blob2;
  hermes::Context ctx;  
  bkt.Get(blob_id, blob2, ctx);
  std::cout << "read '" << blob2.data() << "' from '" << name << "' buffer."
	      << std::endl;

  // Create a stageable bucket
  hermes::Context ctx_s = hermes::BinaryFileStager::BuildContext(30);
  hermes::Bucket bkt_s(name, ctx_s, 30);
  hermes::Blob blob3(30);
  memcpy(blob3.data(), blob2.data(), blob2.size());
  bkt_s.Put(path, blob3, ctx_s);
  CHI_ADMIN->Flush(HSHM_MCTX, chi::DomainQuery::GetGlobalBcast());  
  return 0;

}

#endif

int put(std::string name, std::string tags, std::string path,
	unsigned char* buffer, int nbyte) {

#ifdef USE_HERMES
  put_hermes(name, tags, path, buffer, nbyte);
#endif
#ifdef USE_POCO
  const std::size_t sharedMemorySize = nbyte+1;

  try {
    Poco::File file(name);
    std::cout << "checking existing buffer '" << name << "'...";
    if (file.exists()) {
      std::cout << "yes" << std::endl;
      return 0;
    }
    std::cout << "no" << std::endl;

    std::cout << "creating a new buffer '" << name << "' with '"
	      << tags << "' tags...";
    file.createFile();
    std::ofstream ofs(name, std::ios::binary);
    ofs.seekp(nbyte);
    ofs.put('\0');
    ofs.close();
    std::cout << "done" << std::endl;

    std::cout << "putting " << nbyte << " bytes into '"
	      <<  name << "' buffer...";
    Poco::SharedMemory shm(file, Poco::SharedMemory::AM_WRITE);
	    
    char* data = static_cast<char*>(shm.begin());
    std::memcpy(data, (const char *)buffer, nbyte);
    std::cout << "done" << std::endl;
#ifdef DEBUG    
    std::cout << "wrote '" << shm.begin() << "' to '" << name << "' buffer."
	      << std::endl;
#endif

  } catch (Poco::Exception& e) {
    std::cerr << "Poco Exception: " << e.displayText() << std::endl;
    return -1;
  } catch (std::exception& e) {
    std::cerr << "Standard Exception: " << e.what() << std::endl;
    return -1;
  }
#endif
  return write_meta(name, tags);  
}

#ifdef _WIN32
std::string get_ext(const std::string& filename) {

  std::filesystem::path p(filename);
  std::string extension  =  p.extension().string();

  for (char &c : extension) {
    c = std::tolower(c);
  }
  return extension;
}
#endif

int run_lambda(std::string lambda, std::string name, std::string dest) {
#ifdef _WIN32
  std::string extension = get_ext(lambda);
  if (extension != ".bat") {
    std::cerr << "Error: lambda script extension must be '.bat' on Windows"
	      << std::endl;
    return 1;
  }
#endif

#ifdef USE_POCO  
  try {
#ifdef _WIN32      
    std::string command = "cmd.exe";
#else
    std::string command = lambda;
#endif	
    Poco::Process::Args args;
#ifdef _WIN32	
	args.push_back("/C");
	Poco::Path pocoPath(lambda);
	std::string wpath = pocoPath.toString(Poco::Path::PATH_WINDOWS);
	args.push_back(wpath);
#endif
	args.push_back(name);
	args.push_back(dest);
	
        Poco::Pipe outPipe;
        Poco::Pipe errPipe;
        Poco::ProcessHandle ph = Poco::Process::launch(
            command,
            args,
            0,
            &outPipe,
            &errPipe 
        );


        Poco::PipeInputStream istr(outPipe);
        Poco::PipeInputStream estr(errPipe);

        std::string stdout_output;
        std::string stderr_output;

        Poco::StreamCopier::copyToString(istr, stdout_output);
        Poco::StreamCopier::copyToString(estr, stderr_output);

        int exitCode = ph.wait();

        std::cout << "\n--- Lambda script output ---\n";
        std::cout << "STDOUT:\n" << stdout_output;
        std::cout << "STDERR:\n" << stderr_output;
        std::cout << "-----------------------------------\n";
        std::cout << "Lambda script exited with status: " << exitCode
		  << std::endl;

    } catch (Poco::SystemException& exc) {
        std::cerr << "Error: poco system exception - "
		  << exc.displayText() << std::endl;
        return 1;
    } catch (Poco::Exception& exc) {
        std::cerr << "Error: poco exception - "
		  << exc.displayText() << std::endl;
        return 1;
    } catch (std::exception& exc) {
        std::cerr << "Error: standard exception - "
		  << exc.what() << std::endl;
        return 1;
    }
#endif    
    return 0;  
}

std::string get_file_name(const std::string& uri) {

    size_t lastSlashPos = uri.find_last_of('/');

    if (lastSlashPos == std::string::npos) {
        size_t protocolEnd = uri.find("://");
        if (protocolEnd != std::string::npos) {
            return "";
        }
        std::string filename = uri;
        size_t queryPos = filename.find('?');
        if (queryPos != std::string::npos) {
            filename = filename.substr(0, queryPos);
        }
        size_t fragmentPos = filename.find('#');
        if (fragmentPos != std::string::npos) {
            filename = filename.substr(0, fragmentPos);
        }
        return filename;
    }

    std::string filenameWithParams = uri.substr(lastSlashPos + 1);

    size_t queryPos = filenameWithParams.find('?');
    if (queryPos != std::string::npos) {
        filenameWithParams = filenameWithParams.substr(0, queryPos);
    }

    size_t fragmentPos = filenameWithParams.find('#');
    if (fragmentPos != std::string::npos) {
        filenameWithParams = filenameWithParams.substr(0, fragmentPos);
    }

    return filenameWithParams;
}

#ifdef USE_AWS
int write_s3(std::string dest, char* ptr) {
  
  Aws::SDKOptions options;
  options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;

  const Aws::String prefix = "s3://";
  Aws::InitAPI(options);	  
  if (dest.find(prefix) != 0) {
    std::cerr << "Error: not a valid S3 URL (missing 's3://' prefix)"
	      << std::endl;
    return -1;
  }
  Aws::String path = dest.substr(prefix.length());
  // Find the first '/' to separate bucket and key
  size_t first_slash = path.find('/');
  if (first_slash == Aws::String::npos) {
    std::cerr << "Error: invalid S3 URI (no path separator found)"
	      << std::endl;
    return -1;
  }

  Aws::String bucket_name = path.substr(0, first_slash);
  Aws::String object_key = path.substr(first_slash + 1);

  if (bucket_name.empty())   {
    std::cerr << "Error: bucket name is empty" << std::endl;
    return -1;
  }
  if (object_key.empty())    {
    std::cerr << "Error: object key is empty" << std::endl;
    return -1;
  }

  Aws::Client::ClientConfiguration clientConfig;
  clientConfig.endpointOverride = "localhost:4566";
  clientConfig.scheme = Aws::Http::Scheme::HTTP;
  clientConfig.region = "us-east-1";
  clientConfig.verifySSL = false;	
  clientConfig.useDualStack = false;

  Aws::Auth::AWSCredentials credentials("test", "test");

  Aws::S3::S3Client
    s3_client(clientConfig,
	      Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy::Never, false);
  Aws::S3::Model::CreateBucketRequest createBucketRequest;
  createBucketRequest.SetBucket(bucket_name);

  std::cout << "Creating bucket: " << bucket_name << std::endl;
  auto createBucketOutcome = s3_client.CreateBucket(createBucketRequest);
  if (createBucketOutcome.IsSuccess()) {
    std::cout << "Bucket created successfully!" << std::endl;
  } else {
    std::cerr << "Error creating bucket: "
	      <<  createBucketOutcome.GetError().GetMessage() << std::endl;
  }
	
  // Create a PutObject request
  Aws::S3::Model::PutObjectRequest put_request;
  put_request.SetBucket(bucket_name);
  put_request.SetKey(object_key);

  if (ptr == NULL) {
    std::string filePath = get_file_name(dest);
    std::shared_ptr<Aws::FStream> inputData =
      Aws::MakeShared<Aws::FStream>("SampleAllocationTag",
				    filePath.c_str(),
				    std::ios_base::in | std::ios_base::binary);
    if (!*inputData)
    {
        std::cerr << "Error: Unable to open file " << filePath << std::endl;
        return false;
    }
    put_request.SetBody(inputData);

  }
  else {
    auto input_data =
      Aws::MakeShared<Aws::StringStream>("PutObjectInputStream");
    *input_data << ptr;
    put_request.SetBody(input_data);
  }

  // TODO: Set content type based on mime input in OMNI.
  // put_request.SetContentType("text/plain");
  put_request.SetContentType("application/octet-stream");

  // Execute the PutObject request
  auto put_object_outcome = s3_client.PutObject(put_request);

  if (put_object_outcome.IsSuccess())
    {
      std::cout << "Successfully uploaded object to " << bucket_name
		<< "/" << object_key << std::endl;
    }
  else
    {
      std::cout << "Error: uploading object - " 
		<< put_object_outcome.GetError().GetMessage() << std::endl;
      return -1;
    }
  Aws::ShutdownAPI(options);  
  return 0;
  
}
#endif	  

int download(const std::string& url, const std::string& outputFileName,
		 long long startByte, long long endByte = -1)
{
  
#ifdef USE_POCO    
    try {
      std::string currentUrl = url;
      const std::string caCertFile = "../cacert.pem"; 

      Poco::Net::Context::Ptr context
	= new Poco::Net::Context(Poco::Net::Context::CLIENT_USE,
				 "", 
				 "", "",
				 Poco::Net::Context::VERIFY_NONE, // STRICT
				 9, true, caCertFile);	

      int redirectCount = 0;
      int maxRedirects = 20; // Match Chrome & Firefox
      bool redirected = true;

      while (redirected && redirectCount <= maxRedirects) {
	  
	redirected = false;
	redirectCount++;
	Poco::URI uri(currentUrl);
	    
	std::unique_ptr<Poco::Net::HTTPClientSession> session;

	if (uri.getScheme() == "https") {
	  session = std::make_unique<Poco::Net::HTTPSClientSession>
	    (uri.getHost(), uri.getPort() == 0 ? 443 : uri.getPort(),
	     context);
	} else {
	  session = std::make_unique<Poco::Net::HTTPClientSession>
	    (uri.getHost(), uri.getPort());
	}

	Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET,
				       uri.getPathAndQuery(),
				       Poco::Net::HTTPMessage::HTTP_1_1);
	request.set("User-Agent", "POCO HTTP Redirect Client/1.0");


        // Set the Range header
	if (startByte >= 0) {
	  std::string rangeHeaderValue;
	  if (endByte == -1) { // Request bytes from startByte to end of file
            rangeHeaderValue = "bytes=" +
	      std::to_string(startByte) + "-";
	  } else { // Request a specific range
            rangeHeaderValue = "bytes=" +
	      std::to_string(startByte) + "-"
	      + std::to_string(endByte);
	  }
	  request.set("Range", rangeHeaderValue);
	  std::cout << "Requesting Range: " << rangeHeaderValue << std::endl;
	}

	Poco::Net::HTTPResponse response;

	int status = 0;

	std::cout << "Downloading from: " << url << std::endl;

	session->sendRequest(request);
	std::istream& rs = session->receiveResponse(response);	  
	status = response.getStatus();
	std::cout << "Status: " << status << " - " << response.getReason()
		  << std::endl;


	if (status == Poco::Net::HTTPResponse::HTTP_MOVED_PERMANENTLY ||
	    status == Poco::Net::HTTPResponse::HTTP_FOUND ||
	    status == Poco::Net::HTTPResponse::HTTP_SEE_OTHER ||
	    status == Poco::Net::HTTPResponse::HTTP_TEMPORARY_REDIRECT ||
	    status == Poco::Net::HTTPResponse::HTTP_PERMANENT_REDIRECT)
	  {
	    if (response.has("Location")) {
	      currentUrl = response.get("Location");
	      std::cout << "Redirected to: " << currentUrl << std::endl;
	      redirected = true;  
	      // Consume any remaining data in the current response stream
	      Poco::NullOutputStream nullStream;
	      Poco::StreamCopier::copyStream(rs, nullStream);
	    } else {
	      std::cerr << "Redirect status (" << status
			<< ") received but no Location header found."
			<< std::endl;
	      return -1;
	    }
	  }
	else if (status == Poco::Net::HTTPResponse::HTTP_PARTIAL_CONTENT) {
	  std::cout << "Received Partial Content (206)." << std::endl;
	  if (response.has("Content-Range")) {
	    std::string contentRange = response.get("Content-Range");
	    std::cout << "Content-Range: " << contentRange << std::endl;
	  } else {
	    std::cout << "Warning: 206 status but no Content-Range header." << std::endl;
	  }

	  std::ofstream os(outputFileName, std::ios::binary);
	  if (os.is_open()) {
	    Poco::StreamCopier::copyStream(rs, os);
	    os.close();
	    std::cout << "Partial content downloaded successfully to: " << outputFileName << std::endl;
	  } else {
	    std::cerr << "Error: Could not open file for writing: " << outputFileName << std::endl;
	  }	  
	  
	}
	else if (status == Poco::Net::HTTPResponse::HTTP_OK) {
	  // Success! Download the content
	  std::ofstream os(outputFileName, std::ios::binary);
	  if (os.is_open()) {
	    Poco::StreamCopier::copyStream(rs, os);
	    os.close();
	    std::cout << "File downloaded successfully to: "
		      << outputFileName << std::endl;
	    return 0;
	  } else {
	    std::cerr << "Error: Could not open file for writing: "
		      << outputFileName << std::endl;
	    return -1;
	  }

	} else {
	  // Non-success, non-redirect status
	  std::cerr << "Error: HTTP request failed with status code "
		    << status << std::endl;
	  std::string errorBody;
	  Poco::StreamCopier::copyToString(rs, errorBody);
	  std::cerr << "Response Body: " << errorBody << std::endl;
	  return -1;
	}
	    
      } // while
	
      if (redirectCount > maxRedirects) {
	std::cerr << "Error: Maximum redirect limit (" << maxRedirects
		  << ") exceeded." << std::endl;
      }
	
    } catch (const Poco::Net::NetException& e) {
      std::cerr << "Network Error: " << e.displayText() << std::endl;
    } catch (const Poco::IOException& e) {
      std::cerr << "IO Error: " << e.displayText() << std::endl;
    } catch (const Poco::Exception& e) {
      std::cerr << "POCO Error: " << e.displayText() << std::endl;
    } catch (const std::exception& e) {
      std::cerr << "Standard Error: " << e.what() << std::endl;
    } catch (...) {
      std::cerr << "Unknown Error occurred." << std::endl;
    }
#endif    
    return 0;
}

int read_omni(std::string input_file) {
  
  std::string name;  
  std::string tags;
  std::string path;
#ifdef USE_POCO
  std::string uri;    
  std::string hash;
#endif

  int offset = -1;  
  int nbyte = -1;
  
  bool run = false;
  int res = -1;
  std::string lambda;
  std::string dest;
  
  std::ifstream ifs(input_file);


  if (!ifs.is_open()) {
    std::cerr << "Error: could not open file " << input_file << std::endl;
    return 1;
  }

  try {
    
    YAML::Node root = YAML::Load(ifs);

      
    if (root.IsMap()) {
      

      for (YAML::const_iterator it = root.begin(); it != root.end(); ++it) {
	

        std::string key = it->first.as<std::string>();

	if(key == "name") {
	  name = it->second.as<std::string>();
	}

	if(key == "path") {
	  path = it->second.as<std::string>();
#ifdef USE_POCO	  
	  Poco::File file(path);
	  if (!file.exists()) {
	    std::cerr << "Error: '"
		      << path
		      << "' does not exist"
		      << std::endl;    	
	    return -1;
	  }
#endif
	}
	
#ifdef USE_POCO
	if(key == "uri") {
	  uri = it->second.as<std::string>();
	}

	if(key == "hash") {
	  hash = it->second.as<std::string>();
	}
#endif	
	
	if(key == "offset") {
	  offset = it->second.as<int>();
	}

	if(key == "nbyte") {
	  nbyte = it->second.as<int>();
	  std::vector<char> buffer(nbyte);
          unsigned char* ptr =
	      reinterpret_cast<unsigned char*>(buffer.data());
	  if(!path.empty()) {
#ifdef DEBUG	    
	    std::cout << "path=" << path <<  std::endl;
#endif	    
	    if(read_exact_bytes_from_offset(path.c_str(), offset, nbyte, ptr) ==
	       0) {
#ifdef DEBUG	    
	      std::cout << "buffer=" << ptr << std::endl;
#endif	    
	      put(name, tags, path, ptr, nbyte);
	    }
	    else {
	      return -1;
	    }
	  }
	}

        if (key == "run") {
          run = true;
          lambda = it->second.as<std::string>();
	}
	
        if (key == "dest") {
          dest = it->second.as<std::string>();
#ifndef _WIN32
#ifdef USE_HERMES	  	  
          get_hermes(name, path);
#endif
#endif
	}
	
        if(it->second.IsScalar()){
          std::string value = it->second.as<std::string>();
#ifdef DEBUG	  
          std::cout << key << ": " << value << std::endl;
#endif	  
        } else if (it->second.IsSequence()){
#ifdef DEBUG	  
          std::cout << key << ": " << std::endl;
#endif	  
          for(size_t i = 0; i < it->second.size(); ++i){
            if(it->second[i].IsScalar()){
#ifdef DEBUG	      
              std::cout << " - " << it->second[i].as<std::string>()
			<< std::endl;
#endif	      
	      if(key == "tags") {
		tags += it->second[i].as<std::string>();
		if (i < it->second.size() - 1) {
		  tags += ",";
		}
	      }
	      
            }
	    
          }
        } else if (it->second.IsMap()){
#ifdef DEBUG	  
             std::cout << key << ": " << std::endl;
#endif	     
             for (YAML::const_iterator inner_it = it->second.begin();
		  inner_it != it->second.end(); ++inner_it) {
                std::string inner_key = inner_it->first.as<std::string>();
                if(inner_it->second.IsScalar()){
                  std::string inner_value = inner_it->second.as<std::string>();
#ifdef DEBUG		  
                  std::cout << "  " << inner_key << ": " << inner_value
			    << std::endl;
#endif		  
                }
             }
        }  
      } // for
      
    } else if (root.IsSequence()) {
      for(size_t i = 0; i < root.size(); ++i){
        if(root[i].IsScalar()){
          std::cout << " - " << root[i].as<std::string>() << std::endl;
        }
      }
    } else if (root.IsScalar()){
        std::cout << root.as<std::string>() << std::endl;
    }
   
  } catch (YAML::ParserException& e) {
    std::cerr << "Error: parsing YAML - " << e.what() << std::endl;
    return 1;
  }
  
#if USE_POCO
  if (!uri.empty()){
    long long start = -1;
    long long end = -1;    
    if(offset >=0) {
      start = (long long)offset;
    }
    if(nbyte >=0) {
      end = (long long) (offset + nbyte);
    }
    
    if(download(uri, name, start, end) != 0)
      std::cerr << "Error: downloading '"  << uri
		<< "' failed "
		<< std::endl;
  }
  
  if (!hash.empty()){
    std::string h;	  
    if(!path.empty())
      h = sha256_file(path);
    if(!uri.empty())
      h = sha256_file(name);
	  
    if (hash != h){
      std::cerr << "Error: hash '"
		<< hash << "' is not same as actual '"
		<< h << "'"	      
		<< std::endl;
      return -1;
    }

    if (!dest.empty()) {
#ifdef USE_POCO
	  try {
	    if(run) {
	      res = run_lambda(lambda, name, dest);
	    }
	    else {
#ifdef USE_AWS
	      Poco::File file(name);
	      if (!file.exists()) {
		throw Poco::FileNotFoundException("Error: buffer '"
						  + name
						  + "' not found");
	      }
	      Poco::SharedMemory shm_r(file, Poco::SharedMemory::AM_READ);
	      std::cout << "read '" <<  shm_r.begin() << "' from '" << name
			<< "' buffer."
			<< std::endl;
	    
	      write_s3(dest, shm_r.begin());
#endif	      
	    }
	    if(res == 0) {
#ifdef USE_AWS
	      write_s3(dest, NULL);
#endif
	    }
	    else {
	      std::cerr << "Error: lambda failed to generate '"
			<< dest << "'"
			<< std::endl;
	    }
	  }
	  catch (Poco::Exception& e) {
	    std::cerr << "Error: poco exception - "
		      << e.displayText() << std::endl;
	    return 1;
	  } catch (std::exception& e) {
	    std::cerr << "Error: standard exception - "
		      << e.what() << std::endl;
	    return 1;
	  }
#endif
    }
  }
  
#endif
	  
  return 0;
}

int read_exact_bytes_from_offset(const char *filename, off_t offset,
                                 size_t num_bytes, unsigned char *buffer) {

    int fd = -1;
    ssize_t total_bytes_read = 0;
    ssize_t bytes_read;

    fd = open(filename, O_RDONLY);

    if (fd == -1) {
      std::cerr << "Error: opening file" << filename << std::endl;
      return -1;
    }
    
    if (lseek(fd, offset, SEEK_SET) == -1) {
      std::cerr << "Error: seeking file" << filename << std::endl;
      close(fd);
      return -1;
    }

    while (total_bytes_read < num_bytes) {
        bytes_read = read(fd, buffer + total_bytes_read,
			  num_bytes - total_bytes_read);
        if (bytes_read == -1) {
            perror("Error reading file");
            close(fd);
            return -1;
        }
        if (bytes_read == 0) {
            fprintf(stderr, "End of file reached after reading %zu bytes, expected %zu.\n", total_bytes_read, num_bytes);
            close(fd);
            return -2;
        }
        total_bytes_read += bytes_read;
    }

    if (close(fd) == -1) {
        perror("Error closing file");
        return -1;
    }
    
    if ((size_t)total_bytes_read == num_bytes)
      return 0;
    else
      return 1;

}


std::string read_tags(std::string buf) {

  const std::string filename = ".blackhole/ls";
  
  std::ifstream inputFile(filename);

  if (!inputFile.is_open()) {
    std::cerr
      << "Error: Could not open the file \"" << filename << "\"" << std::endl;
    return "";
  }

  std::string line;
  while (std::getline(inputFile, line)) {

    std::stringstream ss(line);
    std::string firstString;
    std::string secondString;

    // Get the first part of the string, using '|' as the delimiter
    if (std::getline(ss, firstString, '|')) {
      if (firstString == buf) {
	if (std::getline(ss, secondString, '|')) {
	  return secondString;
	}
      }
    }    
  }
  return "";
  
}
  
int write_omni(std::string buf) {
  std::string ofile = buf+".omni.yaml";
  std::cout << "writing output " << ofile << "...";
#ifdef USE_POCO
  std::string h = sha256_file(buf);
#endif 
  std::ofstream of(ofile);
  of << "# OMNI" << std::endl;  
  of << "name: " << buf << std::endl;
  std::string tags = read_tags(buf);
  if (!tags.empty()) {
    of << "tags: " << tags << std::endl;
  }
  else {
    of.close();
    return -1;
  }

#ifdef USE_POCO
  Poco::Path path(buf);
  of << "path: " << path.makeAbsolute().toString() << std::endl;
  if (!h.empty()) {
    of << "hash: " << h << std::endl;
  }
#endif
  of.close();
  std::cout << "done" << std::endl;
  return 0;
  
}

int set_blackhole(){
  
  std::cout << "checking IOWarp runtime...";
#ifdef USE_HERMES
  if(!HERMES_INIT()) {
    std::cerr << std::endl
	      << "Error: HERMES_INIT() failed."
	      << std::endl;
    return -1;
  };
#endif

  if (std::filesystem::exists(".blackhole") == true) {
     std::cout << "yes" << std::endl;
  }
  else {
    std::cout << "no" << std::endl;
    std::cout << "launching a new IOWarp runtime...";
    if (std::filesystem::create_directory(".blackhole")) {
      std::cout << "done" << std::endl;
      return 0;
    } else {
      std::cerr << "Error: failed to create .blackhole directory" << std::endl;
      return -1;
    }
  }
  return 0;
}

int list() {

  const std::string filename = ".blackhole/ls";
  
  std::ifstream inputFile(filename);

  if (!inputFile.is_open()) {
    std::cerr
      << "Error: Could not open the file \"" << filename << "\"" << std::endl;
    return 1;
  }

  std::string line;
  while (std::getline(inputFile, line)) {
    std::cout << line << std::endl;
  }

  if (inputFile.bad()) {
    std::cerr
      << "Error: An unrecoverable error occurred while reading the file."
      << std::endl;
    return 1;
  }
  inputFile.close();

  return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <command> [options]"
		  << std::endl;
        return 1;
    }

    std::string command = argv[1];

    if (command == "put") {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " put <omni.yaml>"
		      << std::endl;
            return 1;
        }
	if (set_blackhole() != 0) {
	  return 1;
	}
	
        std::string name = argv[2];
        return read_omni(name);
	
    } else if (command == "get") {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " get <buffer>"
		      << std::endl;
            return 1;
        }
        std::string name = argv[2];
	return write_omni(name);
	
    } else if (command == "ls") {
      std::cout << "connecting runtime" << std::endl;
      return list();
      
    }
    else {
        std::cerr << "Error: invalid command - " << command << std::endl;
        return 1;
    }

    return 0;
}
