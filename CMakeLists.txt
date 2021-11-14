cmake_minimum_required(VERSION 3.20)

# TEMP must be done outside
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(sdata
  DESCRIPTION "sdata is a marking language used for C++ data structures serialization and deserialization"
  LANGUAGES CXX)

# Project build customization
option(SDATA_BUILD_TESTS "build sdata's test suite ?" ON)

# Source code regex filter
set(SOURCE_FILE_REGEX "[a-z_]")

add_subdirectory(src/sdata/)

if(${SDATA_BUILD_TESTS})
  project(sdata_test
    DESCRIPTION "sdata test suite"
    LANGUAGES CXX)

  add_subdirectory(test/)
endif()
