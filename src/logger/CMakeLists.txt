# src/scene/CMakeLists.txt - Dream Logger CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.tpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_library(logger
    ${SOURCES}
)

