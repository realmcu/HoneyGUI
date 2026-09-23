/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <string.h>
#include "gui_api.h"
#include "gui_components_init.h"
#include "gui_obj.h"
#include "gui_text.h"

#define SYSVAL_TIME_TEXT_SIZE 16
#define SYSVAL_BATTERY_TEXT_SIZE 20
#define SYSVAL_TIME_FONT_SIZE 32
#define SYSVAL_BATTERY_FONT_SIZE 24
#define SYSVAL_KEY_TIME          "system/time"
#define SYSVAL_KEY_BATTERY_LEVEL "power/battery/level"

static char time_content[SYSVAL_TIME_TEXT_SIZE] = "--:--:--";
static char battery_content[SYSVAL_BATTERY_TEXT_SIZE] = "Battery: --%";
static gui_sysval_request_id_t time_request_id = GUI_SYSVAL_REQUEST_INVALID;
static gui_sysval_request_id_t battery_request_id = GUI_SYSVAL_REQUEST_INVALID;

static void time_response_cb(gui_sysval_request_id_t request_id,
                             gui_sysval_status_t status,
                             const char *value,
                             void *user_data)
{
    gui_text_t *text = (gui_text_t *)user_data;

    if (request_id != time_request_id)
    {
        return;
    }
    time_request_id = GUI_SYSVAL_REQUEST_INVALID;

    if (status != GUI_SYSVAL_STATUS_OK || value == NULL)
    {
        snprintf(time_content, sizeof(time_content), "--:--:--");
    }
    else
    {
        snprintf(time_content, sizeof(time_content), "%s", value);
    }

    gui_text_content_set(text, time_content, strlen(time_content));
}

static void battery_response_cb(gui_sysval_request_id_t request_id,
                                gui_sysval_status_t status,
                                const char *value,
                                void *user_data)
{
    gui_text_t *text = (gui_text_t *)user_data;

    if (request_id != battery_request_id)
    {
        return;
    }
    battery_request_id = GUI_SYSVAL_REQUEST_INVALID;

    if (status != GUI_SYSVAL_STATUS_OK || value == NULL)
    {
        snprintf(battery_content, sizeof(battery_content), "Battery: --%%");
    }
    else
    {
        snprintf(battery_content, sizeof(battery_content), "Battery: %s%%",
                 value);
    }

    gui_text_content_set(text, battery_content, strlen(battery_content));
}

static void time_request_cb(void *obj)
{
    if (time_request_id != GUI_SYSVAL_REQUEST_INVALID)
    {
        return;
    }

    time_request_id = gui_sysval_get_request(SYSVAL_KEY_TIME,
                                             time_response_cb,
                                             obj);
}

static void battery_request_cb(void *obj)
{
    if (battery_request_id != GUI_SYSVAL_REQUEST_INVALID)
    {
        return;
    }

    battery_request_id = gui_sysval_get_request(SYSVAL_KEY_BATTERY_LEVEL,
                                                battery_response_cb,
                                                obj);
}

static int app_init(void)
{
    int16_t screen_width = gui_get_screen_width();
    int16_t screen_height = gui_get_screen_height();
    gui_text_t *time_text = gui_text_create(gui_obj_get_root(), "system_time",
                                            0, (screen_height - 96) / 2,
                                            screen_width, 64);
    gui_text_t *battery_text =
        gui_text_create(gui_obj_get_root(), "battery_level",
                        0, (screen_height - 96) / 2 + 64,
                        screen_width, 32);

    gui_text_set(time_text, time_content, GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE, strlen(time_content),
                 SYSVAL_TIME_FONT_SIZE);
    gui_text_mode_set(time_text, MID_CENTER);

    gui_obj_create_timer(GUI_BASE(time_text), 1000, true, time_request_cb);
    gui_obj_start_timer(GUI_BASE(time_text));
    time_request_cb(time_text);

    gui_text_set(battery_text, battery_content, GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE, strlen(battery_content),
                 SYSVAL_BATTERY_FONT_SIZE);
    gui_text_mode_set(battery_text, MID_CENTER);

    gui_obj_create_timer(GUI_BASE(battery_text), 5000, true,
                         battery_request_cb);
    gui_obj_start_timer(GUI_BASE(battery_text));
    battery_request_cb(battery_text);

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
