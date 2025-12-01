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
    if (len_sq > 1e-6f)
    {
        float inv_len = 1.0f / sqrtf(len_sq);
        q->x *= inv_len;
        q->y *= inv_len;
        q->z *= inv_len;
        q->w *= inv_len;
    }
}

// Spherical Linear Interpolation
static void slerp_quaternion(l3_4d_point_t *out, const l3_4d_point_t *start,
                             const l3_4d_point_t *end, float t)
{
    float cos_half_theta = dot_quaternion(start, end);

    l3_4d_point_t temp_end = *end;

    // If the angle between two quaternions is greater than 90 degrees, take the opposite of end.
    if (cos_half_theta < 0.0f)
    {
        temp_end.x = -temp_end.x;
        temp_end.y = -temp_end.y;
        temp_end.z = -temp_end.z;
        temp_end.w = -temp_end.w;
        cos_half_theta = -cos_half_theta;
    }

    // If two quaternions are very close, directly use linear interpolation.
    if (cos_half_theta > 0.9999f)
    {
        out->x = start->x + t * (temp_end.x - start->x);
        out->y = start->y + t * (temp_end.y - start->y);
        out->z = start->z + t * (temp_end.z - start->z);
        out->w = start->w + t * (temp_end.w - start->w);
        normalize_quaternion(out);
        return;
    }

    // Standard Slerp Formula
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

    // 1. Update the animation time
    anim->animation_time += dt;

    // 2. Wrap the animation time within [0, duration)
    if (anim->duration > 0.0f && anim->animation_time > anim->duration)
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

        // 3.1 Find the keyframes interval using binary search - O(log n) instead of O(n)
        uint32_t prev_key_idx = 0;
        uint32_t next_key_idx = 0;

        if (!sampler->input_data || sampler->input_count < 2)
        {
            continue;
        }

        // Handle edge cases
        if (current_time <= sampler->input_data[0])
        {
            prev_key_idx = 0;
            next_key_idx = 0;
        }
        else if (current_time >= sampler->input_data[sampler->input_count - 1])
        {
            prev_key_idx = sampler->input_count - 1;
            next_key_idx = sampler->input_count - 1;
        }
        else
        {
            // Binary search to find interval [left, right] containing current_time
            uint32_t left = 0;
            uint32_t right = sampler->input_count - 1;

            while (left < right - 1)
            {
                uint32_t mid = left + (right - left) / 2;  // Avoid overflow

                if (current_time < sampler->input_data[mid])
                {
                    right = mid;
                }
                else if (current_time > sampler->input_data[mid])
                {
                    left = mid;
                }
                else
                {
                    // Exact match
                    left = mid;
                    right = mid;
                    break;
                }
            }

            prev_key_idx = left;
            next_key_idx = right;
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

        if (prim->material && prim->material->texture_data != NULL)
        {
            tria_img.fill_data = prim->material->texture_data;

            tria_img.fill_type = L3_FILL_IMAGE_RGB565;
        }
        else
        {
            uint8_t *base_color = prim->material ? prim->material->base_color : (uint8_t[]) {128, 128, 128, 255};

            float nz = fabsf(tria_img.p0.normal.uz);
            const float MIN_LIGHT_INTENSITY = 0.7f;
            const float MAX_LIGHT_INTENSITY = 1.0f;
            float light_intensity = fmaxf(MIN_LIGHT_INTENSITY, fminf(MAX_LIGHT_INTENSITY, nz));

            uint8_t color_r = (uint8_t)(base_color[0] * light_intensity);
            uint8_t color_g = (uint8_t)(base_color[1] * light_intensity);
            uint8_t color_b = (uint8_t)(base_color[2] * light_intensity);

            render_color = ((color_r & 0xF8) << 8) |
                           ((color_g & 0xFC) << 3) |
                           ((color_b & 0xF8) >> 3);

            tria_img.fill_type = L3_FILL_COLOR_RGB565;
            tria_img.fill_data = &render_color;
        }

        l3_draw_tria_to_canvas(&tria_img, _this->base.combined_img, _this->depthBuffer);
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

        l3_tria_scene(&face, &_this->base.camera);
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

        l3_tria_scene(&face, &_this->base.camera);
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

void l3_gltf_prepare(l3_gltf_model_t *_this)
{
    if (!_this || !_this->desc || !_this->base.combined_img)
    {
        return;
    }

    // global transform
    if (_this->global_transform_cb != NULL)
    {
        _this->global_transform_cb(_this);
    }

    l3_camera_build_UVN_matrix(&_this->base.camera);
    l3_4x4_matrix_t view_matrix;
    l3_4x4_matrix_mul(&_this->base.camera.mat_cam, &_this->base.world, &view_matrix);

    uint32_t width = _this->base.viewPortWidth;
    uint32_t height = _this->base.viewPortHeight;
    memset(_this->depthBuffer, 0x00, width * height * sizeof(float));

    l3_img_head_t *head = (l3_img_head_t *)_this->base.combined_img->data;
    if (head->type == LITE_RGB565)
    {
        memset((uint8_t *)_this->base.combined_img->data + sizeof(l3_img_head_t), 0xFF, width * height * 2);
    }
    else if (head->type == LITE_ARGB8888)
    {
        memset((uint8_t *)_this->base.combined_img->data + sizeof(l3_img_head_t), 0x00, width * height * 4);
    }

    // Update animation
    if (_this->desc->animation != NULL)
    {
        // Calculate delta time
        uint32_t current_time_ms = gui_ms_get();
        float dt = (_this->last_time_ms > 0) ? ((current_time_ms - _this->last_time_ms) / 1000.0f) : 0.0f;
        _this->last_time_ms = current_time_ms;
        l3_gltf_update_animation(_this, dt);
    }

    // Start recursive rendering from the root node of all scenes.
    for (size_t i = 0; i < _this->desc->scene_root_count; ++i)
    {
        int root_node_index = _this->desc->scene_root_indices[i];

        if (_this->desc->animation == NULL)
        {
            l3_gltf_node_t *node = &_this->desc->nodes[root_node_index];
            l3_4x4_matrix_compose_trs(&node->global_transform, node->translation, node->rotation, node->scale);
        }

        render_node_recursive(_this, root_node_index, &view_matrix);
    }

    _this->base.combined_img->img_w = width;
    _this->base.combined_img->img_h = height;
    _this->base.combined_img->opacity_value = UINT8_MAX;
    _this->base.combined_img->blend_mode = L3_IMG_FILTER_BLACK;

    l3_3x3_matrix_translate(&_this->base.combined_img->matrix, _this->base.x, _this->base.y);
    memcpy(&_this->base.combined_img->inverse, &_this->base.combined_img->matrix,
           sizeof(l3_3x3_matrix_t));
    l3_3x3_matrix_inverse(&_this->base.combined_img->inverse);
    l3_calulate_draw_img_target_area(_this->base.combined_img, NULL);

}







