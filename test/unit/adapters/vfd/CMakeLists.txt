set(CAE_VFD_DIR ${HERMES_ADAPTER_DIR}/vfd)

set(cae_vfd_tests
        cae_vfd_adapter_test
)
add_executable(cae_vfd_adapter_test
        ${CMAKE_CURRENT_SOURCE_DIR}/hermes_vfd_test.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/hermes_vfd_basic_test.cc)
target_include_directories(cae_vfd_adapter_test PRIVATE ${CAE_VFD_DIR})
target_include_directories(cae_vfd_adapter_test PRIVATE ${HERMES_ADAPTER_TEST_DIR})
target_include_directories(cae_vfd_adapter_test
        SYSTEM PRIVATE ${HDF5_CAE_VFD_EXT_INCLUDE_DEPENDENCIES}
)
target_compile_definitions(cae_vfd_adapter_test PUBLIC
        CATCH_CONFIG_ENABLE_BENCHMARKING)
target_link_libraries(cae_vfd_adapter_test
        hdf5_cae_vfd
        Catch2::Catch2
        -lstdc++fs
        -lc
        MPI::MPI_CXX)

foreach(program ${cae_vfd_tests})
  install(
    TARGETS
    ${program}
    RUNTIME DESTINATION ${CAE_INSTALL_BIN_DIR}
  )
endforeach()

jarvis_test(vfd test_wrp_cae_cte_vfd_basic)
jarvis_test(vfd test_wrp_cae_cte_vfd_scratch)
jarvis_test(vfd test_wrp_cae_cte_vfd_python)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/hermes_vfd_py_test.py
        DESTINATION ${CAE_INSTALL_BIN_DIR}
        FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)