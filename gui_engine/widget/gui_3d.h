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
typedef struct
{
    gui_obj_t base;

    draw_img_t *img;

    void *test_img_data;

    void *data; //obj file
    uint32_t len; //obj file length

    // unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
    unsigned int flags;

    tinyobj_attrib_t attrib;
    tinyobj_shape_t *shapes;
    size_t num_shapes;
    tinyobj_material_t *materials;
    size_t num_materials;

    gui_3d_face_t *face;

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
                        void       *data,
                        uint32_t    len,
                        int16_t     x,
                        int16_t     y,
                        int16_t     w,
                        int16_t     h);



#ifdef __cplusplus
}
#endif

#endif
