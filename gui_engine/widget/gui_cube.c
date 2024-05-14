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
#include <acc_init.h>
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
static void gui_cube_scale(Vertex_t *vertex, float scale)
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

static void gui_cube_transfrom_rotate(struct gui_matrix *rotate, Vertex_t *vertex, Vertex_t *rc,
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

static void gui_cube_transfrom_normalZ(struct gui_matrix *rotate, Normal_t *nVec, float *nZ)
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
                                    Vertex_t          *v0,
                                    Vertex_t          *v1,
                                    Vertex_t          *v2,
                                    Vertex_t          *v3,
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

static void gui_cube_prepare(gui_obj_t *obj)
{
    struct gui_matrix rotate_3D;
    struct gui_matrix matrix;
    Vertex_t rv0, rv1, rv2, rv3, rv4, rv5, rv6, rv7;
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
    gui_obj_t *root = (gui_obj_t *)obj;
    touch_info_t *tp = tp_get_info();
    gui_cube_img_event_t cube_event;
    cube_event.size = 0;
    cube_event.event = GUI_EVENT_INVALIDE;
    GUI_UNUSED(root);

    // Scale the cube to proper size
    float cbsize = this->cbsize;

    // Translate the cube to the center of framebuffer.
    float xoff = (this->c_x - dc->screen_width / 2) + dc->screen_width / 2;
    float yoff = (this->c_y - dc->screen_height / 2) + dc->screen_height / 2;

    gui_cube_scale(&cube_v0, cbsize);
    gui_cube_scale(&cube_v1, cbsize);
    gui_cube_scale(&cube_v2, cbsize);
    gui_cube_scale(&cube_v3, cbsize);
    gui_cube_scale(&cube_v4, cbsize);
    gui_cube_scale(&cube_v5, cbsize);
    gui_cube_scale(&cube_v6, cbsize);
    gui_cube_scale(&cube_v7, cbsize);

    // Rotation angles (degree) for axis X, Y, Z
    gui_cube_compute_rotate(this->xrot, this->yrot, this->zrot, &rotate_3D);

    if (this->auto_flag == true)
    {
        gui_fb_change();
        this->yrot = this->yrot + this->step_degree_y;
    }

    // Compute the new cube vertex coordinates transformed by the rotation matrix.
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v0, &rv0, xoff, yoff);
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v1, &rv1, xoff, yoff);
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v2, &rv2, xoff, yoff);
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v3, &rv3, xoff, yoff);
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v4, &rv4, xoff, yoff);
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v5, &rv5, xoff, yoff);
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v6, &rv6, xoff, yoff);
    gui_cube_transfrom_rotate(&rotate_3D, &cube_v7, &rv7, xoff, yoff);

    // Compute the surface normal direction to determine the front/back face.
    gui_cube_transfrom_normalZ(&rotate_3D, &normal0321, &this->nz0321);
    gui_cube_transfrom_normalZ(&rotate_3D, &normal4567, &this->nz4567);
    gui_cube_transfrom_normalZ(&rotate_3D, &normal1265, &this->nz5126);
    gui_cube_transfrom_normalZ(&rotate_3D, &normal0473, &this->nz0473);
    gui_cube_transfrom_normalZ(&rotate_3D, &normal2376, &this->nz7623);
    gui_cube_transfrom_normalZ(&rotate_3D, &normal0154, &this->nz0154);

    draw_img_t *front = &this->draw_img_front;
    draw_img_t *back = &this->draw_img_back;
    draw_img_t *up = &this->draw_img_up;
    draw_img_t *down = &this->draw_img_down;
    draw_img_t *left = &this->draw_img_left;
    draw_img_t *right = &this->draw_img_right;


    if (this->nz0321 > 0.0f)
    {
        gui_cube_transfrom_blit(front->img_w, front->img_h, &rv0, &rv3, &rv2, &rv1, &matrix);

        gui_matrix_t tmp;
        memcpy(&tmp, obj->matrix, sizeof(struct gui_matrix));
        matrix_multiply(&tmp, &matrix);

        memcpy(&front->matrix, &tmp, sizeof(struct gui_matrix));
        memcpy(&front->inverse, &tmp, sizeof(struct gui_matrix));
        matrix_inverse(&front->inverse);
        gui_image_load_scale(front);
        gui_image_new_area(front, NULL);
        if (gui_cube_point_in_rect(front, tp->x, tp->y) == true)
        {
            if (this->nz0321 > cube_event.size)
            {
                cube_event.size = this->nz0321;
                cube_event.event = GUI_EVENT_1;
            }
        }
    }

    if (this->nz4567 > 0.0f)
    {
        gui_cube_transfrom_blit(back->img_w, back->img_h, &rv4, &rv5, &rv6, &rv7, &matrix);
        gui_matrix_t tmp;
        memcpy(&tmp, obj->matrix, sizeof(struct gui_matrix));
        matrix_multiply(&tmp, &matrix);
        memcpy(&back->matrix, &tmp, sizeof(struct gui_matrix));
        memcpy(&back->inverse, &tmp, sizeof(struct gui_matrix));
        matrix_inverse(&back->inverse);
        gui_image_load_scale(back);
        gui_image_new_area(back, NULL);
        if (gui_cube_point_in_rect(back, tp->x, tp->y) == true)
        {
            if (this->nz4567 > cube_event.size)
            {
                cube_event.size = this->nz4567;
                cube_event.event = GUI_EVENT_2;
            }
        }
    }

    if (this->nz5126 > 0.0f)
    {
        gui_cube_transfrom_blit(up->img_w, up->img_h, &rv5, &rv1, &rv2, &rv6, &matrix);
        gui_matrix_t tmp;
        memcpy(&tmp, obj->matrix, sizeof(struct gui_matrix));
        matrix_multiply(&tmp, &matrix);
        memcpy(&up->matrix, &tmp, sizeof(struct gui_matrix));
        memcpy(&up->inverse, &tmp, sizeof(struct gui_matrix));
        matrix_inverse(&up->inverse);
        gui_image_load_scale(up);
        gui_image_new_area(up, NULL);
        if (gui_cube_point_in_rect(up, tp->x, tp->y) == true)
        {
            if (this->nz5126 > cube_event.size)
            {
                cube_event.size = this->nz5126;
                cube_event.event = GUI_EVENT_3;
            }
        }
    }

    if (this->nz0473 > 0.0f)
    {
        gui_cube_transfrom_blit(down->img_w, down->img_h, &rv0, &rv4, &rv7, &rv3, &matrix);
        gui_matrix_t tmp;
        memcpy(&tmp, obj->matrix, sizeof(struct gui_matrix));
        matrix_multiply(&tmp, &matrix);
        memcpy(&down->matrix, &tmp, sizeof(struct gui_matrix));
        memcpy(&down->inverse, &tmp, sizeof(struct gui_matrix));
        matrix_inverse(&down->inverse);
        gui_image_load_scale(down);
        gui_image_new_area(down, NULL);
        if (gui_cube_point_in_rect(down, tp->x, tp->y) == true)
        {
            if (this->nz0473 > cube_event.size)
            {
                cube_event.size = this->nz0473;
                cube_event.event = GUI_EVENT_4;
            }
        }
    }

    if (this->nz7623 > 0.0f)
    {
        gui_cube_transfrom_blit(left->img_w, left->img_h, &rv7, &rv6, &rv2, &rv3, &matrix);
        gui_matrix_t tmp;
        memcpy(&tmp, obj->matrix, sizeof(struct gui_matrix));
        matrix_multiply(&tmp, &matrix);
        memcpy(&left->matrix, &tmp, sizeof(struct gui_matrix));
        memcpy(&left->inverse, &tmp, sizeof(struct gui_matrix));
        matrix_inverse(&left->inverse);
        gui_image_load_scale(left);
        gui_image_new_area(left, NULL);
        if (gui_cube_point_in_rect(left, tp->x, tp->y) == true)
        {
            if (this->nz7623 > cube_event.size)
            {
                cube_event.size = this->nz7623;
                cube_event.event = GUI_EVENT_5;
            }
        }
    }

    if (this->nz0154 > 0.0f)
    {
        gui_cube_transfrom_blit(right->img_w, right->img_h, &rv0, &rv1, &rv5, &rv4, &matrix);
        gui_matrix_t tmp;
        memcpy(&tmp, obj->matrix, sizeof(struct gui_matrix));
        matrix_multiply(&tmp, &matrix);
        memcpy(&right->matrix, &tmp, sizeof(struct gui_matrix));
        memcpy(&right->inverse, &tmp, sizeof(struct gui_matrix));
        matrix_inverse(&right->inverse);
        gui_image_load_scale(right);
        gui_image_new_area(right, NULL);
        if (gui_cube_point_in_rect(right, tp->x, tp->y) == true)
        {
            if (this->nz0154 > cube_event.size)
            {
                cube_event.size = this->nz0154;
                cube_event.event = GUI_EVENT_6;
            }
        }
    }
    if (tp->type == TOUCH_SHORT &&
        cube_event.size > 0 &&
        cube_event.event != GUI_EVENT_INVALIDE)
    {
        gui_obj_event_set(obj, cube_event.event);
    }
}

static void gui_cube_draw_cb(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_cube_t *this = (gui_cube_t *)obj;

    draw_img_t *front = &this->draw_img_front;
    draw_img_t *back = &this->draw_img_back;
    draw_img_t *up = &this->draw_img_up;
    draw_img_t *down = &this->draw_img_down;
    draw_img_t *left = &this->draw_img_left;
    draw_img_t *right = &this->draw_img_right;

    if (this->nz0321 > 0.0f)
    {
        CUBE_JUDEG_FULL_RANK(&front)
        gui_acc_blit_to_dc(front, dc, NULL);
    }

    if (this->nz4567 > 0.0f)
    {
        CUBE_JUDEG_FULL_RANK(&back)
        gui_acc_blit_to_dc(back, dc, NULL);
    }

    if (this->nz5126 > 0.0f)
    {
        CUBE_JUDEG_FULL_RANK(&up)
        gui_acc_blit_to_dc(up, dc, NULL);
    }

    if (this->nz0473 > 0.0f)
    {
        CUBE_JUDEG_FULL_RANK(&down)
        gui_acc_blit_to_dc(down, dc, NULL);
    }

    if (this->nz7623 > 0.0f)
    {
        CUBE_JUDEG_FULL_RANK(&left)
        gui_acc_blit_to_dc(left, dc, NULL);
    }

    if (this->nz0154 > 0.0f)
    {
        CUBE_JUDEG_FULL_RANK(&right)
        gui_acc_blit_to_dc(right, dc, NULL);
    }
}

static void gui_cube_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_cube_t *cube = (gui_cube_t *)obj;
    draw_img_t *cube_img[6];

    cube_img[0] = &cube->draw_img_front;
    cube_img[1] = &cube->draw_img_back;
    cube_img[2] = &cube->draw_img_up;
    cube_img[3] = &cube->draw_img_down;
    cube_img[4] = &cube->draw_img_left;
    cube_img[5] = &cube->draw_img_right;

    for (int i = 0; i < 6; i++)
    {
        if (gui_image_acc_end_cb != NULL)
        {
            gui_image_acc_end_cb(cube_img[i]);
        }
    }
}

static void gui_cube_destory(gui_obj_t *obj)
{
    gui_cube_t *this = (gui_cube_t *)obj;
    draw_img_t *cube_img[6];

    cube_img[0] = &this->draw_img_front;
    cube_img[1] = &this->draw_img_back;
    cube_img[2] = &this->draw_img_up;
    cube_img[3] = &this->draw_img_down;
    cube_img[4] = &this->draw_img_left;
    cube_img[5] = &this->draw_img_right;

    for (int i = 0; i < 6; i++)
    {
        if (cube_img[i]->src_mode)
        {
#ifdef _WIN32
            // free path transforming memory on win
            gui_free(cube_img[i]->data);
#endif
        }
    }
}

static void gui_cube_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_cube_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_cube_draw_cb(obj);
            break;

        case OBJ_END:
            gui_cube_end(obj);
            break;

        case OBJ_DESTORY:
            gui_cube_destory(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_cube_ctor(gui_cube_t         *this,
                          gui_obj_t          *parent,
                          const char         *name,
                          gui_cube_imgfile_t *img_file,
                          int16_t             x,
                          int16_t             y,
                          int16_t             w,
                          int16_t             h)
{
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_t *root = (gui_obj_t *)this;

    //for base class
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    root->type = VG_LITE_CUBE;
    root->obj_cb = gui_cube_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;

    //for self
    draw_img_t *cube_img[6];
    cube_img[0] = &this->draw_img_front;
    cube_img[1] = &this->draw_img_back;
    cube_img[2] = &this->draw_img_up;
    cube_img[3] = &this->draw_img_down;
    cube_img[4] = &this->draw_img_left;
    cube_img[5] = &this->draw_img_right;

    char *img_path[6];
    img_path[0] = img_file->img_path.img_path_front;
    img_path[1] = img_file->img_path.img_path_back;
    img_path[2] = img_file->img_path.img_path_up;
    img_path[3] = img_file->img_path.img_path_down;
    img_path[4] = img_file->img_path.img_path_left;
    img_path[5] = img_file->img_path.img_path_right;

    for (int i = 0; i < 6; i++)
    {
        if (img_file->src_mode[i] == IMG_SRC_FILESYS)
        {
            char *path = img_path[i];
#ifdef _WIN32
            path = gui_filepath_transforming(path);
#endif
            cube_img[i]->data = path;
        }
        else if (img_file->src_mode[i] == IMG_SRC_MEMADDR)
        {
            cube_img[i]->data = img_path[i];
        }

        cube_img[i]->src_mode = img_file->src_mode[i];
        cube_img[i]->opacity_value = UINT8_MAX;
        cube_img[i]->blend_mode = IMG_SRC_OVER_MODE;
    }

    gui_dispdev_t *dc = gui_get_dc();
    this->cbsize = dc->fb_height / 8.0;
    this->c_x = (dc->fb_width - this->cbsize) / 2.0f;
    this->c_y = (dc->fb_width - this->cbsize) / 2.0f;
    // Set the intial cube rotation degree and step.
    this->xrot = 20.0f;
    this->yrot = 0.0f;
    this->zrot = 20.0f;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_cube_auto_rotation_by_y(gui_cube_t *this, uint32_t internal_ms, float degree)
{
    this->auto_flag = true;
    this->interval_ms_y = internal_ms;
    this->step_degree_y = degree;
}

void gui_cube_set_center(gui_cube_t *this, float c_x, float c_y)
{
    this->c_x = c_x;
    this->c_y = c_y;
}

void gui_cube_set_size(gui_cube_t *this, float size)
{
    this->cbsize = size;
}

BLEND_MODE_TYPE gui_cube_get_mode(gui_cube_t *cube, T_CUBE_SIDE_TYPE side)
{
    GUI_ASSERT(cube != NULL);
    draw_img_t *cube_img[6];

    cube_img[0] = &cube->draw_img_front;
    cube_img[1] = &cube->draw_img_back;
    cube_img[2] = &cube->draw_img_up;
    cube_img[3] = &cube->draw_img_down;
    cube_img[4] = &cube->draw_img_left;
    cube_img[5] = &cube->draw_img_right;

    if (side == CUBE_SIDE_ALL)
    {
        // not supported
        return (BLEND_MODE_TYPE)0;
    }
    else
    {
        return (BLEND_MODE_TYPE)cube_img[side]->blend_mode;
    }
}

void gui_cube_set_mode(gui_cube_t *cube, T_CUBE_SIDE_TYPE side, BLEND_MODE_TYPE mode)
{
    GUI_ASSERT(cube != NULL);
    draw_img_t *cube_img[6];
    cube_img[0] = &cube->draw_img_front;
    cube_img[1] = &cube->draw_img_back;
    cube_img[2] = &cube->draw_img_up;
    cube_img[3] = &cube->draw_img_down;
    cube_img[4] = &cube->draw_img_left;
    cube_img[5] = &cube->draw_img_right;

    if (side == CUBE_SIDE_ALL)
    {
        for (int i = 0; i < 6; i++)
        {
            cube_img[i]->blend_mode = mode;
        }
    }
    else
    {
        cube_img[side]->blend_mode = mode;
    }
}

uint8_t gui_cube_get_opacity(gui_cube_t *cube, T_CUBE_SIDE_TYPE side)
{
    GUI_ASSERT(cube != NULL);
    draw_img_t *cube_img[6];

    cube_img[0] = &cube->draw_img_front;
    cube_img[1] = &cube->draw_img_back;
    cube_img[2] = &cube->draw_img_up;
    cube_img[3] = &cube->draw_img_down;
    cube_img[4] = &cube->draw_img_left;
    cube_img[5] = &cube->draw_img_right;

    if (side == CUBE_SIDE_ALL)
    {
        // not supported
        return 0;
    }
    else
    {
        return cube_img[side]->opacity_value;
    }
}

void gui_cube_add_click_cb(gui_cube_t *this, gui_cube_cb_t cb_list)
{
    gui_obj_add_event_cb(this, cb_list.click_cb_front, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(this, cb_list.click_cb_back, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(this, cb_list.click_cb_up, GUI_EVENT_3, NULL);
    gui_obj_add_event_cb(this, cb_list.click_cb_down, GUI_EVENT_4, NULL);
    gui_obj_add_event_cb(this, cb_list.click_cb_left, GUI_EVENT_5, NULL);
    gui_obj_add_event_cb(this, cb_list.click_cb_right, GUI_EVENT_6, NULL);
}

void gui_cube_set_opacity(gui_cube_t *cube, T_CUBE_SIDE_TYPE side, uint8_t opacity)
{
    GUI_ASSERT(cube != NULL);
    draw_img_t *cube_img[6];
    cube_img[0] = &cube->draw_img_front;
    cube_img[1] = &cube->draw_img_back;
    cube_img[2] = &cube->draw_img_up;
    cube_img[3] = &cube->draw_img_down;
    cube_img[4] = &cube->draw_img_left;
    cube_img[5] = &cube->draw_img_right;

    if (side == CUBE_SIDE_ALL)
    {
        for (int i = 0; i < 6; i++)
        {
            cube_img[i]->opacity_value = opacity;
        }
    }
    else
    {
        cube_img[side]->opacity_value = opacity;
    }
}

void gui_cube_set_img(gui_cube_t *cube, gui_cube_imgfile_t *img_file)
{
    gui_cube_t *this = cube;

    GUI_ASSERT(this != NULL);

    draw_img_t *cube_img[6];
    cube_img[0] = &this->draw_img_front;
    cube_img[1] = &this->draw_img_back;
    cube_img[2] = &this->draw_img_up;
    cube_img[3] = &this->draw_img_down;
    cube_img[4] = &this->draw_img_left;
    cube_img[5] = &this->draw_img_right;

    char *img_path[6];
    img_path[0] = img_file->img_path.img_path_front;
    img_path[1] = img_file->img_path.img_path_back;
    img_path[2] = img_file->img_path.img_path_up;
    img_path[3] = img_file->img_path.img_path_down;
    img_path[4] = img_file->img_path.img_path_left;
    img_path[5] = img_file->img_path.img_path_right;

    for (int i = 0; i < 6; i++)
    {
        // reset file data
        if (cube_img[i]->src_mode == IMG_SRC_FILESYS)
        {
#ifdef _WIN32
            gui_free(cube_img[i]->data);
#endif
            cube_img[i]->data = NULL;
        }

        // set new images
        cube_img[i]->src_mode = img_file->src_mode[i];
        if (img_file->src_mode[i] == IMG_SRC_FILESYS)
        {
            char *path = img_path[i];
#ifdef _WIN32
            path = gui_filepath_transforming(path);
#endif
            cube_img[i]->data = path;
        }
        else if (img_file->src_mode[i] == IMG_SRC_MEMADDR)
        {
            cube_img[i]->data = img_path[i];
        }
    }
}

gui_cube_t *gui_cube_create(void               *parent,
                            const char         *name,
                            gui_cube_imgfile_t *img_file,
                            int16_t             x,
                            int16_t             y)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "DEFAULT_CUBE";
    }

    gui_cube_t *cube = gui_malloc(sizeof(gui_cube_t));
    GUI_ASSERT(cube != NULL);
    memset(cube, 0x00, sizeof(gui_cube_t));

    gui_cube_ctor(cube, (gui_obj_t *)parent, name, img_file, x, y, 0, 0);
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
