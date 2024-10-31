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

#include "math.h"
#include "def_3d.h"


void gui_3d_matrix_identity(gui_3d_matrix_t *m)
{
    m->_11 = 1.0f;    m->_12 = 0.0f;    m->_13 = 0.0f;    m->_14 = 0.0f;
    m->_21 = 0.0f;    m->_22 = 1.0f;    m->_23 = 0.0f;    m->_24 = 0.0f;
    m->_31 = 0.0f;    m->_32 = 0.0f;    m->_33 = 1.0f;    m->_34 = 0.0f;
    m->_41 = 0.0f;    m->_42 = 0.0f;    m->_43 = 0.0f;    m->_44 = 1.0f;
}

void gui_3d_matrix_rotateX(gui_3d_matrix_t *m, float rotX)
{

}

void gui_3d_matrix_rotateY(gui_3d_matrix_t *m, float rotY)
{

}

void gui_3d_matrix_rotateZ(gui_3d_matrix_t *m, float rotZ)
{

}

void gui_3d_matrix_scale(gui_3d_matrix_t *m, float scale_x, float scale_y, float scale_z)
{

}

void gui_3d_matrix_translate(gui_3d_matrix_t *m, float t_x, float t_y, float t_z)
{

}

gui_3d_matrix_t gui_3d_matrix_multiply(gui_3d_matrix_t m1, gui_3d_matrix_t m2)
{
    gui_3d_matrix_t m = {0};
    return m;
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

void gui_3d_face_transform_local_to_global(gui_3d_face_t *face, gui_3d_word_t *world)
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

void gui_3d_scene(gui_3d_face_t *face, gui_3d_word_t *world, gui_3d_camera_t *camera)
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


