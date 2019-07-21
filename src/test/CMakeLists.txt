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

source_group("suites" FILES 
    ${TEST_SUITE_SOURCES}
)

add_library(test
    ${SOURCES}
    ${TEST_SUITE_SOURCES}
)

