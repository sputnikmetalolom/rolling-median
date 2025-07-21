cmake_minimum_required(VERSION 3.20)
project(rolling-median VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Werror=return-type -Wsign-conversion -Werror=switch")

# Define real library target
add_library(rolling_median INTERFACE)

# Define namespaced alias for consumers
add_library(kallkod::rolling_median ALIAS rolling_median)

# Set include directories on the real target
target_include_directories(rolling_median INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/rolling-median>
)

# Optional examples and tests
option(ROLLING_MEDIAN_BUILD_EXAMPLES "Build example programs" OFF)
option(ROLLING_MEDIAN_BUILD_TESTS "Build unit tests" OFF)

if (ROLLING_MEDIAN_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if (ROLLING_MEDIAN_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# Installation
install(TARGETS rolling_median EXPORT rolling-medianConfig)
install(EXPORT rolling-medianConfig DESTINATION lib/cmake/rolling-median)
install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/rolling_median.h
  DESTINATION include/rolling-median
)

# Packaging
set(CPACK_PACKAGE_NAME "librolling-median-dev")
set(CPACK_PACKAGE_VENDOR "Kallkod Oy")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Rolling median calculator")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Viacheslav Sobolev <vskod@kallkod.fi>")
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)