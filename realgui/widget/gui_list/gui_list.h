/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_list.c
  * @brief the list widget is a container for any number of list table, and each table can have other widgets added to it.
  * @author shel_deng@realsil.com.cn
  * @date 2025/04/23
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_LIST_H__
#define __GUI_LIST_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_fb.h"
#include "gui_obj.h"
#include "gui_matrix.h"

/*============================================================================*
 *                         Types
 *============================================================================*/
/* LIST_STYLE enum start*/
typedef enum
{
    LIST_CLASSIC = 0x0000, ///< Classic list
    LIST_CIRCLE  = 0x0001, ///< Circle list
    LIST_ZOOM    = 0x0002, ///< Zoom center list
    LIST_CARD    = 0x0003, ///< Stack like card
} LIST_STYLE;

/* LIST_STYLE enum end*/

typedef enum
{
    VERTICAL    = 0x0000,
    HORIZONTAL  = 0x0001,
} LIST_DIR;

/** @brief  list structure */

typedef struct gui_list
{
    gui_obj_t base;
    LIST_STYLE style;
    LIST_DIR dir;        // 0:vertical, 1:horizontal
    uint8_t widget_num;
    uint8_t space;
    uint16_t tab_length; // list tab length
    int16_t speed;
    int16_t record[5];

    int offset;         // offset = hold + tp_delta, when sliding
    int hold;
    int total_length;

    uint8_t checksum;
} gui_list_widget_t;

typedef struct gui_list_tab
{
    gui_obj_t base;
    int start_x;
    int start_y;

    uint8_t checksum;
} gui_list_tab_t;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/
#define OUT_SCOPE 100 // out scope of list
#define GUI_MAX_SPEED 50
#define GUI_MIN_SPEED 7
/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Create a list widget.
 * @param parent The father widget it nested in.
 * @param name The name of the widget.
 * @param x The X-axis coordinate relative to parent widget
 * @param y The Y-axis coordinate relative to parent widget
 * @param w Width
 * @param h Height
 * @param tab_length The length of each tab.
 * @param space The space of each tab.
 * @param dir The direction of the list.
 * @return return the widget object pointer.
 */
gui_list_widget_t *gui_list_create(void       *parent,
                                   const char *name,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h,
                                   uint16_t    tab_length,
                                   uint8_t     space,
                                   LIST_DIR    dir);

/**
 * @brief Add list tab.
 * @param list The list widget it nested in.
 * @return return the tab object pointer.
 */
gui_list_tab_t *gui_list_add_tab(gui_list_widget_t *list);

/**
 * @brief Set list moving style.
 * @param list Pointer to the list widget.
 * @param style The moving style of the list.
 */
void gui_list_set_style(gui_list_widget_t *list, LIST_STYLE style);


#ifdef __cplusplus
}
#endif
#endif
