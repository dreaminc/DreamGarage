# src/scene/CMakeLists.txt - Dream Desktop CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.tpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

file(GLOB HLSL_SOURCES
    "*.hlsl"
)
source_group("" HLSL_SOURCES)

# Windows Subsystem vs Console
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

add_executable(dreamdesktop
    ${SOURCES}
)

target_link_libraries(dreamdesktop 
    dxgi.lib 
    d3d11.lib
)



