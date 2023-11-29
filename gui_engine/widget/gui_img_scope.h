/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_img_scope.h
  * @brief  create a picture with scope
  * @details only display pixels in scope
  * @author triton_yu@realsil.com.cn
  * @date 2023/11/8
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
#ifndef __GUI_IMG_SCOPE_H__
#define __GUI_IMG_SCOPE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>
#include <draw_img.h>
#include "gui_img.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct gui_img_scope
{
    gui_img_t base;
    uint16_t scope_x1;
    uint16_t scope_x2;
    uint16_t scope_y1;
    uint16_t scope_y2;
} gui_img_scope_t;


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
 * @brief create a picture with scope,only display pixels in scope
 *
 * @param parent parent widget
 * @param addr picture address
 * @param x the x coordinate
 * @param y the y coordinate
 * @return gui_img_scope_t*
 */
gui_img_scope_t *gui_img_scope_create(void *parent, void *addr, int16_t x, int16_t y);



#ifdef __cplusplus
}
#endif

#endif

