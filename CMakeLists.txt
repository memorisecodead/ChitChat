cmake_minimum_required(VERSION 3.27.0)

set(PROJECT ChitChat)
project(ChitChat LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CXX_EXTENSIONS off)
set (CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

option(ENABLE_WARNINGS      "Enable to add warnings to a target."   ON)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/Conan)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

include(CMakeConfig)	
include(ConanModule)

if (COVERAGE_MODE)
    include(Coverage)
endif()

enable_testing()

add_subdirectory(Network)
add_subdirectory(Main)
add_subdirectory(Tests)
