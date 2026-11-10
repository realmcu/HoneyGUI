/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file gui_port_indev.c
 * @brief Input port for the browser (WebAssembly) simulator
 *
 * Pointer and keyboard events are delivered from JavaScript (web_sim.js)
 * through the web_sim_* functions below and polled by the engine each frame,
 * matching the desktop simulator's SDL event handling.
 */

#include "guidef.h"
#include "gui_api.h"
#include "gui_port.h"
#include "kb_algo.h"
#include "web_sim.h"
#include <emscripten.h>

gui_touch_port_data_t tp_port_data = {0};
gui_wheel_port_data_t wheel_port_data = {0};

static gui_touch_port_data_t *port_touchpad_get_data(void)
{
    tp_port_data.timestamp_ms = gui_ms_get();
    return &tp_port_data;
}

static gui_wheel_port_data_t *port_wheel_get_data(void)
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
    .quick_slide_time_ms = 200,
    .kb_short_button_time_ms = 30,
    .kb_long_button_time_ms = 800,
};

/* Keys exposed by the desktop simulator's control panel. */
typedef struct
{
    char *name;
    bool state;
    uint32_t press_ms;
    uint32_t release_ms;
} web_key_t;

static web_key_t keys[] =
{
    { (char *)"Home" },
    { (char *)"Back" },
    { (char *)"Menu" },
    { (char *)"Power" },
};

/* event: GUI_TOUCH_EVENT_UP / GUI_TOUCH_EVENT_DOWN (tp_algo.h), or 0 for a
 * pointer move, which keeps the current pressed state like SDL_MOUSEMOTION
 * does in the desktop simulator. */
EMSCRIPTEN_KEEPALIVE void web_sim_touch(int32_t event, int32_t x, int32_t y)
{
    if (event != 0)
    {
        tp_port_data.event = (uint8_t)event;
    }
    /* Not gui_get_screen_width(): that needs the display registered, and the
     * page may deliver input before main() runs. */
    int32_t max_x = web_sim_screen_width() - 1;
    int32_t max_y = web_sim_screen_height() - 1;

    tp_port_data.x_coordinate = (uint16_t)(x < 0 ? 0 : (x > max_x ? max_x : x));
    tp_port_data.y_coordinate = (uint16_t)(y < 0 ? 0 : (y > max_y ? max_y : y));
}

EMSCRIPTEN_KEEPALIVE void web_sim_wheel(int32_t event, int32_t delta)
{
    wheel_port_data.event = (uint8_t)event;
    wheel_port_data.delta = (int16_t)delta;
}

EMSCRIPTEN_KEEPALIVE void web_sim_key(int32_t key, int32_t pressed)
{
    if (key < 0 || key >= (int32_t)(sizeof(keys) / sizeof(keys[0])))
    {
        return;
    }
    web_key_t *k = &keys[key];
    k->state = pressed != 0;
    if (pressed)
    {
        k->press_ms = gui_ms_get();
    }
    else
    {
        k->release_ms = gui_ms_get();
    }
}

void gui_port_indev_init(void)
{
    gui_indev_info_register(&indev);
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
    {
        gui_kb_create(keys[i].name, &keys[i].state, &keys[i].press_ms, &keys[i].release_ms);
    }
}
