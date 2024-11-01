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

gui_point4D_t gui_point_4d(float x, float y, float z)
{
    gui_point4D_t p;
    p.x = x;
    p.y = y;
    p.z = z;
    p.w = 1;
    return p;
}

void gui_3d_matrix_identity(gui_3d_matrix_t *m)
{
    m->_11 = 1.0f;    m->_12 = 0.0f;    m->_13 = 0.0f;    m->_14 = 0.0f;
    m->_21 = 0.0f;    m->_22 = 1.0f;    m->_23 = 0.0f;    m->_24 = 0.0f;
    m->_31 = 0.0f;    m->_32 = 0.0f;    m->_33 = 1.0f;    m->_34 = 0.0f;
    m->_41 = 0.0f;    m->_42 = 0.0f;    m->_43 = 0.0f;    m->_44 = 1.0f;
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

bool gui_3d_camera_UVN_initialize(gui_3d_camera_t *camera, gui_point4D_t cameraPosition,
                                  gui_point4D_t cameraTarget, float near, float far, float fov, float viewPortWidth,
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

void gui_3d_face_transform_local_to_global(gui_3d_face_t *face, gui_3d_world_t *world)
{

}

void gui_3d_camera_build_UVN_matrix(gui_3d_camera_t *camera)
{

}

void gui_3d_face_transform_camera(gui_3d_face_t *face, gui_3d_camera_t *camera)
{

}

void gui_3d_face_cull_region(gui_3d_face_t *face, gui_3d_camera_t *camera)
{

}

void gui_3d_face_calculate_normal(gui_3d_face_t *face)
{

}

void gui_3d_face_update_back_face(gui_3d_face_t *face, GUI_3D_CULLMODE mode)
{

}

void gui_3d_face_transform_perspective(gui_3d_face_t *face, gui_3d_camera_t *camera)
{

}

void gui_3d_face_cull_out_side(gui_3d_face_t *face, gui_3d_camera_t *camera)
{

}

void gui_3d_face_transform_screen(gui_3d_face_t *face, gui_3d_camera_t *camera)
{

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


void gui_3d_test(void)
{
    gui_3d_world_t world;
    gui_3d_camera_t camera;
    gui_3d_face_t face;

    gui_3d_camera_UVN_initialize(&camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 1), 1, 32767, 90,
                                 480, 480);

    gui_3d_world_inititalize(&world, 0, 0, 0, 0, 0, 0, 1);

    gui_3d_scene(&face, &world, &camera);
    printf("function  gui_3d_test\n");
}

