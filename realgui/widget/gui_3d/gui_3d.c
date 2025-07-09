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
#include "acc_api.h"
#include "gui_fb.h"
#include <guidef.h>
#include "gui_3d.h"
#include "gui_3d_rect.h"
#include "gui_3d_tria.h"


/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_3d_t *gui_3d_create(void                 *parent,
                        const char           *name,
                        void                 *desc_addr,
                        GUI_3D_DRAW_TYPE      type,
                        int16_t               x,
                        int16_t               y,
                        int16_t               w,
                        int16_t               h)
{
    gui_3d_description_t *desc = gui_load_3d_description((void *)desc_addr);

    switch (desc->face_type)
    {
    case GUI_3D_FACE_RECTANGLE:
        return gui_3d_rect_create(parent, name, desc, type, x, y, w, h);

    case GUI_3D_FACE_TRIANGLE:
        return gui_3d_tria_create(parent, name, desc, type, x, y, w, h);

    default:
        return NULL;
    }

}

void gui_3d_set_face_image(gui_3d_t *this, uint8_t face_index, void *image_addr)
{
    if (this->desc->face_type == GUI_3D_FACE_RECTANGLE)
    {
        gui_3d_rect_set_face_image(this, face_index, image_addr);
    }
    else
    {
        //todo
    }
}

void gui_3d_set_global_transform_cb(gui_3d_t *this, gui_3d_global_transform_cb cb)
{
    this->global_transform_cb = (void (*)(gui_3d_t *))cb;
}

void gui_3d_set_face_transform_cb(gui_3d_t *this, gui_3d_face_transform_cb cb)
{
    this->face_transform_cb = (gui_3d_matrix_t (*)(gui_3d_t *, size_t))cb;
}


void gui_3d_on_click(gui_3d_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED, parameter);
}
