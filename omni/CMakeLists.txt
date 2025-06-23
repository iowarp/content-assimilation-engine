cmake_minimum_required(VERSION 3.25)
project(OMNI)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

find_package(yaml-cpp)

include_directories(
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
	${YAML_CPP_INCLUDE_DIR}
        .
)

option(AWS "Enable AWS SDK support" OFF)
if(AWS)
  find_package(AWSSDK REQUIRED COMPONENTS core s3)
  add_definitions(-DUSE_AWS)    
  include_directories(${AWSSDK_INCLUDE_DIR})
endif()

option(POCO "Enable POCO support" OFF)
if(POCO)
add_definitions(-DUSE_POCO)
find_package(Poco REQUIRED COMPONENTS Crypto Foundation)
# find_package(OpenSSL REQUIRED COMPONENTS Crypto QUIET)
endif()

option(HERMES "Enable HERMES support" OFF)
if(HERMES)
add_definitions(-DUSE_HERMES)
find_package(Hermes)

find_package(cereal)
message(STATUS "found cereal at ${cereal_DIR}")
set(CDIR  ${cereal_DIR}/../../../include)
message(${CDIR})

find_package(Thallium REQUIRED)
set(TDIR  ${Thallium_DIR}/../../../include)
message(${TDIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules(Margo REQUIRED margo)
message(STATUS "Margo_INCLUDE_DIRS: ${Margo_INCLUDE_DIRS}")

include_directories(
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        ${HERMES_SRC_DIR}
        ${HERMES_ADAPTER_DIR}
        ${HERMES_IO_CLIENT_DIR}
        ${TDIR}
        ${Margo_INCLUDE_DIRS}
        ${CDIR}
	${YAML_CPP_INCLUDE_DIR}
        .
)
endif()



add_executable(wrp wrp.cc)

target_link_libraries(wrp yaml-cpp)

if(HERMES)
  target_link_libraries(wrp hermes::hermes_core_client)
endif()

if(AWS)
#   target_link_libraries(wrp aws-cpp-sdk-s3)
  target_link_libraries(wrp ${AWSSDK_LINK_LIBRARIES})
endif()


if(POCO)
  target_link_libraries(wrp Poco::Crypto Poco::Foundation)
endif()


if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(wrp PRIVATE -coverage)
  target_link_options(wrp PRIVATE -coverage)
endif()  


add_test(NAME put COMMAND wrp put ../test/posix.yml)

add_test(NAME get COMMAND wrp get cae)

add_test(NAME ls COMMAND wrp ls)

add_test(NAME invalid COMMAND wrp invalid)
set_tests_properties(invalid
    PROPERTIES
    WILL_FAIL "TRUE"
    RETURN_VALUE 1
)

add_test(NAME no_cmd COMMAND wrp)
set_tests_properties(no_cmd
    PROPERTIES
    WILL_FAIL "TRUE"
    RETURN_VALUE 1
)

add_test(NAME put_only COMMAND wrp put)
set_tests_properties(put_only
    PROPERTIES
    WILL_FAIL "TRUE"
    RETURN_VALUE 1
)

add_test(NAME get_only COMMAND wrp get)
set_tests_properties(get_only
    PROPERTIES
    WILL_FAIL "TRUE"
    RETURN_VALUE 1
)

add_test(NAME not_found COMMAND wrp put not_found)
set_tests_properties(not_found
    PROPERTIES
    WILL_FAIL "TRUE"
    RETURN_VALUE 1
)

add_test(NAME nested COMMAND wrp put ../test/nested.yml)

add_test(NAME empty COMMAND wrp put ../test/empty.yml)

add_test(NAME root COMMAND wrp put ../test/root.yml)

add_test(NAME scalar COMMAND wrp put ../test/scalar.yml)

add_test(NAME invalid_yml COMMAND wrp put ../test/invalid.yml)
set_tests_properties(invalid_yml
    PROPERTIES
    WILL_FAIL "TRUE"
    RETURN_VALUE 1
)

add_test(NAME path COMMAND wrp put ../test/path.yml)
set_tests_properties(path
    PROPERTIES
    WILL_FAIL "TRUE"
    RETURN_VALUE 1
)
add_test(NAME root_seq COMMAND wrp put ../test/root_seq.yml)

add_test(NAME lseek COMMAND wrp put ../test/lseek.yml)
set_tests_properties(lseek
    PROPERTIES
    WILL_FAIL "TRUE"
    RETURN_VALUE 1
)

install(TARGETS wrp DESTINATION bin)
