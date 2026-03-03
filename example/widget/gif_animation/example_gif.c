/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_gif.h"
#include "gui_win.h"
#include "gui_obj.h"
#include "gui_obj_focus.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gif_demo.txt"

/* gui gif widget example start*/
void gif_event_cb(void *obj, gui_event_t *e)
{
    gui_obj_t *this = (gui_obj_t *)obj;
    const char *dev_name = e->indev_name ? (const char *)e->indev_name : "unknown";
    gui_log("GIF Event: obj name = %s, e = 0x%x, indev = %s !\n", this->name, e->code, dev_name);
}

void gif_event_clicked_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    gui_log("GIF Single clicked!\n");
}

void gif_event_double_clicked_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    gui_log("GIF Double clicked!\n");
}

void gif_event_triple_clicked_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    gui_log("GIF Triple clicked!\n");
}

void gif_event_long_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    gui_log("GIF Touch long!\n");
}

static int app_init(void)
{
    // Use the GIF demo data from gif_demo.txt
    void *addr = (void *)_acgif_demo;

    // Create GIF widget from memory
    gui_gif_t *gif = gui_gif_create_from_mem(gui_obj_get_root(), "gif_demo", addr, 0, 0, 0, 0);

    // Set transformations (optional)
    gui_gif_rotation(gif, 0.0f);
    // gui_gif_scale(gif, 0.5f, 0.5f);
    // gui_gif_translate(gif, 50, 50);
    // gui_gif_set_focus(gif, 50, 50);

    // Set opacity (optional)
    // gui_gif_set_opacity(gif, 200);

    // Set blend mode (optional)
    gui_gif_set_mode(gif, IMG_FILTER_BLACK);

    // Set quality
    // gui_gif_set_quality(gif, true);

    // Add event callbacks
    gui_obj_add_event_cb(gif, (gui_event_cb_t)gif_event_clicked_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(gif, (gui_event_cb_t)gif_event_double_clicked_cb,
                         GUI_EVENT_TOUCH_DOUBLE_CLICKED, NULL);
    gui_obj_add_event_cb(gif, (gui_event_cb_t)gif_event_triple_clicked_cb,
                         GUI_EVENT_TOUCH_TRIPLE_CLICKED, NULL);
    gui_obj_add_event_cb(gif, (gui_event_cb_t)gif_event_long_cb, GUI_EVENT_TOUCH_LONG, NULL);
    gui_obj_add_event_cb(gif, (gui_event_cb_t)gif_event_cb, GUI_EVENT_KB_SHORT_PRESSED, NULL);

    // Set focus
    gui_obj_focus_set(gif);

    return 0;
}

// Note: This is an example function, call it from your application's app_init()
// GUI_INIT_APP_EXPORT(app_init);
/* gui gif widget example end*/
