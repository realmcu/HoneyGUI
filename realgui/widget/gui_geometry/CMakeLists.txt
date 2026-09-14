#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.12)
if(CONFIG_REALTEK_GUI_WIDGET_GEOMETRY)
    option(GUI_CIRCLE_ENABLE_A8 "Use A8 masks for solid circle geometry" ON)
    option(GUI_RECT_ENABLE_A8 "Use A8 masks for solid rectangle geometry" ON)
    option(GUI_ARC_ENABLE_INK_BOUNDS "Crop arc buffers to their actual ink bounds" ON)
    option(GUI_SHAPE_CACHE_ENABLE "Share identical geometry pixel payloads" ON)
    option(GUI_SHAPE_CACHE_USE_LOWER_HEAP "Store geometry cache nodes in lower memory" OFF)

    foreach(geometry_option
            GUI_CIRCLE_ENABLE_A8
            GUI_RECT_ENABLE_A8
            GUI_ARC_ENABLE_INK_BOUNDS
            GUI_SHAPE_CACHE_ENABLE
            GUI_SHAPE_CACHE_USE_LOWER_HEAP)
        if(NOT "${${geometry_option}}" STREQUAL "")
            if(${${geometry_option}})
                target_compile_definitions(${GUI_TARGET_NAME} PRIVATE ${geometry_option}=1)
            else()
                target_compile_definitions(${GUI_TARGET_NAME} PRIVATE ${geometry_option}=0)
            endif()
            message(STATUS "gui_geometry: ${geometry_option} overridden to ${${geometry_option}}")
        endif()
    endforeach()

    file(GLOB SOURCES CONFIGURE_DEPENDS "*.c" "*.cpp")
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
