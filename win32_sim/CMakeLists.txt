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


add_subdirectory(../gui_engine gui_engine.out)