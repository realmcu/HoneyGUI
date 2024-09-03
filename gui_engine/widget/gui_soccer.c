/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_soccer.c
  * @brief soccer widget
  * @details soccer widget
  * @author howie_wang@realsil.com.cn
  * @date 2024/08/16
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
#include "gui_soccer.h"
#include "tp_algo.h"
#include <math.h>
#include "gui_fb.h"

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
const gui_vertex_t vertices[12] =
{
    {-1,  1.61803f,  0}, { 1,  1.61803f,  0}, {-1, -1.61803f,  0}, { 1, -1.61803f,  0},
    { 0, -1,  1.61803f}, { 0,  1,  1.61803f}, { 0, -1, -1.61803f}, { 0,  1, -1.61803f},
    { 1.61803f,  0, -1}, { 1.61803f,  0,  1}, {-1.61803f,  0, -1}, {-1.61803f,  0,  1}
};

int faces[20][3] =
{
    {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
    {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
    {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
    {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
};

/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */

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
static void gui_soccer_scale(gui_vertex_t *vertex, float scale)
{
    // Scale soccer vertex coordinates to proper size.
    vertex->x *= scale;
    vertex->y *= scale;
    vertex->z *= scale;
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
static void gui_soccer_transfrom_blit(float              w,
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


/**
 *
 */
static gui_vertex_t crossProduct(gui_vertex_t v1, gui_vertex_t v2)
{
    gui_vertex_t cross;
    cross.x = v1.y * v2.z - v1.z * v2.y;
    cross.y = v1.z * v2.x - v1.x * v2.z;
    cross.z = v1.x * v2.y - v1.y * v2.x;
    return cross;
}

/**
 *
 */
static gui_vertex_t vectorSubtract(gui_vertex_t a, gui_vertex_t b)
{
    gui_vertex_t result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

/**
 *
 */
static gui_vertex_t computeNormal(gui_vertex_t a, gui_vertex_t b, gui_vertex_t c)
{
    gui_vertex_t ab = vectorSubtract(b, a);
    gui_vertex_t ac = vectorSubtract(c, a);
    gui_vertex_t normal = crossProduct(ab, ac);

    float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    if (length != 0)
    {
        normal.x /= length;
        normal.y /= length;
        normal.z /= length;
    }

    return normal;
}

gui_quaternion_t quat_identity()
{
    return (gui_quaternion_t) {1.0f, 0.0f, 0.0f, 0.0f};
}

gui_quaternion_t quaternion_multiply(gui_quaternion_t q1, gui_quaternion_t q2)
{
    gui_quaternion_t result;
    result.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    result.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    result.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    return result;
}

gui_quaternion_t quaternion_from_angle_axis(float angle, float axisX, float axisY, float axisZ)
{
    gui_quaternion_t result;
    float halfAngleRadian = angle * 0.5f;
    float sinHalfAngle = sinf(halfAngleRadian);
    result.x = axisX * sinHalfAngle;
    result.y = axisY * sinHalfAngle;
    result.z = axisZ * sinHalfAngle;
    result.w = cosf(halfAngleRadian);
    return result;
}

void quaternion_to_matrix(gui_quaternion_t *quaternion, gui_matrix_t *matrix)
{
    float x = quaternion->x;
    float y = quaternion->y;
    float z = quaternion->z;
    float w = quaternion->w;

    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    matrix->m[0][0] = 1 - 2 * (yy + zz);
    matrix->m[0][1] = 2 * (xy - wz);
    matrix->m[0][2] = 2 * (xz + wy);

    matrix->m[1][0] = 2 * (xy + wz);
    matrix->m[1][1] = 1 - 2 * (xx + zz);
    matrix->m[1][2] = 2 * (yz - wx);

    matrix->m[2][0] = 2 * (xz - wy);
    matrix->m[2][1] = 2 * (yz + wx);
    matrix->m[2][2] = 1 - 2 * (xx + yy);
}

static void gui_soccer_prepare(gui_obj_t *obj)
{
    gui_soccer_t *this = (gui_soccer_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = (touch_info_t *)tp_get_info();

    this->yrot = 1;
    this->rotation = quaternion_multiply(quaternion_from_angle_axis(this->yrot / 180.0f * PHI, 0, 1, 0),
                                         this->rotation);
    float scsize = this->scsize;
    float xoff = (this->c_x - dc->screen_width / 2) + dc->screen_width / 2;
    float yoff = (this->c_y - dc->screen_height / 2) + dc->screen_height / 2;

    switch (tp->type)
    {
    case TOUCH_HOLD_X:
        this->rotation = quaternion_multiply(quaternion_from_angle_axis(tp->deltaX / 360.0f, 0, 1, 0),
                                             this->rotation);
        break;

    case TOUCH_HOLD_Y:
        this->rotation = quaternion_multiply(quaternion_from_angle_axis(-tp->deltaY / 360.0f, 1, 0, 0),
                                             this->rotation);
        break;

    default:
        break;
    }

    for (int i = 0; i < 20; i++)
    {
        gui_vertex_t v0 = {vertices[faces[i][0]].x, vertices[faces[i][0]].y, vertices[faces[i][0]].z};
        gui_vertex_t v1 = {vertices[faces[i][1]].x, vertices[faces[i][1]].y, vertices[faces[i][1]].z};
        gui_vertex_t v2 = {vertices[faces[i][2]].x, vertices[faces[i][2]].y, vertices[faces[i][2]].z};

        this->normal[i] = computeNormal(v0, v1, v2);

        gui_vertex_t v_center = {(v0.x + v1.x + v2.x) / 3, (v0.y + v1.y + v2.y) / 3, (v0.z + v1.z + v2.z) / 3};


        gui_vertex_t qv0_bengin = { (1.0f - 2.0f / 3) *v2.x + (2.0f / 3) *v0.x, \
                                    (1.0f - 2.0f / 3) *v2.y + (2.0f / 3) *v0.y, \
                                    (1.0f - 2.0f / 3) *v2.z + (2.0f / 3) *v0.z
                                  };

        gui_vertex_t qv0_end = {(1.0f - 1.0f / 6) *v0.x + (1.0f / 6) *v1.x, \
                                (1.0f - 1.0f / 6) *v0.y + (1.0f / 6) *v1.y, \
                                (1.0f - 1.0f / 6) *v0.z + (1.0f / 6) *v1.z
                               };

        gui_vertex_t qv0 = {1.154700f * (qv0_end.x - qv0_bengin.x) + qv0_bengin.x, \
                            1.154700f * (qv0_end.y - qv0_bengin.y) + qv0_bengin.y, \
                            1.154700f * (qv0_end.z - qv0_bengin.z) + qv0_bengin.z
                           };

        gui_vertex_t qv1_bengin = { (1.0f - 2.0f / 3) *v2.x + (2.0f / 3) *v1.x, \
                                    (1.0f - 2.0f / 3) *v2.y + (2.0f / 3) *v1.y, \
                                    (1.0f - 2.0f / 3) *v2.z + (2.0f / 3) *v1.z
                                  };

        gui_vertex_t qv1_end = {(1.0f - 5.0f / 6) *v0.x + (5.0f / 6) *v1.x, \
                                (1.0f - 5.0f / 6) *v0.y + (5.0f / 6) *v1.y, \
                                (1.0f - 5.0f / 6) *v0.z + (5.0f / 6) *v1.z
                               };

        gui_vertex_t qv1 = {1.154700f * (qv1_end.x - qv1_bengin.x) + qv1_bengin.x, \
                            1.154700f * (qv1_end.y - qv1_bengin.y) + qv1_bengin.y, \
                            1.154700f * (qv1_end.z - qv1_bengin.z) + qv1_bengin.z
                           };

        gui_vertex_t qv2 = {2 * v_center.x - qv0.x, 2 * v_center.y - qv0.y, 2 * v_center.z - qv0.z};
        gui_vertex_t qv3 = {2 * v_center.x - qv1.x, 2 * v_center.y - qv1.y, 2 * v_center.z - qv1.z};

        gui_vertex_t qrv0;
        gui_vertex_t qrv1;
        gui_vertex_t qrv2;
        gui_vertex_t qrv3;

        gui_soccer_scale(&qv0, scsize);
        gui_soccer_scale(&qv1, scsize);
        gui_soccer_scale(&qv2, scsize);
        gui_soccer_scale(&qv3, scsize);

        gui_matrix_t matrix;
        gui_matrix_t rotate_3D;

        quaternion_to_matrix(&this->rotation, &rotate_3D);
        matrix_multiply_normal(&rotate_3D, &this->normal[i]);

        matrix_transfrom_rotate(&rotate_3D, &qv0, &qrv0, xoff, yoff, 0);
        matrix_transfrom_rotate(&rotate_3D, &qv1, &qrv1, xoff, yoff, 0);
        matrix_transfrom_rotate(&rotate_3D, &qv2, &qrv2, xoff, yoff, 0);
        matrix_transfrom_rotate(&rotate_3D, &qv3, &qrv3, xoff, yoff, 0);
        gui_soccer_transfrom_blit(100, 100, &qrv1, &qrv0, &qrv3, &qrv2, &matrix);


        gui_matrix_t tmp;
        memcpy(&tmp, obj->matrix, sizeof(struct gui_matrix));
        matrix_multiply(&tmp, &matrix);
        memcpy(&this->draw_img[i].matrix, &tmp, sizeof(struct gui_matrix));
        memcpy(&this->draw_img[i].inverse, &tmp, sizeof(struct gui_matrix));
        matrix_inverse(&this->draw_img[i].inverse);
        draw_img_new_area(&this->draw_img[i], NULL);
    }
    gui_fb_change();
}

static void gui_soccer_draw_cb(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_soccer_t *this = (gui_soccer_t *)obj;

    for (uint8_t i = 0; i < 20; i++)
    {
        draw_img_t *draw_img = &this->draw_img[i];
        if (this->normal[i].z >= 0.0f)
        {
            if (matrix_is_full_rank(&draw_img->matrix))
            {
                gui_acc_blit_to_dc(draw_img, dc, NULL);
            }

        }
    }
}

static void gui_soccer_end(gui_obj_t *obj)
{
    GUI_ASSERT(obj != NULL);
    gui_soccer_t *this = (gui_soccer_t *)obj;

    for (int i = 0; i < 20; i++)
    {
        if (draw_img_acc_end_cb != NULL)
        {
            draw_img_acc_end_cb(&this->draw_img[i]);
        }
    }
}

static void gui_soccer_destory(gui_obj_t *obj)
{
    return;
}

static void gui_soccer_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_soccer_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_soccer_draw_cb(obj);
            break;

        case OBJ_END:
            gui_soccer_end(obj);
            break;

        case OBJ_DESTORY:
            gui_soccer_destory(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_soccer_ctor(gui_soccer_t       *this,
                            gui_obj_t          *parent,
                            const char         *name,
                            void               *addr,
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
    root->type = VG_LITE_SOCCER;
    root->obj_cb = gui_soccer_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;

    //for self

    for (int i = 0; i < 20; i++)
    {
        this->draw_img[i].opacity_value = UINT8_MAX;
        this->draw_img[i].blend_mode = IMG_SRC_OVER_MODE;
        this->draw_img[i].data = addr;
        this->draw_img[i].img_w = 100;
        this->draw_img[i].img_h = 100;
    }

    gui_dispdev_t *dc = gui_get_dc();
    this->scsize = 100;
    this->c_x = (dc->fb_width - this->scsize) / 2.0f;
    this->c_y = (dc->fb_width - this->scsize) / 2.0f;

    this->rotation = quat_identity();
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/



gui_soccer_t *gui_soccer_create(void               *parent,
                                const char         *name,
                                void               *addr,
                                int16_t             x,
                                int16_t             y)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "DEFAULT_SOCCER";
    }

    gui_soccer_t *soccer = gui_malloc(sizeof(gui_soccer_t));
    GUI_ASSERT(soccer != NULL);
    memset(soccer, 0x00, sizeof(gui_soccer_t));

    gui_soccer_ctor(soccer, (gui_obj_t *)parent, name, addr, x, y, 0, 0);
    gui_list_init(&(GET_BASE(soccer)->child_list));
    if ((GET_BASE(soccer)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(soccer)->parent)->child_list),
                               &(GET_BASE(soccer)->brother_list));
    }
    GET_BASE(soccer)->create_done = true;
    return soccer;
}

BLEND_MODE_TYPE gui_soccer_get_mode(gui_soccer_t *soccer)
{
    GUI_ASSERT(soccer != NULL);
    return (BLEND_MODE_TYPE)soccer->draw_img[0].blend_mode;
}

void gui_soccer_set_mode(gui_soccer_t *soccer, BLEND_MODE_TYPE mode)
{
    GUI_ASSERT(soccer != NULL);

    for (int i = 0; i < 20; i++)
    {
        soccer->draw_img[i].blend_mode = mode;
    }
}

uint8_t gui_soccer_get_opacity(gui_soccer_t *soccer)
{
    GUI_ASSERT(soccer != NULL);
    return (BLEND_MODE_TYPE)soccer->draw_img[0].opacity_value;
}

void gui_soccer_set_opacity(gui_soccer_t *soccer, uint8_t opacity)
{
    GUI_ASSERT(soccer != NULL);

    for (int i = 0; i < 20; i++)
    {
        soccer->draw_img[i].opacity_value = opacity;
    }
}

void gui_soccer_set_img(gui_soccer_t *soccer, void *addr)
{
    GUI_ASSERT(soccer != NULL);

    for (int i = 0; i < 20; i++)
    {
        soccer->draw_img[i].data = addr;
    }
}

void gui_soccer_set_center(gui_soccer_t *this, float c_x, float c_y)
{
    this->c_x = c_x;
    this->c_y = c_y;
}

void gui_soccer_set_size(gui_soccer_t *this, float size)
{
    this->scsize = size;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
