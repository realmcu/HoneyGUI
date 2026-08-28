#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10)
if(1)
    option(GUI_CIRCLE_ENABLE_A8 "Use A8 masks for solid circle geometry" ON)
    option(GUI_RECT_ENABLE_A8 "Use A8 masks for solid rectangle geometry" ON)
    option(GUI_ARC_ENABLE_INK_BOUNDS "Crop arc buffers to their actual ink bounds" ON)
    option(GUI_SHAPE_CACHE_ENABLE "Share identical geometry pixel payloads" ON)
    option(GUI_SHAPE_CACHE_USE_LOWER_HEAP "Store geometry cache nodes in lower memory" OFF)

    if(GUI_CIRCLE_ENABLE_A8)
        target_compile_definitions(${GUI_TARGET_NAME} PRIVATE GUI_CIRCLE_ENABLE_A8=1)
    else()
        target_compile_definitions(${GUI_TARGET_NAME} PRIVATE GUI_CIRCLE_ENABLE_A8=0)
    endif()

    if(GUI_RECT_ENABLE_A8)
        target_compile_definitions(${GUI_TARGET_NAME} PRIVATE GUI_RECT_ENABLE_A8=1)
    else()
        target_compile_definitions(${GUI_TARGET_NAME} PRIVATE GUI_RECT_ENABLE_A8=0)
    endif()

    if(GUI_ARC_ENABLE_INK_BOUNDS)
        target_compile_definitions(${GUI_TARGET_NAME} PRIVATE GUI_ARC_ENABLE_INK_BOUNDS=1)
    else()
        target_compile_definitions(${GUI_TARGET_NAME} PRIVATE GUI_ARC_ENABLE_INK_BOUNDS=0)
    endif()

    if(GUI_SHAPE_CACHE_ENABLE)
        target_compile_definitions(${GUI_TARGET_NAME} PRIVATE GUI_SHAPE_CACHE_ENABLE=1)
    else()
        target_compile_definitions(${GUI_TARGET_NAME} PRIVATE GUI_SHAPE_CACHE_ENABLE=0)
    endif()

    if(GUI_SHAPE_CACHE_USE_LOWER_HEAP)
        target_compile_definitions(${GUI_TARGET_NAME} PRIVATE GUI_SHAPE_CACHE_USE_LOWER_HEAP=1)
    else()
        target_compile_definitions(${GUI_TARGET_NAME} PRIVATE GUI_SHAPE_CACHE_USE_LOWER_HEAP=0)
    endif()

    file(GLOB SOURCES "*.c" "*.cpp")
    file(GLOB HEADERS "*.h" "*.hpp")

    if(SOURCES)
        target_sources(${GUI_TARGET_NAME} PRIVATE ${SOURCES})
    endif()

    target_include_directories(${GUI_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    file(GLOB SUBDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

    foreach(subdir ${SUBDIRS})
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subdir})
            if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/CMakeLists.txt)
                add_subdirectory(${subdir})
            endif()
        endif()
    endforeach()
endif()
