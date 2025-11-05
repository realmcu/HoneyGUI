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
#include "l3_tria_raster.h"
#include "l3_rect_raster.h"
#include "l3_gltf.h"
#include "gui_api.h"

// Vector Linear Interpolation
static void lerp_vector(l3_3d_point_t *out, const l3_3d_point_t *start, const l3_3d_point_t *end,
                        float t)
{
    out->x = start->x + t * (end->x - start->x);
    out->y = start->y + t * (end->y - start->y);
    out->z = start->z + t * (end->z - start->z);
}

// Quaternion Dot Product
static float dot_quaternion(const l3_4d_point_t *q1, const l3_4d_point_t *q2)
{
    return q1->x * q2->x + q1->y * q2->y + q1->z * q2->z + q1->w * q2->w;
}

// Quaternion Normalization
static void normalize_quaternion(l3_4d_point_t *q)
{
    float len_sq = q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w;
    if (len_sq > 0.0f)
    {
        float len = sqrtf(len_sq);
        q->x /= len;
        q->y /= len;
        q->z /= len;
        q->w /= len;
    }
}

// Spherical Linear Interpolation
static void slerp_quaternion(l3_4d_point_t *out, const l3_4d_point_t *start,
                             const l3_4d_point_t *end, float t)
{
    float cos_half_theta = dot_quaternion(start, end);

    l3_4d_point_t temp_end = *end;

    // 如果 cos_half_theta < 0, 说明两个四元数夹角大于90度，
    // 我们应该取 end 的相反数，走更短的路径。
    if (cos_half_theta < 0.0f)
    {
        temp_end.x = -temp_end.x;
        temp_end.y = -temp_end.y;
        temp_end.z = -temp_end.z;
        temp_end.w = -temp_end.w;
        cos_half_theta = -cos_half_theta;
    }

    // 如果两个四元数非常接近，直接使用线性插值以避免精度问题和除以零
    if (cos_half_theta > 0.9999f)
    {
        out->x = start->x + t * (temp_end.x - start->x);
        out->y = start->y + t * (temp_end.y - start->y);
        out->z = start->z + t * (temp_end.z - start->z);
        out->w = start->w + t * (temp_end.w - start->w);
        normalize_quaternion(out);
        return;
    }

    // 标准 Slerp 公式
    float half_theta = acosf(cos_half_theta);
    float sin_half_theta = sqrtf(1.0f - cos_half_theta * cos_half_theta);

    if (fabsf(sin_half_theta) < 0.001f)
    {
        *out = *start;
        return;
    }

    float scale_start = sinf((1.0f - t) * half_theta) / sin_half_theta;
    float scale_end = sinf(t * half_theta) / sin_half_theta;

    out->x = scale_start * start->x + scale_end * temp_end.x;
    out->y = scale_start * start->y + scale_end * temp_end.y;
    out->z = scale_start * start->z + scale_end * temp_end.z;
    out->w = scale_start * start->w + scale_end * temp_end.w;
    normalize_quaternion(out);
}


// Recursively update the global transform of a node and its children
static void update_node_global_transform_recursive(l3_gltf_model_t *_this, int node_index,
                                                   l3_4x4_matrix_t *parent_transform)
{
    l3_gltf_node_t *node = &_this->desc->nodes[node_index];

    // 1. Calculate the local transform matrix from T-R-S
    l3_4x4_matrix_t local_transform;
    l3_4x4_matrix_compose_trs(&local_transform, node->translation, node->rotation, node->scale);

    // 2. global_transform = parent_transform * local_transform
    l3_4x4_matrix_mul(parent_transform, &local_transform, &node->global_transform);

    // 3. Recursively update the global transform of all children
    for (uint32_t i = 0; i < node->num_children; ++i)
    {
        update_node_global_transform_recursive(_this, node->children_indices[i], &node->global_transform);
    }
}


static void l3_gltf_update_animation(l3_gltf_model_t *_this, float dt)
{
    l3_gltf_model_description_t *desc = _this->desc;
    l3_gltf_single_animation_t *anim = desc->animation;

    if (!anim || anim->channel_count == 0)
    {
        return;
    }

    // 1. Update the animation time
    anim->animation_time += dt;

    // 2. Wrap the animation time within [0, duration)
    if (anim->animation_time > anim->duration)
    {
        anim->animation_time = fmodf(anim->animation_time, anim->duration);
    }
    float current_time = anim->animation_time;

    // 3. Process each animation channel
    for (uint32_t i = 0; i < anim->channel_count; ++i)
    {
        l3_gltf_channel_t *channel = &anim->channels[i];
        l3_gltf_sampler_t *sampler = &anim->samplers[channel->sampler_index];
        l3_gltf_node_t *target_node = &desc->nodes[channel->target_node_index];

        // 3.1 Find the keyframes interval that current_time lies between
        uint32_t prev_key_idx = 0;
        uint32_t next_key_idx = 0;
        for (uint32_t j = 0; j < sampler->input_count - 1; ++j)
        {
            if (current_time >= sampler->input_data[j] && current_time <= sampler->input_data[j + 1])
            {
                prev_key_idx = j;
                next_key_idx = j + 1;
                break;
            }
        }

        // 3.2 Interpolate t within [0, 1]
        float prev_key_time = sampler->input_data[prev_key_idx];
        float next_key_time = sampler->input_data[next_key_idx];
        float segment_duration = next_key_time - prev_key_time;
        float t = (segment_duration > 0.0f) ? ((current_time - prev_key_time) / segment_duration) : 0.0f;

        // 3.3 Interpolate the target property based on t
        switch (channel->target_path)
        {
        case GLTF_ANIMATION_PATH_TRANSLATION:
            {
                l3_3d_point_t *start_vec = (l3_3d_point_t *)&sampler->output_data[prev_key_idx * 3];
                l3_3d_point_t *end_vec   = (l3_3d_point_t *)&sampler->output_data[next_key_idx * 3];
                lerp_vector((l3_3d_point_t *)target_node->translation, start_vec, end_vec, t);
                break;
            }
        case GLTF_ANIMATION_PATH_SCALE:
            {
                l3_3d_point_t *start_vec = (l3_3d_point_t *)&sampler->output_data[prev_key_idx * 3];
                l3_3d_point_t *end_vec   = (l3_3d_point_t *)&sampler->output_data[next_key_idx * 3];
                lerp_vector((l3_3d_point_t *)target_node->scale, start_vec, end_vec, t);
                break;
            }
        case GLTF_ANIMATION_PATH_ROTATION:
            {
                l3_4d_point_t *start_quat = (l3_4d_point_t *)&sampler->output_data[prev_key_idx * 4];
                l3_4d_point_t *end_quat   = (l3_4d_point_t *)&sampler->output_data[next_key_idx * 4];
                slerp_quaternion((l3_4d_point_t *)target_node->rotation, start_quat, end_quat, t);
                break;
            }
        }
    }

    // 4. Recursively update the global transform of all nodes
    for (size_t i = 0; i < desc->scene_root_count; ++i)
    {
        int root_node_index = desc->scene_root_indices[i];
        l3_4x4_matrix_t identity_matrix;
        l3_4x4_matrix_identity(&identity_matrix);
        update_node_global_transform_recursive(_this, root_node_index, &identity_matrix);
    }

    // 5. Update the skin joint matrices
    for (uint32_t i = 0; i < desc->skin_count; ++i)
    {
        l3_gltf_skin_t *skin = &desc->skins[i];
        for (uint32_t j = 0; j < skin->joint_count; ++j)
        {
            uint32_t joint_node_index = skin->joint_indices[j];
            l3_gltf_node_t *joint_node = &desc->nodes[joint_node_index];

            // joint_matrices = global_transform * inverse_bind_matrices
            l3_4x4_matrix_mul(&joint_node->global_transform, &skin->inverse_bind_matrices[j],
                              &skin->joint_matrices[j]);
        }
    }
}

static void l3_draw_single_tria(l3_gltf_model_t *_this, l3_gltf_primitive_t *prim,
                                l3_tria_face_t *face)
{
    // Ignore backface
    if (!(face->state & L3_FACESTATE_BACKFACE))
    {
        l3_draw_tria_img_t tria_img;
        uint16_t render_color = 0;

        memset(&tria_img, 0x00, sizeof(l3_draw_tria_img_t));
        memcpy(&tria_img, face->transform_vertex, 3 * sizeof(l3_vertex_t));

        if (prim->material->texture_data != NULL)
        {
            tria_img.fill_data = prim->material->texture_data;

            tria_img.fill_type = L3_FILL_IMAGE_RGB565;
        }
        else
        {
            uint8_t *base_color = prim->material->base_color;
            uint8_t color_r = base_color[0];
            uint8_t color_g = base_color[1];
            uint8_t color_b = base_color[2];
            // uint8_t color_a = base_color[3];

            render_color = ((color_r & 0xF8) << 8) |
                           ((color_g & 0xFC) << 3) |
                           ((color_b & 0xF8) >> 3);

            tria_img.fill_type = L3_FILL_COLOR_RGB565;
            tria_img.fill_data = &render_color;
        }

        l3_draw_tria_to_canvas(&tria_img, _this->combined_img, _this->depthBuffer);
    }


}

static void render_primitive_no_skin(l3_gltf_model_t *_this, l3_gltf_primitive_t *prim,
                                     l3_4x4_matrix_t *mv_matrix)
{
    for (unsigned int i = 0; i < prim->triangle_count; ++i)
    {
        l3_tria_face_t face;
        l3_gltf_triangle_t *tria = &prim->triangles[i];

        {
            l3_gltf_complete_vertex_t *v0 = &tria->vertices[0];
            l3_4d_point_t local_position = {v0->pos.x, v0->pos.y, v0->pos.z, 1.0f};
            face.transform_vertex[0].position = l3_4x4_matrix_mul_4d_point(mv_matrix, local_position);
            face.transform_vertex[0].u = v0->texcoord.u;
            face.transform_vertex[0].v = v0->texcoord.v;
        }
        {
            l3_gltf_complete_vertex_t *v1 = &tria->vertices[1];
            l3_4d_point_t local_position = {v1->pos.x, v1->pos.y, v1->pos.z, 1.0f};
            face.transform_vertex[1].position = l3_4x4_matrix_mul_4d_point(mv_matrix, local_position);
            face.transform_vertex[1].u = v1->texcoord.u;
            face.transform_vertex[1].v = v1->texcoord.v;
        }
        {
            l3_gltf_complete_vertex_t *v2 = &tria->vertices[2];
            l3_4d_point_t local_position = {v2->pos.x, v2->pos.y, v2->pos.z, 1.0f};
            face.transform_vertex[2].position = l3_4x4_matrix_mul_4d_point(mv_matrix, local_position);
            face.transform_vertex[2].u = v2->texcoord.u;
            face.transform_vertex[2].v = v2->texcoord.v;
        }

        l3_tria_scene(&face, &_this->camera);
        l3_draw_single_tria(_this, prim, &face);
    }
}
static void render_primitive_with_skin(l3_gltf_model_t *_this, l3_gltf_primitive_t *prim,
                                       l3_4x4_matrix_t *view_matrix, l3_gltf_skin_t *skin)
{
    for (unsigned int i = 0; i < prim->triangle_count; ++i)
    {
        l3_tria_face_t face;
        l3_gltf_triangle_t *tria = &prim->triangles[i];

        for (int v_idx = 0; v_idx < 3; ++v_idx)
        {
            l3_gltf_complete_vertex_t *v = &tria->vertices[v_idx];
            l3_4d_point_t bind_pose_position = {v->pos.x, v->pos.y, v->pos.z, 1.0f};
            l3_4d_point_t view_space_position;

            l3_4d_point_t skinned_model_space_pos = {0.0f, 0.0f, 0.0f, 1.0f};  // Model space position
            for (int j = 0; j < 4; ++j)
            {
                float weight = v->weights[j];
                if (weight <= 0.0f) { continue; }

                uint16_t joint_index = v->joints[j];
                l3_4x4_matrix_t *joint_matrix = &skin->joint_matrices[joint_index];
                // Transform the vertex from model space to the space influenced by the bones
                l3_4d_point_t posed_position = l3_4x4_matrix_mul_4d_point(joint_matrix, bind_pose_position);

                // weighted accumulation
                skinned_model_space_pos.x += posed_position.x * weight;
                skinned_model_space_pos.y += posed_position.y * weight;
                skinned_model_space_pos.z += posed_position.z * weight;
            }

            // Model space position ==> View space position
            view_space_position = l3_4x4_matrix_mul_4d_point(view_matrix, skinned_model_space_pos);

            face.transform_vertex[v_idx].position = view_space_position;
            face.transform_vertex[v_idx].u = v->texcoord.u;
            face.transform_vertex[v_idx].v = v->texcoord.v;
        }

        l3_tria_scene(&face, &_this->camera);
        l3_draw_single_tria(_this, prim, &face);
    }

}

static void render_node_recursive(l3_gltf_model_t *_this, int node_index,
                                  l3_4x4_matrix_t *view_matrix)
{
    if (node_index < 0 || (unsigned int)node_index >= _this->desc->node_count) { return; }

    l3_gltf_node_t *node = &_this->desc->nodes[node_index];

    // Render the node if it has a mesh
    if (node->mesh_index != -1)
    {
        l3_gltf_skin_t *skin = NULL;
        if (node->skin_index != -1)
        {
            skin = &_this->desc->skins[node->skin_index];
        }

        l3_gltf_mesh_t *mesh = &_this->desc->meshes[node->mesh_index];

        l3_4x4_matrix_t model_matrix;
        l3_4x4_matrix_mul(view_matrix, &node->global_transform, &model_matrix);
        if (skin != NULL)
        {
            for (size_t i = 0; i < mesh->primitive_count; ++i)
            {
                l3_gltf_primitive_t *primitive = &mesh->primitives[i];
                render_primitive_with_skin(_this, primitive, &model_matrix, skin);
            }
        }
        else
        {
            for (size_t i = 0; i < mesh->primitive_count; ++i)
            {
                l3_gltf_primitive_t *primitive = &mesh->primitives[i];
                render_primitive_no_skin(_this, primitive, &model_matrix);
            }
        }
    }

    // Recursively render all child nodes
    for (unsigned int i = 0; i < node->num_children; ++i)
    {
        int child_index = node->children_indices[i];
        render_node_recursive(_this, child_index, view_matrix);
    }
}

void l3_gltf_push(l3_gltf_model_t *_this)
{
    // global transform
    if (_this->global_transform_cb != NULL)
    {
        _this->global_transform_cb(_this);
    }

    l3_camera_build_UVN_matrix(&_this->camera);
    l3_4x4_matrix_t view_matrix;
    l3_4x4_matrix_mul(&_this->camera.mat_cam, &_this->world, &view_matrix);

    uint32_t width = _this->viewPortWidth;
    uint32_t height = _this->viewPortHeight;
    memset(_this->depthBuffer, 0x00, width * height * sizeof(float));

    l3_img_head_t *head = (l3_img_head_t *)_this->combined_img->data;
    if (head->type == LITE_RGB565)
    {
        memset((uint8_t *)_this->combined_img->data + sizeof(l3_img_head_t), 0xFF, width * height * 2);
    }
    else if (head->type == LITE_ARGB8888)
    {
        memset((uint8_t *)_this->combined_img->data + sizeof(l3_img_head_t), 0x00, width * height * 4);
    }

    // Update animation
    static uint32_t current_time_ms = 0;
    current_time_ms = gui_ms_get();
    // gui_log("current_time_ms: %d\n", current_time_ms);
    l3_gltf_update_animation(_this, (current_time_ms - _this->last_time_ms) / 1000.0f);
    _this->last_time_ms = current_time_ms;

    // Start recursive rendering from the root node of all scenes.
    for (size_t i = 0; i < _this->desc->scene_root_count; ++i)
    {
        int root_node_index = _this->desc->scene_root_indices[i];

        render_node_recursive(_this, root_node_index, &view_matrix);
    }

    _this->combined_img->img_w = width;
    _this->combined_img->img_h = height;
    _this->combined_img->opacity_value = UINT8_MAX;
    _this->combined_img->blend_mode = L3_IMG_FILTER_BLACK;

    l3_3x3_matrix_translate(&_this->combined_img->matrix, _this->x, _this->y);
    memcpy(&_this->combined_img->inverse, &_this->combined_img->matrix, sizeof(l3_3x3_matrix_t));
    l3_3x3_matrix_inverse(&_this->combined_img->inverse);
    l3_calulate_draw_img_target_area(_this->combined_img, NULL);

}







