/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file def_3d_tria.c
  * @brief 3D object calculation composed of triaangular grids
  * @details 3D object calculation composed of triaangular grids
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
#include "def_3d_tria.h"



static void gui_3d_tria_face_transform_local_to_global(gui_3d_tria_face_t *face, size_t face_index,
                                                       gui_obj_attrib_t *attrib, gui_3d_world_t *world)
{
    size_t index_offset = face_index * 3;

    for (size_t j = 0; j < 3; j++)
    {
        gui_obj_vertex_index_t idx = attrib->faces[index_offset + j];

        gui_obj_vertex_coordinate_t *v = &attrib->vertices[idx.v_idx];
        // gui_obj_texcoord_coordinate_t *vt = &attrib->texcoords[idx.vt_idx];
        // gui_obj_vertex_coordinate_t *vn = &attrib->normals[idx.vn_idx];

        gui_point_4d_t local_position = {v->x, v->y, v->z, 1.0f};
        face->transform_vertex[j].position = gui_3d_point4D_mul_matrix(local_position, *world);

        // face->transform_vertex[j].u = vt->u;
        // face->transform_vertex[j].v = vt->v;
        // face->transform_vertex[j].normal.x = vn->x;
        // face->transform_vertex[j].normal.y = vn->y;
        // face->transform_vertex[j].normal.z = vn->z;
        // face->transform_vertex[j].normal.w = 1;

    }
}


static void gui_3d_tria_face_transform_camera(gui_3d_tria_face_t *face, gui_3d_camera_t *camera)
{
    face->transform_vertex[0].position = gui_3d_point4D_mul_matrix(face->transform_vertex[0].position,
                                                                   camera->mat_cam);
    face->transform_vertex[1].position = gui_3d_point4D_mul_matrix(face->transform_vertex[1].position,
                                                                   camera->mat_cam);
    face->transform_vertex[2].position = gui_3d_point4D_mul_matrix(face->transform_vertex[2].position,
                                                                   camera->mat_cam);
}


static void gui_3d_tria_face_calculate_normal(gui_3d_tria_face_t *face)
{
    gui_vector4D_t v1, v2;

    v1 = gui_point_4d_sub(face->transform_vertex[1].position, face->transform_vertex[0].position);
    v2 = gui_point_4d_sub(face->transform_vertex[2].position, face->transform_vertex[1].position);
    face->transform_vertex[0].normal = gui_point_4d_unit(gui_point_4d_cross(v1, v2));
    face->transform_vertex[1].normal = face->transform_vertex[0].normal;
    face->transform_vertex[2].normal = face->transform_vertex[0].normal;

}

static void gui_3d_tria_face_update_back_face(gui_3d_tria_face_t *face, GUI_3D_CULLMODE cullmode)
{
    if (cullmode == GUI_3D_CULLMODE_NONE)
    {
        face->state &= ~GUI_3D_FACESTATE_BACKFACE;
        return;
    }

    bool bBackFace = true;
    gui_vector4D_t v1 = face->transform_vertex[0].normal;
    float dot;

    for (int j = 0; j < 3; j++)
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


static void gui_3d_tria_face_transform_perspective(gui_3d_tria_face_t *face,
                                                   gui_3d_camera_t *camera)
{
    float z;
    z = 1 / face->transform_vertex[0].position.z;
    face->transform_vertex[0].position.x = camera->d * face->transform_vertex[0].position.x * z;
    face->transform_vertex[0].position.y = camera->d * face->transform_vertex[0].position.y * z;

    z = 1 / face->transform_vertex[1].position.z;
    face->transform_vertex[1].position.x = camera->d * face->transform_vertex[1].position.x * z;
    face->transform_vertex[1].position.y = camera->d * face->transform_vertex[1].position.y * z;

    z = 1 / face->transform_vertex[2].position.z;
    face->transform_vertex[2].position.x = camera->d * face->transform_vertex[2].position.x * z;
    face->transform_vertex[2].position.y = camera->d * face->transform_vertex[2].position.y * z;
}


static void gui_3d_tria_face_transform_screen(gui_3d_tria_face_t *face, gui_3d_camera_t *camera)
{
    float alpha = 0.5f * (camera->viewport_width - 1);
    float beta = 0.5f * (camera->viewport_height - 1);

    for (int i = 0; i < 3; i++)
    {
        // Transform to screen space
        face->transform_vertex[i].position.x = alpha + alpha * face->transform_vertex[i].position.x;
        face->transform_vertex[i].position.y = beta + beta * face->transform_vertex[i].position.y;
    }

}

void gui_3d_tria_scene(gui_3d_tria_face_t *face, size_t face_index, gui_obj_attrib_t *attrib,
                       gui_3d_world_t *world, gui_3d_camera_t *camera)
{
    gui_3d_tria_face_transform_local_to_global(face, face_index, attrib, world);
    gui_3d_tria_face_transform_camera(face, camera);
    gui_3d_tria_face_calculate_normal(face);
    gui_3d_tria_face_update_back_face(face, GUI_3D_CULLMODE_CCW);

    if (!(face->state & GUI_3D_FACESTATE_BACKFACE))
    {
        // gui_3d_tria_face_cull_region(face, camera);
        gui_3d_tria_face_transform_perspective(face, camera);
        // gui_3d_tria_face_cull_out_side(face, camera);
        gui_3d_tria_face_transform_screen(face, camera);
    }
}




