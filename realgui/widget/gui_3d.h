/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file
  * @brief
  * @details
  * @author
  * @date
  * @version
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
typedef struct gui_3d
{
    gui_obj_t base;

    void *obj_data; //obj file
    uint32_t obj_len; //obj file length

    void *mtl_data; //mtl file
    uint32_t mtl_len; //mtl file length

    // unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
    unsigned int flags;

    tinyobj_attrib_t attrib;
    tinyobj_shape_t *shapes;
    size_t num_shapes;
    tinyobj_material_t *materials;
    size_t num_materials;

    gui_3d_face_t *face;
    draw_img_t *img;

    void (*shape_transform_cb)(struct gui_3d *this, size_t s/*shape_offset*/, gui_3d_world_t *world,
                               gui_3d_camera_t *camera);

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
 * @brief
 *
 * @param parent
 * @param name
 * @param addr
 * @param x
 * @param y
 * @param w
 * @param h
 * @return gui_3d_t*
 */
gui_3d_t *gui_3d_create(void       *parent,
                        const char *name,
                        int16_t     x,
                        int16_t     y,
                        int16_t     w,
                        int16_t     h);

void gui_3d_set_obj(gui_3d_t *this, void *data, uint32_t len);
void gui_3d_set_mtl(gui_3d_t *this, void *data, uint32_t len);

void gui_3d_set_shape_transform_cb(gui_3d_t *this, size_t s/*shape_offset*/,
                                   void (*cb)(gui_3d_t *this, size_t s, gui_3d_world_t *world, gui_3d_camera_t *camera));

#ifdef __cplusplus
}
#endif

#endif
