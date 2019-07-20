# src/core/CmakeLists.txt - Core DOS CMakeLists 

file(GLOB CORE_SOURCES 
  "*.cpp"
  "*.h"
)

file(GLOB BOUNDING_SOURCES 
  "bounding/*.cpp"
  "bounding/*.h"
)

add_library(core ${CORE_SOURCES} ${BOUNDING_SOURCES})

source_group("core" FILES ${CORE_SOURCES})
source_group("bounding" FILES ${BOUNDING_SOURCES})