#include "format/binary_file_omni.h"
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <string>

using namespace cae;

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

int main(int argc, char *argv[]) {
  // Initialize MPI
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Check command line arguments
  if (argc < 4) {
    if (rank == 0) {
      PrintUsage(argv[0]);
    }
    MPI_Finalize();
    return 1;
  }

  try {
    // Parse command line arguments to create FormatContext
    FormatContext ctx;
    ctx.filename_ = argv[1];
    ctx.offset_ = std::stoull(argv[2]);
    ctx.size_ = std::stoull(argv[3]);

    // Optional parameters
    if (argc > 4) {
      ctx.description_ = argv[4];
    }
    if (argc > 5) {
      ctx.hash_ = argv[5];
    }

    if (rank == 0) {
      std::cout << "MPI Binary Format Processor" << std::endl;
      std::cout << "===========================" << std::endl;
      std::cout << "Number of MPI processes: " << size << std::endl;
      std::cout << "File: " << ctx.filename_ << std::endl;
      std::cout << "Total size: " << ctx.size_ << " bytes" << std::endl;
      std::cout << "Starting offset: " << ctx.offset_ << " bytes" << std::endl;
      if (!ctx.description_.empty()) {
        std::cout << "Description: " << ctx.description_ << std::endl;
      }
      if (!ctx.hash_.empty()) {
        std::cout << "Expected hash: " << ctx.hash_ << std::endl;
      }
    }

    // Calculate per-process work distribution
    size_t bytes_per_process = ctx.size_ / size;
    size_t remaining_bytes = ctx.size_ % size;

    // Calculate this process's portion
    size_t process_offset = ctx.offset_ + (rank * bytes_per_process);
    size_t process_size = bytes_per_process;

    // Distribute remaining bytes to the first few processes
    if (rank < remaining_bytes) {
      process_offset += rank;
      process_size += 1;
    } else {
      process_offset += remaining_bytes;
    }

    // Create FormatContext for this process's portion
    FormatContext process_ctx = ctx;
    process_ctx.offset_ = process_offset;
    process_ctx.size_ = process_size;

    if (process_size > 0) {
      std::cout << "Rank " << rank << ": Processing " << process_size
                << " bytes at offset " << process_offset << std::endl;

      // Create binary format client and process data
      BinaryFileOmni client;

      std::cout << "Rank " << rank << ": " << client.Describe(process_ctx)
                << std::endl;

      // Process the data
      client.Import(process_ctx);

      std::cout << "Rank " << rank << ": Completed processing" << std::endl;
    } else {
      std::cout << "Rank " << rank << ": No data to process" << std::endl;
    }

    // Synchronize all processes before finishing
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
      std::cout << "All MPI processes completed successfully!" << std::endl;
    }

  } catch (const std::exception &e) {
    std::cerr << "Error on rank " << rank << ": " << e.what() << std::endl;
    MPI_Abort(MPI_COMM_WORLD, 1);
    return 1;
  }

  MPI_Finalize();
  return 0;
}