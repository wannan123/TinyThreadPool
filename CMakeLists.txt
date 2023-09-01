cmake_minimum_required(VERSION 3.0)
project(ThreadPool)
include_directories(${CMAKE_SOURCE_DIR}/include)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
SET(CMAKE_BUILD_TYPE Debug)
add_executable(test main.cpp src/ThreadPool.cpp)

