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

static gui_event_cb_t event_cb[MAX_EVENT_CNT];
static gui_event_t event_code[MAX_EVENT_CNT];
static void *event_cb_param[MAX_EVENT_CNT];
static void *event_obj[MAX_EVENT_CNT];
static uint8_t event_cnt = 0;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void gui_obj_store_event(gui_obj_t *obj, gui_event_t event)
{

    if (event != GUI_EVENT_TOUCH_RELEASED)
    {
        for (uint32_t i = 0; i < event_cnt; i++)
        {
            if (event_code[i] == event)
            {
                for (uint32_t j = i; j < event_cnt; j++)
                {
                    event_cb[j] = event_cb[j + 1];
                    event_code[j] = event_code[j + 1];
                    event_cb_param[j] = event_cb_param[j + 1];
                    event_obj[j] = event_obj[j + 1];
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
            event_cb[event_cnt] = event_dsc->event_cb;
            event_code[event_cnt] = event;
            event_cb_param[event_cnt] = event_dsc->user_data;
            event_obj[event_cnt] = obj;
            event_cnt++;
        }
    }
}

/*============================================================================*
*                           Public Functions
*============================================================================*/

static bool gui_obj_seek_event(gui_obj_t *obj, gui_event_t event)
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
                          gui_event_t     filter,
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


void gui_obj_enable_event(gui_obj_t *obj, gui_event_t event)
{
    touch_info_t *tp = tp_get_info();
    kb_info_t *kb = kb_get_info();

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
            gui_obj_store_event(obj, event);
        }
        break;
    case GUI_EVENT_TOUCH_LONG:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->type == TOUCH_LONG))
        {
            gui_obj_store_event(obj, event);
        }
        break;
    case GUI_EVENT_TOUCH_DOUBLE_CLICKED:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->type == TOUCH_DOUBLE))
        {
            gui_obj_store_event(obj, event);
        }
        GUI_ASSERT(NULL != NULL);
        break;
    case GUI_EVENT_TOUCH_PRESSED:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->pressed == true))
        {
            gui_obj_store_event(obj, event);
        }
        break;
    case GUI_EVENT_TOUCH_RELEASED:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->released == true))
        {
            gui_obj_store_event(obj, event);
        }
        break;
    case GUI_EVENT_TOUCH_PRESSING:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->pressing == true))
        {
            gui_obj_store_event(obj, event);
        }
        break;
    case GUI_EVENT_KB_SHORT_CLICKED:
        if (kb->type == KB_SHORT)
        {
            gui_obj_store_event(obj, event);
        }
        break;
    case GUI_EVENT_KB_LONG_CLICKED:
        if (kb->type == KB_LONG)
        {
            gui_obj_store_event(obj, event);
        }
        break;
    case GUI_EVENT_TOUCH_MOVE_LEFT:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->left_moved == true))
        {
            gui_obj_store_event(obj, event);
        }
        break;
    case GUI_EVENT_TOUCH_MOVE_RIGHT:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->right_moved == true))
        {
            gui_obj_store_event(obj, event);
        }
        break;
    case GUI_EVENT_TOUCH_MOVE_UP:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->up_moved == true))
        {
            gui_obj_store_event(obj, event);
        }
        break;
    case GUI_EVENT_TOUCH_MOVE_DOWN:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->down_moved == true))
        {
            gui_obj_store_event(obj, event);
        }
        break;
    case GUI_EVENT_TOUCH_SCROLL_HORIZONTAL:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->type == TOUCH_HOLD_X))
        {
            gui_obj_store_event(obj, event);
        }
        break;
    case GUI_EVENT_TOUCH_SCROLL_VERTICAL:
        if ((gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true) && (tp->type == TOUCH_HOLD_Y))
        {
            gui_obj_store_event(obj, event);
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
            event_cb[i](event_obj[i], event_code[i], event_cb_param[i]);
        }
    }
    event_cnt = 0;
}
// #endif
