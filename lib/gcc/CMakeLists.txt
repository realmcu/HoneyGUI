#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required (VERSION 3.10)


set(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS ON) 
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_VERBOSE_MAKEFILE OFF)


set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "Installation Directory" FORCE)

execute_process( 
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/../../tool/convert_dot_config_to.py 
    ${CMAKE_CURRENT_SOURCE_DIR}/.config 
    ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

include(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)

project(HoneyGUI)



set(GUI_TARGET_NAME gui)

add_library(${GUI_TARGET_NAME} STATIC)
add_subdirectory(../../realgui build/realgui)

install(TARGETS ${GUI_TARGET_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

include(${CMAKE_CURRENT_SOURCE_DIR}/../install_headers.cmake)
