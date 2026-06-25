#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10)

if(CONFIG_REALTEK_BUILD_EXAMPLE_STREAM_WIDGET)

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
        set(BIN_FILE_A  "${CMAKE_CURRENT_SOURCE_DIR}/root_image/duck.avi")    # MSV1
        set(OBJ_FILE_A  "${CMAKE_CURRENT_SOURCE_DIR}/root_image/duck_avi.o")
        set(BIN_FILE_B  "${CMAKE_CURRENT_SOURCE_DIR}/root_image/cat_00.avi")  # Cinepak
        set(OBJ_FILE_B  "${CMAKE_CURRENT_SOURCE_DIR}/root_image/cat_00_avi.o")

        if(EXISTS ${BIN_FILE_A})
            add_custom_command(
                OUTPUT  ${OBJ_FILE_A}
                COMMAND objcopy -I binary -O elf64-x86-64 --binary-architecture i386
                        "${BIN_FILE_A}" "${OBJ_FILE_A}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/root_image"
                DEPENDS ${BIN_FILE_A}
                COMMENT "Converting duck.avi to linkable object"
            )
            target_sources(${GUI_TARGET_NAME} PRIVATE ${OBJ_FILE_A})
        endif()

        if(EXISTS ${BIN_FILE_B})
            add_custom_command(
                OUTPUT  ${OBJ_FILE_B}
                COMMAND objcopy -I binary -O elf64-x86-64 --binary-architecture i386
                        "${BIN_FILE_B}" "${OBJ_FILE_B}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/root_image"
                DEPENDS ${BIN_FILE_B}
                COMMENT "Converting cat_00.avi to linkable object"
            )
            target_sources(${GUI_TARGET_NAME} PRIVATE ${OBJ_FILE_B})
        endif()
    endif()

    target_compile_definitions(${GUI_TARGET_NAME} PRIVATE
        DRV_LCD_WIDTH=640
        DRV_LCD_HEIGHT=360
        DRV_PIXEL_BITS=16
        DRV_LCD_CORNER_RADIUS=0
    )

endif()
