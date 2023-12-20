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
#include <guidef.h>
#include <string.h>
#include <gui_obj.h>
#include <draw_img.h>
#include <gui_matrix.h>
#include "gui_cube.h"
#include "tp_algo.h"

#include <math.h>
#include "acc_engine.h"

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
#define CUBE_JUDEG_FULL_RANK(m) if(full_rank(m->matrix))  //!< JUDGE IF THIS IS FULL RANK

/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */
static struct rtgui_matrix matrix, rotate_3D;
static Vertex_t rv0, rv1, rv2, rv3, rv4, rv5, rv6, rv7;
static float nz0321, nz4567, nz5126, nz0473, nz7623, nz0154;

// Set the intial cube rotation degree and step.
static float xrot = 20.0;
static float yrot = 0.0;
static float zrot = 20.0;
static float rotstep = 5.5;

/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */
static void scale_cube(Vertex_t *vertex, float scale)
{
    // Scale cube vertex coordinates to proper size.
    //
    vertex->x *= scale;
    vertex->y *= scale;
    vertex->z *= scale;
}

static void compute_rotate(float rx, float ry, float rz, struct rtgui_matrix *rotate)
{
    // Rotation angles rx, ry, rz (degree) for axis X, Y, Z
    // Compute 3D rotation matrix base on rotation angle rx, ry, rz about axis X, Y, Z.
    //
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

static void transfrom_rotate(struct rtgui_matrix *rotate, Vertex_t *vertex, Vertex_t *rc, float tx,
                             float ty)
{
    // Compute the new cube vertex coordinates transformed by the rotation matrix.
    //
    rc->x = rotate->m[0][0] * vertex->x + rotate->m[0][1] * vertex->y + rotate->m[0][2] * vertex->z;
    rc->y = rotate->m[1][0] * vertex->x + rotate->m[1][1] * vertex->y + rotate->m[1][2] * vertex->z;
    rc->z = rotate->m[2][0] * vertex->x + rotate->m[2][1] * vertex->y + rotate->m[2][2] * vertex->z;

    // Translate the vertex in XY plane.
    //
    rc->x += tx;
    rc->y += ty;
}

static void transfrom_normalZ(struct rtgui_matrix *rotate, Normal_t *nVec, float *nZ)
{
    // Compute the new normal Z coordinate transformed by the rotation matrix.
    //
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
static void transfrom_blit(float w, float h, Vertex_t *v0, Vertex_t *v1, Vertex_t *v2, Vertex_t *v3,
                           struct rtgui_matrix *matrix)
{
    float sx, sy, shx, shy, tx, ty;

    // Compute 3x3 image transform matrix to map a rectangle image (w,h) to
    // a parallelogram (x0,y0), (x1,y1), (x2,y2), (x3,y3) counterclock wise.
    //
    sx = (v1->x - v0->x) / w;
    sy = (v3->y - v0->y) / h;
    shx = (v3->x - v0->x) / h;
    shy = (v1->y - v0->y) / w;
    tx = v0->x;
    ty = v0->y;

    // gui_log("sx = %f;v1->x = %f;v0->x = %f;w = %f  \n", sx, v1->x, v0->x, w);
    // gui_log("sy = %f;v3->y = %f;v0->y = %f;h = %f  \n", sy, v3->y, v0->y, h);
    // gui_log("shx = %f;v3->x = %f;v0->y = %f;h = %f  \n", shx, v3->x, v0->x, h);
    // gui_log("shy = %f;v1->y = %f;v0->y = %f;w = %f  \n", sy, v1->y, v0->y, w);
    // gui_log("tx = %f; \n", tx);
    // gui_log("ty = %f; \n", ty);
    //while(1);
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

// static void print_matrix(struct rtgui_matrix *matrix)
// {
// #if 0
//     gui_log("Blit Matrix:\n");
//     gui_log("    %f    %f    %f \n", matrix->m[0][0], matrix->m[0][1], matrix->m[0][2]);
//     gui_log("    %f    %f    %f \n", matrix->m[1][0], matrix->m[1][1], matrix->m[1][2]);
//     gui_log("    %f    %f    %f \n", matrix->m[2][0], matrix->m[2][1], matrix->m[2][2]);
// #endif
// }

//static void cleanup(void)
//{
//    return;
//}


static void get_new_area(draw_img_t *draw_img)
{

    struct rtgui_pox pox = {0.0f};
    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;

    pox.p[0] = 0.0f;
    pox.p[1] = 0.0f;
    pox.p[2] = 1.0f;
    pox_mul(draw_img->matrix, &pox);
    x_min = pox.p[0];
    x_max = pox.p[0];
    y_min = pox.p[1];
    y_max = pox.p[1];


    pox.p[0] = (float)draw_img->img_w;
    pox.p[1] = 0.0f;
    pox.p[2] = 1.0f;
    pox_mul(draw_img->matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }


    pox.p[0] = 0.0f;
    pox.p[1] = (float)draw_img->img_h;
    pox.p[2] = 1.0f;
    pox_mul(draw_img->matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    pox.p[0] = (float)draw_img->img_w;
    pox.p[1] = (float)draw_img->img_h;
    pox.p[2] = 1.0f;
    pox_mul(draw_img->matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    draw_img->img_x = (int16_t)x_min;
    draw_img->img_y = (int16_t)y_min;

    draw_img->target_w = (int16_t)x_max - (int16_t)x_min;
    draw_img->target_h = (int16_t)y_max - (int16_t)y_min;
}
static bool full_rank(struct rtgui_matrix *m)
{
    for (int i = 0; i < 3; i++)
    {
        //gui_log("%f, %f, %f, ", m->m[0][i], m->m[1][i], m->m[2][i]);
        //gui_log("%f,%f,%f,", m->m[i][0], m->m[i][1], m->m[i][2]);
        if (m->m[i][0] == 0 && m->m[i][1] == 0 && m->m[i][2] == 0)
        {
            //gui_log("\n");
            return false;
        }
        //
        if ((m->m[0][i] * 10000.0f < 1000.0f && m->m[0][i] * 10000.0f > -1000.0f) &&
            (m->m[1][i] * 10000.0f < 1000.0f && m->m[1][i] * 10000.0f > -1000.0f) &&
            (m->m[2][i] * 10000.0f < 1000.0f && m->m[2][i] * 10000.0f > -1000.0f))
        {
            //gui_log("\n");
            return false;
        }
    } //gui_log("\n");
    return true;
}

static void cube_prepare(gui_obj_t *obj)
{
    Vertex_t cube_v0 = {-1.0, -1.0, -1.0};
    Vertex_t cube_v1 = {1.0, -1.0, -1.0};
    Vertex_t cube_v2 = {1.0, 1.0, -1.0};
    Vertex_t cube_v3 = {-1.0, 1.0, -1.0};
    Vertex_t cube_v4 = {-1.0, -1.0, 1.0};
    Vertex_t cube_v5 = {1.0, -1.0, 1.0};
    Vertex_t cube_v6 = {1.0, 1.0, 1.0};
    Vertex_t cube_v7 = {-1.0, 1.0, 1.0};
    Normal_t normal0321 = {0.0, 0.0, -1.0};
    Normal_t normal4567 = {0.0, 0.0, 1.0};
    Normal_t normal1265 = {1.0, 0.0, 0.0};
    Normal_t normal0473 = {-1.0, 0.0, 0.0};
    Normal_t normal2376 = {0.0, 1.0, 0.0};
    Normal_t normal0154 = {0.0, -1.0, 0.0};

    gui_cube_t *this = (gui_cube_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = (touch_info_t *)tp_get_info();
    gui_obj_t *root = (gui_obj_t *)obj;
    // Scale the cube to proper size
    float cbsize = this->cbsize * root->sx;
    // Translate the cube to the center of framebuffer.
    float xoff = (this->c_x - dc->screen_width / 2) * root->sx + dc->screen_width / 2;
    float yoff = (this->c_y - dc->screen_height / 2) * root->sy + dc->screen_height / 2;

    xoff = xoff + obj->dx + obj->ax + obj->tx;
    yoff = yoff + obj->dy + obj->ay + obj->ty;

    scale_cube(&cube_v0, cbsize);
    scale_cube(&cube_v1, cbsize);
    scale_cube(&cube_v2, cbsize);
    scale_cube(&cube_v3, cbsize);
    scale_cube(&cube_v4, cbsize);
    scale_cube(&cube_v5, cbsize);
    scale_cube(&cube_v6, cbsize);
    scale_cube(&cube_v7, cbsize);


    // Rotation angles (degree) for axis X, Y, Z
    compute_rotate(xrot, yrot, zrot, &rotate_3D);
    // xrot += rotstep;
    yrot += rotstep;
    //zrot += rotstep;

    switch (tp->type)
    {
    case TOUCH_HOLD_X:
        this->release_x = tp->deltaX;
        yrot += this->release_x;
        break;
    case TOUCH_HOLD_Y:
        this->release_y = tp->deltaY;
        xrot += this->release_y;
        break;
    default:
        break;
    }

    // Compute the new cube vertex coordinates transformed by the rotation matrix.
    transfrom_rotate(&rotate_3D, &cube_v0, &rv0, xoff, yoff);
    transfrom_rotate(&rotate_3D, &cube_v1, &rv1, xoff, yoff);
    transfrom_rotate(&rotate_3D, &cube_v2, &rv2, xoff, yoff);
    transfrom_rotate(&rotate_3D, &cube_v3, &rv3, xoff, yoff);
    transfrom_rotate(&rotate_3D, &cube_v4, &rv4, xoff, yoff);
    transfrom_rotate(&rotate_3D, &cube_v5, &rv5, xoff, yoff);
    transfrom_rotate(&rotate_3D, &cube_v6, &rv6, xoff, yoff);
    transfrom_rotate(&rotate_3D, &cube_v7, &rv7, xoff, yoff);

    // Compute the surface normal direction to determine the front/back face.
    transfrom_normalZ(&rotate_3D, &normal0321, &nz0321);
    transfrom_normalZ(&rotate_3D, &normal4567, &nz4567);
    transfrom_normalZ(&rotate_3D, &normal1265, &nz5126);
    transfrom_normalZ(&rotate_3D, &normal0473, &nz0473);
    transfrom_normalZ(&rotate_3D, &normal2376, &nz7623);
    transfrom_normalZ(&rotate_3D, &normal0154, &nz0154);

    draw_img_t *front = &this->draw_img_front;
    draw_img_t *back = &this->draw_img_back;
    draw_img_t *up = &this->draw_img_up;
    draw_img_t *down = &this->draw_img_down;
    draw_img_t *left = &this->draw_img_left;
    draw_img_t *right = &this->draw_img_right;


    rtgui_image_load_scale(front);
    rtgui_image_load_scale(back);
    rtgui_image_load_scale(up);
    rtgui_image_load_scale(down);
    rtgui_image_load_scale(left);
    rtgui_image_load_scale(right);


    if (nz0321 > 0.0f)
    {
        transfrom_blit(front->img_w, front->img_h, &rv0, &rv3, &rv2, &rv1, &matrix);
        memcpy(front->matrix, &matrix, sizeof(struct rtgui_matrix));
        memcpy(front->inverse, &matrix, sizeof(struct rtgui_matrix));
        matrix_inverse(front->inverse);
        get_new_area(front);
    }

    if (nz4567 > 0.0f)
    {
        transfrom_blit(back->img_w, back->img_h, &rv4, &rv5, &rv6, &rv7, &matrix);
        memcpy(back->matrix, &matrix, sizeof(struct rtgui_matrix));
        memcpy(back->inverse, &matrix, sizeof(struct rtgui_matrix));
        matrix_inverse(back->inverse);
        get_new_area(back);
    }

    if (nz5126 > 0.0f)
    {
        transfrom_blit(up->img_w, up->img_h, &rv5, &rv1, &rv2, &rv6, &matrix);
        memcpy(up->matrix, &matrix, sizeof(struct rtgui_matrix));
        memcpy(up->inverse, &matrix, sizeof(struct rtgui_matrix));
        matrix_inverse(up->inverse);
        get_new_area(up);
    }

    if (nz0473 > 0.0f)
    {
        transfrom_blit(down->img_w, down->img_h, &rv0, &rv4, &rv7, &rv3, &matrix);
        memcpy(down->matrix, &matrix, sizeof(struct rtgui_matrix));
        memcpy(down->inverse, &matrix, sizeof(struct rtgui_matrix));
        matrix_inverse(down->inverse);
        get_new_area(down);
    }

    if (nz7623 > 0.0f)
    {
        transfrom_blit(left->img_w, left->img_h, &rv7, &rv6, &rv2, &rv3, &matrix);
        memcpy(left->matrix, &matrix, sizeof(struct rtgui_matrix));
        memcpy(left->inverse, &matrix, sizeof(struct rtgui_matrix));
        matrix_inverse(left->inverse);
        get_new_area(left);
    }

    if (nz0154 > 0.0f)
    {
        transfrom_blit(right->img_w, right->img_h, &rv0, &rv1, &rv5, &rv4, &matrix);
        memcpy(right->matrix, &matrix, sizeof(struct rtgui_matrix));
        memcpy(right->inverse, &matrix, sizeof(struct rtgui_matrix));
        matrix_inverse(right->inverse);
        get_new_area(right);
    }
}

static void cube_draw_cb(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_cube_t *this = (gui_cube_t *)obj;

    draw_img_t *front = &this->draw_img_front;
    draw_img_t *back = &this->draw_img_back;
    draw_img_t *up = &this->draw_img_up;
    draw_img_t *down = &this->draw_img_down;
    draw_img_t *left = &this->draw_img_left;
    draw_img_t *right = &this->draw_img_right;

    rtgui_rect_t draw_rect = {0};

    if (nz0321 > 0.0f)
    {
        draw_rect.x1 = this->draw_img_front.img_x;
        draw_rect.y1 = this->draw_img_front.img_y;
        draw_rect.x2 = draw_rect.x1 + obj->w;
        draw_rect.y2 = draw_rect.y1 + obj->h;
        CUBE_JUDEG_FULL_RANK(front)
        gui_get_acc()->blit(front, dc, &draw_rect);
    }

    if (nz4567 > 0.0f)
    {
        draw_rect.x1 = this->draw_img_back.img_x;
        draw_rect.y1 = this->draw_img_back.img_y;
        draw_rect.x2 = draw_rect.x1 + obj->w;
        draw_rect.y2 = draw_rect.y1 + obj->h;
        CUBE_JUDEG_FULL_RANK(back)
        gui_get_acc()->blit(back, dc, &draw_rect);
    }

    if (nz5126 > 0.0f)
    {
        draw_rect.x1 = this->draw_img_up.img_x;
        draw_rect.y1 = this->draw_img_up.img_y;
        draw_rect.x2 = draw_rect.x1 + obj->w;
        draw_rect.y2 = draw_rect.y1 + obj->h;
        CUBE_JUDEG_FULL_RANK(up)
        gui_get_acc()->blit(up, dc, &draw_rect);
    }

    if (nz0473 > 0.0f)
    {
        draw_rect.x1 = this->draw_img_down.img_x;
        draw_rect.y1 = this->draw_img_down.img_y;
        draw_rect.x2 = draw_rect.x1 + obj->w;
        draw_rect.y2 = draw_rect.y1 + obj->h;
        CUBE_JUDEG_FULL_RANK(down)
        gui_get_acc()->blit(down, dc, &draw_rect);
    }

    if (nz7623 > 0.0f)
    {
        draw_rect.x1 = this->draw_img_left.img_x;
        draw_rect.y1 = this->draw_img_left.img_y;
        draw_rect.x2 = draw_rect.x1 + obj->w;
        draw_rect.y2 = draw_rect.y1 + obj->h;
        CUBE_JUDEG_FULL_RANK(left)
        gui_get_acc()->blit(left, dc, &draw_rect);
    }

    if (nz0154 > 0.0f)
    {
        draw_rect.x1 = this->draw_img_right.img_x;
        draw_rect.y1 = this->draw_img_right.img_y;
        draw_rect.x2 = draw_rect.x1 + obj->w;
        draw_rect.y2 = draw_rect.y1 + obj->h;
        CUBE_JUDEG_FULL_RANK(right)
        gui_get_acc()->blit(right, dc, &draw_rect);
    }
}


static void cube_end(gui_obj_t *obj)
{

}
static void cube_destory(gui_obj_t *obj)
{
    gui_cube_t *this = (gui_cube_t *)obj;
    gui_free(this->draw_img_front.matrix);
    gui_free(this->draw_img_back.matrix);
    gui_free(this->draw_img_up.matrix);
    gui_free(this->draw_img_down.matrix);
    gui_free(this->draw_img_left.matrix);
    gui_free(this->draw_img_right.matrix);

    gui_free(this->draw_img_front.inverse);
    gui_free(this->draw_img_back.inverse);
    gui_free(this->draw_img_up.inverse);
    gui_free(this->draw_img_down.inverse);
    gui_free(this->draw_img_left.inverse);
    gui_free(this->draw_img_right.inverse);

}

static void gui_cube_ctor(gui_cube_t *this, gui_obj_t *parent, const char *name,
                          void *addr,
                          int16_t x,
                          int16_t y, int16_t w, int16_t h)
{

    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = VG_LITE_CUBE;
    root->obj_prepare = cube_prepare;
    root->obj_draw = cube_draw_cb;
    root->obj_end = cube_end;
    root->obj_destory = cube_destory;
    //for self
    void **array = (void **)addr;




    this->draw_img_front.opacity_value = UINT8_MAX;
    this->draw_img_back.opacity_value = UINT8_MAX;
    this->draw_img_up.opacity_value = UINT8_MAX;
    this->draw_img_down.opacity_value = UINT8_MAX;
    this->draw_img_left.opacity_value = UINT8_MAX;
    this->draw_img_right.opacity_value = UINT8_MAX;


    this->draw_img_front.data = array[0];
    this->draw_img_back.data = array[1];
    this->draw_img_up.data = array[2];
    this->draw_img_down.data = array[3];
    this->draw_img_left.data = array[4];
    this->draw_img_right.data = array[5];

    this->draw_img_front.blend_mode = IMG_SRC_OVER_MODE;
    this->draw_img_back.blend_mode = IMG_SRC_OVER_MODE;
    this->draw_img_up.blend_mode = IMG_SRC_OVER_MODE;
    this->draw_img_down.blend_mode = IMG_SRC_OVER_MODE;
    this->draw_img_left.blend_mode = IMG_SRC_OVER_MODE;
    this->draw_img_right.blend_mode = IMG_SRC_OVER_MODE;

    this->draw_img_front.matrix = gui_malloc(sizeof(struct rtgui_matrix));
    this->draw_img_back.matrix = gui_malloc(sizeof(struct rtgui_matrix));
    this->draw_img_up.matrix = gui_malloc(sizeof(struct rtgui_matrix));
    this->draw_img_down.matrix = gui_malloc(sizeof(struct rtgui_matrix));
    this->draw_img_left.matrix = gui_malloc(sizeof(struct rtgui_matrix));
    this->draw_img_right.matrix = gui_malloc(sizeof(struct rtgui_matrix));

    this->draw_img_front.inverse = gui_malloc(sizeof(struct rtgui_matrix));
    this->draw_img_back.inverse = gui_malloc(sizeof(struct rtgui_matrix));
    this->draw_img_up.inverse = gui_malloc(sizeof(struct rtgui_matrix));
    this->draw_img_down.inverse = gui_malloc(sizeof(struct rtgui_matrix));
    this->draw_img_left.inverse = gui_malloc(sizeof(struct rtgui_matrix));
    this->draw_img_right.inverse = gui_malloc(sizeof(struct rtgui_matrix));

    gui_dispdev_t *dc = gui_get_dc();
    this->cbsize = dc->fb_height / 8.0;
    this->c_x = (dc->fb_width - this->cbsize) / 2.0f;
    this->c_y = (dc->fb_width - this->cbsize) / 2.0f;

}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_cube_set_center(gui_cube_t *this, float c_x, float c_y)
{
    this->c_x = c_x;
    this->c_y = c_y;
}
void gui_cube_set_size(gui_cube_t *this, float size)
{
    this->cbsize = size;
}


gui_cube_t *gui_cube_create(void *parent,  const char *name, void *data,
                            int16_t x,
                            int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_CUBE";
    }
    gui_cube_t *cube = gui_malloc(sizeof(gui_cube_t));
    GUI_ASSERT(cube != NULL);
    memset(cube, 0x00, sizeof(gui_cube_t));

    gui_cube_ctor(cube, (gui_obj_t *)parent, name, data, x, y, w, h);
    gui_list_init(&(GET_BASE(cube)->child_list));
    if ((GET_BASE(cube)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(cube)->parent)->child_list),
                               &(GET_BASE(cube)->brother_list));
    }
    GET_BASE(cube)->create_done = true;
    return cube;
}


/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */





