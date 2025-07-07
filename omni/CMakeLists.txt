# Content Assimilation Engine (CAE) - OMNI Module
# Note: This CMakeLists.txt is designed to be included from the main project

# Check if we have the necessary dependencies available from the parent project
if(NOT TARGET MPI::MPI_CXX)
    message(STATUS "MPI not found in parent project, attempting to find it")
    find_package(MPI REQUIRED)
endif()

# Check for yaml-cpp
find_package(yaml-cpp REQUIRED)
if (${YAML_CPP_LIBRARIES})
    set(YAML_CPP_LIBS ${YAML_CPP_LIBRARIES})
elseif(TARGET yaml-cpp::yaml-cpp)
        set(YAML_CPP_LIBS yaml-cpp::yaml-cpp)
else()
  set(YAML_CPP_LIBS yaml-cpp)
endif()

message(STATUS "Building OMNI module with yaml-cpp")

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Source files for the factory and repository implementations
set(OMNI_FACTORY_SOURCES
    factory.cc
    format/format_factory.cc
    repo/repo_factory.cc
)

# Create a static library for OMNI components
add_library(omni_lib STATIC ${OMNI_FACTORY_SOURCES})
target_link_libraries(omni_lib MPI::MPI_CXX ${YAML_CPP_LIBS})
target_include_directories(omni_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Main YAML parser and job orchestrator (wrp binary)
add_executable(wrp wrp.cc)
target_link_libraries(wrp omni_lib MPI::MPI_CXX ${YAML_CPP_LIBS})
target_include_directories(wrp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# MPI binary format processor (wrp_binary_format_mpi binary)
add_executable(wrp_binary_format_mpi wrp_binary_format_mpi.cc)
target_link_libraries(wrp_binary_format_mpi MPI::MPI_CXX)
target_include_directories(wrp_binary_format_mpi PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set output directory for binaries to match the main project
set_target_properties(wrp wrp_binary_format_mpi PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Install targets
install(TARGETS wrp wrp_binary_format_mpi
    RUNTIME DESTINATION ${CAE_INSTALL_BIN_DIR}
)

install(TARGETS omni_lib
    ARCHIVE DESTINATION ${CAE_INSTALL_LIB_DIR}
)

# Install header files with new structure
install(FILES 
    format/format_client.h
    format/format_factory.h
    format/binary_file_omni.h
    DESTINATION ${CAE_INSTALL_INCLUDE_DIR}/omni/format
)

install(FILES 
    repo/repo_client.h
    repo/repo_factory.h
    repo/filesystem_repo_omni.h
    DESTINATION ${CAE_INSTALL_INCLUDE_DIR}/omni/repo
)

# Install configuration examples
install(DIRECTORY config/
    DESTINATION ${CAE_INSTALL_DATA_DIR}/omni/config
    FILES_MATCHING PATTERN "*.yaml"
)

# Install documentation
install(FILES README.md
    DESTINATION ${CAE_INSTALL_DATA_DIR}/omni
)

# Configure and install configuration files
configure_file(config/quick_test.yaml ${CMAKE_BINARY_DIR}/omni/config/quick_test.yaml COPYONLY)
configure_file(config/demo_job.yaml ${CMAKE_BINARY_DIR}/omni/config/demo_job.yaml COPYONLY)
configure_file(config/example_job.yaml ${CMAKE_BINARY_DIR}/omni/config/example_job.yaml COPYONLY)

# Install test script with executable permissions
install(PROGRAMS config/run_all_tests.sh
        DESTINATION ${CMAKE_BINARY_DIR}/omni/config/
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                   GROUP_READ GROUP_EXECUTE
                   WORLD_READ WORLD_EXECUTE) 