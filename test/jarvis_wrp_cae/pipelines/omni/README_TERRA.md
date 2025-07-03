# TERRA HDF5 Pipeline Scripts

This directory contains Jarvis pipeline scripts specifically designed to process the TERRA MODIS HDF5 file using the Content Assimilation Engine (CAE) OMNI module.

## Files Overview

### Pipeline Scripts
- **`test_terra_hdf5_load.yaml`** - Simple pipeline using only OMNI parse
- **`test_terra_hdf5_with_adapters.yaml`** - Comprehensive pipeline with CAE adapters and I/O interception
- **`run_terra_pipelines.sh`** - Interactive script to run either pipeline option

### Target File
- **Target**: `~/Downloads/TERRA_BF_L1B_O10204_20011118010522_F000_V001.h5`
- **Type**: TERRA MODIS Level 1B satellite data
- **Date**: November 18, 2001
- **Format**: HDF5

## Prerequisites

### 1. System Requirements
- Jarvis framework installed and configured
- CAE (Content Assimilation Engine) built and available
- MPI environment configured
- Python environment with required Jarvis packages

### 2. File Requirements
- TERRA HDF5 file must exist at: `~/Downloads/TERRA_BF_L1B_O10204_20011118010522_F000_V001.h5`
- OMNI configuration file: `omni/config/terra_hdf5_load.yaml`

### 3. Build Requirements
- Project must be built with CMake (`build/bin/wrp` should exist)
- All CAE adapters should be compiled and available

## Pipeline Options

### Option 1: Simple OMNI Processing
**File**: `test_terra_hdf5_load.yaml`
**Description**: Basic file processing using only the OMNI parser

**Components**:
- `omni_parse` - Executes the OMNI job with the TERRA HDF5 configuration

**Use Case**: Direct file processing without I/O interception or advanced caching

### Option 2: Full CAE Integration
**File**: `test_terra_hdf5_with_adapters.yaml`
**Description**: Complete pipeline with I/O interception and caching capabilities

**Components**:
- `chimaera_run` - IOWarp runtime environment
- `hermes_run` - Hermes I/O acceleration layer  
- `cae_adapter` - I/O interception (POSIX + HDF5 VFD)
- `omni_parse` - OMNI job execution

**Use Case**: Production environment with performance optimization and I/O monitoring

## Usage Methods

### Method 1: Interactive Script (Recommended)
```bash
# From project root directory
./test/jarvis_wrp_cae/pipelines/omni/run_terra_pipelines.sh
```

The script will:
1. Validate the environment and file availability
2. Present a menu with pipeline options
3. Execute the selected pipeline using Jarvis
4. Provide status updates and error handling

### Method 2: Direct Jarvis Commands

#### Simple Pipeline
```bash
# Create and run the basic pipeline
jarvis ppl create terra_hdf5_simple
jarvis ppl load test/jarvis_wrp_cae/pipelines/omni/test_terra_hdf5_load.yaml
jarvis ppl run
```

#### Full Integration Pipeline
```bash
# Create and run the comprehensive pipeline
jarvis ppl create terra_hdf5_adapters
jarvis ppl load test/jarvis_wrp_cae/pipelines/omni/test_terra_hdf5_with_adapters.yaml
jarvis ppl run
```

### Method 3: Manual OMNI Execution
```bash
# Direct execution without Jarvis (from build directory)
./bin/wrp ../omni/config/terra_hdf5_load.yaml
```

## Expected Output

### Successful Execution
```
Processing file: ~/Downloads/TERRA_BF_L1B_O10204_20011118010522_F000_V001.h5
Total size: [file size] bytes
Number of processes: [auto-detected based on file size]
Expected hash: terra_hdf5_full_file
Rank 0: Processed [chunk size] bytes at offset 0
[Additional ranks if MPI scaling is used]
File processing completed successfully
```

### Performance Characteristics
- **File Size**: Typically 200-400 MB for TERRA MODIS Level 1B files
- **Processing Time**: Depends on file size and system capabilities
- **MPI Scaling**: Auto-determined based on file size (minimum 64MB per process)
- **Memory Usage**: Processed in chunks to manage memory efficiently

## Configuration Details

### OMNI Configuration (`terra_hdf5_load.yaml`)
```yaml
name: terra_hdf5_full_load
max_scale: 8  # Up to 8 MPI processes
data:
- path: ~/Downloads/TERRA_BF_L1B_O10204_20011118010522_F000_V001.h5
  offset: 0  # Start from beginning
  # size omitted to read entire file
  description:
    - hdf5
    - binary
    - satellite_data
    - terra_modis
    - level1b
    - scientific_data
    - remote_sensing
    - earth_observation
  hash: terra_hdf5_full_file
```

## Troubleshooting

### Common Issues

1. **File Not Found**
   ```
   Error: Target HDF5 file not found
   ```
   **Solution**: Ensure the TERRA file exists at `~/Downloads/TERRA_BF_L1B_O10204_20011118010522_F000_V001.h5`

2. **Jarvis Not Found**
   ```
   Error: Jarvis command not found
   ```
   **Solution**: Install and configure Jarvis framework, ensure it's in your PATH

3. **wrp Executable Missing**
   ```
   Error: wrp executable not found
   ```
   **Solution**: Build the project (`cmake .. && make`) to generate `build/bin/wrp`

4. **Pipeline Creation Failed**
   ```
   Error: Failed to create pipeline
   ```
   **Solution**: Check Jarvis configuration and ensure proper environment setup

### Debug Options

- Add `do_dbg: true` and `dbg_port: 4000` to pipeline components for debugging
- Check Jarvis logs for detailed execution information
- Use `jarvis ppl status` to monitor pipeline execution
- Review CAE logs for I/O interception details

## Performance Optimization

### For Large Files (>1GB)
- Increase `max_scale` in the OMNI configuration
- Consider using SSD storage for better I/O performance
- Monitor memory usage during processing

### For Multiple Files
- Create batch configurations with multiple data entries
- Use pipeline loops for automated processing
- Implement result aggregation as needed

## Integration Examples

### Batch Processing Multiple TERRA Files
```yaml
vars:
  omni_parse.omni_yaml: 
    - ${PWD}/omni/config/terra_file1.yaml
    - ${PWD}/omni/config/terra_file2.yaml
    - ${PWD}/omni/config/terra_file3.yaml
loop:
  - [omni_parse.omni_yaml]
repeat: 1
```

### Combined with Other CAE Tests
```yaml
pkgs:
  - pkg_type: cae_posix_tests
    pkg_name: cae_posix_tests
    test_file: posix_basic
  - pkg_type: omni_parse
    pkg_name: omni_parse
    omni_yaml: ${PWD}/omni/config/terra_hdf5_load.yaml
```

This documentation should help you effectively use the TERRA HDF5 pipeline scripts for satellite data processing with the Content Assimilation Engine. 