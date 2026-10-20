/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Health values for the simulator. Fixed readings; the measuring flags are
 * stored so the UI can start and stop a measurement. A device port drives
 * the sensor instead.
 */

#include <stddef.h>
#include "gui_components_init.h"
#include "gui_sysval_adapter.h"

static int32_t heart_rate_bpm = 72;
static bool heart_rate_measuring = false;
static int32_t spo2_percent = 97;
static bool spo2_measuring = false;
static int32_t sleep_deep_min = 96;
static int32_t sleep_light_min = 260;

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

static void port_sysval_heart_rate_get(gui_sysval_request_t *request)
{
    respond_int(request, heart_rate_bpm);
}

static void port_sysval_heart_rate_measuring_get(gui_sysval_request_t *request)
{
    respond_bool(request, heart_rate_measuring);
}

static void port_sysval_heart_rate_measuring_set(const gui_sysval_value_t *value,
                                                 gui_sysval_request_t *request)
{
    heart_rate_measuring = value->data.b;
    respond_bool(request, heart_rate_measuring);
}

static void port_sysval_spo2_get(gui_sysval_request_t *request)
{
    respond_int(request, spo2_percent);
}

static void port_sysval_spo2_measuring_get(gui_sysval_request_t *request)
{
    respond_bool(request, spo2_measuring);
}

static void port_sysval_spo2_measuring_set(const gui_sysval_value_t *value,
                                           gui_sysval_request_t *request)
{
    spo2_measuring = value->data.b;
    respond_bool(request, spo2_measuring);
}

static void port_sysval_sleep_deep_get(gui_sysval_request_t *request)
{
    respond_int(request, sleep_deep_min);
}

static void port_sysval_sleep_light_get(gui_sysval_request_t *request)
{
    respond_int(request, sleep_light_min);
}

static gui_sysval_adapter_t heart_rate_adapter =
{
    .key = GUI_SYSVAL_KEY_HEALTH_HEART_RATE,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_heart_rate_get,
};

static gui_sysval_adapter_t heart_rate_measuring_adapter =
{
    .key = GUI_SYSVAL_KEY_HEALTH_HEART_RATE_MEASURING,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_heart_rate_measuring_get,
    .set_request = port_sysval_heart_rate_measuring_set,
};

static gui_sysval_adapter_t spo2_adapter =
{
    .key = GUI_SYSVAL_KEY_HEALTH_SPO2,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_spo2_get,
};

static gui_sysval_adapter_t spo2_measuring_adapter =
{
    .key = GUI_SYSVAL_KEY_HEALTH_SPO2_MEASURING,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_spo2_measuring_get,
    .set_request = port_sysval_spo2_measuring_set,
};

static gui_sysval_adapter_t sleep_deep_adapter =
{
    .key = GUI_SYSVAL_KEY_HEALTH_SLEEP_DEEP,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_sleep_deep_get,
};

static gui_sysval_adapter_t sleep_light_adapter =
{
    .key = GUI_SYSVAL_KEY_HEALTH_SLEEP_LIGHT,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_sleep_light_get,
};

static int port_sysval_health_init(void)
{
    gui_sysval_adapter_register(&heart_rate_adapter);
    gui_sysval_adapter_register(&heart_rate_measuring_adapter);
    gui_sysval_adapter_register(&spo2_adapter);
    gui_sysval_adapter_register(&spo2_measuring_adapter);
    gui_sysval_adapter_register(&sleep_deep_adapter);
    gui_sysval_adapter_register(&sleep_light_adapter);
    return 0;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_health_init);
