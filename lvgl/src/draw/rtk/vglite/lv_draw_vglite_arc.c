/**
 * @file lv_draw_vglite_arc.c
 *
 */

/**
 * MIT License
 *
 * Copyright 2021-2023 NXP
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next paragraph)
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vglite_arc.h"

#if LV_USE_GPU_VG_LITE
#include "lv_vglite_buf.h"
#include <math.h>
#include "trace.h"

/*********************
 *      DEFINES
 *********************/
#define VGLITE_CUBIC_PATH_SIZE      7
#define VGLITE_LINE_PATH_SIZE       3
#define VGLITE_MOVE_PATH_SIZE       3
#define VGLITE_CLOSE_PATH_SIZE      1
#define VGLITE_END_PATH_SIZE        1
#define MAX_VGLITE_ARC_PATH_SIZE    (20 * VGLITE_CUBIC_PATH_SIZE + 2 * VGLITE_LINE_PATH_SIZE + VGLITE_MOVE_PATH_SIZE + 2 * VGLITE_CLOSE_PATH_SIZE + VGLITE_END_PATH_SIZE)
#define RTL_PI      3.1415926535



/**********************
 *  STATIC VARIABLES
 **********************/
static const int16_t sin0_90_table_half_degree[] =
{
    0, 285, 571, 857, 1143, 1429, 1714, 2000, 2285, 2570, 2855, 3140, 3425, 3709, 3993, 4276, 4560, 4843, 5125, 5408, 5689, 5971, 6252, 6532, 6812, 7092, 7370, 7649, 7927, 8204, 8480, 8756, 9031, 9306, 9580, 9853, 10125, 10397, 10667, 10937, 11206, 11475, 11742, 12009, 12274, 12539, 12803, 13065, 13327, 13588, 13847, 14106, 14364, 14620, 14875, 15130, 15383, 15635, 15885, 16135, 16383, 16630, 16876, 17120, 17363, 17605, 17846, 18085, 18323, 18559, 18794, 19027, 19259, 19490, 19719, 19947, 20173, 20397, 20620, 20842, 21062, 21280, 21497, 21712, 21925, 22137, 22347, 22555, 22761, 22966, 23169, 23371, 23570, 23768, 23964, 24158, 24350, 24541, 24729, 24916, 25100, 25283, 25464, 25643, 25820, 25995, 26168, 26339, 26509, 26676, 26841, 27004, 27165, 27323, 27480, 27635, 27787, 27938, 28086, 28233, 28377, 28518, 28658, 28796, 28931, 29064, 29195, 29324, 29450, 29575, 29696, 29816, 29934, 30049, 30162, 30272, 30381, 30486, 30590, 30691, 30790, 30887, 30981, 31073, 31163, 31250, 31335, 31417, 31497, 31575, 31650, 31723, 31793, 31861, 31927, 31990, 32050, 32109, 32164, 32218, 32269, 32317, 32363, 32407, 32448, 32486, 32522, 32556, 32587, 32616, 32642, 32665, 32687, 32705, 32722, 32735, 32747, 32755, 32762, 32765, 32767
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
LV_ATTRIBUTE_FAST_MEM int16_t rtk_trigo_sin(int16_t angle)
{
    int16_t ret = 0;
    angle       = angle % 720;

    if (angle < 0) { angle = 720 + angle; }

    if (angle < 180)
    {
        ret = sin0_90_table_half_degree[angle];
    }
    else if (angle >= 180 && angle < 360)
    {
        angle = 360 - angle;
        ret   = sin0_90_table_half_degree[angle];
    }
    else if (angle >= 360 && angle < 540)
    {
        angle = angle - 360;
        ret   = -sin0_90_table_half_degree[angle];
    }
    else     /*angle >=270*/
    {
        angle = 720 - angle;
        ret   = -sin0_90_table_half_degree[angle];
    }

    return ret;
}

static inline LV_ATTRIBUTE_FAST_MEM int16_t rtk_trigo_cos(int16_t angle)
{
    return rtk_trigo_sin(angle + 180);
}

static void lv_vglite_create_arc_path_data(float *path_data, uint32_t *path_data_size,
                                           const lv_point_t *center, int32_t radius, float width,
                                           int32_t angle, bool full_circle);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_res_t lv_vglite_draw_arc(const lv_point_t *center, int32_t radius, int32_t start_angle,
                            int32_t end_angle,
                            const lv_area_t *clip_area, const lv_draw_arc_dsc_t *dsc)
{
    if (start_angle == end_angle)
    {
        return LV_RES_OK;
    }
    vg_lite_error_t err = VG_LITE_SUCCESS;
    lv_color32_t col32 = {.full = lv_color_to32(dsc->color)}; /*Convert color to RGBA8888*/
    vg_lite_path_t path;
    vg_lite_color_t vgcol; /* vglite takes ABGR */
    bool donut = ((end_angle - start_angle) % 360 == 0) ? true : false;
    vg_lite_buffer_t *vgbuf = lv_vglite_get_dest_buf();
    float path_data[MAX_VGLITE_ARC_PATH_SIZE];
    uint32_t path_data_size = 0;
    lv_memset_00(&path, sizeof(vg_lite_path_t));
    lv_coord_t width = dsc->width;
    start_angle = start_angle % 360;
    if (start_angle < 0)
    {
        start_angle += 360;
    }
    end_angle = end_angle % 360;
    if (end_angle < 0)
    {
        end_angle += 360;
    }

    int32_t angle = end_angle - start_angle;
    lv_vglite_create_arc_path_data(path_data, &path_data_size, center, radius, width, angle, donut);
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    if (start_angle)
    {
        vg_lite_translate(center->x * 1.0f, center->y * 1.0f, &matrix);
        vg_lite_rotate(start_angle * 1.0f, &matrix);
        vg_lite_translate(0 - center->x * 1.0f, 0 - center->y * 1.0f, &matrix);
    }

    err = vg_lite_init_path(&path, VG_LITE_FP32, VG_LITE_HIGH, path_data_size, path_data,
                            (vg_lite_float_t)clip_area->x1, (vg_lite_float_t)clip_area->y1,
                            ((vg_lite_float_t)clip_area->x2) + 1.0f, ((vg_lite_float_t)clip_area->y2) + 1.0f);
    VG_LITE_ERR_RETURN_INV(err, "Init path failed.");

    vg_lite_buffer_format_t color_format = LV_COLOR_DEPTH == 16 ? VG_LITE_BGRA8888 : VG_LITE_ABGR8888;
    if (lv_vglite_premult_and_swizzle(&vgcol, col32, dsc->opa, color_format) != LV_RES_OK)
    {
        VG_LITE_RETURN_INV("Premultiplication and swizzle failed.");
    }

    /*** Draw arc ***/
    err = vg_lite_draw(vgbuf, &path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_SRC_OVER, vgcol);
    VG_LITE_ERR_RETURN_INV(err, "Draw arc failed.");

    if (lv_vglite_run() != LV_RES_OK)
    {
        VG_LITE_RETURN_INV("Run failed.");
    }

    err = vg_lite_clear_path(&path);
    VG_LITE_ERR_RETURN_INV(err, "Clear path failed.");
    return LV_RES_OK;
}

static void lv_vglite_create_arc_path_data(float *path_data, uint32_t *path_data_size,
                                           const lv_point_t *center, int32_t radius, float width,
                                           int32_t angle, bool full_circle)
{
    uint8_t contain = 0;
    char *cpath;
    /* Path data element index */
    uint8_t pidx = 0;
    float cx = center->x;
    float cy = center->y;
    float final_radius = radius * 1.0f;
    if (full_circle)
    {
        if (width >= radius)
        {
            float cpoff = (float)radius * BEZIER_OPTIM_CIRCLE;

            /* Circle case */
            /* Starting point */
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = cx;
            path_data[pidx++] = cy - final_radius;

            /* Top-right arc */
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = cpoff;
            path_data[pidx++] = 0;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = final_radius - cpoff;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = final_radius;

            /* Bottom-right arc*/
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0;
            path_data[pidx++] = cpoff;
            path_data[pidx++] = cpoff - final_radius;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = 0 - final_radius;
            path_data[pidx++] = final_radius;

            /* Bottom-left arc */
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0 - cpoff;
            path_data[pidx++] = 0;
            path_data[pidx++] = 0 - final_radius;
            path_data[pidx++] = cpoff - final_radius;
            path_data[pidx++] = 0 - final_radius;
            path_data[pidx++] = 0 - final_radius;

            /* Top-left arc*/
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0;
            path_data[pidx++] = 0 - cpoff;
            path_data[pidx++] = final_radius - cpoff;
            path_data[pidx++] = 0 - final_radius;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = 0 - final_radius;

            /* Ending point */
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_END;
            pidx++;
        }
        else if (width > 0)
        {
            float cpoff = final_radius * BEZIER_OPTIM_CIRCLE;

            /* Circle case */
            /* Starting point */
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = cx;
            path_data[pidx++] = cy - final_radius;

            /* Top-right arc */
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = cpoff;
            path_data[pidx++] = 0;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = final_radius - cpoff;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = final_radius;

            /* Bottom-right arc*/
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0;
            path_data[pidx++] = cpoff;
            path_data[pidx++] = cpoff - final_radius;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = 0 - final_radius;
            path_data[pidx++] = final_radius;

            /* Bottom-left arc */
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0 - cpoff;
            path_data[pidx++] = 0;
            path_data[pidx++] = 0 - final_radius;
            path_data[pidx++] = cpoff - final_radius;
            path_data[pidx++] = 0 - final_radius;
            path_data[pidx++] = 0 - final_radius;

            /* Top-left arc*/
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0;
            path_data[pidx++] = 0 - cpoff;
            path_data[pidx++] = final_radius - cpoff;
            path_data[pidx++] = 0 - final_radius;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = 0 - final_radius;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;

            final_radius = final_radius - width;
            cpoff = final_radius * BEZIER_OPTIM_CIRCLE;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = cx;
            path_data[pidx++] = cy - final_radius;

            /* Top-right arc */
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = -cpoff;
            path_data[pidx++] = 0;
            path_data[pidx++] = -final_radius;
            path_data[pidx++] = final_radius - cpoff;
            path_data[pidx++] = -final_radius;
            path_data[pidx++] = final_radius;

            /* Bottom-right arc*/
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0;
            path_data[pidx++] = cpoff;
            path_data[pidx++] = -cpoff + final_radius;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = final_radius;

            /* Bottom-left arc */
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = cpoff;
            path_data[pidx++] = 0;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = cpoff - final_radius;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = 0 - final_radius;

            /* Top-left arc*/
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0;
            path_data[pidx++] = 0 - cpoff;
            path_data[pidx++] = -final_radius + cpoff;
            path_data[pidx++] = 0 - final_radius;
            path_data[pidx++] = -final_radius;
            path_data[pidx++] = 0 - final_radius;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;

            /* Ending point */
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_END;
            pidx++;
        }
        else
        {
            return;
        }
    }
    else
    {
        uint16_t rest_angle = angle % 90;
        angle = angle - rest_angle;
        float f_angle = angle * 1.0f;
        int f_rest_angle = rest_angle * 1.0f;
        float line_rel_x = 0, line_rel_y = 0;

        if (angle >= 270)
        {
            contain = (0x01 | 0x02 | 0x04);
            line_rel_x = width;
        }
        else if (angle >= 180)
        {
            contain = (0x01 | 0x02);
            line_rel_y = -width;
        }
        else if (angle >= 90)
        {
            contain = 0x01;
            line_rel_x = -width;
        }
        if (contain == 0 && rest_angle == 0)
        {
            return;
        }
        float cpoff = final_radius * BEZIER_OPTIM_CIRCLE;
        float x1, y1, cp1_x, cp1_y, cp2_x, cp2_y, x2, y2, ax, bx, ay, by, q1, q2, k2;
        /* Circle case */
        /* Starting point */
        cpath = (char *)(path_data + pidx);
        *cpath = VLC_OP_MOVE;
        pidx++;
        path_data[pidx++] = cx;
        path_data[pidx++] = cy - final_radius;

        if (contain & 0x01)
        {
            /* Top-right arc */
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = cpoff;
            path_data[pidx++] = 0;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = final_radius - cpoff;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = final_radius;
        }

        if (contain & 0x02)
        {
            /* Bottom-right arc*/
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0;
            path_data[pidx++] = cpoff;
            path_data[pidx++] = cpoff - final_radius;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = 0 - final_radius;
            path_data[pidx++] = final_radius;
        }

        if (contain & 0x04)
        {
            /* Bottom-left arc */
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0 - cpoff;
            path_data[pidx++] = 0;
            path_data[pidx++] = 0 - final_radius;
            path_data[pidx++] = cpoff - final_radius;
            path_data[pidx++] = 0 - final_radius;
            path_data[pidx++] = 0 - final_radius;
        }

        if (rest_angle)
        {
            x1 = final_radius * rtk_trigo_cos((angle - 90) * 2) / LV_TRIGO_SIN_MAX + cx;
            y1 = final_radius * rtk_trigo_sin((angle - 90) * 2) / LV_TRIGO_SIN_MAX + cy;
            x2 = final_radius * rtk_trigo_cos((angle + rest_angle - 90) * 2) / LV_TRIGO_SIN_MAX + cx;
            y2 = final_radius * rtk_trigo_sin((angle + rest_angle - 90) * 2) / LV_TRIGO_SIN_MAX + cy;
            ax = x1 - cx, ay = y1 - cy;
            bx = x2 - cx, by = y2 - cy;
            q1 = ax * ax + ay * ay;
            q2 = q1 + ax * bx + ay * by;
            //k2 = 4.0f / 3 * (sqrt(2 * q1 * q2) - q2) / (ax * by - ay * bx);
            k2 = 4.0f / 3 * (LV_TRIGO_SIN_MAX - rtk_trigo_cos(rest_angle)) / rtk_trigo_sin(rest_angle);
            cp1_x = cx + ax - k2 * ay;
            cp1_y = cy + ay + k2 * ax;
            cp2_x = cx + bx + k2 * by;
            cp2_y = cy + by - k2 * bx;

            /* Top-right arc */
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC;
            pidx++;
            path_data[pidx++] = cp1_x;
            path_data[pidx++] = cp1_y;
            path_data[pidx++] = cp2_x;
            path_data[pidx++] = cp2_y;
            path_data[pidx++] = x2;
            path_data[pidx++] = y2;

            final_radius = final_radius - width;
            cpoff = final_radius * BEZIER_OPTIM_CIRCLE;

            x1 = final_radius * rtk_trigo_cos((angle + rest_angle - 90) * 2) / LV_TRIGO_SIN_MAX + cx;
            y1 = final_radius * rtk_trigo_sin((angle + rest_angle - 90) * 2) / LV_TRIGO_SIN_MAX + cy;
            x2 = final_radius * rtk_trigo_cos((angle - 90) * 2) / LV_TRIGO_SIN_MAX + cx;
            y2 = final_radius * rtk_trigo_sin((angle - 90) * 2) / LV_TRIGO_SIN_MAX + cy;
            ax = x1 - cx;
            ay = y1 - cy;
            bx = x2 - cx;
            by = y2 - cy;
            q1 = ax * ax + ay * ay;
            q2 = q1 + ax * bx + ay * by;
            //k2 = 4.0f / 3 * (sqrt(2 * q1 * q2) - q2) / (ax * by - ay * bx);
            k2 = -k2;
            cp1_x = cx + ax - k2 * ay;
            cp1_y = cy + ay + k2 * ax;
            cp2_x = cx + bx + k2 * by;
            cp2_y = cy + by - k2 * bx;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC;
            pidx++;
            path_data[pidx++] = cp1_x;
            path_data[pidx++] = cp1_y;
            path_data[pidx++] = cp2_x;
            path_data[pidx++] = cp2_y;
            path_data[pidx++] = x2;
            path_data[pidx++] = y2;
        }

        if (contain && rest_angle == 0)
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE_REL;
            pidx++;
            path_data[pidx++] = line_rel_x;
            path_data[pidx++] = line_rel_y;

            final_radius = final_radius - width;
            cpoff = final_radius * BEZIER_OPTIM_CIRCLE;
        }

        if (contain & 0x04)
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0;
            path_data[pidx++] = cpoff;
            path_data[pidx++] = -cpoff + final_radius;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = final_radius;
        }

        if (contain & 0x02)
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = cpoff;
            path_data[pidx++] = 0;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = cpoff - final_radius;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = 0 - final_radius;
        }

        if (contain & 0x01)
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0;
            path_data[pidx++] = 0 - cpoff;
            path_data[pidx++] = -final_radius + cpoff;
            path_data[pidx++] = 0 - final_radius;
            path_data[pidx++] = -final_radius;
            path_data[pidx++] = 0 - final_radius;
        }

        if (contain || rest_angle)
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE_REL;
            pidx++;
            path_data[pidx++] = 0;
            path_data[pidx++] = -width;
        }

        /* Ending point */
        cpath = (char *)(path_data + pidx);
        *cpath = VLC_OP_END;
        pidx++;
    }
    *path_data_size = pidx * sizeof(float);
}
#endif /*LV_USE_GPU_NXP_VG_LITE*/
