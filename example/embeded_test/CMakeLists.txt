#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.12)

if(CONFIG_REALTEK_BUILD_EMBEDED_TEST)
    target_compile_definitions(${GUI_TARGET_NAME} PRIVATE
        DRV_LCD_WIDTH=410
        DRV_LCD_HEIGHT=502
        DRV_PIXEL_BITS=16
    )
    file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
                                        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
    file(GLOB HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
                                        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

    if(SOURCES)
        target_sources(${GUI_TARGET_NAME} PRIVATE ${SOURCES})
    endif()

    set(TEST_FONT_ASSET_DIR ${CMAKE_CURRENT_BINARY_DIR}/font_assets)
    file(MAKE_DIRECTORY ${TEST_FONT_ASSET_DIR})
    generate_inc_file_for_target(
        ${GUI_TARGET_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/../assets/font/v3/NotoSans_Regular_size32_bits4_bitmap.bin
        ${TEST_FONT_ASSET_DIR}/test_font_noto_bitmap.inc
    )
    generate_inc_file_for_target(
        ${GUI_TARGET_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/../assets/font/v3/NotoSans_Regular_vector.bin
        ${TEST_FONT_ASSET_DIR}/test_font_noto_vector.inc
    )

    target_include_directories(${GUI_TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${TEST_FONT_ASSET_DIR}
        ${CMAKE_SOURCE_DIR}
    )

    file(GLOB SUBDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

    foreach(subdir ${SUBDIRS})
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subdir})
            if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/CMakeLists.txt)
                add_subdirectory(${subdir})
            endif()
        endif()
    endforeach()
endif()
