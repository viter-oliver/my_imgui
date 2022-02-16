# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name

project(afg_anroid)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_subdirectory(json_lib)

#add_subdirectory(freetype)
add_subdirectory(imgui)
#add_subdirectory(graphics_app)
