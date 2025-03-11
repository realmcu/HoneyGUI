/**
 * @file def_vg.h
 * @author wenjing_jiang(wenjing_jiang@realsil.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-06-14
 *
 * @copyright Copyright (c) 2024
 *
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
    TOUCH_HOLD_X                = 0x101,
    TOUCH_HOLD_Y                = 0x102,
    TOUCH_SHORT                 = 0x103,
    TOUCH_LONG                  = 0x104,
    TOUCH_ORIGIN_FROM_X         = 0x105,
    TOUCH_ORIGIN_FROM_Y         = 0x106,
    TOUCH_LEFT_SLIDE            = 0x107,
    TOUCH_RIGHT_SLIDE           = 0x108,
    TOUCH_UP_SLIDE              = 0x109,
    TOUCH_DOWN_SLIDE            = 0x10A,
    TOUCH_SHORT_BUTTON          = 0x10B,
    TOUCH_LONG_BUTTON           = 0x10C,
    TOUCH_UP_SLIDE_TWO_PAGE     = 0x10D,
    TOUCH_DOWN_SLIDE_TWO_PAGE   = 0x10E,
    TOUCH_INVALIDE              = 0x10F,

    KB_INIT                     = 0x200,
    KB_SHORT                    = 0x201,
    KB_LONG                     = 0x202,
    KB_INVALIDE                 = 0x203,

    WHEEL_INIT                  = 0x300,
    WHEEL_ING                   = 0x301,
    WHEEL_FINISHED              = 0x302,
    WHEEL_INVALIDE              = 0x303,
} T_GUI_INPUT_TYPE;
/* T_GUI_INPUT_TYPE enum end*/


typedef struct touch_info
{
    int16_t deltaX;
    int16_t deltaY;
    int16_t x;
    int16_t y;
    int16_t history_x;
    int16_t history_y;
    uint32_t type;
    uint8_t pressed  : 1;
    uint8_t released : 1;
    uint8_t pressing : 1;
    uint8_t button_down  : 1;
    uint8_t button_up  : 1;
    uint8_t button_hold  : 1;
} touch_info_t;

typedef struct kb_info
{
    uint32_t type;
    char *string[20];
    uint8_t pressed  : 1;
    uint8_t released : 1;
    uint8_t pressing : 1;
} kb_info_t;

typedef struct wheel_info
{
    uint32_t type;
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

typedef struct gui_kb_port_data
{
    uint8_t  event;
    uint8_t  name[10];
    uint32_t timestamp_ms_press;            /* The timestamp when pressed, update once */
    uint32_t timestamp_ms_release;          /* The timestamp when when release, update once*/
    uint32_t timestamp_ms_pressing;         /* The timestamp when pressing, update periodically*/
} gui_kb_port_data_t;

typedef struct gui_wheel_port_data
{
    uint8_t  event;//0:no event//1:button down//2:button up//3:scroll
    int16_t  delta;//positive away from the user and negative toward the user
    uint32_t timestamp_ms;             /* The timestamp when the data was received */
} gui_wheel_port_data_t;

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

    gui_kb_port_data_t *(*kb_get_port_data)(void);

    gui_wheel_port_data_t *(*wheel_get_port_data)(void);
} gui_indev_t;



#ifdef __cplusplus
}
#endif

#endif

