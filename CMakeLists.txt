cmake_minimum_required(VERSION 3.23.0)

set(PROJECT ChitChat)
project(ChitChat)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CXX_EXTENSIONS off)
set (CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

include(CMakeConfig)
include(ConanModule)

add_subdirectory(Network)
add_subdirectory(Main)
