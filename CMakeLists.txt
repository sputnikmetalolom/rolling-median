cmake_minimum_required(VERSION 3.20)
project(rolling-median VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Werror=return-type -Wsign-conversion -Werror=switch")
set(CMAKE_CXX_STANDARD 20)

enable_testing()

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download)
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download)
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
  ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
  EXCLUDE_FROM_ALL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(rolling-median INTERFACE)
target_include_directories(rolling-median INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include
  $<INSTALL_INTERFACE:include/rolling-median>
)

add_subdirectory(example)
add_subdirectory(test)

install(TARGETS rolling-median EXPORT rolling-medianConfig)
install(EXPORT rolling-medianConfig DESTINATION lib/cmake/rolling-median)
install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/rolling_median.h
  DESTINATION include/rolling-median
)

set(CPACK_PACKAGE_NAME "librolling-median-dev")
set(CPACK_PACKAGE_VENDOR "Kallkod Oy")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Rolling median calculator")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Viacheslav Sobolev <vskod@kallkod.fi>")
set(CPACK_SOURCE_GENERATOR "TGZ")

include(CPack)
