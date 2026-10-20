/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Clock values for the simulator, read from the host clock.
 *
 * system/time is read-only here: the simulator does not change the host
 * clock, so a set is rejected with GUI_SYSVAL_REQUEST_INVALID.
 */

#include <time.h>
#include "gui_components_init.h"
#include "gui_sysval_adapter.h"

static void port_sysval_time_get(gui_sysval_request_t *request)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    gui_sysval_value_t value;

    value = gui_sysval_value_int(t->tm_hour * 3600 + t->tm_min * 60 + t->tm_sec);
    gui_sysval_respond(request, GUI_SYSVAL_STATUS_OK, &value);
}

static void port_sysval_date_get(gui_sysval_request_t *request)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    gui_sysval_value_t value;

    value = gui_sysval_value_int((t->tm_year + 1900) * 10000 +
                                 (t->tm_mon + 1) * 100 + t->tm_mday);
    gui_sysval_respond(request, GUI_SYSVAL_STATUS_OK, &value);
}

static gui_sysval_adapter_t time_adapter =
{
    .key = GUI_SYSVAL_KEY_SYSTEM_TIME,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_time_get,
};

static gui_sysval_adapter_t date_adapter =
{
    .key = GUI_SYSVAL_KEY_SYSTEM_DATE,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_date_get,
};

static int port_sysval_time_init(void)
{
    gui_sysval_adapter_register(&time_adapter);
    gui_sysval_adapter_register(&date_adapter);
    return 0;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_time_init);
