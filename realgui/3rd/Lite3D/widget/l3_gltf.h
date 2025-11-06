/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file l3_gltf.h
  * @brief Lite 3D widget
  * @details Lite 3D widget
  * @author sienna_shen@realsil.com.cn
  * @date 2025/10/28
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __L3_GLTF_H__
#define __L3_GLTF_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                            Header Files
 *============================================================================*/
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "l3_common.h"

/*============================================================================*
 *                               Types
 *============================================================================*/
/*
    Scene ==> Nodes ==> Meshes ==> Primitives ==> Triangles
                                              ==> Materials ==> Textures

    Animation ==> channels ==> nodes
                           ==> samplers

    Skins ==> joints ==> nodes
*/

// -------------------------- Disk Layout --------------------------
#pragma pack(1)

// Header
typedef struct
{
    uint32_t scene_root_count;
    uint32_t node_count;
    uint32_t mesh_count;
    uint32_t primitive_count;
    uint32_t material_count;
    uint32_t texture_count;

    uint32_t skin_count;
    uint32_t channel_count;
    uint32_t sampler_count;

    // Offset
    uint32_t scene_roots_offset;
    uint32_t nodes_offset;
    uint32_t meshes_offset;
    uint32_t primitives_offset;
    uint32_t materials_offset;
    uint32_t textures_offset;
    uint32_t skins_offset;
    uint32_t channels_offset;
    uint32_t samplers_offset;

    // Data blob
    uint32_t data_blob_offset;
    uint32_t data_blob_size;

} g3m_header_t;

// Node on disk
typedef struct
{
    // Local transform TRS
    float translation[3];
    float rotation[4];
    float scale[3];

    int32_t mesh_index;
    int32_t parent_index;
    uint32_t children_count;
    uint32_t children_offset;

    int32_t skin_index; // -1: no skin
} g3m_node_on_disk_t;

// Mesh on disk
typedef struct
{
    uint32_t primitive_start_index; // Primitive array start index
    uint32_t primitive_count;
} g3m_mesh_on_disk_t;

// Primitive on disk
typedef struct
{
    int32_t material_index;

    uint32_t triangles_offset; // Triangle array start index
    uint32_t triangle_count;
} g3m_primitive_on_disk_t;

// Material on disk
typedef struct
{
    uint8_t base_color[4];
    int32_t texture_index; // Base color texture index in textures array, -1: no texture
} g3m_material_on_disk_t;

// Texture on disk
typedef struct
{
    uint32_t data_offset; // Pixel data offset in data blob
    uint32_t data_size;
} g3m_texture_on_disk_t;

// Skin on disk
typedef struct
{
    uint32_t inverse_bind_matrices_offset;

    uint32_t joints_offset; // Joint array start index in joint_buffer
    uint32_t joints_count; // Number of joints in this skin
} g3m_skin_on_disk_t;

// Animation path type enum
typedef enum
{
    GLTF_ANIMATION_PATH_TRANSLATION,
    GLTF_ANIMATION_PATH_ROTATION,
    GLTF_ANIMATION_PATH_SCALE
} gltf_animation_path_t;

// Animation interpolation type enum
typedef enum
{
    GLTF_INTERPOLATION_TYPE_LINEAR,
    GLTF_INTERPOLATION_TYPE_STEP,
    GLTF_INTERPOLATION_TYPE_CUBICSPLINE
} gltf_interpolation_type_t;

// Sampler output data type enum
typedef enum
{
    GLTF_TYPE_VEC3,
    GLTF_TYPE_VEC4
} gltf_data_type;

// Sampler on disk
typedef struct
{
    // Time keyframe array (float[])
    uint32_t input_offset;
    uint32_t input_count;

    // Keyframe value array (vec3[]/vec4[])
    uint32_t output_offset;
    uint32_t output_count;

    uint32_t interpolation_type;

    uint32_t output_type; // VEC3/VEC4

} g3m_sampler_on_disk_t;

// Channel on disk
typedef struct
{
    uint32_t sampler_index;

    uint32_t target_node_index;
    uint32_t target_path;
} g3m_channel_on_disk_t;

#pragma pack()

typedef struct
{
    uint8_t *data;
    size_t size;
    size_t capacity;
} g3m_data_blob_t;

// -------------------------- Run-time Layout --------------------------
typedef struct
{
    l3_vertex_coordinate_t pos;
    l3_vertex_coordinate_t normal;
    l3_texcoord_coordinate_t texcoord;

    unsigned short joints[4];  // JOINTS_0 : Which four bones affect this vertex
    float weights[4];  // WEIGHTS_0 : The influence weights of the four bones above
} l3_gltf_complete_vertex_t;

typedef struct
{
    l3_gltf_complete_vertex_t vertices[3];
} l3_gltf_triangle_t;

typedef struct
{
    uint8_t base_color[4];   //  0-255
    void *texture_data;
    uint32_t texture_length;
} l3_gltf_material_t;

typedef struct
{
    // Local transform TRS
    float translation[3];
    float rotation[4];
    float scale[3];
    l3_4x4_matrix_t global_transform;

    int mesh_index; // -1: no mesh

    int parent_index; // -1: no parent
    int *children_indices;
    unsigned int num_children;

    int skin_index;

} l3_gltf_node_t;


typedef struct
{
    l3_gltf_material_t *material;

    l3_gltf_triangle_t *triangles;
    unsigned int triangle_count;
} l3_gltf_primitive_t;

typedef struct
{
    l3_gltf_primitive_t *primitives;
    unsigned int primitive_count;
} l3_gltf_mesh_t;


typedef struct
{
    gltf_interpolation_type_t interpolation;

    float *input_data;
    uint32_t input_count;

    // Output: Translation, Rotation, Scale
    float *output_data;
    uint32_t output_count;
    gltf_data_type output_type;  // Vec3: N*3 float, Vec4: N*4 float
} l3_gltf_sampler_t;


typedef struct
{
    uint32_t sampler_index;
    uint32_t target_node_index;
    gltf_animation_path_t target_path;
} l3_gltf_channel_t;

typedef struct
{
    l3_gltf_channel_t *channels;
    uint32_t channel_count;

    l3_gltf_sampler_t *samplers;
    uint32_t sampler_count;

    float duration;
    float animation_time;
} l3_gltf_single_animation_t;

typedef struct
{
    uint32_t *joint_indices; // Joint indices in nodes array
    uint32_t  joint_count;

    l3_4x4_matrix_t *inverse_bind_matrices; // IBM, num = joint_count
    l3_4x4_matrix_t *joint_matrices;
} l3_gltf_skin_t;

typedef struct
{
    // Scene root indices in nodes array
    int *scene_root_indices;
    unsigned int scene_root_count;

    // Nodes array
    l3_gltf_node_t *nodes;
    unsigned int node_count;

    // Meshes array
    l3_gltf_mesh_t *meshes;
    unsigned int mesh_count;

    l3_gltf_material_t *materials;
    unsigned int material_count;

    l3_gltf_skin_t *skins;
    uint32_t skin_count;

    l3_gltf_single_animation_t *animation;

} l3_gltf_model_description_t;


typedef struct l3_gltf_model
{
    l3_gltf_model_description_t *desc;

    l3_draw_rect_img_t *combined_img;  // sort image buffer
    float *depthBuffer;

    l3_canvas_t canvas;

    int16_t x;
    int16_t y;
    float viewPortWidth;
    float viewPortHeight;
    l3_world_t world;
    l3_camera_t camera;
    l3_light_t light;

    uint32_t last_time_ms;   // Last animation time in ms

    void (*global_transform_cb)(struct l3_gltf_model *_this);
    l3_4x4_matrix_t(*face_transform_cb)(struct l3_gltf_model *_this, size_t face_index);

} l3_gltf_model_t;

/**
 * @brief Callback type for global transformations.
 *
 * This callback is used to apply transformations to the entire 3D model,
 * including world coordinates and camera coordinates.
 */
typedef void (*l3_gltf_global_transform_cb)(l3_gltf_model_t *__this);

/**
 * @brief Callback type for face-specific transformations.
 *
 * This callback is used to apply transformations to a specific face of the 3D model.
 * @param face_index The index of the face to transform.
 */
typedef void (*l3_gltf_face_transform_cb)(l3_gltf_model_t *__this,
                                          size_t face_index/*face offset*/);
/*============================================================================*
 *                            Functions
 *============================================================================*/

/**
 * @brief Create a new 3D GLTF model.
 *
 * @param desc_addr Address of the model descriptor.
 * @param x X-coordinate of the model.
 * @param y Y-coordinate of the model.
 * @param view_w Width of the view.
 * @param view_h Height of the view.
 * @return Pointer to the created 3D model.
 */
l3_gltf_model_t *l3_create_gltf_model(void *desc_addr, L3_IMAGE_TYPE image_type, int16_t x,
                                      int16_t y, int16_t view_w, int16_t view_h);

/**
 * @brief Set the target canvas for rendering.
 *
 * @param _this Pointer to the 3D model.
 * @param x X-coordinate of the canvas.
 * @param y Y-coordinate of the canvas.
 * @param w Width of the canvas.
 * @param h Height of the canvas.
 * @param bit_depth Bit depth of the canvas.
 * @param canvas Pointer to the canvas buffer.
 */
void l3_gltf_set_target_canvas(l3_gltf_model_t *_this, \
                               uint16_t x, uint16_t y, \
                               uint16_t w, uint16_t h, \
                               uint16_t bit_depth, \
                               uint8_t *canvas);



/**
 * @brief Set global transform callback.
 *
 * @param _this the 3D model pointer
 * @param cb Set callback functions for the world coordinate system, camera coordinate system,
 *           and light source for all faces
 */
void l3_gltf_set_global_transform(l3_gltf_model_t *_this, l3_gltf_global_transform_cb cb);

/**
 * @brief Push the model's state to the rendering pipeline.
 *
 * @param _this Pointer to the 3D model.
 */
void l3_gltf_push(l3_gltf_model_t *_this);

/**
 * @brief Render the model.
 *
 * @param _this Pointer to the 3D model.
 */
void l3_gltf_draw(l3_gltf_model_t *_this);


/**
 * @brief Free the memory allocated for the 3D model.
 *
 * @param _this Pointer to the 3D model.
 */
void l3_free_gltf_model(l3_gltf_model_t *_this);



#ifdef __cplusplus
}
#endif

#endif
