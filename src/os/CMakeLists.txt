# src/scene/CMakeLists.txt - Dream OS CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.tpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

file(GLOB APP_SOURCES 
  "app/*.cpp"
  "app/*.h"
)
source_group("app" FILES ${MODULE_SOURCES})

file(GLOB MODULE_SOURCES 
  "module/*.cpp"
  "module/*.h"
)
source_group("app" FILES ${MODULE_SOURCES})

# Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

add_library(os
    ${SOURCES}
    ${APP_SOURCES}
    ${MODULE_SOURCES}
)

