/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <SDL.h>
#include "gui_components_init.h"
#include "gui_sysval_provider.h"

#define SIMULATED_BATTERY_LEVEL  76
#define SYSVAL_KEY_BATTERY_LEVEL "power/battery/level"

static bool port_sysval_battery_get(gui_sysval_complete_cb_t complete,
                                    void *request_context)
{
    int percent = -1;
    char value[4];

    SDL_GetPowerInfo(NULL, &percent);
    if (percent < 0 || percent > 100)
    {
        percent = SIMULATED_BATTERY_LEVEL;
    }

    snprintf(value, sizeof(value), "%d", percent);
    complete(request_context, GUI_SYSVAL_STATUS_OK, value);
    return true;
}

static gui_sysval_handler_t battery_handler =
{
    .key = SYSVAL_KEY_BATTERY_LEVEL,
    .get_request = port_sysval_battery_get,
    .set_request = NULL,
    .next = NULL,
};

static int port_sysval_power_init(void)
{
    return gui_sysval_handler_register(&battery_handler) ? 0 : -1;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_power_init);
