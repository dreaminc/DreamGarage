# src/scene/CMakeLists.txt - DreamUserApp CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(dreamuserapp
    ${SOURCES}
)

