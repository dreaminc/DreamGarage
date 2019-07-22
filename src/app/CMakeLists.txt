# src/scene/CMakeLists.txt - Dream App Library CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_subdirectory(Dream2DMouseApp)
add_subdirectory(DreamBrowserApp)
add_subdirectory(DreamContentViewApp)
add_subdirectory(DreamDesktopDuplicationApp)
add_subdirectory(DreamEnvironmentApp)
add_subdirectory(DreamFormApp)
add_subdirectory(DreamGamepadCameraApp)
add_subdirectory(DreamLoginApp)
add_subdirectory(DreamPeerApp)
add_subdirectory(DreamSettingsApp)
add_subdirectory(DreamShareViewApp)
add_subdirectory(DreamTestingApp)
add_subdirectory(DreamUIBarApp)
add_subdirectory(DreamUserApp)
add_subdirectory(DreamUserControlAreaApp)
add_subdirectory(DreamVCamApp)

# Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

add_library(app
    ${SOURCES}
)

