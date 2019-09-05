# src/scene/CMakeLists.txt - DreamEnvironmentApp CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(dreamenvironmentapp
    ${SOURCES}
)

