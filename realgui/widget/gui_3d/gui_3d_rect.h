/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_3d_rect.h
  * @brief 3D widget for objects composed of rectangle mesh
  * @details 3D widget for objects composed of rectangle mesh
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/7
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
#ifndef __GUI_3D_RECT_H__
#define __GUI_3D_RECT_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "def_3d_rect.h"
#include "gui_3d.h"

/*============================================================================*
 *                         Types
 *============================================================================*/


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
 * @brief 3d widget create
 *
 * @param parent parent widget
 * @param name widget name
 * @param desc description file data
 * @param x the X-axis coordinate relative to parent widget
 * @param y the Y-axis coordinate relative to parent widget
 * @param w width
 * @param h height
 * @return the widget object pointer
 */
gui_3d_t *gui_3d_rect_create(void                  *parent,
                             const char            *name,
                             gui_3d_description_t  *desc,
                             GUI_3D_DRAW_TYPE       type,
                             int16_t                x,
                             int16_t                y,
                             int16_t                w,
                             int16_t                h);


/**
 * @brief Set the image for a specific face of the 3D rectangle widget.
 *
 * @param this Pointer to the 3D widget.
 * @param face_index Index of the face to set the image.
 * @param image_addr Pointer to the image data to be set for the face.
 */
void gui_3d_rect_set_face_image(gui_3d_t *this, uint8_t face_index, void *image_addr);


#ifdef __cplusplus
}
#endif

#endif
