#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

# Tests that use gui_geometry and gui_list as scaffolding. Both are frozen in
# nano, so these build only when both are enabled.
if(CONFIG_REALTEK_BUILD_EXAMPLE_TEXT_WIDGET AND
        CONFIG_REALTEK_GUI_WIDGET_GEOMETRY AND
        CONFIG_REALTEK_GUI_WIDGET_LIST)
    file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
    target_sources(${GUI_TARGET_NAME} PRIVATE ${SOURCES})
    target_include_directories(${GUI_TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
    )
    target_compile_definitions(${GUI_TARGET_NAME} PRIVATE
        DRV_LCD_WIDTH=480
        DRV_LCD_HEIGHT=480
        DRV_PIXEL_BITS=16
    )
endif()
