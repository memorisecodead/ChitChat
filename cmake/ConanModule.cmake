if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.0/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(REQUIRES boost/1.81.0
						 catch2/3.4.0
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/Conan)

find_package(Boost REQUIRED)
find_package(Catch2 REQUIRED)
