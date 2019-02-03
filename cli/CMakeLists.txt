cmake_minimum_required(VERSION 3.10)
project(cli)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_library(lcli STATIC
        lib/cli.cpp)

add_executable(cli main.cpp)
target_link_libraries(cli LINK_PUBLIC lcli)

set(gtest_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/googletest-release-1.8.1/googletest)
add_subdirectory(${gtest_SOURCE_DIR} ${gtest_SOURCE_DIR}/cmake-build-debug)
include_directories(${gtest_SOURCE_DIR}/include)

include(CTest)
find_package(GTest REQUIRED)
enable_testing()

add_executable(cli_test
        test/sample_test.cpp)
target_link_libraries(cli_test LINK_PUBLIC lcli gtest gtest_main)
gtest_add_tests(cli_test "" AUTO)