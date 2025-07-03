#ifndef CAE_BINARY_FILE_OMNI_H_
#define CAE_BINARY_FILE_OMNI_H_

#include "OMNI.h"
#include <cstdio>
#include <vector>

namespace cae {

/**
 * Binary file format client implementation
 * Reads files using stdio in chunks of up to 16MB
 */
class BinaryFileOmniClient : public FormatClient {
private:
  static constexpr size_t MAX_CHUNK_SIZE = 16 * 1024 * 1024; // 16MB

public:
  /**
   * Import data from a binary file
   * Reads the file in chunks within the specified range (offset, size)
   * @param ctx Format context containing filename, offset, and size
   */
  void Import(const FormatContext &ctx) override;

private:
  /**
   * Read a chunk of data from the file
   * @param file File pointer
   * @param buffer Buffer to read into
   * @param chunk_size Size of chunk to read
   * @return Number of bytes actually read
   */
  size_t ReadChunk(FILE *file, std::vector<char> &buffer, size_t chunk_size);

  /**
   * Process the read data (placeholder for actual processing logic)
   * @param buffer Buffer containing the data
   * @param bytes_read Number of bytes in the buffer
   * @param global_offset Global offset in the file
   */
  void ProcessData(const std::vector<char> &buffer, size_t bytes_read,
                   size_t global_offset);
};

} // namespace cae

#endif // CAE_BINARY_FILE_OMNI_H_