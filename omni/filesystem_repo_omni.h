#ifndef CAE_FILESYSTEM_REPO_OMNI_H_
#define CAE_FILESYSTEM_REPO_OMNI_H_

#include "OMNI.h"
#include <sys/stat.h>

namespace cae {

/**
 * Filesystem repository client implementation
 * Recommends scale based on file size (minimum 64MB per process)
 */
class FilesystemRepoClient : public RepoClient {
private:
  static constexpr size_t MIN_BYTES_PER_PROCESS = 64 * 1024 * 1024; // 64MB

public:
  /**
   * Recommend scale based on file size
   * Objective: at least 64MB per process
   * @param max_scale Maximum number of processes allowed
   * @param nprocs Output: recommended number of processes
   * @param nthreads Output: recommended number of threads per process
   */
  void RecommendScale(int max_scale, int &nprocs, int &nthreads) override;

  /**
   * Download data from filesystem (essentially a no-op for local files)
   * @param ctx Repository context containing file path
   */
  void Download(const RepoContext &ctx) override;

  /**
   * Get file size for the given path
   * @param path File path
   * @return File size in bytes, or 0 if file doesn't exist
   */
  size_t GetFileSize(const std::string &path);

  /**
   * Recommend scale specifically for a file path
   * @param file_path Path to the file
   * @param max_scale Maximum number of processes allowed
   * @param nprocs Output: recommended number of processes
   * @param nthreads Output: recommended number of threads per process
   */
  void RecommendScaleForFile(const std::string &file_path, int max_scale,
                             int &nprocs, int &nthreads);
};

} // namespace cae

#endif // CAE_FILESYSTEM_REPO_OMNI_H_