#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required (VERSION 3.10)
cmake_policy(SET CMP0123 NEW)

set(CMAKE_SYSTEM_NAME Generic)

# Try to find arm-none-eabi-gcc in PATH first (Linux CI)
find_program(ARM_NONE_EABI_GCC arm-none-eabi-gcc)
if(ARM_NONE_EABI_GCC)
    set(CMAKE_C_COMPILER arm-none-eabi-gcc)
    set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
    message(STATUS "Using arm-none-eabi-gcc from PATH: ${ARM_NONE_EABI_GCC}")
else()
    # Fallback to Windows path for local development
    set(ARMCLANG_PATH "C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/13.2 Rel1/bin")
    set(CMAKE_C_COMPILER "${ARMCLANG_PATH}/arm-none-eabi-gcc.exe")
    set(CMAKE_CXX_COMPILER "${ARMCLANG_PATH}/arm-none-eabi-g++.exe")
    message(STATUS "Using arm-none-eabi-gcc from Windows path: ${ARMCLANG_PATH}")
endif()


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS ON) 
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_VERBOSE_MAKEFILE OFF)


set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "Installation Directory" FORCE)

# Default SOC
set(DEFAULT_SOC "RTL8762G")

# Check if SOC is defined
if(NOT DEFINED SOC)
    message(WARNING "SOC variable not specified. Using default SOC: ${DEFAULT_SOC}.")
    set(SOC "${DEFAULT_SOC}")
endif()
message("build SOC = ${SOC}")

# Set the SOC file path
set(SOC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${SOC}.cmake")

# Check if the SOC file exists and include it
if(EXISTS "${SOC_FILE}")
    include("${SOC_FILE}")
else()
    message(FATAL_ERROR "SOC file ${SOC_FILE} does not exist.")
endif()

# Locate the host Python interpreter. It must accept both "python3" and
# "python": CI machines differ, and the GitHub workflows currently paper over
# that by installing the python-is-python3 package. NO_CMAKE_FIND_ROOT_PATH
# keeps the search on the host -- CMAKE_SYSTEM_NAME is Generic here, so without
# it the lookup can be redirected into the target sysroot.
if(CMAKE_HOST_WIN32)
    # The official Windows installer ships python.exe only. A "python3" hit on
    # Windows is usually the Microsoft Store app-execution alias, which opens
    # the Store instead of running the script, so try python first there.
    set(HONEYGUI_PYTHON_NAMES python python3)
else()
    # On Linux "python" may be absent or still point at Python 2.
    set(HONEYGUI_PYTHON_NAMES python3 python)
endif()
find_program(HONEYGUI_PYTHON NAMES ${HONEYGUI_PYTHON_NAMES} NO_CMAKE_FIND_ROOT_PATH)
if(NOT HONEYGUI_PYTHON)
    message(FATAL_ERROR
        "No Python interpreter found (tried python3, python). Install Python 3 "
        "or pass -DHONEYGUI_PYTHON=<path>.")
endif()

execute_process(
    COMMAND ${HONEYGUI_PYTHON}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../tool/git_generate_version.py
            ${CMAKE_CURRENT_SOURCE_DIR}/../../realgui/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE gui_version_result
    ERROR_VARIABLE gui_version_error
    OUTPUT_VARIABLE gui_version_output
)
if(gui_version_result)
    message(FATAL_ERROR "git_generate_version.py failed: ${gui_version_error}")
endif()

execute_process(
    COMMAND ${HONEYGUI_PYTHON}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../tool/convert_dot_config_to.py
            ${CMAKE_CURRENT_SOURCE_DIR}/.config
            ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE gui_config_result
    ERROR_VARIABLE gui_config_error
    OUTPUT_VARIABLE gui_config_output
)
if(gui_config_result)
    message(FATAL_ERROR "convert_dot_config_to.py failed: ${gui_config_error}")
endif()


include(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)
project(HoneyGUI)

set(GUI_TARGET_NAME gui)

add_library(${GUI_TARGET_NAME} STATIC)
# set_target_properties(${GUI_TARGET_NAME} PROPERTIES PREFIX "" OUTPUT_NAME ${GUI_TARGET_NAME} SUFFIX ".lib")

add_subdirectory(../../realgui build/realgui)

install(TARGETS ${GUI_TARGET_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

include(${CMAKE_CURRENT_SOURCE_DIR}/../install_headers.cmake)
