/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_img.c
  * @brief image widget
  * @details image widget is used to show image on the screen
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

#include <stdio.h>
#include <math.h>
#include "def_3d.h"

#if 1
// Set the intial cube rotation degree and step.
static const int16_t sin_table[] =
{
    0,     572,   1144,  1715,  2286,  2856,  3425,  3993,  4560,  5126,  5690,  6252,  6813,  7371,  7927,  8481,
    9032,  9580,  10126, 10668, 11207, 11743, 12275, 12803, 13328, 13848, 14364, 14876, 15383, 15886, 16383, 16876,
    17364, 17846, 18323, 18794, 19260, 19720, 20173, 20621, 21062, 21497, 21925, 22347, 22762, 23170, 23571, 23964,
    24351, 24730, 25101, 25465, 25821, 26169, 26509, 26841, 27165, 27481, 27788, 28087, 28377, 28659, 28932, 29196,
    29451, 29697, 29934, 30162, 30381, 30591, 30791, 30982, 31163, 31335, 31498, 31650, 31794, 31927, 32051, 32165,
    32269, 32364, 32448, 32523, 32587, 32642, 32687, 32722, 32747, 32762, 32767
};

static float gui_3d_sin(int angle)
{
    int16_t ret = 0;
    angle       = angle % 360;

    if (angle < 0) { angle = 360 + angle; }

    if (angle < 90)
    {
        ret = sin_table[angle];
    }
    else if (angle >= 90 && angle < 180)
    {
        angle = 180 - angle;
        ret   = sin_table[angle];
    }
    else if (angle >= 180 && angle < 270)
    {
        angle = angle - 180;
        ret   = -sin_table[angle];
    }
    else     /*angle >=270*/
    {
        angle = 360 - angle;
        ret   = -sin_table[angle];
    }

    return ret / 32767.0f;
}

static float gui_3d_cos(int angle)
{
    return gui_3d_sin(angle + 90);
}
#endif

gui_point_4d_t gui_point_4d(float x, float y, float z)
{
    gui_point_4d_t p;
    p.x = x;
    p.y = y;
    p.z = z;
    p.w = 1;
    return p;
}

float gui_point4D_dot(gui_point_4d_t p1, gui_point_4d_t p2)
{
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

gui_point_4d_t gui_point_4d_unit(gui_point_4d_t p)
{
    gui_point_4d_t pt;
    if (p.x || p.y || p.z)
    {
        float sqare = sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);
        pt.x = p.x / sqare;
        pt.y = p.y / sqare;
        pt.z = p.z / sqare;
        pt.w = 1;
        return pt;
    }
    return p;
}

gui_point_4d_t gui_point_4d_cross(gui_point_4d_t p1, gui_point_4d_t p2)
{
    gui_point_4d_t pt;
    pt.x = p1.y * p2.z - p2.y * p1.z;
    pt.y = p1.z * p2.x - p2.z * p1.x;
    pt.z = p1.x * p2.y - p2.x * p1.y;
    pt.w = 1;
    return pt;
}

gui_point_4d_t gui_point_4d_sub(gui_point_4d_t p1, gui_point_4d_t p2)
{
    gui_point_4d_t v;
    v.x = p1.x - p2.x;
    v.y = p1.y - p2.y;
    v.z = p1.z - p2.z;
    v.w = 1;
    return v;
}

void gui_3d_matrix_identity(gui_3d_matrix_t *m)
{
    m->_11 = 1.0f;    m->_12 = 0.0f;    m->_13 = 0.0f;    m->_14 = 0.0f;
    m->_21 = 0.0f;    m->_22 = 1.0f;    m->_23 = 0.0f;    m->_24 = 0.0f;
    m->_31 = 0.0f;    m->_32 = 0.0f;    m->_33 = 1.0f;    m->_34 = 0.0f;
    m->_41 = 0.0f;    m->_42 = 0.0f;    m->_43 = 0.0f;    m->_44 = 1.0f;
}

void gui_3d_matrix_zero(gui_3d_matrix_t *Mat)
{
    Mat->m[0][0] = 0; Mat->m[0][1] = 0; Mat->m[0][2] = 0; Mat->m[0][3] = 0;
    Mat->m[1][0] = 0; Mat->m[1][1] = 0; Mat->m[1][2] = 0; Mat->m[1][3] = 0;
    Mat->m[2][0] = 0; Mat->m[2][1] = 0; Mat->m[2][2] = 0; Mat->m[2][3] = 0;
    Mat->m[3][0] = 0; Mat->m[3][1] = 0; Mat->m[3][2] = 0; Mat->m[3][3] = 0;
}

void gui_3d_matrix_rotateX(gui_3d_matrix_t *m, float rotX)
{
    m->_11 = 1.0f;    m->_12 = 0;                               m->_13 = 0.0f;
    m->_14 = 0.0f;
    m->_21 = 0.0f;    m->_22 = gui_3d_cos(rotX);                m->_23 = gui_3d_sin(rotX);
    m->_24 = 0.0f;
    m->_31 = 0.0f;    m->_32 = -gui_3d_sin(rotX);               m->_33 = gui_3d_cos(rotX);
    m->_34 = 0.0f;
    m->_41 = 0.0f;    m->_42 = 0.0f;                            m->_43 = 0.0f;
    m->_44 = 1.0f;
}

void gui_3d_matrix_rotateY(gui_3d_matrix_t *m, float rotY)
{
    gui_3d_matrix_t *mat = m;
    float Angle = rotY;
    mat->_11 = gui_3d_cos(Angle);     mat->_12 = 0.0f;
    mat->_13 = gui_3d_sin(Angle);         mat->_14 = 0.0f;
    mat->_21 = 0.0f;                      mat->_22 = 1.0f;                          mat->_23 = 0.0f;
    mat->_24 = 0.0f;
    mat->_31 = -gui_3d_sin(Angle);        mat->_32 = 0.0f;
    mat->_33 = gui_3d_cos(Angle);         mat->_34 = 0.0f;
    mat->_41 = 0.0f;                      mat->_42 = 0.0f;                          mat->_43 = 0.0f;
    mat->_44 = 1.0f;
}

void gui_3d_matrix_rotateZ(gui_3d_matrix_t *m, float rotZ)
{
    gui_3d_matrix_t *mat = m;
    float Angle = rotZ;
    mat->_11 = gui_3d_cos(Angle);     mat->_12 = gui_3d_sin(Angle);         mat->_13 = 0.0f;
    mat->_14 = 0.0f;
    mat->_21 = -gui_3d_sin(Angle);        mat->_22 = gui_3d_cos(Angle);         mat->_23 = 0.0f;
    mat->_24 = 0.0f;
    mat->_31 = 0.0f;                      mat->_32 = 0.0f;                          mat->_33 = 1.0f;
    mat->_34 = 0.0f;
    mat->_41 = 0.0f;                      mat->_42 = 0.0f;                          mat->_43 = 0.0f;
    mat->_44 = 1.0f;
}

void gui_3d_matrix_scale(gui_3d_matrix_t *m, float scale_x, float scale_y, float scale_z)
{
    gui_3d_matrix_t *mat = m;
    mat->_11 = scale_x;   mat->_12 = 0.0f;      mat->_13 = 0.0f;      mat->_14 = 0.0f;
    mat->_21 = 0.0f;      mat->_22 = scale_y;   mat->_23 = 0.0f;      mat->_24 = 0.0f;
    mat->_31 = 0.0f;      mat->_32 = 0.0f;      mat->_33 = scale_z;   mat->_34 = 0.0f;
    mat->_41 = 0.0f;      mat->_42 = 0.0f;      mat->_43 = 0.0f;      mat->_44 = 1.0f;
}

void gui_3d_matrix_translate(gui_3d_matrix_t *m, float t_x, float t_y, float t_z)
{
    gui_3d_matrix_t *mat = m;
    mat->_11 = 1.0f;  mat->_12 = 0.0f;  mat->_13 = 0.0f;  mat->_14 = 0.0f;
    mat->_21 = 0.0f;  mat->_22 = 1.0f;  mat->_23 = 0.0f;  mat->_24 = 0.0f;
    mat->_31 = 0.0f;  mat->_32 = 0.0f;  mat->_33 = 1.0f;  mat->_34 = 0.0f;
    mat->_41 = t_x;   mat->_42 = t_y;   mat->_43 = t_z;   mat->_44 = 1.0f;
}

gui_3d_matrix_t gui_3d_matrix_multiply(gui_3d_matrix_t Mat1, gui_3d_matrix_t Mat2)
{
    gui_3d_matrix_t ptmat;
    ptmat.m[0][0] = Mat1.m[0][0] * Mat2.m[0][0]/**/ + Mat1.m[0][1] * Mat2.m[1][0]/**/ + Mat1.m[0][2] *
                    Mat2.m[2][0]/**/ + Mat1.m[0][3] * Mat2.m[3][0];
    ptmat.m[0][1] = Mat1.m[0][0] * Mat2.m[0][1]/**/ + Mat1.m[0][1] * Mat2.m[1][1]/**/ + Mat1.m[0][2] *
                    Mat2.m[2][1]/**/ + Mat1.m[0][3] * Mat2.m[3][1];
    ptmat.m[0][2] = Mat1.m[0][0] * Mat2.m[0][2]/**/ + Mat1.m[0][1] * Mat2.m[1][2]/**/ + Mat1.m[0][2] *
                    Mat2.m[2][2]/**/ + Mat1.m[0][3] * Mat2.m[3][2];
    ptmat.m[0][3] = Mat1.m[0][0] * Mat2.m[0][3]/**/ + Mat1.m[0][1] * Mat2.m[1][3]/**/ + Mat1.m[0][2] *
                    Mat2.m[2][3]/**/ + Mat1.m[0][3] * Mat2.m[3][3];

    ptmat.m[1][0] = Mat1.m[1][0] * Mat2.m[0][0]/**/ + Mat1.m[1][1] * Mat2.m[1][0]/**/ + Mat1.m[1][2] *
                    Mat2.m[2][0]/**/ + Mat1.m[1][3] * Mat2.m[3][0];
    ptmat.m[1][1] = Mat1.m[1][0] * Mat2.m[0][1]/**/ + Mat1.m[1][1] * Mat2.m[1][1]/**/ + Mat1.m[1][2] *
                    Mat2.m[2][1]/**/ + Mat1.m[1][3] * Mat2.m[3][1];
    ptmat.m[1][2] = Mat1.m[1][0] * Mat2.m[0][2]/**/ + Mat1.m[1][1] * Mat2.m[1][2]/**/ + Mat1.m[1][2] *
                    Mat2.m[2][2]/**/ + Mat1.m[1][3] * Mat2.m[3][2];
    ptmat.m[1][3] = Mat1.m[1][0] * Mat2.m[0][3]/**/ + Mat1.m[1][1] * Mat2.m[1][3]/**/ + Mat1.m[1][2] *
                    Mat2.m[2][3]/**/ + Mat1.m[1][3] * Mat2.m[3][3];

    ptmat.m[2][0] = Mat1.m[2][0] * Mat2.m[0][0]/**/ + Mat1.m[2][1] * Mat2.m[1][0]/**/ + Mat1.m[2][2] *
                    Mat2.m[2][0]/**/ + Mat1.m[2][3] * Mat2.m[3][0];
    ptmat.m[2][1] = Mat1.m[2][0] * Mat2.m[0][1]/**/ + Mat1.m[2][1] * Mat2.m[1][1]/**/ + Mat1.m[2][2] *
                    Mat2.m[2][1]/**/ + Mat1.m[2][3] * Mat2.m[3][1];
    ptmat.m[2][2] = Mat1.m[2][0] * Mat2.m[0][2]/**/ + Mat1.m[2][1] * Mat2.m[1][2]/**/ + Mat1.m[2][2] *
                    Mat2.m[2][2]/**/ + Mat1.m[2][3] * Mat2.m[3][2];
    ptmat.m[2][3] = Mat1.m[2][0] * Mat2.m[0][3]/**/ + Mat1.m[2][1] * Mat2.m[1][3]/**/ + Mat1.m[2][2] *
                    Mat2.m[2][3]/**/ + Mat1.m[2][3] * Mat2.m[3][3];

    ptmat.m[3][0] = Mat1.m[3][0] * Mat2.m[0][0]/**/ + Mat1.m[3][1] * Mat2.m[1][0]/**/ + Mat1.m[3][2] *
                    Mat2.m[2][0]/**/ + Mat1.m[3][3] * Mat2.m[3][0];
    ptmat.m[3][1] = Mat1.m[3][0] * Mat2.m[0][1]/**/ + Mat1.m[3][1] * Mat2.m[1][1]/**/ + Mat1.m[3][2] *
                    Mat2.m[2][1]/**/ + Mat1.m[3][3] * Mat2.m[3][1];
    ptmat.m[3][2] = Mat1.m[3][0] * Mat2.m[0][2]/**/ + Mat1.m[3][1] * Mat2.m[1][2]/**/ + Mat1.m[3][2] *
                    Mat2.m[2][2]/**/ + Mat1.m[3][3] * Mat2.m[3][2];
    ptmat.m[3][3] = Mat1.m[3][0] * Mat2.m[0][3]/**/ + Mat1.m[3][1] * Mat2.m[1][3]/**/ + Mat1.m[3][2] *
                    Mat2.m[2][3]/**/ + Mat1.m[3][3] * Mat2.m[3][3];
    return ptmat;
}

gui_point_4d_t gui_3d_point4D_mul_matrix(gui_point_4d_t p, gui_3d_matrix_t mat)
{
    gui_point_4d_t point;
    point.x = p.x * mat._11 + p.y * mat._21 + p.z * mat._31 + 1 * mat._41;
    point.y = p.x * mat._12 + p.y * mat._22 + p.z * mat._32 + 1 * mat._42;
    point.z = p.x * mat._13 + p.y * mat._23 + p.z * mat._33 + 1 * mat._43;
    point.w = p.x * mat._14 + p.y * mat._24 + p.z * mat._34 + 1 * mat._44;
    return point;
}


void gui_3d_world_inititalize(gui_3d_matrix_t *world, float x, float y, float z, float rotX,
                              float rotY, float rotZ, float scale)
{
    gui_3d_matrix_t mrotx, mroty, mrotz, mscale, mtrans;
    gui_3d_matrix_identity(world);
    gui_3d_matrix_rotateX(&mrotx, rotX);
    gui_3d_matrix_rotateY(&mroty, rotY);
    gui_3d_matrix_rotateZ(&mrotz, rotZ);
    gui_3d_matrix_scale(&mscale, scale, scale, scale);
    gui_3d_matrix_translate(&mtrans, x, y, z);

    *world = gui_3d_matrix_multiply(*world, mscale);
    *world = gui_3d_matrix_multiply(*world, mrotx);
    *world = gui_3d_matrix_multiply(*world, mroty);
    *world = gui_3d_matrix_multiply(*world, mrotz);
    *world = gui_3d_matrix_multiply(*world, mtrans);
}

bool gui_3d_camera_UVN_initialize(gui_3d_camera_t *camera, gui_point_4d_t cameraPosition,
                                  gui_point_4d_t cameraTarget, float near, float far, float fov, float viewPortWidth,
                                  float viewPortHeight)
{
    camera->position = cameraPosition;
    camera->targetDirection = cameraTarget;
    camera->near_z = near;
    camera->far_z = far;
    camera->viewport_width = viewPortWidth;
    camera->viewport_height = viewPortHeight;

    camera->viewport_center_x = (viewPortWidth - 1) / 2;
    camera->viewport_center_y = (viewPortHeight - 1) / 2;

    camera->aspect_ratio = viewPortWidth / (float)viewPortHeight;

    gui_3d_matrix_identity(&camera->mat_cam);
    gui_3d_matrix_identity(&camera->mat_per);
    gui_3d_matrix_identity(&camera->mat_scr);

    camera->fov = fov;
    camera->viewplane_width = 2.0f;
    camera->viewplane_height = 2.0f / camera->aspect_ratio;

    if (fov == 90)
    {
        camera->d = 1;
    }
    else
    {
        camera->d = (0.5f) * (camera->viewplane_width) * tanf(fov);
    }
    //camera->zbuffer=(float *)malloc(sizeof(float)*camera->viewport_height*camera->viewport_width);
    //return camera->zbuffer!=NULL;
    return true;
}

void gui_3d_face_transform(gui_3d_face_t *face, gui_3d_matrix_t mat, GUI_3D_FACE_TRANSFORM mode)
{
    switch (mode)
    {
    case GUI_3D_FACE_TRANSFORM_LOCAL_TO_LOCAL:
        {
            // int i;
            // gui_3d_face_t *pface=NULL;
            // for (i=0;i<list->facestream.size;i++)
            {
                face->vertex[0].position = gui_3d_point4D_mul_matrix(face->vertex[0].position, mat);
                face->vertex[1].position = gui_3d_point4D_mul_matrix(face->vertex[1].position, mat);
                face->vertex[2].position = gui_3d_point4D_mul_matrix(face->vertex[2].position, mat);
            }
            break;
        }
    case GUI_3D_FACE_TRANSFORM_LOACL_TO_GLOBAL:
        {
            // int i;
            // gui_3d_face_t *face=NULL;
            // for (i=0;i<list->facestream.size;i++)
            {
                face->transform_vertex[0] = face->vertex[0];
                face->transform_vertex[1] = face->vertex[1];
                face->transform_vertex[2] = face->vertex[2];
                face->transform_vertex[0].position = gui_3d_point4D_mul_matrix(face->vertex[0].position, mat);
                face->transform_vertex[1].position = gui_3d_point4D_mul_matrix(face->vertex[1].position, mat);
                face->transform_vertex[2].position = gui_3d_point4D_mul_matrix(face->vertex[2].position, mat);
            }
            break;
        }
    case GUI_3D_FACE_TRANSFORM_GLOBAL_TO_GLOBAL:
        {
            // int i;
            // gui_3d_face_t *face=NULL;
            // for (i=0;i<list->facestream.size;i++)
            {
                face->transform_vertex[0].position = gui_3d_point4D_mul_matrix(face->transform_vertex[0].position,
                                                                               mat);
                face->transform_vertex[1].position = gui_3d_point4D_mul_matrix(face->transform_vertex[1].position,
                                                                               mat);
                face->transform_vertex[2].position = gui_3d_point4D_mul_matrix(face->transform_vertex[2].position,
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

void gui_3d_face_transform_local_to_global(gui_3d_face_t *face, gui_3d_world_t *world)
{
    gui_3d_face_transform(face, *world, GUI_3D_FACE_TRANSFORM_LOACL_TO_GLOBAL);
}

void gui_3d_camera_build_UVN_matrix(gui_3d_camera_t *camera)
{
    gui_3d_matrix_t mt_inv;
    gui_3d_matrix_t mt_uvn;
    gui_point_4d_t u, v, n;

    gui_3d_matrix_identity(&mt_inv);
    mt_inv._41 = -camera->position.x;
    mt_inv._42 = -camera->position.y;
    mt_inv._43 = -camera->position.z;

    n.x = camera->targetDirection.x - camera->position.x;
    n.y = camera->targetDirection.y - camera->position.y;
    n.z = camera->targetDirection.z - camera->position.z;
    n.w = 1;

    n = gui_point_4d_unit(n);
    v = gui_point_4d(0, 1, 0);
    u = gui_point_4d_cross(v, n);
    v = gui_point_4d_cross(n, u);
    gui_3d_matrix_zero(&mt_uvn);
    mt_uvn._11 = u.x;
    mt_uvn._12 = v.x;
    mt_uvn._13 = n.x;

    mt_uvn._21 = u.y;
    mt_uvn._22 = v.y;
    mt_uvn._23 = n.y;

    mt_uvn._31 = u.z;
    mt_uvn._32 = v.z;
    mt_uvn._33 = n.z;

    mt_uvn._44 = 1;
    camera->mat_cam = gui_3d_matrix_multiply(mt_inv, mt_uvn);
}

void gui_3d_face_transform_camera(gui_3d_face_t *face, gui_3d_camera_t *camera)
{
    gui_3d_face_transform(face, camera->mat_cam, GUI_3D_FACE_TRANSFORM_GLOBAL_TO_GLOBAL);
}

void gui_3d_face_cull_region(gui_3d_face_t *face, gui_3d_camera_t *camera)
{
    bool outside;
    int j;
    // int i;
    // gui_3d_face_t *face=NULL;
    // for (i=0;i<list->facestream.size;i++)
    {
        outside = false;
        for (j = 0; j < 3; j++)
        {
            //behind
            if (face->transform_vertex[j].position.z <= camera->near_z)
            {
                outside = true;
                break;
            }
        }
        if (face->transform_vertex[0].position.z > camera->far_z && \
            face->transform_vertex[1].position.z > camera->far_z && \
            face->transform_vertex[2].position.z > camera->far_z)
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
}

void gui_3d_face_calculate_normal(gui_3d_face_t *face)
{
    gui_vector4D_t v1, v2;
    // int i;
    // gui_3d_face_t *face=NULL;
    // for (i=0;i<list->facestream.size;i++)
    {
        v1 = gui_point_4d_sub(face->transform_vertex[1].position, face->transform_vertex[0].position);
        v2 = gui_point_4d_sub(face->transform_vertex[2].position, face->transform_vertex[1].position);
        face->transform_vertex[0].normal = gui_point_4d_unit(gui_point_4d_cross(v1, v2));
        face->transform_vertex[1].normal = face->transform_vertex[0].normal;
        face->transform_vertex[2].normal = face->transform_vertex[0].normal;
    }

}

void gui_3d_face_update_back_face(gui_3d_face_t *face, GUI_3D_CULLMODE cullmode)
{
    int j;
    float dot;
    bool bBackFace;
    gui_vector4D_t v1, v2;
    // int i;
    // gui_3d_face_t *face=NULL;
    // for (i=0;i<list->facestream.size;i++)
    {
        bBackFace = true;
        v1 = face->transform_vertex[0].normal;
        for (j = 0; j < 3; j++)
        {
            v2 = face->transform_vertex[j].position;
            dot = gui_point4D_dot(v1, v2);
            if (cullmode == GUI_3D_CULLMODE_NONE)
            {
                bBackFace = false;
            }
            else
            {
                if (dot > 0)
                {
                    if (cullmode != GUI_3D_CULLMODE_CCW)
                    {
                        bBackFace = false;
                        break;
                    }
                }
                else
                {
                    if (cullmode != GUI_3D_CULLMODE_CW)
                    {
                        bBackFace = false;
                        break;
                    }
                }
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
}

void gui_3d_face_transform_perspective(gui_3d_face_t *face, gui_3d_camera_t *camera)
{
    // int i;
    // gui_3d_face_t *face=NULL;
    // for (i=0;i<list->facestream.size;i++)
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
    }
}

void gui_3d_face_cull_out_side(gui_3d_face_t *face, gui_3d_camera_t *camera)
{

    bool outside;
    // int i;
    // gui_3d_face_t *face=NULL;
    // for (i=0;i<list->facestream.size;i++)
    {
        outside = false;

        if (face->transform_vertex[0].position.y > camera->viewplane_height / 2 && \
            face->transform_vertex[1].position.y > camera->viewplane_height / 2 && \
            face->transform_vertex[2].position.y > camera->viewplane_height / 2)
        {
            //up
            outside = true;
        }
        else if (face->transform_vertex[0].position.y < -camera->viewplane_height / 2 && \
                 face->transform_vertex[1].position.y < -camera->viewplane_height / 2 && \
                 face->transform_vertex[2].position.y < -camera->viewplane_height / 2)
        {
            //down
            outside = true;
        }
        else if (face->transform_vertex[0].position.x < -camera->viewplane_width / 2 && \
                 face->transform_vertex[1].position.x < -camera->viewplane_width / 2 && \
                 face->transform_vertex[2].position.x < -camera->viewplane_width / 2)
        {
            //left
            outside = true;
        }
        else if (face->transform_vertex[0].position.x > camera->viewplane_width / 2 && \
                 face->transform_vertex[1].position.x > camera->viewplane_width / 2 && \
                 face->transform_vertex[2].position.x > camera->viewplane_width / 2)
        {
            //right
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
}

void gui_3d_face_transform_screen(gui_3d_face_t *face, gui_3d_camera_t *camera)
{

    float alpha = (0.5f * camera->viewport_width - 0.5f);
    float beta = (0.5f * camera->viewport_height - 0.5f);
    // int i;
    // gui_3d_face_t *face=NULL;
    // for (i=0;i<list->facestream.size;i++)
    {
        // face=GUI_VECTOR_AT(gui_3d_face_t,&list->facestream,i);
        face->transform_vertex[0].position.x = alpha + alpha * face->transform_vertex[0].position.x;
        face->transform_vertex[0].position.y = beta + beta * face->transform_vertex[0].position.y;

        face->transform_vertex[1].position.x = alpha + alpha * face->transform_vertex[1].position.x;
        face->transform_vertex[1].position.y = beta + beta * face->transform_vertex[1].position.y;

        face->transform_vertex[2].position.x = alpha + alpha * face->transform_vertex[2].position.x;
        face->transform_vertex[2].position.y = beta + beta * face->transform_vertex[2].position.y;

        //inv y
        face->transform_vertex[0].position.y = -face->transform_vertex[0].position.y +
                                               camera->viewport_height;
        face->transform_vertex[1].position.y = -face->transform_vertex[1].position.y +
                                               camera->viewport_height;
        face->transform_vertex[2].position.y = -face->transform_vertex[2].position.y +
                                               camera->viewport_height;
    }

}

void gui_3d_scene(gui_3d_face_t *face, gui_3d_world_t *world, gui_3d_camera_t *camera)
{
    gui_3d_face_transform_local_to_global(face, world);
    gui_3d_camera_build_UVN_matrix(camera);
    gui_3d_face_transform_camera(face, camera);
    gui_3d_face_cull_region(face, camera);
    gui_3d_face_calculate_normal(face);
    gui_3d_face_update_back_face(face, GUI_3D_CULLMODE_NONE);
    gui_3d_face_transform_perspective(face, camera);
    gui_3d_face_cull_out_side(face, camera);
    gui_3d_face_transform_screen(face, camera);
}




