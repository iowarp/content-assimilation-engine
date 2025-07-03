#include "binary_file_omni.h"
#include "OMNI_factory.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <mpi.h>

using namespace cae;

void BinaryFileOmniClient::Import(const FormatContext &ctx) {
  FILE *file = fopen(ctx.filename_.c_str(), "rb");
  if (!file) {
    std::cerr << "Error: Cannot open file " << ctx.filename_ << std::endl;
    return;
  }

  // Seek to the specified offset
  if (fseek(file, ctx.offset_, SEEK_SET) != 0) {
    std::cerr << "Error: Cannot seek to offset " << ctx.offset_ << std::endl;
    fclose(file);
    return;
  }

  size_t remaining_bytes = ctx.size_;
  size_t current_offset = ctx.offset_;
  std::vector<char> buffer(MAX_CHUNK_SIZE);

  while (remaining_bytes > 0) {
    size_t chunk_size = std::min(remaining_bytes, MAX_CHUNK_SIZE);
    size_t bytes_read = ReadChunk(file, buffer, chunk_size);

    if (bytes_read == 0) {
      break; // End of file or error
    }

    ProcessData(buffer, bytes_read, current_offset);

    remaining_bytes -= bytes_read;
    current_offset += bytes_read;
  }

  fclose(file);
}

size_t BinaryFileOmniClient::ReadChunk(FILE *file, std::vector<char> &buffer,
                                       size_t chunk_size) {
  return fread(buffer.data(), 1, chunk_size, file);
}

void BinaryFileOmniClient::ProcessData(const std::vector<char> &buffer,
                                       size_t bytes_read,
                                       size_t global_offset) {
  // Placeholder for actual data processing
  // In a real implementation, this would assimilate the data into the content
  // engine
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::cout << "Rank " << rank << ": Processed " << bytes_read
            << " bytes at offset " << global_offset << std::endl;
}

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  // Parse command line arguments: path, offset, size, description, hash
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0]
              << " <path> <offset> <size> [description] [hash]" << std::endl;
    MPI_Finalize();
    return 1;
  }

  int rank, nprocs;
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Parse arguments
  FormatContext fctx;
  fctx.filename_ = argv[1];
  size_t global_offset = std::stoull(argv[2]);
  size_t total_size = std::stoull(argv[3]);

  if (argc > 4) {
    fctx.description_ = argv[4];
  }
  if (argc > 5) {
    fctx.hash_ = argv[5]; // Store as string, not parse as number
  }

  // Divide the work among MPI processes
  size_t chunk_size_per_process = total_size / nprocs;
  size_t remainder = total_size % nprocs;

  // Calculate this process's portion
  fctx.offset_ = global_offset + (rank * chunk_size_per_process);
  fctx.size_ = chunk_size_per_process;

  // Last process gets any remainder bytes
  if (rank == nprocs - 1) {
    fctx.size_ += remainder;
  }

  if (rank == 0) {
    std::cout << "Processing file: " << fctx.filename_ << std::endl;
    std::cout << "Total size: " << total_size << " bytes" << std::endl;
    std::cout << "Number of processes: " << nprocs << std::endl;
    if (!fctx.hash_.empty()) {
      std::cout << "Expected hash: " << fctx.hash_ << std::endl;
    }
  }

  // Get the format client and process the data
  std::unique_ptr<FormatClient> client = FormatFactory::Get(Format::kPosix);
  client->Import(fctx);

  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == 0) {
    std::cout << "File processing completed successfully" << std::endl;
  }

  MPI_Finalize();
  return 0;
}