cmake_minimum_required(VERSION 3.7)
project(array)

set(CMAKE_CXX_STANDARD 11)

include_directories(../include)

set(SOURCE_FILES main.cpp)
add_executable(array ${SOURCE_FILES})