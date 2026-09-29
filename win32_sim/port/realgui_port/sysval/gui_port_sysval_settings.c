/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Device settings for the simulator.
 *
 * A PC has no backlight, speaker or vibration motor that maps onto these,
 * so each one is held in a variable: a write is stored and the next read
 * returns it. That is deliberately more than a stub returning a constant,
 * because it makes the read-write round trip testable -- a settings screen
 * with sliders and switches can be exercised end to end on the simulator,
 * and only the effect on hardware is missing.
 *
 * Values do not persist across a restart. Adding that would mean choosing a
 * config file location, which belongs to the application rather than here.
 */

#include <stddef.h>
#include "gui_components_init.h"
#include "gui_sysval_keys.h"
#include "gui_sysval_provider.h"

static int32_t sysval_brightness = 80;
static int32_t sysval_volume = 50;
static bool sysval_mute;
static bool sysval_vibrate = true;

static void sysval_complete_int(gui_sysval_complete_cb_t complete,
                                void *request_context,
                                int32_t number)
{
    gui_sysval_value_t value = gui_sysval_value_int(number);

    complete(request_context, GUI_SYSVAL_STATUS_OK, &value);
}

static void sysval_complete_bool(gui_sysval_complete_cb_t complete,
                                 void *request_context,
                                 bool state)
{
    gui_sysval_value_t value = gui_sysval_value_bool(state);

    complete(request_context, GUI_SYSVAL_STATUS_OK, &value);
}

static bool sysval_percent_valid(int32_t number)
{
    return number >= GUI_SYSVAL_PERCENT_MIN &&
           number <= GUI_SYSVAL_PERCENT_MAX;
}

static bool port_sysval_brightness_get(gui_sysval_complete_cb_t complete,
                                       void *request_context)
{
    sysval_complete_int(complete, request_context, sysval_brightness);
    return true;
}

static bool port_sysval_brightness_set(const gui_sysval_value_t *value,
                                       gui_sysval_complete_cb_t complete,
                                       void *request_context)
{
    if (!sysval_percent_valid(value->data.i))
    {
        complete(request_context, GUI_SYSVAL_STATUS_INVALID_VALUE, NULL);
        return true;
    }

    sysval_brightness = value->data.i;
    sysval_complete_int(complete, request_context, sysval_brightness);
    return true;
}

static bool port_sysval_volume_get(gui_sysval_complete_cb_t complete,
                                   void *request_context)
{
    sysval_complete_int(complete, request_context, sysval_volume);
    return true;
}

static bool port_sysval_volume_set(const gui_sysval_value_t *value,
                                   gui_sysval_complete_cb_t complete,
                                   void *request_context)
{
    if (!sysval_percent_valid(value->data.i))
    {
        complete(request_context, GUI_SYSVAL_STATUS_INVALID_VALUE, NULL);
        return true;
    }

    sysval_volume = value->data.i;

    /* Raising the volume clears mute, which is what a volume key does. */
    if (sysval_volume > 0)
    {
        sysval_mute = false;
    }

    sysval_complete_int(complete, request_context, sysval_volume);
    return true;
}

static bool port_sysval_mute_get(gui_sysval_complete_cb_t complete,
                                 void *request_context)
{
    sysval_complete_bool(complete, request_context, sysval_mute);
    return true;
}

static bool port_sysval_mute_set(const gui_sysval_value_t *value,
                                 gui_sysval_complete_cb_t complete,
                                 void *request_context)
{
    sysval_mute = value->data.b;
    sysval_complete_bool(complete, request_context, sysval_mute);
    return true;
}

static bool port_sysval_vibrate_get(gui_sysval_complete_cb_t complete,
                                    void *request_context)
{
    sysval_complete_bool(complete, request_context, sysval_vibrate);
    return true;
}

static bool port_sysval_vibrate_set(const gui_sysval_value_t *value,
                                    gui_sysval_complete_cb_t complete,
                                    void *request_context)
{
    sysval_vibrate = value->data.b;
    sysval_complete_bool(complete, request_context, sysval_vibrate);
    return true;
}

static gui_sysval_handler_t brightness_handler =
{
    .key = GUI_SYSVAL_KEY_DISPLAY_BRIGHTNESS,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_brightness_get,
    .set_request = port_sysval_brightness_set,
    .next = NULL,
};

static gui_sysval_handler_t volume_handler =
{
    .key = GUI_SYSVAL_KEY_AUDIO_VOLUME,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_volume_get,
    .set_request = port_sysval_volume_set,
    .next = NULL,
};

static gui_sysval_handler_t mute_handler =
{
    .key = GUI_SYSVAL_KEY_AUDIO_MUTED,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_mute_get,
    .set_request = port_sysval_mute_set,
    .next = NULL,
};

static gui_sysval_handler_t vibrate_handler =
{
    .key = GUI_SYSVAL_KEY_HAPTICS_ENABLED,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_vibrate_get,
    .set_request = port_sysval_vibrate_set,
    .next = NULL,
};

static int port_sysval_settings_init(void)
{
    if (!gui_sysval_handler_register(&brightness_handler) ||
        !gui_sysval_handler_register(&volume_handler) ||
        !gui_sysval_handler_register(&mute_handler) ||
        !gui_sysval_handler_register(&vibrate_handler))
    {
        return -1;
    }

    return 0;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_settings_init);
