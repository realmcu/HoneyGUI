/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * File      : gui_matrix.c
 * This file is part of GUI Engine
 */

#include "guidef.h"
#include "gui_matrix.h"
#include "gui_math.h"
#include <string.h>
#include <math.h>

/*============================================================================
 *                          Rectangle Operations
 *============================================================================*/

bool rect_intersect(gui_rect_t *result_rect, gui_rect_t *rect1, gui_rect_t *rect2)
{
    int16_t x1, x2, y1, y2;
    x1 = MATRIX_MAX(rect1->x1, rect2->x1);
    y1 = MATRIX_MAX(rect1->y1, rect2->y1);
    x2 = MATRIX_MIN(rect1->x2, rect2->x2);
    y2 = MATRIX_MIN(rect1->y2, rect2->y2);
    if (x1 > x2 || y1 > y2)
    {
        return false;
    }
    else
    {
        if (result_rect != NULL)
        {
            result_rect->x1 = x1;
            result_rect->x2 = x2;
            result_rect->y1 = y1;
            result_rect->y2 = y2;
        }
        return true;
    }
}

void rect_move(gui_rect_t *out, gui_rect_t *in, int16_t x, int16_t y)
{
    out->x1 = in->x1 + x;
    out->x2 = in->x2 + x;
    out->y1 = in->y1 + y;
    out->y2 = in->y2 + y;
}

/*============================================================================
 *                      Gaussian Elimination Solver
 *============================================================================*/

static void swap(float *a, float *b)
{
    float c = *a;
    *a = *b;
    *b = c;
}

/*
 * Gaussian elimination solver for linear system Ax = b
 * A: coefficient matrix [equ x var+1], last column is b
 * equ: number of equations (rows)
 * var: number of variables (columns - 1)
 * answer: output solution vector
 */
static bool Gauss(float A[][9], int equ, int var, float *answer)
{
    int row, col;
    for (row = 0, col = 0; col < var && row < equ; col++, row++)
    {
        int max_r = row;
        for (int i = row + 1; i < equ; i++)
        {
            if (fabsf(A[i][col]) > fabsf(A[max_r][col]))
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

        if (fabsf(A[row][col]) < (1e-6f))
        {
            return false;
        }

        for (int i = row + 1; i < equ; i++)
        {
            if (fabsf(A[i][col]) < (1e-6f))
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

/*
 * Calculate perspective transformation coefficients
 * Maps (xi,yi) to (ui,vi), i=1,2,3,4:
 *
 *      c00*xi + c01*yi + c02
 * ui = ---------------------
 *      c20*xi + c21*yi + c22
 *
 *      c10*xi + c11*yi + c12
 * vi = ---------------------
 *      c20*xi + c21*yi + c22
 *
 * Coefficients calculated by solving linear system:
 * / x0 y0  1  0  0  0 -x0*u0 -y0*u0 \ /c00\ /u0\
 * | x1 y1  1  0  0  0 -x1*u1 -y1*u1 | |c01| |u1|
 * | x2 y2  1  0  0  0 -x2*u2 -y2*u2 | |c02| |u2|
 * | x3 y3  1  0  0  0 -x3*u3 -y3*u3 |.|c10|=|u3|,
 * |  0  0  0 x0 y0  1 -x0*v0 -y0*v0 | |c11| |v0|
 * |  0  0  0 x1 y1  1 -x1*v1 -y1*v1 | |c12| |v1|
 * |  0  0  0 x2 y2  1 -x2*v2 -y2*v2 | |c20| |v2|
 * \  0  0  0 x3 y3  1 -x3*v3 -y3*v3 / \c21/ \v3/
 *
 * where: cij - matrix coefficients, c22 = 1
 */
static bool getPerspectiveTransform(gui_point2f_t *src, gui_point2f_t *dst, float *ret)
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

/*============================================================================
 *                      Perspective Transform Operations
 *============================================================================*/

/*
 * Compute 3x3 image transform matrix to map a rectangle image (w,h) to
 * a parallelogram (x0,y0), (x1,y1), (x2,y2), (x3,y3) counterclockwise.
 * w: image width
 * h: image height
 * p: perspective point
 * v0-v3: four corner vertices
 * matrix: output transformation matrix
 */
void matrix_transfrom_blit(float w, float h, gui_vertex_t *p, gui_vertex_t *v0, gui_vertex_t *v1,
                           gui_vertex_t *v2, gui_vertex_t *v3, struct gui_matrix *matrix)
{
    float x0 = p->x - (p->x - v0->x) * p->z / (p->z - v0->z);
    float y0 = p->y - (p->y - v0->y) * p->z / (p->z - v0->z);
    float x1 = p->x - (p->x - v1->x) * p->z / (p->z - v1->z);
    float y1 = p->y - (p->y - v1->y) * p->z / (p->z - v1->z);
    float x2 = p->x - (p->x - v2->x) * p->z / (p->z - v2->z);
    float y2 = p->y - (p->y - v2->y) * p->z / (p->z - v2->z);
    float x3 = p->x - (p->x - v3->x) * p->z / (p->z - v3->z);
    float y3 = p->y - (p->y - v3->y) * p->z / (p->z - v3->z);

    gui_point2f_t src[4] = {{0, 0}, {w, 0}, {w, h}, {0, h}};
    gui_point2f_t dst[4] = {{x0, y0}, {x1, y1}, {x2, y2}, {x3, y3}};

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

/*============================================================================
 *                      Matrix Basic Operations
 *============================================================================*/

bool matrix_is_identity(struct gui_matrix *matrix)
{
    if (matrix == NULL)
    {
        return false;
    }

    return (matrix->m[0][0] == 1.0f && matrix->m[0][1] == 0.0f && matrix->m[0][2] == 0.0f &&
            matrix->m[1][0] == 0.0f && matrix->m[1][1] == 1.0f && matrix->m[1][2] == 0.0f &&
            matrix->m[2][0] == 0.0f && matrix->m[2][1] == 0.0f && matrix->m[2][2] == 1.0f);
}

bool matrix_only_translate(struct gui_matrix *matrix)
{
    if (matrix == NULL)
    {
        return false;
    }

    return (matrix->m[0][0] == 1.0f && matrix->m[0][1] == 0.0f &&
            matrix->m[1][0] == 0.0f && matrix->m[1][1] == 1.0f &&
            matrix->m[2][0] == 0.0f && matrix->m[2][1] == 0.0f && matrix->m[2][2] == 1.0f);
}

void matrix_identity(struct gui_matrix *matrix)
{
    if (matrix == NULL)
    {
        return;
    }

    matrix->m[0][0] = 1.0f;
    matrix->m[0][1] = 0.0f;
    matrix->m[0][2] = 0.0f;
    matrix->m[1][0] = 0.0f;
    matrix->m[1][1] = 1.0f;
    matrix->m[1][2] = 0.0f;
    matrix->m[2][0] = 0.0f;
    matrix->m[2][1] = 0.0f;
    matrix->m[2][2] = 1.0f;
}

/*============================================================================
 *                      Matrix Multiplication Operations
 *============================================================================*/

void matrix_multiply_normal(struct gui_matrix *matrix, gui_vertex_t *normal)
{
    float m00 = matrix->m[0][0];
    float m01 = matrix->m[0][1];
    float m02 = matrix->m[0][2];
    float m10 = matrix->m[1][0];
    float m11 = matrix->m[1][1];
    float m12 = matrix->m[1][2];
    float m20 = matrix->m[2][0];
    float m21 = matrix->m[2][1];
    float m22 = matrix->m[2][2];

    float x = normal->x;
    float y = normal->y;
    float z = normal->z;

    normal->x = (m00 * x) + (m01 * y) + (m02 * z);
    normal->y = (m10 * x) + (m11 * y) + (m12 * z);
    normal->z = (m20 * x) + (m21 * y) + (m22 * z);
}

void matrix_multiply(struct gui_matrix *matrix, struct gui_matrix *mult)
{
    float m00 = matrix->m[0][0];
    float m01 = matrix->m[0][1];
    float m02 = matrix->m[0][2];
    float m10 = matrix->m[1][0];
    float m11 = matrix->m[1][1];
    float m12 = matrix->m[1][2];
    float m20 = matrix->m[2][0];
    float m21 = matrix->m[2][1];
    float m22 = matrix->m[2][2];

    float t00 = mult->m[0][0];
    float t01 = mult->m[0][1];
    float t02 = mult->m[0][2];
    float t10 = mult->m[1][0];
    float t11 = mult->m[1][1];
    float t12 = mult->m[1][2];
    float t20 = mult->m[2][0];
    float t21 = mult->m[2][1];
    float t22 = mult->m[2][2];

    matrix->m[0][0] = (m00 * t00) + (m01 * t10) + (m02 * t20);
    matrix->m[0][1] = (m00 * t01) + (m01 * t11) + (m02 * t21);
    matrix->m[0][2] = (m00 * t02) + (m01 * t12) + (m02 * t22);
    matrix->m[1][0] = (m10 * t00) + (m11 * t10) + (m12 * t20);
    matrix->m[1][1] = (m10 * t01) + (m11 * t11) + (m12 * t21);
    matrix->m[1][2] = (m10 * t02) + (m11 * t12) + (m12 * t22);
    matrix->m[2][0] = (m20 * t00) + (m21 * t10) + (m22 * t20);
    matrix->m[2][1] = (m20 * t01) + (m21 * t11) + (m22 * t21);
    matrix->m[2][2] = (m20 * t02) + (m21 * t12) + (m22 * t22);
}

void matrix_multiply_point(struct gui_matrix *matrix, struct gui_point3f *pox)
{
    float a = pox->x;
    float b = pox->y;
    float c = pox->z;

    pox->x = (matrix->m[0][0] * a) + (matrix->m[0][1] * b) + (matrix->m[0][2] * c);
    pox->y = (matrix->m[1][0] * a) + (matrix->m[1][1] * b) + (matrix->m[1][2] * c);
    pox->z = (matrix->m[2][0] * a) + (matrix->m[2][1] * b) + (matrix->m[2][2] * c);

    pox->x = pox->x / pox->z;
    pox->y = pox->y / pox->z;
    pox->z = 1;
}

void matrix_inverse(struct gui_matrix *matrix)
{
    float m00 = matrix->m[0][0];
    float m01 = matrix->m[0][1];
    float m02 = matrix->m[0][2];
    float m10 = matrix->m[1][0];
    float m11 = matrix->m[1][1];
    float m12 = matrix->m[1][2];
    float m20 = matrix->m[2][0];
    float m21 = matrix->m[2][1];
    float m22 = matrix->m[2][2];

    float detal = m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 -
                  m00 * m12 * m21 - m01 * m10 * m22 - m02 * m11 * m20;

    matrix->m[0][0] = (m11 * m22 - m12 * m21) / detal;
    matrix->m[1][0] = -(m22 * m10 - m12 * m20) / detal;
    matrix->m[2][0] = (m10 * m21 - m20 * m11) / detal;
    matrix->m[0][1] = -(m01 * m22 - m02 * m21) / detal;
    matrix->m[1][1] = (m22 * m00 - m20 * m02) / detal;
    matrix->m[2][1] = -(m00 * m21 - m20 * m01) / detal;
    matrix->m[0][2] = (m01 * m12 - m11 * m02) / detal;
    matrix->m[1][2] = -(m00 * m12 - m02 * m10) / detal;
    matrix->m[2][2] = (m00 * m11 - m01 * m10) / detal;
}

/*============================================================================
 *                      3D Rotation Matrix Computation
 *============================================================================*/

void matrix_compute_rotate(float rx, float ry, float rz, struct gui_matrix *rotate)
{
    int frz = roundf(rz);
    int frx = roundf(rx);
    int fry = roundf(ry);

    float fxcos = fix_cos(frx);
    float fxsin = fix_sin(frx);
    float fycos = fix_cos(fry);
    float fysin = fix_sin(fry);
    float fzcos = fix_cos(frz);
    float fzsin = fix_sin(frz);

    rotate->m[0][0] = fzcos * fycos;
    rotate->m[0][1] = fzcos * fysin * fxsin - fzsin * fxcos;
    rotate->m[0][2] = fzcos * fysin * fxcos + fzsin * fxsin;
    rotate->m[1][0] = fzsin * fycos;
    rotate->m[1][1] = fzsin * fysin * fxsin + fzcos * fxcos;
    rotate->m[1][2] = fzsin * fysin * fxcos - fzcos * fxsin;
    rotate->m[2][0] = -fysin;
    rotate->m[2][1] = fycos * fxsin;
    rotate->m[2][2] = fycos * fxcos;
}

void matrix_transfrom_rotate(struct gui_matrix *rotate, gui_vertex_t *vertex, gui_vertex_t *rc,
                             float tx, float ty, float tz)
{
    rc->x = rotate->m[0][0] * vertex->x + rotate->m[0][1] * vertex->y + rotate->m[0][2] * vertex->z;
    rc->y = rotate->m[1][0] * vertex->x + rotate->m[1][1] * vertex->y + rotate->m[1][2] * vertex->z;
    rc->z = rotate->m[2][0] * vertex->x + rotate->m[2][1] * vertex->y + rotate->m[2][2] * vertex->z;

    rc->x += tx;
    rc->y += ty;
    rc->z += tz;
}

/*============================================================================
 *                      Matrix Transformation Operations
 *============================================================================*/

void matrix_translate(float x, float y, struct gui_matrix *matrix)
{
    if ((x == 0) && (y == 0))
    {
        return;
    }

    struct gui_matrix t =
    {
        {
            {1.0f, 0.0f, x},
            {0.0f, 1.0f, y},
            {0.0f, 0.0f, 1.0f}
        }
    };

    matrix_multiply(matrix, &t);
}

void matrix_rotate(float degrees, struct gui_matrix *matrix)
{
    if (degrees == 0)
    {
        return;
    }

    float angle = degrees / 180.0f * 3.1415926f;
    float cos_angle = cosf(angle);
    float sin_angle = sinf(angle);

    struct gui_matrix r =
    {
        {
            {cos_angle, -sin_angle, 0.0f},
            {sin_angle, cos_angle, 0.0f},
            {0.0f, 0.0f, 1.0f}
        }
    };

    matrix_multiply(matrix, &r);
}

void matrix_scale(float scale_x, float scale_y, struct gui_matrix *matrix)
{
    if ((scale_x == 1.0f) && (scale_y == 1.0f))
    {
        return;
    }

    struct gui_matrix s =
    {
        {
            {scale_x, 0.0f, 0.0f},
            {0.0f, scale_y, 0.0f},
            {0.0f, 0.0f, 1.0f}
        }
    };

    matrix_multiply(matrix, &s);
}


/*============================================================================
 *                      Matrix Utility Functions
 *============================================================================*/

bool matrix_is_full_rank(struct gui_matrix *m)
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

bool matrix_get_transform_area(gui_matrix_t *matrix, gui_rect_t *input_area,
                               gui_rect_t *result_area)
{
    gui_point3f_t pox = {0};
    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;

    float x1 = input_area->x1;
    float y1 = input_area->y1;
    float x2 = input_area->x2;
    float y2 = input_area->y2;

    pox.x = x1;
    pox.y = y1;
    pox.z = 1.0f;
    matrix_multiply_point(matrix, &pox);
    x_min = pox.x;
    x_max = pox.x;
    y_min = pox.y;
    y_max = pox.y;

    pox.x = x2;
    pox.y = y1;
    pox.z = 1.0f;
    matrix_multiply_point(matrix, &pox);
    if (x_min > pox.x)
    {
        x_min = pox.x;
    }
    if (x_max < pox.x)
    {
        x_max = pox.x;
    }
    if (y_min > pox.y)
    {
        y_min = pox.y;
    }
    if (y_max < pox.y)
    {
        y_max = pox.y;
    }

    pox.x = x2;
    pox.y = y2;
    pox.z = 1.0f;
    matrix_multiply_point(matrix, &pox);
    if (x_min > pox.x)
    {
        x_min = pox.x;
    }
    if (x_max < pox.x)
    {
        x_max = pox.x;
    }
    if (y_min > pox.y)
    {
        y_min = pox.y;
    }
    if (y_max < pox.y)
    {
        y_max = pox.y;
    }

    pox.x = x1;
    pox.y = y2;
    pox.z = 1.0f;
    matrix_multiply_point(matrix, &pox);
    if (x_min > pox.x)
    {
        x_min = pox.x;
    }
    if (x_max < pox.x)
    {
        x_max = pox.x;
    }
    if (y_min > pox.y)
    {
        y_min = pox.y;
    }
    if (y_max < pox.y)
    {
        y_max = pox.y;
    }

    result_area->x1 = (int16_t)x_min;
    result_area->y1 = (int16_t)y_min;
    result_area->x2 = ceilf(x_max);
    result_area->y2 = ceilf(y_max);
    return true;
}

