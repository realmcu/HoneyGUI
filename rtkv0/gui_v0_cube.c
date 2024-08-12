/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_cube.c
  * @brief cube widget
  * @details cube widget
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/19
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "guidef.h"
#include <gui_matrix.h>
#include <math.h>

#define RAD(d)        (d*3.1415926f/180.0f) //!< angle to rad

/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */

/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */

/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */
#define CUBE_JUDEG_FULL_RANK(m) if(gui_cube_full_rank(m->matrix))  //!< JUDGE IF THIS IS FULL RANK

/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */

/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */
static void gui_cube_scale(gui_vertex_t *vertex, float scale)
{
    // Scale cube vertex coordinates to proper size.
    vertex->x *= scale;
    vertex->y *= scale;
    vertex->z *= scale;
}

static void gui_cube_compute_rotate(float rx, float ry, float rz, struct gui_matrix *rotate)
{
    // Rotation angles rx, ry, rz (degree) for axis X, Y, Z
    // Compute 3D rotation matrix base on rotation angle rx, ry, rz about axis X, Y, Z.
    rotate->m[0][0] = cos(RAD(rz)) * cos(RAD(ry));
    rotate->m[0][1] = cos(RAD(rz)) * sin(RAD(ry)) * sin(RAD(rx)) - sin(RAD(rz)) * cos(RAD(rx));
    rotate->m[0][2] = cos(RAD(rz)) * sin(RAD(ry)) * cos(RAD(rx)) + sin(RAD(rz)) * sin(RAD(rx));
    rotate->m[1][0] = sin(RAD(rz)) * cos(RAD(ry));
    rotate->m[1][1] = sin(RAD(rz)) * sin(RAD(ry)) * sin(RAD(rx)) + cos(RAD(rz)) * cos(RAD(rx));
    rotate->m[1][2] = sin(RAD(rz)) * sin(RAD(ry)) * cos(RAD(rx)) - cos(RAD(rz)) * sin(RAD(rx));
    rotate->m[2][0] = -sin(RAD(ry));
    rotate->m[2][1] = cos(RAD(ry)) * sin(RAD(rx));
    rotate->m[2][2] = cos(RAD(ry)) * cos(RAD(rx));
}

static void gui_cube_transfrom_rotate(struct gui_matrix *rotate, gui_vertex_t *vertex,
                                      gui_vertex_t *rc,
                                      float tx,
                                      float ty)
{
    // Compute the new cube vertex coordinates transformed by the rotation matrix.
    rc->x = rotate->m[0][0] * vertex->x + rotate->m[0][1] * vertex->y + rotate->m[0][2] * vertex->z;
    rc->y = rotate->m[1][0] * vertex->x + rotate->m[1][1] * vertex->y + rotate->m[1][2] * vertex->z;
    rc->z = rotate->m[2][0] * vertex->x + rotate->m[2][1] * vertex->y + rotate->m[2][2] * vertex->z;

    // Translate the vertex in XY plane.
    rc->x += tx;
    rc->y += ty;
}

static void gui_cube_transfrom_normalZ(struct gui_matrix *rotate, gui_vertex_t *nVec, float *nZ)
{
    // Compute the new normal Z coordinate transformed by the rotation matrix.
    *nZ = rotate->m[2][0] * nVec->x + rotate->m[2][1] * nVec->y + rotate->m[2][2] * nVec->z;
}

// From 6.5.2 of OpenVG1.1 Spec: An affine transformation maps a point (x, y) into the point
// (x*sx + y*shx + tx, x*shy + y*sy + ty) using homogeneous matrix multiplication.
// To map a rectangle image (0,0),(w,0),(w,h),(0,h) to a parallelogram (x0,y0),(x1,y1),(x2,y2),(x3,y3).
// We get the following equations:
//
//     x0 = tx;
//     y0 = ty;
//     x1 = w*sx + tx;
//     y1 = w*shy + ty;
//     x3 = h*shx + tx;
//     y3 = h*sy + ty;
//
// So the homogeneous matrix sx, sy, shx, shy, tx, ty can be easily solved from above equations.
//
static void gui_cube_transfrom_blit(float              w,
                                    float              h,
                                    gui_vertex_t          *v0,
                                    gui_vertex_t          *v1,
                                    gui_vertex_t          *v2,
                                    gui_vertex_t          *v3,
                                    struct gui_matrix *matrix)
{
    float sx, sy, shx, shy, tx, ty;

    // Compute 3x3 image transform matrix to map a rectangle image (w,h) to
    // a parallelogram (x0,y0), (x1,y1), (x2,y2), (x3,y3) counterclock wise.
    sx = (v1->x - v0->x) / w;
    sy = (v3->y - v0->y) / h;
    shx = (v3->x - v0->x) / h;
    shy = (v1->y - v0->y) / w;
    tx = v0->x;
    ty = v0->y;

    // Set the Blit transformation matrix
    matrix->m[0][0] = sx;
    matrix->m[0][1] = shx;
    matrix->m[0][2] = tx;
    matrix->m[1][0] = shy;
    matrix->m[1][1] = sy;
    matrix->m[1][2] = ty;
    matrix->m[2][0] = 0.0;
    matrix->m[2][1] = 0.0;
    matrix->m[2][2] = 1.0;
}

static bool gui_cube_point_in_rect(draw_img_t *img, int16_t x, int16_t y)
{
    if (x < img->img_target_x)
    {
        return false;
    }
    if (x > img->img_target_x + img->img_target_x)
    {
        return false;
    }
    if (y < img->img_target_y)
    {
        return false;
    }
    if (y > img->img_target_y + img->img_target_h)
    {
        return false;
    }
    return true;
}

static bool gui_cube_full_rank(struct gui_matrix *m)
{
    for (int i = 0; i < 3; i++)
    {
        if (m->m[i][0] == 0 && m->m[i][1] == 0 && m->m[i][2] == 0)
        {
            return false;
        }

        if ((m->m[0][i] * 10000.0f < 1000.0f && m->m[0][i] * 10000.0f > -1000.0f)
            && (m->m[1][i] * 10000.0f < 1000.0f && m->m[1][i] * 10000.0f > -1000.0f)
            && (m->m[2][i] * 10000.0f < 1000.0f && m->m[2][i] * 10000.0f > -1000.0f))
        {
            return false;
        }
    }
    return true;
}

void gui_v0_cube(float cbsize, \
                 float c_x, \
                 float c_y, \
                 float xrot, \
                 float yrot, \
                 float zrot, \
                 uint16_t img_w, uint16_t img_h, \
                 gui_matrix_t *m0, \
                 gui_matrix_t *m1, \
                 gui_matrix_t *m2, \
                 gui_matrix_t *m3, \
                 gui_matrix_t *m4, \
                 gui_matrix_t *m5)
{
    struct gui_matrix rotate_3D;
    struct gui_matrix matrix;
    gui_vertex_t rv0, rv1, rv2, rv3, rv4, rv5, rv6, rv7;
    gui_vertex_t cube_v0 = {-1.0, -1.0, -1.0};
    gui_vertex_t cube_v1 = {1.0, -1.0, -1.0};
    gui_vertex_t cube_v2 = {1.0, 1.0, -1.0};
    gui_vertex_t cube_v3 = {-1.0, 1.0, -1.0};
    gui_vertex_t cube_v4 = {-1.0, -1.0, 1.0};
    gui_vertex_t cube_v5 = {1.0, -1.0, 1.0};
    gui_vertex_t cube_v6 = {1.0, 1.0, 1.0};
    gui_vertex_t cube_v7 = {-1.0, 1.0, 1.0};
    gui_vertex_t normal0321 = {0.0, 0.0, -1.0};
    gui_vertex_t normal4567 = {0.0, 0.0, 1.0};
    gui_vertex_t normal1265 = {1.0, 0.0, 0.0};
    gui_vertex_t normal0473 = {-1.0, 0.0, 0.0};
    gui_vertex_t normal2376 = {0.0, 1.0, 0.0};
    gui_vertex_t normal0154 = {0.0, -1.0, 0.0};

    gui_dispdev_t *dc = gui_get_dc();

    // Translate the cube to the center of framebuffer.
    float xoff = (c_x - dc->screen_width / 2) + dc->screen_width / 2;
    float yoff = (c_y - dc->screen_height / 2) + dc->screen_height / 2;


    gui_cube_scale(&cube_v0, cbsize);
    gui_cube_scale(&cube_v1, cbsize);
    gui_cube_scale(&cube_v2, cbsize);
    gui_cube_scale(&cube_v3, cbsize);
    gui_cube_scale(&cube_v4, cbsize);
    gui_cube_scale(&cube_v5, cbsize);
    gui_cube_scale(&cube_v6, cbsize);
    gui_cube_scale(&cube_v7, cbsize);

    // Rotation angles (degree) for axis X, Y, Z
    gui_cube_compute_rotate(xrot, yrot, zrot, &rotate_3D);


    // Compute the new cube vertex coordinates transformed by the rotation matrix.
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v0, &rv0, xoff, yoff);
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v1, &rv1, xoff, yoff);
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v2, &rv2, xoff, yoff);
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v3, &rv3, xoff, yoff);
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v4, &rv4, xoff, yoff);
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v5, &rv5, xoff, yoff);
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v6, &rv6, xoff, yoff);
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v7, &rv7, xoff, yoff);

    float nz0321;
    float nz4567;
    float nz5126;
    float nz0473;
    float nz7623;
    float nz0154;
    // Compute the surface normal direction to determine the front/back face.
    gui_cube_transfrom_normalZ(&rotate_3D, &normal0321, &nz0321);
    gui_cube_transfrom_normalZ(&rotate_3D, &normal4567, &nz4567);
    gui_cube_transfrom_normalZ(&rotate_3D, &normal1265, &nz5126);
    gui_cube_transfrom_normalZ(&rotate_3D, &normal0473, &nz0473);
    gui_cube_transfrom_normalZ(&rotate_3D, &normal2376, &nz7623);
    gui_cube_transfrom_normalZ(&rotate_3D, &normal0154, &nz0154);


    if (nz0321 > 0.0f)
    {
        gui_cube_transfrom_blit(img_w, img_h, &rv0, &rv3, &rv2, &rv1, m0);

    }

    if (nz4567 > 0.0f)
    {
        gui_cube_transfrom_blit(img_w, img_h, &rv4, &rv5, &rv6, &rv7, m1);
    }

    if (nz5126 > 0.0f)
    {
        gui_cube_transfrom_blit(img_w, img_h, &rv5, &rv1, &rv2, &rv6, m2);

    }

    if (nz0473 > 0.0f)
    {
        gui_cube_transfrom_blit(img_w, img_h, &rv0, &rv4, &rv7, &rv3, m3);

    }

    if (nz7623 > 0.0f)
    {
        gui_cube_transfrom_blit(img_w, img_h, &rv7, &rv6, &rv2, &rv3, m4);

    }

    if (nz0154 > 0.0f)
    {
        gui_cube_transfrom_blit(img_w, img_h, &rv0, &rv1, &rv5, &rv4, m5);

    }
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
