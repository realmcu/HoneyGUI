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
#include "guidef.h"
#include "gui_obj.h"
#include "gui_fb.h"
#include "gui_lite3d.h"
#include "draw_img.h"
#include "tp_algo.h"

static void gui_lite3d_prepare(gui_obj_t *obj)
{
    gui_lite3d_t *this = (gui_lite3d_t *)obj;
    l3_push(this->model);

    touch_info_t *tp = tp_get_info();
    if (tp->type == TOUCH_SHORT)
    {
        if (this->model->draw_type == L3_DRAW_FRONT_AND_SORT)
        {
            const int target_x = this->model->combined_img->img_target_x;
            const int target_y = this->model->combined_img->img_target_y;
            const int target_w = this->model->combined_img->img_target_w;
            const int target_h = this->model->combined_img->img_target_h;

            if (tp->x >= target_x &&
                tp->x <= (target_x + target_w) &&
                tp->y >= target_y &&
                tp->y <= (target_y + target_h))
            {
                gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
            }
        }
        else
        {
            // Cache the num_face_num_verts
            const int num_face_vertices = this->model->desc->attrib.num_face_num_verts;

            for (int i = 0; i < num_face_vertices; ++i)
            {
                // Cache the img_target and face attributes
                const int target_x = this->model->img[i].img_target_x;
                const int target_y = this->model->img[i].img_target_y;
                const int target_w = this->model->img[i].img_target_w;
                const int target_h = this->model->img[i].img_target_h;

                if (tp->x >= target_x &&
                    tp->x <= (target_x + target_w) &&
                    tp->y >= target_y &&
                    tp->y <= (target_y + target_h))
                {
                    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
                    break;
                }
            }
        }

    }

    gui_fb_change();
}

static void gui_lite3d_draw(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_lite3d_t *this = (gui_lite3d_t *)obj;

    l3_set_target_canvas(this->model, dc->section.x1, dc->section.y1,
                         dc->fb_width, dc->fb_height, 16/*LITE_RGB565*/, dc->frame_buf);
    l3_draw(this->model);

}

static void gui_lite3d_end(gui_obj_t *obj)
{
    gui_lite3d_t *this = (gui_lite3d_t *)obj;
    if (this->model->img != NULL)
    {
        for (unsigned int i = 0; i < this->model->desc->attrib.num_face_num_verts; i++)
        {
            if (draw_img_acc_end_cb != NULL)
            {
                l3_draw_rect_img_t *img = &this->model->img[i];
                draw_img_acc_end_cb((draw_img_t *)img);
            }
        }
    }
}

static void gui_lite3d_destroy(gui_obj_t *obj)
{
    gui_lite3d_t *this = (gui_lite3d_t *)obj;
    if (this->model != NULL)
    {
        l3_free_model(this->model);
        this->model = NULL;
    }
}

static void gui_lite3d_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_lite3d_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_lite3d_draw(obj);
            break;

        case OBJ_END:
            gui_lite3d_end(obj);
            break;

        case OBJ_DESTROY:
            gui_lite3d_destroy(obj);
            break;

        default:
            break;
        }
    }
}



static void gui_lite3d_ctor(gui_lite3d_t  *this,
                            gui_obj_t     *parent,
                            const char    *name,
                            int16_t        x,
                            int16_t        y,
                            int16_t        w,
                            int16_t        h)
{
    gui_obj_t *obj = (gui_obj_t *)this;

    gui_obj_ctor(obj, parent, name, x, y, w, h);

    obj->obj_cb = gui_lite3d_cb;
    obj->has_prepare_cb = true;
    obj->has_draw_cb = true;
    obj->has_end_cb = true;
    obj->has_destroy_cb = true;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_lite3d_t *gui_lite3d_create(void                  *parent,
                                const char            *name,
                                l3_model_t            *model,
                                int16_t                x,
                                int16_t                y,
                                int16_t                w,
                                int16_t                h)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "GUI_LITE3D";
    }

    gui_lite3d_t *this = gui_malloc(sizeof(gui_lite3d_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_lite3d_t));

    this->model = model;
    gui_lite3d_ctor(this, (gui_obj_t *)parent, name, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}

void gui_lite3d_on_click(gui_lite3d_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED, parameter);
}

