#include "factory.h"
#include "OMNI.h"

namespace CAE {

std::unique_ptr<RepoClient> CreateRepoClient(const std::string& type) {
    if (type == "filesystem" || type == "posix" || type == "local") {
        return std::make_unique<FilesystemRepoClient>();
    } else if (type == "aws" || type == "s3") {
        return std::make_unique<S3RepoClient>();
    } else if (type == "hermes") {
        return std::make_unique<HermesRepoClient>();
    }
    // Add more repo types here (e.g., "globus")
    return nullptr;
}

std::unique_ptr<FormatClient> CreateFormatClient(const std::string& type) {
    if (type == "posix" || type == "csv") {
        return std::make_unique<PosixFormatClient>();
    } else if (type == "hdf5") {
        return std::make_unique<HDF5FormatClient>();
    } else if (type == "parquet") {
        return std::make_unique<ParquetFormatClient>();
    }
    // Add more format types here
    return nullptr;
}

} // namespace CAE 