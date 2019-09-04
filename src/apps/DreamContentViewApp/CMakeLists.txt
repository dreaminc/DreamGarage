# src/scene/CMakeLists.txt - DreamContentViewApp CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(dreamcontentviewapp
    ${SOURCES}
)

