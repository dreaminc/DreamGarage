# src/scene/CMakeLists.txt - Dream2DMouseApp CMake file

message("dos/src/apps/dream2dmouseapp/cmakelists.txt")

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(dream2dmouseapp
    ${SOURCES}
)

