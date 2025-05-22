/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_img_scope.h
  * @brief  Create a picture with scope.
  * @details Only display pixels in scope.
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
#include "guidef.h"
#include "gui_api.h"
#include "draw_img.h"
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
    int16_t ax, ay;
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
 * @brief Create a picture with scope. Only display pixels in scope.
 *
 * @param parent Parent widget.
 * @param addr Ftl or xip address, use xip address.
 * @param x The x coordinate.
 * @param y The y coordinate.
 * @return gui_img_scope_t*
 */
gui_img_scope_t *gui_img_scope_create(void    *parent,
                                      const char *name,
                                      void    *addr,
                                      int16_t  x,
                                      int16_t  y);
/**
 * @brief Create a picture from with scope. Only display pixels in scope.
 *
 * @param parent Parent widget.
 * @param ftl Not xip address, use ftl address.
 * @param x The x coordinate.
 * @param y The y coordinate.
 * @return gui_img_scope_t*
 */
gui_img_scope_t *gui_img_scope_create_from_ftl(void    *parent,
                                               const char *name,
                                               void    *ftl,
                                               int16_t  x,
                                               int16_t  y);

/**
 * @brief Set the range of the image scope.
 *
 * @param _this The image scope object.
 * @param x1 The x1 coordinate.
 * @param y1 The y1 coordinate.
 * @param x2 The x2 coordinate.
 * @param y2 The y2 coordinate.
 */
void gui_img_scope_set_range(gui_img_scope_t *_this, int16_t x1, int16_t y1, int16_t x2,
                             int16_t y2);


#ifdef __cplusplus
}
#endif

#endif
