include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src)

# ------------------------------------------------------------------------------
# STDIO Adapter Internal tests
# ------------------------------------------------------------------------------
add_executable(cae_stdio_adapter_mapper_test
  ${CMAKE_CURRENT_SOURCE_DIR}/stdio_adapter_test.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stdio_adapter_mapper_test.cc)
add_dependencies(cae_stdio_adapter_mapper_test
  cae_stdio)
target_link_libraries(cae_stdio_adapter_mapper_test
  cae_stdio)
target_compile_definitions(cae_stdio_adapter_mapper_test PUBLIC
  CATCH_CONFIG_ENABLE_BENCHMARKING)
jarvis_test(stdio test_wrp_cae_cte_stdio_mapper)
jarvis_test(stdio test_wrp_cae_stdio_mapper)

# ------------------------------------------------------------------------------
# Non-Hermes, None-MPI STDIO basic
add_executable(stdio_adapter_test
  stdio_adapter_test.cc
  stdio_adapter_basic_test.cc
  stdio_adapter_func_test.cc
  stdio_adapter_rs_test.cc)
target_link_libraries(stdio_adapter_test
  cae_config)
target_compile_definitions(stdio_adapter_test PUBLIC
  HERMES_MPI_TESTS=false)

# Hermes, None-MPI STDIO basic
add_executable(cae_stdio_adapter_test
  ${CMAKE_CURRENT_SOURCE_DIR}/stdio_adapter_test.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stdio_adapter_basic_test.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stdio_adapter_rs_test.cc)
add_dependencies(cae_stdio_adapter_test
  cae_stdio)
target_link_libraries(cae_stdio_adapter_test
  cae_stdio)
target_compile_definitions(cae_stdio_adapter_test PUBLIC
  CATCH_CONFIG_ENABLE_BENCHMARKING)
jarvis_test(stdio test_wrp_cae_cte_stdio_basic_small)
jarvis_test(stdio test_wrp_cae_cte_stdio_basic_large)
jarvis_test(stdio test_wrp_cae_stdio_basic_small)
jarvis_test(stdio test_wrp_cae_stdio_basic_large)
jarvis_test(stdio test_wrp_cae_stdio_basic)

# Non-Hermes, MPI STDIO basic
add_executable(stdio_adapter_mpi_test
  stdio_adapter_test.cc
  stdio_adapter_basic_test.cc
  stdio_adapter_rs_test.cc
  stdio_adapter_shared_test.cc)
target_link_libraries(stdio_adapter_mpi_test
  cae_config)
target_compile_definitions(stdio_adapter_mpi_test PUBLIC
  HERMES_MPI_TESTS=true)

# Hermes, MPI STDIO basic
add_executable(cae_stdio_adapter_mpi_test
  ${CMAKE_CURRENT_SOURCE_DIR}/stdio_adapter_test.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stdio_adapter_basic_test.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stdio_adapter_rs_test.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stdio_adapter_shared_test.cc)
target_link_libraries(cae_stdio_adapter_mpi_test
  cae_stdio)
add_dependencies(cae_stdio_adapter_mpi_test
  cae_stdio)
target_compile_definitions(cae_stdio_adapter_mpi_test PUBLIC
  CATCH_CONFIG_ENABLE_BENCHMARKING)
jarvis_test(stdio test_wrp_cae_cte_stdio_basic_mpi_small)
jarvis_test(stdio test_wrp_cae_cte_stdio_basic_mpi_large)
jarvis_test(stdio test_wrp_cae_stdio_basic_mpi_small)
jarvis_test(stdio test_wrp_cae_stdio_basic_mpi_large)
jarvis_test(stdio test_wrp_cae_stdio_basic_mpi)

# Hermes, Low buffer space
add_executable(cae_stdio_low_buf_adapter_test
  ${CMAKE_CURRENT_SOURCE_DIR}/stdio_adapter_test.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stdio_adapter_low_buffer_space_test.cc)
target_link_libraries(cae_stdio_low_buf_adapter_test cae_stdio)
add_dependencies(cae_stdio_low_buf_adapter_test cae_stdio)
target_compile_definitions(cae_stdio_low_buf_adapter_test PUBLIC
  CATCH_CONFIG_ENABLE_BENCHMARKING)
jarvis_test(stdio test_wrp_cae_cte_stdio_low_buf)
jarvis_test(stdio test_wrp_cae_stdio_low_buf)

# Hermes, Adapter mode
add_executable(cae_stdio_adapter_mode_test
  ${CMAKE_CURRENT_SOURCE_DIR}/stdio_adapter_test.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stdio_adapter_mode_test.cc)
target_link_libraries(cae_stdio_adapter_mode_test
  cae_stdio)
add_dependencies(cae_stdio_adapter_mode_test
  cae_stdio)
target_compile_definitions(cae_stdio_adapter_mode_test PUBLIC
  CATCH_CONFIG_ENABLE_BENCHMARKING)
jarvis_test(stdio test_wrp_cae_cte_stdio_adapter_bypass)
jarvis_test(stdio test_wrp_cae_cte_stdio_adapter_default)
jarvis_test(stdio test_wrp_cae_cte_stdio_adapter_scratch)
jarvis_test(stdio test_wrp_cae_stdio_adapter_bypass)
jarvis_test(stdio test_wrp_cae_stdio_adapter_default)
jarvis_test(stdio test_wrp_cae_stdio_adapter_scratch)

set(stdio_tests
  stdio_adapter_test
  cae_stdio_adapter_test
  cae_stdio_low_buf_adapter_test
  cae_stdio_adapter_mapper_test
  cae_stdio_adapter_mode_test
  stdio_adapter_mpi_test
  cae_stdio_adapter_mpi_test)

foreach(program ${stdio_tests})
  target_include_directories(${program} PRIVATE ${HERMES_ADAPTER_DIR})
  target_include_directories(${program} PRIVATE ${HERMES_ADAPTER_TEST_DIR})
  target_link_libraries(${program} Catch2::Catch2 -lstdc++fs -lc MPI::MPI_CXX)
endforeach()

foreach(program ${stdio_tests})
  install(
    TARGETS
    ${program}
    RUNTIME DESTINATION ${CAE_INSTALL_BIN_DIR}
  )
endforeach()
