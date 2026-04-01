/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_MENU_CELLULAR_H__
#define __GUI_MENU_CELLULAR_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_fb.h"
#include "gui_img.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  Menu_cellular structure. */
typedef struct gui_menu_cellular
{
    gui_obj_t base;
    int16_t ver_speed;
    int16_t ver_record[5];
    int16_t ver_hold;
    int16_t hor_hold;
    int16_t ver_offset;     //!< Vertical offset.
    int16_t hor_offset;     //!< Horizontal offset.
    int16_t ver_offset_min; //!< Minimum vertical offset.
    int16_t icon_size;
} gui_menu_cellular_t;

struct gui_menu_cellular_gesture_parameter
{
    gui_event_cb_t callback_function;
    void *parameter;
};

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Config offset.
 * @param menu_cellular Menu_cellular pointer.
 * @param offset_x X-axis offset.
 * @param offset_y Y-axis offset.
 * @param src_mode Image source mode, support file system and memory address.
 */
gui_menu_cellular_t *gui_menu_cellular_create(void     *parent,
                                              int       icon_size,
                                              void     *icon_array[],
                                              int       array_size,
                                              IMG_SOURCE_MODE_TYPE src_mode);

/**
 * @brief Add click events on images based on the cellular menu.
 * @param menu_cellular Menu_cellular pointer.
 * @param para_array Array of gesture parameters corresponding to the click event.
 * @param array_length Length of the gesture parameters array.
 */
void gui_menu_cellular_on_click(gui_menu_cellular_t *menu_cellular,
                                struct gui_menu_cellular_gesture_parameter *para_array, int array_length);

/**
 * @brief Config offset.
 * @param menu_cellular Menu_cellular pointer.
 * @param offset_x X-axis offset.
 * @param offset_y Y-axis offset.
 */
void gui_menu_cellular_offset(gui_menu_cellular_t *menu_cellular, int offset_x, int offset_y);

#ifdef __cplusplus
}
#endif

#endif
