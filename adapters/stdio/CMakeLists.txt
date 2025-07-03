include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CAE_SRC_DIR}
        ${CAE_ADAPTER_DIR}
        ${CAE_IO_CLIENT_DIR}
        .)

# Create the STDIO interceptor
set(INTERCEPTOR_DEPS
        hermes::hermes_core_client hermes_fs_base)
add_library(hermes_stdio SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/stdio_api.cc)
add_dependencies(hermes_stdio ${INTERCEPTOR_DEPS})
target_link_libraries(hermes_stdio MPI::MPI_CXX stdc++fs ${libelf_LIBRARIES} ${INTERCEPTOR_DEPS})

# -----------------------------------------------------------------------------
# Add Target(s) to CMake Install
# -----------------------------------------------------------------------------
install(
        TARGETS
        hermes_stdio
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
        ${CMAKE_INSTALL_PREFIX}/include/adapters/stdio
        COMPONENT
        headers
)

# -----------------------------------------------------------------------------
# Add Target(s) to Coverage
# -----------------------------------------------------------------------------
if(CAE_ENABLE_COVERAGE)
        set_coverage_flags(hermes_stdio)
endif()
