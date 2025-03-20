cmake_minimum_required(VERSION 3.25)
project(cae)
set(CMAKE_CXX_STANDARD 17)
find_package(Hermes REQUIRED)
message("Found hermes at ${HERMES_LIB_DIR}")

add_executable(cae posix_api.cc)
target_link_libraries(cae stdc++fs hermes::core_client)
