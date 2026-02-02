/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_api.h"
#include "gui_port.h"
#include <SDL.h>
#include <pthread.h>
#include "unistd.h"
#include "tp_algo.h"
#include "kb_algo.h"
#include "gui_message.h"


gui_touch_port_data_t tp_port_data = {0};

gui_wheel_port_data_t wheel_port_data = {0};


gui_touch_port_data_t *port_touchpad_get_data(void)
{
    tp_port_data.timestamp_ms = gui_ms_get();//todo
    return &tp_port_data;
}

gui_wheel_port_data_t *port_wheel_get_data(void)
{
    wheel_port_data.timestamp_ms = gui_ms_get();
    return &wheel_port_data;
}


static struct gui_indev indev =
{
    .tp_get_data = port_touchpad_get_data,
    .wheel_get_port_data = port_wheel_get_data,
    .tp_height = 0,
    .tp_witdh = 0,
    .touch_timeout_ms = 110,
    .long_button_time_ms = 800,
    .short_button_time_ms = 800,
    .quick_slide_time_ms = 50,

    .kb_short_button_time_ms = 30,
    .kb_long_button_time_ms = 800,
};


bool home_state = false;
bool back_state = false;
bool menu_state = false;
bool power_state = false;
uint32_t home_timestamp_ms_press = 0;
uint32_t home_timestamp_ms_release = 0;
uint32_t home_timestamp_ms_pressing = 0;
uint32_t back_timestamp_ms_press = 0;
uint32_t back_timestamp_ms_release = 0;
uint32_t back_timestamp_ms_pressing = 0;
uint32_t menu_timestamp_ms_press = 0;
uint32_t menu_timestamp_ms_release = 0;
uint32_t menu_timestamp_ms_pressing = 0;
uint32_t power_timestamp_ms_press = 0;
uint32_t power_timestamp_ms_release = 0;
uint32_t power_timestamp_ms_pressing = 0;

void gui_port_indev_init(void)
{
    gui_indev_info_register(&indev);

    gui_kb_create("Home", &home_state,
                  &home_timestamp_ms_press,
                  &home_timestamp_ms_release);
    gui_kb_create("Back", &back_state,
                  &back_timestamp_ms_press,
                  &back_timestamp_ms_release);
    gui_kb_create("Menu", &menu_state,
                  &menu_timestamp_ms_press,
                  &menu_timestamp_ms_release);
    gui_kb_create("Power", &power_state,
                  &power_timestamp_ms_press,
                  &power_timestamp_ms_release);
}




