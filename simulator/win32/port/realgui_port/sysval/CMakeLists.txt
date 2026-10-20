#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10)

file(GLOB SOURCES "*.c" "*.cpp")

if(SOURCES)
    target_sources(${GUI_TARGET_NAME} PRIVATE ${SOURCES})
endif()

target_include_directories(${GUI_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
