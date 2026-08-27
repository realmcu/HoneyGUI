#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required (VERSION 3.13)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS ON) 

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_VERBOSE_MAKEFILE OFF)


project(HoneyGUI)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

execute_process(
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/../tool/convert_dot_config_to.py
            ${CMAKE_SOURCE_DIR}/.config
            ${CMAKE_SOURCE_DIR}/config.cmake
    RESULT_VARIABLE result
    ERROR_VARIABLE error_output
    OUTPUT_VARIABLE output
)

# Check result of the process
if(result)
    message(FATAL_ERROR "Python script failed with error: ${error_output}")
else()
    message(STATUS "Python script output: ${output}")
endif()

# Include the generated config
include(${CMAKE_SOURCE_DIR}/config.cmake)

# Screen size and demo-specific definitions are set in each application's CMakeLists.txt



if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
    message(STATUS "Target processor is ARM.")
    set(GUI_PLATFORM_EMBEDDED ON)
else()
	message(STATUS "Target processor is X86.")
	set(GUI_PLATFORM_SIMULATOR ON)
endif()

set(GUI_BASE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
message("HoneyGUI base path ${GUI_BASE_PATH}")

message(STATUS "GUI_PLATFORM_SIMULATOR: ${GUI_PLATFORM_SIMULATOR}")
message(STATUS "GUI_PLATFORM_EMBEDDED: ${GUI_PLATFORM_EMBEDDED}")

set(GUI_TARGET_NAME gui)

add_executable(${GUI_TARGET_NAME} main.c)

# Define HoneyGUI simulator macro for PC simulation environment
target_compile_definitions(${GUI_TARGET_NAME} PRIVATE _HONEYGUI_SIMULATOR_)

if(WIN32)
    set(SDL2_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../lib/sim/SDL2-2.26.0-STATIC")
    target_include_directories(${GUI_TARGET_NAME} PRIVATE
        ${SDL2_ROOT}/include
        ${SDL2_ROOT}/include/SDL2
    )
else()
    set(SDL2_INCLUDE_DIRS "$ENV{SDL2_INCLUDE_DIR}" CACHE STRING
        "SDL2 include directories")
    set(SDL2_LIBRARY_DIRS "$ENV{SDL2_LIB_DIR}" CACHE STRING
        "SDL2 library directories")
    if(SDL2_INCLUDE_DIRS)
        file(TO_CMAKE_PATH "${SDL2_INCLUDE_DIRS}" SDL2_INCLUDE_DIRS)
    endif()
    if(SDL2_LIBRARY_DIRS)
        file(TO_CMAKE_PATH "${SDL2_LIBRARY_DIRS}" SDL2_LIBRARY_DIRS)
    endif()

    find_path(SDL2_INCLUDE_DIR SDL.h
        HINTS ${SDL2_INCLUDE_DIRS}
        PATH_SUFFIXES SDL2
    )
    find_library(SDL2_LIBRARY SDL2
        HINTS ${SDL2_LIBRARY_DIRS}
    )
    if(NOT SDL2_INCLUDE_DIR OR NOT SDL2_LIBRARY)
        message(FATAL_ERROR
            "SDL2 development files were not found. Install SDL2 development "
            "packages or set SDL2_INCLUDE_DIR and SDL2_LIB_DIR.")
    endif()
    target_include_directories(${GUI_TARGET_NAME} PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIR}
    )
endif()

add_subdirectory(../realgui realgui)
add_subdirectory(../example example)
add_subdirectory(../lib/sim sim)
add_subdirectory(port port)

get_property(GUI_BINARY_RESOURCES GLOBAL PROPERTY GUI_BINARY_RESOURCES)
if(GUI_BINARY_RESOURCES)
    target_link_libraries(${GUI_TARGET_NAME} PRIVATE ${GUI_BINARY_RESOURCES})
endif()


if(WIN32)
    find_library(SDL2_MAIN_LIBRARY SDL2main PATHS ${SDL2_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
    find_library(SDL2_LIBRARY SDL2 PATHS ${SDL2_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
    target_link_libraries(${GUI_TARGET_NAME} PRIVATE
        mingw32
        ${SDL2_MAIN_LIBRARY}
        ${SDL2_LIBRARY}
        setupapi
        imm32
        version
        winmm
        gdi32
        user32
        shell32
        opengl32
        -static-libgcc
        -static-libstdc++
    )
else()
    find_package(OpenGL REQUIRED)
    find_package(Threads REQUIRED)
    target_link_libraries(${GUI_TARGET_NAME} PRIVATE
        ${SDL2_LIBRARY}
        OpenGL::GL
        Threads::Threads
        m
        dl
    )
endif()

target_link_options(${GUI_TARGET_NAME} PRIVATE "-Wl,-Map=${GUI_TARGET_NAME}.map")
if(WIN32)
    set(CUSTOM_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/honeygui_mingw.ld")
else()
    set(CUSTOM_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/honeygui_linux.lds")
    target_link_options(${GUI_TARGET_NAME} PRIVATE "-Wl,-z,noexecstack")
endif()
target_link_options(${GUI_TARGET_NAME} PRIVATE "-T${CUSTOM_LINKER_SCRIPT}")
