# src/client/CMakeLists.txt - Dream Client CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_executable(dreamclient
    ${SOURCES}
)

