# Content Assimilation Engine
[![win omni r](https://github.com/iowarp/content-assimilation-engine/actions/workflows/win-omni-r.yml/badge.svg)](https://github.com/iowarp/content-assimilation-engine/actions/workflows/win-omni-r.yml)
[![mac omni r](https://github.com/iowarp/content-assimilation-engine/actions/workflows/mac-omni-r.yml/badge.svg)](https://github.com/iowarp/content-assimilation-engine/actions/workflows/mac-omni-r.yml)
[![ubu omni r](https://github.com/iowarp/content-assimilation-engine/actions/workflows/ubu-omni-r.yml/badge.svg)](https://github.com/iowarp/content-assimilation-engine/actions/workflows/ubu-omni-r.yml)
[![docker](https://github.com/iowarp/content-assimilation-engine/actions/workflows/docker.yml/badge.svg)](https://github.com/iowarp/content-assimilation-engine/actions/workflows/docker.yml) [![synology](https://github.com/iowarp/content-assimilation-engine/actions/workflows/synology.yml/badge.svg)](https://github.com/iowarp/content-assimilation-engine/actions/workflows/synology.yml)

## Installation (Manual)

```
spack install iowarp +mpiio +vfd +compress +encrypt
spack load iowarp
```

```
cd content-assimilation-engine
mkdir build
cd build
cmake --preset debug .
cd build
cmake --build .
cmake --install .
```

## CTest

```
spack load iowarp
module load content-assimilation-engine
jarvis env build hermes
ctest
```