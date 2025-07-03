#include "OMNI_factory.h"
#include "binary_file_omni.h"
#include "filesystem_repo_omni.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace cae {

std::unique_ptr<FormatClient> FormatFactory::Get(Format format) {
  switch (format) {
  case Format::kPosix:
  case Format::kBinary:
    return std::make_unique<BinaryFileOmniClient>();
  case Format::kHDF5:
    // TODO: Implement HDF5 client
    throw std::runtime_error("HDF5 format client not yet implemented");
  default:
    throw std::runtime_error("Unknown format type");
  }
}

std::unique_ptr<FormatClient>
FormatFactory::Get(const std::string &format_str) {
  std::string lower_format = format_str;
  std::transform(lower_format.begin(), lower_format.end(), lower_format.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (lower_format == "posix" || lower_format == "binary") {
    return Get(Format::kPosix);
  } else if (lower_format == "hdf5") {
    return Get(Format::kHDF5);
  } else {
    throw std::runtime_error("Unknown format string: " + format_str);
  }
}

std::unique_ptr<RepoClient> RepoFactory::Get(Repository repo) {
  switch (repo) {
  case Repository::kFilesystem:
    return std::make_unique<FilesystemRepoClient>();
  case Repository::kGlobus:
    // TODO: Implement Globus client
    throw std::runtime_error("Globus repository client not yet implemented");
  case Repository::kS3:
    // TODO: Implement S3 client
    throw std::runtime_error("S3 repository client not yet implemented");
  default:
    throw std::runtime_error("Unknown repository type");
  }
}

std::unique_ptr<RepoClient> RepoFactory::Get(const std::string &repo_str) {
  std::string lower_repo = repo_str;
  std::transform(lower_repo.begin(), lower_repo.end(), lower_repo.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (lower_repo == "filesystem" || lower_repo == "posix" ||
      lower_repo == "local") {
    return Get(Repository::kFilesystem);
  } else if (lower_repo == "globus") {
    return Get(Repository::kGlobus);
  } else if (lower_repo == "s3" || lower_repo == "amazon") {
    return Get(Repository::kS3);
  } else {
    throw std::runtime_error("Unknown repository string: " + repo_str);
  }
}

} // namespace cae