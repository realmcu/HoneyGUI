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
#include "acc_init.h"
#include "gui_fb.h"
#include <guidef.h>


static void gui_3d_tria_update_att(gui_obj_t *obj)
{
    gui_3d_tria_t *this = (void *)obj;
    if (this->animate)
    {
        animate_frame_update(this->animate, obj);
    }
}

static void gui_3d_generate_triangle_img(gui_3d_tria_t *this, int width, int height)
{
    this->img->data = gui_malloc(width * height * 2 + sizeof(gui_rgb_data_head_t));
    memset(this->img->data, 0x00, width * height * 2 + sizeof(gui_rgb_data_head_t));
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->img->data;
    head->w = width;
    head->h = height;
    head->type = RGB565;

    uint8_t *pixelData = (uint8_t *)this->img->data + sizeof(gui_rgb_data_head_t);
    float *depthBuffer = gui_malloc(width * height * sizeof(float));
    memset(depthBuffer, 0x00, width * height * sizeof(float));

    for (uint32_t i = 0; i < this->desc->attrib.num_face_num_verts; i++)
    {
        // Non-backfaces
        if (!(this->face[i].state & GUI_3D_FACESTATE_BACKFACE))
        {
            // Cache vertex positions
            gui_3d_vertex_t *vertices = this->face[i].transform_vertex;
            float x0 = vertices[0].position.x, y0 = vertices[0].position.y, z0 = vertices[0].position.z;
            float x1 = vertices[1].position.x, y1 = vertices[1].position.y, z1 = vertices[1].position.z;
            float x2 = vertices[2].position.x, y2 = vertices[2].position.y, z2 = vertices[2].position.z;

            int min_x = floor(fminf(fminf(x0, x1), x2));
            int max_x = ceil(fmaxf(fmaxf(x0, x1), x2));
            int min_y = floor(fminf(fminf(y0, y1), y2));
            int max_y = ceil(fmaxf(fmaxf(y0, y1), y2));

            // for w0 w1
            float dx1 = x1 - x0;
            float dy1 = y1 - y0;
            float dx2 = x2 - x0;
            float dy2 = y2 - y0;
            float inv_area = 1.0f / (dx1 * dy2 - dx2 * dy1);

            float initial_w0_x_comp = dy2 * inv_area;
            float initial_w1_x_comp = dy1 * inv_area;

            uint16_t color_value = 0;
            int material_id = this->desc->attrib.material_ids[i];
            if (material_id >= 0)
            {
                float *color_diffuse = this->desc->materials[material_id].diffuse;
                int color_r = (int)(*color_diffuse * 255);
                int color_g = (int)(*(color_diffuse + 1) * 255);
                int color_b = (int)(*(color_diffuse + 2) * 255);
                color_value = ((color_r & 0xF8) << 8) |
                              ((color_g & 0xFC) << 3) |
                              ((color_b & 0xF8) >> 3);
            }
            else
            {
                float nz = vertices[0].normal.z;
                uint8_t color_intensity = (uint8_t)(255 * fmaxf(0.0f, fminf(1.0f, nz)));
                color_value = ((color_intensity & 0xF8) << 8) |
                              ((color_intensity & 0xFC) << 3) |
                              ((color_intensity & 0xF8) >> 3);
            }


            // Iterate over the bounding box
            for (int y = min_y; y <= max_y; y++)
            {
                float w0y = (y - y0) * dx2 * inv_area;
                float w1y = (y - y0) * dx1 * inv_area;

                int pixel_offset = y * width;

                for (int x = min_x; x <= max_x; x++)
                {
                    float w0 = (x - x0) * initial_w0_x_comp - w0y;
                    float w1 = w1y - (x - x0) * initial_w1_x_comp;
                    float w2 = 1.0f - w0 - w1;

                    // Inside the triangle
                    if ((w0 >= 0 && w1 >= 0 && w2 >= 0))
                    {
                        float z = w0 * z0 + w1 * z1 + w2 * z2;
                        int index = x + pixel_offset;

                        // Foreground
                        if (depthBuffer[index] == 0 || z > depthBuffer[index])
                        {
                            depthBuffer[index] = z;
                            ((uint16_t *)pixelData)[index] = color_value;
                        }
                    }
                }
            }
        }

    }

    gui_free(depthBuffer);

    this->img->img_w = width;
    this->img->img_h = height;
    this->img->opacity_value = UINT8_MAX;
    this->img->blend_mode = IMG_BYPASS_MODE;
    // this->img->high_quality = true;

    gui_obj_t *obj = (gui_obj_t *)this;
    memcpy(&this->img->matrix, obj->matrix, sizeof(struct gui_matrix));
    memcpy(&this->img->inverse, &this->img->matrix, sizeof(gui_matrix_t));
    matrix_inverse(&this->img->inverse);
    draw_img_new_area(this->img, NULL);
}

static void gui_3d_tria_prepare(gui_3d_tria_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_3d_tria_update_att(obj);

    this->face = gui_malloc(sizeof(gui_3d_tria_face_t) * this->desc->attrib.num_face_num_verts);
    memset(this->face, 0x1, sizeof(gui_3d_tria_face_t) * this->desc->attrib.num_face_num_verts);

    this->img = gui_malloc(sizeof(draw_img_t));
    memset(this->img, 0x00, sizeof(draw_img_t));

    gui_3d_world_t world = {0};
    gui_3d_camera_t camera = {0};

    // global transform
    if (this->global_shape_transform_cb != NULL)
    {
        this->global_shape_transform_cb(this, &world, &camera);
    }

    gui_3d_camera_build_UVN_matrix(&camera);

    for (size_t i = 0; i < this->desc->num_shapes; i++)
    {
        for (size_t j = 0; j < this->desc->shapes[i].length /*number of face*/; j++)
        {
            size_t face_offset = this->desc->shapes[i].face_offset + j;

            // local transform
            if (this->local_shape_transform_cb != NULL)
            {
                this->local_shape_transform_cb(this, face_offset, &world, &camera);
            }

            gui_3d_tria_scene(this->face + face_offset, face_offset, &this->desc->attrib, &world, &camera);

        }
    }

    gui_3d_generate_triangle_img(this, 380, 380);

    gui_fb_change();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
}

static void gui_3d_tria_draw(gui_3d_tria_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    draw_img_cache(this->img, IMG_SRC_MEMADDR);
    gui_acc_blit_to_dc(this->img, dc, NULL);
    draw_img_free(this->img, IMG_SRC_MEMADDR);

}

static void gui_3d_tria_end(gui_3d_tria_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    if (this->img != NULL)
    {
        gui_free(this->img->data);
        this->img->data = NULL;
        if (draw_img_acc_end_cb != NULL)
        {
            draw_img_acc_end_cb(this->img);
        }
        gui_free(this->img);
        this->img = NULL;
    }

    gui_free(this->face);
    this->face = NULL;
}


static void gui_3d_tria_destroy(gui_3d_tria_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    gui_free(this->desc->textures);
    gui_free(this->desc);
}

static void gui_3d_tria_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_3d_tria_prepare((gui_3d_tria_t *)obj);
            break;

        case OBJ_DRAW:
            gui_3d_tria_draw((gui_3d_tria_t *)obj);
            break;

        case OBJ_END:
            gui_3d_tria_end((gui_3d_tria_t *)obj);
            break;

        case OBJ_DESTROY:
            gui_3d_tria_destroy((gui_3d_tria_t *)obj);
            break;

        default:
            break;
        }
    }
}



void gui_3d_tria_ctor(gui_3d_tria_t          *this,
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

    obj->obj_cb = gui_3d_tria_cb;
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
void gui_3d_tria_set_global_shape_transform_cb(gui_3d_tria_t *this,
                                               void (*cb)(gui_3d_tria_t *this, gui_3d_world_t *world, gui_3d_camera_t *camera))
{
    this->global_shape_transform_cb = cb;
}

void gui_3d_tria_set_local_shape_transform_cb(gui_3d_tria_t *this, size_t face/*face_offset*/,
                                              void (*cb)(gui_3d_tria_t *this, size_t face, gui_3d_world_t *world, gui_3d_camera_t *camera))
{
    this->local_shape_transform_cb = cb;
}

void gui_3d_tria_set_animate(gui_3d_tria_t *this,
                             uint32_t       dur,
                             int            repeat_count,
                             void          *callback,
                             void          *p)
{
    GUI_SET_ANIMATE_HELPER
}


gui_3d_tria_t *gui_3d_tria_create(void                  *parent,
                                  const char            *name,
                                  gui_3d_description_t  *desc,
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

    gui_3d_tria_t *this = gui_malloc(sizeof(gui_3d_tria_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_3d_tria_t));

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

