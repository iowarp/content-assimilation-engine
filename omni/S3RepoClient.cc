#include "OMNI.h"
#include <iostream>
#include <fstream>
#ifdef USE_AWS
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#endif

namespace CAE {

void S3RepoClient::Download(const RepoContext &ctx) {
#ifdef USE_AWS
    const std::string& s3url = ctx.key_;
    const std::string prefix = "s3://";
    if (s3url.find(prefix) != 0) {
        std::cerr << "Error: not a valid S3 URL (missing 's3://' prefix)" << std::endl;
        return;
    }
    std::string path = s3url.substr(prefix.length());
    size_t first_slash = path.find('/');
    if (first_slash == std::string::npos) {
        std::cerr << "Error: invalid S3 URI (no path separator found)" << std::endl;
        return;
    }
    std::string bucket_name = path.substr(0, first_slash);
    std::string object_key = path.substr(first_slash + 1);
    if (bucket_name.empty()) {
        std::cerr << "Error: bucket name is empty" << std::endl;
        return;
    }
    if (object_key.empty()) {
        std::cerr << "Error: object key is empty" << std::endl;
        return;
    }
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        Aws::Client::ClientConfiguration clientConfig;
        clientConfig.region = "us-east-1";
        // Optionally set endpointOverride, credentials, etc.
        Aws::S3::S3Client s3_client(clientConfig);
        Aws::S3::Model::GetObjectRequest get_request;
        get_request.SetBucket(bucket_name.c_str());
        get_request.SetKey(object_key.c_str());
        auto get_object_outcome = s3_client.GetObject(get_request);
        if (get_object_outcome.IsSuccess()) {
            std::string outputFileName = object_key.substr(object_key.find_last_of("/") + 1);
            Aws::OFStream local_file;
            local_file.open(outputFileName.c_str(), std::ios::out | std::ios::binary);
            local_file << get_object_outcome.GetResult().GetBody().rdbuf();
            std::cout << "Successfully downloaded S3 object to " << outputFileName << std::endl;
        } else {
            std::cerr << "Error: downloading object - "
                      << get_object_outcome.GetError().GetMessage() << std::endl;
        }
    }
    Aws::ShutdownAPI(options);
#else
    std::cerr << "S3RepoClient::Download called (AWS SDK not available) for key: " << ctx.key_ << std::endl;
#endif
}

void S3RepoClient::Upload(const RepoContext &ctx, const std::string& local_path) {
#ifdef USE_AWS
    const std::string& s3url = ctx.key_;
    const std::string prefix = "s3://";
    if (s3url.find(prefix) != 0) {
        std::cerr << "Error: not a valid S3 URL (missing 's3://' prefix)" << std::endl;
        return;
    }
    std::string path = s3url.substr(prefix.length());
    size_t first_slash = path.find('/');
    if (first_slash == std::string::npos) {
        std::cerr << "Error: invalid S3 URI (no path separator found)" << std::endl;
        return;
    }
    std::string bucket_name = path.substr(0, first_slash);
    std::string object_key = path.substr(first_slash + 1);
    if (bucket_name.empty() || object_key.empty()) {
        std::cerr << "Error: bucket or key is empty" << std::endl;
        return;
    }
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        Aws::Client::ClientConfiguration clientConfig;
        clientConfig.region = "us-east-1";
        Aws::S3::S3Client s3_client(clientConfig);
        Aws::S3::Model::PutObjectRequest put_request;
        put_request.SetBucket(bucket_name.c_str());
        put_request.SetKey(object_key.c_str());
        auto input_data = Aws::MakeShared<Aws::FStream>("PutObjectInputStream", local_path.c_str(), std::ios_base::in | std::ios_base::binary);
        if (!*input_data) {
            std::cerr << "Error: Unable to open file " << local_path << std::endl;
            Aws::ShutdownAPI(options);
            return;
        }
        put_request.SetBody(input_data);
        put_request.SetContentType("application/octet-stream");
        auto put_object_outcome = s3_client.PutObject(put_request);
        if (put_object_outcome.IsSuccess()) {
            std::cout << "Successfully uploaded " << local_path << " to S3: " << s3url << std::endl;
        } else {
            std::cerr << "Error: uploading object - " << put_object_outcome.GetError().GetMessage() << std::endl;
        }
    }
    Aws::ShutdownAPI(options);
#else
    std::cerr << "S3RepoClient::Upload called (AWS SDK not available) for key: " << ctx.key_ << std::endl;
#endif
}

} // namespace CAE 