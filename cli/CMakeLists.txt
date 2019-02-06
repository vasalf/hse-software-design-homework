cmake_minimum_required(VERSION 3.10)
project(cli)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_library(lcli STATIC
    lib/cli.cpp
    lib/common/ext_char.cpp
    lib/tokenize/tokenize_dfa.cpp
    lib/tokenize/token.cpp
    lib/tokenize/tokenizer.cpp
    lib/environment/environment.cpp
    lib/environment/var_expander.cpp
    lib/parser/command.cpp
    lib/parser/parse.cpp
    lib/common/char_utils.cpp
    lib/executor/executor.cpp
    lib/executor/private/external_executor.cpp
    lib/executor/execute.cpp
    lib/common/istream_wrapper.cpp
    lib/executor/private/builtin_executors.cpp
    lib/common/exit_exception.cpp
        lib/common/pipe.cpp lib/common/pipe.h)
target_link_libraries(lcli LINK_PUBLIC stdc++fs)

add_executable(cli main.cpp)
target_link_libraries(cli LINK_PUBLIC lcli)

set(gtest_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/googletest-release-1.8.1/googletest)
add_subdirectory(${gtest_SOURCE_DIR} ${gtest_SOURCE_DIR}/cmake-build-debug)
include_directories(${gtest_SOURCE_DIR}/include)

include(CTest)
find_package(GTest REQUIRED)
enable_testing()

add_executable(cli_test
    test/tokenizer_test.cpp
    test/environment_test.cpp
    test/var_expander_test.cpp
    test/command_test.cpp
    test/parse_test.cpp
    test/executor_test.cpp
    test/execute_test.cpp
)
target_link_libraries(cli_test LINK_PUBLIC lcli gtest gtest_main)
gtest_add_tests(cli_test "" AUTO)