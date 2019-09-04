# src/scene/CMakeLists.txt - DreamFormApp CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(dreamformapp
    ${SOURCES}
)

