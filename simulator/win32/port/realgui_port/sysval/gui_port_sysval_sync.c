/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Example: a value the port has at hand. Answer inside get_request.
 *
 *     sim/sync/value   int, read-only
 */

#include <stddef.h>
#include "gui_components_init.h"
#include "gui_sysval_adapter.h"

#define SYSVAL_KEY_SYNC_VALUE "sim/sync/value"

static int32_t sync_value = 100;

static void port_sysval_sync_get(gui_sysval_request_t *request)
{
    gui_sysval_value_t value = gui_sysval_value_int(sync_value);

    gui_sysval_respond(request, GUI_SYSVAL_STATUS_OK, &value);
}

static gui_sysval_adapter_t sync_value_adapter =
{
    .key = SYSVAL_KEY_SYNC_VALUE,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_sync_get,
    .set_request = NULL,
};

static int port_sysval_sync_init(void)
{
    return gui_sysval_adapter_register(&sync_value_adapter) ? 0 : -1;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_sync_init);
