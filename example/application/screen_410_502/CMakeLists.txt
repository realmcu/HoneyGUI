#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

# CMakeLists.txt for screen_410_502 demo
cmake_minimum_required(VERSION 3.10)

if(CONFIG_REALTEK_BUILD_GUI_410_502_DEMO)
    # Collect all source files
    file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
    file(GLOB SOURCES_CPP "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
    
    # Add sources to target
    if(SOURCES OR SOURCES_CPP)
        target_sources(${GUI_TARGET_NAME} PRIVATE ${SOURCES} ${SOURCES_CPP})
    endif()
    
    # Add include directories
    target_include_directories(${GUI_TARGET_NAME} PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/root_image
    )
    
    # Add compile definitions
    target_compile_definitions(${GUI_TARGET_NAME} PRIVATE
        ENABLE_RTK_GUI_410_502_DEMO
        DRV_LCD_WIDTH=410
        DRV_LCD_HEIGHT=502
        DRV_PIXEL_BITS=16
        DRV_LCD_CORNER_RADIUS=50
    )
    
    message(STATUS "Building screen_410_502 demo (410x502)")
    
    message(STATUS "Added screen_410_502 demo sources")
endif()
