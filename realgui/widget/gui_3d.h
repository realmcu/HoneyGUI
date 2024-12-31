/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_3d.h
  * @brief 3D widget
  * @details 3D widget
  * @author howie_wang@realsil.com.cn
  * @date 2024/11/16
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
#include "def_3d.h"
#include "tinyobj_loader_c.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  ... */
typedef struct
{
    tinyobj_attrib_t attrib;

    unsigned int num_shapes;
    tinyobj_shape_t *shapes;

    unsigned int num_materials;
    tinyobj_material_t *materials;

    unsigned int *texture_sizes;
    unsigned char **textures;

} gui_3d_description_t;

typedef struct gui_3d
{
    gui_obj_t base;

    // unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
    unsigned int flags;

    gui_3d_description_t *desc;
    uint8_t face_flags;
    gui_3d_face_t *face;
    draw_img_t *img;
    draw_img_t *mask_img;
    gui_animate_t *animate;

    void (*shape_transform_cb)(struct gui_3d *this, size_t s/*shape_offset*/, gui_3d_world_t *world,
                               gui_3d_camera_t *camera, gui_3d_light_t *light);

} gui_3d_t;

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
 * @param desc_addr description file data
 * @param x the X-axis coordinate relative to parent widget
 * @param y the Y-axis coordinate relative to parent widget
 * @param w width
 * @param h height
 * @return the widget object pointer
 */
gui_3d_t *gui_3d_create(void       *parent,
                        const char *name,
                        void       *desc_addr,
                        int16_t     x,
                        int16_t     y,
                        int16_t     w,
                        int16_t     h);

/**
 * @brief set shape transform callback
 *
 * @param this the 3d widget pointer
 * @param face face offset
 * @param cb Set callback functions for the world coordinate system, camera coordinate system,
 *           and light source for the specified face
 */
void gui_3d_set_shape_transform_cb(gui_3d_t *this, size_t face,
                                   void (*cb)(gui_3d_t *this, size_t face, gui_3d_world_t *world, gui_3d_camera_t *camera,
                                              gui_3d_light_t *light));

/**
 * @brief set 3D animation effects
 *
 * @param this the 3d widget pointer
 * @param dur animation time cost in ms
 * @param repeat_count rounds to repeat
 * @param callback every frame callback
 * @param p callback's parameter
 */
void gui_3d_set_animate(gui_3d_t     *this,
                        uint32_t      dur,
                        int           repeat_count,
                        void         *callback,
                        void         *p);

/**
 * @brief 3d widget constructor
 *
 * @param this the 3d widget pointer
 * @param parent parent widget
 * @param name widget name
 * @param desc_addr description file data
 * @param x the X-axis coordinate relative to parent widget
 * @param y the Y-axis coordinate relative to parent widget
 * @param w width
 * @param h height
 */
void gui_3d_ctor(gui_3d_t               *this,
                 gui_obj_t              *parent,
                 const char             *name,
                 void                   *desc_addr,
                 int16_t                 x,
                 int16_t                 y,
                 int16_t                 w,
                 int16_t                 h);

/**
 * @brief set on_click event
 *
 * @param this the 3d widget pointer
 * @param callback callback function
 * @param parameter callback function parameter
 */
void gui_3d_on_click(gui_3d_t *this, void *callback, void *parameter);

#ifdef __cplusplus
}
#endif

#endif
