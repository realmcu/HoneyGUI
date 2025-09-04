/*
 * File      : gui_img.c
 * This file is part of GUI Engine
 */

#include <guidef.h>
#include <string.h>
#include <gui_matrix.h>
#include <math.h>

#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)

#define USE_FIX_SIN 1

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

bool rect_intersect(gui_rect_t *result_rect, gui_rect_t *rect1, gui_rect_t *rect2)
{
    int16_t x1, x2, y1, y2;
    x1 = MAX(rect1->x1, rect2->x1);
    y1 = MAX(rect1->y1, rect2->y1);
    x2 = MIN(rect1->x2, rect2->x2);
    y2 = MIN(rect1->y2, rect2->y2);
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

float fix_sin(int angle)
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

float fix_cos(int angle)
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
            //row--;
            //continue;
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

// w means image width
// h means image height, use w and h can get four point in XY plane

// p means the perspective point,  use this parameter can get four point in XY plane
void matrix_transfrom_blit(float w, float h, gui_vertex_t *p, gui_vertex_t *v0, gui_vertex_t *v1,
                           gui_vertex_t *v2,
                           gui_vertex_t *v3, struct gui_matrix *matrix)
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

bool matrix_is_identity(struct gui_matrix *matrix)
{
    if (matrix == NULL)
    {
        return false;
    }

    if (
        matrix->m[0][0] == 1.0f && \
        matrix->m[0][1] == 0.0f && \
        matrix->m[0][2] == 0.0f && \
        matrix->m[1][0] == 0.0f && \
        matrix->m[1][1] == 1.0f && \
        matrix->m[1][2] == 0.0f && \
        matrix->m[2][0] == 0.0f && \
        matrix->m[2][1] == 0.0f && \
        matrix->m[2][2] == 1.0f
    )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool matrix_only_translate(struct gui_matrix *matrix)
{
    if (matrix == NULL)
    {
        return false;
    }

    if (
        matrix->m[0][0] == 1.0f && \
        matrix->m[0][1] == 0.0f && \
        // matrix->m[0][2] == 0.0f &&
        matrix->m[1][0] == 0.0f && \
        matrix->m[1][1] == 1.0f && \
        // matrix->m[1][2] == 0.0f &&
        matrix->m[2][0] == 0.0f && \
        matrix->m[2][1] == 0.0f && \
        matrix->m[2][2] == 1.0f
    )
    {
        return true;
    }
    else
    {
        return false;
    }
}
void matrix_identity(struct gui_matrix *matrix)
{
    if (matrix == NULL)
    {
        return;
    }
    /* Set identify matrix. */
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
#if 0
void matrix_multiply_normal(struct gui_matrix *matrix,
                            gui_vertex_t *normal)
{
    gui_vertex_t temp;

    /* Process all rows. */

    temp.x = (matrix->m[0][0] * normal->x) + (matrix->m[0][1] * normal->y) +
             (matrix->m[0][2] * normal->z);
    temp.y = (matrix->m[1][0] * normal->x) + (matrix->m[1][1] * normal->y) +
             (matrix->m[1][2] * normal->z);
    temp.z = (matrix->m[2][0] * normal->x) + (matrix->m[2][1] * normal->y) +
             (matrix->m[2][2] * normal->z);

    memcpy(normal, &temp, sizeof(temp));
}
void matrix_multiply(struct gui_matrix *matrix, struct gui_matrix *mult)
{
    int row, column;

    /* Process all rows. */
    for (row = 0; row < 3; row++)
    {
        float t1, t2, t3;
        t1 = matrix->m[row][0];
        t2 = matrix->m[row][1];
        t3 = matrix->m[row][2];
        /* Process all columns. */
        for (column = 0; column < 3; column++)
        {
            /* Compute matrix entry. */
            matrix->m[row][column] = (t1 * mult->m[0][column])
                                     + (t2 * mult->m[1][column])
                                     + (t3 * mult->m[2][column]);
        }
    }

}

void matrix_multiply_point(struct gui_matrix *matrix, struct gui_point3f_t *pox)
{
    struct gui_point3f_t temp;
    int row;

    /* Process all rows. */
    for (row = 0; row < 3; row++)
    {
        temp.p[row] = (matrix->m[row][0] * pox->p[0])
                      + (matrix->m[row][1] * pox->p[1])
                      + (matrix->m[row][2] * pox->p[2]);
    }
    temp.p[0] = temp.p[0] / temp.p[2];
    temp.p[1] = temp.p[1] / temp.p[2];
    temp.p[2] = 1;

    memcpy(pox, &temp, sizeof(temp));
}
void matrix_inverse(struct gui_matrix *matrix)
{
    struct gui_matrix temp;

    float detal = matrix->m[0][0] * matrix->m[1][1] * matrix->m[2][2] + \
                  matrix->m[0][1] * matrix->m[1][2] * matrix->m[2][0] + \
                  matrix->m[0][2] * matrix->m[1][0] * matrix->m[2][1] - \
                  matrix->m[0][0] * matrix->m[1][2] * matrix->m[2][1] - \
                  matrix->m[0][1] * matrix->m[1][0] * matrix->m[2][2] - \
                  matrix->m[0][2] * matrix->m[1][1] * matrix->m[2][0];

    temp.m[0][0] = (matrix->m[1][1] * matrix->m[2][2] - matrix->m[1][2] * matrix->m[2][1]) / detal;
    temp.m[1][0] = -(matrix->m[2][2] * matrix->m[1][0] - matrix->m[1][2] * matrix->m[2][0]) / detal;
    temp.m[2][0] = (matrix->m[1][0] * matrix->m[2][1] - matrix->m[2][0] * matrix->m[1][1]) / detal;

    temp.m[0][1] = -(matrix->m[0][1] * matrix->m[2][2] - matrix->m[0][2] * matrix->m[2][1]) / detal;
    temp.m[1][1] = (matrix->m[2][2] * matrix->m[0][0] - matrix->m[2][0] * matrix->m[0][2]) / detal;
    temp.m[2][1] = -(matrix->m[0][0] * matrix->m[2][1] - matrix->m[2][0] * matrix->m[0][1]) / detal;

    temp.m[0][2] = (matrix->m[0][1] * matrix->m[1][2] - matrix->m[1][1] * matrix->m[0][2]) / detal;
    temp.m[1][2] = -(matrix->m[0][0] * matrix->m[1][2] - matrix->m[0][2] * matrix->m[1][0]) / detal;
    temp.m[2][2] = (matrix->m[0][0] * matrix->m[1][1] - matrix->m[0][1] * matrix->m[1][0]) / detal;


    /* Copy temporary matrix into result. */
    memcpy(matrix, &temp, sizeof(temp));
}

void matrix_compute_rotate(float rx, float ry, float rz, struct gui_matrix *rotate)
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
#else
void matrix_multiply_normal(struct gui_matrix *matrix, gui_vertex_t *normal)
{
    //gui_vertex_t temp;

    float m00, m01, m02, m10, m11, m12, m20, m21, m22, x, y, z;
    m00 = matrix->m[0][0];
    m01 = matrix->m[0][1];
    m02 = matrix->m[0][2];

    m10 = matrix->m[1][0];
    m11 = matrix->m[1][1];
    m12 = matrix->m[1][2];

    m20 = matrix->m[2][0];
    m21 = matrix->m[2][1];
    m22 = matrix->m[2][2];

    x = normal->x;
    y = normal->y;
    z = normal->z;
    /* Process all rows. */

    normal->x = (m00 * x) + (m01 * y) + (m02 * z);
    normal->y = (m10 * x) + (m11 * y) + (m12 * z);
    normal->z = (m20 * x) + (m21 * y) + (m22 * z);

}
void matrix_multiply(struct gui_matrix *matrix, struct gui_matrix *mult)
{
    //int row, column;

    float m00, m01, m02, m10, m11, m12, m20, m21, m22;
    m00 = matrix->m[0][0];
    m01 = matrix->m[0][1];
    m02 = matrix->m[0][2];

    m10 = matrix->m[1][0];
    m11 = matrix->m[1][1];
    m12 = matrix->m[1][2];

    m20 = matrix->m[2][0];
    m21 = matrix->m[2][1];
    m22 = matrix->m[2][2];

    float t00, t01, t02, t10, t11, t12, t20, t21, t22;
    t00 = mult->m[0][0];
    t01 = mult->m[0][1];
    t02 = mult->m[0][2];

    t10 = mult->m[1][0];
    t11 = mult->m[1][1];
    t12 = mult->m[1][2];

    t20 = mult->m[2][0];
    t21 = mult->m[2][1];
    t22 = mult->m[2][2];
    /* Compute matrix entry. */
    //row = 0;
    matrix->m[0][0] = (m00 * t00) + (m01 * t10) + (m02 * t20);
    matrix->m[0][1] = (m00 * t01) + (m01 * t11) + (m02 * t21);
    matrix->m[0][2] = (m00 * t02) + (m01 * t12) + (m02 * t22);
    //row = 1;
    matrix->m[1][0] = (m10 * t00) + (m11 * t10) + (m12 * t20);
    matrix->m[1][1] = (m10 * t01) + (m11 * t11) + (m12 * t21);
    matrix->m[1][2] = (m10 * t02) + (m11 * t12) + (m12 * t22);
    ///row = 2;
    matrix->m[2][0] = (m20 * t00) + (m21 * t10) + (m22 * t20);
    matrix->m[2][1] = (m20 * t01) + (m21 * t11) + (m22 * t21);
    matrix->m[2][2] = (m20 * t02) + (m21 * t12) + (m22 * t22);
}
void matrix_multiply_point(struct gui_matrix *matrix, struct gui_point3f *pox)
{
    //struct gui_point3f_t temp;
    //int row;
    float m_row0, m_row1, m_row2;

    float a = pox->p[0];
    float b = pox->p[1];
    float c = pox->p[2];

    /* Process all rows. */
    m_row0 = matrix->m[0][0];
    m_row1 = matrix->m[0][1];
    m_row2 = matrix->m[0][2];
    pox->p[0] = (m_row0 * a) + (m_row1 * b) + (m_row2 * c);

    m_row0 = matrix->m[1][0];
    m_row1 = matrix->m[1][1];
    m_row2 = matrix->m[1][2];
    pox->p[1] = (m_row0 * a) + (m_row1 * b) + (m_row2 * c);

    m_row0 = matrix->m[2][0];
    m_row1 = matrix->m[2][1];
    m_row2 = matrix->m[2][2];
    pox->p[2] = (m_row0 * a) + (m_row1 * b) + (m_row2 * c);

    pox->p[0] = pox->p[0] / pox->p[2];
    pox->p[1] = pox->p[1] / pox->p[2];
    pox->p[2] = 1;

    //memcpy(pox, &temp, sizeof(temp));
}
void matrix_inverse(struct gui_matrix *matrix)
{
    //struct gui_matrix temp;

    float m00, m01, m02, m10, m11, m12, m20, m21, m22;
    m00 = matrix->m[0][0];
    m01 = matrix->m[0][1];
    m02 = matrix->m[0][2];

    m10 = matrix->m[1][0];
    m11 = matrix->m[1][1];
    m12 = matrix->m[1][2];

    m20 = matrix->m[2][0];
    m21 = matrix->m[2][1];
    m22 = matrix->m[2][2];

    float detal = m00 * m11 * m22 + \
                  m01 * m12 * m20 + \
                  m02 * m10 * m21 - \
                  m00 * m12 * m21 - \
                  m01 * m10 * m22 - \
                  m02 * m11 * m20;

    matrix->m[0][0] = (m11 * m22 - m12 * m21) / detal;
    matrix->m[1][0] = -(m22 * m10 - m12 * m20) / detal;
    matrix->m[2][0] = (m10 * m21 - m20 * m11) / detal;

    matrix->m[0][1] = -(m01 * m22 - m02 * m21) / detal;
    matrix->m[1][1] = (m22 * m00 - m20 * m02) / detal;
    matrix->m[2][1] = -(m00 * m21 - m20 * m01) / detal;

    matrix->m[0][2] = (m01 * m12 - m11 * m02) / detal;
    matrix->m[1][2] = -(m00 * m12 - m02 * m10) / detal;
    matrix->m[2][2] = (m00 * m11 - m01 * m10) / detal;


    /* Copy temporary matrix into result. */
    //memcpy(matrix, &temp, sizeof(temp));
}

void matrix_compute_rotate(float rx, float ry, float rz, struct gui_matrix *rotate)
{
    // Rotation angles rx, ry, rz (degree) for axis X, Y, Z
    // Compute 3D rotation matrix base on rotation angle rx, ry, rz about axis X, Y, Z.
    //
#if USE_FIX_SIN
    int frz = roundf(rz), frx = roundf(rx), fry = roundf(ry);
    float fxcos = fix_cos((int)(frx));
    float fxsin = fix_sin((int)(frx));
    float fycos = fix_cos((int)(fry));
    float fysin = fix_sin((int)(fry));
    float fzcos = fix_cos((int)(frz));
    float fzsin = fix_sin((int)(frz));

    rotate->m[0][0] = fzcos * fycos;
    rotate->m[0][1] = fzcos * fysin * fxsin - fzsin * fxcos;
    rotate->m[0][2] = fzcos * fysin * fxcos + fzsin * fxsin;
    rotate->m[1][0] = fzsin * fycos;
    rotate->m[1][1] = fzsin * fysin * fxsin + fzcos * fxcos;
    rotate->m[1][2] = fzsin * fysin * fxcos - fzcos * fxsin;
    rotate->m[2][0] = -fysin;
    rotate->m[2][1] = fycos * fxsin;
    rotate->m[2][2] = fycos * fxcos;
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
#endif


void matrix_translate(float x, float y, struct gui_matrix *matrix)
{
    if ((x == 0) && (y == 0))
    {
        return;
    }
    /* Set translation matrix. */
    struct gui_matrix t = { { {1.0f, 0.0f, x},
            {0.0f, 1.0f, y},
            {0.0f, 0.0f, 1.0f}
        }
    };

    /* Multiply with current matrix. */
    matrix_multiply(matrix, &t);
}

void matrix_rotate(float degrees, struct gui_matrix *matrix)
{
    if (degrees == 0)
    {
        return;
    }
    /* Convert degrees into radians. */
    float angle = degrees / 180.0f * 3.1415926f;

    /* Compuet cosine and sine values. */
    float cos_angle = cosf(angle);
    float sin_angle = sinf(angle);

    /* Set rotation matrix. */
    struct gui_matrix r = { { {cos_angle, -sin_angle, 0.0f},
            {sin_angle, cos_angle, 0.0f},
            {0.0f, 0.0f, 1.0f}
        }
    };

    /* Multiply with current matrix. */
    matrix_multiply(matrix, &r);
}



void matrix_scale(float scale_x, float scale_y, struct gui_matrix *matrix)
{
    if ((scale_x == 1.0f) && (scale_y == 1.0f))
    {
        return;
    }
    /* Set scale matrix. */
    struct gui_matrix s = { { {scale_x, 0.0f, 0.0f},
            {0.0f, scale_y, 0.0f},
            {0.0f, 0.0f, 1.0f}
        }
    };

    /* Multiply with current matrix. */
    matrix_multiply(matrix, &s);
}

void matrix_perspective(float px, float py, struct gui_matrix *matrix)
{
    /* set prespective matrix */
    struct gui_matrix p = { { {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {px, py, 1.0f}
        }
    };
    /* Multiply with current matrix. */
    matrix_multiply(matrix, &p);
}


void matrix_transfrom_rotate(struct gui_matrix *rotate, gui_vertex_t *vertex, gui_vertex_t *rc,
                             float tx,
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
    gui_point3f_t pox = {{0.0f}};
    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;

    float x1 = input_area->x1;
    float y1 = input_area->y1;
    float x2 = input_area->x2;
    float y2 = input_area->y2;

    pox.p[0] = x1;
    pox.p[1] = y1;
    pox.p[2] = 1.0f;
    matrix_multiply_point(matrix, &pox);
    x_min = pox.p[0];
    x_max = pox.p[0];
    y_min = pox.p[1];
    y_max = pox.p[1];

    pox.p[0] = x2;
    pox.p[1] = y1;
    pox.p[2] = 1.0f;
    matrix_multiply_point(matrix, &pox);
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

    pox.p[0] = x2;
    pox.p[1] = y2;
    pox.p[2] = 1.0f;
    matrix_multiply_point(matrix, &pox);
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

    pox.p[0] = x1;
    pox.p[1] = y2;
    pox.p[2] = 1.0f;
    matrix_multiply_point(matrix, &pox);
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

    result_area->x1 = (int16_t)x_min;
    result_area->y1 = (int16_t)y_min;
    result_area->x2 = ceilf(x_max);
    result_area->y2 = ceilf(y_max);
    return true;
}

