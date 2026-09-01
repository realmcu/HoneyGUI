#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10)
if(CONFIG_REALTEK_BUILD_GUI_BOX2D)
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
