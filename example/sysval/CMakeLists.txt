#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10)

set(SYSVAL_EXAMPLE_SOURCE "")
if(CONFIG_REALTEK_BUILD_EXAMPLE_SYSVAL_REQUEST)
    set(SYSVAL_EXAMPLE_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/example_sysval_request.c)
elseif(CONFIG_REALTEK_BUILD_EXAMPLE_SYSVAL_SUBSCRIBE)
    set(SYSVAL_EXAMPLE_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/example_sysval_subscribe.c)
endif()

if(SYSVAL_EXAMPLE_SOURCE)
    target_sources(${GUI_TARGET_NAME} PRIVATE
        ${SYSVAL_EXAMPLE_SOURCE}
    )
    target_include_directories(${GUI_TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    target_compile_definitions(${GUI_TARGET_NAME} PRIVATE
        DRV_LCD_WIDTH=480
        DRV_LCD_HEIGHT=480
        DRV_PIXEL_BITS=16
    )
endif()
