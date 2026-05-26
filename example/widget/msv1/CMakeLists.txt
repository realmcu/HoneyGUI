#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10)

if(CONFIG_REALTEK_BUILD_EXAMPLE_MSV1_WIDGET)

    file(GLOB SOURCES "*.c")
    file(GLOB HEADERS "*.h")

    if(SOURCES AND HEADERS)
        target_sources(${GUI_TARGET_NAME} PRIVATE ${SOURCES})
        target_include_directories(${GUI_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    elseif(SOURCES)
        target_sources(${GUI_TARGET_NAME} PRIVATE ${SOURCES})
    elseif(HEADERS)
        target_include_directories(${GUI_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    # Binary resource embedding for CMake builds (host/simulator only)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(BIN_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/root_image/output_msv1_360.avi")
        set(OBJ_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/root_image/output_msv1_360_avi.o")
        if(EXISTS ${BIN_FILE})
            add_custom_command(
                OUTPUT  ${OBJ_FILE}
                COMMAND objcopy -I binary -O elf64-x86-64 --binary-architecture i386
                        "${BIN_FILE}" "${OBJ_FILE}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/root_image"
                DEPENDS ${BIN_FILE}
                COMMENT "Converting output_msv1_360.avi to linkable object"
            )
            target_sources(${GUI_TARGET_NAME} PRIVATE ${OBJ_FILE})
        endif()
    endif()

    target_compile_definitions(${GUI_TARGET_NAME} PRIVATE
        DRV_LCD_WIDTH=360
        DRV_LCD_HEIGHT=360
        DRV_PIXEL_BITS=16
        DRV_LCD_CORNER_RADIUS=0
    )

endif()
