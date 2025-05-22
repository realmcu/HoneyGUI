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
#include "guidef.h"
#include "gui_api.h"
#include "gui_canvas.h"
#include "gui_colorwheel.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

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
 * @brief Create a colorwheel widget.
 * @param parent Parent widget.
 * @param name Widget name.
 * @param addr Address.
 * @param x Left.
 * @param y Top.
 * @param w Width.
 * @param h Height.
 * @return Return the colorwheel widget pointer.
 */
gui_colorwheel_t *gui_colorwheel_create(void       *parent,
                                        const char *name,
                                        void       *addr,
                                        int16_t     x,
                                        int16_t     y,
                                        int16_t     w,
                                        int16_t     h);

/**
 * @brief Set the size of the colorwheel.
 * @param this Widget pointer.
 * @param x Left.
 * @param y Top.
 * @param w Width.
 * @param h Height.
 * @param t One frame time.
 */
void gui_colorwheel_set(gui_colorwheel_t *this, float x, float y, float w, float h, float t);


#ifdef __cplusplus
}
#endif

#endif
