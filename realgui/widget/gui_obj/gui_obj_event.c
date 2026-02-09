/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include <stdio.h>
#include "gui_obj.h"
#include "gui_obj_event.h"
#include "gui_api.h"
#include "tp_algo.h"
#include "kb_algo.h"

/*============================================================================*
 *                           Types
 *============================================================================*/

typedef struct
{
    gui_event_cb_t cb;
    gui_event_t event;
    void *obj;
} pending_event_t;

/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/

#define MAX_EVENT_CNT   10

/*============================================================================*
 *                            Variables
 *============================================================================*/

static pending_event_t pending_events[MAX_EVENT_CNT];
static uint8_t event_cnt = 0;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void gui_obj_store_event(gui_obj_t *obj, gui_event_code_t event, const void *indev_name)
{
    if (event != GUI_EVENT_TOUCH_RELEASED)
    {
        for (uint32_t i = 0; i < event_cnt; i++)
        {
            if (pending_events[i].event.code == event)
            {
                for (uint32_t j = i; j < event_cnt; j++)
                {
                    pending_events[j] = pending_events[j + 1];
                }
                event_cnt--;
                break;
            }
        }
    }

    for (uint8_t i = 0; i < obj->event_dsc_cnt; i++)
    {
        gui_event_dsc_t *event_dsc = obj->event_dsc + i;
        if (event_dsc->filter == event && event_cnt < MAX_EVENT_CNT)
        {
            pending_events[event_cnt].cb = event_dsc->event_cb;
            pending_events[event_cnt].event.code = event;
            pending_events[event_cnt].event.user_data = event_dsc->user_data;
            pending_events[event_cnt].event.indev_name = indev_name;
            pending_events[event_cnt].obj = obj;
            event_cnt++;
        }
    }
}

/*============================================================================*
*                           Public Functions
*============================================================================*/

static bool gui_obj_seek_event(gui_obj_t *obj, gui_event_code_t event)
{
    for (uint8_t i = 0; i < obj->event_dsc_cnt; i++)
    {
        gui_event_dsc_t *event_dsc = obj->event_dsc + i;
        if (event_dsc->filter == event)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Adds an event callback to a GUI object.
 *
 * This function registers a callback function to be called when a specific event occurs on the GUI object.
 *
 * @param obj Pointer to the GUI object to which the event callback will be added.
 * @param event_cb The callback function to be registered for the event.
 */
void gui_obj_add_event_cb(void           *obj,
                          gui_event_cb_t  event_cb,
                          gui_event_code_t filter,
                          void           *user_data)
{
    if (gui_obj_seek_event(obj, filter) == true)
    {
        return;
    }

    gui_obj_t *object = (gui_obj_t *)obj;

    object->event_dsc_cnt++;
    object->event_dsc = gui_realloc(object->event_dsc, sizeof(gui_event_dsc_t) * object->event_dsc_cnt);

    gui_event_dsc_t *event_dsc = object->event_dsc + object->event_dsc_cnt - 1;
    event_dsc->event_cb = event_cb;
    event_dsc->filter = filter;
    event_dsc->user_data = user_data;

    // gui_obj_suppress_set(object, filter);

}


void gui_obj_enable_event(gui_obj_t *obj, gui_event_code_t event, const void *indev_name)
{
    touch_info_t *tp = tp_get_info();

    if (gui_obj_seek_event(obj, event) == false || obj->not_show == true)
    {
        return;
    }

    // if (gui_obj_event_is_suppressed(obj, event) == true)
    // {
    //     return;
    // }

    switch (event)
    {
    case GUI_EVENT_TOUCH_CLICKED:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->type == TOUCH_SHORT))
        {
            gui_obj_store_event(obj, event, indev_name);
        }
        break;
    case GUI_EVENT_TOUCH_LONG:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->type == TOUCH_LONG))
        {
            gui_obj_store_event(obj, event, indev_name);
        }
        break;
    case GUI_EVENT_TOUCH_DOUBLE_CLICKED:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->type == TOUCH_DOUBLE))
        {
            gui_obj_store_event(obj, event, indev_name);
        }
        // GUI_ASSERT(NULL != NULL);
        break;
    case GUI_EVENT_TOUCH_TRIPLE_CLICKED:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->type == TOUCH_TRIPLE))
        {
            gui_obj_store_event(obj, event, indev_name);
        }
        // GUI_ASSERT(NULL != NULL);
        break;
    case GUI_EVENT_TOUCH_PRESSED:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->pressed == true))
        {
            gui_obj_store_event(obj, event, indev_name);
        }
        break;
    case GUI_EVENT_TOUCH_RELEASED:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->released == true))
        {
            gui_obj_store_event(obj, event, indev_name);
        }
        break;
    case GUI_EVENT_TOUCH_PRESSING:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->pressing == true))
        {
            gui_obj_store_event(obj, event, indev_name);
        }
        break;
    case GUI_EVENT_KB_SHORT_PRESSED:
        gui_obj_store_event(obj, event, indev_name);
        break;
    case GUI_EVENT_KB_LONG_PRESSED:
        gui_obj_store_event(obj, event, indev_name);
        break;
    case GUI_EVENT_TOUCH_MOVE_LEFT:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->left_moved == true))
        {
            gui_obj_store_event(obj, event, indev_name);
        }
        break;
    case GUI_EVENT_TOUCH_MOVE_RIGHT:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->right_moved == true))
        {
            gui_obj_store_event(obj, event, indev_name);
        }
        break;
    case GUI_EVENT_TOUCH_MOVE_UP:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->up_moved == true))
        {
            gui_obj_store_event(obj, event, indev_name);
        }
        break;
    case GUI_EVENT_TOUCH_MOVE_DOWN:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->down_moved == true))
        {
            gui_obj_store_event(obj, event, indev_name);
        }
        break;
    case GUI_EVENT_TOUCH_SCROLL_HORIZONTAL:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->type == TOUCH_HOLD_X))
        {
            gui_obj_store_event(obj, event, indev_name);
        }
        break;
    case GUI_EVENT_TOUCH_SCROLL_VERTICAL:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->type == TOUCH_HOLD_Y))
        {
            gui_obj_store_event(obj, event, indev_name);
        }
        break;
    default:
        GUI_ASSERT(0);
        break;
    }
}

void gui_obj_event_dispatch(bool enable_event)
{
    if (enable_event == true)
    {
        for (uint32_t i = 0; i < event_cnt; i++)
        {
            pending_events[i].cb(pending_events[i].obj, &pending_events[i].event);
        }
    }
    event_cnt = 0;
}
// #endif
