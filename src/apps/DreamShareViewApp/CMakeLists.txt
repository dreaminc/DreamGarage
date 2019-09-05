# src/scene/CMakeLists.txt - DreamShareViewApp CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(dreamshareviewapp
    ${SOURCES}
)

