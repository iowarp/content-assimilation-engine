include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CAE_SRC_DIR}
        ${CAE_ADAPTER_DIR}
        ${CAE_IO_CLIENT_DIR}
        .)

# Create the MPIIO interceptor
if(CAE_MPICH)
        message(STATUS "Using CAE_MPICH")
        add_definitions(-DCAE_MPICH)
elseif(CAE_OPENMPI)
        message(STATUS "Using CAE_OPENMPI")
        add_definitions(-DCAE_OPENMPI)
endif()

set(INTERCEPTOR_DEPS
        cae_config hermes_fs_base)
add_library(hermes_mpiio SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/mpiio_api.cc)
add_dependencies(hermes_mpiio ${INTERCEPTOR_DEPS})
target_link_libraries(hermes_mpiio
        MPI::MPI_CXX stdc++fs ${libelf_LIBRARIES} dl ${INTERCEPTOR_DEPS})

# -----------------------------------------------------------------------------
# Add Target(s) to CMake Install
# -----------------------------------------------------------------------------
install(
        TARGETS
        hermes_mpiio
        LIBRARY DESTINATION ${CAE_INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${CAE_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${CAE_INSTALL_BIN_DIR}
)

# -----------------------------------------------------------------------------
# Install headers
# -----------------------------------------------------------------------------
file(GLOB_RECURSE CAE_HEADERS "*.h")
install(
        FILES
        ${CAE_HEADERS}
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/include/adapters/mpiio
        COMPONENT
        headers
)

# -----------------------------------------------------------------------------
# Add Target(s) to Coverage
# -----------------------------------------------------------------------------
if(CAE_ENABLE_COVERAGE)
        set_coverage_flags(hermes_mpiio)
endif()