#include "filesystem_repo_omni.h"
#include <algorithm>
#include <iostream>

namespace cae {

void FilesystemRepoClient::RecommendScale(int max_scale, int &nprocs,
                                          int &nthreads) {
  // For filesystem operations, we typically don't need multiple threads per
  // process
  nthreads = 1;

  // Default to single process if we can't determine file size
  nprocs = 1;
}

void FilesystemRepoClient::Download(const RepoContext &ctx) {
  // For local filesystem, this is essentially a no-op
  // The file is already "downloaded" (accessible locally)

  size_t file_size = GetFileSize(ctx.path_);
  if (file_size == 0) {
    std::cerr << "Warning: File not found or empty: " << ctx.path_ << std::endl;
    return;
  }

  std::cout << "File available locally: " << ctx.path_ << " (" << file_size
            << " bytes)" << std::endl;
}

size_t FilesystemRepoClient::GetFileSize(const std::string &path) {
  struct stat stat_buf;
  if (stat(path.c_str(), &stat_buf) != 0) {
    return 0; // File doesn't exist or can't be accessed
  }

  if (!S_ISREG(stat_buf.st_mode)) {
    return 0; // Not a regular file
  }

  return static_cast<size_t>(stat_buf.st_size);
}

// Helper function to recommend scale based on file size
void FilesystemRepoClient::RecommendScaleForFile(const std::string &file_path,
                                                 int max_scale, int &nprocs,
                                                 int &nthreads) {
  size_t file_size = GetFileSize(file_path);

  // Calculate number of processes needed for at least 64MB per process
  if (file_size <= MIN_BYTES_PER_PROCESS) {
    nprocs = 1;
  } else {
    nprocs = static_cast<int>((file_size + MIN_BYTES_PER_PROCESS - 1) /
                              MIN_BYTES_PER_PROCESS);
    nprocs = std::min(nprocs, max_scale);
  }

  // For filesystem operations, single thread per process is usually optimal
  nthreads = 1;

  std::cout << "Recommended scale for file " << file_path
            << " (size: " << file_size << " bytes): " << nprocs
            << " processes, " << nthreads << " threads per process"
            << std::endl;
}

} // namespace cae