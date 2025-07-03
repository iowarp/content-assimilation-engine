include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_cuda_library(hermes_nvidia_gds SHARED TRUE cufile_api.cc)
target_link_libraries(hermes_nvidia_gds PUBLIC hshm::cudacxx MPI::MPI_CXX)

install(TARGETS hermes_nvidia_gds
        LIBRARY DESTINATION ${CAE_INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${CAE_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${CAE_INSTALL_BIN_DIR}
        )