/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_pagelist.h
  * @brief pagelist widget
  * @details A ram friendly page widget to show large amount of file lists.
  *          Please refer to demo "example\screen_454_454_bb2plus_watch\gui_record\pagelist_test.c"
  * @author haonan_shi@realsil.com
  * @date 2023/12/15
  * @version v0.1
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_PAGELIST_H__
#define __GUI_PAGELIST_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "gui_img.h"
#include "gui_switch.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  Pagelist widget structure */

typedef void (*gui_pagelist_update_cb_t)(void *obj, void *list);

typedef struct gui_pagelist
{
    gui_obj_t base;
    int8_t show_list_max;
    int16_t real_list_max;
    gui_switch_t *list_first;
    gui_switch_t *list_last;
    gui_img_t *scroll_bar;
    int16_t x_init;
    int16_t y_init;
    int16_t x_stop_scroll;
    int16_t y_stop_scroll;
    int16_t list_gap_y;
    int16_t show_border_top;
    int16_t show_border_bottom;
    bool h_set_done;
    gui_pagelist_update_cb_t update_list_first_cb;
    gui_pagelist_update_cb_t update_list_last_cb;
    int16_t list_count;
    IMG_SOURCE_MODE_TYPE src_mode;   // scroll_bar
} gui_pagelist_t;


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
 * @brief Increase the number of real lists.
 *
 * @param this Pagelist widget.
 * @param real_list_max Number to increase.
 */
void gui_pagelist_increase_real_list_max(gui_pagelist_t *this, int16_t real_list_max);

/**
 * @brief Decrease the number of real lists.
 *
 * @param this Pagelist widget.
 * @param real_list_max Number to decrease.
 */
void gui_pagelist_decrease_real_list_max(gui_pagelist_t *this, int16_t real_list_max);

/**
 * @brief Add update callback when pagelist slides up and down to update text typically.
 *
 * @param this Pagelist widget.
 * @param list_first_cb Update callback of the top show list.
 * @param list_last_cb Update callback of the bottom show list.
 */

void gui_pagelist_add_list_update_cb(gui_pagelist_t          *this,
                                     gui_pagelist_update_cb_t list_first_cb,
                                     gui_pagelist_update_cb_t list_last_cb);

/**
 * @brief Set attribute for a pagelist widget.
 *
 * @param this Pagelist widget.
 * @param real_list_max Real quality of file lists to show. Larger than show_list_max typically.
 * @param show_list_max Quality of switch widgets, which should be created as child nodes of this pagelist widget.
 * @param list_gap_y Y gap between two show lists.
 * @param list_first Index of the top show list.
 * @param list_last Index of the bottom show list.
 */
void gui_pagelist_set_att(gui_pagelist_t *this,
                          int16_t         real_list_max,
                          int16_t         show_list_max,
                          int16_t         list_gap_y,
                          gui_switch_t   *list_first,
                          gui_switch_t   *list_last);

/**
 * @brief Add a scroll bar picture for a pagelist widget, file source is memory address.
 *
 * @param this Pagelist widget.
 * @param bar_pic Bar picture.
 */
void gui_pagelist_add_scroll_bar(gui_pagelist_t *this, void *bar_pic,
                                 IMG_SOURCE_MODE_TYPE src_mode);

/**
 * @brief Create a pagelist widget.
 *
 * @param parent Parent of this widget.
 * @param name Name of this widget.
 * @param x X of this widget.
 * @param y Y of this widget.
 * @param w W of this widget.
 * @param h H of this widget.
 * @return gui_pagelist_t*.
 */
gui_pagelist_t *gui_pagelist_create(void       *parent,
                                    const char *name,
                                    int16_t     x,
                                    int16_t     y,
                                    int16_t     w,
                                    int16_t     h);


#ifdef __cplusplus
}
#endif

#endif
