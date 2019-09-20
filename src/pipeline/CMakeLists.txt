# src/pipeline/CMakeLists.txt - Dream Pipeline Library CMake file

message("dos/src/pipeline/cmakelists.txt")

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(pipeline
    ${SOURCES}
)

