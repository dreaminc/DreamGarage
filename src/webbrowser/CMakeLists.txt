# src/scene/CMakeLists.txt - Dream WebBrowser CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.tpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

# TODO: Revisit this
file(GLOB CEF_SOURCES 
  "cefbrowser/*.cpp"
  "cefbrowser/*.tpp"
  "cefbrowser/*.h"
)
source_group("cefbrowser" FILES ${CEF_SOURCES})

# DREAM_CONTROL_VIEW_Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

add_library(webbrowser
    ${SOURCES}
    ${CEF_SOURCES}
)

