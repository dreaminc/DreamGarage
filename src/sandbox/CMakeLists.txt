# src/sandbox/CMakeLists.txt - Dream Sandbox Library CMake file

message("dos/src/sandbox/cmakelists.txt")

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

# TODO: This should not be here
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/webrtc/include/webrtc)

# TODO: This should not be here (only in HMD yo)
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/openvr/headers)

IF(APPLE
   # Mac OS X specific code
   # TODO: Add OSX vs. iOS
   #add_library(sandbox
   # ${SOURCES}
   # ${OSX_SOURCES}
   #)
#
   #add_library(sandbox
   # ${SOURCES}
   # ${IOS_SOURCES}
   #)
)
ELSEIF(UNIX AND NOT APPLE)
  #add_library(sandbox
  #  ${SOURCES}
  #  ${LINUX_SOURCES}
  #)
ELSEIF(WIN32)
   add_library(sandbox
    ${SOURCES}
    ${WIN64_SOURCES}
   )
ENDIF()

