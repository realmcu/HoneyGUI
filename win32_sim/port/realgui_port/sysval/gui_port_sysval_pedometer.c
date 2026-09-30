/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Pedometer values for the simulator. Fixed values, except the step target,
 * which the UI can set. A device port reads the step counter.
 */

#include <stddef.h>
#include "gui_components_init.h"
#include "gui_sysval_adapter.h"

static int32_t steps = 3200;
static int32_t distance_m = 2240;
static int32_t calories_kcal = 128;
static int32_t step_target = 10000;

static void respond_int(gui_sysval_request_t *request, int32_t i)
{
    gui_sysval_value_t value = gui_sysval_value_int(i);

    gui_sysval_respond(request, GUI_SYSVAL_STATUS_OK, &value);
}

static void port_sysval_steps_get(gui_sysval_request_t *request)
{
    respond_int(request, steps);
}

static void port_sysval_distance_get(gui_sysval_request_t *request)
{
    respond_int(request, distance_m);
}

static void port_sysval_calories_get(gui_sysval_request_t *request)
{
    respond_int(request, calories_kcal);
}

static void port_sysval_step_target_get(gui_sysval_request_t *request)
{
    respond_int(request, step_target);
}

static void port_sysval_step_target_set(const gui_sysval_value_t *value,
                                        gui_sysval_request_t *request)
{
    /* Range from gui_sysval_keys.h: health/pedometer/step_target. */
    if (value->data.i < 1000 || value->data.i > 100000)
    {
        gui_sysval_respond(request, GUI_SYSVAL_STATUS_INVALID_VALUE, NULL);
        return;
    }

    step_target = value->data.i;
    respond_int(request, step_target);
}

static gui_sysval_adapter_t steps_adapter =
{
    .key = GUI_SYSVAL_KEY_HEALTH_PEDOMETER_STEPS,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_steps_get,
};

static gui_sysval_adapter_t distance_adapter =
{
    .key = GUI_SYSVAL_KEY_HEALTH_PEDOMETER_DISTANCE,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_distance_get,
};

static gui_sysval_adapter_t calories_adapter =
{
    .key = GUI_SYSVAL_KEY_HEALTH_PEDOMETER_CALORIES,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_calories_get,
};

static gui_sysval_adapter_t step_target_adapter =
{
    .key = GUI_SYSVAL_KEY_HEALTH_PEDOMETER_STEP_TARGET,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_step_target_get,
    .set_request = port_sysval_step_target_set,
};

static int port_sysval_pedometer_init(void)
{
    gui_sysval_adapter_register(&steps_adapter);
    gui_sysval_adapter_register(&distance_adapter);
    gui_sysval_adapter_register(&calories_adapter);
    gui_sysval_adapter_register(&step_target_adapter);
    return 0;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_pedometer_init);
