/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "draw_img.h"
#include <stdio.h>
#include <stdint.h>
#include "acc_sw_define.h"

static void a8_2_a8_1d_bypass(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;

    uint32_t len = x_end - x_start + 1;

    for (int32_t i = y_start; i <= y_end; i++)
    {
        uint32_t source_off = (i + m12) * source_w + x_start + m02;
        uint32_t des_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + x_start -
                           dc->section.x1;
        memcpy((uint8_t *)writebuf + des_off, (uint8_t *)(uintptr_t)image_base + source_off, len);
    }
}

static void a8_2_a8_1d_filter(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;

    if (opacity_value == 0)
    {
        return;// fully transparent, nothing to draw
    }
    else if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_1D(
            color_a8_t,
        {
            if (pixel->a != 0)
            {
                writebuf[write_offset] = pixel->a;
            }
        };
        );
    }
    else
    {
        PROCESS_IMAGE_PIXEL_1D(
            color_a8_t,
        {
            if (pixel->a != 0)
            {
                writebuf[write_offset] = pixel->a * opacity_value;
            }
        };
        );
    }
}

static void a8_2_a8_1d_source_over(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;

    if (opacity_value == 0)
    {
        return;// fully transparent, nothing to draw
    }
    else if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_1D(
            color_a8_t,
        {
            writebuf[write_offset] = pixel->a;
        };
        );
    }
    else
    {
        PROCESS_IMAGE_PIXEL_1D(
            color_a8_t,
        {
            writebuf[write_offset] = pixel->a * opacity_value + writebuf[write_offset] * (255 - opacity_value);
        };
        );
    }
}

static void a8_2_a8_1d(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    if (image->blend_mode == IMG_BYPASS_MODE)
    {
        a8_2_a8_1d_bypass(image, dc, rect);
    }
    else if (image->blend_mode == IMG_FILTER_BLACK)
    {
        a8_2_a8_1d_filter(image, dc, rect);
    }
    else if (image->blend_mode == IMG_SRC_OVER_MODE)
    {
        a8_2_a8_1d_source_over(image, dc, rect);
    }
}

static void a8_2_a8_2d(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    GUI_UNUSED(image);
    GUI_UNUSED(dc);
    GUI_UNUSED(rect);
    GUI_ASSERT(0);
}

static void a8_2_a8_3d(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    GUI_UNUSED(image);
    GUI_UNUSED(dc);
    GUI_UNUSED(rect);
    GUI_ASSERT(0);
}

void a8_2_a8(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{

    float m00 = image->matrix.m[0][0];
    float m01 = image->matrix.m[0][1];
    float m10 = image->matrix.m[1][0];
    float m11 = image->matrix.m[1][1];
    float m20 = image->matrix.m[2][0];
    float m21 = image->matrix.m[2][1];
    float m22 = image->matrix.m[2][2];

    if ((m00 == 1) && (m11 == 1) &&
        (m01 == 0) && (m10 == 0) &&
        (m20 == 0) && (m21 == 0) &&
        (m22 == 1))
    {
        a8_2_a8_1d(image, dc, rect);
    }
    else if ((image->matrix.m[2][2] == 1) && (image->matrix.m[2][0] == 0) &&
             (image->matrix.m[2][1] == 0))
    {
        a8_2_a8_2d(image, dc, rect);
    }
    else
    {
        a8_2_a8_3d(image, dc, rect);
    }
}

