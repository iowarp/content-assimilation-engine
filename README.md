# Content Assimilation Engine

A high-performance data ingestion and processing engine designed for heterogeneous storage systems and scientific workflows.

[![win omni r](https://github.com/iowarp/content-assimilation-engine/actions/workflows/win-omni-r.yml/badge.svg)](https://github.com/iowarp/content-assimilation-engine/actions/workflows/win-omni-r.yml)
[![mac omni r](https://github.com/iowarp/content-assimilation-engine/actions/workflows/mac-omni-r.yml/badge.svg)](https://github.com/iowarp/content-assimilation-engine/actions/workflows/mac-omni-r.yml)
[![ubu omni r](https://github.com/iowarp/content-assimilation-engine/actions/workflows/ubu-omni-r.yml/badge.svg)](https://github.com/iowarp/content-assimilation-engine/actions/workflows/ubu-omni-r.yml)
[![docker](https://github.com/iowarp/content-assimilation-engine/actions/workflows/docker.yml/badge.svg)](https://github.com/iowarp/content-assimilation-engine/actions/workflows/docker.yml)
[![synology](https://github.com/iowarp/content-assimilation-engine/actions/workflows/synology.yml/badge.svg)](https://github.com/iowarp/content-assimilation-engine/actions/workflows/synology.yml)

## Quick Start with OMNI

### IOWarp

**Ubuntu/Debian:**
```bash
spack install iowarp
```

### Building OMNI (manually)

```bash
git clone https://github.com/iowarp/content-assimilation-engine.git
cd content-assimilation-engine
mkdir build && cd build
cmake ..
make
make install
```

This builds and installs two executables:
- `wrp` - Main YAML job orchestrator
- `wrp_binary_format_mpi` - MPI binary format processor

### Running an Example

The repository includes a simple example configuration at `omni/config/example_simple.yaml`:

```yaml
# Simple OMNI example using repository data files
name: example_data_ingestion
max_scale: 4  # Maximum number of processes

data:
  - path: data/A46_xx.parquet
    offset: 0
    size: 31744
    description:
      - parquet
      - structured_data

  - path: data/datahub.csv
    range: [0, 671]
    description:
      - csv
      - tabular
```

Run it from the repository root:

```bash
mpirun -np 4 wrp omni/config/example_simple.yaml
```

### OMNI Format Specification

The OMNI format uses YAML to describe data ingestion jobs:

```yaml
name: job_name              # Job identifier (required)
max_scale: 100              # Max number of MPI processes (required)

data:                       # List of data sources
  - path: /file/path        # File path (required)
    offset: 0               # Starting byte offset (optional)
    size: 1024              # Bytes to read from offset (optional)
    range: [0, 1024]        # Alternative: [start, end] byte range (optional)
    description:            # Tags describing the data (optional)
      - binary
      - structured
    hash: sha256_value      # Integrity verification (optional)
```

**Key Fields:**
- `path`: Absolute or relative file path
- `offset` + `size`: Read `size` bytes starting at `offset`
- `range`: Alternative to offset/size, specifies [start, end] bytes
- `description`: List of tags for metadata/categorization
- `hash`: SHA256 hash for data verification

### Example Configurations

The repository includes several example configurations in `omni/config/`:
- `quick_test.yaml` - Simple test case
- `demo_job.yaml` - Demonstration job
- `example_job.yaml` - Annotated example with all options
- `wildcard_test.yaml` - Pattern matching examples

Run examples:
```bash
cd build/omni/config
mpirun -np 2 ../../bin/wrp quick_test.yaml
```

## Development

### Project Structure

- `omni/` - OMNI module (job orchestration and format processing)
  - `format/` - Binary format handlers
  - `repo/` - Repository and storage backends
  - `config/` - Example job configurations
- `data/` - Sample datasets for testing


## License

This project is licensed under the BSD-3-Clause License - see the [LICENSE](LICENSE) file for details.

**Copyright (c) 2024, Gnosis Research Center, Illinois Institute of Technology**

## Links

- **IOWarp Organization**: [https://github.com/iowarp](https://github.com/iowarp)
- **Issues**: [GitHub Issues](https://github.com/iowarp/content-assimilation-engine/issues)
