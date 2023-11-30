[![Clang Linux Build](https://github.com/memorisecodead/ChitChat/actions/workflows/Clang%20Linux%20Build.yml/badge.svg)](https://github.com/memorisecodead/ChitChat/actions/workflows/Clang%20Linux%20Build.yml)
[![Windows Build](https://github.com/memorisecodead/ChitChat/actions/workflows/Windows%20Build.yml/badge.svg)](https://github.com/memorisecodead/ChitChat/actions/workflows/Windows%20Build.yml)
[![GCC Linux Build](https://github.com/memorisecodead/ChitChat/actions/workflows/GCC%20Linux%20Build.yml/badge.svg)](https://github.com/memorisecodead/ChitChat/actions/workflows/GCC%20Linux%20Build.yml)

![](./ChitChat_logo.png)

A simple and efficient C++ implementation of a messenger using the popular boost::beast and boost::asio libraries for networking.

--- 

## Getting started

[Conan](https://conan.io/index.html)

This is an open source, decentralized and multi-platform package manager for C/C++.

 Install Python: 

 - Windows: [Python 3.12.0.exe](https://www.python.org/ftp/python/3.12.0/)

 - Linux: 
```$ sudo apt-get update ```

```$ sudo apt-get install python3.12```

Install Conan: 

 - Windows/Linux: ``` $ pip install conan==1.61.0```
 - Windows/Linux: [conan-source-execution files](https://github.com/conan-io/conan/releases/tag/1.61.0)

[CMake](https://cmake.org)

CMake is a cross-platform, open-source build system generator. 

Install CMake:

 - Windows: [cmake-3.27.0-win64-x64.msi](https://github.com/Kitware/CMake/releases/download/v3.27.0-rc1/cmake-3.27.0-rc1-windows-x86_64.msi)


 - Linux: [cmake-3.27.0-Linux-x86_64.sh](https://github.com/Kitware/CMake/releases/download/v3.27.0-rc1/cmake-3.27.0-rc1-linux-x86_64.sh)

## Build

```cmake .. -DCMAKE_BUILD_TYPE=[BUILD_TYPE]```
