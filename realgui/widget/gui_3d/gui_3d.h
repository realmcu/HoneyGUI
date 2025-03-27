/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_3d.h
  * @brief 3D widget
  * @details 3D widget
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
#ifndef __GUI_3D_H__
#define __GUI_3D_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "def_3d_common.h"
#include "def_3d_rect.h"

/*============================================================================*
 *                         Types
 *============================================================================*/
typedef void (*gui_3d_shape_transform_cb)(void *this, gui_3d_world_t *world,
                                          gui_3d_camera_t *camera, void *extra);

typedef struct
{
    gui_obj_t base;
    gui_3d_description_t *desc;
} gui_3d_base_t;

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
void *gui_3d_create(void                 *parent,
                    const char           *name,
                    gui_3d_description_t *desc,
                    int16_t               x,
                    int16_t               y,
                    int16_t               w,
                    int16_t               h);

/**
 * @brief set global shape transform callback
 *
 * @param this the 3d widget pointer
 * @param cb Set callback functions for the world coordinate system, camera coordinate system,
 *           and light source for all faces
 */
void gui_3d_set_global_shape_transform_cb(void *this, gui_3d_shape_transform_cb cb);

/**
 * @brief set local shape transform callback
 *
 * @param this the 3d widget pointer
 * @param face face offset
 * @param cb Set callback functions for the world coordinate system, camera coordinate system,
 *           and light source for the specified face
 */
void gui_3d_set_local_shape_transform_cb(void *this, size_t face, gui_3d_shape_transform_cb cb);

/**
 * @brief set 3D animation effects
 *
 * @param this the 3d widget pointer
 * @param dur animation time cost in ms
 * @param repeat_count rounds to repeat
 * @param callback every frame callback
 * @param p callback's parameter
 */
void gui_3d_set_animate(void       *this,
                        uint32_t    dur,
                        int         repeat_count,
                        void       *callback,
                        void       *p);

/**
 * @brief Set a callback function for when the 3D widget is clicked.
 *
 * @param this Pointer to the 3D widget.
 * @param callback Callback function to execute on click.
 * @param parameter Additional parameter for the callback.
 */
void gui_3d_on_click(void *this, void *callback, void *parameter);


#ifdef __cplusplus
}
#endif

#endif
