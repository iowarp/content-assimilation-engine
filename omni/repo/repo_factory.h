#ifndef CAE_REPO_REPO_FACTORY_H_
#define CAE_REPO_REPO_FACTORY_H_

#include "repo_client.h"
#include <memory>

namespace cae {

/**
 * Enumeration of supported repositories
 */
enum class Repository { kFilesystem, kGlobus, kS3 };

/**
 * Factory class for creating repository clients
 */
class RepoFactory {
public:
  /**
   * Get a repository client instance for the specified repository
   * @param repo The repository type to create
   * @return Unique pointer to the repository client
   */
  static std::unique_ptr<RepoClient> Get(Repository repo);

  /**
   * Get a repository client instance from string
   * @param repo_str String representation of repository
   * @return Unique pointer to the repository client
   */
  static std::unique_ptr<RepoClient> Get(const std::string &repo_str);
};

} // namespace cae

#endif // CAE_REPO_REPO_FACTORY_H_