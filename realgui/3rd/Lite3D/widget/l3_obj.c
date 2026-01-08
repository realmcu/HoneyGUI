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
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "l3_obj.h"
#include "l3_common.h"
#include "l3_tria.h"
#include "l3_rect.h"

/*============================================================================*
 *                               Types
 *============================================================================*/

static const l3_model_ops_t obj_model_ops =
{
    .push = l3_obj_push,
    .draw = l3_obj_draw,
    .model_free = l3_free_obj_model,

    .set_global_transform = l3_obj_set_global_transform,
    .set_face_transform = l3_obj_set_face_transform,
    .set_target_canvas = l3_obj_set_target_canvas,
    .set_face_image = l3_obj_set_face_image,
    .on_click = l3_obj_model_on_click,
};
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static l3_obj_model_description_t *l3_load_obj_description(void *desc_addr)
{
    unsigned char *ptr = (unsigned char *)desc_addr;
    l3_obj_model_description_t *desc = (l3_obj_model_description_t *)l3_malloc(sizeof(
                                                                                   l3_obj_model_description_t));

    // file head
    desc->file_head = *((l3_desc_file_head_t *)ptr);
    if (desc->file_head.face_type == LITE_3D_FACE_RECTANGLE_TRIANGLE)
    {
        l3_free(desc);
        return NULL;
    }
    ptr += desc->file_head.payload_offset;

    // attrib
    desc->attrib.num_vertices = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    desc->attrib.num_normals = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    desc->attrib.num_texcoords = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    desc->attrib.num_faces = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    desc->attrib.num_face_num_verts = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    desc->attrib.pad0 = *((int *)ptr);
    ptr += sizeof(int);

    desc->attrib.vertices = (l3_vertex_coordinate_t *)ptr;
    ptr += desc->attrib.num_vertices * sizeof(l3_vertex_coordinate_t);
    desc->attrib.normals = (l3_vertex_coordinate_t *)ptr;
    ptr += desc->attrib.num_normals * sizeof(l3_vertex_coordinate_t);
    desc->attrib.texcoords = (l3_texcoord_coordinate_t *)ptr;
    ptr += desc->attrib.num_texcoords * sizeof(l3_texcoord_coordinate_t);
    desc->attrib.faces = (l3_vertex_index_t *)ptr;
    ptr += desc->attrib.num_faces * sizeof(l3_vertex_index_t);
    desc->attrib.face_num_verts = (int *)ptr;
    ptr += desc->attrib.num_face_num_verts * sizeof(int);
    desc->attrib.material_ids = (int *)ptr;
    ptr += desc->attrib.num_face_num_verts * sizeof(int);

    // shape
    desc->num_shapes = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    desc->shapes = (l3_obj_shape_t *)ptr;
    ptr += desc->num_shapes * sizeof(l3_obj_shape_t);

    // material
    desc->num_materials = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    if (desc->num_materials > 0)
    {
        desc->materials = (l3_obj_material_t *)ptr;
        ptr += desc->num_materials * sizeof(l3_obj_material_t);

        // textures size
        desc->texture_sizes = (unsigned int *)ptr;
        ptr += desc->num_materials * sizeof(unsigned int);
        desc->textures = (unsigned char **)l3_malloc(desc->num_materials * sizeof(unsigned char *));
        if (!desc->textures)
        {
            l3_free(desc);
            return NULL;
        }
        // texture content
        for (uint32_t i = 0; i < desc->num_materials; i++)
        {
            if (desc->texture_sizes[i] > 0)
            {
                desc->textures[i] = (unsigned char *)ptr;
            }
            else
            {
                desc->textures[i] = NULL;
            }
            ptr += desc->texture_sizes[i];
        }

    }
    else
    {
        desc->materials = NULL;
        desc->textures = NULL;
        desc->texture_sizes = NULL;
    }

    return desc;

}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/

l3_obj_model_t *l3_create_obj_model(void                 *desc_addr,
                                    L3_DRAW_TYPE          draw_type,
                                    int16_t               x,
                                    int16_t               y,
                                    int16_t               w,
                                    int16_t               h)
{
    l3_obj_model_description_t *desc = l3_load_obj_description((void *)desc_addr);
    if (!desc)
    {
        return NULL;
    }

    l3_obj_model_t *this = l3_malloc(sizeof(l3_obj_model_t));
    memset(this, 0x00, sizeof(l3_obj_model_t));

    this->base.ops = &obj_model_ops;

    this->base.x = x;
    this->base.y = y;
    this->base.viewPortWidth = w;
    this->base.viewPortHeight = h;
    this->base.draw_type = draw_type;

    this->desc = desc;

    switch (this->desc->file_head.face_type)
    {
    case LITE_3D_FACE_RECTANGLE:
        {
            this->face.rect_face = l3_malloc(sizeof(l3_rect_face_t) *
                                             this->desc->attrib.num_face_num_verts);
            memset(this->face.rect_face, 0x1,
                   sizeof(l3_rect_face_t) * this->desc->attrib.num_face_num_verts);

            if (this->base.draw_type != L3_DRAW_FRONT_AND_SORT)
            {
                this->img = l3_malloc(sizeof(l3_draw_rect_img_t) * this->desc->attrib.num_face_num_verts);
                memset(this->img, 0x00, sizeof(l3_draw_rect_img_t) * this->desc->attrib.num_face_num_verts);
            }
            else
            {
                this->base.combined_img = l3_malloc(sizeof(l3_draw_rect_img_t));
                memset(this->base.combined_img, 0x00, sizeof(l3_draw_rect_img_t));

                this->base.combined_img->data = l3_malloc(w * h * 2 + sizeof(l3_img_head_t));
                memset(this->base.combined_img->data, 0x00, w * h * 2 + sizeof(l3_img_head_t));

                l3_img_head_t *head = (l3_img_head_t *)this->base.combined_img->data;
                head->w = w;
                head->h = h;
                head->type = LITE_RGB565;
            }
        }
        break;

    case LITE_3D_FACE_TRIANGLE:
        {
            this->face.tria_face = l3_malloc(sizeof(l3_tria_face_t) *
                                             this->desc->attrib.num_face_num_verts);
            memset(this->face.tria_face, 0x1,
                   sizeof(l3_tria_face_t) * this->desc->attrib.num_face_num_verts);

            this->base.combined_img = l3_malloc(sizeof(l3_draw_rect_img_t));
            memset(this->base.combined_img, 0x00, sizeof(l3_draw_rect_img_t));

            // Check if model has materials/textures
            bool has_material = (this->desc->num_materials > 0 && this->desc->textures != NULL);

            if (has_material)
            {
                // Use RGB565 for models with materials
                this->base.combined_img->data = l3_malloc(w * h * 2 + sizeof(l3_img_head_t));
                memset(this->base.combined_img->data, 0x00, w * h * 2 + sizeof(l3_img_head_t));

                l3_img_head_t *head = (l3_img_head_t *)this->base.combined_img->data;
                head->w = w;
                head->h = h;
                head->type = LITE_RGB565;
            }
            else
            {
                // Use I8 for models without materials (save 50% memory)
                // I8 format: header + clut_count(4) + clut_data(count*4 ABGR) + pixel_index(w*h)
                uint32_t clut_count = 256;
                uint32_t clut_size = clut_count * 4; // 256 ABGR entries
                uint32_t total_size = sizeof(l3_img_head_t) + sizeof(uint32_t) + clut_size + w * h * 1;
                this->base.combined_img->data = l3_malloc(total_size);
                memset(this->base.combined_img->data, 0x00, total_size);

                l3_img_head_t *head = (l3_img_head_t *)this->base.combined_img->data;
                head->w = w;
                head->h = h;
                head->type = LITE_I8;

                // Write CLUT count
                uint32_t *clut_count_ptr = (uint32_t *)((uint8_t *)this->base.combined_img->data + sizeof(
                                                            l3_img_head_t));
                *clut_count_ptr = clut_count;

                // Initialize grayscale CLUT (256 levels) in ABGR format
                uint32_t *clut = clut_count_ptr + 1;
                for (int i = 0; i < 256; i++)
                {
                    // ABGR format: A=0xFF, B=i, G=i, R=i
                    clut[i] = 0xFF000000 | (i << 16) | (i << 8) | i;
                }
            }
        }
        break;

    default:
        break;
    }


    return this;
}

void l3_obj_set_target_canvas(l3_model_base_t *base, \
                              uint16_t x, uint16_t y, \
                              uint16_t w, uint16_t h, \
                              uint16_t bit_depth, \
                              uint8_t *canvas)
{
    l3_obj_model_t *_this = (l3_obj_model_t *)base;
    _this->base.canvas.frame_buf = canvas;
    _this->base.canvas.bit_depth = bit_depth;
    _this->base.canvas.section.x1 = x;
    _this->base.canvas.section.y1 = y;
    _this->base.canvas.section.x2 = x + w - 1;
    _this->base.canvas.section.y2 = y + h - 1;
}

void l3_obj_set_face_image(l3_model_base_t *base, uint8_t face_index, void *image_addr)
{
    l3_obj_model_t *_this = (l3_obj_model_t *)base;
    if (_this->desc->file_head.face_type == LITE_3D_FACE_RECTANGLE)
    {
        l3_set_rect_face_image(_this, face_index, image_addr);
    }
    else
    {
        //todo
    }
}

void l3_obj_set_global_transform(l3_model_base_t *base, l3_global_transform_cb cb)
{
    l3_obj_model_t *_this = (l3_obj_model_t *)base;
    _this->global_transform_cb = (void (*)(l3_obj_model_t *))cb;
}

void l3_obj_set_face_transform(l3_model_base_t *base, l3_face_transform_cb cb)
{
    l3_obj_model_t *_this = (l3_obj_model_t *)base;
    _this->face_transform_cb = (l3_4x4_matrix_t(*)(l3_obj_model_t *, size_t))cb;
}

void l3_obj_push(l3_model_base_t *base, l3_3x3_matrix_t *parent_matrix)
{
    l3_obj_model_t *_this = (l3_obj_model_t *)base;
    if (_this == NULL)
    {
        return;
    }

    switch (_this->desc->file_head.face_type)
    {
    case LITE_3D_FACE_RECTANGLE:
        l3_rect_push(_this, parent_matrix);
        break;

    case LITE_3D_FACE_TRIANGLE:
        l3_tria_push(_this, parent_matrix);
        break;

    default:
        break;
    }
}

void l3_obj_draw(l3_model_base_t *base)
{
    l3_obj_model_t *_this = (l3_obj_model_t *)base;
    if (_this == NULL)
    {
        return;
    }

    switch (_this->desc->file_head.face_type)
    {
    case LITE_3D_FACE_RECTANGLE:
        l3_rect_draw(_this);
        break;

    case LITE_3D_FACE_TRIANGLE:
        l3_tria_draw(_this);
        break;

    default:
        break;
    }
}


void l3_free_obj_model(l3_model_base_t *base)
{
    l3_obj_model_t *_this = (l3_obj_model_t *)base;
    switch (_this->desc->file_head.face_type)
    {
    case LITE_3D_FACE_RECTANGLE:
        l3_rect_free_model(_this);
        break;

    case LITE_3D_FACE_TRIANGLE:
        l3_tria_free_model(_this);
        break;

    default:
        break;
    }

    if (_this->base.combined_img != NULL)
    {
        l3_free(_this->base.combined_img->data);
        _this->base.combined_img->data = NULL;

        l3_free(_this->base.combined_img);
        _this->base.combined_img = NULL;
    }

    if (_this->desc != NULL)
    {
        if (_this->desc->textures != NULL)
        {
            l3_free(_this->desc->textures);
            _this->desc->textures = NULL;
        }

        l3_free(_this->desc);
        _this->desc = NULL;
    }

    if (_this->base.raw_data_from_ftl != NULL)
    {
        l3_free(_this->base.raw_data_from_ftl);
        _this->base.raw_data_from_ftl = NULL;
    }

    l3_free(_this);

}

bool l3_obj_model_on_click(l3_model_base_t *base, int16_t x, int16_t y)
{
    l3_obj_model_t *_this = (l3_obj_model_t *)base;

    if (_this->base.draw_type == L3_DRAW_FRONT_AND_SORT)
    {
        const int target_x = _this->base.combined_img->img_target_x;
        const int target_y = _this->base.combined_img->img_target_y;
        const int target_w = _this->base.combined_img->img_target_w;
        const int target_h = _this->base.combined_img->img_target_h;

        if (x >= target_x && x <= (target_x + target_w) &&
            y >= target_y && y <= (target_y + target_h))
        {
            return true;
        }
    }
    else
    {
        // Cache the num_face_num_verts
        const int num_face_vertices = _this->desc->attrib.num_face_num_verts;

        for (int i = 0; i < num_face_vertices; ++i)
        {
            // Cache the img_target and face attributes
            const int target_x = _this->img[i].img_target_x;
            const int target_y = _this->img[i].img_target_y;
            const int target_w = _this->img[i].img_target_w;
            const int target_h = _this->img[i].img_target_h;

            if (x >= target_x && x <= (target_x + target_w) &&
                y >= target_y && y <= (target_y + target_h))
            {
                return true;
            }
        }
    }
    return false;
}