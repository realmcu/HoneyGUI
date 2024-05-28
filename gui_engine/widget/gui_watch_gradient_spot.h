/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_watch_gradient_spot.h
  * @brief  This file is part of GUI Engine
  * @details  create watch gradient spot
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
#ifndef __GUI_WATCH_GRADIENT_SPOT_H__
#define __GUI_WATCH_GRADIENT_SPOT_H__



#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "gui_canvas.h"
#include "gui_watch_gradient_spot.h"

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

/** @brief  ... */
typedef struct
{
    gui_obj_t base;
    float c_x;
    float c_y;
} gui_watch_gradient_spot_t;


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
#define WATCH_GRADIENT_SPOT_BASE      454.0f
#define WATCH_GRADIENT_SPOT_SCLAE     ((float)(GET_BASE(this)->w)/WATCH_GRADIENT_SPOT_BASE)

extern NVGcontext *nvgCreateAGGE(uint32_t w, uint32_t h, uint32_t stride,
                                 enum NVGtexture format, uint8_t *data);
extern void nvgDeleteAGGE(NVGcontext *ctx);

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
  * @brief  create a watch widget
  * @param  parent  parent widget
  * @param  name    widget name
  * @param  x       left
  * @param  y       top
  * @param  w       width
  * @param  h       high
  * @return         gui_watch_gradient_spot_t* widget pointer
  *
  * <b>Example usage</b>
  * \code{.c}
  * void example_watch(void *parent)
  * {
  *     win_watch = gui_win_create(parent, "win", 0, 0, 368, 448);
  *     gui_watch_gradient_spot_t *watch = gui_watch_gradient_spot_create(win_watch, "watchface", 0, 0, 0, 0);
  *
  * }
  * \endcode
  */
gui_watch_gradient_spot_t *gui_watch_gradient_spot_create(void       *parent,
                                                          const char *name,
                                                          int16_t     x,
                                                          int16_t     y,
                                                          int16_t     w,
                                                          int16_t     h);

/**
  * @brief  set watch center
  * @param  this  gui_watch_gradient_spot_t widget
  * @param  c_x   left
  * @param  c_y   top
  * @return       void
  *
  * <b>Example usage</b>
  * \code{.c}
  * void example_watch(void *parent)
  * {
  *     win_watch = gui_win_create(parent, "win", 0, 0, 368, 448);
  *     gui_watch_gradient_spot_t *watch = gui_watch_gradient_spot_create(win_watch, "watchface", 0, 0, 0, 0);
  *    gui_watch_gradient_spot_t *watch = gui_watch_gradient_spot_set_center(watch, 368 / 2, 448 / 2);
  *
  * }
  * \endcode
  */
void gui_watch_gradient_spot_set_center(gui_watch_gradient_spot_t *this, float c_x, float c_y);

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
