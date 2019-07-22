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

# Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

add_library(hmd
    ${SOURCES}
    ${OCULUS_SOURCES}
    ${OPENVR_SOURCES}
)

