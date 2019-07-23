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

# Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

add_executable(dreamdesktop
    ${SOURCES}
)

