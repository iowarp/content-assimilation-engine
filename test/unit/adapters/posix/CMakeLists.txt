include_directories(${CMAKE_SOURCE_DIR}/src/api)
include_directories(${CMAKE_SOURCE_DIR}/src)

# ------------------------------------------------------------------------------
# Posix Adapter tests
# ------------------------------------------------------------------------------
add_executable(posix_adapter_test
        posix_adapter_test.cc
        posix_adapter_basic_test.cc
        posix_adapter_rs_test.cc)
target_link_libraries(posix_adapter_test
        cae_config)
target_compile_definitions(posix_adapter_test PUBLIC
        HERMES_MPI_TESTS=false)

add_executable(cae_posix_adapter_test
        posix_adapter_test.cc
        posix_adapter_basic_test.cc
        posix_adapter_rs_test.cc)
add_dependencies(cae_posix_adapter_test
        cae_posix)
target_link_libraries(cae_posix_adapter_test
        cae_posix)
target_compile_definitions(cae_posix_adapter_test PUBLIC
        HERMES_INTERCEPT=1 HERMES_MPI_TESTS=false CATCH_CONFIG_ENABLE_BENCHMARKING)
jarvis_test(posix test_wrp_cae_cte_posix_basic_small)
jarvis_test(posix test_wrp_cae_cte_posix_basic_large)

add_executable(posix_adapter_mpi_test
        posix_adapter_test.cc
        posix_adapter_basic_test.cc
        posix_adapter_rs_test.cc
        posix_adapter_shared_test.cc)
target_link_libraries(posix_adapter_mpi_test
        cae_config)
target_compile_definitions(posix_adapter_mpi_test PUBLIC
        HERMES_MPI_TESTS=true)

add_executable(cae_posix_adapter_mpi_test
        posix_adapter_test.cc
        posix_adapter_basic_test.cc
        posix_adapter_rs_test.cc
        posix_adapter_shared_test.cc)
add_dependencies(cae_posix_adapter_mpi_test
        cae_posix)
target_link_libraries(cae_posix_adapter_mpi_test
        cae_posix Catch2::Catch2 MPI::MPI_C MPI::MPI_CXX)
target_compile_definitions(cae_posix_adapter_mpi_test PUBLIC
        HERMES_INTERCEPT=1 HERMES_MPI_TESTS=true)
jarvis_test(posix test_wrp_cae_cte_posix_basic_mpi_small)
jarvis_test(posix test_wrp_cae_cte_posix_basic_mpi_large)

include_directories(${CMAKE_SOURCE_DIR}/adapter)
add_executable(posix_simple_io_mpi posix_simple_io_mpi.cc)
add_dependencies(posix_simple_io_mpi cae_posix)
target_link_libraries(posix_simple_io_mpi
        cae_posix Catch2::Catch2 MPI::MPI_CXX)

add_executable(cae_posix_simple_io_omp posix_simple_io_omp.cc)
add_dependencies(cae_posix_simple_io_omp cae_posix)
target_link_libraries(cae_posix_simple_io_omp
        cae_posix Catch2::Catch2 OpenMP::OpenMP_CXX)

add_executable(posix_simple_io_omp posix_simple_io_omp.cc)
target_link_libraries(posix_simple_io_omp
        cae_config Catch2::Catch2 OpenMP::OpenMP_CXX)
target_compile_definitions(posix_simple_io_omp PUBLIC
        HERMES_MPI_TESTS=false)

jarvis_test(posix test_wrp_cae_cte_posix_simple_io_omp)

set(POSIX_TESTS
        posix_adapter_test
        cae_posix_adapter_test
        posix_adapter_mpi_test
        cae_posix_adapter_mpi_test
        posix_simple_io_mpi
        cae_posix_simple_io_omp
        posix_simple_io_omp
)

foreach(program ${POSIX_TESTS})
        target_include_directories(${program} PRIVATE ${HERMES_ADAPTER_DIR})
        target_include_directories(${program} PRIVATE ${HERMES_ADAPTER_TEST_DIR})
        target_link_libraries(${program} Catch2::Catch2 -lstdc++fs -lc
                MPI::MPI_CXX)
endforeach()

foreach(program ${POSIX_TESTS})
        install(
                TARGETS
                ${program}
                RUNTIME DESTINATION ${CAE_INSTALL_BIN_DIR}
        )
endforeach()