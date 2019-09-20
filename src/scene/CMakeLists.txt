# src/scene/CMakeLists.txt - Dream Scene Library CMake file

message("dos/src/scene/cmakelists.txt")

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(scene
    ${SOURCES}
)

