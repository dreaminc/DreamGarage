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
source_group("ogl//gl" FILES ${OGL_GL_SOURCES})

# Shaders Folder
file(GLOB OGL_SHADERS_SOURCES 
    "ogl/shaders/*.cpp"
    "ogl/shaders/*.tpp"
    "ogl/shaders/*.h"
)
source_group("ogl//shaders" FILES ${OGL_SHADERS_SOURCES})

# GLSL 
file(GLOB OGL_SHADERS_GLSL 
    "ogl/shaders/glsl/v440/*.shader"
    "ogl/shaders/glsl/v440/*.vert"
    "ogl/shaders/glsl/v440/*.frag"
)
source_group("ogl//shaders//glsl//v440" FILES ${OGL_SHADERS_GLSL})

# Win64 
file(GLOB OGL_WIN64_SOURCES 
    "ogl/win64/*.cpp"
    "ogl/win64/*.tpp"
    "ogl/win64/*.h"
)
source_group("ogl//win64" FILES ${OGL_WIN64_SOURCES})

# TODO: Vulkan

# TODO: DirectX

# TODO: Metal / MoltenVK etc

# Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

add_library(hal
    ${SOURCES}

    # OGL
    ${OGL_SOURCES}
    ${OGL_GL_SOURCES}
    ${OGL_SHADERS_SOURCES}
    ${OGL_WIN64_SOURCES}
    # TODO: Vulkan
    # TODO: DirectX
    # TODO: Metal / MoltenVK etc
)


