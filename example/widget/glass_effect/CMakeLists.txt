#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10)

if(CONFIG_REALTEK_BUILD_EXAMPLE_GLASS_EFFECT_WIDGET)
    target_sources(${GUI_TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/example_glass_effect.c
    )
    target_include_directories(${GUI_TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/root_image
    )
    target_compile_definitions(${GUI_TARGET_NAME} PRIVATE
        DRV_LCD_WIDTH=410
        DRV_LCD_HEIGHT=502
        DRV_PIXEL_BITS=16
        DRV_LCD_CORNER_RADIUS=50
    )

endif()
