include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CAE_SRC_DIR}
        ${CAE_ADAPTER_DIR}
        ${CAE_IO_CLIENT_DIR}
        .)

# Create the metadata manager singleton + FS base class
add_library(hermes_fs_base SHARED
        filesystem.cc
        filesystem.h
        filesystem_io_client.h
        filesystem_mdm.h)
target_link_libraries(hermes_fs_base
        MPI::MPI_CXX
        hermes::hermes_core_client)

# -----------------------------------------------------------------------------
# Add Target(s) to CMake Install
# -----------------------------------------------------------------------------
install(
        TARGETS
        hermes_fs_base
        LIBRARY DESTINATION ${CAE_INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${CAE_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${CAE_INSTALL_BIN_DIR}
)
install(
        FILES
        filesystem_io_client.h
        DESTINATION
        ${CAE_INSTALL_INCLUDE_DIR}
        COMPONENT
        headers
)

# -----------------------------------------------------------------------------
# Install headers
# -----------------------------------------------------------------------------
file(GLOB_RECURSE CAE_HEADERS "*.h")
install(
        FILES
        ${CAE_HEADERS}
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/include/adapters/filesystem
        COMPONENT
        headers
)

# -----------------------------------------------------------------------------
# Add Target(s) to Coverage
# -----------------------------------------------------------------------------
if(CAE_ENABLE_COVERAGE)
        set_coverage_flags(hermes_fs_base)
endif()
