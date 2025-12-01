/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_canvas_rect.h
  * @brief Image canvas rect widget header file.
  * @details Image widget is used to show image on the screen.
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
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
#ifndef __GUI_CANVAS_RECT_H__
#define __GUI_CANVAS_RECT_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>
#include <draw_img.h>
#include <gui_matrix.h>
#include "gui_obj.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct gui_canvas_rect
{
    gui_obj_t base;
    draw_img_t *draw_img;
    gui_color_t color;
    uint8_t opacity_value;
    uint8_t tp_block : 1;
} gui_canvas_rect_t;


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
 * @brief
 *
 * @param parent
 * @param x
 * @param y
 * @param w
 * @param h
 * @param r
 * @param color
 * @return gui_canvas_rect_t*
 */
gui_canvas_rect_t *gui_canvas_rect_create(gui_obj_t   *parent,
                                          const char *name,
                                          int          x,
                                          int          y,
                                          int          w,
                                          int          h,
                                          gui_color_t  color);


/**
 * @brief set cavas_rect as tp blocker, tp will not be recived by widget under cavas_rect.
 *
 * @param this the cavas_rect widget pointer.
 * @param block block or not.
 */
void gui_canvas_rect_set_tp_block(gui_canvas_rect_t *, bool block);


/**
 * @brief
 *
 * @param this
 * @param opacity_value
 */
void gui_canvas_rect_set_opacity(gui_canvas_rect_t *, unsigned char opacity_value);


/**
 * @brief set canvas rect color
 *
 * @param this
 * @param color
 */
void gui_canvas_rect_set_color(gui_canvas_rect_t *, gui_color_t  color);


/**
 * @brief set canvas rect location, start X and Y
 *
 * @param this
 * @param start x
 * @param start y
 */
void gui_canvas_rect_set_location(gui_canvas_rect_t *, int x, int y);


/**
 * @brief set canvas rect size, width and heigh
 *
 * @param this
 * @param width
 * @param heigh
 */
void gui_canvas_rect_set_size(gui_canvas_rect_t *, int w, int h);



#ifdef __cplusplus
}
#endif

#endif
