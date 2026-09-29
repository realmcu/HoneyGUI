/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Pedometer system values for the simulator.
 *
 * Modelled on the RTL8762D wristband firmware, where the UI reads
 * RtkWristbandSys.gPedoData: global_steps, global_distance, global_calorys
 * and daily_step_target. A step counter is the one system value a watch UI
 * reads on nearly every screen -- watch face, activity ring, history chart.
 *
 * Two differences from that firmware are deliberate.
 *
 * It exposes converted units. The firmware stores raw accumulator values
 * and each screen divides: "global_calorys / 4000" for kcal,
 * "global_distance / 1600" for distance. Those divisors are a property of
 * the sensor, not of a screen, so they belong on this side of the boundary.
 * A UI that has to know them is coupled to the hardware it is drawing.
 *
 * The step count advances while the simulator runs. A frozen counter would
 * let a progress ring or a "target reached" transition look correct without
 * ever being exercised; a moving one makes those paths testable without
 * hardware.
 */

#include <SDL.h>
#include "gui_components_init.h"
#include "gui_sysval_keys.h"
#include "gui_sysval_provider.h"

/** Steps already walked when the simulator starts, so a ring is partly full. */
#define SYSVAL_STEPS_INITIAL   3200

/** Simulated walking pace. One step per interval. */
#define SYSVAL_STEP_INTERVAL_MS 600

/** Default daily goal, matching DEFAULT_STEP_TASK in the wristband firmware. */
#define SYSVAL_STEP_TARGET_DEFAULT 10000

/*
 * Stride and burn rate per step, in centimetres and in calories scaled by
 * 1000. Chosen so that a 10000 step day lands near 7 km and 400 kcal, which
 * is what a wristband of this class reports for an adult.
 */
#define SYSVAL_CM_PER_STEP      70
#define SYSVAL_MCAL_PER_STEP    40

static uint32_t sysval_step_target = SYSVAL_STEP_TARGET_DEFAULT;

/** Simulator start time, used to derive the step count. */
static Uint32 sysval_step_epoch;

/**
 * @brief Current step count.
 *
 * Derived from elapsed time rather than incremented by a timer: there is no
 * count to lose if a frame is slow, and it needs no lock even though reads
 * arrive on the GUI server thread.
 */
static uint32_t sysval_steps_get(void)
{
    Uint32 elapsed = SDL_GetTicks() - sysval_step_epoch;

    return SYSVAL_STEPS_INITIAL + (uint32_t)(elapsed / SYSVAL_STEP_INTERVAL_MS);
}

static void sysval_complete_uint(gui_sysval_complete_cb_t complete,
                                 void *request_context,
                                 uint32_t number)
{
    gui_sysval_value_t value = gui_sysval_value_int((int32_t)number);

    complete(request_context, GUI_SYSVAL_STATUS_OK, &value);
}

static bool port_sysval_steps_get(gui_sysval_complete_cb_t complete,
                                  void *request_context)
{
    sysval_complete_uint(complete, request_context, sysval_steps_get());
    return true;
}

/* Metres walked, so the UI formats the unit it wants to show. */
static bool port_sysval_distance_get(gui_sysval_complete_cb_t complete,
                                     void *request_context)
{
    uint32_t metres = sysval_steps_get() * SYSVAL_CM_PER_STEP / 100u;

    sysval_complete_uint(complete, request_context, metres);
    return true;
}

/* Whole kilocalories, which is the unit a watch displays. */
static bool port_sysval_calories_get(gui_sysval_complete_cb_t complete,
                                     void *request_context)
{
    uint32_t kcal = sysval_steps_get() * SYSVAL_MCAL_PER_STEP / 1000u;

    sysval_complete_uint(complete, request_context, kcal);
    return true;
}

static bool port_sysval_step_target_get(gui_sysval_complete_cb_t complete,
                                        void *request_context)
{
    sysval_complete_uint(complete, request_context, sysval_step_target);
    return true;
}

/*
 * Writable because the goal is a user setting: in the wristband firmware it
 * arrives from the phone app and is restored from FTL at boot. The step
 * count itself has no setter, since it comes from the sensor.
 */
static bool port_sysval_step_target_set(const gui_sysval_value_t *value,
                                        gui_sysval_complete_cb_t complete,
                                        void *request_context)
{
    int32_t target = value->data.i;

    if (target < GUI_SYSVAL_STEP_TARGET_MIN ||
        target > GUI_SYSVAL_STEP_TARGET_MAX)
    {
        complete(request_context, GUI_SYSVAL_STATUS_INVALID_VALUE, NULL);
        return true;
    }

    sysval_step_target = (uint32_t)target;
    sysval_complete_uint(complete, request_context, sysval_step_target);
    return true;
}

static gui_sysval_handler_t steps_handler =
{
    .key = GUI_SYSVAL_KEY_HEALTH_PEDOMETER_STEPS,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_steps_get,
    .set_request = NULL,
    .next = NULL,
};

static gui_sysval_handler_t distance_handler =
{
    .key = GUI_SYSVAL_KEY_HEALTH_PEDOMETER_DISTANCE,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_distance_get,
    .set_request = NULL,
    .next = NULL,
};

static gui_sysval_handler_t calories_handler =
{
    .key = GUI_SYSVAL_KEY_HEALTH_PEDOMETER_CALORIES,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_calories_get,
    .set_request = NULL,
    .next = NULL,
};

static gui_sysval_handler_t step_target_handler =
{
    .key = GUI_SYSVAL_KEY_HEALTH_PEDOMETER_STEP_TARGET,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_step_target_get,
    .set_request = port_sysval_step_target_set,
    .next = NULL,
};

static int port_sysval_pedometer_init(void)
{
    sysval_step_epoch = SDL_GetTicks();

    if (!gui_sysval_handler_register(&steps_handler) ||
        !gui_sysval_handler_register(&distance_handler) ||
        !gui_sysval_handler_register(&calories_handler) ||
        !gui_sysval_handler_register(&step_target_handler))
    {
        return -1;
    }

    return 0;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_pedometer_init);
