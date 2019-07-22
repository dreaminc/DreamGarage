# src/scene/CMakeLists.txt - Dream Cloud Library CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

file(GLOB ENVIRONMENT_SOURCES 
  "Environment/*.cpp"
  "Environment/*.h"
)
source_group("Environment" FILES ${ENVIRONMENT_SOURCES})

file(GLOB HTTP_SOURCES 
  "HTTP/*.cpp"
  "HTTP/*.h"
)
source_group("HTTP" FILES ${HTTP_SOURCES})

file(GLOB MENU_SOURCES 
  "Menu/*.cpp"
  "Menu/*.h"
)
source_group("Menu" FILES ${MENU_SOURCES})

file(GLOB USER_SOURCES 
  "User/*.cpp"
  "User/*.h"
)
source_group("User" FILES ${USER_SOURCES})

file(GLOB WEBRTC_SOURCES 
  "webrtc/*.cpp"
  "webrtc/*.h"
)
source_group("webrtc" FILES ${WEBRTC_SOURCES})

file(GLOB WEBSOCKETS_SOURCES 
  "Websockets/*.cpp"
  "Websockets/*.h"
)
source_group("Websockets" FILES ${WEBSOCKETS_SOURCES})

# Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

add_library(cloud
    ${SOURCES}
    ${ENVIRONMENT_SOURCES}
    ${HTTP_SOURCES}
    ${MENU_SOURCES}
    ${USER_SOURCES}
    ${WEBRTC_SOURCES}
    ${WEBSOCKETS_SOURCES}
)

