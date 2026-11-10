/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Example: a value the port only gets later, such as a BLE round trip.
 *
 * get_request just saves the request. A worker thread plays the driver: it
 * waits, then answers from its own thread. HoneyGUI hands the answer to the
 * UI on the GUI thread.
 *
 *     sim/async/value   int, read-only, answered about 500 ms later
 */

#include "gui_api_os.h"
#include "gui_components_init.h"
#include "gui_sysval_adapter.h"

#define SYSVAL_KEY_ASYNC_VALUE "sim/async/value"
#define SYSVAL_ASYNC_DELAY_MS  500

static int32_t async_value = 42;

/* Request waiting for the "driver". Written by get_request, taken by the
 * worker. */
static gui_sysval_request_t *volatile async_request;

static void port_sysval_async_get(gui_sysval_request_t *request)
{
    /* One request at a time. */
    if (async_request != NULL)
    {
        gui_sysval_respond(request, GUI_SYSVAL_STATUS_BUSY, NULL);
        return;
    }

    async_request = request;
}

/* Worker thread: the "driver". */
static void async_worker(void *param)
{
    GUI_UNUSED(param);

    while (1)
    {
        gui_thread_mdelay(SYSVAL_ASYNC_DELAY_MS);

        if (async_request != NULL)
        {
            gui_sysval_request_t *request = async_request;
            gui_sysval_value_t value = gui_sysval_value_int(async_value);

            async_request = NULL;
            gui_sysval_respond(request, GUI_SYSVAL_STATUS_OK, &value);
        }
    }
}

static gui_sysval_adapter_t async_value_adapter =
{
    .key = SYSVAL_KEY_ASYNC_VALUE,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_async_get,
    .set_request = NULL,
};

static int port_sysval_async_init(void)
{
    if (!gui_sysval_adapter_register(&async_value_adapter))
    {
        return -1;
    }

    gui_thread_create("sysval_async", async_worker, NULL, 1024, 5);
    return 0;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_async_init);
