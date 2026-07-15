#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#
# 3D examples (mirrors the per-CONFIG source selection from SConscript):
#   Each example_3d_*.c is guarded by its own CONFIG_REALTEK_BUILD_REAL_*_3D.
#   Resource dirs (dog3d/donut3d/... containing .obj/.mtl/.txt) are asset
#   data only and are not compiled here.
#

cmake_minimum_required(VERSION 3.10)

# Add include path / global define only when at least one 3D demo is on.
set(_gui3d_any OFF)
foreach(_cfg
    CONFIG_REALTEK_BUILD_REAL_DOG_3D
    CONFIG_REALTEK_BUILD_REAL_EARTH_3D
    CONFIG_REALTEK_BUILD_REAL_DONUT_3D
    CONFIG_REALTEK_BUILD_REAL_PIKACHU_GLTF_3D
    CONFIG_REALTEK_BUILD_REAL_FLAG_GLTF_3D
    CONFIG_REALTEK_BUILD_REAL_ROBOT_GLTF_3D)
    if(${_cfg})
        set(_gui3d_any ON)
    endif()
endforeach()

if(_gui3d_any)
    target_include_directories(${GUI_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_definitions(${GUI_TARGET_NAME} PRIVATE DRV_PIXEL_BITS=16)
endif()

# Per-CONFIG source selection, one-to-one with SConscript.
if(CONFIG_REALTEK_BUILD_REAL_DOG_3D)
    target_sources(${GUI_TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/example_3d_dog.c)
endif()

if(CONFIG_REALTEK_BUILD_REAL_EARTH_3D)
    target_sources(${GUI_TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/example_3d_earth.c)
endif()

if(CONFIG_REALTEK_BUILD_REAL_DONUT_3D)
    target_sources(${GUI_TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/example_3d_donut.c)
endif()

if(CONFIG_REALTEK_BUILD_REAL_PIKACHU_GLTF_3D)
    target_sources(${GUI_TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/example_3d_pikachu_gltf.c)
endif()

if(CONFIG_REALTEK_BUILD_REAL_FLAG_GLTF_3D)
    target_sources(${GUI_TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/example_3d_flag_gltf.c)
endif()

if(CONFIG_REALTEK_BUILD_REAL_ROBOT_GLTF_3D)
    target_sources(${GUI_TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/example_3d_robot_gltf.c)
endif()
