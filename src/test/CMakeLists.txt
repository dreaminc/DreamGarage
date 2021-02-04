# src/test/CMakeLists.txt - Dream Test Libraries CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

file(GLOB TEST_SUITE_SOURCES
  "suites/*.cpp"
  "suites/*.h"
)
source_group("suites" FILES ${TEST_SUITE_SOURCES})

# TODO: This should not be here!!!!!!!!!!
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/webrtc/include/webrtc)

add_library(test_suite
    ${SOURCES}
    ${TEST_SUITE_SOURCES}
)

