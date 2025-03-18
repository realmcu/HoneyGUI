/**
 * @file lv_custom_tile_slide.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_custom_matrix.h"
#include "math.h"

#if LV_USE_MATRIX

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void swap(float *a, float *b);
static bool Gauss(float A[][9], int equ, int var, float *answer);
static bool getPerspectiveTransform(lv_point2f_t *src, lv_point2f_t *dst, float *ret);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#ifndef M_PI
#define M_PI    ((float)3.14159265358979323846)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_matrix_transfrom_blit(float w, float h, lv_vertex_t *p,
                              lv_vertex_t *v0, lv_vertex_t *v1,
                              lv_vertex_t *v2, lv_vertex_t *v3,
                              lv_matrix_t *matrix)
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


    lv_point2f_t src[4] = {{0, 0}, {w, 0}, {w, h}, {0, h}};
    lv_point2f_t dst[4] = {{x0, y0}, {x1, y1}, {x2, y2}, {x3, y3}};
    v0->x = x0;
    v1->x = x1;
    v2->x = x2;
    v3->x = x3;
    v0->y = y0;
    v1->y = y1;
    v2->y = y2;
    v3->y = y3;
    float *answer = (float *)matrix->m;
    getPerspectiveTransform(src, dst, answer);
}

void lv_matrix_compute_rotate(float degree_x, float degree_y, float degree_z, lv_matrix_t *rotate)
{
    float radian_x = degree_x / 180.0f * (float)M_PI;
    float radian_y = degree_y / 180.0f * (float)M_PI;
    float radian_z = degree_z / 180.0f * (float)M_PI;

    float cos_x = cosf(radian_x);
    float sin_x = sinf(radian_x);
    float cos_y = cosf(radian_y);
    float sin_y = sinf(radian_y);
    float cos_z = cosf(radian_z);
    float sin_z = sinf(radian_z);

    rotate->m[0][0] = cos_z * cos_y;
    rotate->m[0][1] = cos_z * sin_y * sin_x - sin_z * cos_x;
    rotate->m[0][2] = cos_z * sin_y * cos_x + sin_z * sin_x;
    rotate->m[1][0] = sin_z * cos_y;
    rotate->m[1][1] = sin_z * sin_y * sin_x + cos_z * cos_x;
    rotate->m[1][2] = sin_z * sin_y * cos_x - cos_z * sin_x;
    rotate->m[2][0] = -sin_y;
    rotate->m[2][1] = cos_y * sin_x;
    rotate->m[2][2] = cos_y * cos_x;
}

void lv_matrix_transfrom_rotate(lv_matrix_t *rotate, lv_vertex_t *vertex, lv_vertex_t *rc, float tx,
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

/**********************
 *   STATIC FUNCTIONS
 **********************/

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

static bool getPerspectiveTransform(lv_point2f_t *src, lv_point2f_t *dst, float *ret)
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

#endif /*LV_USE_MATRIX*/

