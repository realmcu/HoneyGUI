/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file def_3d_rect.c
  * @brief 3D object calculation composed of rectangular grids
  * @details 3D object calculation composed of rectangular grids
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/7
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

#include <stdio.h>
#include <math.h>
#include "def_3d_rect.h"



void gui_3d_light_inititalize(gui_3d_light_t *light, gui_point_4d_t lightPosition,
                              gui_point_4d_t lightTarget, float included_angle, float blend_ratio, gui_3d_RGBAcolor_t color)
{
    light->position = lightPosition;
    light->targetDirection = lightTarget;
    light->included_angle = included_angle;
    light->blend_ratio = blend_ratio;
    light->color = color;
    light->initialized = true;
}

void gui_3d_rect_face_transform(gui_3d_rect_face_t *face, gui_3d_matrix_t mat,
                                GUI_3D_FACE_TRANSFORM mode)
{
    switch (mode)
    {
    case GUI_3D_FACE_TRANSFORM_LOCAL_TO_LOCAL:
        {
            {
                face->vertex[0].position = gui_3d_point4D_mul_matrix(face->vertex[0].position, mat);
                face->vertex[1].position = gui_3d_point4D_mul_matrix(face->vertex[1].position, mat);
                face->vertex[2].position = gui_3d_point4D_mul_matrix(face->vertex[2].position, mat);
                face->vertex[3].position = gui_3d_point4D_mul_matrix(face->vertex[3].position, mat);
            }
            break;
        }
    case GUI_3D_FACE_TRANSFORM_LOACL_TO_GLOBAL:
        {
            {
                face->transform_vertex[0] = face->vertex[0];
                face->transform_vertex[1] = face->vertex[1];
                face->transform_vertex[2] = face->vertex[2];
                face->transform_vertex[3] = face->vertex[3];
                face->transform_vertex[0].position = gui_3d_point4D_mul_matrix(face->vertex[0].position, mat);
                face->transform_vertex[1].position = gui_3d_point4D_mul_matrix(face->vertex[1].position, mat);
                face->transform_vertex[2].position = gui_3d_point4D_mul_matrix(face->vertex[2].position, mat);
                face->transform_vertex[3].position = gui_3d_point4D_mul_matrix(face->vertex[3].position, mat);

                face->transform_world_vertex[0].position = face->transform_vertex[0].position;
                face->transform_world_vertex[1].position = face->transform_vertex[1].position;
                face->transform_world_vertex[2].position = face->transform_vertex[2].position;
                face->transform_world_vertex[3].position = face->transform_vertex[3].position;

            }
            break;
        }
    case GUI_3D_FACE_TRANSFORM_GLOBAL_TO_GLOBAL:
        {
            {
                face->transform_vertex[0].position = gui_3d_point4D_mul_matrix(face->transform_vertex[0].position,
                                                                               mat);
                face->transform_vertex[1].position = gui_3d_point4D_mul_matrix(face->transform_vertex[1].position,
                                                                               mat);
                face->transform_vertex[2].position = gui_3d_point4D_mul_matrix(face->transform_vertex[2].position,
                                                                               mat);
                face->transform_vertex[3].position = gui_3d_point4D_mul_matrix(face->transform_vertex[3].position,
                                                                               mat);
            }
            break;
        }
    default:
        {
            break;
        }
    }
}

void gui_3d_rect_face_transform_local_to_global(gui_3d_rect_face_t *face, gui_3d_world_t *world)
{
    gui_3d_rect_face_transform(face, *world, GUI_3D_FACE_TRANSFORM_LOACL_TO_GLOBAL);
}


void gui_3d_rect_face_transform_local_to_local(gui_3d_rect_face_t *face, gui_3d_matrix_t *m)
{
    gui_3d_rect_face_transform(face, *m, GUI_3D_FACE_TRANSFORM_LOCAL_TO_LOCAL);
}

void gui_3d_rect_face_transform_camera(gui_3d_rect_face_t *face, gui_3d_camera_t *camera)
{
    gui_3d_rect_face_transform(face, camera->mat_cam, GUI_3D_FACE_TRANSFORM_GLOBAL_TO_GLOBAL);
}

void gui_3d_rect_face_cull_region(gui_3d_rect_face_t *face, gui_3d_camera_t *camera)
{
    bool outside = false;
    int beyondCount = 0;

    for (int j = 0; j < 4; j++)
    {
        float z = face->transform_vertex[j].position.z;
        // Check if the vertex is behind the near plane
        if (z <= camera->near_z)
        {
            outside = true;
            break;
        }

        // Count vertices beyond the far plane
        if (z > camera->far_z)
        {
            beyondCount++;
        }
    }
    // If all vertices are beyond the far plane
    if (beyondCount == 4)
    {
        outside = true;
    }
    if (outside)
    {
        face->state |= GUI_3D_FACESTATE_CLIPPED;
    }
    else
    {
        face->state &= ~GUI_3D_FACESTATE_CLIPPED;
    }

}

void gui_3d_rect_face_calculate_normal(gui_3d_rect_face_t *face)
{
    gui_vector4D_t v1, v2;

    v1 = gui_point_4d_sub(face->transform_vertex[1].position, face->transform_vertex[0].position);
    v2 = gui_point_4d_sub(face->transform_vertex[2].position, face->transform_vertex[1].position);
    face->transform_vertex[0].normal = gui_point_4d_unit(gui_point_4d_cross(v1, v2));
    face->transform_vertex[1].normal = face->transform_vertex[0].normal;
    face->transform_vertex[2].normal = face->transform_vertex[0].normal;
    face->transform_vertex[3].normal = face->transform_vertex[0].normal;

}

void gui_3d_rect_face_update_back_face(gui_3d_rect_face_t *face, GUI_3D_CULLMODE cullmode)
{
    if (cullmode == GUI_3D_CULLMODE_NONE)
    {
        face->state &= ~GUI_3D_FACESTATE_BACKFACE;
        return;
    }

    bool bBackFace = true;
    gui_vector4D_t v1 = face->transform_vertex[0].normal;
    float dot;

    for (int j = 0; j < 4; j++)
    {
        gui_vector4D_t v2 = face->transform_vertex[j].position;
        dot = gui_point4D_dot(v1, v2);

        if ((dot > 0 && cullmode != GUI_3D_CULLMODE_CCW) ||
            (dot <= 0 && cullmode != GUI_3D_CULLMODE_CW))
        {
            bBackFace = false;
            break;
        }
    }

    if (bBackFace)
    {
        face->state |= GUI_3D_FACESTATE_BACKFACE;
    }
    else
    {
        face->state &= ~GUI_3D_FACESTATE_BACKFACE;
    }

}


void gui_3d_rect_face_transform_perspective(gui_3d_rect_face_t *face, gui_3d_camera_t *camera)
{
    float z;
    z = face->transform_vertex[0].position.z;
    face->transform_vertex[0].position.x = camera->d * face->transform_vertex[0].position.x / z;
    face->transform_vertex[0].position.y = camera->d * face->transform_vertex[0].position.y / z;

    z = face->transform_vertex[1].position.z;
    face->transform_vertex[1].position.x = camera->d * face->transform_vertex[1].position.x / z;
    face->transform_vertex[1].position.y = camera->d * face->transform_vertex[1].position.y / z;

    z = face->transform_vertex[2].position.z;
    face->transform_vertex[2].position.x = camera->d * face->transform_vertex[2].position.x / z;
    face->transform_vertex[2].position.y = camera->d * face->transform_vertex[2].position.y / z;

    z = face->transform_vertex[3].position.z;
    face->transform_vertex[3].position.x = camera->d * face->transform_vertex[3].position.x / z;
    face->transform_vertex[3].position.y = camera->d * face->transform_vertex[3].position.y / z;
}


void gui_3d_rect_face_cull_out_side(gui_3d_rect_face_t *face, const gui_3d_camera_t *camera)
{
    float half_viewplane_height = camera->viewplane_height / 2;
    float half_viewplane_width = camera->viewplane_width / 2;

    if (face->transform_vertex[0].position.y > half_viewplane_height && \
        face->transform_vertex[1].position.y > half_viewplane_height && \
        face->transform_vertex[2].position.y > half_viewplane_height)
    {
        //up
        face->state |= GUI_3D_FACESTATE_CLIPPED;
        return;
    }
    else if (face->transform_vertex[0].position.y < -half_viewplane_height && \
             face->transform_vertex[1].position.y < -half_viewplane_height && \
             face->transform_vertex[2].position.y < -half_viewplane_height)
    {
        //down
        face->state |= GUI_3D_FACESTATE_CLIPPED;
        return;
    }
    else if (face->transform_vertex[0].position.x < -half_viewplane_width && \
             face->transform_vertex[1].position.x < -half_viewplane_width && \
             face->transform_vertex[2].position.x < -half_viewplane_width)
    {
        //left
        face->state |= GUI_3D_FACESTATE_CLIPPED;
        return;
    }
    else if (face->transform_vertex[0].position.x > half_viewplane_width && \
             face->transform_vertex[1].position.x > half_viewplane_width && \
             face->transform_vertex[2].position.x > half_viewplane_width)
    {
        //right
        face->state |= GUI_3D_FACESTATE_CLIPPED;
        return;
    }

    face->state &= ~GUI_3D_FACESTATE_CLIPPED;
}

void gui_3d_rect_face_transform_screen(gui_3d_rect_face_t *face, gui_3d_camera_t *camera)
{
    float alpha = 0.5f * (camera->viewport_width - 1);
    float beta = 0.5f * (camera->viewport_height - 1);

    for (int i = 0; i < 4; i++)
    {
        // Transform to screen space
        face->transform_vertex[i].position.x = alpha + alpha * face->transform_vertex[i].position.x;
        face->transform_vertex[i].position.y = beta + beta * face->transform_vertex[i].position.y;

        // Invert y-axis
        face->transform_vertex[i].position.y = camera->viewport_height -
                                               face->transform_vertex[i].position.y;
    }

}

void gui_3d_rect_scene(gui_3d_rect_face_t *face, gui_3d_world_t *world, gui_3d_camera_t *camera)
{
    gui_3d_rect_face_transform_local_to_global(face, world);
    gui_3d_camera_build_UVN_matrix(camera);
    gui_3d_rect_face_transform_camera(face, camera);
    gui_3d_rect_face_cull_region(face, camera);
    gui_3d_rect_face_calculate_normal(face);
    gui_3d_rect_face_update_back_face(face, GUI_3D_CULLMODE_CCW);
    gui_3d_rect_face_transform_perspective(face, camera);
    gui_3d_rect_face_cull_out_side(face, camera);
    gui_3d_rect_face_transform_screen(face, camera);
}




