cmake_minimum_required(VERSION 3.16)
project(RollingMedianBenchmark LANGUAGES CXX)

# Use C++20set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory relative to this CMakeLists.txt
include_directories(${CMAKE_SOURCE_DIR}/../include)

# Add the benchmark executable
add_executable(benchmark_matrix benchmark_matrix.cpp)

# Link fmt
find_package(fmt REQUIRED)target_link_libraries(benchmark_matrix PRIVATE fmt::fmt)