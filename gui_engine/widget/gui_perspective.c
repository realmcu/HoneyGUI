/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_perspective.c
  * @brief perspective widget
  * @details create 3D widget
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/20
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
#include <tp_algo.h>
#include <draw_img.h>
#include <gui_matrix.h>
#include "gui_perspective.h"
#include "acc_engine.h"
#include <math.h>

/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */
typedef struct Point2f
{
    float x;
    float y;
} Point2f_t;
/** @brief  ... */

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

#define USE_FIX_SIN 1

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


/**
  * @brief  ...
  * @note   ...
  * @param[IN]  param1 ...
  * @param[IN]  param2 ...
  * @return ...
  * @retval ...
  */
#if USE_FIX_SIN
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

static float fix_sin(int angle)
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

static inline float fix_cos(int angle)
{
    return fix_sin(angle + 90);
}
#endif
/* Calculates coefficients of perspective transformation
* which maps (xi,yi) to (ui,vi), (i=1,2,3,4):
*
*      c00*xi + c01*yi + c02
* ui = ---------------------
*      c20*xi + c21*yi + c22
*
*      c10*xi + c11*yi + c12
* vi = ---------------------
*      c20*xi + c21*yi + c22
*
* Coefficients are calculated by solving linear system:
* / x0 y0  1  0  0  0 -x0*u0 -y0*u0 \ /c00\ /u0\
* | x1 y1  1  0  0  0 -x1*u1 -y1*u1 | |c01| |u1|
* | x2 y2  1  0  0  0 -x2*u2 -y2*u2 | |c02| |u2|
* | x3 y3  1  0  0  0 -x3*u3 -y3*u3 |.|c10|=|u3|,
* |  0  0  0 x0 y0  1 -x0*v0 -y0*v0 | |c11| |v0|
* |  0  0  0 x1 y1  1 -x1*v1 -y1*v1 | |c12| |v1|
* |  0  0  0 x2 y2  1 -x2*v2 -y2*v2 | |c20| |v2|
* \  0  0  0 x3 y3  1 -x3*v3 -y3*v3 / \c21/ \v3/
*
* where:
*   cij - matrix coefficients, c22 = 1
*/
static void swap(float *a, float *b)
{
    float c = *a;
    *a = *b;
    *b = c;
}
static bool Gauss(float A[][9], int equ, int var, float *answer)   //epu:A's row  var:A's col-1
{
    int row, col;
    for (row = 0, col = 0; col < var && row < equ; col++, row++)
    {
        int max_r = row;
        for (int i = row + 1; i < equ; i++)
        {
            if (fabs(A[i][col]) > fabs(A[max_r][col]))
            {
                max_r = i;
            }
        }
        if (max_r != row)
        {
            for (int j = 0; j < var + 1; j++)
            {
                swap(&A[row][j], &A[max_r][j]);
            }
        }

        if (fabs(A[row][col]) < (1e-6))
        {
            //row--;
            //continue;
            return false;
        }

        for (int i = row + 1; i < equ; i++)
        {

            if (fabs(A[i][col]) < (1e-6))
            {
                continue;
            }
            float tmp = -A[i][col] / A[row][col];
            for (int j = col; j < var + 1; j++)
            {
                A[i][j] += tmp * A[row][j];
            }
        }
    }


    for (int i = var - 1; i >= 0; i--)
    {
        float tmp = 0;
        for (int j = i + 1; j < var; j++)
        {
            tmp += A[i][j] * (*(answer + j));
        }
        answer[i] = (A[i][var] - tmp) / A[i][i];
    }
    return true;
}

static bool getPerspectiveTransform(Point2f_t *src, Point2f_t *dst, float *ret)
{
    float x0 = src[0].x, x1 = src[1].x, x2 = src[3].x, x3 = src[2].x;
    float y0 = src[0].y, y1 = src[1].y, y2 = src[3].y, y3 = src[2].y;
    float u0 = dst[0].x, u1 = dst[1].x, u2 = dst[3].x, u3 = dst[2].x;
    float v0 = dst[0].y, v1 = dst[1].y, v2 = dst[3].y, v3 = dst[2].y;
    float A[8][9] =
    {
        { x0, y0, 1, 0, 0, 0, -x0 * u0, -y0 * u0, u0 },
        { x1, y1, 1, 0, 0, 0, -x1 * u1, -y1 * u1, u1 },
        { x2, y2, 1, 0, 0, 0, -x2 * u2, -y2 * u2, u2 },
        { x3, y3, 1, 0, 0, 0, -x3 * u3, -y3 * u3, u3 },
        { 0, 0, 0, x0, y0, 1, -x0 * v0, -y0 * v0, v0 },
        { 0, 0, 0, x1, y1, 1, -x1 * v1, -y1 * v1, v1 },
        { 0, 0, 0, x2, y2, 1, -x2 * v2, -y2 * v2, v2 },
        { 0, 0, 0, x3, y3, 1, -x3 * v3, -y3 * v3, v3 },
    };
    if (Gauss(A, 8, 8, ret) == true)
    {
        *(ret + 8) = 1;
        return true;
    }
    return false;

}

static void scale_3d(Vertex_t *vertex, float scale)
{
    // Scale cube vertex coordinates to proper size.
    //
    vertex->x *= scale;
    vertex->y *= scale;
    vertex->z *= scale;
}

static void compute_rotate(float rx, float ry, float rz, struct gui_matrix *rotate)
{
    // Rotation angles rx, ry, rz (degree) for axis X, Y, Z
    // Compute 3D rotation matrix base on rotation angle rx, ry, rz about axis X, Y, Z.
    //
#if USE_FIX_SIN
    int frz = round(rz), frx = round(rx), fry = round(ry);
    rotate->m[0][0] = fix_cos((int)(frz)) * fix_cos((int)(fry));
    rotate->m[0][1] = fix_cos((int)(frz)) * fix_sin((int)(fry)) * fix_sin((int)(frx)) - fix_sin((int)(
                          frz)) * fix_cos((int)(frx));
    rotate->m[0][2] = fix_cos((int)(frz)) * fix_sin((int)(fry)) * fix_cos((int)(frx)) + fix_sin((int)(
                          frz)) * fix_sin((int)(frx));
    rotate->m[1][0] = fix_sin((int)(frz)) * fix_cos((int)(fry));
    rotate->m[1][1] = fix_sin((int)(frz)) * fix_sin((int)(fry)) * fix_sin((int)(frx)) + fix_cos((int)(
                          frz)) * fix_cos((int)(frx));
    rotate->m[1][2] = fix_sin((int)(frz)) * fix_sin((int)(fry)) * fix_cos((int)(frx)) - fix_cos((int)(
                          frz)) * fix_sin((int)(frx));
    rotate->m[2][0] = -fix_sin((int)(fry));
    rotate->m[2][1] = fix_cos((int)(fry)) * fix_sin((int)(frx));
    rotate->m[2][2] = fix_cos((int)(fry)) * fix_cos((int)(frx));
#else
    rotate->m[0][0] = cos(RAD(rz)) * cos(RAD(ry));
    rotate->m[0][1] = cos(RAD(rz)) * sin(RAD(ry)) * sin(RAD(rx)) - sin(RAD(rz)) * cos(RAD(rx));
    rotate->m[0][2] = cos(RAD(rz)) * sin(RAD(ry)) * cos(RAD(rx)) + sin(RAD(rz)) * sin(RAD(rx));
    rotate->m[1][0] = sin(RAD(rz)) * cos(RAD(ry));
    rotate->m[1][1] = sin(RAD(rz)) * sin(RAD(ry)) * sin(RAD(rx)) + cos(RAD(rz)) * cos(RAD(rx));
    rotate->m[1][2] = sin(RAD(rz)) * sin(RAD(ry)) * cos(RAD(rx)) - cos(RAD(rz)) * sin(RAD(rx));
    rotate->m[2][0] = -sin(RAD(ry));
    rotate->m[2][1] = cos(RAD(ry)) * sin(RAD(rx));
    rotate->m[2][2] = cos(RAD(ry)) * cos(RAD(rx));
#endif
}

static void transfrom_rotate(struct gui_matrix *rotate, Vertex_t *vertex, Vertex_t *rc, float tx,
                             float ty, float tz)
{
    // Compute the new cube vertex coordinates transformed by the rotation matrix.
    //
    rc->x = rotate->m[0][0] * vertex->x + rotate->m[0][1] * vertex->y + rotate->m[0][2] * vertex->z;
    rc->y = rotate->m[1][0] * vertex->x + rotate->m[1][1] * vertex->y + rotate->m[1][2] * vertex->z;
    rc->z = rotate->m[2][0] * vertex->x + rotate->m[2][1] * vertex->y + rotate->m[2][2] * vertex->z;

    // Translate the vertex in XYZ plane.
    //
    rc->x += tx;
    rc->y += ty;
    rc->z += tz;
}

// w means image width
// h means image height, use w and h can get four point in XY plane

// p means the perspective point,  use this parameter can get four point in XY plane
static void transfrom_blit(float w, float h, Vertex_t *p, Vertex_t *v0, Vertex_t *v1, Vertex_t *v2,
                           Vertex_t *v3,
                           struct gui_matrix *matrix)
{
    // Compute 3x3 image transform matrix to map a rectangle image (w,h) to
    // a parallelogram (x0,y0), (x1,y1), (x2,y2), (x3,y3) counterclock wise.

    float x0 = p->x - (p->x - v0->x) * p->z / (p->z - v0->z);
    float y0 = p->y - (p->y - v0->y) * p->z / (p->z - v0->z);
    float x1 = p->x - (p->x - v1->x) * p->z / (p->z - v1->z);
    float y1 = p->y - (p->y - v1->y) * p->z / (p->z - v1->z);
    float x2 = p->x - (p->x - v2->x) * p->z / (p->z - v2->z);
    float y2 = p->y - (p->y - v2->y) * p->z / (p->z - v2->z);
    float x3 = p->x - (p->x - v3->x) * p->z / (p->z - v3->z);
    float y3 = p->y - (p->y - v3->y) * p->z / (p->z - v3->z);


    Point2f_t src[4] = {{0, 0}, {w, 0}, {w, h}, {0, h}};
    Point2f_t dst[4] = {{x0, y0}, {x1, y1}, {x2, y2}, {x3, y3}};
    float *answer = (float *)matrix->m;
    getPerspectiveTransform(src, dst, answer);
}

#if 0
static bool full_rank(struct gui_matrix *m)
{
    for (int i = 0; i < 3; i++)
    {
        if (m->m[i][0] == 0 && m->m[i][1] == 0 && m->m[i][2] == 0)
        {
            return false;
        }

        if ((m->m[0][i] * 10000.0f < 1000.0f && m->m[0][i] * 10000.0f > -1000.0f) &&
            (m->m[1][i] * 10000.0f < 1000.0f && m->m[1][i] * 10000.0f > -1000.0f) &&
            (m->m[2][i] * 10000.0f < 1000.0f && m->m[2][i] * 10000.0f > -1000.0f))
        {

            return false;
        }
    }
    return true;
}
#endif



static void prepare(gui_obj_t *obj)
{
    struct gui_matrix rotate_3D;
    Vertex_t rv0, rv1, rv2, rv3;
    Vertex_t tv0, tv1, tv2, tv3;
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();

    gui_perspective_t *this = (gui_perspective_t *)obj;
    for (int i = 0; i < 6; i++)
    {
        gui_image_load_scale(&this->img[i]);
        matrix_identity(this->img[i].matrix);
    }

    float w = 300;
    float h = 300;
#if USE_FIX_SIN
    float d = w * fix_sin(60);
#else
    float d = w * sin(RAD(60));
#endif
    Vertex_t v0 = {-w / 2 + 10, -h / 2, d};
    Vertex_t v1 = {w  / 2 - 10, -h / 2, d};
    Vertex_t v2 = {w  / 2 - 10, h  / 2, d};
    Vertex_t v3 = {-w / 2 + 10, h  / 2, d};

    scale_3d(&v0, 1.0f);
    scale_3d(&v1, 1.0f);
    scale_3d(&v2, 1.0f);
    scale_3d(&v3, 1.0f);

    for (uint32_t i = 0; i < 6; i++)
    {
        this->ry[i] = i * 60;
    }
    switch (tp->type)
    {
    case TOUCH_HOLD_X:
        this->release_x = tp->deltaX;

        break;
    default:
        break;
    }

    for (uint32_t i = 0; i < 6; i++)
    {
        this->ry[i] += this->release_x;
        this->temp[i] = this->ry[i];
    }

    for (uint32_t i = 0; i < 6 - 1; i++)
    {
        for (uint32_t j = 0; j < 6 - i - 1; j++)
        {
            if (fix_cos(this->temp[j]) > fix_cos(this->temp[j + 1]))
            {
                float t = this->temp[j];
                this->temp[j] = this->temp[j + 1];
                this->temp[j + 1] = t;
            }
        }
    }

    for (uint32_t i = 0; i < 6; i++)
    {

        compute_rotate(0, this->ry[i], 0, &rotate_3D);

        transfrom_rotate(&rotate_3D, &v0, &tv0, 0, 0, 0);
        transfrom_rotate(&rotate_3D, &v1, &tv1, 0, 0, 0);
        transfrom_rotate(&rotate_3D, &v2, &tv2, 0, 0, 0);
        transfrom_rotate(&rotate_3D, &v3, &tv3, 0, 0, 0);

        compute_rotate(0, 0, 0, &rotate_3D);//set here for x-axis

        float xoff = (float)dc->screen_width / 2;
        float yoff = (float)dc->screen_height / 2 + 100;
        float zoff = -2 * d;

        transfrom_rotate(&rotate_3D, &tv0, &rv0, xoff, yoff, zoff);
        transfrom_rotate(&rotate_3D, &tv1, &rv1, xoff, yoff, zoff);
        transfrom_rotate(&rotate_3D, &tv2, &rv2, xoff, yoff, zoff);
        transfrom_rotate(&rotate_3D, &tv3, &rv3, xoff, yoff, zoff);

        Vertex_t p = {(float)(dc->screen_width) / 2, 0, 2 * d};
        transfrom_blit(this->img[i].img_w, this->img[i].img_h, &p, &rv0, &rv1, &rv2, &rv3,
                       this->img[i].matrix);

        memcpy(this->img[i].inverse, this->img[i].matrix, sizeof(struct gui_matrix));
        matrix_inverse(this->img[i].inverse);
        gui_image_new_area(&this->img[i]);
    }

    uint8_t last = this->checksum;
    this->checksum = 0;
    this->checksum = this->release_x;

    if ((last != this->checksum) || (this->release_x == 0))
    {
        gui_fb_change();
    }

}

extern void gui_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);
static void draw_cb(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    gui_perspective_t *this = (gui_perspective_t *)obj;



    for (uint32_t j = 0; j < 6; j++)
    {
        for (uint32_t i = 0; i < 6; i++)
        {
            if (this->temp[j] == this->ry[i])
            {
                draw_img_t *draw_img = &this->img[i];

                gui_rect_t rect = {0};
                rect.x1 = obj->dx + draw_img->img_x;
                rect.y1 = obj->dy + draw_img->img_y;
                rect.x2 = rect.x1 + obj->w - 1;
                rect.y2 = rect.y1 + obj->h - 1;

                gui_acc_blit(draw_img, dc, &rect);
            }
        }
    }

}

static void end(gui_obj_t *obj)
{

}
static void destory(gui_obj_t *obj)
{
    // gui_log("perspective %s \n", __FUNCTION__);
    for (int i = 0; i < 6; i++)
    {
        draw_img_t *draw_img = &((gui_perspective_t *)obj)->img[i];
        gui_free(draw_img->inverse);
        gui_free(draw_img->matrix);
        if (draw_img->src_mode == IMG_SRC_FILESYS)
        {
#ifdef _WIN32
            // free path transforming memory on win
            gui_free(draw_img->data);
#endif
        }
    }
}
extern char *gui_img_filepath_transforming(void *addr);
static void gui_perspective_ctor(gui_perspective_t *this, gui_obj_t *parent, const char *name,
                                 gui_perspective_imgfile_t *img_file,
                                 int16_t x,
                                 int16_t y, int16_t w, int16_t h)
{
    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = VG_LITE_CUBE;
    root->obj_prepare = prepare;
    root->obj_draw = draw_cb;
    root->obj_end = end;
    root->obj_destory = destory;

    //for self
    void **array = (void **)img_file->img_path;
    for (int i = 0; i < 6; i++)
    {
        draw_img_t *draw_img = &this->img[i];
        draw_img->src_mode = img_file->src_mode[i];
        if (img_file->src_mode[i] == IMG_SRC_FILESYS)
        {
            char *path = array[i];
#ifdef _WIN32
            path = gui_img_filepath_transforming(array[i]);
#endif
            draw_img->data = path;
        }
        else if (img_file->src_mode[i] == IMG_SRC_MEMADDR)
        {
            draw_img->data = array[i];
        }
        draw_img->opacity_value = UINT8_MAX;
        draw_img->blend_mode = IMG_SRC_OVER_MODE;
        draw_img->matrix = gui_malloc(sizeof(struct gui_matrix));
        draw_img->inverse = gui_malloc(sizeof(struct gui_matrix));
        this->temp[i] = i * 30;
        this->ry[i] = i * 30;
    }
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_perspective_set_mode(gui_perspective_t *perspective, uint8_t img_index,
                              BLEND_MODE_TYPE mode)
{
    GUI_ASSERT(perspective != NULL);
    draw_img_t *draw_img = &perspective->img[img_index];
    draw_img->blend_mode = mode;
}

void gui_perspective_set_img(gui_perspective_t *perspective, gui_perspective_imgfile_t *img_file)
{
    gui_perspective_t *this = perspective;

    GUI_ASSERT(this != NULL);
    for (int i = 0; i < 6; i++)
    {
        draw_img_t *draw_img = &this->img[i];
        // reset file data
        if (draw_img->src_mode == IMG_SRC_FILESYS)
        {
#ifdef _WIN32
            gui_free(draw_img->data);
#endif
            draw_img->data = NULL;
        }

        // set new images
        draw_img->src_mode = img_file->src_mode[i];
        if (img_file->src_mode[i] == IMG_SRC_FILESYS)
        {
            void *path = (void *)img_file->img_path[i];
#ifdef _WIN32
            path = gui_img_filepath_transforming(path);
#endif
            draw_img->data = path;
        }
        else if (img_file->src_mode[i] == IMG_SRC_MEMADDR)
        {
            draw_img->data = img_file->data_addr[i];
        }
    }
}

gui_perspective_t *gui_perspective_create(void *parent,  const char *name,
                                          gui_perspective_imgfile_t *img_file,
                                          int16_t x,
                                          int16_t y)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_PERS";
    }
    gui_perspective_t *this = gui_malloc(sizeof(gui_perspective_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_perspective_t));

    gui_perspective_ctor(this, (gui_obj_t *)parent, name, img_file, x, y, 0, 0);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }
    GET_BASE(this)->create_done = true;
    return this;
}
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */






