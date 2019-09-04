# src/scene/CMakeLists.txt - DreamVCamApp CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(dreamvcamapp
    ${SOURCES}
)

