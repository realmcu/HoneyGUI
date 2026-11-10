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

static void rgb565_2_rgb565_1d_bypass(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;

    uint32_t len = x_end - x_start + 1;

    for (int32_t i = y_start; i <= y_end; i++)
    {
        uint32_t source_off = (i - rel_y) * source_w + x_start - rel_x;
        uint32_t des_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + x_start -
                           dc->section.x1;
        memcpy(writebuf + des_off, (uint16_t *)(uintptr_t)image_base + source_off, 2 * len);
    }
}
static void rgb565_2_rgb565_1d_filter(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;

    if (opacity_value == 0)
    {
        return;// fully transparent, nothing to draw
    }
    else if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_1D(
            color_rgb565_t,
        {
            if (pixel->color.rgb565 != 0)
            {
                writebuf[write_offset] = pixel->color.rgb565;
            }
        };
        );
    }
    else
    {
        PROCESS_IMAGE_PIXEL_1D(
            color_rgb565_t,
        {
            if (pixel->color.rgb565 != 0)
            {
                writebuf[write_offset] = rgb565_fast_blending(pixel->color.rgb565, writebuf[write_offset],
                                                              opacity_value);
            }
        };
        );
    }

}

static void rgb565_2_rgb565_1d_cover(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;

    if (opacity_value == 0)
    {
        return;// fully transparent, nothing to draw
    }
    else if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_1D(
            color_rgb565_t,
        {
            if (pixel->color.rgb565 != 0)
            {
                writebuf[write_offset] = pixel->color.rgb565;
            }
        };
        );
    }
    else
    {
        uint8_t alpha = (opacity_value + 4) >> 3;
        PROCESS_IMAGE_PIXEL_1D(
            color_rgb565_t,
        {
            if (pixel->color.rgb565 != 0)
            {
                uint32_t fg = ((pixel->color.rgb565 | (pixel->color.rgb565 << 16)) & 0x07e0f81f);
                uint32_t result = ((((fg * alpha) >> 5)) & 0x07e0f81f);
                writebuf[write_offset] = (uint16_t)((result >> 16) | result);
            }
        };
        );
    }

}

static void rgb565_2_rgb565_1d_src(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;
    if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_1D(
            color_rgb565_t,
        {
            writebuf[write_offset] = pixel->color.rgb565;
        };
        );
    }
    else
    {
        PROCESS_IMAGE_PIXEL_1D(
            color_rgb565_t,
        {
            writebuf[write_offset] = rgb565_fast_blending(pixel->color.rgb565, 0x0000, opacity_value);
        };
        );
    }
}


static void rgb565_2_rgb565_1d(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    if (image->blend_mode == IMG_BYPASS_MODE)
    {
        rgb565_2_rgb565_1d_bypass(image, dc, rect);
    }
    else if (image->blend_mode == IMG_FILTER_BLACK)
    {
        rgb565_2_rgb565_1d_filter(image, dc, rect);
    }
    else if (image->blend_mode == IMG_COVER_MODE)
    {
        rgb565_2_rgb565_1d_cover(image, dc, rect);
    }
    else if (image->blend_mode == IMG_SRC_MODE)
    {
        rgb565_2_rgb565_1d_src(image, dc, rect);
    }
    else
    {
        rgb565_2_rgb565_1d_bypass(image, dc, rect);
    }
}

void rgb565_2_rgb565(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    rgb565_2_rgb565_1d(image, dc, rect);
}

