/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_colorwheel.h
  * @brief colorwheel widget
  * @details colorwheel widget
  * @author howie_wang@realsil.com.cn
  * @date 2023/11/07
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
#ifndef __GUI_COLORWHEEL_H__
#define __GUI_COLORWHEEL_H__



#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>
#include <gui_canvas.h>
#include "gui_colorwheel.h"


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

/** @brief  COLORWHEEL widget structure */
typedef struct gui_colorwheel
{
    gui_obj_t base;
    float x;
    float y;
    float w;
    float h;
    float t;
} gui_colorwheel_t;

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
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @brief
  * @{
  */

/**
 * @brief colorwheel create
 *
 * @param parent parent widget
 * @param name  widget name
 * @param addr
 * @param x  left
 * @param y  top
 * @param w  width
 * @param h  hight
 * @return gui_colorwheel_t* widget pointer
 * <b>Example usage</b>
 * \code{.c}
 *{
 *    gui_dispdev_t *dc = gui_get_dc();
 *
 *    gui_colorwheel_t *colorwheel = gui_colorwheel_create(parent,  "tb_10", NULL, 0, 0, 454, 454);
 *    gui_colorwheel_set(colorwheel, 0, 0, dc->screen_width, dc->screen_height, 0);
 *}
 * \endcode
 */
gui_colorwheel_t *gui_colorwheel_create(void *parent,  const char *name, void *addr,
                                        int16_t x, int16_t y, int16_t w, int16_t h);

/**
 * @brief set colorwheel size
 *
 * @param this widget pointer
 * @param x  left
 * @param y  top
 * @param w  width
 * @param h  hight
 * @param t  one frame time
 */
void gui_colorwheel_set(gui_colorwheel_t *this, float x, float y, float w, float h, float t);


/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif

