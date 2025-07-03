# Content Assimilation Engine (CAE) - OMNI Module

The OMNI module provides a YAML-based content assimilation engine that can download and process data from various repositories in different formats using MPI for scalability.

## Architecture

The OMNI module follows a factory pattern architecture with the following components:

### Core Components

1. **OMNI.h** - Abstract base classes and data structures
   - `FormatClient` - Abstract interface for format processors
   - `RepoClient` - Abstract interface for repository clients
   - `FormatContext` & `RepoContext` - Context structures

2. **OMNI_factory.h/.cc** - Factory classes for creating clients
   - `FormatFactory` - Creates format client instances
   - `RepoFactory` - Creates repository client instances

3. **Binary File Processor**
   - `binary_file_omni.h/.cc` - Processes binary files using stdio
   - Reads files in chunks up to 16MB
   - Supports MPI parallelization

4. **Filesystem Repository**
   - `filesystem_repo_omni.h/.cc` - Local filesystem repository client
   - Recommends optimal MPI scaling (minimum 64MB per process)

5. **Main Parser**
   - `wrp.cc` - YAML parser and job orchestrator
   - Parses OMNI YAML format and launches MPI jobs

## OMNI YAML Format

The OMNI format is a YAML-based specification for describing data assimilation jobs:

```yaml
name: cae posix job          # Job name (optional)
max_scale: 100               # Maximum number of MPI processes (optional, default: 100)
data:                        # Array of data entries to process
- path: /path/to/file.txt    # File path (required)
  range: [0, 1024]           # Byte range [start, end] (optional)
  offset: 0                  # Starting offset in bytes (optional, default: 0)
  size: 1024                 # Size to read in bytes (optional, derived from range if not specified)
  description:               # Data description tags (optional)
    - text
    - unstructured
  hash: sha256_value         # Integrity hash (optional)
```

### Field Descriptions

- **name**: Human-readable job name
- **max_scale**: Maximum number of MPI processes to use
- **data**: Array of data entries to process
  - **path**: File system path to the data file (required)
  - **range**: Byte range as [start_offset, end_offset] (optional)
  - **offset**: Starting byte offset (optional, default: 0)
  - **size**: Number of bytes to read (optional, derived from range if not specified)
  - **description**: Array of descriptive tags (optional)
  - **hash**: Integrity hash value (optional)

## Building

```bash
# From the project root
mkdir build && cd build
cmake ..
make -j4

# Or build just the OMNI module
cd omni
mkdir build && cd build
cmake ..
make -j4
```

### Dependencies

- CMake 3.16+
- MPI implementation (OpenMPI, MPICH, etc.)
- yaml-cpp library
- C++17 compatible compiler

### Installing Dependencies (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install cmake libopenmpi-dev libyaml-cpp-dev pkg-config
```

## Usage

### 1. Create an OMNI YAML file

```yaml
name: my_data_job
max_scale: 8
data:
- path: /path/to/my_data.bin
  offset: 0
  size: 67108864  # 64MB
  description:
    - binary
    - sensor_data
```

### 2. Run the job

```bash
# Execute the OMNI job
./bin/wrp my_job.yaml
```

The `wrp` binary will:
1. Parse the YAML file
2. Analyze each data file to recommend optimal MPI scaling
3. Launch MPI jobs using `binary_file_omni` to process each data entry
4. Report progress and completion status

### 3. Manual binary execution

You can also run the binary processor directly:

```bash
# Process a file with 4 MPI processes
mpirun -np 4 ./bin/binary_file_omni /path/to/file.bin 0 1048576 "binary,data" "hash_value"
```

## Scaling Strategy

The filesystem repository client automatically recommends MPI scaling based on:
- **Minimum**: 64MB per MPI process
- **Maximum**: User-specified `max_scale` value
- **Default**: Single process for files < 64MB

### Examples

- **32MB file**: 1 process (below minimum threshold)
- **128MB file**: 2 processes (64MB each)
- **1GB file, max_scale=8**: 8 processes (128MB each)
- **1GB file, max_scale=20**: 16 processes (64MB each, respects minimum)

## Extending the Engine

### Adding New Format Clients

1. Create a new class inheriting from `FormatClient`
2. Implement the `Import()` method
3. Add the format to the `Format` enum in `OMNI.h`
4. Update `FormatFactory::Get()` in `factory.cc`

### Adding New Repository Clients

1. Create a new class inheriting from `RepoClient`
2. Implement `RecommendScale()` and `Download()` methods
3. Add the repository to the `Repository` enum in `OMNI.h`
4. Update `RepoFactory::Get()` in `factory.cc`

## File Structure

```
omni/
├── OMNI.h                    # Core interfaces and data structures
├── OMNI_factory.h           # Factory class declarations
├── factory.cc               # Factory implementations
├── binary_file_omni.h       # Binary file format client header
├── binary_file_omni.cc      # Binary file processor + MPI main()
├── filesystem_repo_omni.h   # Filesystem repository client header
├── filesystem_repo_client.cc # Filesystem repository implementation
├── wrp.cc                   # Main YAML parser and job orchestrator
├── CMakeLists.txt           # Build configuration
├── example_job.yaml         # Sample OMNI job file
└── README.md               # This documentation
```

## Future Enhancements

- **HDF5 Format Client**: Support for HDF5 file processing
- **Globus Repository Client**: Integration with Globus data transfer
- **S3 Repository Client**: Amazon S3 and compatible object storage
- **Advanced Scheduling**: Job queuing and dependency management
- **Data Validation**: Hash verification and integrity checking
- **Progress Tracking**: Real-time job progress monitoring 