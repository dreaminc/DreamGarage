# src/ui/CMakeLists.txt - Dream UI CMake file

message("dos/src/ui/cmakelists.txt")

file(GLOB SOURCES 
  "*.cpp"
  "*.tpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

# TODO: Revisit this
file(GLOB DREAM_CONTROL_VIEW_SOURCES 
  "DreamControlView/*.cpp"
  "DreamControlView/*.tpp"
  "DreamControlView/*.h"
)
source_group("DreamControlView" FILES ${DREAM_CONTROL_VIEW_SOURCES})

# TODO: Revisit this
file(GLOB USER_AREA_CONTROLS_SOURCES 
  "UserAreaControls/*.cpp"
  "UserAreaControls/*.h"
)
source_group("UserAreaControls" FILES ${USER_AREA_CONTROLS_SOURCES})

# TODO: This should not be here!!!
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/webrtc/include/webrtc)

add_library(ui
    ${SOURCES}
    ${DREAM_CONTROL_VIEW_SOURCES}
    ${USER_AREA_CONTROLS_SOURCES}
)

