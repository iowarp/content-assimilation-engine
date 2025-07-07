#ifndef CAE_FORMAT_FORMAT_CLIENT_H_
#define CAE_FORMAT_FORMAT_CLIENT_H_

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

  /** Describe the format/file */
  virtual std::string Describe(const FormatContext &ctx) = 0;
};

} // namespace cae

#endif // CAE_FORMAT_FORMAT_CLIENT_H_