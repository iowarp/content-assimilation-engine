#include "repo_factory.h"
#include "filesystem_repo_omni.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace cae {

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