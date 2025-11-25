/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file l3_obj.h
  * @brief Lite 3D obj widget
  * @details Lite 3D obj widget
  * @author sienna_shen@realsil.com.cn
  * @date 2025/4/11
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
#ifndef __L3_OBJ_H__
#define __L3_OBJ_H__
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
#include "l3.h"

/*============================================================================*
 *                               Types
 *============================================================================*/
typedef struct
{
    unsigned int num_vertices;
    unsigned int num_normals;
    unsigned int num_texcoords;
    unsigned int num_faces;
    unsigned int num_face_num_verts;

    int pad0;

    l3_vertex_coordinate_t *vertices;
    l3_vertex_coordinate_t *normals;
    l3_texcoord_coordinate_t *texcoords;
    l3_vertex_index_t *faces;
    int *face_num_verts;
    int *material_ids;
} l3_obj_attrib_t;

typedef struct
{
    unsigned int face_offset;
    unsigned int length;
} l3_obj_shape_t;

typedef struct
{
    float ambient[3];
    float diffuse[3];
    float specular[3];
    float transmittance[3];
    float emission[3];
    float shininess;
    float ior;      /* index of refraction */
    float dissolve; /* 1 == opaque; 0 == fully transparent */
    /* illumination model (see http://www.fileformat.info/format/material/) */
    int illum;

} l3_obj_material_t;

typedef struct l3_description
{
    l3_desc_file_head_t file_head;

    l3_obj_attrib_t attrib;

    unsigned int num_shapes;
    l3_obj_shape_t *shapes;

    unsigned int num_materials;
    l3_obj_material_t *materials;

    unsigned int *texture_sizes;
    unsigned char **textures;

} l3_obj_model_description_t;

typedef struct l3_obj_model
{
    l3_model_base_t base;

    l3_obj_model_description_t *desc;
    union
    {
        l3_rect_face_t *rect_face;
        l3_tria_face_t *tria_face;
    } face;
    l3_draw_rect_img_t *img;

    void (*global_transform_cb)(struct l3_obj_model *this);
    l3_4x4_matrix_t(*face_transform_cb)(struct l3_obj_model *this, size_t face_index);

} l3_obj_model_t;

/*============================================================================*
 *                            Functions
 *============================================================================*/

/**
 * @brief Create a new 3D OBJ model.
 *
 * @param desc_addr Address of the model descriptor.
 * @param draw_type Drawing type for the model.
 * @param x X-coordinate of the model.
 * @param y Y-coordinate of the model.
 * @param view_w Width of the view.
 * @param view_h Height of the view.
 * @return Pointer to the created 3D model.
 */
l3_obj_model_t *l3_create_obj_model(void *desc_addr, L3_DRAW_TYPE draw_type, int16_t x,
                                    int16_t y, int16_t view_w, int16_t view_h);

/**
 * @brief Set the target canvas for rendering.
 *
 * @param base Pointer to the 3D base model.
 * @param x X-coordinate of the canvas.
 * @param y Y-coordinate of the canvas.
 * @param w Width of the canvas.
 * @param h Height of the canvas.
 * @param bit_depth Bit depth of the canvas.
 * @param canvas Pointer to the canvas buffer.
 */
void l3_obj_set_target_canvas(l3_model_base_t *base, \
                              uint16_t x, uint16_t y, \
                              uint16_t w, uint16_t h, \
                              uint16_t bit_depth, \
                              uint8_t *canvas);

/**
 * @brief Set the image for a specific face of the model.
 *
 * @param base Pointer to the 3D base model.
 * @param face_index Index of the face.
 * @param image_addr Address of the image data.
 */
void l3_obj_set_face_image(l3_model_base_t *base, uint8_t face_index, void *image_addr);

/**
 * @brief Set global transform callback.
 *
 * @param base Pointer to the 3D base model.
 * @param cb Set callback functions for the world coordinate system, camera coordinate system,
 *           and light source for all faces.
 */
void l3_obj_set_global_transform(l3_model_base_t *base, l3_global_transform_cb cb);

/**
 * @brief Set face transform callback.
 *
 * @param base Pointer to the 3D base model.
 * @param cb Set callback functions for the world coordinate system, camera coordinate system,
 *           and light source for the specified face
 */
void l3_obj_set_face_transform(l3_model_base_t *base, l3_face_transform_cb cb);

/**
 * @brief Push the model's state to the rendering pipeline.
 *
 * @param base Pointer to the 3D base model.
 */
void l3_obj_push(l3_model_base_t *base);

/**
 * @brief Render the model.
 *
 * @param base Pointer to the 3D base model.
 */
void l3_obj_draw(l3_model_base_t *base);


/**
 * @brief Free the memory allocated for the 3D model.
 *
 * @param base Pointer to the 3D base model.
 */
void l3_free_obj_model(l3_model_base_t *base);

/**
 * @brief Check if the model is clicked at the specified coordinates.
 *
 * @param base Pointer to the 3D base model.
 * @param x X-coordinate of the click.
 * @param y Y-coordinate of the click.
 * @return True if the model is clicked, false otherwise.
 */
bool l3_obj_model_on_click(l3_model_base_t *base, int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif
