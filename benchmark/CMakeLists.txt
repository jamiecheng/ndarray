cmake_minimum_required(VERSION 3.7)
project(array_benchmark)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-O3")

add_subdirectory(lib)

include_directories(lib/include
        ../include
        third_party/my_old_lib)

set(SOURCE_FILES src/benchmark.cpp)
add_executable(array_benchmark ${SOURCE_FILES})

target_link_libraries(array_benchmark benchmark)