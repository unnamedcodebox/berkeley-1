cmake_minimum_required(VERSION 3.10)
project(berkeley)

include(gtest.cmake)

set(CMAKE_CXX_STANDARD 14)
set(WARNING_FLAGS "-Wall -Wextra -Werror -Woverloaded-virtual -Winit-self -Wno-unused-parameter -Wno-deprecated-declarations")

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

message ("Compile flags: ${CMAKE_CXX_FLAGS}")

add_subdirectory(halifax)
add_subdirectory(server)
add_subdirectory(client)

