/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file def_3d_common.c
  * @brief 3d common calculation for 3d widget
  * @details 3d common calculation for 3d widget
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/7
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "l3_common.h"

#define GUI_ENABLE_MVE      0

#if defined(__ARM_FEATURE_MVE) && defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#if GUI_ENABLE_MVE
#define GUI_3D_USE_MVE
#include <arm_mve.h>
#endif
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
static void l3_swap(float *a, float *b)
{
    float c = *a;
    *a = *b;
    *b = c;
}


static bool l3_gauss(float A[][9], int equ, int var,
                     float *answer)   //epu:A's row  var:A's col-1
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
                l3_swap(&A[row][j], &A[max_r][j]);
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




bool l3_generate_3x3_matrix(l3_2d_point_t *src, l3_2d_point_t *dst, float *ret)
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
    if (l3_gauss(A, 8, 8, ret) == true)
    {
        *(ret + 8) = 1;
        return true;
    }
    return false;

}

#if 1
// Set the intial cube rotation degree and step.
static int16_t sin_table[] =
{
    0,     572,   1144,  1715,  2286,  2856,  3425,  3993,  4560,  5126,  5690,  6252,  6813,  7371,  7927,  8481,
    9032,  9580,  10126, 10668, 11207, 11743, 12275, 12803, 13328, 13848, 14364, 14876, 15383, 15886, 16383, 16876,
    17364, 17846, 18323, 18794, 19260, 19720, 20173, 20621, 21062, 21497, 21925, 22347, 22762, 23170, 23571, 23964,
    24351, 24730, 25101, 25465, 25821, 26169, 26509, 26841, 27165, 27481, 27788, 28087, 28377, 28659, 28932, 29196,
    29451, 29697, 29934, 30162, 30381, 30591, 30791, 30982, 31163, 31335, 31498, 31650, 31794, 31927, 32051, 32165,
    32269, 32364, 32448, 32523, 32587, 32642, 32687, 32722, 32747, 32762, 32767
};

static float l3_sin(int angle)
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

static float l3_cos(int angle)
{
    return l3_sin(angle + 90);
}
#endif

l3_4d_point_t l3_4d_point(float x, float y, float z)
{
    l3_4d_point_t p;
    p.x = x;
    p.y = y;
    p.z = z;
    p.w = 1;
    return p;
}

float l3_4d_point_dot(l3_4d_point_t p1, l3_4d_point_t p2)
{
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

l3_4d_vector_t l3_4d_vector_unit(l3_4d_vector_t v)
{
    l3_4d_vector_t vec;
    if (v.ux || v.uy || v.uz)
    {
        float square = sqrtf(v.ux * v.ux + v.uy * v.uy + v.uz * v.uz);
        vec.ux = v.ux / square;
        vec.uy = v.uy / square;
        vec.uz = v.uz / square;
        vec.uw = 1.0f;
        return vec;
    }
    return v;
}
l3_4d_vector_t l3_4d_vector_cross(l3_4d_vector_t v1, l3_4d_vector_t v2)
{
    l3_4d_vector_t vec;
    vec.ux = v1.uy * v2.uz - v2.uy * v1.uz;
    vec.uy = v1.uz * v2.ux - v2.uz * v1.ux;
    vec.uz = v1.ux * v2.uy - v2.ux * v1.uy;
    vec.uw = 1.0f;
    return vec;
}

l3_4d_vector_t l3_4d_point_sub(l3_4d_point_t p1, l3_4d_point_t p2)
{
    l3_4d_vector_t v;
    v.ux = p1.x - p2.x;
    v.uy = p1.y - p2.y;
    v.uz = p1.z - p2.z;
    v.uw = 1;
    return v;
}


static l3_3x3_matrix_t identity_3x3_matrix =
{
    .u.m =
    {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}
    }
};

static l3_4x4_matrix_t identity_4x4_matrix =
{
    .u.m =
    {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }
};

void l3_3x3_matrix_inverse(l3_3x3_matrix_t *m)
{
    float det = m->u.e._11 * (m->u.e._22 * m->u.e._33 - m->u.e._23 * m->u.e._32) -
                m->u.e._12 * (m->u.e._21 * m->u.e._33 - m->u.e._23 * m->u.e._31) +
                m->u.e._13 * (m->u.e._21 * m->u.e._32 - m->u.e._22 * m->u.e._31);

    if (det == 0)
    {
        return;
    }

    float inv_det = 1.0f / det;

    l3_3x3_matrix_t inv;
    inv.u.e._11 = (m->u.e._22 * m->u.e._33 - m->u.e._23 * m->u.e._32) * inv_det;
    inv.u.e._12 = -(m->u.e._12 * m->u.e._33 - m->u.e._13 * m->u.e._32) * inv_det;
    inv.u.e._13 = (m->u.e._12 * m->u.e._23 - m->u.e._13 * m->u.e._22) * inv_det;
    inv.u.e._21 = -(m->u.e._21 * m->u.e._33 - m->u.e._23 * m->u.e._31) * inv_det;
    inv.u.e._22 = (m->u.e._11 * m->u.e._33 - m->u.e._13 * m->u.e._31) * inv_det;
    inv.u.e._23 = -(m->u.e._11 * m->u.e._23 - m->u.e._13 * m->u.e._21) * inv_det;
    inv.u.e._31 = (m->u.e._21 * m->u.e._32 - m->u.e._22 * m->u.e._31) * inv_det;
    inv.u.e._32 = -(m->u.e._11 * m->u.e._32 - m->u.e._12 * m->u.e._31) * inv_det;
    inv.u.e._33 = (m->u.e._11 * m->u.e._22 - m->u.e._12 * m->u.e._21) * inv_det;

    memcpy(m, &inv, sizeof(l3_3x3_matrix_t));
}

void l3_3x3_matrix_mul_3d_point(l3_3x3_matrix_t *m, l3_3d_point_t *p)
{
    l3_3d_point_t pt;
    pt.x = p->x * m->u.e._11 + p->y * m->u.e._12 + p->z * m->u.e._13;
    pt.y = p->x * m->u.e._21 + p->y * m->u.e._22 + p->z * m->u.e._23;
    pt.z = p->x * m->u.e._31 + p->y * m->u.e._32 + p->z * m->u.e._33;

    pt.x = pt.x / pt.z;
    pt.y = pt.y / pt.z;
    pt.z = 1;

    memcpy(p, &pt, sizeof(l3_3d_point_t));
}

void l3_3x3_matrix_identity(l3_3x3_matrix_t *m)
{
    memcpy(m, &identity_3x3_matrix, sizeof(l3_3x3_matrix_t));
}

void l3_3x3_matrix_translate(l3_3x3_matrix_t *m, float t_x, float t_y)
{
    memcpy(m, &identity_3x3_matrix, sizeof(l3_3x3_matrix_t));
    m->u.e._13 = t_x;   m->u.e._23 = t_y;

}

void l3_4x4_matrix_identity(l3_4x4_matrix_t *m)
{
    memcpy(m, &identity_4x4_matrix, sizeof(l3_4x4_matrix_t));
}

void l3_4x4_matrix_zero(l3_4x4_matrix_t *Mat)
{
    memset(Mat, 0, sizeof(l3_4x4_matrix_t));
}

void l3_4x4_matrix_rotateX(l3_4x4_matrix_t *m, float rotX)
{
    float cosX = l3_cos(rotX);
    float sinX = l3_sin(rotX);

    memcpy(m, &identity_4x4_matrix, sizeof(l3_4x4_matrix_t));
    m->u.e._22 = cosX;  m->u.e._32 = sinX;
    m->u.e._23 = -sinX; m->u.e._33 = cosX;
}

void l3_4x4_matrix_rotateY(l3_4x4_matrix_t *m, float rotY)
{
    float cosX = l3_cos(rotY);
    float sinX = l3_sin(rotY);

    memcpy(m, &identity_4x4_matrix, sizeof(l3_4x4_matrix_t));
    m->u.e._11 = cosX; m->u.e._31 = sinX;
    m->u.e._13 = -sinX; m->u.e._33 = cosX;
}

void l3_4x4_matrix_rotateZ(l3_4x4_matrix_t *m, float rotZ)
{
    float cosX = l3_cos(rotZ);
    float sinX = l3_sin(rotZ);

    memcpy(m, &identity_4x4_matrix, sizeof(l3_4x4_matrix_t));
    m->u.e._11 = cosX;  m->u.e._21 = sinX;
    m->u.e._12 = -sinX; m->u.e._22 = cosX;
}

void l3_4x4_matrix_scale(l3_4x4_matrix_t *m, float scale_x, float scale_y, float scale_z)
{
    memcpy(m, &identity_4x4_matrix, sizeof(l3_4x4_matrix_t));
    m->u.e._11 = scale_x;
    m->u.e._22 = scale_y;
    m->u.e._33 = scale_z;
}

void l3_4x4_matrix_translate(l3_4x4_matrix_t *m, float t_x, float t_y, float t_z)
{
    memcpy(m, &identity_4x4_matrix, sizeof(l3_4x4_matrix_t));
    m->u.e._14 = t_x;   m->u.e._24 = t_y;   m->u.e._34 = t_z;

}

void l3_4x4_matrix_compose_trs(l3_4x4_matrix_t *m, float translation[3], float rotation[4],
                               float scale[3])
{
    memcpy(m, &identity_4x4_matrix, sizeof(l3_4x4_matrix_t));

    float x = rotation[0], y = rotation[1], z = rotation[2], w = rotation[3];

    float x2 = x + x, y2 = y + y, z2 = z + z;
    float xx = x * x2, xy = x * y2, xz = x * z2;
    float yy = y * y2, yz = y * z2, zz = z * z2;
    float wx = w * x2, wy = w * y2, wz = w * z2;

    m->u.e._11 = scale[0] * (1.0f - (yy + zz));
    m->u.e._12 = scale[0] * (xy - wz);
    m->u.e._13 = scale[0] * (xz + wy);

    m->u.e._21 = scale[1] * (xy + wz);
    m->u.e._22 = scale[1] * (1.0f - (xx + zz));
    m->u.e._23 = scale[1] * (yz - wx);

    m->u.e._31 = scale[2] * (xz - wy);
    m->u.e._32 = scale[2] * (yz + wx);
    m->u.e._33 = scale[2] * (1.0f - (xx + yy));

    m->u.e._14 = translation[0];
    m->u.e._24 = translation[1];
    m->u.e._34 = translation[2];
}

__attribute__((noinline)) bool l3_4x4_matrix_mul(l3_4x4_matrix_t *input_left,
                                                 l3_4x4_matrix_t *input_right,
                                                 l3_4x4_matrix_t *output)
{
#ifdef GUI_3D_USE_MVE
    float *a = (float *)input_left->u.m;
    float *b = (float *)input_right->u.m;
    float *c = (float *)output->u.m;

    float32x4_t m2;
    float32x4_t out00, out10, out20, out30;
    m2 = vldrwq_f32(&b[0]);
    out00 = vmulq_n_f32(m2, a[0]);
    out10 = vmulq_n_f32(m2, a[4]);
    out20 = vmulq_n_f32(m2, a[8]);
    out30 = vmulq_n_f32(m2, a[12]);

    m2 = vldrwq_f32(&b[4]);
    out00 = vfmaq_n_f32(out00, m2, a[1]);
    out10 = vfmaq_n_f32(out10, m2, a[5]);
    out20 = vfmaq_n_f32(out20, m2, a[9]);
    out30 = vfmaq_n_f32(out30, m2, a[13]);

    m2 = vldrwq_f32(&b[8]);
    out00 = vfmaq_n_f32(out00, m2, a[2]);
    out10 = vfmaq_n_f32(out10, m2, a[6]);
    out20 = vfmaq_n_f32(out20, m2, a[10]);
    out30 = vfmaq_n_f32(out30, m2, a[14]);

    m2 = vldrwq_f32(&b[12]);
    out00 = vfmaq_n_f32(out00, m2, a[3]);
    out10 = vfmaq_n_f32(out10, m2, a[7]);
    out20 = vfmaq_n_f32(out20, m2, a[11]);
    out30 = vfmaq_n_f32(out30, m2, a[15]);

    vst1q_f32(&c[0], out00);
    vst1q_f32(&c[4], out10);
    vst1q_f32(&c[8], out20);
    vst1q_f32(&c[12], out30);

#else

    output->u.m[0][0] = input_left->u.m[0][0] * input_right->u.m[0][0] + input_left->u.m[0][1] *
                        input_right->u.m[1][0] +
                        input_left->u.m[0][2] * input_right->u.m[2][0] + input_left->u.m[0][3] * input_right->u.m[3][0];
    output->u.m[0][1] = input_left->u.m[0][0] * input_right->u.m[0][1] + input_left->u.m[0][1] *
                        input_right->u.m[1][1] +
                        input_left->u.m[0][2] * input_right->u.m[2][1] + input_left->u.m[0][3] * input_right->u.m[3][1];
    output->u.m[0][2] = input_left->u.m[0][0] * input_right->u.m[0][2] + input_left->u.m[0][1] *
                        input_right->u.m[1][2] +
                        input_left->u.m[0][2] * input_right->u.m[2][2] + input_left->u.m[0][3] * input_right->u.m[3][2];
    output->u.m[0][3] = input_left->u.m[0][0] * input_right->u.m[0][3] + input_left->u.m[0][1] *
                        input_right->u.m[1][3] +
                        input_left->u.m[0][2] * input_right->u.m[2][3] + input_left->u.m[0][3] * input_right->u.m[3][3];

    output->u.m[1][0] = input_left->u.m[1][0] * input_right->u.m[0][0] + input_left->u.m[1][1] *
                        input_right->u.m[1][0] +
                        input_left->u.m[1][2] * input_right->u.m[2][0] + input_left->u.m[1][3] * input_right->u.m[3][0];
    output->u.m[1][1] = input_left->u.m[1][0] * input_right->u.m[0][1] + input_left->u.m[1][1] *
                        input_right->u.m[1][1] +
                        input_left->u.m[1][2] * input_right->u.m[2][1] + input_left->u.m[1][3] * input_right->u.m[3][1];
    output->u.m[1][2] = input_left->u.m[1][0] * input_right->u.m[0][2] + input_left->u.m[1][1] *
                        input_right->u.m[1][2] +
                        input_left->u.m[1][2] * input_right->u.m[2][2] + input_left->u.m[1][3] * input_right->u.m[3][2];
    output->u.m[1][3] = input_left->u.m[1][0] * input_right->u.m[0][3] + input_left->u.m[1][1] *
                        input_right->u.m[1][3] +
                        input_left->u.m[1][2] * input_right->u.m[2][3] + input_left->u.m[1][3] * input_right->u.m[3][3];

    output->u.m[2][0] = input_left->u.m[2][0] * input_right->u.m[0][0] + input_left->u.m[2][1] *
                        input_right->u.m[1][0] +
                        input_left->u.m[2][2] * input_right->u.m[2][0] + input_left->u.m[2][3] * input_right->u.m[3][0];
    output->u.m[2][1] = input_left->u.m[2][0] * input_right->u.m[0][1] + input_left->u.m[2][1] *
                        input_right->u.m[1][1] +
                        input_left->u.m[2][2] * input_right->u.m[2][1] + input_left->u.m[2][3] * input_right->u.m[3][1];
    output->u.m[2][2] = input_left->u.m[2][0] * input_right->u.m[0][2] + input_left->u.m[2][1] *
                        input_right->u.m[1][2] +
                        input_left->u.m[2][2] * input_right->u.m[2][2] + input_left->u.m[2][3] * input_right->u.m[3][2];
    output->u.m[2][3] = input_left->u.m[2][0] * input_right->u.m[0][3] + input_left->u.m[2][1] *
                        input_right->u.m[1][3] +
                        input_left->u.m[2][2] * input_right->u.m[2][3] + input_left->u.m[2][3] * input_right->u.m[3][3];

    output->u.m[3][0] = input_left->u.m[3][0] * input_right->u.m[0][0] + input_left->u.m[3][1] *
                        input_right->u.m[1][0] +
                        input_left->u.m[3][2] * input_right->u.m[2][0] + input_left->u.m[3][3] * input_right->u.m[3][0];
    output->u.m[3][1] = input_left->u.m[3][0] * input_right->u.m[0][1] + input_left->u.m[3][1] *
                        input_right->u.m[1][1] +
                        input_left->u.m[3][2] * input_right->u.m[2][1] + input_left->u.m[3][3] * input_right->u.m[3][1];
    output->u.m[3][2] = input_left->u.m[3][0] * input_right->u.m[0][2] + input_left->u.m[3][1] *
                        input_right->u.m[1][2] +
                        input_left->u.m[3][2] * input_right->u.m[2][2] + input_left->u.m[3][3] * input_right->u.m[3][2];
    output->u.m[3][3] = input_left->u.m[3][0] * input_right->u.m[0][3] + input_left->u.m[3][1] *
                        input_right->u.m[1][3] +
                        input_left->u.m[3][2] * input_right->u.m[2][3] + input_left->u.m[3][3] * input_right->u.m[3][3];
#endif

    return true;
}


l3_4d_point_t l3_4x4_matrix_mul_4d_point(l3_4x4_matrix_t *mat, l3_4d_point_t p)
{
#ifdef GUI_3D_USE_MVE
    l3_4d_point_t point;
    float32x4_t out;

    uint32x4_t col_offsets = {0, 16, 32, 48};

    float32x4_t col0 = __arm_vldrwq_gather_offset_f32(&mat->u.m[0][0],
                                                      col_offsets); //// col0 = [m[0][0], m[1][0], m[2][0], m[3][0]]
    out  = vmulq_n_f32(col0, p.x);

    float32x4_t col1 = __arm_vldrwq_gather_offset_f32(&mat->u.m[0][1],
                                                      col_offsets); //// col1 = [m[0][1], m[1][1], m[2][1], m[3][1]]
    out += vmulq_n_f32(col1, p.y);

    float32x4_t col2 = __arm_vldrwq_gather_offset_f32(&mat->u.m[0][2],
                                                      col_offsets); //// col2 = [m[0][2], m[1][2], m[2][2], m[3][2]]
    out += vmulq_n_f32(col2, p.z);

    float32x4_t col3 = __arm_vldrwq_gather_offset_f32(&mat->u.m[0][3],
                                                      col_offsets); //// col3 = [m[0][3], m[1][3], m[2][3], m[3][3]]
    out += vmulq_n_f32(col3, p.w);

    vst1q_f32((float *)&point, out);

    return point;
#else
    l3_4d_point_t point;

    point.x = mat->u.m[0][0] * p.x + mat->u.m[0][1] * p.y + mat->u.m[0][2] * p.z + mat->u.m[0][3] * p.w;
    point.y = mat->u.m[1][0] * p.x + mat->u.m[1][1] * p.y + mat->u.m[1][2] * p.z + mat->u.m[1][3] * p.w;
    point.z = mat->u.m[2][0] * p.x + mat->u.m[2][1] * p.y + mat->u.m[2][2] * p.z + mat->u.m[2][3] * p.w;
    point.w = mat->u.m[3][0] * p.x + mat->u.m[3][1] * p.y + mat->u.m[3][2] * p.z + mat->u.m[3][3] * p.w;

    return point;
#endif
}


static void l3_generate_rotate_around_line(l3_4x4_matrix_t *mrot, float px, float py, float pz,
                                           float ux, float uy, float uz, float angle_degrees)
{
    // Normalize the direction vector (u)
    float mag = sqrtf(ux * ux + uy * uy + uz * uz);
    float a = ux / mag, b = uy / mag, c = uz / mag;

    // Step 1: Translate to the origin
    l3_4x4_matrix_t T1;
    l3_4x4_matrix_identity(&T1);
    T1.u.m[0][3] = -px;
    T1.u.m[1][3] = -py;
    T1.u.m[2][3] = -pz;

    // Step 2: Rotate to align u with the Z-axis
    float theta = atan2f(b, c) * (180.0f / M_PI_F);  // Convert radians to degrees
    l3_4x4_matrix_t Rx;
    l3_4x4_matrix_identity(&Rx);
    Rx.u.m[1][1] = l3_cos(theta);
    Rx.u.m[1][2] = -l3_sin(theta);
    Rx.u.m[2][1] = l3_sin(theta);
    Rx.u.m[2][2] = l3_cos(theta);

    float phi = atan2f(a, sqrtf(b * b + c * c)) * (180.0f / M_PI_F);
    l3_4x4_matrix_t Ry;
    l3_4x4_matrix_identity(&Ry);
    Ry.u.m[0][0] = l3_cos(phi);
    Ry.u.m[0][2] = -l3_sin(phi);
    Ry.u.m[2][0] = l3_sin(phi);
    Ry.u.m[2][2] = l3_cos(phi);

    // Step 3: Rotate around the Z-axis
    l3_4x4_matrix_t Rz;
    l3_4x4_matrix_identity(&Rz);
    Rz.u.m[0][0] = l3_cos(angle_degrees);
    Rz.u.m[0][1] = -l3_sin(angle_degrees);
    Rz.u.m[1][0] = l3_sin(angle_degrees);
    Rz.u.m[1][1] = l3_cos(angle_degrees);

    // Inverse rotations
    l3_4x4_matrix_t Ryi;
    l3_4x4_matrix_identity(&Ryi);
    Ryi.u.m[0][0] = l3_cos(-phi);
    Ryi.u.m[0][2] = -l3_sin(-phi);
    Ryi.u.m[2][0] = l3_sin(-phi);
    Ryi.u.m[2][2] = l3_cos(-phi);

    l3_4x4_matrix_t Rxi;
    l3_4x4_matrix_identity(&Rxi);
    Rxi.u.m[1][1] = l3_cos(-theta);
    Rxi.u.m[1][2] = -l3_sin(-theta);
    Rxi.u.m[2][1] = l3_sin(-theta);
    Rxi.u.m[2][2] = l3_cos(-theta);

    // Step 4: Translate back to the original position
    l3_4x4_matrix_t T2;
    l3_4x4_matrix_identity(&T2);
    T2.u.m[0][3] = px;
    T2.u.m[1][3] = py;
    T2.u.m[2][3] = pz;

    // Combine all transformations
    l3_4x4_matrix_t temp1, temp2, temp3, temp4, temp5;
    l3_4x4_matrix_mul(&Rxi, &Ryi, &temp1);
    l3_4x4_matrix_mul(&Rz, &temp1, &temp2);
    l3_4x4_matrix_mul(&Ry, &temp2, &temp3);
    l3_4x4_matrix_mul(&Rx, &temp3, &temp4);
    l3_4x4_matrix_mul(&T1, &temp4, &temp5);
    l3_4x4_matrix_mul(&temp5, &T2, mrot);
}


l3_4d_vector_t l3_4d_vector(float ux, float uy, float uz)
{
    l3_4d_vector_t vector;
    vector.ux = ux;
    vector.uy = uy;
    vector.uz = uz;
    vector.uw = 1.0f;
    return vector;
}


// Helper function to determine triangle orientation
void l3_adjust_triangle_winding(l3_vertex_t *p0, l3_vertex_t *p1, l3_vertex_t *p2)
{
    float x01m;
    float x0 = p0->position.x;
    float y0 = p0->position.y;
    float x1 = p1->position.x;
    float y1 = p1->position.y;
    float x2 = p2->position.x;
    float y2 = p2->position.y;

    if (x0 == x1)
    {
        x01m = x0;
    }
    else
    {
        float k01 = (y0 - y1) / (x0 - x1);
        float b01 = y0 - k01 * x0;
        x01m = (y2 - b01) / k01;
    }

    if (x01m > x2)
    {
        l3_vertex_t t = *p2;
        *p2 = *p1;
        *p1 = t;
    }
}

void l3_calculator_4x4_matrix(l3_4x4_matrix_t *matrix, \
                              float tx, float ty, float tz, \
                              l3_4d_point_t point, l3_4d_vector_t vector, float degrees, \
                              float scale)
{
    l3_4x4_matrix_t mrot, mscale, mtrans;
    float px = point.x;
    float py = point.y;
    float pz = point.z;
    float ux = vector.ux;
    float uy = vector.uy;
    float uz = vector.uz;
    l3_4x4_matrix_identity(matrix);

    l3_generate_rotate_around_line(&mrot, px, py, pz, ux, uy, uz, degrees);

    l3_4x4_matrix_scale(&mscale, scale, scale, scale);
    l3_4x4_matrix_translate(&mtrans, tx, ty, tz);

    l3_4x4_matrix_t temp1, temp2;
    l3_4x4_matrix_mul(&mscale, matrix, &temp1);
    l3_4x4_matrix_mul(&mrot, &temp1, &temp2);
    l3_4x4_matrix_mul(&mtrans, &temp2, matrix);
}

void l3_world_initialize(l3_4x4_matrix_t *world, float x, float y, float z, float rotX,
                         float rotY, float rotZ, float scale)
{
    l3_4x4_matrix_t mrotx, mroty, mrotz, mscale, mtrans;
    l3_4x4_matrix_identity(world);
    l3_4x4_matrix_rotateX(&mrotx, rotX);
    l3_4x4_matrix_rotateY(&mroty, rotY);
    l3_4x4_matrix_rotateZ(&mrotz, rotZ);
    l3_4x4_matrix_scale(&mscale, scale, scale, scale);
    l3_4x4_matrix_translate(&mtrans, x, y, z);

    l3_4x4_matrix_t temp1, temp2, temp3, temp4;
    l3_4x4_matrix_mul(&mscale, world, &temp1);
    l3_4x4_matrix_mul(&mrotx, &temp1, &temp2);
    l3_4x4_matrix_mul(&mroty, &temp2, &temp3);
    l3_4x4_matrix_mul(&mrotz, &temp3, &temp4);
    l3_4x4_matrix_mul(&mtrans, &temp4, world);

}

bool l3_camera_UVN_initialize(l3_camera_t *camera, l3_4d_point_t cameraPosition,
                              l3_4d_point_t cameraTarget, float near, float far, float fov, float viewPortWidth,
                              float viewPortHeight)
{
    camera->position = cameraPosition;
    camera->targetDirection = cameraTarget;
    // camera->near_z = near;
    // camera->far_z = far;
    (void)near;
    (void)far;
    camera->viewport_width = viewPortWidth;
    camera->viewport_height = viewPortHeight;

    camera->viewport_center_x = 0.5f * (viewPortWidth - 1);
    camera->viewport_center_y = 0.5f * (viewPortHeight - 1);

    camera->aspect_ratio = viewPortWidth / (float)viewPortHeight;

    l3_4x4_matrix_identity(&camera->mat_cam);
    // l3_4x4_matrix_identity(&camera->mat_per);
    // l3_4x4_matrix_identity(&camera->mat_scr);

    camera->fov = fov;
    camera->viewplane_width = 2.0f;
    // camera->viewplane_height = 2.0f / camera->aspect_ratio;

    if (fov == 90)
    {
        camera->d = 1;
    }
    else
    {
        camera->d = (0.5f) * (camera->viewplane_width) * tanf(fov * M_PI_F / 360.0f);
    }
    //camera->zbuffer=(float *)malloc(sizeof(float)*camera->viewport_height*camera->viewport_width);
    //return camera->zbuffer!=NULL;
    return true;
}


void l3_camera_build_UVN_matrix(l3_camera_t *camera)
{
    l3_4x4_matrix_t mt_inv;
    l3_4x4_matrix_t mt_uvn;
    l3_4d_vector_t n = l3_4d_point_sub(camera->targetDirection, camera->position);
    l3_4d_vector_t v = {0, -1, 0, 1};  // up is -y
    l3_4d_vector_t u;

    l3_4x4_matrix_identity(&mt_inv);
    mt_inv.u.e._14 = -camera->position.x;
    mt_inv.u.e._24 = -camera->position.y;
    mt_inv.u.e._34 = -camera->position.z;



    n = l3_4d_vector_unit(n);
    u = l3_4d_vector_cross(n, v);  // Right handed system U = N x V
    v = l3_4d_vector_cross(n, u);
    l3_4x4_matrix_zero(&mt_uvn);
    mt_uvn.u.e._11 = u.ux;
    mt_uvn.u.e._21 = v.ux;
    mt_uvn.u.e._31 = n.ux;

    mt_uvn.u.e._12 = u.uy;
    mt_uvn.u.e._22 = v.uy;
    mt_uvn.u.e._32 = n.uy;

    mt_uvn.u.e._13 = u.uz;
    mt_uvn.u.e._23 = v.uz;
    mt_uvn.u.e._33 = n.uz;

    mt_uvn.u.e._44 = 1;
    l3_4x4_matrix_mul(&mt_uvn, &mt_inv, &camera->mat_cam);
}

l3_description_t *l3_load_description(void *desc_addr)
{
    unsigned char *ptr = (unsigned char *)desc_addr;
    l3_description_t *desc = (l3_description_t *)l3_malloc(sizeof(l3_description_t));

    // file head
    desc->file_head = *((l3_desc_file_head_t *)ptr);
    if (desc->file_head.magic != 0x3344 || desc->file_head.model_type != 0 ||
        desc->file_head.face_type == 2)
    {
        l3_free(desc);
        return NULL;
    }
    ptr += desc->file_head.payload_offset;

    // attrib
    desc->attrib.num_vertices = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    desc->attrib.num_normals = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    desc->attrib.num_texcoords = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    desc->attrib.num_faces = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    desc->attrib.num_face_num_verts = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    desc->attrib.pad0 = *((int *)ptr);
    ptr += sizeof(int);

    desc->attrib.vertices = (l3_vertex_coordinate_t *)ptr;
    ptr += desc->attrib.num_vertices * sizeof(l3_vertex_coordinate_t);
    desc->attrib.normals = (l3_vertex_coordinate_t *)ptr;
    ptr += desc->attrib.num_normals * sizeof(l3_vertex_coordinate_t);
    desc->attrib.texcoords = (l3_texcoord_coordinate_t *)ptr;
    ptr += desc->attrib.num_texcoords * sizeof(l3_texcoord_coordinate_t);
    desc->attrib.faces = (l3_vertex_index_t *)ptr;
    ptr += desc->attrib.num_faces * sizeof(l3_vertex_index_t);
    desc->attrib.face_num_verts = (int *)ptr;
    ptr += desc->attrib.num_face_num_verts * sizeof(int);
    desc->attrib.material_ids = (int *)ptr;
    ptr += desc->attrib.num_face_num_verts * sizeof(int);

    // shape
    desc->num_shapes = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    desc->shapes = (l3_shape_t *)ptr;
    ptr += desc->num_shapes * sizeof(l3_shape_t);

    // material
    desc->num_materials = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    if (desc->num_materials > 0)
    {
        desc->materials = (l3_material_t *)ptr;
        ptr += desc->num_materials * sizeof(l3_material_t);

        // textures size
        desc->texture_sizes = (unsigned int *)ptr;
        ptr += desc->num_materials * sizeof(unsigned int);
        desc->textures = (unsigned char **)l3_malloc(desc->num_materials * sizeof(unsigned char *));
        // texture content
        for (uint32_t i = 0; i < desc->num_materials; i++)
        {
            if (desc->texture_sizes[i] > 0)
            {
                desc->textures[i] = (unsigned char *)ptr;
            }
            else
            {
                desc->textures[i] = NULL;
            }
            ptr += desc->texture_sizes[i];
        }

    }
    else
    {
        desc->materials = NULL;
        desc->textures = NULL;
        desc->texture_sizes = NULL;
    }

    return desc;

}

bool l3_calulate_draw_img_target_area(l3_draw_rect_img_t *img, l3_rect_t *rect)
{
    l3_3d_point_t pox = {0};
    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;

    float x1 = 0;
    float y1 = 0;
    float x2 = 0;
    float y2 = 0;

    if (rect == NULL)
    {
        x1 = 0;
        y1 = 0;
        x2 = img->img_w - 1;
        y2 = img->img_h - 1;
    }
    else
    {
        x1 = _UI_MAX(0, rect->x1);
        y1 = _UI_MAX(0, rect->y1);
        x2 = _UI_MIN(img->img_w - 1, rect->x2);
        y2 = _UI_MIN(img->img_h - 1, rect->y2);
    }

    pox.x = x1;
    pox.y = y1;
    pox.z = 1.0f;
    l3_3x3_matrix_mul_3d_point(&img->matrix, &pox);
    x_min = pox.x;
    x_max = pox.x;
    y_min = pox.y;
    y_max = pox.y;

    pox.x = x2;
    pox.y = y1;
    pox.z = 1.0f;
    l3_3x3_matrix_mul_3d_point(&img->matrix, &pox);
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
    l3_3x3_matrix_mul_3d_point(&img->matrix, &pox);
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
    l3_3x3_matrix_mul_3d_point(&img->matrix, &pox);
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

    img->img_target_x = (int16_t)x_min;
    img->img_target_y = (int16_t)y_min;
    img->img_target_w = ceilf(x_max) - (int16_t)x_min + 1;
    img->img_target_h = ceilf(y_max) - (int16_t)y_min + 1;
    return true;
}

void *(*l3_malloc_imp)(size_t size) = NULL;
void (*l3_free_imp)(void *ptr) = NULL;

void *l3_malloc(size_t size)
{
    if (l3_malloc_imp != NULL)
    {
        return l3_malloc_imp(size);
    }
    return malloc(size);
}
void l3_free(void *ptr)
{
    if (l3_free_imp != NULL)
    {
        l3_free_imp(ptr);
        return;
    }
    free(ptr);
}

