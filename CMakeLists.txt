cmake_minimum_required(VERSION 3.27.0)

set(PROJECT ChitChat)
project(ChitChat LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CXX_EXTENSIONS off)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_BINARY_DIR}/cmake)

include(CMakeConfig)
include(ConanModule)

add_subdirectory(Network)
add_subdirectory(Main)
add_subdirectory(Tests)
