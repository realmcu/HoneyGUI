/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <time.h>
#include "gui_components_init.h"
#include "gui_sysval_provider.h"

#define SYSVAL_KEY_TIME "system/time"

static bool port_sysval_time_get(gui_sysval_complete_cb_t complete,
                                 void *request_context)
{
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);
    char value[16];

    if (now == (time_t) - 1 || local_time == NULL)
    {
        complete(request_context, GUI_SYSVAL_STATUS_ERROR, NULL);
        return true;
    }

    snprintf(value, sizeof(value), "%02d:%02d:%02d",
             local_time->tm_hour,
             local_time->tm_min,
             local_time->tm_sec);

    complete(request_context, GUI_SYSVAL_STATUS_OK, value);
    return true;
}

static gui_sysval_handler_t time_handler =
{
    .key = SYSVAL_KEY_TIME,
    .get_request = port_sysval_time_get,
    .set_request = NULL,
    .next = NULL,
};

static int port_sysval_time_init(void)
{
    return gui_sysval_handler_register(&time_handler) ? 0 : -1;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_time_init);
