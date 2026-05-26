#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10)

if(1) #set to 1 to enable or variable to for dependency

    # Glob only *.c -- this naturally excludes msvideo1.cpp (ScummVM reference, must NOT be compiled).
    # file(GLOB) always returns absolute paths, avoiding CMP0076 relative-path issues.
    file(GLOB SOURCES "*.c")
    file(GLOB HEADERS "*.h")

    if(SOURCES AND HEADERS)
        target_sources(${GUI_TARGET_NAME} PRIVATE ${SOURCES})
        target_include_directories(${GUI_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    elseif(HEADERS)
        target_include_directories(${GUI_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

endif()
