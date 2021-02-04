# src/scene/CMakeLists.txt - DreamContentViewApp CMake file

file(GLOB ANIMATION_ENGINE_SOURCES 
  "AnimationEngine/*.cpp"
  "AnimationEngine*.h"
)
source_group("AnimationEngine" FILES ${ANIMATION_ENGINE_SOURCES})

file(GLOB DREAM_SOUND_SYSTEM_SOURCES 
  "DreamSoundSystem/*.cpp"
  "DreamSoundSystem/*.h"
)
source_group("DreamSoundSystem" FILES ${DREAM_SOUND_SYSTEM_SOURCES})

file(GLOB INTERACTION_ENGINE_SOURCES 
  "InteractionEngine/*.cpp"
  "InteractionEngine/*.h"
)
source_group("InteractionEngine" FILES ${INTERACTION_ENGINE_SOURCES})

file(GLOB PHYSICS_ENGINE_SOURCES 
  "PhysicsEngine/*.cpp"
  "PhysicsEngine/*.h"
)
source_group("PhysicsEngine" FILES ${PHYSICS_ENGINE_SOURCES})

file(GLOB TIME_MANAGER_SOURCES 
  "TimeManager/*.cpp"
  "TimeManager/*.h"
)
source_group("TimeManager" FILES ${TIME_MANAGER_SOURCES})

# TODO: This should not be here!!!!!
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/webrtc/include/webrtc)

IF(ANDROID)
    
    add_library(modules ${PHYSICS_ENGINE_SOURCES})

ELSEIF(WIN32)
    add_library(modules
        ${ANIMATION_ENGINE_SOURCES}
        ${DREAM_SOUND_SYSTEM_SOURCES}
        ${INTERACTION_ENGINE_SOURCES}
        ${PHYSICS_ENGINE_SOURCES}
        ${TIME_MANAGER_SOURCES}
    )
ENDIF()



