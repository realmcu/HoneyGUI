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
#include "gui_3d_rect.h"
#include "tp_algo.h"
#include "def_3d_rect.h"
#include "draw_img.h"
#include "acc_init.h"
#include "gui_fb.h"
#include <guidef.h>


static float gui_3d_calculate_light_to_point(gui_3d_light_t *light, float point_x, float point_y,
                                             float point_z)
{
    gui_point_4d_t lightToPoint =
    {
        .x = point_x - light->position.x,
        .y = point_y - light->position.y,
        .z = point_z - light->position.z,
    };
    float distance = sqrt(lightToPoint.x * lightToPoint.x + lightToPoint.y * lightToPoint.y +
                          lightToPoint.z * lightToPoint.z);

    // Distance attenuation
    float constant = 1.0f, linear = 0.1f;
    float disAttenuation = 1.0f / (constant + linear * distance);

    lightToPoint = gui_point_4d_unit(lightToPoint);

    gui_point_4d_t lightDirection =
    {
        .x = light->targetDirection.x - light->position.x,
        .y = light->targetDirection.y - light->position.y,
        .z = light->targetDirection.z - light->position.z,
    };
    lightDirection = gui_point_4d_unit(lightDirection);

    // Angle attenuation
    float cosTheta = gui_point4D_dot(lightDirection, lightToPoint);

    float halfAngle = light->included_angle / 2.0f;
    float innerCircleRadius = halfAngle * (1.0f - light->blend_ratio) * (M_PI / 180.0f);
    float outerCircleRadius = halfAngle * (M_PI / 180.0f);

    float innerCos = cosf(innerCircleRadius);
    float outerCos = cosf(outerCircleRadius);

    if (cosTheta >= innerCos)
    {
        return disAttenuation;
    }
    else if (cosTheta >= outerCos)
    {
        float angleAttenuation = (cosTheta - outerCos) / (innerCos - outerCos);
        return disAttenuation * angleAttenuation;
    }
    return 0;
}

static gui_point_4d_t gui_3d_calculate_point_in_quad(gui_point_4d_t P0, gui_point_4d_t P1,
                                                     gui_point_4d_t P2,
                                                     gui_point_4d_t P3, float u, float v)
{
    gui_point_4d_t point;

    // Horizontal interpolation
    gui_point_4d_t Q0, Q1;
    Q0.x = (1 - u) * P3.x + u * P0.x;
    Q0.y = (1 - u) * P3.y + u * P0.y;
    Q0.z = (1 - u) * P3.z + u * P0.z;
    Q0.w = (1 - u) * P3.w + u * P0.w;

    Q1.x = (1 - u) * P2.x + u * P1.x;
    Q1.y = (1 - u) * P2.y + u * P1.y;
    Q1.z = (1 - u) * P2.z + u * P1.z;
    Q1.w = (1 - u) * P2.w + u * P1.w;

    // Vertical interpolation
    point.x = (1 - v) * Q1.x + v * Q0.x;
    point.y = (1 - v) * Q1.y + v * Q0.y;
    point.z = (1 - v) * Q1.z + v * Q0.z;
    point.w = (1 - v) * Q1.w + v * Q0.w;

    return point;

}

static void gui_3d_rect_update_att(gui_obj_t *obj)
{
    gui_3d_rect_t *this = (void *)obj;
    if (this->animate)
    {
        animate_frame_update(this->animate, obj);
    }
}

static void gui_3d_light_apply(gui_3d_rect_t *this, size_t i /*face_offset*/,
                               gui_3d_world_t *world, gui_3d_light_t *light)
{
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->img[i].data;
    float width = head->w;
    float height = head->h;
    int data_size = sizeof(gui_rgb_data_head_t) + width * height * 4;

    memcpy(this->mask_img + i, this->img + i, sizeof(draw_img_t));
    this->mask_img[i].data = (unsigned char *)gui_malloc(data_size);
    memcpy(this->mask_img[i].data, this->img[i].data, data_size);


    unsigned char mask_color[4] = {light->color.r, light->color.g, light->color.b, light->color.a};
    float alpha = mask_color[3] / 255.0f;

    for (int p_y = 0; p_y < height; p_y++)
    {
        for (int p_x = 0; p_x < width; p_x++)
        {
            float u = (float)p_x / (width - 1);

            float v = (float)p_y / (height - 1);

            if (this->face[i].vertex[0].v == 1)
            {
                v = 1.0f - v;
            }

            gui_point_4d_t point = gui_3d_calculate_point_in_quad(
                                       this->face[i].transform_world_vertex[0].position,
                                       this->face[i].transform_world_vertex[1].position,
                                       this->face[i].transform_world_vertex[2].position,
                                       this->face[i].transform_world_vertex[3].position, u, v);

            float intensity = gui_3d_calculate_light_to_point(light, point.x, point.y, point.z);

            if (intensity != 0)
            {
                unsigned char *origin_pixel = (unsigned char *)this->img[i].data + sizeof(gui_rgb_data_head_t) +
                                              (p_y * head->w + p_x) * 4;
                unsigned char *pixel = (unsigned char *)this->mask_img[i].data + sizeof(gui_rgb_data_head_t)  +
                                       (p_y * head->w + p_x) * 4;

                for (int c = 0; c < 3; ++c)
                {
                    pixel[c] = (unsigned char)(mask_color[c] * alpha * intensity + origin_pixel[c] *
                                               (1 - alpha * intensity));
                }
            }

        }
    }

}

static void gui_3d_rect_face_transfrom(gui_3d_rect_t *this, size_t s/*shape_offset*/,
                                       size_t i /*face_offset*/,
                                       gui_3d_world_t *world, gui_3d_camera_t *camera)
{
    gui_3d_rect_scene(this->face + i, world, camera);

    int material_id = this->desc->attrib.material_ids[i];
    this->img[i].data = (void *)this->desc->textures[material_id];

    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->img[i].data;
    float width = head->w;
    float height = head->h;

    gui_3d_point_2d_t src[4];
    gui_3d_point_2d_t dst[4];
    for (size_t j = 0; j < 4; ++j)
    {
        src[j].x = this->face[i].vertex[j].u * width;
        src[j].y = (1.0f - this->face[i].vertex[j].v) * height;

        dst[j].x = this->face[i].transform_vertex[j].position.x;
        dst[j].y = this->face[i].transform_vertex[j].position.y;
    }

    gui_3d_generate_2d_matrix(src, dst, (float *)&this->img[i].matrix);
    memcpy(&this->img[i].inverse, &this->img[i].matrix, sizeof(gui_matrix_t));
    matrix_inverse(&this->img[i].inverse);

    this->img[i].img_w = width;
    this->img[i].img_h = height;
    this->img[i].blend_mode = IMG_SRC_OVER_MODE;
    this->img[i].high_quality = true;
    this->img[i].opacity_value = this->desc->materials[material_id].dissolve * UINT8_MAX;

    draw_img_new_area(this->img + i, NULL);
}

static void gui_3d_rect_convert_to_face(gui_3d_rect_t *this, size_t i /*face_offset*/)
{
    size_t index_offset = 0;
    for (size_t s = 0; s < i; s++)
    {
        index_offset += this->desc->attrib.face_num_verts[i];
    }

    for (size_t j = 0; j < this->desc->attrib.face_num_verts[i]; j++)
    {
        gui_obj_vertex_index_t idx = this->desc->attrib.faces[index_offset + j];

        gui_obj_vertex_coordinate_t *v = &this->desc->attrib.vertices[idx.v_idx];
        gui_obj_texcoord_coordinate_t *vt = &this->desc->attrib.texcoords[idx.vt_idx];
        gui_obj_vertex_coordinate_t *vn = &this->desc->attrib.normals[idx.vn_idx];

        this->face[i].vertex[j].position.x = v->x;
        this->face[i].vertex[j].position.y = v->y;
        this->face[i].vertex[j].position.z = v->z;
        this->face[i].vertex[j].position.w = 1;

        this->face[i].vertex[j].u = vt->u;
        this->face[i].vertex[j].v = vt->v;

        this->face[i].vertex[j].normal.x = vn->x;
        this->face[i].vertex[j].normal.y = vn->y;
        this->face[i].vertex[j].normal.z = vn->z;
        this->face[i].vertex[j].normal.w = 1;

    }
}

static void gui_3d_rect_prepare(gui_3d_rect_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_3d_rect_update_att(obj);

    this->face = gui_malloc(sizeof(gui_3d_rect_face_t) * this->desc->attrib.num_face_num_verts);
    memset(this->face, 0x00, sizeof(gui_3d_rect_face_t) * this->desc->attrib.num_face_num_verts);

    this->img = gui_malloc(sizeof(draw_img_t) * this->desc->attrib.num_face_num_verts);
    memset(this->img, 0x00, sizeof(draw_img_t) * this->desc->attrib.num_face_num_verts);

    this->mask_img = gui_malloc(sizeof(draw_img_t) * this->desc->attrib.num_face_num_verts);
    memset(this->mask_img, 0x00, sizeof(draw_img_t) * this->desc->attrib.num_face_num_verts);

    gui_3d_world_t world;
    gui_3d_camera_t camera;
    gui_3d_light_t light;
    light.initialized = false;

    // global transform
    if (this->global_shape_transform_cb != NULL)
    {
        this->global_shape_transform_cb(this, &world, &camera, &light);
    }

    for (size_t i = 0; i < this->desc->num_shapes; i++)
    {
        for (size_t j = 0; j < this->desc->shapes[i].length /*number of face*/; j++)
        {
            size_t face_offset = this->desc->shapes[i].face_offset + j;

            // local transform
            if (this->local_shape_transform_cb != NULL)
            {
                this->local_shape_transform_cb(this, face_offset, &world, &camera, &light);
            }
            gui_3d_rect_convert_to_face(this, face_offset);

            gui_3d_rect_face_transfrom(this, i, face_offset, &world, &camera);

            if (light.initialized)
            {
                gui_3d_light_apply(this, face_offset, &world, &light);
            }

        }
    }

    if (tp->type == TOUCH_SHORT)
    {
        // Cache the num_face_num_verts
        const int num_face_vertices = this->desc->attrib.num_face_num_verts;

        for (int i = 0; i < num_face_vertices; ++i)
        {
            // Cache the img_target and face attributes
            const int target_x = this->img[i].img_target_x;
            const int target_y = this->img[i].img_target_y;
            const int target_w = this->img[i].img_target_w;
            const int target_h = this->img[i].img_target_h;

            if (tp->x >= target_x &&
                tp->x <= (target_x + target_w) &&
                tp->y >= target_y &&
                tp->y <= (target_y + target_h))
            {
                gui_obj_enable_event(obj, GUI_EVENT_1);
                break;
            }
        }
    }

    gui_fb_change();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
}

static void gui_3d_rect_draw(gui_3d_rect_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(obj);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    for (uint32_t i = 0; i < this->desc->attrib.num_face_num_verts; i++)
    {
        if (!(this->face[i].state & GUI_3D_FACESTATE_BACKFACE)) // Non-backfaces
        {
            continue;
        }
        if (this->mask_img->data != NULL)
        {
            draw_img_cache(this->mask_img + i, IMG_SRC_MEMADDR);
            gui_acc_blit_to_dc(this->mask_img + i, dc, NULL);
            draw_img_free(this->mask_img + i, IMG_SRC_MEMADDR);
        }
        else
        {
            draw_img_cache(this->img + i, IMG_SRC_MEMADDR);
            gui_acc_blit_to_dc(this->img + i, dc, NULL);
            draw_img_free(this->img + i, IMG_SRC_MEMADDR);
        }
    }

    for (uint32_t i = 0; i < this->desc->attrib.num_face_num_verts; i++)
    {
        if (this->face[i].state & GUI_3D_FACESTATE_BACKFACE)  // backfaces
        {
            continue;
        }
        if (this->mask_img->data != NULL)
        {
            draw_img_cache(this->mask_img + i, IMG_SRC_MEMADDR);
            gui_acc_blit_to_dc(this->mask_img + i, dc, NULL);
            draw_img_free(this->mask_img + i, IMG_SRC_MEMADDR);
        }
        else
        {
            draw_img_cache(this->img + i, IMG_SRC_MEMADDR);
            gui_acc_blit_to_dc(this->img + i, dc, NULL);
            draw_img_free(this->img + i, IMG_SRC_MEMADDR);
        }
    }

}

static void gui_3d_rect_end(gui_3d_rect_t *this)
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
        for (int i = 0; i < this->desc->attrib.num_face_num_verts; i++)
        {
            if (draw_img_acc_end_cb != NULL)
            {
                draw_img_acc_end_cb(this->img + i);
            }
        }
        gui_free(this->img);
        this->img = NULL;
    }

    if (this->mask_img != NULL)
    {
        for (int i = 0; i < this->desc->attrib.num_face_num_verts; i++)
        {
            if (this->mask_img[i].data != NULL)
            {
                gui_free(this->mask_img[i].data);
                this->mask_img[i].data = NULL;
            }
        }
    }
    gui_free(this->mask_img);
    this->mask_img = NULL;

    gui_free(this->face);
    this->face = NULL;
}

static void gui_3d_rect_destroy(gui_3d_rect_t *this)
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

static void gui_3d_rect_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_3d_rect_prepare((gui_3d_rect_t *)obj);
            break;

        case OBJ_DRAW:
            gui_3d_rect_draw((gui_3d_rect_t *)obj);
            break;

        case OBJ_END:
            gui_3d_rect_end((gui_3d_rect_t *)obj);
            break;

        case OBJ_DESTROY:
            gui_3d_rect_destroy((gui_3d_rect_t *)obj);
            break;

        default:
            break;
        }
    }
}






void gui_3d_rect_ctor(gui_3d_rect_t          *this,
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

    obj->obj_cb = gui_3d_rect_cb;
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

void gui_3d_rect_set_global_shape_transform_cb(gui_3d_rect_t *this,
                                               void (*cb)(gui_3d_rect_t *this, gui_3d_world_t *world, gui_3d_camera_t *camera,
                                                          gui_3d_light_t *light))
{
    this->global_shape_transform_cb = cb;
}

void gui_3d_rect_set_local_shape_transform_cb(gui_3d_rect_t *this, size_t face/*face_offset*/,
                                              void (*cb)(gui_3d_rect_t *this, size_t face, gui_3d_world_t *world, gui_3d_camera_t *camera,
                                                         gui_3d_light_t *light))
{
    this->local_shape_transform_cb = cb;
}

void gui_3d_rect_set_animate(gui_3d_rect_t *this,
                             uint32_t       dur,
                             int            repeat_count,
                             void          *callback,
                             void          *p)
{
    GUI_SET_ANIMATE_HELPER
}


gui_3d_rect_t *gui_3d_rect_create(void                  *parent,
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

    gui_3d_rect_t *this = gui_malloc(sizeof(gui_3d_rect_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_3d_rect_t));

    gui_3d_rect_ctor(this, (gui_obj_t *)parent, name, desc, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}
void gui_3d_rect_on_click(gui_3d_rect_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb(this, (gui_event_cb_t)callback, GUI_EVENT_1, parameter);
}

