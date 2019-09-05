# src/scene/CMakeLists.txt - DreamLoginApp CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(dreamloginapp
    ${SOURCES}
)

