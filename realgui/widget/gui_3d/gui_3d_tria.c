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
#include "gui_3d_tria.h"
#include "tp_algo.h"
#include "draw_img.h"
#include "acc_api.h"
#include "gui_fb.h"
#include <guidef.h>


static void gui_3d_generate_triangle_img(gui_3d_t *this, int width, int height)
{
    this->combined_img->data = gui_malloc(width * height * 2 + sizeof(gui_rgb_data_head_t));
    memset(this->combined_img->data, 0x00, width * height * 2 + sizeof(gui_rgb_data_head_t));
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->combined_img->data;
    head->w = width;
    head->h = height;
    head->type = RGB565;

    uint8_t *pixelData = (uint8_t *)this->combined_img->data + sizeof(gui_rgb_data_head_t);
    float *depthBuffer = gui_malloc(width * height * sizeof(float));
    memset(depthBuffer, 0x00, width * height * sizeof(float));

    uint16_t color_value = 0;
    for (uint32_t i = 0; i < this->desc->attrib.num_face_num_verts; i++)
    {
        // Non-backfaces
        if (!(this->face.tria_face[i].state & GUI_3D_FACESTATE_BACKFACE))
        {
            // Cache vertex positions
            gui_3d_vertex_t p0 = this->face.tria_face[i].transform_vertex[0];
            gui_3d_vertex_t p1 = this->face.tria_face[i].transform_vertex[1];
            gui_3d_vertex_t p2 = this->face.tria_face[i].transform_vertex[2];

            int material_id = this->desc->attrib.material_ids[i];
            if (material_id >= 0)
            {
                float *color_diffuse = this->desc->materials[material_id].diffuse;
                uint8_t color_r = (uint8_t)(*color_diffuse * 255);
                uint8_t color_g = (uint8_t)(*(color_diffuse + 1) * 255);
                uint8_t color_b = (uint8_t)(*(color_diffuse + 2) * 255);
                color_value = ((color_r & 0xF8) << 8) |
                              ((color_g & 0xFC) << 3) |
                              ((color_b & 0xF8) >> 3);
            }
            else
            {
                float nz = fabsf(p0.normal.z);
                uint8_t color_intensity = (uint8_t)(255 * fmaxf(0.0f, fminf(1.0f, nz)));
                color_value = ((color_intensity & 0xF8) << 8) |
                              ((color_intensity & 0xFC) << 3) |
                              ((color_intensity & 0xF8) >> 3);
            }

            gui_3d_fill_triangle(p0, p1, p2, depthBuffer, (uint32_t *)pixelData, width, height,
                                 GUI_3D_FILL_COLOR_RGB565, &color_value, UINT8_MAX);

        }

    }

    gui_free(depthBuffer);

    this->combined_img->img_w = width;
    this->combined_img->img_h = height;
    this->combined_img->opacity_value = UINT8_MAX;
    this->combined_img->blend_mode = IMG_FILTER_BLACK;

    gui_obj_t *obj = (gui_obj_t *)this;
    memcpy(&this->combined_img->matrix, obj->matrix, sizeof(struct gui_matrix));
    memcpy(&this->combined_img->inverse, &this->combined_img->matrix, sizeof(gui_matrix_t));
    matrix_inverse(&this->combined_img->inverse);
    draw_img_new_area(this->combined_img, NULL);
}

static void gui_3d_tria_prepare(gui_3d_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *obj = (gui_obj_t *)this;

    this->face.tria_face = gui_malloc(sizeof(gui_3d_tria_face_t) *
                                      this->desc->attrib.num_face_num_verts);
    memset(this->face.tria_face, 0x1,
           sizeof(gui_3d_tria_face_t) * this->desc->attrib.num_face_num_verts);

    this->combined_img = gui_malloc(sizeof(draw_img_t));
    memset(this->combined_img, 0x00, sizeof(draw_img_t));


    gui_3d_matrix_t transform_matrix;

    // global transform
    if (this->global_transform_cb != NULL)
    {
        this->global_transform_cb(this);
        transform_matrix = this->world;
    }

    gui_3d_camera_build_UVN_matrix(&this->camera);

    for (size_t i = 0; i < this->desc->attrib.num_face_num_verts; i++)
    {
        // local transform
        if (this->face_transform_cb != NULL)
        {
            transform_matrix = this->face_transform_cb(this, i);
        }

        gui_3d_tria_scene(this->face.tria_face + i, i, &this->desc->attrib,
                          &transform_matrix, &this->camera);

    }

    gui_3d_generate_triangle_img(this, this->camera.viewport_width, this->camera.viewport_height);

    if (tp->type == TOUCH_SHORT)
    {
        const int target_x = this->combined_img->img_target_x;
        const int target_y = this->combined_img->img_target_y;
        const int target_w = this->combined_img->img_target_w;
        const int target_h = this->combined_img->img_target_h;

        if (tp->x >= target_x &&
            tp->x <= (target_x + target_w) &&
            tp->y >= target_y &&
            tp->y <= (target_y + target_h))
        {
            gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
        }
    }

    gui_fb_change();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
}

static void gui_3d_tria_draw(gui_3d_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    draw_img_cache(this->combined_img, IMG_SRC_MEMADDR);
    gui_acc_blit_to_dc(this->combined_img, dc, NULL);
    draw_img_free(this->combined_img, IMG_SRC_MEMADDR);

}

static void gui_3d_tria_end(gui_3d_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    if (this->combined_img != NULL)
    {
        gui_free(this->combined_img->data);
        this->combined_img->data = NULL;
        if (draw_img_acc_end_cb != NULL)
        {
            draw_img_acc_end_cb(this->combined_img);
        }
        gui_free(this->combined_img);
        this->combined_img = NULL;
    }

    gui_free(this->face.tria_face);
    this->face.tria_face = NULL;
}


static void gui_3d_tria_destroy(gui_3d_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    if (this->desc != NULL)
    {
        if (this->desc->textures != NULL)
        {
            gui_free(this->desc->textures);
            this->desc->textures = NULL;
        }

        gui_free(this->desc);
        this->desc = NULL;
    }
}

static void gui_3d_tria_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_3d_tria_prepare((gui_3d_t *)obj);
            break;

        case OBJ_DRAW:
            gui_3d_tria_draw((gui_3d_t *)obj);
            break;

        case OBJ_END:
            gui_3d_tria_end((gui_3d_t *)obj);
            break;

        case OBJ_DESTROY:
            gui_3d_tria_destroy((gui_3d_t *)obj);
            break;

        default:
            break;
        }
    }
}



void gui_3d_tria_ctor(gui_3d_t          *this,
                      gui_obj_t              *parent,
                      const char             *name,
                      gui_3d_description_t   *desc,
                      int16_t                 x,
                      int16_t                 y,
                      int16_t                 w,
                      int16_t                 h)
{
    //for obj class
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_obj_ctor(obj, parent, name, x, y, w, h);

    if (this->draw_type == GUI_3D_DRAW_FRONT_AND_SORT)
    {
        obj->obj_cb = gui_3d_tria_cb;
    }

    obj->has_prepare_cb = true;
    obj->has_draw_cb = true;
    obj->has_end_cb = true;
    obj->has_destroy_cb = true;

    // for self
    this->desc = desc;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_3d_t *gui_3d_tria_create(void                  *parent,
                             const char            *name,
                             gui_3d_description_t  *desc,
                             GUI_3D_DRAW_TYPE       type,
                             int16_t                x,
                             int16_t                y,
                             int16_t                w,
                             int16_t                h)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "WIDGET_3D";
    }

    gui_3d_t *this = gui_malloc(sizeof(gui_3d_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_3d_t));

    this->draw_type = type;
    gui_3d_tria_ctor(this, (gui_obj_t *)parent, name, desc, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}

