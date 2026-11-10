/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * System value subscription demo.
 *
 * This screen only consumes. It subscribes to battery and charging state and
 * shows whatever is published afterwards; nothing is read up front, and this
 * file does not publish either, so both lines stay as "--" until the platform
 * reports a change.
 *
 * The subscriptions update widgets that outlive them here, so they are never
 * cancelled. A screen that can be closed must call gui_sysval_unsubscribe()
 * before releasing the widgets its callbacks touch.
 */

#include <stdio.h>
#include <string.h>
#include "gui_api.h"
#include "gui_components_init.h"
#include "gui_obj.h"
#include "gui_sysval.h"
#include "gui_text.h"
#include "gui_obj_focus.h"

#define TEXT_SIZE      40
#define FONT_SIZE      24
#define LINE_HEIGHT    40
#define LINE_COUNT     3

static char battery_buf[TEXT_SIZE];
static char charging_buf[TEXT_SIZE];
static char event_buf[TEXT_SIZE];

static gui_text_t *battery_text;
static gui_text_t *charging_text;
static gui_text_t *event_text;

static void text_update(gui_text_t *text, char *buf)
{
    gui_text_content_set(text, buf, strlen(buf));
}

static void battery_changed(const char *key,
                            const gui_sysval_value_t *value,
                            void *user_data)
{
    GUI_UNUSED(key);

    if (value == NULL || value->type != GUI_SYSVAL_TYPE_INT)
    {
        return;
    }

    snprintf(battery_buf, TEXT_SIZE, "battery: %d%% (sub)", (int)value->data.i);
    snprintf(event_buf, TEXT_SIZE, "event: battery %d%%", (int)value->data.i);
    text_update((gui_text_t *)user_data, battery_buf);
    text_update(event_text, event_buf);
    gui_log("battery subscription: %d%%\n", (int)value->data.i);
}

static void charging_changed(const char *key,
                             const gui_sysval_value_t *value,
                             void *user_data)
{
    GUI_UNUSED(key);

    if (value == NULL || value->type != GUI_SYSVAL_TYPE_BOOL)
    {
        return;
    }

    snprintf(charging_buf, TEXT_SIZE, "charging: %s (sub)",
             value->data.b ? "yes" : "no");
    snprintf(event_buf, TEXT_SIZE, "event: charging %s",
             value->data.b ? "yes" : "no");
    text_update((gui_text_t *)user_data, charging_buf);
    text_update(event_text, event_buf);
    gui_log("charging subscription: %s\n", value->data.b ? "yes" : "no");
}


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

/* Stand-in for a fuel gauge that reports a new level. A real producer does the
 * same from whatever thread sees the change. */
static void click_main_button(void *obj, gui_event_t *e)
{
    static int32_t level = 76;
    gui_sysval_value_t value;

    GUI_UNUSED(obj);

    if (e == NULL || e->indev_name == NULL ||
        strcmp(e->indev_name, "Home") != 0)
    {
        return;
    }

    level = (level == 76) ? 64 : 76;
    value = gui_sysval_value_int(level);
    if (!gui_sysval_publish(GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL, &value))
    {
        gui_log("sysval publish %s failed\n",
                GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL);
        return;
    }
    gui_log("publish battery level %d%%\n", (int)level);
}

static int app_init(void)
{
    gui_obj_add_event_cb(gui_obj_get_root(), click_main_button, GUI_EVENT_KB_SHORT_PRESSED, NULL);
    gui_obj_focus_set(gui_obj_get_root());

    battery_text = text_create("battery", battery_buf, 0);
    charging_text = text_create("charging", charging_buf, 1);
    event_text = text_create("event", event_buf, 2);

    /* The texts live on the root for the whole run, so the handles are only
     * needed to report a failure. A screen that can be closed keeps them and
     * unsubscribes before destroying the widgets. */
    if (gui_sysval_subscribe(GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL,
                             battery_changed, battery_text) == NULL ||
        gui_sysval_subscribe(GUI_SYSVAL_KEY_POWER_BATTERY_CHARGING,
                             charging_changed, charging_text) == NULL)
    {
        gui_log("sysval subscribe failed for battery state\n");
    }

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
