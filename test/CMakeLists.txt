cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")

add_executable(test_rolling_median test_rolling_median.cpp)
target_link_libraries(test_rolling_median gtest_main gmock)
add_test(NAME test_rolling_median COMMAND test_rolling_median)
