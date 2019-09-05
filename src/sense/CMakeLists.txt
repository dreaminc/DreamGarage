# src/scene/CMakeLists.txt - Dream Sense Library CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

file(GLOB WIN64_SOURCES 
  "win64/*.cpp"
  "win64/*.h"
)
source_group("win64" FILES ${WIN64_SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/src/third_party/LeapSDK/include)

# TODO: This should not be here!!
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/webrtc/include/webrtc)

add_library(sense
    ${SOURCES}
    ${WIN64_SOURCES}
)

