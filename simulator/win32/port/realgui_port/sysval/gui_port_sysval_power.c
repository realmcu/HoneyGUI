/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Battery values for the simulator. Fixed values; a device port reads the
 * fuel gauge instead.
 */

#include <stddef.h>
#include "gui_components_init.h"
#include "gui_sysval_adapter.h"

static int32_t battery_level = 76;
static bool battery_charging = false;
static int32_t battery_remaining_seconds = 5 * 3600;

static void port_sysval_battery_level_get(gui_sysval_request_t *request)
{
    gui_sysval_value_t value = gui_sysval_value_int(battery_level);

    gui_sysval_respond(request, GUI_SYSVAL_STATUS_OK, &value);
}

static void port_sysval_battery_charging_get(gui_sysval_request_t *request)
{
    gui_sysval_value_t value = gui_sysval_value_bool(battery_charging);

    gui_sysval_respond(request, GUI_SYSVAL_STATUS_OK, &value);
}

static void port_sysval_battery_remaining_get(gui_sysval_request_t *request)
{
    gui_sysval_value_t value = gui_sysval_value_int(battery_remaining_seconds);

    gui_sysval_respond(request, GUI_SYSVAL_STATUS_OK, &value);
}

static gui_sysval_adapter_t battery_level_adapter =
{
    .key = GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_battery_level_get,
};

static gui_sysval_adapter_t battery_charging_adapter =
{
    .key = GUI_SYSVAL_KEY_POWER_BATTERY_CHARGING,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_battery_charging_get,
};

static gui_sysval_adapter_t battery_remaining_adapter =
{
    .key = GUI_SYSVAL_KEY_POWER_BATTERY_REMAINING_SECONDS,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_battery_remaining_get,
};

static int port_sysval_power_init(void)
{
    gui_sysval_adapter_register(&battery_level_adapter);
    gui_sysval_adapter_register(&battery_charging_adapter);
    gui_sysval_adapter_register(&battery_remaining_adapter);
    return 0;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_power_init);
