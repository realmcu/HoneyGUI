/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __DEF_EVENT_H__
#define __DEF_EVENT_H__



#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  ... */

typedef enum
{
    GUI_EVENT_INVALID                = 0x0000,
    GUI_EVENT_SYSTEM_RESET,
    GUI_EVENT_SYSTEM_FREE,
    GUI_EVENT_SYSTEM_SLEEP,
    GUI_EVENT_SYSTEM_WAKEUP,
    GUI_EVENT_SYSTEM_REBOOT,
    GUI_EVENT_SYSTEM_POWER_OFF,
    GUI_EVENT_SYSTEM_UPDATE,
    /** touch device events*/
    GUI_EVENT_TOUCH_CLICKED          = 0x0100,
    GUI_EVENT_TOUCH_DOUBLE_CLICKED,
    GUI_EVENT_TOUCH_TRIPLE_CLICKED,
    GUI_EVENT_TOUCH_LONG,
    GUI_EVENT_TOUCH_PRESSED,
    GUI_EVENT_TOUCH_PRESSING, // GUI_EVENT_TOUCH_HOLD,
    GUI_EVENT_TOUCH_RELEASED,

    // GUI_EVENT_TOUCH_MOVE,
    GUI_EVENT_TOUCH_MOVE_LEFT,
    GUI_EVENT_TOUCH_MOVE_RIGHT,
    GUI_EVENT_TOUCH_MOVE_UP,
    GUI_EVENT_TOUCH_MOVE_DOWN,

    GUI_EVENT_TOUCH_SCROLL_HORIZONTAL,
    GUI_EVENT_TOUCH_SCROLL_VERTICAL,

    GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE,
    GUI_EVENT_TOUCH_TOUCH_LEFT_SLIDE,


    /** keyboard device events*/
    GUI_EVENT_KB_SHORT_PRESSED          = 0x0200,
    GUI_EVENT_KB_LONG_PRESSED,

    GUI_EVENT_VIEW_SWITCH_DIRECT
    /** wheel device events*/

} gui_event_code_t;

typedef struct _gui_event_t
{
    gui_event_code_t code;
    void *user_data;
    const void *indev_name;
} gui_event_t;

typedef void (*gui_event_cb_t)(void *obj, gui_event_t *e);

typedef struct _gui_event_dsc_t
{
    gui_event_cb_t event_cb;
    void *user_data;
    gui_event_code_t filter;
} gui_event_dsc_t;

#ifdef __cplusplus
}
#endif

#endif

