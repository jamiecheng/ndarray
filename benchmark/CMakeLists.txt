cmake_minimum_required(VERSION 3.7)
project(array_benchmark)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(lib)

include_directories(lib/include ../include)

set(SOURCE_FILES benchmark.cpp)
add_executable(array_benchmark ${SOURCE_FILES})

target_link_libraries(array_benchmark benchmark)