cmake_minimum_required(VERSION 2.8)
project(SANDBOX)

if (NOT SANDBOX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

include_directories(src)

add_executable(radix src/radix.cpp)
