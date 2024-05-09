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
