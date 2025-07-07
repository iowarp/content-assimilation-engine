#------------------------------------------------------------------------------
# Build adapters
#------------------------------------------------------------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(config)
add_subdirectory(filesystem)
if (CAE_ENABLE_POSIX_ADAPTER)
    add_subdirectory(posix)
endif()
if (CAE_ENABLE_STDIO_ADAPTER)
    add_subdirectory(stdio)
endif()
if (CAE_ENABLE_MPIIO_ADAPTER)
    add_subdirectory(mpiio)
endif()
if (CAE_ENABLE_VFD)
    add_subdirectory(vfd)
endif()
if (CAE_ENABLE_NVIDIA_GDS_ADAPTER)
    add_subdirectory(nvidia_gds)
endif()

#-----------------------------------------------------------------------------
# Install adapter headers
#-----------------------------------------------------------------------------
file(GLOB_RECURSE CAE_HEADERS "*.h")
install(
        FILES
        ${CAE_HEADERS}
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/include/adapters
        COMPONENT
        headers
)
install(DIRECTORY mapper DESTINATION ${CMAKE_INSTALL_PREFIX}/include/adapters)