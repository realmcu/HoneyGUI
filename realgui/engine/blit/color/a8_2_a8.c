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
        uint32_t source_off = (i - rel_y) * source_w + x_start - rel_x;
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

void a8_2_a8(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    a8_2_a8_1d(image, dc, rect);
}

