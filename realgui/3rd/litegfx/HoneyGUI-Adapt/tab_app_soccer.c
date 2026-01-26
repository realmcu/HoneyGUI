/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "gui_widget_litegfx.h"
#include "root_image/ui_resource.h"

static const void *iconList[20] =
{
    SOCCER_ACTIVITIES_BIN,
    SOCCER_ALARM_BIN,
    SOCCER_ALIPAY_BIN,
    SOCCER_CARDS_BIN,
    SOCCER_COMPASS_BIN,
    SOCCER_COUNTDOWN_BIN,
    SOCCER_FINDPHONE_BIN,
    SOCCER_GOAL_BIN,
    SOCCER_HEARTRATE_BIN,
    SOCCER_HOME_BIN,
    SOCCER_MUSIC_BIN,
    SOCCER_SCHEDULE_BIN,
    SOCCER_SETTINGS_BIN,
    SOCCER_SLEEP_BIN,
    SOCCER_SPO2_BIN,
    SOCCER_SPORTS_BIN,
    SOCCER_STOPWATCH_BIN,
    SOCCER_STRESS_BIN,
    SOCCER_TEMPERATURE_BIN,
    SOCCER_WEATHER_BIN,
};

static uintptr_t app_soccer_user_cb(uint16_t cmd, uintptr_t param, uintptr_t user_data)
{
    if (cmd == LX_CMD_ID_INIT)
    {
        gui_obj_t *obj = (gui_obj_t *)user_data;

        //先设置，优先缓存至Sram
        gui_widget_litegfx_set_image(obj, 0, SOCCER_STAR_BIN);
        gui_widget_litegfx_set_image(obj, 1, SOCCER_BOARD_BIN);

        for (uint8_t index = 0; index < 20; index++)
        {
            gui_widget_litegfx_set_image(obj, index + 2, (void *)iconList[index]);
        }
    }

    if (cmd == LX_CMD_ID_CLICK)
    {
        gui_log("clicked item:%d", param);

    }

    return 0;
}

void design_tab_app_soccer(void *parent)
{
    gui_widget_litegfx_t *widget = gui_widget_litegfx_create(parent, "lx vglite soccer",
                                                             LX_VGWIDGET_INS_SOCCER, app_soccer_user_cb, 0, 0, 454, 454);
}