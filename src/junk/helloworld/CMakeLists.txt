# src/junk/helloworld/CMakeLists.txt - Hello world multiplatform project

message("flags: ${CMAKE_CXX_FLAGS}")
message("debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message("release flags: ${CMAKE_CXX_FLAGS_RELEASE}")

file(GLOB SOURCES
        "*.cpp"
        "*.h"
        )
source_group("" FILES ${SOURCES})

add_executable(helloworld
        ${SOURCES}
        )
