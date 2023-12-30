# Specify the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(BENCH)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)

# Add executable target with source file
add_executable(BENCH BENCH.cpp)

target_link_libraries(BENCH pqxx pq)

