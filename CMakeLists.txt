# /CMakeList.txt : CMake project for dos

cmake_minimum_required(VERSION 3.15)

project(dos LANGUAGES CXX)

include_directories(src)

add_subdirectory(src)

# This is how you set the start up project 
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT testclient)

