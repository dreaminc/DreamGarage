# src/scene/CMakeLists.txt - DreamTestingApp CMake file

# This is a test / example / template app

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(dreamtestingapp
    ${SOURCES}
)

