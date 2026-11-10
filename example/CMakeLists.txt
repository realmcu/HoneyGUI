#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10)

function(gui_add_binary_resource)
    cmake_parse_arguments(RESOURCE "" "TARGET;INPUT;OUTPUT_NAME" "" ${ARGN})

    if(NOT RESOURCE_TARGET OR NOT RESOURCE_INPUT OR NOT RESOURCE_OUTPUT_NAME)
        message(FATAL_ERROR "gui_add_binary_resource requires TARGET, INPUT, and OUTPUT_NAME")
    endif()

    get_filename_component(INPUT_FILE "${RESOURCE_INPUT}" ABSOLUTE
                           BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    if(NOT EXISTS "${INPUT_FILE}")
        message(FATAL_ERROR "Binary resource not found: ${INPUT_FILE}")
    endif()

    if(NOT CMAKE_OBJCOPY)
        find_program(CMAKE_OBJCOPY NAMES objcopy)
    endif()
    if(NOT CMAKE_OBJCOPY)
        message(FATAL_ERROR "objcopy is required to embed ${INPUT_FILE}")
    endif()

    get_filename_component(INPUT_DIR "${INPUT_FILE}" DIRECTORY)
    get_filename_component(INPUT_NAME "${INPUT_FILE}" NAME)
    set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/resources")
    set(OUTPUT_FILE "${OUTPUT_DIR}/${RESOURCE_OUTPUT_NAME}")

    if(WIN32)
        set(OUTPUT_FORMAT pe-x86-64)
    else()
        set(OUTPUT_FORMAT elf64-x86-64)
    endif()

    add_custom_command(
        OUTPUT "${OUTPUT_FILE}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${OUTPUT_DIR}"
        COMMAND "${CMAKE_OBJCOPY}" -I binary -O "${OUTPUT_FORMAT}"
                --binary-architecture i386:x86-64
                "${INPUT_NAME}" "${OUTPUT_FILE}"
        WORKING_DIRECTORY "${INPUT_DIR}"
        DEPENDS "${INPUT_FILE}"
        VERBATIM
    )
    string(MAKE_C_IDENTIFIER
           "${CMAKE_CURRENT_BINARY_DIR}_${RESOURCE_OUTPUT_NAME}"
           RESOURCE_TARGET_NAME)
    add_custom_target(${RESOURCE_TARGET_NAME} DEPENDS "${OUTPUT_FILE}")
    add_dependencies(${RESOURCE_TARGET} ${RESOURCE_TARGET_NAME})
    set_property(GLOBAL APPEND PROPERTY GUI_BINARY_RESOURCES "${OUTPUT_FILE}")
endfunction()
if(1)
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
