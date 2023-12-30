cmake_minimum_required(VERSION 3.10)
project(iperf-3.14)

set(CMAKE_CXX_STANDARD 11)

# Find the Google Benchmark package
find_package(benchmark REQUIRED)

# Add the first executable
add_executable(iperfbench iperfbench.cpp)

# Link libraries for the first executable
target_link_libraries(iperfbench benchmark::benchmark pthread)
