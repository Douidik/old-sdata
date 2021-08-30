cmake_minimum_required(VERSION 3.20)

project(sdata
  DESCRIPTION "sdata is a marking language primarly used for C++ data structures serialization and deserialization."
  LANGUAGES CXX)

# Project build customization
option(SDATA_BUILD_TESTS "Build sdata test suite" ON)
option(SDATA_BUILD_SHARED_LIBS "Build sdata as shared library" ${BUILD_SHARED_LIBS})

# TEMPORARY, must be done outside CMakeLists
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Filter for source code file names
set(SOURCE_FILE_REGEX "[a-z_]")

add_subdirectory(src/sdata/)

if (${SDATA_BUILD_TESTS})
 project(sdata_test
         DESCRIPTION "sdata test suite"
         LANGUAGES CXX)

 add_subdirectory(test/)
endif()
