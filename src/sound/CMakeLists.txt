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

# Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

add_library(sound
    ${SOURCES}
    ${WIN64_SOURCES}
    ${WIN64_WASAPI_SOURCES}
    ${WIN64_XAUDIO2_SOURCES}
)

