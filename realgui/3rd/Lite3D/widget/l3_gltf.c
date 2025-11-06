/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file l3_gltf.c
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
#include "l3.h"
#include "l3_common.h"
#include "l3_gltf.h"
#include "l3_rect_raster.h"
#include "gui_api.h"

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static float get_animation_duration(l3_gltf_single_animation_t *anim)
{
    float max_duration = 0.0f;

    if (!anim)
    {
        return 0.0f;
    }

    for (uint32_t i = 0; i < anim->sampler_count; ++i)
    {
        l3_gltf_sampler_t *sampler = &anim->samplers[i];
        if (sampler->input_count > 0)
        {
            // The input_data stores timestamps, and the last timestamp represents the end time of this sampler.
            float last_time = sampler->input_data[sampler->input_count - 1];
            if (last_time > max_duration)
            {
                max_duration = last_time;
            }
        }
    }
    return max_duration;
}

static l3_gltf_model_description_t *l3_load_gltf_description(void *desc_addr)
{
    unsigned char *ptr = (unsigned char *)desc_addr;
    g3m_header_t *header = (g3m_header_t *)ptr;

    int                     *roots_on_disk      = (int *)(ptr + header->scene_roots_offset);
    g3m_node_on_disk_t      *nodes_on_disk      = (g3m_node_on_disk_t *)(ptr + header->nodes_offset);
    g3m_mesh_on_disk_t      *meshes_on_disk     = (g3m_mesh_on_disk_t *)(ptr + header->meshes_offset);
    g3m_primitive_on_disk_t *primitives_on_disk = (g3m_primitive_on_disk_t *)(
                                                      ptr + header->primitives_offset);
    g3m_material_on_disk_t  *materials_on_disk  = (g3m_material_on_disk_t *)(
                                                      ptr + header->materials_offset);
    g3m_texture_on_disk_t   *textures_on_disk   = (g3m_texture_on_disk_t *)(
                                                      ptr + header->textures_offset);
    g3m_skin_on_disk_t      *skins_on_disk      = (g3m_skin_on_disk_t *)(ptr + header->skins_offset);
    g3m_channel_on_disk_t   *channels_on_disk   = (g3m_channel_on_disk_t *)(
                                                      ptr + header->channels_offset);
    g3m_sampler_on_disk_t   *samplers_on_disk   = (g3m_sampler_on_disk_t *)(
                                                      ptr + header->samplers_offset);
    unsigned char           *data_blob_base     = ptr + header->data_blob_offset;

    // 1. Allocate memory for the model description structure.
    l3_gltf_model_description_t *desc = (l3_gltf_model_description_t *)l3_malloc(sizeof(
            l3_gltf_model_description_t));
    memset(desc, 0, sizeof(l3_gltf_model_description_t));
    desc->scene_root_count = header->scene_root_count;
    desc->node_count = header->node_count;
    desc->mesh_count = header->mesh_count;
    desc->material_count = header->material_count;
    desc->skin_count = header->skin_count;

    // 2. Allocate memory for the top-level arrays.
    desc->nodes = (l3_gltf_node_t *)l3_malloc(sizeof(l3_gltf_node_t) * desc->node_count);
    desc->meshes = (l3_gltf_mesh_t *)l3_malloc(sizeof(l3_gltf_mesh_t) * desc->mesh_count);
    desc->materials = (l3_gltf_material_t *)l3_malloc(sizeof(l3_gltf_material_t) *
                                                      desc->material_count);
    l3_gltf_primitive_t *all_primitives_ram = (l3_gltf_primitive_t *)l3_malloc(sizeof(
                                                                                   l3_gltf_primitive_t) * header->primitive_count);
    desc->skins = (l3_gltf_skin_t *)l3_malloc(sizeof(l3_gltf_skin_t) * desc->skin_count);
    desc->animation = (l3_gltf_single_animation_t *)l3_malloc(sizeof(l3_gltf_single_animation_t));

    // 2. Pointer Redirection
    // 2.1 Scene root
    desc->scene_root_indices = (int *)roots_on_disk;

    // 2.2 Material Reconstruction
    for (uint32_t i = 0; i < desc->material_count; ++i)
    {
        g3m_material_on_disk_t *src_mat = &materials_on_disk[i];
        l3_gltf_material_t *dst_mat = &desc->materials[i];

        memcpy(dst_mat->base_color, src_mat->base_color, sizeof(float) * 4);

        if (src_mat->texture_index != -1)
        {
            g3m_texture_on_disk_t *tex_disk = &textures_on_disk[src_mat->texture_index];
            dst_mat->texture_data = (unsigned char *)(data_blob_base +
                                                      tex_disk->data_offset);
            dst_mat->texture_length = tex_disk->data_size;
        }
        else
        {
            dst_mat->texture_data = NULL;
            dst_mat->texture_length = 0;
        }
    }

    // 2.3 Primitive Reconstruction
    for (uint32_t i = 0; i < header->primitive_count; ++i)
    {
        g3m_primitive_on_disk_t *src_prim = &primitives_on_disk[i];
        l3_gltf_primitive_t *dst_prim = &all_primitives_ram[i];

        dst_prim->triangle_count = src_prim->triangle_count;
        dst_prim->triangles = (l3_gltf_triangle_t *)(data_blob_base +
                                                     src_prim->triangles_offset);

        if (src_prim->material_index != -1)
        {
            dst_prim->material = &desc->materials[src_prim->material_index];
        }
        else
        {
            dst_prim->material = NULL;
        }
    }

    // 2.4 Mesh Reconstruction
    for (uint32_t i = 0; i < desc->mesh_count; ++i)
    {
        g3m_mesh_on_disk_t *src_mesh = &meshes_on_disk[i];
        l3_gltf_mesh_t *dst_mesh = &desc->meshes[i];

        dst_mesh->primitive_count = src_mesh->primitive_count;
        dst_mesh->primitives =
            &all_primitives_ram[src_mesh->primitive_start_index];
    }

    // 2.5 Node Reconstruction
    for (uint32_t i = 0; i < desc->node_count; ++i)
    {
        g3m_node_on_disk_t *src_node = &nodes_on_disk[i];
        l3_gltf_node_t *dst_node = &desc->nodes[i];

        memcpy(dst_node->translation, src_node->translation, sizeof(float) * 3);
        memcpy(dst_node->rotation, src_node->rotation, sizeof(float) * 4);
        memcpy(dst_node->scale, src_node->scale, sizeof(float) * 3);

        dst_node->mesh_index = src_node->mesh_index;
        dst_node->parent_index = src_node->parent_index;
        dst_node->num_children = src_node->children_count;
        dst_node->skin_index = src_node->skin_index;

        if (src_node->children_count > 0)
        {
            dst_node->children_indices = (int *)(data_blob_base + src_node->children_offset);
        }
        else
        {
            dst_node->children_indices = NULL;
        }
    }

    // 2.6 Skin Reconstruction
    for (uint32_t i = 0; i < desc->skin_count; ++i)
    {
        g3m_skin_on_disk_t *src_skin = &skins_on_disk[i];
        l3_gltf_skin_t *dst_skin = &desc->skins[i];

        dst_skin->joint_count = src_skin->joints_count;
        dst_skin->joint_indices = (uint32_t *)(data_blob_base + src_skin->joints_offset);
        dst_skin->inverse_bind_matrices = (l3_4x4_matrix_t *)(data_blob_base +
                                                              src_skin->inverse_bind_matrices_offset);
        dst_skin->joint_matrices = (l3_4x4_matrix_t *)l3_malloc(sizeof(l3_4x4_matrix_t) *
                                                                dst_skin->joint_count);
        for (uint32_t j = 0; j < dst_skin->joint_count; ++j)
        {
            l3_4x4_matrix_identity(&dst_skin->joint_matrices[j]);
        }
    }

    // 2.7 Sampler Reconstruction
    desc->animation->sampler_count = header->sampler_count;
    desc->animation->samplers = (l3_gltf_sampler_t *)l3_malloc(sizeof(l3_gltf_sampler_t) *
                                                               desc->animation->sampler_count);
    for (uint32_t i = 0; i < desc->animation->sampler_count; ++i)
    {
        g3m_sampler_on_disk_t *src_sampler = &samplers_on_disk[i];
        l3_gltf_sampler_t *dst_sampler = &desc->animation->samplers[i];

        dst_sampler->interpolation = (gltf_interpolation_type_t)src_sampler->interpolation_type;
        dst_sampler->input_count = src_sampler->input_count;
        dst_sampler->output_count = src_sampler->output_count;
        dst_sampler->output_type = (gltf_data_type)src_sampler->output_type;

        dst_sampler->input_data = (float *)(data_blob_base + src_sampler->input_offset);
        dst_sampler->output_data = (float *)(data_blob_base + src_sampler->output_offset);
    }

    // 2.8 Channel Reconstruction
    desc->animation->channel_count = header->channel_count;
    desc->animation->channels = (l3_gltf_channel_t *)l3_malloc(sizeof(l3_gltf_channel_t) *
                                                               desc->animation->channel_count);
    for (uint32_t i = 0; i < desc->animation->channel_count; ++i)
    {
        g3m_channel_on_disk_t *src_channel = &channels_on_disk[i];
        l3_gltf_channel_t *dst_channel = &desc->animation->channels[i];

        dst_channel->sampler_index = src_channel->sampler_index;
        dst_channel->target_node_index = src_channel->target_node_index;
        dst_channel->target_path = (gltf_animation_path_t)src_channel->target_path;
    }

    // gui_log("Header Size:         %zu bytes\n", sizeof(g3m_header_t));
    // gui_log("Nodes Array Size:    %zu bytes\n", sizeof(g3m_node_on_disk_t) * header->node_count);
    // gui_log("Meshes Array Size:   %zu bytes\n", sizeof(g3m_mesh_on_disk_t) * header->mesh_count);
    // gui_log("Primitives Array Size:%zu bytes\n", sizeof(g3m_primitive_on_disk_t) * header->primitive_count);
    // gui_log("Materials Array Size:%zu bytes\n", sizeof(g3m_material_on_disk_t) * header->material_count);
    // gui_log("Textures Array Size:%zu bytes\n", sizeof(g3m_texture_on_disk_t) * header->texture_count);
    // gui_log("Skins Array Size:%zu bytes\n", sizeof(g3m_skin_on_disk_t) * header->skin_count);
    // gui_log("Channels Array Size:%zu bytes\n", sizeof(g3m_channel_on_disk_t) * header->channel_count);
    // gui_log("Samplers Array Size:%zu bytes\n", sizeof(g3m_sampler_on_disk_t) * header->sampler_count);

    desc->animation->duration = get_animation_duration(desc->animation);
    desc->animation->animation_time = 0.0f;

    return desc;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

l3_gltf_model_t *l3_create_gltf_model(void                 *desc_addr,
                                      L3_IMAGE_TYPE         image_type,
                                      int16_t               x,
                                      int16_t               y,
                                      int16_t               w,
                                      int16_t               h)
{
    l3_gltf_model_description_t *desc = l3_load_gltf_description((void *)desc_addr);

    l3_gltf_model_t *this = l3_malloc(sizeof(l3_gltf_model_t));
    memset(this, 0x00, sizeof(l3_gltf_model_t));

    this->x = x;
    this->y = y;
    this->viewPortWidth = w;
    this->viewPortHeight = h;

    this->desc = desc;

    this->combined_img = l3_malloc(sizeof(l3_draw_rect_img_t));
    memset(this->combined_img, 0x00, sizeof(l3_draw_rect_img_t));

    this->depthBuffer = l3_malloc(w * h * sizeof(float));
    memset(this->depthBuffer, 0x00, w * h * sizeof(float));

    if (image_type == LITE_RGB565)
    {
        this->combined_img->data = l3_malloc(w * h * 2 + sizeof(l3_img_head_t));
        memset(this->combined_img->data, 0x00, w * h * 2 + sizeof(l3_img_head_t));

        l3_img_head_t *head = (l3_img_head_t *)this->combined_img->data;
        head->w = w;
        head->h = h;
        head->type = LITE_RGB565;
    }
    else if (image_type == LITE_ARGB8888)
    {
        this->combined_img->data = l3_malloc(w * h * 4 + sizeof(l3_img_head_t));
        memset(this->combined_img->data, 0x00, w * h * 4 + sizeof(l3_img_head_t));

        l3_img_head_t *head = (l3_img_head_t *)this->combined_img->data;
        head->w = w;
        head->h = h;
        head->type = LITE_ARGB8888;
    }

    return this;
}

void l3_gltf_set_target_canvas(l3_gltf_model_t *_this, \
                               uint16_t x, uint16_t y, \
                               uint16_t w, uint16_t h, \
                               uint16_t bit_depth, \
                               uint8_t *canvas)
{
    _this->canvas.frame_buf = canvas;
    _this->canvas.bit_depth = bit_depth;
    _this->canvas.section.x1 = x;
    _this->canvas.section.y1 = y;
    _this->canvas.section.x2 = x + w - 1;
    _this->canvas.section.y2 = y + h - 1;
}


void l3_gltf_set_global_transform(l3_gltf_model_t *_this, l3_gltf_global_transform_cb cb)
{
    _this->global_transform_cb = (void (*)(l3_gltf_model_t *))cb;
}


void l3_gltf_draw(l3_gltf_model_t *_this)
{
    l3_draw_rect_img_to_canvas(_this->combined_img, &_this->canvas, NULL);
}


void l3_free_gltf_model(l3_gltf_model_t *_this)
{
    if (_this->combined_img != NULL)
    {
        l3_free(_this->combined_img->data);
        _this->combined_img->data = NULL;

        l3_free(_this->combined_img);
        _this->combined_img = NULL;
    }

    if (_this->depthBuffer != NULL)
    {
        l3_free(_this->depthBuffer);
        _this->depthBuffer = NULL;
    }

    if (_this->desc != NULL)
    {
        if (_this->desc->mesh_count > 0 && _this->desc->meshes && _this->desc->meshes[0].primitives)
        {
            l3_free(_this->desc->meshes[0].primitives);
            _this->desc->meshes[0].primitives = NULL;
        }
        if (_this->desc->nodes != NULL)
        {
            l3_free(_this->desc->nodes);
            _this->desc->nodes = NULL;
        }
        if (_this->desc->meshes != NULL)
        {
            l3_free(_this->desc->meshes);
            _this->desc->meshes = NULL;
        }
        if (_this->desc->materials != NULL)
        {
            l3_free(_this->desc->materials);
            _this->desc->materials = NULL;
        }
        if (_this->desc->skins != NULL)
        {
            for (uint32_t i = 0; i < _this->desc->skin_count; ++i)
            {
                l3_free(_this->desc->skins[i].joint_matrices);
            }
            l3_free(_this->desc->skins);
            _this->desc->skins = NULL;
        }
        if (_this->desc->animation != NULL)
        {
            if (_this->desc->animation->channels != NULL)
            {
                l3_free(_this->desc->animation->channels);
                _this->desc->animation->channels = NULL;
            }
            if (_this->desc->animation->samplers != NULL)
            {
                l3_free(_this->desc->animation->samplers);
                _this->desc->animation->samplers = NULL;
            }
            l3_free(_this->desc->animation);
            _this->desc->animation = NULL;
        }

        l3_free(_this->desc);
        _this->desc = NULL;
    }
    l3_free(_this);

}
