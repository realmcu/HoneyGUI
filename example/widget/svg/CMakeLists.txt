#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10)
if(CONFIG_REALTEK_BUILD_EXAMPLE_SVG_WIDGET)
    file(GLOB SOURCES "*.c" "*.cpp")
    file(GLOB HEADERS "*.h" "*.hpp")

    if(SOURCES)
        target_sources(${GUI_TARGET_NAME} PRIVATE ${SOURCES})
        target_include_directories(${GUI_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
        target_compile_definitions(${GUI_TARGET_NAME} PRIVATE
            DRV_LCD_WIDTH=480
            DRV_LCD_HEIGHT=480
            DRV_PIXEL_BITS=16
        )
    endif()
endif()
