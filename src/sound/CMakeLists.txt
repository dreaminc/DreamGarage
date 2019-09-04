# src/scene/CMakeLists.txt - Dream Sound Library CMake file

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

file(GLOB WIN64_WASAPI_SOURCES 
  "win64/wasapi/*.cpp"
  "win64/wasapi/*.h"
)
source_group("win64\\wasapi" FILES ${WIN64_WASAPI_SOURCES})

file(GLOB WIN64_XAUDIO2_SOURCES 
  "win64/xaudio2/*.cpp"
  "win64/xaudio2/*.h"
)
source_group("win64\\xaudio2" FILES ${WIN64_XAUDIO2_SOURCES})

# TODO: This should not be here!!
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/webrtc/include/webrtc)

add_library(sound
    ${SOURCES}
    ${WIN64_SOURCES}
    ${WIN64_WASAPI_SOURCES}
    ${WIN64_XAUDIO2_SOURCES}
)

