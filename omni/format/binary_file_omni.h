#ifndef CAE_FORMAT_BINARY_FILE_OMNI_H_
#define CAE_FORMAT_BINARY_FILE_OMNI_H_

#include "format_client.h"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <memory>
#include <sys/stat.h>
#include <vector>

/**
 * Simple Binary File Processing Strategy:
 *
 * This module provides straightforward binary file processing using standard
 * C stdio functions:
 *
 * 1. File Reading: Uses fopen, fread, fclose for all file operations
 * 2. Chunked Processing: Reads files in manageable chunks for memory efficiency
 * 3. Simple Output: Provides basic file processing information and statistics
 */

namespace cae {

/**
 * Binary file content processing client using standard stdio
 */
class BinaryFileOmni : public FormatClient {
private:
  static constexpr size_t DEFAULT_CHUNK_SIZE = 1024 * 1024; // 1MB chunks

public:
  /** Default constructor */
  BinaryFileOmni() = default;

  /** Destructor */
  ~BinaryFileOmni() override = default;

  /** Describe the file */
  std::string Describe(const FormatContext &ctx) override {
    return "Binary file: " + ctx.filename_ +
           " (size: " + std::to_string(ctx.size_) +
           " bytes, offset: " + std::to_string(ctx.offset_) + ")";
  }

  /** Process a binary file using standard stdio functions */
  void Import(const FormatContext &ctx) override {
    std::cout << "Processing file: " << ctx.filename_ << std::endl;
    std::cout << "Size: " << ctx.size_ << " bytes" << std::endl;
    std::cout << "Offset: " << ctx.offset_ << " bytes" << std::endl;
    if (!ctx.hash_.empty()) {
      std::cout << "Expected hash: " << ctx.hash_ << std::endl;
    }

    // Open file for reading
    FILE *file = fopen(ctx.filename_.c_str(), "rb");
    if (!file) {
      std::cerr << "Error: Failed to open file " << ctx.filename_ << std::endl;
      return;
    }

    // Seek to the specified offset
    if (fseek(file, (long)ctx.offset_, SEEK_SET) != 0) {
      std::cerr << "Error: Failed to seek to offset " << ctx.offset_
                << " in file " << ctx.filename_ << std::endl;
      fclose(file);
      return;
    }

    // Process the file in chunks
    std::vector<char> buffer(DEFAULT_CHUNK_SIZE);
    size_t total_read = 0;
    size_t remaining = ctx.size_;

    std::cout << "Reading file in chunks of " << DEFAULT_CHUNK_SIZE << " bytes"
              << std::endl;

    while (remaining > 0 && total_read < ctx.size_) {
      size_t chunk_size = std::min(remaining, DEFAULT_CHUNK_SIZE);
      size_t bytes_read = fread(buffer.data(), 1, chunk_size, file);

      if (bytes_read == 0) {
        if (feof(file)) {
          std::cout << "Reached end of file after reading " << total_read
                    << " bytes" << std::endl;
          break;
        } else {
          std::cerr << "Error reading file after " << total_read << " bytes"
                    << std::endl;
          break;
        }
      }

      total_read += bytes_read;
      remaining -= bytes_read;

      // Process the chunk (for now, just report progress)
      std::cout << "Read chunk: " << bytes_read
                << " bytes (total: " << total_read << "/" << ctx.size_ << ")"
                << std::endl;

      // Call progress callback
      OnChunkProcessed(total_read);
    }

    fclose(file);

    std::cout << "File processing completed. Total bytes read: " << total_read
              << "/" << ctx.size_ << std::endl;

    if (total_read == ctx.size_) {
      std::cout << "Successfully processed entire requested range" << std::endl;
    } else {
      std::cout << "Warning: Only processed " << total_read << " out of "
                << ctx.size_ << " requested bytes" << std::endl;
    }
  }

protected:
  virtual void OnChunkProcessed(size_t bytes_processed) {}
};

} // namespace cae

#endif // CAE_FORMAT_BINARY_FILE_OMNI_H_