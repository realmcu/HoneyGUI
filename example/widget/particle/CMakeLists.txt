# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.10)

# Effect files: depend on particle system core
if(CONFIG_REALTEK_BUILD_PARTICLE_SYSTEM)
    set(EFFECT_SOURCES
        effect_snow.c
        effect_rocket.c
        effect_firework.c
        effect_trail.c
        effect_touch.c
        effect_bubble.c
        effect_custom.c
        effect_tunnel.c
        effect_vortex.c
        effect_lightning.c
        effect_galaxy.c
        effect_fireflies.c
        effect_ripple.c
        effect_rain.c
        effect_magic_circle.c
        effect_light_beam.c
    )
    target_sources(${GUI_TARGET_NAME} PRIVATE ${EFFECT_SOURCES})
    target_include_directories(${GUI_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# Demo, launcher, example: depend on particle demo selection
if(CONFIG_REALTEK_BUILD_PARTICLE_DEMO)
    set(DEMO_SOURCES
        particle_demo.c
        particle_launcher.c
        example_particle.c
    )
    target_sources(${GUI_TARGET_NAME} PRIVATE ${DEMO_SOURCES})
endif()
