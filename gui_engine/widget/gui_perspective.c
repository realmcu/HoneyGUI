/*
 * File      : gui_cube.c
 * This file is part of GUI Engine
 */
#include <guidef.h>
#include <string.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include <gui_kb.h>
#include <draw_img.h>
#include <gui_matrix.h>
#include "gui_perspective.h"
#include "acc_engine.h"
#include <math.h>

static struct rtgui_matrix rotate_3D;
static Vertex_t rv0, rv1, rv2, rv3;





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

typedef struct Point2f
{
    float x;
    float y;
} Point2f_t;

static void swap(float *a, float *b)
{
    float c = *a;
    *a = *b;
    *b = c;
}

bool Gauss(float A[][9], int equ, int var, float *answer)   //epu:A's row  var:A's col-1
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



bool getPerspectiveTransform(Point2f_t *src, Point2f_t *dst, float *ret)
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
                           struct rtgui_matrix *matrix)
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


#if 0
static bool full_rank(struct rtgui_matrix *m)
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
    return;
}

static void draw_cb(gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    touch_info_t *tp = tp_get_info();
    //tp->deltaX
    //gui_log("%d,tp->deltaX = %d\n", __LINE__, tp->deltaX);



    gui_perspective_t *this = (gui_perspective_t *)obj;
    rtgui_image_load_scale(&this->img);
    matrix_identity(this->img.matrix);

    float w = 200;
    float h = 300;
    float d = w * sin(RAD(60));

    Vertex_t v0 = {-w / 2, -h / 2, d};
    Vertex_t v1 = {w / 2, -h / 2, d};
    Vertex_t v2 = {w / 2, h / 2, d};
    Vertex_t v3 = {-w / 2, h / 2, d};

    scale_3d(&v0, 1.0f);
    scale_3d(&v1, 1.0f);
    scale_3d(&v2, 1.0f);
    scale_3d(&v3, 1.0f);

    int ry[6] = {0, 60, 120, 180, 240, 300};


    switch (tp->type)
    {
    case TOUCH_HOLD_X:
        for (uint32_t i = 0; i < 6; i++)
        {
            ry[i] += tp->deltaX;
            ry[i] += 720;
            ry[i] = ry[i] % 360;
        }
        break;
    default:
        break;
    }


    float rryy[6];
    int k = 0;
    for (uint32_t i = 0; i < 6; i++)
    {
        if ((ry[i] > 90) && (ry[i] < 270))
        {
            rryy[k] = ry[i];
            k++;
        }
    }
    for (uint32_t i = 0; i < 6; i++)
    {
        if ((ry[i] <= 90) || (ry[i] >= 270))
        {
            rryy[k] = ry[i];
            k++;
        }
    }


    for (uint32_t i = 0; i < 6; i++)
    {

        compute_rotate(0, rryy[i], 0, &rotate_3D);

        float xoff = 454 / 2;
        float yoff = 454 / 2;
        float zoff = -454 / 2;


        transfrom_rotate(&rotate_3D, &v0, &rv0, xoff, yoff, zoff);
        transfrom_rotate(&rotate_3D, &v1, &rv1, xoff, yoff, zoff);
        transfrom_rotate(&rotate_3D, &v2, &rv2, xoff, yoff, zoff);
        transfrom_rotate(&rotate_3D, &v3, &rv3, xoff, yoff, zoff);

        Vertex_t p = {454 / 2, 0, 500};
        transfrom_blit(256, 256, &p, &rv0, &rv1, &rv2, &rv3, this->img.matrix);

        memcpy(this->img.inverse, this->img.matrix, sizeof(struct rtgui_matrix));
        matrix_inverse(this->img.inverse);
        get_new_area(&this->img);

        draw_img_t *draw_img = &this->img;
        draw_img->opacity_value = UINT8_MAX;

        rtgui_rect_t rect = {0};
        rect.x1 = obj->dx + draw_img->img_x;
        rect.y1 = obj->dy + draw_img->img_y;
        rect.x2 = rect.x1 + obj->w;
        rect.y2 = rect.y1 + obj->h;

        gui_get_acc()->blit(draw_img, dc, &rect);
    }


    return;

}


static void end(gui_obj_t *obj)
{

}
static void destory(gui_obj_t *obj)
{

}

void gui_perspective_ctor(gui_perspective_t *this, gui_obj_t *parent, const char *name,
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
    root->obj_prepare = prepare;
    root->obj_draw = draw_cb;
    root->obj_end = end;
    root->obj_destory = destory;
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

    this->draw_img_front.blend_mode = IMG_MAGIC_MATRIX;
    this->draw_img_back.blend_mode = IMG_MAGIC_MATRIX;
    this->draw_img_up.blend_mode = IMG_MAGIC_MATRIX;
    this->draw_img_down.blend_mode = IMG_MAGIC_MATRIX;
    this->draw_img_left.blend_mode = IMG_MAGIC_MATRIX;
    this->draw_img_right.blend_mode = IMG_MAGIC_MATRIX;

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

    this->img.data = addr;
    this->img.blend_mode = IMG_MAGIC_MATRIX;
    this->img.matrix = gui_malloc(sizeof(struct rtgui_matrix));
    this->img.inverse = gui_malloc(sizeof(struct rtgui_matrix));

    gui_dispdev_t *dc = gui_get_dc();
    this->cbsize = dc->fb_height / 8.0;
    this->c_x = (dc->fb_width - this->cbsize) / 2.0f;
    this->c_y = (dc->fb_width - this->cbsize) / 2.0f;

}



gui_perspective_t *gui_perspective_create(void *parent,  const char *name, void *data,
                                          int16_t x,
                                          int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_PERS";
    }
    gui_perspective_t *this = gui_malloc(sizeof(gui_perspective_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_perspective_t));

    gui_perspective_ctor(this, (gui_obj_t *)parent, name, data, x, y, w, h);
    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }
    GET_BASE(this)->create_done = true;
    return this;
}



