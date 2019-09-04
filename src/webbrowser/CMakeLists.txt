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

add_library(webbrowser
    ${SOURCES}
    ${CEF_SOURCES}
)

