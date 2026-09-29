/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Clock system values for the simulator, backed by the host clock.
 *
 * Setting the time does not touch the host clock, which would need
 * privileges and would affect the whole machine. Instead a signed offset is
 * kept and added to every reading, so a UI that sets the clock sees the
 * change it asked for and the host is left alone.
 */

#include <time.h>
#include "gui_components_init.h"
#include "gui_sysval_keys.h"
#include "gui_sysval_provider.h"

/*
 * No separate weekday key: it is a function of the date, so a port that
 * served both would be offering the same fact twice and could contradict
 * itself. The UI derives it from GUI_SYSVAL_KEY_SYSTEM_DATE.
 */
/** Offset from the host clock, in seconds, applied to every reading. */
static long sysval_time_offset;

/**
 * @brief Read the simulated local time.
 *
 * @param out Receives the broken-down time.
 * @return true on success.
 */
static bool sysval_local_time(struct tm *out)
{
    time_t now = time(NULL);
    struct tm *local_time;

    if (now == (time_t) - 1)
    {
        return false;
    }

    now += (time_t)sysval_time_offset;

    local_time = localtime(&now);
    if (local_time == NULL)
    {
        return false;
    }

    *out = *local_time;
    return true;
}

static bool port_sysval_time_get(gui_sysval_complete_cb_t complete,
                                 void *request_context)
{
    struct tm local_time;
    gui_sysval_value_t value;

    if (!sysval_local_time(&local_time))
    {
        complete(request_context, GUI_SYSVAL_STATUS_ERROR, NULL);
        return true;
    }

    value = gui_sysval_value_int(local_time.tm_hour * 3600 +
                                 local_time.tm_min * 60 +
                                 local_time.tm_sec);
    complete(request_context, GUI_SYSVAL_STATUS_OK, &value);
    return true;
}

/*
 * Stores the offset that makes the clock read the requested second of
 * today. The host clock itself is left alone.
 */
static bool port_sysval_time_set(const gui_sysval_value_t *value,
                                 gui_sysval_complete_cb_t complete,
                                 void *request_context)
{
    struct tm local_time;
    struct tm target;
    int32_t seconds = value->data.i;
    time_t now;
    time_t target_time;
    gui_sysval_value_t applied;

    if (seconds < 0 || seconds > 86399)
    {
        complete(request_context, GUI_SYSVAL_STATUS_INVALID_VALUE, NULL);
        return true;
    }

    now = time(NULL);
    if (now == (time_t) - 1 || !sysval_local_time(&local_time))
    {
        complete(request_context, GUI_SYSVAL_STATUS_ERROR, NULL);
        return true;
    }

    /* Keep today's date, replace the time of day. */
    target = local_time;
    target.tm_hour = (int)(seconds / 3600);
    target.tm_min = (int)(seconds / 60 % 60);
    target.tm_sec = (int)(seconds % 60);
    target.tm_isdst = -1;

    target_time = mktime(&target);
    if (target_time == (time_t) - 1)
    {
        complete(request_context, GUI_SYSVAL_STATUS_ERROR, NULL);
        return true;
    }

    sysval_time_offset = (long)(target_time - now);

    /* Report what took effect. mktime() may have moved it across a DST
     * transition, so read it back from the normalised struct. */
    applied = gui_sysval_value_int(target.tm_hour * 3600 +
                                   target.tm_min * 60 +
                                   target.tm_sec);
    complete(request_context, GUI_SYSVAL_STATUS_OK, &applied);
    return true;
}

static bool port_sysval_date_get(gui_sysval_complete_cb_t complete,
                                 void *request_context)
{
    struct tm local_time;
    gui_sysval_value_t value;
    int year;
    int month;
    int day;

    if (!sysval_local_time(&local_time))
    {
        complete(request_context, GUI_SYSVAL_STATUS_ERROR, NULL);
        return true;
    }

    /* YYYYMMDD only reads back correctly while each field stays in its
     * calendar range. localtime() should never return anything else, which
     * is why a value outside it is reported as an error rather than
     * clamped. */
    year = local_time.tm_year + 1900;
    month = local_time.tm_mon + 1;
    day = local_time.tm_mday;

    if (year < 0 || year > 9999 ||
        month < 1 || month > 12 ||
        day < 1 || day > 31)
    {
        complete(request_context, GUI_SYSVAL_STATUS_ERROR, NULL);
        return true;
    }

    value = gui_sysval_value_int(year * 10000 + month * 100 + day);
    complete(request_context, GUI_SYSVAL_STATUS_OK, &value);
    return true;
}

static gui_sysval_handler_t time_handler =
{
    .key = GUI_SYSVAL_KEY_SYSTEM_TIME,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_time_get,
    .set_request = port_sysval_time_set,
    .next = NULL,
};

static gui_sysval_handler_t date_handler =
{
    .key = GUI_SYSVAL_KEY_SYSTEM_DATE,
    .type = GUI_SYSVAL_TYPE_INT,
    .get_request = port_sysval_date_get,
    .set_request = NULL,
    .next = NULL,
};

static int port_sysval_time_init(void)
{
    if (!gui_sysval_handler_register(&time_handler) ||
        !gui_sysval_handler_register(&date_handler))
    {
        return -1;
    }

    return 0;
}

GUI_INIT_DEVICE_EXPORT(port_sysval_time_init);
