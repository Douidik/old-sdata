file(GLOB_RECURSE SDATA_SRC ${SOURCE_FILE_REGEX}*.hpp ${SOURCE_FILE_REGEX}*.cpp)
add_library(sdata STATIC ${SDATA_SRC})

# Include sdata's source root directory
target_include_directories(sdata PUBLIC ./)

set_target_properties(sdata PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/

  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
  LINKER_LANGUAGE CXX)
