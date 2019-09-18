# src/scene/CMakeLists.txt - Dream HAL CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.tpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

# OpenGL

# TL
file(GLOB OGL_SOURCES 
    "ogl/*.cpp"
    "olg/*.tpp"
    "ogl/*.h"
)
source_group("ogl" FILES ${OGL_SOURCES})

# GL Folder
file(GLOB OGL_GL_SOURCES 
    "ogl/gl/*.cpp"
    "olg/gl/*.tpp"
    "ogl/gl/*.h"
)
source_group("ogl\\gl" FILES ${OGL_GL_SOURCES})

# Shaders Folder
file(GLOB OGL_SHADERS_SOURCES 
    "ogl/shaders/*.cpp"
    "ogl/shaders/*.tpp"
    "ogl/shaders/*.h"
)
source_group("ogl\\shaders" FILES ${OGL_SHADERS_SOURCES})

# GLSL 
file(GLOB OGL_SHADERS_GLSL 
    "ogl/shaders/glsl/v440/*.shader"
    "ogl/shaders/glsl/v440/*.vert"
    "ogl/shaders/glsl/v440/*.frag"
)
source_group("ogl\\shaders\\glsl\\v440" FILES ${OGL_SHADERS_GLSL})
set_source_files_properties(${OGL_SHADERS_GLSL} PROPERTIES HEADER_FILE_ONLY TRUE)

# Win64 
file(GLOB OGL_WIN64_SOURCES 
    "ogl/win64/*.cpp"
    "ogl/win64/*.tpp"
    "ogl/win64/*.h"
)
source_group("ogl\\win64" FILES ${OGL_WIN64_SOURCES})

# Vulkan
file(GLOB VULKAN_SOURCES 
    "vulkan/*.cpp"
    "vulkan/*.tpp"
    "vulkan/*.h"
)
source_group("vulkan" FILES ${VULKAN_SOURCES})

# DirectX 11
#file(GLOB OGL_SOURCES 
#    "ogl/*.cpp"
#    "olg/*.tpp"
#    "ogl/*.h"
#)
#source_group("ogl" FILES ${OGL_SOURCES})

# Easy Logging
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/easyloggingpp/include)

# TODO: Vulkan

# TODO: DirectX

# TODO: Metal / MoltenVK etc

# TODO: This should not be here!!!!!
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/webrtc/include/webrtc)

add_library(hal
    ${SOURCES}

    # OGL
    ${OGL_SOURCES}
    ${OGL_GL_SOURCES}
    ${OGL_SHADERS_SOURCES}
    ${OGL_WIN64_SOURCES}
    
    # Vulkan
    ${VULKAN_SOURCES}

    # TODO: DirectX
    # TODO: Metal / MoltenVK etc

    # GLSL Shaders
    ${OGL_SHADERS_GLSL}
)


