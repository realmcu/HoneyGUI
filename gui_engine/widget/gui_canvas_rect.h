/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_canvas_rect.h
  * @brief image canvas rect widget header file
  * @details image widget is used to show image on the screen
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
typedef struct gui_canvas_rect
{
    gui_obj_t base;
    draw_img_t *draw_img;
    gui_color_t color;
    uint8_t opacity_value;
} gui_canvas_rect_t;

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
 * @brief
 *
 * @param this
 * @param opacity_value
 */
void gui_canvas_rect_set_opacity(gui_canvas_rect_t *this, unsigned char opacity_value);


/**
 * @brief set canvas rect color
 *
 * @param this
 * @param color
 */
void gui_canvas_rect_set_color(gui_canvas_rect_t *this, gui_color_t  color);


/**
 * @brief set canvas rect location, start X and Y
 *
 * @param this
 * @param start x
 * @param start y
 */
void gui_canvas_rect_set_location(gui_canvas_rect_t *this, int x, int y);


/**
 * @brief set canvas rect size, width and heigh
 *
 * @param this
 * @param width
 * @param heigh
 */
void gui_canvas_rect_set_size(gui_canvas_rect_t *this, int w, int h);


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
