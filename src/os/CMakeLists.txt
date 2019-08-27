# src/scene/CMakeLists.txt - Dream OS CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.tpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

file(GLOB APP_SOURCES 
  "app/*.cpp"
  "app/*.h"
)
source_group("app" FILES ${MODULE_SOURCES})

file(GLOB MODULE_SOURCES 
  "module/*.cpp"
  "module/*.h"
)
source_group("app" FILES ${MODULE_SOURCES})

# TODO: This is an architecture bug
# webrtc should only be included in cloud (or it's own module in the future)
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/webrtc/include/webrtc)
add_definitions(-DWEBRTC_WIN)

# Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

# Multithreaded 
set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MTd")

add_library(os
    ${SOURCES}
    ${APP_SOURCES}
    ${MODULE_SOURCES}
)

