cmake_minimum_required(VERSION 3.10)
project(bench)

set(CMAKE_CXX_STANDARD 11)

# Find the Google Benchmark package
find_package(benchmark REQUIRED)

# Add the first executable
add_executable(bench bench.cpp)

# Link libraries for the first executable
target_link_libraries(bench benchmark::benchmark pthread)

