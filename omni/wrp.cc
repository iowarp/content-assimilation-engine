#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

#ifdef USE_HERMES
#include <hermes/hermes.h>

int put(std::string name, std::string tags, std::string path,
	unsigned char* buffer, int nbyte) {

  std::cout << tags << std::endl;

  CHIMAERA_CLIENT_INIT();  
  hermes::Context ctx;
  hermes::Bucket bkt(name);
  hermes::Blob blob(nbyte);
  memcpy(blob.data(), buffer, blob.size());
  hermes::BlobId blob_id = bkt.Put(path, blob, ctx);

  return 0;
}
#endif

int read_exact_bytes_from_offset(const char *filename, off_t offset,
                                 size_t num_bytes, unsigned char *buffer);

int parse_yaml(std::string input_file) {
  
  std::string name;  
  std::string tags;
  std::string path;
  
  std::ifstream ifs(input_file);
  int offset;

  if (!ifs.is_open()) {
    std::cerr << "Error: Could not open file " << input_file << std::endl;
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
	}
	
	if(key == "offset") {
	  offset = it->second.as<int>();
	}
	if(key == "nbyte") {
	  int nbyte = it->second.as<int>();
#ifndef _WIN32	  	  
	  unsigned char buffer[nbyte];
  	  read_exact_bytes_from_offset(path.c_str(), offset, nbyte, buffer);
  	  std::cout << "buffer=" << buffer << std::endl;
#ifdef HERMES	  
	  put(name, tags, path, buffer, nbyte);
#endif	  
#endif 
	}	
	
        if(it->second.IsScalar()){
          std::string value = it->second.as<std::string>();
          std::cout << key << ": " << value << std::endl;
        } else if (it->second.IsSequence()){
          std::cout << key << ": " << std::endl;
          for(size_t i = 0; i < it->second.size(); ++i){
            if(it->second[i].IsScalar()){
              std::cout << " - " << it->second[i].as<std::string>()
			<< std::endl;
	      if(key == "tags") {
		tags += it->second[i].as<std::string>();
		if (i < it->second.size() - 1) {
		  tags += ",";
		}
	      }
	      
            }
	    
          }
        } else if (it->second.IsMap()){
             std::cout << key << ": " << std::endl;
             for (YAML::const_iterator inner_it = it->second.begin();
		  inner_it != it->second.end(); ++inner_it) {
                std::string inner_key = inner_it->first.as<std::string>();
                if(inner_it->second.IsScalar()){
                  std::string inner_value = inner_it->second.as<std::string>();
                  std::cout << "  " << inner_key << ": " << inner_value
			    << std::endl;
                }
             }
        }
      }
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
    std::cerr << "Error parsing YAML: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
#ifndef _WIN32
int read_exact_bytes_from_offset(const char *filename, off_t offset,
                                 size_t num_bytes, unsigned char *buffer) {
    int fd = -1;
    ssize_t total_bytes_read = 0;
    ssize_t bytes_read;

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return -1;
    }

    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Error seeking file");
        close(fd);
        return -1;
    }

    while (total_bytes_read < num_bytes) {
        bytes_read = read(fd, buffer + total_bytes_read, num_bytes - total_bytes_read);
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
#endif

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <command> [options]"
		  << std::endl;
        return 1;
    }

    std::string command = argv[1];

    if (command == "put") {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " put <input.omni>"
		      << std::endl;
            return 1;
        }
        std::string name = argv[2];
        std::cout << "input: " << name << std::endl;
        return parse_yaml(name);
	
    } else if (command == "get") {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " get <output.omni>"
		      << std::endl;
            return 1;
        }
        std::string name = argv[2];
        std::cout << "output: " << name << std::endl;
    } else if (command == "ls") {
      std::cout << "connecting runtime" << std::endl;
    }
    else {
        std::cerr << "Invalid command: " << command << std::endl;
        return 1;
    }

    return 0;
}
