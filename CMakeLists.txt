cmake_minimum_required(VERSION 3.25)
project(cae VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)

# -----------------------------------------------------------------------------
# Set CAE Version Variables
# -----------------------------------------------------------------------------
set(CAE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CAE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CAE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# -----------------------------------------------------------------------------
# Define Options
# -----------------------------------------------------------------------------
option(BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" ON)
option(BUILD_MPI_TESTS "Build tests which depend on MPI" ON)
option(CAE_ENABLE_CMAKE_DOTENV "Load environment variables from .env.cmake" OFF)

option(CAE_ENABLE_NVIDIA_GDS_ADAPTER "Build the Hermes NVIDIA GDS adapter." OFF)
option(CAE_ENABLE_POSIX_ADAPTER "Build the Hermes POSIX adapter." ON)
option(CAE_ENABLE_STDIO_ADAPTER "Build the Hermes stdio adapter." OFF)
option(CAE_ENABLE_MPIIO_ADAPTER "Build the Hermes MPI-IO adapter." OFF)
option(CAE_ENABLE_VFD "Build the Hermes HDF5 Virtual File Driver" OFF)

option(CAE_MPICH "Specify that this a MPICH build" OFF)
option(CAE_OPENMPI "Specify that this a OpenMPI build" OFF)

option(CAE_ENABLE_CUDA "Enable CUDA support" OFF)
option(CAE_ENABLE_ROCM "Enable ROCm support" OFF)

# A hack for spack to get dependencies
option(CAE_NO_COMPILE "Don't compile the code" OFF)

if(CAE_NO_COMPILE)
    install(
        FILES
        ${CMAKE_SOURCE_DIR}/.gitignore
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/.gitignore
    )
    return()
endif()

# ------------------------------------------------------------------------------
# DOTENV
# ------------------------------------------------------------------------------ 
if(CAE_ENABLE_CMAKE_DOTENV)
    if(EXISTS ${CMAKE_SOURCE_DIR}/.env.cmake)
        include(.env.cmake)
    endif() 
endif()

# -----------------------------------------------------------------------------
# Compiler Optimization
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("IN DEBUG MODE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    add_compile_definitions(CAE_LOG_VERBOSITY=10)
else()
    message("IN RELEASE MODE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O3")
    add_compile_definitions(CAE_LOG_VERBOSITY=1)
endif()

# Enable Position Independent Code globally
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# -----------------------------------------------------------------------------
# Targets built within this project are exported at Install time for use
# by other projects.
# -----------------------------------------------------------------------------
if(NOT CHIMAERA_EXPORTED_TARGETS)
    set(CHIMAERA_EXPORTED_TARGETS "HermesCore")
endif()

# -----------------------------------------------------------------------------
# Code Coverage
# -----------------------------------------------------------------------------
if(CAE_ENABLE_COVERAGE)
    set(COVERAGE_FLAGS "-fprofile-arcs -ftest-coverage --coverage" CACHE STRING
        "Flags to the coverage program to perform coverage inspection"
    )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${COVERAGE_FLAGS}")

    mark_as_advanced(COVERAGE_FLAGS)

    macro(set_coverage_flags target)
        target_link_libraries(${target} gcov)

        # set_target_properties(${target}
        # PROPERTIES
        # COMPILE_FLAGS ${COVERAGE_FLAGS}
        # LINK_FLAGS ${COVERAGE_FLAGS}
        # )
    endmacro()
endif()

add_custom_target(coverage COMMAND bash ${CMAKE_SOURCE_DIR}/ci/coverage.sh
    ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR})

# ------------------------------------------------------------------------------
# Setup install and output Directories
# ------------------------------------------------------------------------------
include(GNUInstallDirs)

if(NOT CAE_INSTALL_BIN_DIR)
    set(CAE_INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR})
endif()

if(NOT CAE_INSTALL_LIB_DIR)
    set(CAE_INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
endif()

if(NOT CAE_INSTALL_INCLUDE_DIR)
    set(CAE_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR})
endif()

if(NOT CAE_INSTALL_DATA_DIR)
    set(CAE_INSTALL_DATA_DIR ${CMAKE_INSTALL_DATADIR})
endif()

# -----------------------------------------------------------------------------
# Find Packages
# -----------------------------------------------------------------------------

# This is for compatibility with CLANG + Spack
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_INSTALL_RPATH "${CAE_INSTALL_LIB_DIR}")
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# ------------------------------------------------------------------------------
# Setup CMake Environment
# ------------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables.")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries.")

# -----------------------------------------------------------------------------
# Build Hermes Main Packages
# -----------------------------------------------------------------------------
# Set CAE directory variables for adapters
set(CAE_SRC_DIR ${CMAKE_SOURCE_DIR})
set(CAE_ADAPTER_DIR ${CMAKE_SOURCE_DIR}/adapters)
set(CAE_IO_CLIENT_DIR ${CMAKE_SOURCE_DIR}/adapters)
set(CAE_VFD_DIR_NAME "vfd")

set(TEST_MAIN ${CMAKE_SOURCE_DIR}/test/unit) 
# add_subdirectory(adapters)
add_subdirectory(omni)
