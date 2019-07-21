# src/testclient/CMakeLists.txt - Dream Test Client Libraries CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

add_executable(testclient
    ${SOURCES}
)

