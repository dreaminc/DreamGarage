# src/scene/CMakeLists.txt - Dream Garage CMake file
# TODO: What is the differentiation between Garage and OS
# TODO: Renaming for Garage (the collab app vs client etc)

file(GLOB SOURCES 
  "*.cpp"
  "*.tpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

# Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

add_library(garage
    ${SOURCES}
)

