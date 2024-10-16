cmake_minimum_required(VERSION 3.10)

project(SkipList VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(
    SkipList
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)

target_include_directories(
    SkipList PRIVATE 
    "${PROJECT_SOURCE_DIR}/include"
)