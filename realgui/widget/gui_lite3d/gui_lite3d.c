/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
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

    // Check if obj->matrix is identity matrix
    l3_3x3_matrix_t *parent_matrix = NULL;
    if (!matrix_is_identity(obj->matrix))
    {
        parent_matrix = (l3_3x3_matrix_t *)obj->matrix;
    }

    l3_push(this->model, parent_matrix);

    touch_info_t *tp = tp_get_info();
    if (tp->type == TOUCH_SHORT)
    {
        if (l3_model_on_click(this->model, tp->x, tp->y) == true)
        {
            gui_log("l3_model_on_click\n");
            gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
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
    obj->has_end_cb = false;
    obj->has_destroy_cb = true;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_lite3d_t *gui_lite3d_create(void                  *parent,
                                const char            *name,
                                l3_model_base_t       *model,
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

