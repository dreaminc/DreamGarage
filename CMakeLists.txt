# /CMakeList.txt : CMake project for dos

cmake_minimum_required(VERSION 3.15)

# Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

project(dos LANGUAGES CXX)

include_directories(src)
include_directories(src/third_party/Freetype/freetype-2.8/include)

add_subdirectory(src)

# This is how you set the start up project 
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT testclient)