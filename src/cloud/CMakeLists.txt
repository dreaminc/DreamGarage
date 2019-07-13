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
# TODO: I believe WebRTC Client is deprecated but unclear 
list(FILTER WEBRTC_SOURCES EXCLUDE REGEX ".*WebRTCClient.*$")

source_group("webrtc" FILES ${WEBRTC_SOURCES})


file(GLOB WEBSOCKETS_SOURCES 
  "Websockets/*.cpp"
  "Websockets/*.h"
)
source_group("Websockets" FILES ${WEBSOCKETS_SOURCES})

# Header Only Deps
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/websocketpp)
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/asio/asio-1.11.0/include)

# Dependencies

# OpenSSL
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/include)

# Lib CURL
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/libcurl/curl-7.49.1/include)

# WebRTC
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/webrtc/include/jsoncpp)
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

add_library(cloud
    ${SOURCES}
    ${ENVIRONMENT_SOURCES}
    ${HTTP_SOURCES}
    ${MENU_SOURCES}
    ${USER_SOURCES}
    ${WEBRTC_SOURCES}
    ${WEBSOCKETS_SOURCES}
)

