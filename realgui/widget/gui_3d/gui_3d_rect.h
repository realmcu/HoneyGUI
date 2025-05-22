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


/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct gui_3d_rect
{
    gui_obj_t base;

    gui_3d_description_t *desc;
    gui_3d_rect_face_t *face;
    draw_img_t *img;
    draw_img_t *mask_img;
    gui_animate_t *animate;

    void (*global_shape_transform_cb)(struct gui_3d_rect *this, gui_3d_world_t *world,
                                      gui_3d_camera_t *camera, gui_3d_light_t *light);
    void (*local_shape_transform_cb)(struct gui_3d_rect *this, size_t s/*shape_offset*/,
                                     gui_3d_world_t *world,
                                     gui_3d_camera_t *camera, gui_3d_light_t *light);

} gui_3d_rect_t;

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
 * @brief Creates a 3D widget for rectangular mesh objects.
 *
 * @param parent Parent widget.
 * @param name Widget name.
 * @param desc Description file data.
 * @param x The X-axis coordinate relative to the parent widget.
 * @param y The Y-axis coordinate relative to the parent widget.
 * @param w Width.
 * @param h Height.
 * @return The widget object pointer.
 */
gui_3d_rect_t *gui_3d_rect_create(void                  *parent,
                                  const char            *name,
                                  gui_3d_description_t  *desc,
                                  int16_t                x,
                                  int16_t                y,
                                  int16_t                w,
                                  int16_t                h);

/**
 * @brief Sets the global shape transform callback for the 3D widget.
 *
 * @param this The 3D widget pointer.
 * @param cb Callback function for the world coordinate system, camera coordinate system, and light source for all faces.
 */
void gui_3d_rect_set_global_shape_transform_cb(gui_3d_rect_t *this,
                                               void (*cb)(gui_3d_rect_t *this, gui_3d_world_t *world, gui_3d_camera_t *camera,
                                                          gui_3d_light_t *light));

/**
 * @brief Sets the local shape transform callback for the 3D widget.
 *
 * @param this The 3D widget pointer.
 * @param face Face offset.
 * @param cb Callback function for the world coordinate system, camera coordinate system, and light source for the specified face.
 */
void gui_3d_rect_set_local_shape_transform_cb(gui_3d_rect_t *this, size_t face,
                                              void (*cb)(gui_3d_rect_t *this, size_t face, gui_3d_world_t *world, gui_3d_camera_t *camera,
                                                         gui_3d_light_t *light));

/**
 * @brief Sets 3D animation effects for the widget.
 *
 * @param this The 3D widget pointer.
 * @param dur Animation time cost in milliseconds.
 * @param repeat_count Number of rounds to repeat.
 * @param callback Callback function for every frame.
 * @param p Parameter for the callback function.
 */
void gui_3d_rect_set_animate(gui_3d_rect_t *this,
                             uint32_t       dur,
                             int            repeat_count,
                             void          *callback,
                             void          *p);

/**
 * @brief Initializes the attributes of a 3D widget.
 *
 * @param this Pointer to the 3D widget.
 * @param parent Pointer to the parent GUI object.
 * @param name Name of the widget.
 * @param desc_addr Pointer to description data.
 * @param x X-axis coordinate.
 * @param y Y-axis coordinate.
 * @param w Width of the widget.
 * @param h Height of the widget.
 */
void gui_3d_rect_ctor(gui_3d_rect_t          *this,
                      gui_obj_t              *parent,
                      const char             *name,
                      gui_3d_description_t   *desc,
                      int16_t                 x,
                      int16_t                 y,
                      int16_t                 w,
                      int16_t                 h);

/**
 * @brief Sets a callback function for when the 3D widget is clicked.
 *
 * @param this Pointer to the 3D widget.
 * @param callback Callback function to execute on click.
 * @param parameter Additional parameter for the callback.
 */
void gui_3d_rect_on_click(gui_3d_rect_t *this, void *callback, void *parameter);

#ifdef __cplusplus
}
#endif

#endif
