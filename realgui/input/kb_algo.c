/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * File      : kb_algo.c
 */
#include "gui_server.h"
#include "guidef.h"
#include "gui_api.h"
#include "kb_algo.h"
#include "gui_obj_focus.h"


#define RTK_KB_DEBUG

#ifdef RTK_KB_DEBUG

#define KB_LOG(format, ...) gui_log(format, ##__VA_ARGS__)

#else

#define KB_LOG(format, ...)

#endif

static kb_info_t g_kb_info = {.type = TOUCH_INVALID};


kb_info_t *kb_algo_process(gui_obj_t *object)
{
    gui_node_list_t *node = NULL;

    if (gui_obj_focus_get() == NULL)
    {
        // KB_LOG("No focused object for keyboard event processing.\n");
        g_kb_info.pressing = false;
        return &g_kb_info;
    }

    gui_list_for_each(node, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        gui_indev_kb_t *kb = (gui_indev_kb_t *)obj;

        // Only process when key is released and not yet handled
        if (*(kb->state) == false &&
            *(kb->timestamp_ms_release) > *(kb->timestamp_ms_press) &&
            *(kb->timestamp_ms_release) != kb->last_processed_release)
        {
            uint32_t duration = *(kb->timestamp_ms_release) - *(kb->timestamp_ms_press);

            if (duration < 200)
            {
                KB_LOG("short press detected: %s, duration: %d ms\n", obj->name, duration);
                gui_obj_enable_event(gui_obj_focus_get(), GUI_EVENT_KB_SHORT_PRESSED, obj->name);
            }
            else
            {
                KB_LOG("long press detected: %s, duration: %d ms\n", obj->name, duration);
                gui_obj_enable_event(gui_obj_focus_get(), GUI_EVENT_KB_LONG_PRESSED, obj->name);
            }

            kb->last_processed_release = *(kb->timestamp_ms_release);
        }

        if (*(kb->state) == true)
        {
            // Update pressing timestamp periodically when key is pressed
            // *(kb->timestamp_ms_pressing) = *(kb->timestamp_ms_press);
            g_kb_info.pressing = true;
        }
        else
        {
            // Reset pressing timestamp when key is not pressed
            // *(kb->timestamp_ms_pressing) = 0;
            g_kb_info.pressing = false;
        }
    }
    return &g_kb_info;
}


gui_indev_kb_t *gui_kb_create(char *name, bool *state, uint32_t *timestamp_ms_press,
                              uint32_t *timestamp_ms_release)
{
    gui_indev_kb_t *kb = gui_malloc(sizeof(gui_indev_kb_t));
    GUI_ASSERT(kb != NULL);
    if (kb == NULL) { return NULL; }
    gui_obj_t *this = (gui_obj_t *)kb;
    memset(kb, 0x00, sizeof(gui_indev_kb_t));

    kb->base.name = name;
    kb->base.magic = GUI_MAGIC_NUMBER;
    kb->base.parent = gui_obj_get_kb_root();

    gui_list_init(&(this->child_list));
    if ((this->parent) != ((void *)0))
    {
        gui_list_insert_before(&(this->parent->child_list), &(this->brother_list));
    }
    this->create_done = true;

    kb->state = state;
    kb->timestamp_ms_press = timestamp_ms_press;
    kb->timestamp_ms_release = timestamp_ms_release;
    kb->last_processed_release = 0;

    return kb;
}


