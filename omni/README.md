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

## Installation

### Prerequisites

Before building the OMNI module, ensure you have the following dependencies installed:

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install cmake libopenmpi-dev libyaml-cpp-dev pkg-config build-essential

# CentOS/RHEL/Fedora
sudo yum install cmake openmpi-devel yaml-cpp-devel pkgconfig gcc-c++
# or for newer versions:
sudo dnf install cmake openmpi-devel yaml-cpp-devel pkgconfig gcc-c++

# macOS (with Homebrew)
brew install cmake open-mpi yaml-cpp pkg-config
```

### Building from Source

#### Option 1: Build as part of the main CAE project (Recommended)

```bash
# Clone the CAE repository
git clone <repository-url>
cd content-assimilation-engine

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project (includes OMNI module)
make -j4
# or if using ninja:
ninja

# Executables will be in build/bin/
ls bin/wrp bin/binary_file_omni
```

#### Option 2: Build OMNI module standalone

```bash
# Navigate to the OMNI directory
cd omni

# Create build directory
mkdir build && cd build

# Configure with CMake (requires MPI and yaml-cpp)
cmake ..

# Build
make -j4

# Executables will be in the current directory
ls wrp binary_file_omni
```

### Installation

```bash
# From the main build directory
make install
# or
ninja install

# This installs:
# - Executables to ${CMAKE_INSTALL_PREFIX}/bin/
# - Libraries to ${CMAKE_INSTALL_PREFIX}/lib/
# - Headers to ${CMAKE_INSTALL_PREFIX}/include/omni/
```

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

## Quick Start

### 1. Quick Test

First, validate your installation with a simple test:

```bash
# From the build directory
./bin/wrp ../omni/config/quick_test.yaml
```

Expected output:
```
Parsed OMNI job: quick_test
Max scale: 2
Number of data entries: 1

Processing data entry 1: ../data/A46_xx.csv
Recommended scale for file ../data/A46_xx.csv (size: 106922 bytes): 1 processes, 1 threads per process
Executing: which mpirun >/dev/null 2>&1 && mpirun -np 1 bin/binary_file_omni "../data/A46_xx.csv" 0 1000 "csv,test" "quick_test_hash" || bin/binary_file_omni "../data/A46_xx.csv" 0 1000 "csv,test" "quick_test_hash"
Processing file: ../data/A46_xx.csv
Total size: 1000 bytes
Number of processes: 1
Expected hash: quick_test_hash
Rank 0: Processed 1000 bytes at offset 0
File processing completed successfully

All data entries processed!
```

### 2. Run Demo Job

Test with multiple data files:

```bash
# From the build directory
./bin/wrp ../omni/config/demo_job.yaml
```

### 3. Manual Binary Execution

You can also run the binary processor directly:

```bash
# Process a file directly (without MPI)
./bin/binary_file_omni /path/to/file.bin 0 1048576 "binary,data" "hash_value"

# With MPI (if available)
mpirun -np 4 ./bin/binary_file_omni /path/to/file.bin 0 1048576 "binary,data" "hash_value"
```

## Running Test Cases

The OMNI module includes three pre-configured test cases to demonstrate different capabilities and validate your installation. All commands should be run from the `build/` directory.

### Test Case 1: Quick Validation Test

**Purpose**: Verify basic functionality with minimal data processing
**File**: `config/quick_test.yaml`
**Data**: 1KB from CSV file

```bash
# Command
./bin/wrp ../omni/config/quick_test.yaml

# Expected Output
Parsed OMNI job: quick_test
Max scale: 2
Number of data entries: 1

Processing data entry 1: ../data/A46_xx.csv
Recommended scale for file ../data/A46_xx.csv (size: 106922 bytes): 1 processes, 1 threads per process
Executing: which mpirun >/dev/null 2>&1 && mpirun -np 1 bin/binary_file_omni "../data/A46_xx.csv" 0 1000 "csv,test" "quick_test_hash" || bin/binary_file_omni "../data/A46_xx.csv" 0 1000 "csv,test" "quick_test_hash"
Processing file: ../data/A46_xx.csv
Total size: 1000 bytes
Number of processes: 1
Expected hash: quick_test_hash
Rank 0: Processed 1000 bytes at offset 0
File processing completed successfully

All data entries processed!
```

**What it tests:**
- YAML parsing functionality
- File access and reading
- Basic data processing pipeline
- Command-line argument handling

### Test Case 2: Multi-File Demonstration

**Purpose**: Showcase processing multiple files with different formats and configurations
**File**: `config/demo_job.yaml`
**Data**: CSV (10KB), Parquet (5KB), HDF5 (8KB from offset 1024)

```bash
# Command
./bin/wrp ../omni/config/demo_job.yaml

# Expected Output
Parsed OMNI job: cae_demo_assimilation
Max scale: 4
Number of data entries: 3

Processing data entry 1: ../data/A46_xx.csv
Recommended scale for file ../data/A46_xx.csv (size: 106922 bytes): 1 processes, 1 threads per process
Executing: which mpirun >/dev/null 2>&1 && mpirun -np 1 bin/binary_file_omni "../data/A46_xx.csv" 0 10000 "csv,text,demo_data,aerospace_data" "demo_csv_hash" || bin/binary_file_omni "../data/A46_xx.csv" 0 10000 "csv,text,demo_data,aerospace_data" "demo_csv_hash"
Processing file: ../data/A46_xx.csv
Total size: 10000 bytes
Number of processes: 1
Expected hash: demo_csv_hash
Rank 0: Processed 10000 bytes at offset 0
File processing completed successfully

Processing data entry 2: ../data/A46_xx.arrow.parquet
Recommended scale for file ../data/A46_xx.arrow.parquet (size: 35206 bytes): 1 processes, 1 threads per process
Executing: which mpirun >/dev/null 2>&1 && mpirun -np 1 bin/binary_file_omni "../data/A46_xx.arrow.parquet" 0 5000 "parquet,binary,arrow_format,aerospace_data" || bin/binary_file_omni "../data/A46_xx.arrow.parquet" 0 5000 "parquet,binary,arrow_format,aerospace_data"
Processing file: ../data/A46_xx.arrow.parquet
Total size: 5000 bytes
Number of processes: 1
Rank 0: Processed 5000 bytes at offset 0
File processing completed successfully

Processing data entry 3: ../data/A46_xx.h5
Recommended scale for file ../data/A46_xx.h5 (size: 113704 bytes): 1 processes, 1 threads per process
Executing: which mpirun >/dev/null 2>&1 && mpirun -np 1 bin/binary_file_omni "../data/A46_xx.h5" 1024 8192 "hdf5,binary,scientific_data,aerospace_data" "demo_h5_hash" || bin/binary_file_omni "../data/A46_xx.h5" 1024 8192 "hdf5,binary,scientific_data,aerospace_data" "demo_h5_hash"
Processing file: ../data/A46_xx.h5
Total size: 8192 bytes
Number of processes: 1
Expected hash: demo_h5_hash
Rank 0: Processed 8192 bytes at offset 1024
File processing completed successfully

All data entries processed!
```

**What it tests:**
- Multi-file job processing
- Different file formats (CSV, Parquet, HDF5)
- File offset handling (starting from byte 1024 for HDF5)
- Multiple data descriptions and hash values
- Scale recommendation for different file sizes

### Test Case 3: Custom Job Template

**Purpose**: Provide a template for creating custom jobs
**File**: `config/example_job.yaml`
**Data**: Template with placeholder paths

```bash
# First, copy and customize the template
cp ../omni/config/example_job.yaml my_custom_job.yaml

# Edit the file to point to your actual data files
nano my_custom_job.yaml

# Example customization:
# Change paths from "/path/to/file.txt" to actual file paths
# Adjust offsets and sizes for your data
# Update descriptions and hash values

# Run your custom job
./bin/wrp my_custom_job.yaml
```

**What it demonstrates:**
- Template structure for custom jobs
- All available YAML configuration options
- Different data sizes and processing scenarios
- How to specify multiple data entries

### Test Case Validation

To verify all test cases are working correctly, run them in sequence:

```bash
# Run all test cases
echo "=== Running Quick Test ==="
./bin/wrp ../omni/config/quick_test.yaml

echo -e "\n=== Running Demo Job ==="
./bin/wrp ../omni/config/demo_job.yaml

echo -e "\n=== All tests completed ==="
```

**Alternatively, use the automated test script:**

```bash
# Run the comprehensive test suite
bash ../omni/config/run_all_tests.sh
```

This script will:
- Verify prerequisites (executables and data files)
- Run all test cases in sequence
- Provide clear success/failure indicators
- Give guidance on next steps

### Expected Test Results

✅ **Success indicators:**
- All jobs parse without YAML errors
- File size detection works correctly
- Data processing completes successfully
- No "Error: Cannot open file" messages
- Process scaling recommendations are reasonable

❌ **Failure indicators:**
- YAML parsing errors
- File not found errors
- Process crashes or segmentation faults
- Missing executable errors

### Performance Notes

- **quick_test.yaml**: ~1 second execution time
- **demo_job.yaml**: ~3 seconds execution time (3 files)
- **Actual performance** depends on storage speed and system load

## Jarvis Integration

OMNI includes a Jarvis package (`omni_parse`) for seamless integration with the Jarvis workflow management system. This allows you to:

- Execute OMNI jobs through Jarvis pipeline management
- Integrate with CAE interceptors for I/O interception
- Run automated test suites and experiments
- Manage complex multi-stage workflows

### Quick Jarvis Setup

```bash
# Create and run a simple OMNI pipeline
jarvis ppl create test_omni
jarvis ppl append omni_parse omni_yaml=${PWD}/omni/config/quick_test.yaml
jarvis ppl run
```

### With CAE Adapter Integration

```bash
# Run OMNI with POSIX interception
jarvis ppl create test_omni_posix
jarvis ppl append cae_adapter +posix
jarvis ppl append omni_parse omni_yaml=${PWD}/omni/config/demo_job.yaml
jarvis ppl run
```

### Available Pipeline Examples

The package includes several pre-configured pipeline examples in `test/jarvis_wrp_cae/pipelines/omni/`:

- `test_omni_quick.yaml`: Basic validation pipeline
- `test_omni_demo.yaml`: Multi-file demonstration pipeline  
- `test_omni_scaling.yaml`: Multi-configuration test suite
- `test_omni_with_posix_adapter.yaml`: Integration with I/O interception

For detailed Jarvis integration documentation, see `test/jarvis_wrp_cae/jarvis_wrp_cae/omni_parse/README.md`.

## Example Configurations

The `config/` directory contains several example configurations:

### `config/quick_test.yaml`
- **Purpose**: Quick validation test
- **Data**: 1KB from CSV file
- **Use case**: Verify installation and basic functionality

### `config/demo_job.yaml`
- **Purpose**: Comprehensive demonstration
- **Data**: Multiple file formats (CSV, Parquet, HDF5)
- **Use case**: Showcase multi-file processing capabilities

### `config/example_job.yaml`
- **Purpose**: Template for creating your own jobs
- **Data**: Placeholder paths and configurations
- **Use case**: Starting point for custom job creation

## Creating Your Own Jobs

### Step 1: Create a Job File

```bash
# Copy a template
cp omni/config/example_job.yaml my_job.yaml

# Edit with your file paths and requirements
nano my_job.yaml
```

### Step 2: Configure Your Data

```yaml
name: my_custom_job
max_scale: 8  # Adjust based on your system
data:
- path: /path/to/your/data.bin
  offset: 0
  size: 67108864  # 64MB
  description:
    - binary
    - sensor_data
    - experiment_001
  hash: sha256_of_your_file
```

### Step 3: Run Your Job

```bash
./bin/wrp my_job.yaml
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

## Troubleshooting

### Common Issues

1. **"yaml-cpp not found"**
   ```bash
   # Install yaml-cpp development package
   sudo apt install libyaml-cpp-dev  # Ubuntu/Debian
   sudo dnf install yaml-cpp-devel   # Fedora/CentOS
   ```

2. **"MPI not found"**
   ```bash
   # Install MPI development package
   sudo apt install libopenmpi-dev  # Ubuntu/Debian
   sudo dnf install openmpi-devel   # Fedora/CentOS
   ```

3. **"wrp: command not found"**
   ```bash
   # Ensure you're running from the build directory
   cd build
   ./bin/wrp ../omni/config/quick_test.yaml
   ```

4. **"File not found" errors**
   ```bash
   # Check file paths in your YAML configuration
   # Paths are relative to where you run the wrp command
   ls -la ../data/  # From build directory
   ```

### Debug Mode

For debugging, you can run components separately:

```bash
# Test YAML parsing only
./bin/wrp --parse-only my_job.yaml

# Run binary processor with verbose output
./bin/binary_file_omni /path/to/file 0 1024 "debug" "test" 2>&1 | tee debug.log
```

## Performance Tips

1. **Optimal File Sizes**: Files >= 64MB benefit most from MPI parallelization
2. **Process Count**: Use `max_scale` to limit processes based on your system
3. **I/O Optimization**: Place data files on fast storage (SSD) when possible
4. **Memory Usage**: Each process uses up to 16MB for buffering

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
├── config/                  # Example configurations
│   ├── quick_test.yaml      # Quick validation test
│   ├── demo_job.yaml        # Multi-file demonstration
│   └── example_job.yaml     # Template for custom jobs
└── README.md               # This documentation
```

## Future Enhancements

- **HDF5 Format Client**: Support for HDF5 file processing
- **Globus Repository Client**: Integration with Globus data transfer
- **S3 Repository Client**: Amazon S3 and compatible object storage
- **Advanced Scheduling**: Job queuing and dependency management
- **Data Validation**: Hash verification and integrity checking
- **Progress Tracking**: Real-time job progress monitoring
- **Web Interface**: Browser-based job creation and monitoring 