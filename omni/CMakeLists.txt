# Content Assimilation Engine (CAE) - OMNI Module
# Note: This CMakeLists.txt is designed to be included from the main project

# Check if we have the necessary dependencies available from the parent project
if(NOT TARGET MPI::MPI_CXX)
    message(STATUS "MPI not found in parent project, attempting to find it")
    find_package(MPI REQUIRED)
endif()

# Check for yaml-cpp
find_package(PkgConfig REQUIRED)
pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)

if(NOT YAML_CPP_FOUND)
    message(WARNING "yaml-cpp not found, OMNI module will not be built")
    return()
endif()

message(STATUS "Building OMNI module with yaml-cpp")

# Include directories
include_directories(${YAML_CPP_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files for the factory and repository implementations
set(OMNI_FACTORY_SOURCES
    factory.cc
    filesystem_repo_client.cc
)

# Create a static library for OMNI components
add_library(omni_lib STATIC ${OMNI_FACTORY_SOURCES})
target_link_libraries(omni_lib MPI::MPI_CXX ${YAML_CPP_LIBRARIES})
target_include_directories(omni_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Binary file OMNI processor (MPI binary)
add_executable(binary_file_omni binary_file_omni.cc)
target_link_libraries(binary_file_omni omni_lib MPI::MPI_CXX)
target_include_directories(binary_file_omni PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Main YAML parser (wrp binary)
add_executable(wrp wrp.cc)
target_link_libraries(wrp omni_lib MPI::MPI_CXX ${YAML_CPP_LIBRARIES})
target_include_directories(wrp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set output directory for binaries to match the main project
set_target_properties(binary_file_omni wrp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Install targets
install(TARGETS binary_file_omni wrp
    RUNTIME DESTINATION ${CAE_INSTALL_BIN_DIR}
)

install(TARGETS omni_lib
    ARCHIVE DESTINATION ${CAE_INSTALL_LIB_DIR}
)

install(FILES 
    OMNI.h 
    OMNI_factory.h
    binary_file_omni.h
    filesystem_repo_omni.h
    DESTINATION ${CAE_INSTALL_INCLUDE_DIR}/omni
) 