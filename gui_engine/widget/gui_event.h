/*
 * File      : guidef.h
 */
#ifndef __GUI_EVENT_H__
#define __GUI_EVENT_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif


//#define GUI_EVENT_PRESSED               (1<<0)
//#define GUI_EVENT_PRESSING              (1<<1)
//#define GUI_EVENT_CLICKED               (1<<2)
//#define GUI_EVENT_TOUCH_RIGHT_SLIDE     (1<<3)
//#define GUI_EVENT_TOUCH_LEFT_SLIDE      (1<<4)

//#define GUI_EVENT_KB_UP_PRESSED         (1<<5)
//#define GUI_EVENT_KB_UP_RELEASED        (1<<6)
//#define GUI_EVENT_KB_DOWN_PRESSED       (1<<7)
//#define GUI_EVENT_KB_DOWN_RELEASED      (1<<8)

//#define GUI_EVENT_PROCESSED             (0xFFFFFFFF)


typedef enum
{
    GUI_EVENT_INVALIDE,
    /** Input device events*/
    GUI_EVENT_TOUCH_PRESSED,
    GUI_EVENT_TOUCH_PRESSING,
    GUI_EVENT_TOUCH_CLICKED,
    GUI_EVENT_TOUCH_LONG,
    GUI_EVENT_TOUCH_RELEASED,
    GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE,
    GUI_EVENT_TOUCH_TOUCH_LEFT_SLIDE,

    GUI_EVENT_KB_UP_PRESSED,
    GUI_EVENT_KB_UP_RELEASED,
    GUI_EVENT_KB_DOWN_PRESSED,
    GUI_EVENT_KB_DOWN_RELEASED,
    /** Special events*/
    /** Other events*/
    GUI_EVENT_1,
    GUI_EVENT_2,
    GUI_EVENT_3,
    GUI_EVENT_4,
    GUI_EVENT_5,
    GUI_EVENT_6,
    GUI_EVENT_7,
    GUI_EVENT_8,
} gui_event_t;



typedef void (*gui_event_cb_t)(void *parameter, gui_event_t e);

typedef struct _gui_event_dsc_t
{
    gui_event_cb_t event_cb;
    void *user_data;
    gui_event_t filter;
    gui_event_t event_code;
} gui_event_dsc_t;

#ifdef __cplusplus
}
#endif

#endif

