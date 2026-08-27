# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.10)

if(CONFIG_REALTEK_BUILD_PARTICLE_DEMO)
    target_sources(${GUI_TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/particle_demo.c
        ${CMAKE_CURRENT_SOURCE_DIR}/particle_launcher.c
        ${CMAKE_CURRENT_SOURCE_DIR}/example_particle.c
    )
    target_include_directories(${GUI_TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/root_image
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../realgui/widget/gui_particle/effects
    )
    target_compile_definitions(${GUI_TARGET_NAME} PRIVATE
        DRV_LCD_WIDTH=410
        DRV_LCD_HEIGHT=502
        DRV_PIXEL_BITS=16
    )
endif()
