# src/scene/CMakeLists.txt - DreamPeerApp CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(dreampeerapp
    ${SOURCES}
)

