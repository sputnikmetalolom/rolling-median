cmake_minimum_required(VERSION 3.20)
project(complex_median)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(complex_median complex_median.cpp)
add_custom_target(run
    COMMAND complex_median
    DEPENDS complex_median
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(complex_median PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include")
find_package(fmt REQUIRED)
target_link_libraries(complex_median PRIVATE fmt::fmt)