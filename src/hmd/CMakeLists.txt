# src/hmd/CMakeLists.txt - Dream HMD Library CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

# TODO: Might want to split off platform
file(GLOB OCULUS_SOURCES 
  "oculus/*.cpp"
  "oculus/*.h"
  # We need one file from the platform code to not miss a given external symbol 
  "${CMAKE_SOURCE_DIR}/src/third_party/OCULUS/OVRPlatformSDK_v1.24.0/Windows/*.cpp"
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

add_library(hmd
    ${SOURCES}
    ${OCULUS_SOURCES}
    ${OPENVR_SOURCES}
    ${OPENVR_MATRICES_SOURCES}
)

