# /CMakeList.txt : CMake project for dos

cmake_minimum_required(VERSION 3.14)

set(DREAM_VERSION 1.1.0)

project(dos LANGUAGES CXX)

include_directories(src)

# Third Party Includes
# TODO: Switch over to pull code etc + build system for cross platform
include_directories(src/third_party/Freetype/freetype-2.8/include)
include_directories(src/third_party/assimp/3.3.1/include)
include_directories(src/third_party/Freeimage/include)
include_directories(src/third_party/CEF/CEF_BINARY_3538)

# Header Only Includes
include_directories(src/third_party/json/include)

add_definitions(-DUNICODE)

configure_file(${CMAKE_SOURCE_DIR}/src/core/config.h.in ${CMAKE_SOURCE_DIR}/src/core/config.h)

add_subdirectory(src)

# This is how you set the start up project 
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT testclient)
# set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT cloud)