#ifndef CAE_FORMAT_FORMAT_FACTORY_H_
#define CAE_FORMAT_FORMAT_FACTORY_H_

#include "format_client.h"
#include <memory>

namespace cae {

/**
 * Enumeration of supported formats
 */
enum class Format { kPosix, kHDF5, kBinary };

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

} // namespace cae

#endif // CAE_FORMAT_FORMAT_FACTORY_H_