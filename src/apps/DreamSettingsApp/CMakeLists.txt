# src/scene/CMakeLists.txt - DreamSettingsApp CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(dreamsettingsapp
    ${SOURCES}
)

