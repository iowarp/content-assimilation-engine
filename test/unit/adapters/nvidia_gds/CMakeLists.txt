set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
project(MegaGPU LANGUAGES C CXX CUDA)

add_cuda_executable(gds_adapter_basic_test TRUE
        gds_adapter_basic_test.cc)
target_link_libraries(gds_adapter_basic_test
        PUBLIC hshm::cudacxx cufile) # hermes added
jarvis_test(nvidia_gds test_wrp_cae_nvidia_gds_basic)

add_cuda_executable(gds_adapter_simple_test TRUE
        gds_adapter_simple_test.cc)
target_link_libraries(gds_adapter_simple_test
        PUBLIC hshm::cudacxx cufile) # hermes added

SET(NVIDIA_GDS_TESTS
        gds_adapter_basic_test
        gds_adapter_simple_test
)

foreach(program ${NVIDIA_GDS_TESTS})
        install(
                TARGETS
                ${program}
                LIBRARY DESTINATION ${CAE_INSTALL_LIB_DIR}
                ARCHIVE DESTINATION ${CAE_INSTALL_LIB_DIR}
                RUNTIME DESTINATION ${CAE_INSTALL_BIN_DIR}
        )
endforeach()