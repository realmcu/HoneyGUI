# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.10)

# Particle System Example
if(CONFIG_REALTEK_BUILD_PARTICLE_DEMO)
    file(GLOB SOURCES "*.c")
    file(GLOB HEADERS "*.h")

    if(SOURCES)
        target_sources(${GUI_TARGET_NAME} PRIVATE ${SOURCES})
    endif()

    if(HEADERS)
        target_include_directories(${GUI_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
endif()
