/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Distributed under BSD 3-Clause license.                                   *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Illinois Institute of Technology.                        *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of Hermes. The full Hermes copyright notice, including  *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the top directory. If you do not  *
 * have access to the file, you may request a copy from help@hdfgroup.org.   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef CAE_ADAPTER_ADAPTER_TYPES_H_
#define CAE_ADAPTER_ADAPTER_TYPES_H_

#include "adapters/posix/posix_api.h"
#include <stdexcept>
#include <string>

namespace cae {

/** Adapter types */
enum class AdapterType { kNone, kPosix, kStdio, kMpiio, kPubsub, kVfd };

/** Adapter modes */
enum class AdapterMode { kNone, kDefault, kBypass, kScratch, kWorkflow };

/** Flushing modes */
enum class FlushingMode { kSync, kAsync };

/** Adapter Mode converter */
class AdapterModeConv {
public:
  static std::string ToStr(AdapterMode mode) {
    switch (mode) {
    case AdapterMode::kDefault:
      return "kDefault";
    case AdapterMode::kBypass:
      return "kBypass";
    case AdapterMode::kScratch:
      return "kScratch";
    case AdapterMode::kWorkflow:
      return "kWorkflow";
    default:
      return "kNone";
    }
  }

  static AdapterMode ToEnum(const std::string &mode) {
    if (mode == "kDefault") {
      return AdapterMode::kDefault;
    } else if (mode == "kBypass") {
      return AdapterMode::kBypass;
    } else if (mode == "kScratch") {
      return AdapterMode::kScratch;
    } else if (mode == "kWorkflow") {
      return AdapterMode::kWorkflow;
    }
    return AdapterMode::kNone;
  }
};

/** Flushing Mode converter */
class FlushingModeConv {
public:
  static std::string ToStr(FlushingMode mode) {
    switch (mode) {
    case FlushingMode::kSync:
      return "kSync";
    case FlushingMode::kAsync:
      return "kAsync";
    default:
      return "kSync";
    }
  }

  static FlushingMode ToEnum(const std::string &mode) {
    if (mode == "kAsync") {
      return FlushingMode::kAsync;
    }
    return FlushingMode::kSync;
  }
};

/**
 * Per-Object Adapter Settings.
 * An object may be a file, for example.
 */
struct AdapterObjectConfig {
  AdapterMode mode_;
  size_t page_size_;

  AdapterObjectConfig()
      : mode_(AdapterMode::kDefault), page_size_(1024 * 1024) {}

  AdapterObjectConfig(AdapterMode mode, size_t page_size)
      : mode_(mode), page_size_(page_size) {}
};

/** Parse a size string with units (e.g., "1MB", "1024KB") */
static inline size_t ParseSize(const std::string &size_str) {
  size_t size = 0;
  size_t multiplier = 1;
  std::string num_str;
  std::string unit;

  // Split number and unit
  for (char c : size_str) {
    if (isdigit(c)) {
      num_str += c;
    } else if (isalpha(c)) {
      unit += c;
    }
  }

  if (num_str.empty()) {
    throw std::runtime_error("Invalid size format: " + size_str);
  }

  size = std::stoull(num_str);

  if (unit == "KB" || unit == "K") {
    multiplier = 1024;
  } else if (unit == "MB" || unit == "M") {
    multiplier = 1024 * 1024;
  } else if (unit == "GB" || unit == "G") {
    multiplier = 1024 * 1024 * 1024;
  } else if (unit == "TB" || unit == "T") {
    multiplier = 1024ULL * 1024ULL * 1024ULL * 1024ULL;
  } else if (!unit.empty()) {
    throw std::runtime_error("Unknown size unit: " + unit);
  }

  return size * multiplier;
}

struct AdapterInfo {
  int file_id_;
  int fd_;
  int open_flags_;
  int mode_flags_;
  int refcnt_;
  std::string path_;
  AdapterMode adapter_mode_;

  ~AdapterInfo() {
    if (fd_ >= 0) {
      CAE_POSIX_API->close(fd_);
    }
  }
};

} // namespace cae

#endif // CAE_ADAPTER_ADAPTER_TYPES_H_
