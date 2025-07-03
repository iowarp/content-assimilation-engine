#ifndef CAE_OMNI_FACTORY_H_
#define CAE_OMNI_FACTORY_H_

#include "OMNI.h"
#include <memory>

namespace cae {

/**
 * Factory class for creating format clients
 */
class FormatFactory {
public:
  /**
   * Get a format client instance for the specified format
   * @param format The format type to create
   * @return Unique pointer to the format client
   */
  static std::unique_ptr<FormatClient> Get(Format format);

  /**
   * Get a format client instance from string
   * @param format_str String representation of format
   * @return Unique pointer to the format client
   */
  static std::unique_ptr<FormatClient> Get(const std::string &format_str);
};

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

#endif // CAE_OMNI_FACTORY_H_