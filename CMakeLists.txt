cmake_minimum_required(VERSION 3.10)

message("Looking for vcpkg.")
if(NOT DEFINED ENV{VCPKG_ROOT})
  message("Not using vcpkg to manage dependencies.")
endif()

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "")
    message(" [INFO] VCPKG CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")
endif()

project(vol)

find_package(range-v3 CONFIG REQUIRED)

set(APPLE_CPP_ROOT /Library/Developer/CommandLineTools)
set(CATCH_BUILD_TESTING ON)

add_compile_options(-std=c++17)

subdirs(src/main src/test)

