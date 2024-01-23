cmake_minimum_required (VERSION 3.5)

#include cmake module 
#include(./cmake/build.cmake)
#include(./cmake/compile.cmake)

#for window mingw32 makefile: avoid make error
#set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
#set(CMAKE_SYSTEM_NAME Generic)

#how to set project name
project(HoneyGUI)

message("#### current cmake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

set(GUI_BASE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
message("HoneyGUI base path ${GUI_BASE_PATH}")


add_subdirectory(../ HoneyGUI.out)


add_executable(gui main.c)

target_sources(gui PRIVATE ${gui_sources})

target_include_directories(gui PUBLIC ${gui_headers})