# src/hmd/CMakeLists.txt - Dream HMD Library CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

file(GLOB OCULUS_SOURCES 
  "oculus/*.cpp"
  "oculus/*.h"
)
source_group("oculus" FILES ${OCULUS_SOURCES})

file(GLOB OPENVR_SOURCES 
  "openvr/*.cpp"
  "openvr/*.h"
)
source_group("openvr" FILES ${OPENVR_SOURCES})

file(GLOB OPENVR_MATRICES_SOURCES
  "${CMAKE_SOURCE_DIR}/src/third_party/Matrices/*.cpp"
  "${CMAKE_SOURCE_DIR}/src/third_party/Matrices/*.h"
)
source_group("openvr\\Matrices" FILES ${OPENVR_MATRICES_SOURCES})

# Oculus
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/OCULUS/v1.12.0/LibOVR/Include)
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/OCULUS/OVRPlatformSDK_v1.24.0/Include)

# OpenVR
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/openvr/headers)

# TODO: This should not be here!!!!
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

add_library(hmd
    ${SOURCES}
    ${OCULUS_SOURCES}
    ${OPENVR_SOURCES}
    ${OPENVR_MATRICES_SOURCES}
)

