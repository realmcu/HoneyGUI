/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __DEF_INPUT_H__
#define __DEF_INPUT_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define GUI_WHEEL_NO_EVENT 0
#define GUI_WHEEL_BUTTON_DOWN 1
#define GUI_WHEEL_BUTTON_UP 2
#define GUI_WHEEL_SCROLL 3


/* T_GUI_INPUT_TYPE enum start*/
typedef enum
{
    TOUCH_INIT                  = 0x100,
    TOUCH_HOLD_X,
    TOUCH_HOLD_Y,
    TOUCH_SHORT,
    TOUCH_LONG,
    TOUCH_DOUBLE,
    TOUCH_TRIPLE,
    TOUCH_ORIGIN_FROM_X,
    TOUCH_ORIGIN_FROM_Y,
    TOUCH_LEFT_SLIDE,
    TOUCH_RIGHT_SLIDE,
    TOUCH_UP_SLIDE,
    TOUCH_DOWN_SLIDE,
    TOUCH_SHORT_BUTTON,
    TOUCH_LONG_BUTTON,
    TOUCH_UP_SLIDE_TWO_PAGE,
    TOUCH_DOWN_SLIDE_TWO_PAGE,
    TOUCH_INVALID              = 0x1FF,

    KB_INIT                     = 0x200,
    KB_SHORT                    = 0x201,
    KB_LONG                     = 0x202,
    KB_INVALID                 = 0x2FF,

    WHEEL_INIT                  = 0x300,
    WHEEL_ING,
    WHEEL_FINISHED,
    WHEEL_INVALID              = 0x3FF,
} T_GUI_INPUT_TYPE;
/* T_GUI_INPUT_TYPE enum end*/


typedef struct touch_info
{
    T_GUI_INPUT_TYPE type;
    int16_t deltaX;
    int16_t deltaY;
    int16_t x;
    int16_t y;
    int16_t history_x;
    int16_t history_y;
    uint8_t pressed  : 1;
    uint8_t released : 1;
    uint8_t pressing : 1;
    uint8_t left_moved : 1;
    uint8_t right_moved : 1;
    uint8_t up_moved : 1;
    uint8_t down_moved : 1;
    uint8_t button_down  : 1;
    uint8_t button_up  : 1;
    uint8_t button_hold  : 1;
} touch_info_t;

typedef struct kb_info
{
    T_GUI_INPUT_TYPE type;
    char *string[20];
    uint8_t pressed             : 1;
    uint8_t released            : 1;
    uint8_t pressing            : 1;
} kb_info_t;

typedef struct wheel_info
{
    T_GUI_INPUT_TYPE type;
    int16_t delta;
} wheel_info_t;

typedef struct gui_touch_port_data
{
    uint8_t  event;                 /* The touch event of the data */
    uint8_t  track_id;              /* Track id of point */
    uint16_t width;                 /* Point of width */
    uint16_t x_coordinate;          /* Point of x coordinate */
    uint16_t y_coordinate;          /* Point of y coordinate */
    uint32_t timestamp_ms;          /* The timestamp when pressing, update periodically*/
    uint16_t x_coordinate_start;    /* Point of x when pressed, update once, for tp algo v2*/
    uint16_t y_coordinate_start;    /* Point of y when pressed, update once, for tp algo v2*/
    uint32_t timestamp_ms_start;    /* The timestamp when pressed, update once, for tp algo v2*/
    void *data;
} gui_touch_port_data_t;

typedef struct gui_wheel_port_data
{
    uint8_t  event;//0:no event//1:button down//2:button up//3:scroll
    int16_t  delta;//positive away from the user and negative toward the user
    uint32_t timestamp_ms;             /* The timestamp when the data was received */
} gui_wheel_port_data_t;

/* gui_indev struct define start */
typedef struct gui_indev
{
    uint16_t tp_witdh;
    uint16_t tp_height;
    uint32_t touch_timeout_ms;
    uint16_t long_button_time_ms;
    uint16_t short_button_time_ms;
    uint16_t kb_long_button_time_ms;
    uint16_t kb_short_button_time_ms;
    uint16_t quick_slide_time_ms;

    void (*ext_button_indicate)(void (*callback)(void));

    gui_touch_port_data_t *(*tp_get_data)(void);

    gui_wheel_port_data_t *(*wheel_get_port_data)(void);
} gui_indev_t;
/* gui_indev struct define end */


#ifdef __cplusplus
}
#endif

#endif

