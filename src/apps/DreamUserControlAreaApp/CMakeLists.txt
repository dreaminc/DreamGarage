# src/scene/CMakeLists.txt - DreamUserControlAreaApp CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(dreamusercontrolareaapp
    ${SOURCES}
)

