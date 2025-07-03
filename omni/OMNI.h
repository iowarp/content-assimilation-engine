#ifndef CAE_OMNI_H_
#define CAE_OMNI_H_

#include <memory>
#include <string>

namespace cae {

/**
 * Context information for format processing
 */
struct FormatContext {
  std::string description_;
  std::string filename_;
  size_t offset_;
  size_t size_;
  std::string hash_;

  FormatContext() : offset_(0), size_(0) {}
};

/**
 * Abstract base class for format clients that can import data
 */
class FormatClient {
public:
  virtual ~FormatClient() = default;
  virtual void Import(const FormatContext &ctx) = 0;
};

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

/**
 * Enumeration of supported formats
 */
enum class Format { kPosix, kHDF5, kBinary };

/**
 * Enumeration of supported repositories
 */
enum class Repository { kFilesystem, kGlobus, kS3 };

} // namespace cae

#endif // CAE_OMNI_H_