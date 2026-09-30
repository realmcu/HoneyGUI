/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Device settings for the simulator. Each value is a global: a set stores
 * it, a get returns it. A device port drives the backlight, codec or motor.
 */

#include <stddef.h>
#include "gui_components_init.h"
#include "gui_sysval_adapter.h"

static int32_t brightness = 80;
static int32_t volume = 50;
static bool muted = false;
static bool haptics = true;

static void respond_int(gui_sysval_request_t *request, int32_t i)
{
    gui_sysval_value_t value = gui_sysval_value_int(i);

    gui_sysval_respond(request, GUI_SYSVAL_STATUS_OK, &value);
}

static void respond_bool(gui_sysval_request_t *request, bool b)
{
    gui_sysval_value_t value = gui_sysval_value_bool(b);

    gui_sysval_respond(request, GUI_SYSVAL_STATUS_OK, &value);
}

/* Store a percentage, or reject it if out of range. */
static void set_percent(int32_t *target, const gui_sysval_value_t *value,
                        gui_sysval_request_t *request)
{
    if (value->data.i < GUI_SYSVAL_PERCENT_MIN ||
        value->data.i > GUI_SYSVAL_PERCENT_MAX)
    {
        gui_sysval_respond(request, GUI_SYSVAL_STATUS_INVALID_VALUE, NULL);
        return;
    }

    *target = value->data.i;
    respond_int(request, *target);
}

static void port_sysval_brightness_get(gui_sysval_request_t *request)
{
    respond_int(request, brightness);
}

static void port_sysval_brightness_set(const gui_sysval_value_t *value,
                                       gui_sysval_request_t *request)
{
    set_percent(&brightness, value, request);
}

static void port_sysval_volume_get(gui_sysval_request_t *request)
{
    respond_int(request, volume);
}

static void port_sysval_volume_set(const gui_sysval_value_t *value,
                                   gui_sysval_request_t *request)
{
    set_percent(&volume, value, request);
}

static void port_sysval_muted_get(gui_sysval_request_t *request)
{
    respond_bool(request, muted);
}

static void port_sysval_muted_set(const gui_sysval_value_t *value,
                                  gui_sysval_request_t *request)
{
    muted = value->data.b;
    respond_bool(request, muted);
}

static void port_sysval_haptics_get(gui_sysval_request_t *request)
{
    respond_bool(request, haptics);
}

static void port_sysval_haptics_set(const gui_sysval_value_t *value,
                                    gui_sysval_request_t *request)
{
    haptics = value->data.b;
    respond_bool(request, haptics);
}

static gui_sysval_adapter_t brightness_adapter =
{
    .key = GUI_SYSVAL_KEY_DISPLAY_BRIGHTNESS,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_brightness_get,
    .set_request = port_sysval_brightness_set,
};

static gui_sysval_adapter_t volume_adapter =
{
    .key = GUI_SYSVAL_KEY_AUDIO_VOLUME,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_volume_get,
    .set_request = port_sysval_volume_set,
};

static gui_sysval_adapter_t muted_adapter =
{
    .key = GUI_SYSVAL_KEY_AUDIO_MUTED,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_muted_get,
    .set_request = port_sysval_muted_set,
};

static gui_sysval_adapter_t haptics_adapter =
{
    .key = GUI_SYSVAL_KEY_HAPTICS_ENABLED,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_haptics_get,
    .set_request = port_sysval_haptics_set,
};

static int port_sysval_settings_init(void)
{
    gui_sysval_adapter_register(&brightness_adapter);
    gui_sysval_adapter_register(&volume_adapter);
    gui_sysval_adapter_register(&muted_adapter);
    gui_sysval_adapter_register(&haptics_adapter);
    return 0;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_settings_init);
