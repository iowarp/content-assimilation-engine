include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CAE_SRC_DIR}
        ${CAE_ADAPTER_DIR}
        ${CAE_IO_CLIENT_DIR}
        .)

# Create the POSIX interceptor
set(INTERCEPTOR_DEPS
        cae_config cae_fs_base)
add_library(cae_posix SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/posix_api.cc)
add_dependencies(cae_posix ${INTERCEPTOR_DEPS})
target_link_libraries(cae_posix MPI::MPI_CXX stdc++fs ${libelf_LIBRARIES} ${INTERCEPTOR_DEPS})
set_target_properties(cae_posix PROPERTIES POSITION_INDEPENDENT_CODE ON)


# -----------------------------------------------------------------------------
# Add Target(s) to CMake Install
# -----------------------------------------------------------------------------
install(
        TARGETS
        cae_posix
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
        ${CMAKE_INSTALL_PREFIX}/include/adapters/posix
        COMPONENT
        headers
)

# -----------------------------------------------------------------------------
# Add Target(s) to Coverage
# -----------------------------------------------------------------------------
if(CAE_ENABLE_COVERAGE)
        set_coverage_flags(cae_posix)
endif()
