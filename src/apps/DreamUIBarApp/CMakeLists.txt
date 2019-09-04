# src/scene/CMakeLists.txt - DreamUIBarApp CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(dreamuibarapp
    ${SOURCES}
)

