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
# Build OMNI Module
# -----------------------------------------------------------------------------
add_subdirectory(omni)
