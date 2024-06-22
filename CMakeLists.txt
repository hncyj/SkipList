cmake_minimum_required(VERSION 3.10)

project(SkipList VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(SkipList ${SRC_FILES})

# set head files path
target_include_directories(SkipList PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)


# Optionally, create a separate executable for the tests
add_executable(StressTest test/stress_test.cpp)
target_include_directories(StressTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)