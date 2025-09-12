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
#include <assert.h>
#include "l3_common.h"
#include "l3_tria_transform.h"
#include "l3_sw_raster.h"
#include "l3_port.h"

static void __l3_push_tria_img(l3_model_t *_this)
{
    uint32_t width = _this->viewPortWidth;
    uint32_t height = _this->viewPortHeight;

    float *depthBuffer = l3_malloc(width * height * sizeof(float));
    memset(depthBuffer, 0x00, width * height * sizeof(float));

    memset((uint8_t *)_this->combined_img->data + sizeof(l3_img_head_t), 0x00, width * height * 2);

    uint16_t render_color = 0;
    uint8_t opacity_value = UINT8_MAX;

    for (uint32_t i = 0; i < _this->desc->attrib.num_face_num_verts; i++)
    {
        if (_this->face.tria_face[i].state & L3_FACESTATE_BACKFACE)
        {
            continue;
        }

        l3_draw_tria_img_t tria_img;

        memset(&tria_img, 0x00, sizeof(l3_draw_tria_img_t));
        memcpy(&tria_img, _this->face.tria_face[i].transform_vertex, 3 * sizeof(l3_vertex_t));

        int material_id = _this->desc->attrib.material_ids[i];
        if (material_id >= 0)
        {
            tria_img.fill_data = _this->desc->textures[material_id];
            opacity_value = _this->desc->materials[material_id].dissolve * UINT8_MAX;

            if (tria_img.fill_data == NULL) // Fill with material color
            {
                float *color_diffuse = _this->desc->materials[material_id].diffuse;
                uint8_t color_r = (uint8_t)(*color_diffuse * opacity_value);
                uint8_t color_g = (uint8_t)(*(color_diffuse + 1) * opacity_value);
                uint8_t color_b = (uint8_t)(*(color_diffuse + 2) * opacity_value);

                render_color = ((color_r & 0xF8) << 8) |
                               ((color_g & 0xFC) << 3) |
                               ((color_b & 0xF8) >> 3);

                tria_img.fill_type = L3_FILL_COLOR_RGB565;
                tria_img.fill_data = &render_color;
            }
            else // Fill with texture image
            {
                tria_img.fill_type = L3_FILL_IMAGE_RGB565;

                tria_img.p0.v = 1.0f - tria_img.p0.v; // Inverse v coordinate
                tria_img.p1.v = 1.0f - tria_img.p1.v;
                tria_img.p2.v = 1.0f - tria_img.p2.v;
            }
        }
        else
        {
            float nz = fabsf(tria_img.p0.normal.uz);
            uint8_t color_intensity = (uint8_t)(opacity_value * fmaxf(0.0f, fminf(1.0f, nz)));
            render_color = ((color_intensity & 0xF8) << 8) |
                           ((color_intensity & 0xFC) << 3) |
                           ((color_intensity & 0xF8) >> 3);
            tria_img.fill_type = L3_FILL_COLOR_RGB565;
            tria_img.fill_data = &render_color;
        }

        l3_draw_tria_to_canvas(&tria_img, _this->combined_img, depthBuffer);
    }

    l3_free(depthBuffer);

    _this->combined_img->img_w = width;
    _this->combined_img->img_h = height;
    _this->combined_img->opacity_value = UINT8_MAX;
    _this->combined_img->blend_mode = L3_IMG_FILTER_BLACK;

    l3_3x3_matrix_translate(&_this->combined_img->matrix, _this->x, _this->y);
    memcpy(&_this->combined_img->inverse, &_this->combined_img->matrix, sizeof(l3_3x3_matrix_t));
    l3_3x3_matrix_inverse(&_this->combined_img->inverse);
    l3_calulate_draw_img_target_area(_this->combined_img, NULL);
}

void l3_tria_push(l3_model_t *_this)
{
    _this->light.initialized = false;
    l3_4x4_matrix_t transform_matrix;

    // global transform
    if (_this->global_transform_cb != NULL)
    {
        _this->global_transform_cb(_this);
        transform_matrix = _this->world;
    }

    l3_camera_build_UVN_matrix(&_this->camera);


    for (size_t i = 0; i < _this->desc->attrib.num_face_num_verts; i++)
    {
        // local transform
        if (_this->face_transform_cb != NULL)
        {
            transform_matrix = _this->face_transform_cb(_this, i);
        }

        l3_tria_face_t *face = &_this->face.tria_face[i];
        l3_attrib_t *attrib = &_this->desc->attrib;

        size_t vertex_offset = i * 3;

        for (size_t j = 0; j < 3; j++)
        {
            l3_vertex_index_t idx = attrib->faces[vertex_offset + j];
            l3_vertex_coordinate_t *v = &attrib->vertices[idx.v_idx];
            l3_texcoord_coordinate_t *vt = &attrib->texcoords[idx.vt_idx];

            l3_4d_point_t local_position = {v->x, v->y, v->z, 1.0f};
            face->transform_vertex[j].position = l3_4x4_matrix_mul_4d_point(&transform_matrix, local_position);

            face->transform_vertex[j].u = vt->u;
            face->transform_vertex[j].v = vt->v;
        }

        l3_tria_scene(face, &_this->camera);
    }

    __l3_push_tria_img(_this);
}

void l3_tria_draw(l3_model_t *_this)
{
    l3_draw_rect_img_to_canvas(_this->combined_img, &_this->canvas, NULL);
}



void l3_tria_free_model(l3_model_t *_this)
{
    l3_free(_this->face.tria_face);
    _this->face.tria_face = NULL;
}






