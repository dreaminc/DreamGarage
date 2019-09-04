# src/scene/CMakeLists.txt - Dream2DMouseApp CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(dream2dmouseapp
    ${SOURCES}
)

