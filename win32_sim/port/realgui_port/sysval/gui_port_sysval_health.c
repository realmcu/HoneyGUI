/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Health sensor values for the simulator: heart rate, blood oxygen, sleep.
 *
 * Modelled on the RTL8762D wristband firmware, where heart rate and blood
 * oxygen are not always-on like the pedometer but measured on demand: a
 * screen starts hal_hrs_timer() / hal_bos_timer(), waits while the sensor
 * runs, and receives a value back. So each comes as two things, a
 * measurement session and a result.
 *
 *     health/heart_rate/measuring  bool, read-write - session state
 *     health/heart_rate            bpm,  read-only  - most recent result
 *     health/spo2/measuring        bool, read-write
 *     health/spo2                  %,    read-only
 *
 * Setting a measuring key to true starts a session and completes immediately
 * with the applied state. The result key changes after the simulated sensor
 * delay. Setting measuring to false stops the session and drops any stale
 * value.
 *
 * Sleep is read-only daily history, in minutes, as in T_SYNC_SLP where deep
 * and light are bitfields.
 */

#include <SDL.h>
#include "gui_components_init.h"
#include "gui_server.h"
#include "gui_sysval_keys.h"
#include "gui_sysval_provider.h"

/** How long a simulated measurement runs before a value appears. */
#define SYSVAL_MEASURE_MS  2600

#define SYSVAL_HR_BASE     72
#define SYSVAL_HR_SPREAD   18
#define SYSVAL_SPO2_BASE   97
#define SYSVAL_SPO2_SPREAD 2

/* Today's sleep, whole minutes, as the firmware reports a typical night. */
#define SYSVAL_SLEEP_DEEP  96
#define SYSVAL_SLEEP_LIGHT 260

/** One simulated sensor and its current session. */
typedef struct
{
    bool measuring;     /* true while the sensor is running */
    Uint32 started_at;  /* 0 when no session */
    int result;         /* -1 = no valid result yet */
    Uint32 measure_ms;
    int base;
    int spread;
} sysval_measure_t;

static sysval_measure_t sysval_hr =
{
    false, 0, -1, SYSVAL_MEASURE_MS, SYSVAL_HR_BASE, SYSVAL_HR_SPREAD
};
static sysval_measure_t sysval_spo2 =
{
    false, 0, -1, SYSVAL_MEASURE_MS, SYSVAL_SPO2_BASE, SYSVAL_SPO2_SPREAD
};

/** Pseudo-random within +-spread of base, keyed by a tick so it wiggles. */
static int sysval_reading(int base, int spread)
{
    return base + (int)(((SDL_GetTicks() * 2654435761u) >> 24) %
                        (uint32_t)(2 * spread + 1)) - spread;
}

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

static void sysval_measure_update(sysval_measure_t *m, Uint32 now)
{
    if (!m->measuring ||
        !SDL_TICKS_PASSED(now, m->started_at + m->measure_ms))
    {
        return;
    }

    m->result = sysval_reading(m->base, m->spread);
    m->measuring = false;
    m->started_at = 0;
}

static void port_sysval_health_tick(void)
{
    Uint32 now = SDL_GetTicks();

    sysval_measure_update(&sysval_hr, now);
    sysval_measure_update(&sysval_spo2, now);
}

static bool sysval_measure_get(sysval_measure_t *m,
                               gui_sysval_complete_cb_t complete,
                               void *request_context)
{
    /* No valid reading yet: report it, not a made-up number. */
    if (m->result < 0)
    {
        complete(request_context, GUI_SYSVAL_STATUS_ERROR, NULL);
        return true;
    }

    sysval_complete_int(complete, request_context, m->result);
    return true;
}

/**
 * @brief Start or stop a measurement.
 *
 * The property write completes as soon as the state changes. The result is
 * updated later by the GUI server hook.
 */
static bool sysval_measure_set(sysval_measure_t *m,
                               const gui_sysval_value_t *value,
                               gui_sysval_complete_cb_t complete,
                               void *request_context)
{
    /* Stop: invalidate any stale value so it cannot be mistaken for live. */
    if (!value->data.b)
    {
        m->result = -1;
        m->measuring = false;
        m->started_at = 0;
        sysval_complete_bool(complete, request_context, false);
        return true;
    }

    /* Setting an already active state is an idempotent property write. */
    if (m->measuring)
    {
        sysval_complete_bool(complete, request_context, true);
        return true;
    }

    m->measuring = true;
    m->started_at = SDL_GetTicks();
    sysval_complete_bool(complete, request_context, true);
    return true;
}

/* ---- heart rate ---- */

static bool port_sysval_hr_get(gui_sysval_complete_cb_t complete,
                               void *request_context)
{
    return sysval_measure_get(&sysval_hr, complete, request_context);
}

static bool port_sysval_hr_measuring_get(gui_sysval_complete_cb_t complete,
                                         void *request_context)
{
    sysval_complete_bool(complete, request_context, sysval_hr.measuring);
    return true;
}

static bool port_sysval_hr_measuring_set(const gui_sysval_value_t *value,
                                         gui_sysval_complete_cb_t complete,
                                         void *request_context)
{
    return sysval_measure_set(&sysval_hr, value, complete, request_context);
}

/* ---- blood oxygen ---- */

static bool port_sysval_spo2_get(gui_sysval_complete_cb_t complete,
                                 void *request_context)
{
    return sysval_measure_get(&sysval_spo2, complete, request_context);
}

static bool port_sysval_spo2_measuring_get(gui_sysval_complete_cb_t complete,
                                           void *request_context)
{
    sysval_complete_bool(complete, request_context, sysval_spo2.measuring);
    return true;
}

static bool port_sysval_spo2_measuring_set(const gui_sysval_value_t *value,
                                           gui_sysval_complete_cb_t complete,
                                           void *request_context)
{
    return sysval_measure_set(&sysval_spo2, value, complete, request_context);
}

/* ---- sleep: read-only daily history ---- */

static bool port_sysval_sleep_deep_get(gui_sysval_complete_cb_t complete,
                                       void *request_context)
{
    sysval_complete_int(complete, request_context, SYSVAL_SLEEP_DEEP);
    return true;
}

static bool port_sysval_sleep_light_get(gui_sysval_complete_cb_t complete,
                                        void *request_context)
{
    sysval_complete_int(complete, request_context, SYSVAL_SLEEP_LIGHT);
    return true;
}

static gui_sysval_handler_t heart_rate_handler =
{
    .key = GUI_SYSVAL_KEY_HEALTH_HEART_RATE,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_hr_get,
    .set_request = NULL,
    .next = NULL,
};

static gui_sysval_handler_t hr_measuring_handler =
{
    .key = GUI_SYSVAL_KEY_HEALTH_HEART_RATE_MEASURING,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_hr_measuring_get,
    .set_request = port_sysval_hr_measuring_set,
    .next = NULL,
};

static gui_sysval_handler_t spo2_handler =
{
    .key = GUI_SYSVAL_KEY_HEALTH_SPO2,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_spo2_get,
    .set_request = NULL,
    .next = NULL,
};

static gui_sysval_handler_t spo2_measuring_handler =
{
    .key = GUI_SYSVAL_KEY_HEALTH_SPO2_MEASURING,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_spo2_measuring_get,
    .set_request = port_sysval_spo2_measuring_set,
    .next = NULL,
};

static gui_sysval_handler_t sleep_deep_handler =
{
    .key = GUI_SYSVAL_KEY_HEALTH_SLEEP_DEEP,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_sleep_deep_get,
    .set_request = NULL,
    .next = NULL,
};

static gui_sysval_handler_t sleep_light_handler =
{
    .key = GUI_SYSVAL_KEY_HEALTH_SLEEP_LIGHT,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_sleep_light_get,
    .set_request = NULL,
    .next = NULL,
};

static int port_sysval_health_init(void)
{
    if (!gui_register_server_hook(port_sysval_health_tick))
    {
        return -1;
    }

    if (!gui_sysval_handler_register(&heart_rate_handler) ||
        !gui_sysval_handler_register(&hr_measuring_handler) ||
        !gui_sysval_handler_register(&spo2_handler) ||
        !gui_sysval_handler_register(&spo2_measuring_handler) ||
        !gui_sysval_handler_register(&sleep_deep_handler) ||
        !gui_sysval_handler_register(&sleep_light_handler))
    {
        return -1;
    }

    return 0;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_health_init);
