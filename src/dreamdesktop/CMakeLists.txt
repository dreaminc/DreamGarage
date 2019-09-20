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

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

    # Windows Subsystem vs Console
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

    message("dos/src/dreamdesktop/cmakelists.txt msvc")
    message("flags: ${CMAKE_CXX_FLAGS}")
    message("debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
    message("release flags: ${CMAKE_CXX_FLAGS_RELEASE}")

endif()

add_executable(dreamdesktop
    ${SOURCES}
)

target_link_libraries(dreamdesktop 
    dxgi.lib 
    d3d11.lib
)



