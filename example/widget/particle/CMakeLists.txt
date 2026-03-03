# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.10)

# Demo, launcher, example: depend on particle demo selection
if(CONFIG_REALTEK_BUILD_PARTICLE_DEMO)
    # Effect headers are now in realgui/widget/gui_particle/
    target_include_directories(${GUI_TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../realgui/widget/gui_particle/effects)
    set(DEMO_SOURCES
        particle_demo.c
        particle_launcher.c
        example_particle.c
    )
    target_sources(${GUI_TARGET_NAME} PRIVATE ${DEMO_SOURCES})
endif()
