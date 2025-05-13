cmake_minimum_required(VERSION 3.25)
project(cae)
set(CMAKE_CXX_STANDARD 17)

enable_testing()

if(HERMES)
find_package(Hermes REQUIRED)
message("Found hermes at ${HERMES_LIB_DIR}")

add_executable(f posix_api.cc)
target_link_libraries(f MPI::MPI_CXX stdc++fs hermes::hermes_core_client)

add_executable(b b.cc)
target_link_libraries(b hermes::hermes_core_client)
endif()

