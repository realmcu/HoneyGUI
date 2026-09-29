/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Power system values for the simulator.
 *
 * SDL_GetPowerInfo() reports the host machine's real battery, so these
 * values are genuine on a laptop. A desktop has no battery: SDL then
 * reports SDL_POWERSTATE_NO_BATTERY, and the level falls back to a fixed
 * percentage so the UI still has something plausible to draw.
 */

#include <SDL.h>
#include "gui_components_init.h"
#include "gui_sysval_keys.h"
#include "gui_sysval_provider.h"

/** Level reported when the host has no battery, as on a desktop. */
#define SIMULATED_BATTERY_LEVEL  76

static bool port_sysval_battery_get(gui_sysval_complete_cb_t complete,
                                    void *request_context)
{
    int percent = -1;
    gui_sysval_value_t value;

    SDL_GetPowerInfo(NULL, &percent);
    if (percent < 0 || percent > 100)
    {
        percent = SIMULATED_BATTERY_LEVEL;
    }

    value = gui_sysval_value_int(percent);
    complete(request_context, GUI_SYSVAL_STATUS_OK, &value);
    return true;
}

/*
 * Charging state. SDL_POWERSTATE_CHARGED counts as charging: the cable is
 * in, which is what a charging indicator is meant to show. A host without a
 * battery reads as charging for the same reason, since it runs on mains.
 */
static bool port_sysval_battery_charging_get(gui_sysval_complete_cb_t complete,
                                             void *request_context)
{
    SDL_PowerState state = SDL_GetPowerInfo(NULL, NULL);
    gui_sysval_value_t value;

    switch (state)
    {
    case SDL_POWERSTATE_CHARGING:
    case SDL_POWERSTATE_CHARGED:
    case SDL_POWERSTATE_NO_BATTERY:
        value = gui_sysval_value_bool(true);
        break;

    case SDL_POWERSTATE_ON_BATTERY:
        value = gui_sysval_value_bool(false);
        break;

    default:
        /* SDL cannot tell. Reporting a guess here would make a charging
         * icon flicker, so fail and let the UI keep its placeholder. */
        complete(request_context, GUI_SYSVAL_STATUS_ERROR, NULL);
        return true;
    }

    complete(request_context, GUI_SYSVAL_STATUS_OK, &value);
    return true;
}

/*
 * Seconds of battery left. Only meaningful while discharging; SDL returns
 * -1 when plugged in or when it cannot estimate, which is reported as an
 * error rather than as a zero that would read as "battery empty".
 */
static bool port_sysval_battery_time_get(gui_sysval_complete_cb_t complete,
                                         void *request_context)
{
    int seconds = -1;
    gui_sysval_value_t value;

    SDL_GetPowerInfo(&seconds, NULL);
    if (seconds < 0)
    {
        complete(request_context, GUI_SYSVAL_STATUS_ERROR, NULL);
        return true;
    }

    value = gui_sysval_value_int(seconds);
    complete(request_context, GUI_SYSVAL_STATUS_OK, &value);
    return true;
}

static gui_sysval_handler_t battery_handler =
{
    .key = GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_battery_get,
    .set_request = NULL,
    .next = NULL,
};

static gui_sysval_handler_t battery_charging_handler =
{
    .key = GUI_SYSVAL_KEY_POWER_BATTERY_CHARGING,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_battery_charging_get,
    .set_request = NULL,
    .next = NULL,
};

static gui_sysval_handler_t battery_time_handler =
{
    .key = GUI_SYSVAL_KEY_POWER_BATTERY_REMAINING_SECONDS,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_battery_time_get,
    .set_request = NULL,
    .next = NULL,
};

static int port_sysval_power_init(void)
{
    if (!gui_sysval_handler_register(&battery_handler) ||
        !gui_sysval_handler_register(&battery_charging_handler) ||
        !gui_sysval_handler_register(&battery_time_handler))
    {
        return -1;
    }

    return 0;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_power_init);
