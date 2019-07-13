# src/scene/CMakeLists.txt - Dream App Library CMake file

# TODO: This should not be here!!!!!!!
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/webrtc/include/webrtc)
add_definitions(-DWEBRTC_WIN)

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

