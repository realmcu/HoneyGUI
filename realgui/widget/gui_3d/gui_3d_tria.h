/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_3d_tria.h
  * @brief 3D widget for objects composed of triangular mesh
  * @details 3D widget for objects composed of triangular mesh
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
#ifndef __GUI_3D_TRIA_H__
#define __GUI_3D_TRIA_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "def_3d_tria.h"
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
gui_3d_t *gui_3d_tria_create(void                  *parent,
                             const char            *name,
                             gui_3d_description_t  *desc,
                             GUI_3D_DRAW_TYPE       type,
                             int16_t                x,
                             int16_t                y,
                             int16_t                w,
                             int16_t                h);


#ifdef __cplusplus
}
#endif

#endif
