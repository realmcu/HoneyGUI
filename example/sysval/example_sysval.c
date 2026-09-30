/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * System value demo.
 *
 * A one-second timer reads a few system values with gui_sysval_get_request()
 * and shows each one in its own text widget. Shortly after start-up, a
 * one-shot timer writes some values with gui_sysval_set_request(), so the
 * display changes and the log shows the result of each write.
 */

#include <stdio.h>
#include <string.h>
#include "gui_api.h"
#include "gui_components_init.h"
#include "gui_obj.h"
#include "gui_sysval.h"
#include "gui_text.h"

#define TEXT_SIZE      32
#define FONT_SIZE      24
#define LINE_HEIGHT    32
#define LINE_COUNT     10

#define REFRESH_MS     1000
#define WRITE_DELAY_MS 1500

/* gui_text_content_set() keeps the pointer, so each text needs its own
 * buffer that outlives the callback. */
static char time_buf[TEXT_SIZE];
static char date_buf[TEXT_SIZE];
static char battery_buf[TEXT_SIZE];
static char charging_buf[TEXT_SIZE];
static char brightness_buf[TEXT_SIZE];
static char wifi_buf[TEXT_SIZE];
static char steps_buf[TEXT_SIZE];
static char heart_rate_buf[TEXT_SIZE];
static char sync_buf[TEXT_SIZE];
static char async_buf[TEXT_SIZE];

/* Simulator-only keys from gui_port_sysval_sync.c and _async.c. */
#define SYSVAL_KEY_SYNC_VALUE  "sim/sync/value"
#define SYSVAL_KEY_ASYNC_VALUE "sim/async/value"

static void text_update(gui_text_t *text, char *buf)
{
    gui_text_content_set(text, buf, strlen(buf));
}


/*============================================================================*
 *                         Read callbacks
 *============================================================================*/

/* system/time is seconds since midnight. */
static void time_done(gui_sysval_request_id_t request_id,
                      gui_sysval_status_t status,
                      const gui_sysval_value_t *value,
                      void *user_data)
{
    GUI_UNUSED(request_id);

    if (status == GUI_SYSVAL_STATUS_OK && value != NULL)
    {
        int s = (int)value->data.i;

        snprintf(time_buf, TEXT_SIZE, "time: %02d:%02d:%02d",
                 s / 3600, s / 60 % 60, s % 60);
    }
    else
    {
        snprintf(time_buf, TEXT_SIZE, "time: --");
    }
    text_update((gui_text_t *)user_data, time_buf);
}

/* system/date is YYYYMMDD. */
static void date_done(gui_sysval_request_id_t request_id,
                      gui_sysval_status_t status,
                      const gui_sysval_value_t *value,
                      void *user_data)
{
    GUI_UNUSED(request_id);

    if (status == GUI_SYSVAL_STATUS_OK && value != NULL)
    {
        int d = (int)value->data.i;

        snprintf(date_buf, TEXT_SIZE, "date: %04d-%02d-%02d",
                 d / 10000, d / 100 % 100, d % 100);
    }
    else
    {
        snprintf(date_buf, TEXT_SIZE, "date: --");
    }
    text_update((gui_text_t *)user_data, date_buf);
}

static void battery_done(gui_sysval_request_id_t request_id,
                         gui_sysval_status_t status,
                         const gui_sysval_value_t *value,
                         void *user_data)
{
    GUI_UNUSED(request_id);

    if (status == GUI_SYSVAL_STATUS_OK && value != NULL)
    {
        snprintf(battery_buf, TEXT_SIZE, "battery: %d%%", (int)value->data.i);
    }
    else
    {
        snprintf(battery_buf, TEXT_SIZE, "battery: --");
    }
    text_update((gui_text_t *)user_data, battery_buf);
}

static void charging_done(gui_sysval_request_id_t request_id,
                          gui_sysval_status_t status,
                          const gui_sysval_value_t *value,
                          void *user_data)
{
    GUI_UNUSED(request_id);

    if (status == GUI_SYSVAL_STATUS_OK && value != NULL)
    {
        snprintf(charging_buf, TEXT_SIZE, "charging: %s",
                 value->data.b ? "yes" : "no");
    }
    else
    {
        snprintf(charging_buf, TEXT_SIZE, "charging: --");
    }
    text_update((gui_text_t *)user_data, charging_buf);
}

static void brightness_done(gui_sysval_request_id_t request_id,
                            gui_sysval_status_t status,
                            const gui_sysval_value_t *value,
                            void *user_data)
{
    GUI_UNUSED(request_id);

    if (status == GUI_SYSVAL_STATUS_OK && value != NULL)
    {
        snprintf(brightness_buf, TEXT_SIZE, "brightness: %d%%",
                 (int)value->data.i);
    }
    else
    {
        snprintf(brightness_buf, TEXT_SIZE, "brightness: --");
    }
    text_update((gui_text_t *)user_data, brightness_buf);
}

static void wifi_done(gui_sysval_request_id_t request_id,
                      gui_sysval_status_t status,
                      const gui_sysval_value_t *value,
                      void *user_data)
{
    GUI_UNUSED(request_id);

    if (status == GUI_SYSVAL_STATUS_OK && value != NULL)
    {
        snprintf(wifi_buf, TEXT_SIZE, "wifi: %s", value->data.b ? "on" : "off");
    }
    else
    {
        snprintf(wifi_buf, TEXT_SIZE, "wifi: --");
    }
    text_update((gui_text_t *)user_data, wifi_buf);
}

static void steps_done(gui_sysval_request_id_t request_id,
                       gui_sysval_status_t status,
                       const gui_sysval_value_t *value,
                       void *user_data)
{
    GUI_UNUSED(request_id);

    if (status == GUI_SYSVAL_STATUS_OK && value != NULL)
    {
        snprintf(steps_buf, TEXT_SIZE, "steps: %d", (int)value->data.i);
    }
    else
    {
        snprintf(steps_buf, TEXT_SIZE, "steps: --");
    }
    text_update((gui_text_t *)user_data, steps_buf);
}

static void heart_rate_done(gui_sysval_request_id_t request_id,
                            gui_sysval_status_t status,
                            const gui_sysval_value_t *value,
                            void *user_data)
{
    GUI_UNUSED(request_id);

    if (status == GUI_SYSVAL_STATUS_OK && value != NULL)
    {
        snprintf(heart_rate_buf, TEXT_SIZE, "heart rate: %d bpm",
                 (int)value->data.i);
    }
    else
    {
        snprintf(heart_rate_buf, TEXT_SIZE, "heart rate: --");
    }
    text_update((gui_text_t *)user_data, heart_rate_buf);
}

/* sim/sync/value is answered inside the port's get_request. */
static void sync_done(gui_sysval_request_id_t request_id,
                      gui_sysval_status_t status,
                      const gui_sysval_value_t *value,
                      void *user_data)
{
    GUI_UNUSED(request_id);

    if (status == GUI_SYSVAL_STATUS_OK && value != NULL)
    {
        snprintf(sync_buf, TEXT_SIZE, "sync: %d", (int)value->data.i);
    }
    else
    {
        snprintf(sync_buf, TEXT_SIZE, "sync: --");
    }
    text_update((gui_text_t *)user_data, sync_buf);
}

/* sim/async/value is answered about 500 ms later, from another thread. */
static void async_done(gui_sysval_request_id_t request_id,
                       gui_sysval_status_t status,
                       const gui_sysval_value_t *value,
                       void *user_data)
{
    GUI_UNUSED(request_id);

    if (status == GUI_SYSVAL_STATUS_OK && value != NULL)
    {
        snprintf(async_buf, TEXT_SIZE, "async: %d", (int)value->data.i);
    }
    else
    {
        snprintf(async_buf, TEXT_SIZE, "async: --");
    }
    text_update((gui_text_t *)user_data, async_buf);
}


/*============================================================================*
 *                         Reading
 *============================================================================*/

static gui_text_t *time_text;
static gui_text_t *date_text;
static gui_text_t *battery_text;
static gui_text_t *charging_text;
static gui_text_t *brightness_text;
static gui_text_t *wifi_text;
static gui_text_t *steps_text;
static gui_text_t *heart_rate_text;
static gui_text_t *sync_text;
static gui_text_t *async_text;

/* Timer callback: read every value once. Each result arrives on the GUI
 * thread, after this function has returned, in the callback passed here. */
static void refresh(void *obj)
{
    GUI_UNUSED(obj);

    gui_sysval_get_request(GUI_SYSVAL_KEY_SYSTEM_TIME,
                           time_done, time_text);
    gui_sysval_get_request(GUI_SYSVAL_KEY_SYSTEM_DATE,
                           date_done, date_text);
    gui_sysval_get_request(GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL,
                           battery_done, battery_text);
    gui_sysval_get_request(GUI_SYSVAL_KEY_POWER_BATTERY_CHARGING,
                           charging_done, charging_text);
    gui_sysval_get_request(GUI_SYSVAL_KEY_DISPLAY_BRIGHTNESS,
                           brightness_done, brightness_text);
    gui_sysval_get_request(GUI_SYSVAL_KEY_CONNECTIVITY_WIFI_ENABLED,
                           wifi_done, wifi_text);
    gui_sysval_get_request(GUI_SYSVAL_KEY_HEALTH_PEDOMETER_STEPS,
                           steps_done, steps_text);
    gui_sysval_get_request(GUI_SYSVAL_KEY_HEALTH_HEART_RATE,
                           heart_rate_done, heart_rate_text);
    gui_sysval_get_request(SYSVAL_KEY_SYNC_VALUE,
                           sync_done, sync_text);
    gui_sysval_get_request(SYSVAL_KEY_ASYNC_VALUE,
                           async_done, async_text);
}


/*============================================================================*
 *                         Writing
 *============================================================================*/

static const char *status_name(gui_sysval_status_t status)
{
    switch (status)
    {
    case GUI_SYSVAL_STATUS_OK:
        return "ok";
    case GUI_SYSVAL_STATUS_INVALID_VALUE:
        return "invalid value";
    case GUI_SYSVAL_STATUS_BUSY:
        return "busy";
    case GUI_SYSVAL_STATUS_TIMEOUT:
        return "timeout";
    default:
        return "error";
    }
}

/* A write may complete later; log the value that actually took effect. */
static void write_done(gui_sysval_request_id_t request_id,
                       gui_sysval_status_t status,
                       const gui_sysval_value_t *value,
                       void *user_data)
{
    const char *key = (const char *)user_data;

    GUI_UNUSED(request_id);

    if (status != GUI_SYSVAL_STATUS_OK || value == NULL)
    {
        gui_log("sysval set %s -> failed, %s\n", key, status_name(status));
    }
    else if (value->type == GUI_SYSVAL_TYPE_BOOL)
    {
        gui_log("sysval set %s -> %s\n", key, value->data.b ? "true" : "false");
    }
    else
    {
        gui_log("sysval set %s -> %d\n", key, (int)value->data.i);
    }
}

/* One-shot timer callback: write a few values once. */
static void write_demo(void *obj)
{
    gui_sysval_value_t value;
    gui_sysval_request_id_t id;

    value = gui_sysval_value_int(35);
    gui_sysval_set_request(GUI_SYSVAL_KEY_DISPLAY_BRIGHTNESS, &value,
                           write_done, GUI_SYSVAL_KEY_DISPLAY_BRIGHTNESS);

    value = gui_sysval_value_bool(true);
    gui_sysval_set_request(GUI_SYSVAL_KEY_CONNECTIVITY_WIFI_ENABLED, &value,
                           write_done, GUI_SYSVAL_KEY_CONNECTIVITY_WIFI_ENABLED);

    value = gui_sysval_value_bool(true);
    gui_sysval_set_request(GUI_SYSVAL_KEY_HEALTH_HEART_RATE_MEASURING, &value,
                           write_done, GUI_SYSVAL_KEY_HEALTH_HEART_RATE_MEASURING);

    /* Rejected at once: the key is read-only. A rejected request returns
     * GUI_SYSVAL_REQUEST_INVALID and never calls back. */
    value = gui_sysval_value_int(999);
    id = gui_sysval_set_request(GUI_SYSVAL_KEY_HEALTH_PEDOMETER_STEPS, &value,
                                write_done, GUI_SYSVAL_KEY_HEALTH_PEDOMETER_STEPS);
    if (id == GUI_SYSVAL_REQUEST_INVALID)
    {
        gui_log("sysval set %s -> rejected, read-only\n",
                GUI_SYSVAL_KEY_HEALTH_PEDOMETER_STEPS);
    }

    /* Rejected at once: the key is a bool, not an int. */
    value = gui_sysval_value_int(1);
    id = gui_sysval_set_request(GUI_SYSVAL_KEY_AUDIO_MUTED, &value,
                                write_done, GUI_SYSVAL_KEY_AUDIO_MUTED);
    if (id == GUI_SYSVAL_REQUEST_INVALID)
    {
        gui_log("sysval set %s -> rejected, wrong type\n",
                GUI_SYSVAL_KEY_AUDIO_MUTED);
    }

    /* Accepted, then failed: the type is right, so the request starts, but
     * 150 is out of range. The backend reports it through the callback as
     * GUI_SYSVAL_STATUS_INVALID_VALUE. */
    value = gui_sysval_value_int(150);
    gui_sysval_set_request(GUI_SYSVAL_KEY_AUDIO_VOLUME, &value,
                           write_done, GUI_SYSVAL_KEY_AUDIO_VOLUME);

    gui_obj_stop_timer(GUI_BASE(obj));
}


/*============================================================================*
 *                         Init
 *============================================================================*/

static gui_text_t *text_create(const char *name, char *buf, int line)
{
    int16_t top = (int16_t)((gui_get_screen_height() -
                             LINE_COUNT * LINE_HEIGHT) / 2);
    gui_text_t *text = gui_text_create(gui_obj_get_root(), name, 0,
                                       (int16_t)(top + line * LINE_HEIGHT),
                                       gui_get_screen_width(), LINE_HEIGHT);

    snprintf(buf, TEXT_SIZE, "%s: --", name);
    gui_text_set(text, buf, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(buf),
                 FONT_SIZE);
    gui_text_mode_set(text, MID_CENTER);
    return text;
}

static int app_init(void)
{
    gui_win_t *win;

    time_text       = text_create("time",       time_buf,       0);
    date_text       = text_create("date",       date_buf,       1);
    battery_text    = text_create("battery",    battery_buf,    2);
    charging_text   = text_create("charging",   charging_buf,   3);
    brightness_text = text_create("brightness", brightness_buf, 4);
    wifi_text       = text_create("wifi",       wifi_buf,       5);
    steps_text      = text_create("steps",      steps_buf,      6);
    heart_rate_text = text_create("heart rate", heart_rate_buf, 7);
    sync_text       = text_create("sync",       sync_buf,       8);
    async_text      = text_create("async",      async_buf,      9);

    /* A widget holds one timer, so each timer gets its own window. */
    win = gui_win_create(gui_obj_get_root(), "sysval_refresh", 0, 0, 0, 0);
    gui_obj_create_timer(GUI_BASE(win), REFRESH_MS, true, refresh);
    gui_obj_start_timer(GUI_BASE(win));
    refresh(win);

    win = gui_win_create(gui_obj_get_root(), "sysval_write", 0, 0, 0, 0);
    gui_obj_create_timer(GUI_BASE(win), WRITE_DELAY_MS, false, write_demo);
    gui_obj_start_timer(GUI_BASE(win));

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
