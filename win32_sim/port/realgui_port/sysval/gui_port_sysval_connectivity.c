/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Connectivity system values for the simulator.
 *
 * A PC has no controller these can drive, so the enable flags are held in
 * variables. What is worth simulating is the timing: on real hardware,
 * turning a radio on takes hundreds of milliseconds, and the request
 * completes long after the call that started it returned.
 *
 * So a write here does not complete inline. It arms an SDL timer and
 * completes from that timer's thread, which exercises the path a real
 * driver uses -- including the handoff to the GUI server thread inside
 * gui_sysval.c. A UI that only ever saw instant completions on the
 * simulator would hide exactly the ordering bugs this design exists to
 * prevent, such as assuming a value has changed the moment set returns.
 *
 * Read-only values that depend on a radio being present are not faked:
 * signal strength has no host equivalent, and inventing a number would make
 * a signal bar look functional while showing nothing real.
 */

#include <SDL.h>
#include "gui_components_init.h"
#include "gui_sysval_keys.h"
#include "gui_sysval_provider.h"

/** Time a radio takes to come up, matching the order of real hardware. */
#define SYSVAL_RADIO_SWITCH_MS 400

/** One radio's state and the write currently being applied to it. */
typedef struct
{
    bool enabled;
    bool connected;

    /* Pending write. The timer callback owns these until it fires. */
    SDL_TimerID timer;
    bool pending_value;
    gui_sysval_complete_cb_t complete;
    void *request_context;
} sysval_radio_t;

static sysval_radio_t sysval_bt = { true, true, 0, false, NULL, NULL };
static sysval_radio_t sysval_wifi = { false, false, 0, false, NULL, NULL };

static void sysval_complete_bool(gui_sysval_complete_cb_t complete,
                                 void *request_context,
                                 bool state)
{
    gui_sysval_value_t value = gui_sysval_value_bool(state);

    complete(request_context, GUI_SYSVAL_STATUS_OK, &value);
}

/**
 * @brief Finish a radio write once its simulated delay has elapsed.
 *
 * Runs on the SDL timer thread, not the GUI server thread. Completing from
 * here is the point of the exercise: gui_sysval.c is responsible for moving
 * the response onto the server thread, so this also checks that path.
 *
 * @return 0 so SDL does not reschedule this one-shot timer.
 */
static Uint32 sysval_radio_apply(Uint32 interval, void *param)
{
    sysval_radio_t *radio = (sysval_radio_t *)param;
    gui_sysval_complete_cb_t complete = radio->complete;
    void *request_context = radio->request_context;

    (void)interval;

    radio->enabled = radio->pending_value;

    /* Switching a radio off drops its link; switching on does not create
     * one, since association would take another round trip. */
    if (!radio->enabled)
    {
        radio->connected = false;
    }

    radio->timer = 0;
    radio->complete = NULL;
    radio->request_context = NULL;

    if (complete != NULL)
    {
        sysval_complete_bool(complete, request_context, radio->enabled);
    }

    return 0;
}

/**
 * @brief Start switching a radio on or off.
 */
static bool sysval_radio_set(sysval_radio_t *radio,
                             const gui_sysval_value_t *value,
                             gui_sysval_complete_cb_t complete,
                             void *request_context)
{
    bool enable = value->data.b;

    /* One write at a time, as a driver with a single command queue would
     * behave. Without this the second write would overwrite the first
     * request's context and its caller would never hear back. */
    if (radio->timer != 0)
    {
        complete(request_context, GUI_SYSVAL_STATUS_BUSY, NULL);
        return true;
    }

    /* Already in the requested state: nothing to wait for. */
    if (radio->enabled == enable)
    {
        sysval_complete_bool(complete, request_context, radio->enabled);
        return true;
    }

    radio->pending_value = enable;
    radio->complete = complete;
    radio->request_context = request_context;

    radio->timer = SDL_AddTimer(SYSVAL_RADIO_SWITCH_MS, sysval_radio_apply,
                                radio);
    if (radio->timer == 0)
    {
        radio->complete = NULL;
        radio->request_context = NULL;
        complete(request_context, GUI_SYSVAL_STATUS_ERROR, NULL);
        return true;
    }

    return true;
}

static bool port_sysval_bt_enable_get(gui_sysval_complete_cb_t complete,
                                      void *request_context)
{
    sysval_complete_bool(complete, request_context, sysval_bt.enabled);
    return true;
}

static bool port_sysval_bt_enable_set(const gui_sysval_value_t *value,
                                      gui_sysval_complete_cb_t complete,
                                      void *request_context)
{
    return sysval_radio_set(&sysval_bt, value, complete, request_context);
}

static bool port_sysval_bt_connected_get(gui_sysval_complete_cb_t complete,
                                         void *request_context)
{
    sysval_complete_bool(complete, request_context, sysval_bt.connected);
    return true;
}

static bool port_sysval_wifi_enable_get(gui_sysval_complete_cb_t complete,
                                        void *request_context)
{
    sysval_complete_bool(complete, request_context, sysval_wifi.enabled);
    return true;
}

static bool port_sysval_wifi_enable_set(const gui_sysval_value_t *value,
                                        gui_sysval_complete_cb_t complete,
                                        void *request_context)
{
    return sysval_radio_set(&sysval_wifi, value, complete, request_context);
}

static bool port_sysval_wifi_connected_get(gui_sysval_complete_cb_t complete,
                                           void *request_context)
{
    sysval_complete_bool(complete, request_context, sysval_wifi.connected);
    return true;
}

static gui_sysval_handler_t bt_enable_handler =
{
    .key = GUI_SYSVAL_KEY_CONNECTIVITY_BLUETOOTH_ENABLED,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_bt_enable_get,
    .set_request = port_sysval_bt_enable_set,
    .next = NULL,
};

static gui_sysval_handler_t bt_connected_handler =
{
    .key = GUI_SYSVAL_KEY_CONNECTIVITY_BLUETOOTH_CONNECTED,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_bt_connected_get,
    .set_request = NULL,
    .next = NULL,
};

static gui_sysval_handler_t wifi_enable_handler =
{
    .key = GUI_SYSVAL_KEY_CONNECTIVITY_WIFI_ENABLED,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_wifi_enable_get,
    .set_request = port_sysval_wifi_enable_set,
    .next = NULL,
};

static gui_sysval_handler_t wifi_connected_handler =
{
    .key = GUI_SYSVAL_KEY_CONNECTIVITY_WIFI_CONNECTED,
    .type = GUI_SYSVAL_TYPE_BOOL,
    .get_request = port_sysval_wifi_connected_get,
    .set_request = NULL,
    .next = NULL,
};

static int port_sysval_connectivity_init(void)
{
    /* sdl_driver_init() brings up video and audio only. Safe to call again
     * for a subsystem that is already up. */
    if (SDL_InitSubSystem(SDL_INIT_TIMER) < 0)
    {
        return -1;
    }

    if (!gui_sysval_handler_register(&bt_enable_handler) ||
        !gui_sysval_handler_register(&bt_connected_handler) ||
        !gui_sysval_handler_register(&wifi_enable_handler) ||
        !gui_sysval_handler_register(&wifi_connected_handler))
    {
        return -1;
    }

    return 0;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_connectivity_init);
