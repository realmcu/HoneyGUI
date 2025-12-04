/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file def_3d_rect.c
  * @brief 3D object calculation composed of rectangular grids
  * @details 3D object calculation composed of rectangular grids
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/7
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

#include <stdio.h>
#include <math.h>
#include "l3_common.h"


// void l3_light_initialize(l3_light_t *light, l3_4d_point_t lightPosition,
//                          l3_4d_point_t lightTarget, float included_angle, float blend_ratio, l3_color_argb8888_t color)
// {
//     light->position = lightPosition;
//     light->targetDirection = lightTarget;
//     light->included_angle = included_angle;
//     light->blend_ratio = blend_ratio;
//     light->color = color;
//     light->initialized = true;
// }


// static void l3_rect_face_transform_camera(l3_rect_face_t *face, l3_camera_t *camera)
// {
//     face->transform_vertex[0].position = l3_4x4_matrix_mul_4d_point(&camera->mat_cam,
//                                                                     face->transform_vertex[0].position);
//     face->transform_vertex[1].position = l3_4x4_matrix_mul_4d_point(&camera->mat_cam,
//                                                                     face->transform_vertex[1].position);
//     face->transform_vertex[2].position = l3_4x4_matrix_mul_4d_point(&camera->mat_cam,
//                                                                     face->transform_vertex[2].position);
//     face->transform_vertex[3].position = l3_4x4_matrix_mul_4d_point(&camera->mat_cam,
//                                                                     face->transform_vertex[3].position);
// }


static void l3_rect_face_calculate_normal(l3_rect_face_t *face)
{
    l3_4d_vector_t v1, v2;

    v1 = l3_4d_point_sub(face->transform_vertex[1].position, face->transform_vertex[0].position);
    v2 = l3_4d_point_sub(face->transform_vertex[2].position, face->transform_vertex[1].position);
    face->transform_vertex[0].normal = l3_4d_vector_unit(l3_4d_vector_cross(v1, v2));
    face->transform_vertex[1].normal = face->transform_vertex[0].normal;
    face->transform_vertex[2].normal = face->transform_vertex[0].normal;
    face->transform_vertex[3].normal = face->transform_vertex[0].normal;

}

static void l3_rect_face_update_back_face(l3_rect_face_t *face, L3_CULLMODE cullmode)
{
    if (cullmode == L3_CULLMODE_NONE)
    {
        face->state &= ~L3_FACESTATE_BACKFACE;
        return;
    }

    bool bBackFace = true;
    l3_4d_vector_t n = face->transform_vertex[0].normal;
    l3_4d_point_t v1 = {n.ux, n.uy, n.uz, 1.0f};
    float dot;

    for (int j = 0; j < 4; j++)
    {
        l3_4d_point_t v2 = face->transform_vertex[j].position;
        dot = l3_4d_point_dot(v1, v2);

        if ((dot > 0 && cullmode != L3_CULLMODE_CCW) ||
            (dot <= 0 && cullmode != L3_CULLMODE_CW))
        {
            bBackFace = false;
            break;
        }
    }

    if (bBackFace)
    {
        face->state |= L3_FACESTATE_BACKFACE;
    }
    else
    {
        face->state &= ~L3_FACESTATE_BACKFACE;
    }

}


static void l3_rect_face_transform_perspective(l3_rect_face_t *face, l3_camera_t *camera)
{
    float z;

    for (int i = 0; i < 4; i++)
    {
        z = 1 / face->transform_vertex[i].position.z;
        face->transform_vertex[i].position.x = camera->d * face->transform_vertex[i].position.x * z /
                                               camera->aspect_ratio;
        face->transform_vertex[i].position.y = camera->d * face->transform_vertex[i].position.y * z;
    }
}


static void l3_rect_face_transform_screen(l3_rect_face_t *face, l3_camera_t *camera)
{
    for (int i = 0; i < 4; i++)
    {
        // Transform to screen space
        face->transform_vertex[i].position.x = camera->viewport_center_x + camera->viewport_center_x *
                                               face->transform_vertex[i].position.x;
        face->transform_vertex[i].position.y = camera->viewport_center_y + camera->viewport_center_y *
                                               face->transform_vertex[i].position.y;
    }

}

void l3_rect_scene(l3_rect_face_t *face, l3_camera_t *camera)
{
    // l3_rect_face_transform_camera(face, camera);
    l3_rect_face_calculate_normal(face);
    l3_rect_face_update_back_face(face, L3_CULLMODE_CCW);
    l3_rect_face_transform_perspective(face, camera);
    l3_rect_face_transform_screen(face, camera);
}




