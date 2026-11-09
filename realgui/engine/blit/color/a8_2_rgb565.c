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


static void a8_2_rgb565_1d_fix_bg(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
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
            writebuf[write_offset] = rgb565_fast_blending(fg, bg, pixel->a);
        };
        );
    }
    else
    {
        PROCESS_IMAGE_PIXEL_1D(
            color_a8_t,
        {
            writebuf[write_offset] = rgb565_fast_blending(fg, bg, pixel->a * opacity_value >> 8);
        };
        );
    }
}

static void a8_2_rgb565_1d_use_bg(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
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
            writebuf[write_offset] = rgb565_fast_blending(fg, writebuf[write_offset], pixel->a);
        };
        );
    }
    else
    {
        PROCESS_IMAGE_PIXEL_1D(
            color_a8_t,
        {
            writebuf[write_offset] = rgb565_fast_blending(fg, writebuf[write_offset], pixel->a * opacity_value >> 8);
        };
        );
    }
}

static void a8_2_rgb565_1d(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    if (image->blend_mode == IMG_2D_SW_FIX_A8_FG)
    {
        a8_2_rgb565_1d_use_bg(image, dc, rect);
    }
    else if (image->blend_mode == IMG_2D_SW_FIX_A8_BGFG)
    {
        a8_2_rgb565_1d_fix_bg(image, dc, rect);
    }
}

void a8_2_rgb565(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    a8_2_rgb565_1d(image, dc, rect);
}

