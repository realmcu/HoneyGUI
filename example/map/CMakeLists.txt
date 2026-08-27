#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10)

if(CONFIG_REALTEK_BUILD_MAP_NAVIGATION)
    file(GLOB MAP_SOURCES "*.c" "*.cpp")
    target_sources(${GUI_TARGET_NAME} PRIVATE ${MAP_SOURCES})
    target_include_directories(${GUI_TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_subdirectory(core)
    add_subdirectory(graphics)
    add_subdirectory(platform)
    add_subdirectory(render)
    add_subdirectory(state)
endif()
