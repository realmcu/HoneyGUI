/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "gui_widget_litegfx.h"
#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_text.h"

static uint32_t get_image_size(void *data)
{
    uint32_t size = 0;
    struct gui_rgb_data_head *head_p = (struct gui_rgb_data_head *)data;
    uint8_t source_bytes_per_pixel = 0;

    switch (head_p->type)
    {
    case RGB565:
        source_bytes_per_pixel = 2;
        break;
    case RTKARGB8565:
        source_bytes_per_pixel = 3;
        break;
    case ARGB8565:
        source_bytes_per_pixel = 3;
        break;
    case RGB888:
        source_bytes_per_pixel = 3;
        break;
    case ARGB8888:
        source_bytes_per_pixel = 4;
        break;
    default:
        break;
    }

    size = sizeof(struct gui_rgb_data_head) + head_p->w * head_p->h * source_bytes_per_pixel;
    return size;
}

uintptr_t app_notifications_user_cb(uint16_t cmd, uintptr_t param, uintptr_t user_data)
{
    if (cmd == LX_CMD_ID_INIT)
    {
        gui_obj_t *obj = (gui_obj_t *)user_data;

        gui_widget_litegfx_set_image(obj, 0, (void *)NOTIFICATIONS_ENVELOPE_BIN);
        gui_widget_litegfx_set_image(obj, 1, (void *)NOTIFICATIONS_ENVELOPEBG_BIN);
        gui_widget_litegfx_set_image(obj, 2, (void *)NOTIFICATIONS_FLAPOPEN_BIN);
        gui_widget_litegfx_set_image(obj, 3, (void *)NOTIFICATIONS_FLAPCLOSE_BIN);

        gui_widget_litegfx_set_param(obj, LX_CMD_ID_SET_NUMBER,
                                     6);//设置通知个数，最小3，最大12，默认为6
    }
    else if (cmd == LX_CMD_ID_GET_IMAGE)
    {
        static lx_vglite_image_t img;
        struct gui_rgb_data_head *head_p = NULL;

        head_p = (struct gui_rgb_data_head *)NOTIFICATIONS_LETTERBG_BIN;

        img.format = LX_IMG_CF_RGB565;
        img.w = head_p->w;
        img.h = head_p->h;
        img.data = NOTIFICATIONS_LETTERBG_BIN;
        img.size = get_image_size(NOTIFICATIONS_LETTERBG_BIN);

        return (uintptr_t)&img;
    }
    else if (cmd == LX_CMD_ID_FREE_IMAGE)
    {

    }

    return 0;
}

void design_tab_app_notifications(void *parent)
{
    gui_widget_litegfx_t *widget = gui_widget_litegfx_create(parent, "lx vglite notifications",
                                                             LX_VGWIDGET_INS_NOTIFICATIONS, app_notifications_user_cb, 0, 0, 454, 454);
}