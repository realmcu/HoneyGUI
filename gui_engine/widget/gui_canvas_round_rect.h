/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_canvas_round_rect.h
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
#ifndef __GUI_CANVAS_ROUND_RECT_H__
#define __GUI_CANVAS_ROUND_RECT_H__
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
typedef struct gui_canvas_round_rect
{
    gui_obj_t base;
    draw_img_t *circle_00;
    draw_img_t *circle_01;
    draw_img_t *circle_10;
    draw_img_t *circle_11;
    uint8_t *circle_data;
    draw_img_t *rect_0;
    draw_img_t *rect_1;
    draw_img_t *rect_2;
    uint8_t *rect_data;
    int  r;
    gui_color_t color;
    uint8_t opacity_value;
    bool use_external_picture;
} gui_canvas_round_rect_t;

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
gui_canvas_round_rect_t *gui_canvas_round_rect_create(gui_obj_t   *parent,
                                                      const char *name,
                                                      int          x,
                                                      int          y,
                                                      int          w,
                                                      int          h,
                                                      int          r,
                                                      gui_color_t  color);

/**
 * @brief Set the opacity of the canvas round rectangle.
 *
 * This function sets the opacity level for a given round rectangle on the canvas.
 * The opacity value ranges from `0` (fully transparent) to `255` (fully opaque).
 *
 * @param _this           Pointer to the gui_canvas_round_rect_t structure.
 * @param opacity_value   The opacity value to be set (0-255).
 */
void gui_canvas_round_rect_set_opacity(gui_canvas_round_rect_t *_this, unsigned char opacity_value);


/**
 * @brief Set the color of the canvas round rectangle.
 *
 * This function assigns a specific color to the round rectangle on the canvas.
 *
 * @param _this  Pointer to the gui_canvas_round_rect_t structure.
 * @param color  The color value to be set, represented as gui_color_t.
 */
void gui_canvas_round_rect_set_color(gui_canvas_round_rect_t *_this, gui_color_t color);


/**
 * @brief Set the location of the canvas round rectangle.
 *
 * This function specifies the starting coordinates (X and Y) for the round rectangle
 * on the canvas.
 *
 * @param _this  Pointer to the gui_canvas_round_rect_t structure.
 * @param x      The start X-coordinate.
 * @param y      The start Y-coordinate.
 */
void gui_canvas_round_rect_set_location(gui_canvas_round_rect_t *_this, int x, int y);


/**
 * @brief
 *
 * @param _this
 * @param data
 */
void gui_canvas_round_rect_set_external_picture(gui_canvas_round_rect_t *_this, void *data);

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
