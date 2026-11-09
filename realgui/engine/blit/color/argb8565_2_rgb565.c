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


static void argb8565_2_rgb565_1d_src_over(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;

    if (opacity_value == 0)
    {
        return;// fully transparent, nothing to draw
    }
    else if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_1D(
            color_argb8565_t,
        {
            writebuf[write_offset] = rgb565_fast_blending(pixel->color.rgb565, writebuf[write_offset], pixel->a);
        };
        );
    }
    else
    {
        PROCESS_IMAGE_PIXEL_1D(
            color_argb8565_t,
        {
            writebuf[write_offset] = rgb565_fast_blending(pixel->color.rgb565, writebuf[write_offset], pixel->a * opacity_value >> 8);
        };
        );
    }

}

static void argb8565_2_rgb565_1d_src(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;
    if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_1D(
            color_argb8565_t,
        {
            writebuf[write_offset] = rgb565_fast_blending(pixel->color.rgb565, writebuf[write_offset], pixel->a);
        };
        );
    }
    else
    {
        PROCESS_IMAGE_PIXEL_1D(
            color_argb8565_t,
        {
            uint16_t src = rgb565_fast_blending(pixel->color.rgb565, 0x0000, opacity_value);
            writebuf[write_offset] = rgb565_fast_blending(src, writebuf[write_offset], pixel->a);
        };
        );
    }
}

static void argb8565_2_rgb565_1d(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    if (image->blend_mode == IMG_SRC_MODE)
    {
        argb8565_2_rgb565_1d_src(image, dc, rect);
    }
    else
    {
        argb8565_2_rgb565_1d_src_over(image, dc, rect);
    }
}

void argb8565_2_rgb565(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    argb8565_2_rgb565_1d(image, dc, rect);
}

