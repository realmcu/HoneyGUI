#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10)

# Browser-specific port: os, display, input, filesystem, flash.
file(GLOB SOURCES "*.c")
target_sources(${GUI_TARGET_NAME} PRIVATE ${SOURCES})
target_include_directories(${GUI_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Platform-neutral parts of the desktop port are shared with win32_sim so the
# two simulators cannot drift apart: software acceleration (gui_port_acc.c),
# system values (sysval/) and the port header.
set(WIN32_SIM_PORT ${CMAKE_CURRENT_SOURCE_DIR}/../../win32_sim/port/realgui_port)
target_sources(${GUI_TARGET_NAME} PRIVATE ${WIN32_SIM_PORT}/gui_port_acc.c)
target_include_directories(${GUI_TARGET_NAME} PUBLIC ${WIN32_SIM_PORT})
add_subdirectory(${WIN32_SIM_PORT}/sysval sysval)
