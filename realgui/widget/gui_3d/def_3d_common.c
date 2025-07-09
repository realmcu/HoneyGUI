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
#include "def_3d_common.h"
#include "gui_api.h"

#define GUI_ENABLE_MVE      1
#if  __ARM_FEATURE_MVE & GUI_ENABLE_MVE
#define GUI_3D_USE_MVE
#include <arm_mve.h>
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
static void gui_3d_swap(float *a, float *b)
{
    float c = *a;
    *a = *b;
    *b = c;
}


static bool gui_3d_gauss(float A[][9], int equ, int var,
                         float *answer)   //epu:A's row  var:A's col-1
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
                gui_3d_swap(&A[row][j], &A[max_r][j]);
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

bool gui_3d_generate_2d_matrix(gui_3d_point_2d_t *src, gui_3d_point_2d_t *dst, float *ret)
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
    if (gui_3d_gauss(A, 8, 8, ret) == true)
    {
        *(ret + 8) = 1;
        return true;
    }
    return false;

}

#if 1
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

static float gui_3d_sin(int angle)
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

static float gui_3d_cos(int angle)
{
    return gui_3d_sin(angle + 90);
}
#endif

gui_point_4d_t gui_point_4d(float x, float y, float z)
{
    gui_point_4d_t p;
    p.x = x;
    p.y = y;
    p.z = z;
    p.w = 1;
    return p;
}

float gui_point4D_dot(gui_point_4d_t p1, gui_point_4d_t p2)
{
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

gui_point_4d_t gui_point_4d_unit(gui_point_4d_t p)
{
    gui_point_4d_t pt;
    if (p.x || p.y || p.z)
    {
        float sqare = sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);
        pt.x = p.x / sqare;
        pt.y = p.y / sqare;
        pt.z = p.z / sqare;
        pt.w = 1;
        return pt;
    }
    return p;
}

gui_point_4d_t gui_point_4d_cross(gui_point_4d_t p1, gui_point_4d_t p2)
{
    gui_point_4d_t pt;
    pt.x = p1.y * p2.z - p2.y * p1.z;
    pt.y = p1.z * p2.x - p2.z * p1.x;
    pt.z = p1.x * p2.y - p2.x * p1.y;
    pt.w = 1;
    return pt;
}

gui_point_4d_t gui_point_4d_sub(gui_point_4d_t p1, gui_point_4d_t p2)
{
    gui_point_4d_t v;
    v.x = p1.x - p2.x;
    v.y = p1.y - p2.y;
    v.z = p1.z - p2.z;
    v.w = 1;
    return v;
}

static gui_3d_matrix_t identity_matrix =
{
    .m =
    {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }
};
void gui_3d_matrix_identity(gui_3d_matrix_t *m)
{
    memcpy(m, &identity_matrix, sizeof(gui_3d_matrix_t));
}

void gui_3d_matrix_zero(gui_3d_matrix_t *Mat)
{
    memset(Mat, 0, sizeof(gui_3d_matrix_t));
}

void gui_3d_matrix_rotateX(gui_3d_matrix_t *m, float rotX)
{
    float cosX = gui_3d_cos(rotX);
    float sinX = gui_3d_sin(rotX);

    memcpy(m, &identity_matrix, sizeof(gui_3d_matrix_t));
    m->_22 = cosX;  m->_23 = sinX;
    m->_32 = -sinX; m->_33 = cosX;
}

void gui_3d_matrix_rotateY(gui_3d_matrix_t *m, float rotY)
{
    float cosX = gui_3d_cos(rotY);
    float sinX = gui_3d_sin(rotY);

    memcpy(m, &identity_matrix, sizeof(gui_3d_matrix_t));
    m->_11 = cosX; m->_13 = sinX;
    m->_31 = -sinX; m->_33 = cosX;
}

void gui_3d_matrix_rotateZ(gui_3d_matrix_t *m, float rotZ)
{
    float cosX = gui_3d_cos(rotZ);
    float sinX = gui_3d_sin(rotZ);

    memcpy(m, &identity_matrix, sizeof(gui_3d_matrix_t));
    m->_11 = cosX;  m->_12 = sinX;
    m->_21 = -sinX; m->_22 = cosX;
}

void gui_3d_matrix_scale(gui_3d_matrix_t *m, float scale_x, float scale_y, float scale_z)
{
    memcpy(m, &identity_matrix, sizeof(gui_3d_matrix_t));
    m->_11 = scale_x;
    m->_22 = scale_y;
    m->_33 = scale_z;
}

void gui_3d_matrix_translate(gui_3d_matrix_t *m, float t_x, float t_y, float t_z)
{
    memcpy(m, &identity_matrix, sizeof(gui_3d_matrix_t));
    m->_41 = t_x;   m->_42 = t_y;   m->_43 = t_z;
}

gui_3d_matrix_t gui_3d_matrix_multiply(gui_3d_matrix_t Mat1, gui_3d_matrix_t Mat2)
{
#ifdef GUI_3D_USE_MVE
    gui_3d_matrix_t ptmat;
    float *a = (float *)Mat1.m;
    float *b = (float *)Mat2.m;
    float *c = (float *)ptmat.m;

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

    return ptmat;
#else
    gui_3d_matrix_t ptmat;
    ptmat.m[0][0] = Mat1.m[0][0] * Mat2.m[0][0] + Mat1.m[0][1] * Mat2.m[1][0] + Mat1.m[0][2] *
                    Mat2.m[2][0] + Mat1.m[0][3] * Mat2.m[3][0];
    ptmat.m[0][1] = Mat1.m[0][0] * Mat2.m[0][1] + Mat1.m[0][1] * Mat2.m[1][1] + Mat1.m[0][2] *
                    Mat2.m[2][1] + Mat1.m[0][3] * Mat2.m[3][1];
    ptmat.m[0][2] = Mat1.m[0][0] * Mat2.m[0][2] + Mat1.m[0][1] * Mat2.m[1][2] + Mat1.m[0][2] *
                    Mat2.m[2][2] + Mat1.m[0][3] * Mat2.m[3][2];
    ptmat.m[0][3] = Mat1.m[0][0] * Mat2.m[0][3] + Mat1.m[0][1] * Mat2.m[1][3] + Mat1.m[0][2] *
                    Mat2.m[2][3] + Mat1.m[0][3] * Mat2.m[3][3];

    ptmat.m[1][0] = Mat1.m[1][0] * Mat2.m[0][0] + Mat1.m[1][1] * Mat2.m[1][0] + Mat1.m[1][2] *
                    Mat2.m[2][0] + Mat1.m[1][3] * Mat2.m[3][0];
    ptmat.m[1][1] = Mat1.m[1][0] * Mat2.m[0][1] + Mat1.m[1][1] * Mat2.m[1][1] + Mat1.m[1][2] *
                    Mat2.m[2][1] + Mat1.m[1][3] * Mat2.m[3][1];
    ptmat.m[1][2] = Mat1.m[1][0] * Mat2.m[0][2] + Mat1.m[1][1] * Mat2.m[1][2] + Mat1.m[1][2] *
                    Mat2.m[2][2] + Mat1.m[1][3] * Mat2.m[3][2];
    ptmat.m[1][3] = Mat1.m[1][0] * Mat2.m[0][3] + Mat1.m[1][1] * Mat2.m[1][3] + Mat1.m[1][2] *
                    Mat2.m[2][3] + Mat1.m[1][3] * Mat2.m[3][3];

    ptmat.m[2][0] = Mat1.m[2][0] * Mat2.m[0][0] + Mat1.m[2][1] * Mat2.m[1][0] + Mat1.m[2][2] *
                    Mat2.m[2][0] + Mat1.m[2][3] * Mat2.m[3][0];
    ptmat.m[2][1] = Mat1.m[2][0] * Mat2.m[0][1] + Mat1.m[2][1] * Mat2.m[1][1] + Mat1.m[2][2] *
                    Mat2.m[2][1] + Mat1.m[2][3] * Mat2.m[3][1];
    ptmat.m[2][2] = Mat1.m[2][0] * Mat2.m[0][2] + Mat1.m[2][1] * Mat2.m[1][2] + Mat1.m[2][2] *
                    Mat2.m[2][2] + Mat1.m[2][3] * Mat2.m[3][2];
    ptmat.m[2][3] = Mat1.m[2][0] * Mat2.m[0][3] + Mat1.m[2][1] * Mat2.m[1][3] + Mat1.m[2][2] *
                    Mat2.m[2][3] + Mat1.m[2][3] * Mat2.m[3][3];

    ptmat.m[3][0] = Mat1.m[3][0] * Mat2.m[0][0] + Mat1.m[3][1] * Mat2.m[1][0] + Mat1.m[3][2] *
                    Mat2.m[2][0] + Mat1.m[3][3] * Mat2.m[3][0];
    ptmat.m[3][1] = Mat1.m[3][0] * Mat2.m[0][1] + Mat1.m[3][1] * Mat2.m[1][1] + Mat1.m[3][2] *
                    Mat2.m[2][1] + Mat1.m[3][3] * Mat2.m[3][1];
    ptmat.m[3][2] = Mat1.m[3][0] * Mat2.m[0][2] + Mat1.m[3][1] * Mat2.m[1][2] + Mat1.m[3][2] *
                    Mat2.m[2][2] + Mat1.m[3][3] * Mat2.m[3][2];
    ptmat.m[3][3] = Mat1.m[3][0] * Mat2.m[0][3] + Mat1.m[3][1] * Mat2.m[1][3] + Mat1.m[3][2] *
                    Mat2.m[2][3] + Mat1.m[3][3] * Mat2.m[3][3];
    return ptmat;

#endif
}



gui_point_4d_t gui_3d_point4D_mul_matrix(gui_point_4d_t p, gui_3d_matrix_t mat)
{
#ifdef GUI_3D_USE_MVE
    gui_point_4d_t point;
    float32x4_t m200, m210, m220, m230;
    float32x4_t out;

    m200 = vldrwq_f32(&mat.m[0][0]);
    m210 = vldrwq_f32(&mat.m[1][0]);
    m220 = vldrwq_f32(&mat.m[2][0]);
    m230 = vldrwq_f32(&mat.m[3][0]);

    out = vmulq_n_f32(m200, p.x) + vmulq_n_f32(m210, p.y) + vmulq_n_f32(m220, p.z);
    out = vaddq_f32(out, m230);

    vst1q_f32((float *)&point, out);

    return point;
#else
    gui_point_4d_t point;
    point.x = p.x * mat._11 + p.y * mat._21 + p.z * mat._31 + 1 * mat._41;
    point.y = p.x * mat._12 + p.y * mat._22 + p.z * mat._32 + 1 * mat._42;
    point.z = p.x * mat._13 + p.y * mat._23 + p.z * mat._33 + 1 * mat._43;
    point.w = p.x * mat._14 + p.y * mat._24 + p.z * mat._34 + 1 * mat._44;
    return point;
#endif
}


static void gui_3d_generate_rotate_around_line(gui_3d_matrix_t *mrot, float px, float py, float pz,
                                               float ux, float uy, float uz, float angle_degrees)
{
    // Normalize the direction vector (u)
    float mag = sqrtf(ux * ux + uy * uy + uz * uz);
    float a = ux / mag, b = uy / mag, c = uz / mag;

    // Step 1: Translate to the origin
    gui_3d_matrix_t T1;
    gui_3d_matrix_identity(&T1);
    T1.m[3][0] = -px;
    T1.m[3][1] = -py;
    T1.m[3][2] = -pz;

    // Step 2: Rotate to align u with the Z-axis
    float theta = atan2f(b, c) * (180.0f / 3.141562f);  // Convert radians to degrees
    gui_3d_matrix_t Rx;
    gui_3d_matrix_identity(&Rx);
    Rx.m[1][1] = gui_3d_cos(theta);
    Rx.m[1][2] = -gui_3d_sin(theta);
    Rx.m[2][1] = gui_3d_sin(theta);
    Rx.m[2][2] = gui_3d_cos(theta);

    float phi = atan2f(a, sqrtf(b * b + c * c)) * (180.0f / 3.141562f);
    gui_3d_matrix_t Ry;
    gui_3d_matrix_identity(&Ry);
    Ry.m[0][0] = gui_3d_cos(phi);
    Ry.m[0][2] = gui_3d_sin(phi);
    Ry.m[2][0] = -gui_3d_sin(phi);
    Ry.m[2][2] = gui_3d_cos(phi);

    // Step 3: Rotate around the Z-axis
    gui_3d_matrix_t Rz;
    gui_3d_matrix_identity(&Rz);
    Rz.m[0][0] = gui_3d_cos(angle_degrees);
    Rz.m[0][1] = -gui_3d_sin(angle_degrees);
    Rz.m[1][0] = gui_3d_sin(angle_degrees);
    Rz.m[1][1] = gui_3d_cos(angle_degrees);

    // Inverse rotations
    gui_3d_matrix_t Ryi;
    gui_3d_matrix_identity(&Ryi);
    Ryi.m[0][0] = gui_3d_cos(-phi);
    Ryi.m[0][2] = gui_3d_sin(-phi);
    Ryi.m[2][0] = -gui_3d_sin(-phi);
    Ryi.m[2][2] = gui_3d_cos(-phi);

    gui_3d_matrix_t Rxi;
    gui_3d_matrix_identity(&Rxi);
    Rxi.m[1][1] = gui_3d_cos(-theta);
    Rxi.m[1][2] = -gui_3d_sin(-theta);
    Rxi.m[2][1] = gui_3d_sin(-theta);
    Rxi.m[2][2] = gui_3d_cos(-theta);

    // Step 4: Translate back to the original position
    gui_3d_matrix_t T2;
    gui_3d_matrix_identity(&T2);
    T2.m[3][0] = px;
    T2.m[3][1] = py;
    T2.m[3][2] = pz;

    // Combine all transformations
    gui_3d_matrix_t temp1 = gui_3d_matrix_multiply(Ryi, Rxi);
    gui_3d_matrix_t temp2 = gui_3d_matrix_multiply(temp1, Rz);
    gui_3d_matrix_t temp3 = gui_3d_matrix_multiply(temp2, Ry);
    gui_3d_matrix_t temp4 = gui_3d_matrix_multiply(temp3, Rx);
    gui_3d_matrix_t temp5 = gui_3d_matrix_multiply(temp4, T1);
    *mrot = gui_3d_matrix_multiply(T2, temp5);
}

gui_point_4d_t gui_3d_point(float x, float y, float z)
{
    gui_point_4d_t point;
    point.x = x;
    point.y = y;
    point.z = z;
    point.w = 1.0f;
    return point;
}

gui_vector_4d_t gui_3d_vector(float ux, float uy, float uz)
{
    gui_vector_4d_t vector;
    vector.ux = ux;
    vector.uy = uy;
    vector.uz = uz;
    vector.uw = 1.0f;
    return vector;
}

void gui_3d_calculator_matrix(gui_3d_matrix_t *matrix, \
                              float x, float y, float z, \
                              gui_point_4d_t point, gui_vector_4d_t vector, float degrees, \
                              float scale)
{
    gui_3d_matrix_t mrot, mscale, mtrans;
    float px = point.x;
    float py = point.y;
    float pz = point.z;
    float ux = vector.ux;
    float uy = vector.uy;
    float uz = vector.uz;
    gui_3d_matrix_identity(matrix);

    gui_3d_generate_rotate_around_line(&mrot, px, py, pz, ux, uy, uz, degrees);

    gui_3d_matrix_scale(&mscale, scale, scale, scale);
    gui_3d_matrix_translate(&mtrans, x, y, z);

    *matrix = gui_3d_matrix_multiply(*matrix, mscale);

    *matrix = gui_3d_matrix_multiply(*matrix, mrot);

    *matrix = gui_3d_matrix_multiply(*matrix, mtrans);
}

void gui_3d_world_inititalize(gui_3d_matrix_t *world, float x, float y, float z, float rotX,
                              float rotY, float rotZ, float scale)
{
    gui_3d_matrix_t mrotx, mroty, mrotz, mscale, mtrans;
    gui_3d_matrix_identity(world);
    gui_3d_matrix_rotateX(&mrotx, rotX);
    gui_3d_matrix_rotateY(&mroty, rotY);
    gui_3d_matrix_rotateZ(&mrotz, rotZ);
    gui_3d_matrix_scale(&mscale, scale, scale, scale);
    gui_3d_matrix_translate(&mtrans, x, y, z);

    *world = gui_3d_matrix_multiply(*world, mscale);
    *world = gui_3d_matrix_multiply(*world, mrotx);
    *world = gui_3d_matrix_multiply(*world, mroty);

    *world = gui_3d_matrix_multiply(*world, mrotz);
    *world = gui_3d_matrix_multiply(*world, mtrans);


}

bool gui_3d_camera_UVN_initialize(gui_3d_camera_t *camera, gui_point_4d_t cameraPosition,
                                  gui_point_4d_t cameraTarget, float near, float far, float fov, float viewPortWidth,
                                  float viewPortHeight)
{
    camera->position = cameraPosition;
    camera->targetDirection = cameraTarget;
    camera->near_z = near;
    camera->far_z = far;
    camera->viewport_width = viewPortWidth;
    camera->viewport_height = viewPortHeight;

    camera->viewport_center_x = (viewPortWidth - 1) / 2;
    camera->viewport_center_y = (viewPortHeight - 1) / 2;

    camera->aspect_ratio = viewPortWidth / (float)viewPortHeight;

    gui_3d_matrix_identity(&camera->mat_cam);
    gui_3d_matrix_identity(&camera->mat_per);
    gui_3d_matrix_identity(&camera->mat_scr);

    camera->fov = fov;
    camera->viewplane_width = 2.0f;
    camera->viewplane_height = 2.0f / camera->aspect_ratio;

    if (fov == 90)
    {
        camera->d = 1;
    }
    else
    {
        camera->d = (0.5f) * (camera->viewplane_width) * tanf(fov * M_PI_F / 180.0f);
    }
    //camera->zbuffer=(float *)malloc(sizeof(float)*camera->viewport_height*camera->viewport_width);
    //return camera->zbuffer!=NULL;
    return true;
}


void gui_3d_camera_build_UVN_matrix(gui_3d_camera_t *camera)
{
    gui_3d_matrix_t mt_inv;
    gui_3d_matrix_t mt_uvn;
    gui_point_4d_t u, v, n;

    gui_3d_matrix_identity(&mt_inv);
    mt_inv._41 = -camera->position.x;
    mt_inv._42 = -camera->position.y;
    mt_inv._43 = -camera->position.z;

    n.x = camera->targetDirection.x - camera->position.x;
    n.y = camera->targetDirection.y - camera->position.y;
    n.z = camera->targetDirection.z - camera->position.z;
    n.w = 1;

    n = gui_point_4d_unit(n);
    v = gui_point_4d(0, -1, 0);  // up is -y
    u = gui_point_4d_cross(n, v);  // Right handed system U = N x V
    v = gui_point_4d_cross(n, u);
    gui_3d_matrix_zero(&mt_uvn);
    mt_uvn._11 = u.x;
    mt_uvn._12 = v.x;
    mt_uvn._13 = n.x;

    mt_uvn._21 = u.y;
    mt_uvn._22 = v.y;
    mt_uvn._23 = n.y;

    mt_uvn._31 = u.z;
    mt_uvn._32 = v.z;
    mt_uvn._33 = n.z;

    mt_uvn._44 = 1;
    camera->mat_cam = gui_3d_matrix_multiply(mt_inv, mt_uvn);
}

gui_3d_description_t *gui_load_3d_description(void *desc_addr)
{
    unsigned char *ptr = (unsigned char *)desc_addr;
    gui_3d_description_t *desc = (gui_3d_description_t *)gui_malloc(sizeof(gui_3d_description_t));

    // face type
    desc->face_type = (GUI_3D_FACE_TYPE) * ((int *)ptr);
    ptr += sizeof(int);

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

    desc->attrib.vertices = (gui_obj_vertex_coordinate_t *)ptr;
    ptr += desc->attrib.num_vertices * sizeof(gui_obj_vertex_coordinate_t);
    desc->attrib.normals = (gui_obj_vertex_coordinate_t *)ptr;
    ptr += desc->attrib.num_normals * sizeof(gui_obj_vertex_coordinate_t);
    desc->attrib.texcoords = (gui_obj_texcoord_coordinate_t *)ptr;
    ptr += desc->attrib.num_texcoords * sizeof(gui_obj_texcoord_coordinate_t);
    desc->attrib.faces = (gui_obj_vertex_index_t *)ptr;
    ptr += desc->attrib.num_faces * sizeof(gui_obj_vertex_index_t);
    desc->attrib.face_num_verts = (int *)ptr;
    ptr += desc->attrib.num_face_num_verts * sizeof(int);
    desc->attrib.material_ids = (int *)ptr;
    ptr += desc->attrib.num_face_num_verts * sizeof(int);

    // shape
    desc->num_shapes = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    desc->shapes = (gui_obj_shape_t *)ptr;
    ptr += desc->num_shapes * sizeof(gui_obj_shape_t);

    // material
    desc->num_materials = *((unsigned int *)ptr);
    ptr += sizeof(unsigned int);
    if (desc->num_materials > 0)
    {
        desc->materials = (gui_obj_material_t *)ptr;
        ptr += desc->num_materials * sizeof(gui_obj_material_t);

        // textures size
        desc->texture_sizes = (unsigned int *)ptr;
        ptr += desc->num_materials * sizeof(unsigned int);
        desc->textures = (unsigned char **)gui_malloc(desc->num_materials * sizeof(unsigned char *));
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

// Helper function to determine triangle orientation
static void determine_triangle_orientation(gui_3d_vertex_t *p0, gui_3d_vertex_t *p1,
                                           gui_3d_vertex_t *p2)
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
        gui_3d_vertex_t t = *p2;
        *p2 = *p1;
        *p1 = t;
    }
}

// Calculate edge properties (slope and intercept)
static void calculate_edge_properties(float x0, float y0, float x1, float y1,
                                      bool *infinite, float *k, float *b)
{
    if (x0 == x1)
    {
        *infinite = true;
        *k = 1;
        *b = x0;
    }
    else
    {
        *infinite = false;
        *k = (y0 - y1) / (x0 - x1);
        *b = y0 - *k * x0;
    }
}

// Fill with image data rgb565
static void gui_3d_fill_image_rgb565(int y, float xleft, float xright, float oneoverz,
                                     float oneoverz_step,
                                     float soverz, float soverz_step, float toverz, float toverz_step,
                                     float *zbuffer, uint16_t *pixelData, int width, int height,
                                     int src_head_w, int src_head_h, uint16_t *src_pixels)
{
    const int iy = (int)y;
    int startX = (int)(xleft + 0.5f);
    int endX = ((int)(xright + 0.5f) > width) ? width : (int)(xright + 0.5f);

    if (startX < 0)
    {
        oneoverz += (0 - startX) * oneoverz_step;
        soverz += (0 - startX) * soverz_step;
        toverz += (0 - startX) * toverz_step;
        startX = 0;
    }

    int rowOffset = iy * width;

    for (int ix = startX; ix < endX; ++ix)
    {
        float originalZ = 1.0f / oneoverz;

        // Calculate texture coordinates
        int srcX = (int)(soverz * originalZ * (src_head_w - 1));
        int srcY = (int)(toverz * originalZ * (src_head_h - 1));

        if (srcX >= 0 && srcX < src_head_w && srcY >= 0 && srcY < src_head_h)
        {
            int pixelIdx = ix + rowOffset;
            uint16_t originalColor = src_pixels[srcX + srcY * src_head_w];

            if (zbuffer[pixelIdx] == 0 || originalZ < zbuffer[pixelIdx])
            {
                // Completely opaque pixel processing
                zbuffer[pixelIdx] = originalZ;
                pixelData[pixelIdx] = originalColor;
            }
        }

        oneoverz += oneoverz_step;
        soverz += soverz_step;
        toverz += toverz_step;
    }
}

// Fill with image data argb8888
static void gui_3d_fill_image_argb8888(int y, float xleft, float xright, float oneoverz,
                                       float oneoverz_step,
                                       float soverz, float soverz_step, float toverz, float toverz_step,
                                       float *zbuffer, uint32_t *pixelData, int width, int height,
                                       int src_head_w, int src_head_h, uint32_t *src_pixels,
                                       uint8_t opacity_value)
{
    const int iy = (int)y;
    int startX = (int)(xleft + 0.5f);
    int endX = ((int)(xright + 0.5f) > width) ? width : (int)(xright + 0.5f);

    if (startX < 0)
    {
        oneoverz += (0 - startX) * oneoverz_step;
        soverz += (0 - startX) * soverz_step;
        toverz += (0 - startX) * toverz_step;
        startX = 0;
    }

    int rowOffset = iy * width;

    for (int ix = startX; ix < endX; ++ix)
    {
        float originalZ = 1.0f / oneoverz;

        // Calculate texture coordinates
        int srcX = (int)(soverz * originalZ * (src_head_w - 1));
        int srcY = (int)(toverz * originalZ * (src_head_h - 1));

        if (srcX >= 0 && srcX < src_head_w && srcY >= 0 && srcY < src_head_h)
        {
            int pixelIdx = ix + rowOffset;
            uint32_t originalColor = src_pixels[srcX + srcY * src_head_w];
            uint8_t alpha = (originalColor >> 24) & 0xFF;
            alpha = (alpha * opacity_value) / 255;

            if (zbuffer[pixelIdx] == 0 || originalZ < zbuffer[pixelIdx])
            {
                // Completely opaque pixel processing
                if (alpha == 255)
                {
                    zbuffer[pixelIdx] = originalZ;
                    pixelData[pixelIdx] = originalColor;
                }
                // Translucent pixel processing, without updating depth buffer (keeping the objects behind visible)
                else if (alpha > 0)
                {
                    uint32_t dstColor = pixelData[pixelIdx];

                    uint8_t srcR = (originalColor >> 16) & 0xFF;
                    uint8_t srcG = (originalColor >> 8) & 0xFF;
                    uint8_t srcB = originalColor & 0xFF;

                    uint8_t dstR = (dstColor >> 16) & 0xFF;
                    uint8_t dstG = (dstColor >> 8) & 0xFF;
                    uint8_t dstB = dstColor & 0xFF;
                    uint8_t dstA = (dstColor >> 24) & 0xFF;

                    // Blending
                    uint8_t outA = alpha + (dstA * (255 - alpha)) / 255;
                    uint8_t outR = (srcR * alpha + dstR * (255 - alpha)) / 255;
                    uint8_t outG = (srcG * alpha + dstG * (255 - alpha)) / 255;
                    uint8_t outB = (srcB * alpha + dstB * (255 - alpha)) / 255;

                    pixelData[pixelIdx] = (outA << 24) | (outR << 16) | (outG << 8) | outB;
                }
            }
        }

        oneoverz += oneoverz_step;
        soverz += soverz_step;
        toverz += toverz_step;
    }
}

// Fill with RGB565 color
static void gui_3d_fill_color_rgb565(float y, float xleft, float xright, float oneoverz,
                                     float oneoverz_step,
                                     float *zbuffer, uint16_t *pixelData, int width, uint16_t color)
{
    const int iy = (int)y;
    int startX = (int)(xleft + 0.5f);
    int endX = ((int)(xright + 0.5f) > width) ? width : (int)(xright + 0.5f);

    if (startX < 0)
    {
        oneoverz += (0 - startX) * oneoverz_step;
        startX = 0;
    }

    int rowOffset = iy * width;

    for (int ix = startX; ix < endX; ++ix)
    {
        int pixelIdx = ix + rowOffset;
        const float originalZ = 1.0f / oneoverz;

        if (zbuffer[pixelIdx] == 0 || originalZ < zbuffer[pixelIdx])
        {
            zbuffer[pixelIdx] = originalZ;
            pixelData[pixelIdx] = color;
        }
        oneoverz += oneoverz_step;
    }
}

// Fill with ARGB8888 color
static void gui_3d_fill_color_argb8888(float y, float xleft, float xright, float oneoverz,
                                       float oneoverz_step,
                                       float *zbuffer, uint32_t *pixelData, int width, uint32_t color)
{
    const int iy = (int)y;
    int startX = (int)(xleft + 0.5f);
    int endX = ((int)(xright + 0.5f) > width) ? width : (int)(xright + 0.5f);

    if (startX < 0)
    {
        oneoverz += (0 - startX) * oneoverz_step;
        startX = 0;
    }

    int rowOffset = iy * width;
    const uint8_t alpha = (color >> 24) & 0xFF;

    if (alpha == 255)
    {
        for (int ix = startX; ix < endX; ++ix)
        {
            int pixelIdx = ix + rowOffset;
            const float originalZ = 1.0f / oneoverz;
            if (zbuffer[pixelIdx] == 0 || originalZ < zbuffer[pixelIdx])
            {
                zbuffer[pixelIdx] = originalZ;
                pixelData[pixelIdx] = color;
            }
            oneoverz += oneoverz_step;
        }
    }
    else if (alpha > 0)
    {
        const uint8_t srcR = (color >> 16) & 0xFF;
        const uint8_t srcG = (color >> 8) & 0xFF;
        const uint8_t srcB = color & 0xFF;
        const uint16_t inv_alpha = 255 - alpha;
        for (int ix = startX; ix < endX; ++ix)
        {
            int pixelIdx = ix + rowOffset;
            const float originalZ = 1.0f / oneoverz;
            if (zbuffer[pixelIdx] == 0 || originalZ < zbuffer[pixelIdx])
            {
                zbuffer[pixelIdx] = originalZ;
                const uint32_t dstColor = pixelData[pixelIdx];

                const uint16_t dstR = (dstColor >> 16) & 0xFF;
                const uint16_t dstG = (dstColor >> 8) & 0xFF;
                const uint16_t dstB = dstColor & 0xFF;
                const uint16_t dstA = (dstColor >> 24) & 0xFF;

                pixelData[pixelIdx] = ((alpha + (dstA * inv_alpha) / 255) << 24) |
                                      (((srcR * alpha + dstR * inv_alpha) / 255) << 16) |
                                      (((srcG * alpha + dstG * inv_alpha) / 255) << 8) |
                                      ((srcB * alpha + dstB * inv_alpha) / 255);
            }

            oneoverz += oneoverz_step;
        }

    }
}

static void gui_3d_render_with_texture_rgb565(float y, float y0, float y1, float y2,
                                              float inv_z0, float inv_z1, float inv_z2,
                                              float soverz0, float soverz1, float soverz2,
                                              float toverz0, float toverz1, float toverz2,
                                              bool k01infinite, float k01, float b01,
                                              bool k02infinite, float k02, float b02,
                                              float *zbuffer, uint16_t *pixelData, int width, int height,
                                              int src_head_w, int src_head_h, uint16_t *src_pixels, uint8_t opacity_value)
{
    float xleft = k01infinite ? b01 : (y - b01) / k01;
    float xright = k02infinite ? b02 : (y - b02) / k02;

    const float y_y0 = y - y0;
    const float inv_y1_y0 = 1.0f / (y1 - y0);
    const float inv_y2_y0 = 1.0f / (y2 - y0);

    // Interpolate depth
    float oneoverz_left = y_y0 * (inv_z1 - inv_z0) * inv_y1_y0 +
                          inv_z0;         // 1/z interpolation on the left edge
    float oneoverz_right = y_y0 * (inv_z2 - inv_z0) * inv_y2_y0  +
                           inv_z0;       // 1/z interpolation on the right edge
    float oneoverz_step = (oneoverz_right - oneoverz_left) / (xright -
                                                              xleft);   // Horizontal step size of 1/z


    float soverz_left = y_y0 * (soverz1 - soverz0) * inv_y1_y0 +
                        soverz0;      // Interpolation of texture coordinates s (u) on the left edge
    float soverz_right = y_y0 * (soverz2 - soverz0) * inv_y2_y0 +
                         soverz0;     // Interpolation of texture coordinates s (u) on the right edge
    float soverz_step = (soverz_right - soverz_left) / (xright -
                                                        xleft);       // Horizontal step size of texture coordinates s (u)

    float toverz_left = y_y0 * (toverz1 - toverz0) * inv_y1_y0 +
                        toverz0;      // Interpolation of texture coordinates t (v) on the left edge
    float toverz_right = y_y0 * (toverz2 - toverz0) * inv_y2_y0 +
                         toverz0;     // Interpolation of texture coordinates t (v) on the right edge
    float toverz_step = (toverz_right - toverz_left) / (xright -
                                                        xleft);       // Horizontal step size of texture coordinate t (v)

    gui_3d_fill_image_rgb565(y, xleft, xright, oneoverz_left, oneoverz_step,
                             soverz_left, soverz_step, toverz_left, toverz_step,
                             zbuffer, pixelData, width, height, src_head_w, src_head_h, src_pixels);
}

static void gui_3d_render_with_texture_argb8888(float y, float y0, float y1, float y2,
                                                float inv_z0, float inv_z1, float inv_z2,
                                                float soverz0, float soverz1, float soverz2,
                                                float toverz0, float toverz1, float toverz2,
                                                bool k01infinite, float k01, float b01,
                                                bool k02infinite, float k02, float b02,
                                                float *zbuffer, uint32_t *pixelData, int width, int height,
                                                int src_head_w, int src_head_h, uint32_t *src_pixels, uint8_t opacity_value)
{
    float xleft = k01infinite ? b01 : (y - b01) / k01;
    float xright = k02infinite ? b02 : (y - b02) / k02;

    const float y_y0 = y - y0;
    const float inv_y1_y0 = 1.0f / (y1 - y0);
    const float inv_y2_y0 = 1.0f / (y2 - y0);

    // Interpolate depth
    float oneoverz_left = y_y0 * (inv_z1 - inv_z0) * inv_y1_y0 +
                          inv_z0;         // 1/z interpolation on the left edge
    float oneoverz_right = y_y0 * (inv_z2 - inv_z0) * inv_y2_y0  +
                           inv_z0;       // 1/z interpolation on the right edge
    float oneoverz_step = (oneoverz_right - oneoverz_left) / (xright -
                                                              xleft);   // Horizontal step size of 1/z


    float soverz_left = y_y0 * (soverz1 - soverz0) * inv_y1_y0 +
                        soverz0;      // Interpolation of texture coordinates s (u) on the left edge
    float soverz_right = y_y0 * (soverz2 - soverz0) * inv_y2_y0 +
                         soverz0;     // Interpolation of texture coordinates s (u) on the right edge
    float soverz_step = (soverz_right - soverz_left) / (xright -
                                                        xleft);       // Horizontal step size of texture coordinates s (u)

    float toverz_left = y_y0 * (toverz1 - toverz0) * inv_y1_y0 +
                        toverz0;      // Interpolation of texture coordinates t (v) on the left edge
    float toverz_right = y_y0 * (toverz2 - toverz0) * inv_y2_y0 +
                         toverz0;     // Interpolation of texture coordinates t (v) on the right edge
    float toverz_step = (toverz_right - toverz_left) / (xright -
                                                        xleft);       // Horizontal step size of texture coordinate t (v)

    gui_3d_fill_image_argb8888(y, xleft, xright, oneoverz_left, oneoverz_step,
                               soverz_left, soverz_step, toverz_left, toverz_step,
                               zbuffer, pixelData, width, height, src_head_w, src_head_h, src_pixels, opacity_value);
}

static void gui_3d_render_with_color_rgb565(float y, float y0, float y1, float y2,
                                            float inv_z0, float inv_z1, float inv_z2,
                                            bool k01infinite, float k01, float b01,
                                            bool k02infinite, float k02, float b02,
                                            float *zbuffer, uint32_t *pixelData, int width,
                                            GUI_3D_FILL_TYPE fillType, void *fillData)
{
    float xleft = k01infinite ? b01 : (y - b01) / k01;
    float xright = k02infinite ? b02 : (y - b02) / k02;

    const float y_y0 = y - y0;
    const float inv_y1_y0 = 1.0f / (y1 - y0);
    const float inv_y2_y0 = 1.0f / (y2 - y0);

    // Interpolate depth
    float oneoverz_left = y_y0 * (inv_z1 - inv_z0) * inv_y1_y0 +
                          inv_z0;         // 1/z interpolation on the left edge
    float oneoverz_right = y_y0 * (inv_z2 - inv_z0) * inv_y2_y0  +
                           inv_z0;       // 1/z interpolation on the right edge
    float oneoverz_step = (oneoverz_right - oneoverz_left) / (xright -
                                                              xleft);   // Horizontal step size of 1/z

    gui_3d_fill_color_rgb565(y, xleft, xright, oneoverz_left, oneoverz_step, zbuffer,
                             (uint16_t *)pixelData,
                             width, *((uint16_t *)fillData));

}

static void gui_3d_render_with_color_argb8888(float y, float y0, float y1, float y2,
                                              float inv_z0, float inv_z1, float inv_z2,
                                              bool k01infinite, float k01, float b01,
                                              bool k02infinite, float k02, float b02,
                                              float *zbuffer, uint32_t *pixelData, int width,
                                              GUI_3D_FILL_TYPE fillType, void *fillData)
{
    float xleft = k01infinite ? b01 : (y - b01) / k01;
    float xright = k02infinite ? b02 : (y - b02) / k02;

    const float y_y0 = y - y0;
    const float inv_y1_y0 = 1.0f / (y1 - y0);
    const float inv_y2_y0 = 1.0f / (y2 - y0);

    // Interpolate depth
    float oneoverz_left = y_y0 * (inv_z1 - inv_z0) * inv_y1_y0 +
                          inv_z0;         // 1/z interpolation on the left edge
    float oneoverz_right = y_y0 * (inv_z2 - inv_z0) * inv_y2_y0  +
                           inv_z0;       // 1/z interpolation on the right edge
    float oneoverz_step = (oneoverz_right - oneoverz_left) / (xright -
                                                              xleft);   // Horizontal step size of 1/z

    gui_3d_fill_color_argb8888(y, xleft, xright, oneoverz_left, oneoverz_step, zbuffer, pixelData,
                               width,
                               *((uint32_t *)fillData));

}


void gui_3d_fill_triangle(gui_3d_vertex_t p0, gui_3d_vertex_t p1, gui_3d_vertex_t p2,
                          float *zbuffer, uint32_t *pixelData, int width, int height,
                          GUI_3D_FILL_TYPE fillType, void *fillData, uint8_t opacity_value)
{
    bool k01infinite, k02infinite;
    float k01, b01, k02, b02;
    float x0, y0, z0, s0, t0;
    float x1, y1, z1, s1, t1;
    float x2, y2, z2, s2, t2;

    float inv_z0, inv_z1, inv_z2;
    float soverz0, soverz1, soverz2;
    float toverz0, toverz1, toverz2;


    //    p0
    // p1   p2
    // First pass: top part of the triangle
    if (p1.position.y < p0.position.y)
    {
        gui_3d_vertex_t vt;
        vt = p1;
        p1 = p0;
        p0 = vt;
    }
    if (p2.position.y < p0.position.y)
    {
        gui_3d_vertex_t vt;
        vt = p2;
        p2 = p0;
        p0 = vt;
    }

    float midy = p2.position.y < p1.position.y ? p2.position.y : p1.position.y;

    determine_triangle_orientation(&p0, &p1, &p2);

    // Reassignment and reordering of vertex coordinates and texture coordinates
    x0 = p0.position.x; y0 = p0.position.y; z0 = p0.position.z; s0 = p0.u; t0 = p0.v;
    x1 = p1.position.x; y1 = p1.position.y; z1 = p1.position.z; s1 = p1.u; t1 = p1.v;
    x2 = p2.position.x; y2 = p2.position.y; z2 = p2.position.z; s2 = p2.u; t2 = p2.v;

    // Calculate slope and intercept
    calculate_edge_properties(x0, y0, x1, y1, &k01infinite, &k01, &b01);
    calculate_edge_properties(x0, y0, x2, y2, &k02infinite, &k02, &b02);

    // Pre calculate constants
    inv_z0 = 1.0f / z0;
    inv_z1 = 1.0f / z1;
    inv_z2 = 1.0f / z2;

    float y_start = fmaxf((int)(y0 + 0.5f) + 0.5f, 0.5f);
    float y_end = fminf(midy, height);

    if (fillType == GUI_3D_FILL_IMAGE_RGB565)
    {
        // Interpolate texture coordinates
        soverz0 = s0 * inv_z0;
        soverz1 = s1 * inv_z1;
        soverz2 = s2 * inv_z2;

        toverz0 = t0 * inv_z0;
        toverz1 = t1 * inv_z1;
        toverz2 = t2 * inv_z2;

        gui_rgb_data_head_t *src_head = (gui_rgb_data_head_t *)fillData;
        uint16_t *src_pixels = (uint16_t *)((unsigned char *)fillData + sizeof(gui_rgb_data_head_t));

        for (float y = y_start; y <= y_end; y++)
        {
            gui_3d_render_with_texture_rgb565(y, y0, y1, y2, inv_z0, inv_z1, inv_z2,
                                              soverz0, soverz1, soverz2, toverz0, toverz1, toverz2,
                                              k01infinite, k01, b01, k02infinite, k02, b02,
                                              zbuffer, (uint16_t *)pixelData, width, height, src_head->w, src_head->h, src_pixels, opacity_value);

        }
    }
    else if (fillType == GUI_3D_FILL_IMAGE_ARGB8888)
    {
        // Interpolate texture coordinates
        soverz0 = s0 * inv_z0;
        soverz1 = s1 * inv_z1;
        soverz2 = s2 * inv_z2;

        toverz0 = t0 * inv_z0;
        toverz1 = t1 * inv_z1;
        toverz2 = t2 * inv_z2;

        gui_rgb_data_head_t *src_head = (gui_rgb_data_head_t *)fillData;
        uint32_t *src_pixels = (uint32_t *)((unsigned char *)fillData + sizeof(gui_rgb_data_head_t));

        for (float y = y_start; y <= y_end; y++)
        {
            gui_3d_render_with_texture_argb8888(y, y0, y1, y2, inv_z0, inv_z1, inv_z2,
                                                soverz0, soverz1, soverz2, toverz0, toverz1, toverz2,
                                                k01infinite, k01, b01, k02infinite, k02, b02,
                                                zbuffer, pixelData, width, height, src_head->w, src_head->h, src_pixels, opacity_value);

        }
    }
    else if (fillType == GUI_3D_FILL_COLOR_RGB565)
    {
        for (float y = y_start; y <= y_end; y++)
        {
            gui_3d_render_with_color_rgb565(y, y0, y1, y2, inv_z0, inv_z1, inv_z2,
                                            k01infinite, k01, b01, k02infinite, k02, b02,
                                            zbuffer, pixelData, width, fillType, fillData);
        }
    }
    else
    {
        for (float y = y_start; y <= y_end; y++)
        {
            gui_3d_render_with_color_argb8888(y, y0, y1, y2, inv_z0, inv_z1, inv_z2,
                                              k01infinite, k01, b01, k02infinite, k02, b02,
                                              zbuffer, pixelData, width, fillType, fillData);
        }
    }


    // p1   p2
    //    p0
    // Second pass: bottom part of the triangle
    if (p1.position.y > p0.position.y)
    {
        gui_3d_vertex_t vt = p1;
        p1 = p0;
        p0 = vt;
    }
    if (p2.position.y > p0.position.y)
    {
        gui_3d_vertex_t vt = p2;
        p2 = p0;
        p0 = vt;
    }

    midy = p2.position.y < p1.position.y ? p1.position.y : p2.position.y;

    determine_triangle_orientation(&p0, &p1, &p2);

    x0 = p0.position.x; y0 = p0.position.y; z0 = p0.position.z; s0 = p0.u; t0 = p0.v;
    x1 = p1.position.x; y1 = p1.position.y; z1 = p1.position.z; s1 = p1.u; t1 = p1.v;
    x2 = p2.position.x; y2 = p2.position.y; z2 = p2.position.z; s2 = p2.u; t2 = p2.v;

    calculate_edge_properties(x0, y0, x1, y1, &k01infinite, &k01, &b01);
    calculate_edge_properties(x0, y0, x2, y2, &k02infinite, &k02, &b02);

    // Pre calculate constants
    inv_z0 = 1.0f / z0;
    inv_z1 = 1.0f / z1;
    inv_z2 = 1.0f / z2;

    y_start = fmaxf((int)(midy + 0.5f) + 0.5f, 0.5f);
    y_end = fminf(y0, height);

    if (fillType == GUI_3D_FILL_IMAGE_RGB565)
    {
        // Interpolate texture coordinates
        soverz0 = s0 * inv_z0;
        soverz1 = s1 * inv_z1;
        soverz2 = s2 * inv_z2;

        toverz0 = t0 * inv_z0;
        toverz1 = t1 * inv_z1;
        toverz2 = t2 * inv_z2;

        gui_rgb_data_head_t *src_head = (gui_rgb_data_head_t *)fillData;
        uint16_t *src_pixels = (uint16_t *)((unsigned char *)fillData + sizeof(gui_rgb_data_head_t));

        for (float y = y_start; y <= y_end; y++)
        {
            gui_3d_render_with_texture_rgb565(y, y0, y1, y2, inv_z0, inv_z1, inv_z2,
                                              soverz0, soverz1, soverz2, toverz0, toverz1, toverz2,
                                              k01infinite, k01, b01, k02infinite, k02, b02,
                                              zbuffer, (uint16_t *)pixelData, width, height, src_head->w, src_head->h, src_pixels, opacity_value);

        }
    }
    else if (fillType == GUI_3D_FILL_IMAGE_ARGB8888)
    {
        // Interpolate texture coordinates
        soverz0 = s0 * inv_z0;
        soverz1 = s1 * inv_z1;
        soverz2 = s2 * inv_z2;

        toverz0 = t0 * inv_z0;
        toverz1 = t1 * inv_z1;
        toverz2 = t2 * inv_z2;

        gui_rgb_data_head_t *src_head = (gui_rgb_data_head_t *)fillData;
        uint32_t *src_pixels = (uint32_t *)((unsigned char *)fillData + sizeof(gui_rgb_data_head_t));

        for (float y = y_start; y <= y_end; y++)
        {
            gui_3d_render_with_texture_argb8888(y, y0, y1, y2, inv_z0, inv_z1, inv_z2,
                                                soverz0, soverz1, soverz2, toverz0, toverz1, toverz2,
                                                k01infinite, k01, b01, k02infinite, k02, b02,
                                                zbuffer, pixelData, width, height, src_head->w, src_head->h, src_pixels, opacity_value);

        }
    }
    else if (fillType == GUI_3D_FILL_COLOR_RGB565)
    {
        for (float y = y_start; y <= y_end; y++)
        {
            gui_3d_render_with_color_rgb565(y, y0, y1, y2, inv_z0, inv_z1, inv_z2,
                                            k01infinite, k01, b01, k02infinite, k02, b02,
                                            zbuffer, pixelData, width, fillType, fillData);
        }
    }
    else
    {
        for (float y = y_start; y <= y_end; y++)
        {
            gui_3d_render_with_color_argb8888(y, y0, y1, y2, inv_z0, inv_z1, inv_z2,
                                              k01infinite, k01, b01, k02infinite, k02, b02,
                                              zbuffer, pixelData, width, fillType, fillData);
        }
    }

}


