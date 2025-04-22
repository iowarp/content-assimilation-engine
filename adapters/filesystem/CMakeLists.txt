find_package(Hermes REQUIRED)
include_directories(
        ${CMAKE_SOURCE_DIR}
        ${HERMES_SRC_DIR}
        ${HERMES_ADAPTER_DIR}
        ${HERMES_IO_CLIENT_DIR}
        .)

# Create the metadata manager singleton + FS base class
add_library(hermes_fs_base INTERFACE)
add_dependencies(hermes_fs_base hermes::hermes_core_client)
target_link_libraries(hermes_fs_base INTERFACE
        MPI::MPI_CXX
        ${Hermes_CLIENT_DEPS})

# -----------------------------------------------------------------------------
# Add Target(s) to CMake Install
# -----------------------------------------------------------------------------
install(
        TARGETS
        hermes_fs_base
        LIBRARY DESTINATION ${HERMES_LIB_DIR}
        ARCHIVE DESTINATION ${HERMES_LIB_DIR}
        RUNTIME DESTINATION ${HERMES_BIN_DIR}
)
install(
        FILES
        filesystem_io_client.h
        DESTINATION
        ${HERMES_INCLUDE_DIR}
        COMPONENT
        headers
)

# -----------------------------------------------------------------------------
# Install headers
# -----------------------------------------------------------------------------
file(GLOB_RECURSE HERMES_HEADERS "*.h")
install(
        FILES
        ${HERMES_HEADERS}
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/include/hermes_adapters/filesystem
        COMPONENT
        headers
)

# -----------------------------------------------------------------------------
# Add Target(s) to Coverage
# -----------------------------------------------------------------------------
if(HERMES_ENABLE_COVERAGE)
        set_coverage_flags(hermes_fs_base)
endif()
