enable_testing()

file(GLOB_RECURSE SDATA_TEST_SRC ${SOURCE_FILE_REGEX}*.hpp ${SOURCE_FILE_REGEX}*.cpp)
add_executable(sdata_test ${SDATA_TEST_SRC})

# Catch2 setup
include(FetchContent)

fetchcontent_declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.0.0-preview3
)

fetchcontent_makeavailable(Catch2)

# Setup dependencies
target_link_libraries(sdata_test PRIVATE sdata Catch2::Catch2WithMain)
target_include_directories(sdata_test PRIVATE ${CMAKE_SOURCE_DIR}/src/)

set_target_properties( sdata_test PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/

  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
  LINKER_LANGUAGE CXX)
