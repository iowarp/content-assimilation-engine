# Content Assimilation Engine

A high-performance data ingestion and processing engine designed for heterogeneous storage systems and scientific workflows.

[![win omni r](https://github.com/iowarp/content-assimilation-engine/actions/workflows/win-omni-r.yml/badge.svg)](https://github.com/iowarp/content-assimilation-engine/actions/workflows/win-omni-r.yml)
[![mac omni r](https://github.com/iowarp/content-assimilation-engine/actions/workflows/mac-omni-r.yml/badge.svg)](https://github.com/iowarp/content-assimilation-engine/actions/workflows/mac-omni-r.yml)
[![ubu omni r](https://github.com/iowarp/content-assimilation-engine/actions/workflows/ubu-omni-r.yml/badge.svg)](https://github.com/iowarp/content-assimilation-engine/actions/workflows/ubu-omni-r.yml)
[![docker](https://github.com/iowarp/content-assimilation-engine/actions/workflows/docker.yml/badge.svg)](https://github.com/iowarp/content-assimilation-engine/actions/workflows/docker.yml)
[![synology](https://github.com/iowarp/content-assimilation-engine/actions/workflows/synology.yml/badge.svg)](https://github.com/iowarp/content-assimilation-engine/actions/workflows/synology.yml)



## Quick Start

### Prerequisites

- Spack package manager
- CMake 3.16+
- C++17-capable compiler

### Installation

**Option 1: Using Spack (Recommended)**

```bash
spack install iowarp +mpiio +vfd +compress +encrypt
spack load iowarp
```

**Option 2: Manual Build**

```bash
cd content-assimilation-engine
mkdir build
cd build
cmake --preset debug .
cd build
cmake --build .
cmake --install .
```

## Testing

### Running Tests

```bash
spack load iowarp
module load content-assimilation-engine
jarvis env build hermes
ctest
```

### Continuous Integration

The project includes comprehensive CI/CD pipelines:
- **Cross-platform testing**: Windows, macOS, Ubuntu
- **Container testing**: Docker and Synology NAS
- **Automated builds**: Multiple configurations and environments

## Development

### Build Presets

The project uses CMake presets for different build configurations:
- `debug`: Development build with debugging symbols
- Additional presets available in `CMakePresets.json`

### Project Structure

- `adapters/`: Storage and I/O adapters
- `omni/`: Core engine components
- `test/`: Unit tests and integration tests
- `data/`: Sample datasets and test data
- `config/`: Configuration files and templates

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the BSD-3-Clause License - see the [LICENSE](LICENSE) file for details.

**Copyright (c) 2024, Gnosis Research Center, Illinois Institute of Technology**

## Links

- **IOWarp Organization**: [https://github.com/iowarp](https://github.com/iowarp)
- **Documentation**: Coming soon
- **Issues**: [GitHub Issues](https://github.com/iowarp/content-assimilation-engine/issues)