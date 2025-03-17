/**
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
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include <math.h>
#include "guidef.h"
#include "gui_obj.h"
#include "tp_algo.h"
#include "draw_img.h"
#include "acc_init.h"
#include "gui_fb.h"
#include <guidef.h>
#include "gui_3d.h"
#include "gui_3d_rect.h"
#include "gui_3d_tria.h"


/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void *gui_3d_create(void                 *parent,
                    const char           *name,
                    void                 *desc_addr,
                    int16_t               x,
                    int16_t               y,
                    int16_t               w,
                    int16_t               h)
{
    gui_3d_description_t *desc = gui_load_3d_description((void *)desc_addr);

    switch (desc->face_type)
    {
    case GUI_3D_FACE_RECTANGLE:
        return gui_3d_rect_create(parent, name, desc, x, y, w, h);

    case GUI_3D_FACE_TRIANGLE:
        return gui_3d_tria_create(parent, name, desc, x, y, w, h);

    default:
        return NULL;
    }

}


void gui_3d_set_global_shape_transform_cb(void *this, gui_3d_shape_transform_cb cb)
{
    gui_3d_base_t *base = (gui_3d_base_t *)this;
    switch (base->desc->face_type)
    {
    case GUI_3D_FACE_RECTANGLE:
        ((gui_3d_rect_t *)this)->global_shape_transform_cb = (void (*)(gui_3d_rect_t *, gui_3d_world_t *,
                                                                       gui_3d_camera_t *, gui_3d_light_t *))cb;
        break;

    case GUI_3D_FACE_TRIANGLE:
        ((gui_3d_tria_t *)this)->global_shape_transform_cb = (void (*)(gui_3d_tria_t *, gui_3d_world_t *,
                                                                       gui_3d_camera_t *))cb;
        break;

    default:
        break;
    }
}

void gui_3d_set_local_shape_transform_cb(void *this, size_t face, gui_3d_shape_transform_cb cb)
{
    gui_3d_base_t *base = (gui_3d_base_t *)this;
    switch (base->desc->face_type)
    {
    case GUI_3D_FACE_RECTANGLE:
        ((gui_3d_rect_t *)this)->local_shape_transform_cb = (void (*)(gui_3d_rect_t *, size_t,
                                                                      gui_3d_world_t *, gui_3d_camera_t *, gui_3d_light_t *))cb;
        break;

    case GUI_3D_FACE_TRIANGLE:
        ((gui_3d_tria_t *)this)->local_shape_transform_cb = (void (*)(gui_3d_tria_t *, size_t,
                                                                      gui_3d_world_t *, gui_3d_camera_t *))cb;
        break;

    default:
        break;
    }
}


void gui_3d_on_click(void *this, void *callback, void *parameter)
{
    gui_3d_base_t *base = (gui_3d_base_t *)this;
    switch (base->desc->face_type)
    {
    case GUI_3D_FACE_RECTANGLE:
        gui_3d_rect_on_click((gui_3d_rect_t *)this, callback, parameter);
        break;

    default:
        break;
    }
}
