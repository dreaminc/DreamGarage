# src/scene/CMakeLists.txt - DreamGamepadCameraApp CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(dreamgamepadcameraapp
    ${SOURCES}
)

