cmake_minimum_required(VERSION 3.8)
project(unit_tests)

set(CMAKE_CXX_STANDARD 14)

include_directories(../../include)

set(SOURCE_FILES test_cases.cpp)

add_executable(unit_tests ${SOURCE_FILES})

target_link_libraries(unit_tests gtest gtest_main)