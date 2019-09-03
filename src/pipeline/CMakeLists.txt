# src/scene/CMakeLists.txt - Dream Pipeline Library CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(pipeline
    ${SOURCES}
)

