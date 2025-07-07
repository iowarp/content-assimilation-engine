#include "format/binary_file_omni.h"
#include "format/progress_bar.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mpi.h>
#include <string>

namespace cae {

void PrintUsage(const char *program_name) {
  std::cerr << "Usage: " << program_name
            << " <filename> <offset> <size> [description] [hash]" << std::endl;
  std::cerr << "Parameters:" << std::endl;
  std::cerr << "  filename    - Path to the file to process (required)"
            << std::endl;
  std::cerr << "  offset      - Starting offset in bytes (required)"
            << std::endl;
  std::cerr << "  size        - Number of bytes to process (required)"
            << std::endl;
  std::cerr << "  description - Optional description string" << std::endl;
  std::cerr << "  hash        - Optional hash value for verification"
            << std::endl;
}

class BinaryFileOmniWithProgress : public BinaryFileOmni {
public:
  BinaryFileOmniWithProgress(const std::string &filename, size_t total_size,
                             int rank)
      : progress_(std::make_unique<ProgressBar>(
            std::filesystem::path(filename).filename().string(), total_size,
            rank)),
        total_size_(total_size) {}

protected:
  virtual void OnChunkProcessed(size_t bytes_processed) override {
    progress_->Update(bytes_processed);
    if (bytes_processed == total_size_) {
      progress_->Finish();
      std::cout << std::endl; // Move to next line after completion
    }
  }

private:
  std::unique_ptr<ProgressBar> progress_;
  size_t total_size_;
};

} // namespace cae

int main(int argc, char *argv[]) {
  // Initialize MPI
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Check command line arguments
  if (argc < 2) {
    if (rank == 0) {
      std::cerr << "Usage: " << argv[0] << " <binary_file>" << std::endl;
    }
    MPI_Finalize();
    return 1;
  }

  try {
    std::string filename = argv[1];
    size_t file_size = 0;

    // Get file size (only rank 0 needs to do this)
    if (rank == 0) {
      std::ifstream file(filename, std::ios::binary | std::ios::ate);
      if (!file) {
        throw std::runtime_error("Could not open file: " + filename);
      }
      file_size = file.tellg();
    }

    // Broadcast file size to all ranks
    MPI_Bcast(&file_size, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);

    // Calculate per-process work distribution
    size_t bytes_per_process = file_size / size;
    size_t remaining_bytes = file_size % size;

    // Calculate this process's portion
    size_t process_offset = rank * bytes_per_process;
    size_t process_size = bytes_per_process;

    // Distribute remaining bytes to the first few processes
    if (rank < remaining_bytes) {
      process_offset += rank;
      process_size += 1;
    } else {
      process_offset += remaining_bytes;
    }

    // Create format client with progress bar
    cae::BinaryFileOmniWithProgress format(filename, process_size, rank);

    // Create context for this process's portion
    cae::FormatContext ctx;
    ctx.filename_ = filename;
    ctx.offset_ = process_offset;
    ctx.size_ = process_size;

    // Process the data
    format.Import(ctx);

    // Wait for all ranks to complete
    MPI_Barrier(MPI_COMM_WORLD);

  } catch (const std::exception &e) {
    std::cerr << "Rank " << rank << " error: " << e.what() << std::endl;
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  MPI_Finalize();
  return 0;
}