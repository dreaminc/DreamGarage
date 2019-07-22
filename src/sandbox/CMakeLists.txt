# src/scene/CMakeLists.txt - Dream Sandbox Library CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

file(GLOB ANDROID_SOURCES 
  "android/*.cpp"
  "android/*.h"
)
source_group("android" FILES ${ANDROID_SOURCES})

file(GLOB IOS_SOURCES 
  "ios/*.cpp"
  "ios/*.h"
)
source_group("ios" FILES ${IOS_SOURCES})

file(GLOB LINUX_SOURCES 
  "linux/*.cpp"
  "linux/*.h"
)
source_group("linux" FILES ${LINUX_SOURCES})

file(GLOB OSX_SOURCES 
  "osx/*.cpp"
  "osx/*.h"
)
source_group("osx" FILES ${OSX_SOURCES})

file(GLOB WIN64_SOURCES 
  "win64/*.cpp"
  "win64/*.h"
)
source_group("win64" FILES ${WIN64_SOURCES})

# Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

add_library(sandbox
    ${SOURCES}
    ${ANDROID_SOURCES}
    ${IOS_SOURCES}
    ${LINUX_SOURCES}
    ${OSX_SOURCES}
    ${WIN64_SOURCES}
)

