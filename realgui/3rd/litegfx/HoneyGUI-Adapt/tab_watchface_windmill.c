/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "gui_widget_litegfx.h"
#include "root_image/ui_resource.h"
#include <app_gui_main.h>
#if 1

uintptr_t watchface_windmill_user_cb(uint16_t cmd, uintptr_t param, uintptr_t user_data)
{
    if (cmd == LX_CMD_ID_INIT)
    {
        gui_obj_t *obj = (gui_obj_t *)user_data;

        gui_widget_litegfx_set_image(obj, 0, WINDMILL_BUBBLE_BIN);
        gui_widget_litegfx_set_image(obj, 1, WINDMILL_NAIL_BIN);
        gui_widget_litegfx_set_image(obj, 2, WINDMILL_FACEBLUE_BIN);
        gui_widget_litegfx_set_image(obj, 3, WINDMILL_FACEGREEN_BIN);
        gui_widget_litegfx_set_image(obj, 4, WINDMILL_FACERED_BIN);
        gui_widget_litegfx_set_image(obj, 5, WINDMILL_FACEYELLOW_BIN);
        gui_widget_litegfx_set_image(obj, 6, WINDMILL_FACEBLUE_SHADOW_BIN);
        gui_widget_litegfx_set_image(obj, 7, WINDMILL_FACEGREEN_SHADOW_BIN);
        gui_widget_litegfx_set_image(obj, 8, WINDMILL_FACERED_SHADOW_BIN);
        gui_widget_litegfx_set_image(obj, 9, WINDMILL_FACEYELLOW_SHADOW_BIN);
        gui_widget_litegfx_set_image(obj, 10, WINDMILL_STICK_BIN);
        gui_widget_litegfx_set_image(obj, 11, WINDMILL_BACKGROUND_BIN);

    }
    return 0;
}

void design_tab_watchface_windmill(void *parent)
{
    gui_widget_litegfx_t *widget = gui_widget_litegfx_create(parent, "lx vglite windmill",
                                                             LX_VGWIDGET_INS_WINDMILL, watchface_windmill_user_cb, 0, 0, 454, 454);
}
#endif