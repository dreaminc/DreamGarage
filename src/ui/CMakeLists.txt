# src/scene/CMakeLists.txt - Dream UI CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.tpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

# TODO: Revisit this
file(GLOB DREAM_CONTROL_VIEW_SOURCES 
  "DreamControlView/*.cpp"
  "DreamControlView/*.tpp"
  "DreamControlView/*.h"
)
source_group("" FILES ${DREAM_CONTROL_VIEW_SOURCES})

# TODO: This should not be here!!!
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/webrtc/include/webrtc)
add_definitions(-DWEBRTC_WIN)

# DREAM_CONTROL_VIEW_Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

# Multithreaded 
set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MTd")

add_library(ui
    ${SOURCES}
    ${DREAM_CONTROL_VIEW_SOURCES}
)

