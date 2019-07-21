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

# Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

add_library(test
    ${SOURCES}
    ${TEST_SUITE_SOURCES}
)

