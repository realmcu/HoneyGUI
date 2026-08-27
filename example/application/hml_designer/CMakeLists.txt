#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

if(CONFIG_REALTEK_BUILD_HML_DESIGNER)
    target_sources(${GUI_TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/app_hml_designer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/app_romfs.c
    )
    target_include_directories(${GUI_TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()
