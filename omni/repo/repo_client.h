#ifndef CAE_REPO_REPO_CLIENT_H_
#define CAE_REPO_REPO_CLIENT_H_

#include <memory>
#include <string>

namespace cae {

/**
 * Context information for repository operations
 */
struct RepoContext {
public:
  std::string username_;
  std::string passwd_;
  std::string key_;
  std::string path_;
  size_t max_scale_;

  RepoContext() : max_scale_(100) {}
};

/**
 * Abstract base class for repository clients that can download data
 */
class RepoClient {
public:
  virtual ~RepoClient() = default;
  virtual void RecommendScale(int max_scale, int &nprocs, int &nthreads) = 0;
  virtual void Download(const RepoContext &ctx) = 0;
};

} // namespace cae

#endif // CAE_REPO_REPO_CLIENT_H_