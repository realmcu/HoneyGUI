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
#include <guidef.h>
#include <gui_api.h>
#include <gui_img.h>
#include <gui_switch.h>

/** @defgroup WIDGET WIDGET
  * @brief
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @brief
  * @{
  */

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
    bool flg_fs;   // scroll_bar

} gui_pagelist_t;

/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @brief
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @brief
  * @{
  */


/** End of WIDGET_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @brief
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/

/** @defgroup WIDGET_Exported_GUI_Functions WIDGET Exported Functions
  * @brief
  * @{
  */

/**
 * @brief increase number of real lists
 *
 * @param this pagelist widget
 * @param real_list_max number to increase
 * @return
 */
void gui_pagelist_increase_real_list_max(gui_pagelist_t *this, int16_t real_list_max);

/**
 * @brief decrease number of real lists
 *
 * @param this pagelist widget
 * @param real_list_max number to decrease
 * @return
 */
void gui_pagelist_decrease_real_list_max(gui_pagelist_t *this, int16_t real_list_max);

/**
 * @brief add update callback when pagelist slide up and down.
 *        to update text typically
 *
 * @param this pagelist widget
 * @param list_first_cb update callback of the top show list
 * @param list_last_cb update callback of the botoom show list
 * @return
 */

void gui_pagelist_add_list_update_cb(gui_pagelist_t *this, gui_pagelist_update_cb_t list_first_cb,
                                     gui_pagelist_update_cb_t list_last_cb);
/**
 * @brief set attribute for a pagelist widget
 *
 * @param this pagelist widget
 * @param real_list_max real quality of file lists to show. larger than show_list_max typically
 * @param show_list_max quality of switch widgets, which should be created as child nodes of this pagelist widget
 * @param list_gap_y y gap between two show lists
 * @param list_first index of the top show list
 * @param list_last index of the bottom show list
 * @return
 */

void gui_pagelist_set_att(gui_pagelist_t *this, int16_t real_list_max, int16_t show_list_max,
                          int16_t list_gap_y, gui_switch_t *list_first, gui_switch_t *list_last);
/**
 * @brief add a scroll bar picture for a pagelist widget, file source is memory address
 *
 * @param this pagelist widget
 * @param bar_pic bar picture
 * @return
 */

void gui_pagelist_add_scroll_bar(gui_pagelist_t *this, void *bar_pic);

/**
 * @brief add a scroll bar picture for a pagelist widget, file source is filesystem
 *
 * @param this pagelist widget
 * @param bar_pic bar picture filepath
 * @return
 */

void gui_pagelist_add_scroll_bar_from_fs(gui_pagelist_t *this, void *bar_pic);

/**
 * @brief create a pagelist widget
 *
 * @param parent parent of this widget
 * @param name name of this widget
 * @param x x of this widget
 * @param y y of this widget
 * @param w w of this widget
 * @param h h of this widget
 * @return gui_pagelist_t*
 */
gui_pagelist_t *gui_pagelist_create(void *parent,  const char *name,
                                    int16_t x, int16_t y, int16_t w, int16_t h);

/** End of WIDGET_Exported_GUI_Functions
  * @}
  */


/** End of WIDGET
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif

