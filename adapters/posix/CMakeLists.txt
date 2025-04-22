find_package(Hermes REQUIRED)
find_package(Chimaera CONFIG REQUIRED)
message(STATUS "found chimaera at ${CHIMAERA_INCLUDE_DIR}")

find_package(Thallium REQUIRED)
message(STATUS "Thallium_DIR: ${Thallium_DIR}")
message(STATUS "Thallium_INCLUDE_DIRECTORIES: ${Thallium_INCLUDE_DIRECTORIES}")
message(STATUS "THALLIUM_INCLUDE_DIR: ${THALLIUM_INCLUDE_DIR}")
message(STATUS "Thallium_INCLUDES: ${Thallium_INCLUDES}")
set(TDIR  ${Thallium_DIR}/../../../include)
message(${TDIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules(Margo REQUIRED margo)
message(STATUS "Margo_INCLUDE_DIRS: ${Margo_INCLUDE_DIRS}")

find_package(cereal REQUIRED)
message(STATUS "found cereal at ${cereal_DIR}")
set(CDIR  ${cereal_DIR}/../../../include)
message(${CDIR})

find_package(yaml-cpp REQUIRED)
message(STATUS "found yaml-cpp at ${YAML_CPP_INCLUDE_DIR}")

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${HERMES_SRC_DIR}
        ${HERMES_ADAPTER_DIR}
        ${HERMES_IO_CLIENT_DIR}
        ${TDIR}
        ${Margo_INCLUDE_DIRS}
        ${CDIR}
	${YAML_CPP_INCLUDE_DIR}
        .)

# Create the POSIX interceptor
set(INTERCEPTOR_DEPS hermes::hermes_core_client hermes_fs_base)
add_library(hermes_posix SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/posix_api.cc)
add_dependencies(hermes_posix ${INTERCEPTOR_DEPS})
target_link_libraries(hermes_posix MPI::MPI_CXX stdc++fs ${libelf_LIBRARIES} ${INTERCEPTOR_DEPS})

# -----------------------------------------------------------------------------
# Add Target(s) to CMake Install
# -----------------------------------------------------------------------------
install(
        TARGETS
        hermes_posix
        LIBRARY DESTINATION ${HERMES_INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${HERMES_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${HERMES_INSTALL_BIN_DIR}
)

# -----------------------------------------------------------------------------
# Install headers
# -----------------------------------------------------------------------------
file(GLOB_RECURSE HERMES_HEADERS "*.h")
install(
        FILES
        ${HERMES_HEADERS}
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/include/hermes_adapters/posix
        COMPONENT
        headers
)

# -----------------------------------------------------------------------------
# Add Target(s) to Coverage
# -----------------------------------------------------------------------------
if(HERMES_ENABLE_COVERAGE)
        set_coverage_flags(hermes_posix)
endif()
