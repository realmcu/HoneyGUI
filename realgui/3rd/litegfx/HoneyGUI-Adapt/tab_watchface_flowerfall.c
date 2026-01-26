/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "gui_widget_litegfx.h"
#include "root_image/ui_resource.h"
#if 1

uintptr_t watchface_flowerfall_user_cb(uint16_t cmd, uintptr_t param, uintptr_t user_data)
{
    if (cmd == LX_CMD_ID_INIT)
    {
        gui_obj_t *obj = (gui_obj_t *)user_data;

        gui_widget_litegfx_set_image(obj, 0, FLOWERFALL_DOT01_BIN);
        gui_widget_litegfx_set_image(obj, 1, FLOWERFALL_DOT02_BIN);
        gui_widget_litegfx_set_image(obj, 2, FLOWERFALL_LEAF01_BIN);
        gui_widget_litegfx_set_image(obj, 3, FLOWERFALL_LEAF02_BIN);
        gui_widget_litegfx_set_image(obj, 4, FLOWERFALL_LEAF03_BIN);
        gui_widget_litegfx_set_image(obj, 5, FLOWERFALL_LEAF04_BIN);
        gui_widget_litegfx_set_image(obj, 6, FLOWERFALL_LEAF05_BIN);
        gui_widget_litegfx_set_image(obj, 7, FLOWERFALL_LEAF06_BIN);
        gui_widget_litegfx_set_image(obj, 8, FLOWERFALL_BACKGROUND_BIN);
        gui_widget_litegfx_set_image(obj, 9, FLOWERFALL_BRANCH01_BIN);
        gui_widget_litegfx_set_image(obj, 10, FLOWERFALL_BRANCH02_BIN);

    }
    return 0;
}

void design_tab_watchface_flowerfall(void *parent)
{
    gui_widget_litegfx_t *widget = gui_widget_litegfx_create(parent, "lx vglite flower fall",
                                                             LX_VGWIDGET_INS_FLOWER_FALL, watchface_flowerfall_user_cb, 0, 0, 454, 454);
}
#endif