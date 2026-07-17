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

#if defined(__ARM_FEATURE_MVE)
#include <arm_mve.h>
#define RGB565_BLIT_USE_MVE   1
#else
#define RGB565_BLIT_USE_MVE   0
#endif

#if RGB565_BLIT_USE_MVE
/*
 * Bit-exact MVE port of rgb565_fast_blending() operating on 4 pixels at once.
 *
 * It uses the same 32-bit packed-RGB565 layout as the scalar helper
 * (rrrrrggggggbbbbb -> 00000gggggg00000rrrrr000000bbbbb, mask 0x07E0F81F), so
 * every lane reproduces the scalar result bit-for-bit. Only 4 lanes fit per
 * vector because the packed form needs the full 32-bit width per pixel; the
 * caller handles the <4 tail with the scalar helper.
 *
 * fg / bg : four RGB565 values zero-extended into u32 lanes.
 * alpha8  : four 8-bit alphas (0..255), one per lane.
 * returns : four blended values; the caller stores each lane's low 16 bits.
 */
static inline uint32x4_t rgb565_fast_blending_mve_x4(uint32x4_t fg, uint32x4_t bg,
                                                     uint32x4_t alpha8)
{
    const uint32x4_t mask = vdupq_n_u32(0x07e0f81f);

    /* alpha converted from [0..255] to [0..32] for 5-bit precision */
    uint32x4_t alpha = vshrq_n_u32(vaddq_n_u32(alpha8, 4), 3);

    bg = vandq_u32(vorrq_u32(bg, vshlq_n_u32(bg, 16)), mask);
    fg = vandq_u32(vorrq_u32(fg, vshlq_n_u32(fg, 16)), mask);

    /* result = (((fg - bg) * alpha) >> 5) + bg, masked back into the fields */
    uint32x4_t result = vmulq_u32(vsubq_u32(fg, bg), alpha);
    result = vaddq_u32(vshrq_n_u32(result, 5), bg);
    result = vandq_u32(result, mask);

    /* pack: (result >> 16) | result; caller keeps only the low 16 bits */
    return vorrq_u32(vshrq_n_u32(result, 16), result);
}
#endif /* RGB565_BLIT_USE_MVE */

static void rgb565_2_rgb565_1d_bypass(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;

    uint32_t len = x_end - x_start + 1;

    for (int32_t i = y_start; i <= y_end; i++)
    {
        uint32_t source_off = (i + m12) * source_w + x_start + m02;
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

static void rgb565_2_rgb565_1d_raster(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;
    float prog = image->raster_prog;

    if (prog <= 0.f)
    {
        return;
    }
    else if (prog >= 1.0f)
    {
        uint32_t len = (uint32_t)(x_end - x_start + 1);
        for (int32_t i = y_start; i <= y_end; i++)
        {
            uint32_t source_off = (i + m12) * source_w + x_start + m02;
            uint32_t des_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + x_start -
                               dc->section.x1;
            memcpy(writebuf + des_off, (uint16_t *)(uintptr_t)image_base + source_off, len * 2);
        }
    }
    else
    {
        const float raster_width = 0.25f;
        if (image->blend_mode == IMG_RASTER_HORIZONTAL)
        {
            int16_t raster_region_length = (int16_t)(raster_width * dc->screen_width);
            int16_t raster_region_center = -raster_region_length / 2 + (int16_t)(prog *
                                                                                 (dc->screen_width + raster_region_length));
            int16_t raster_region_start = raster_region_center - raster_region_length / 2;
            int16_t raster_region_end = raster_region_center + raster_region_length / 2;

            for (int32_t i = y_start; i <= y_end; i++)
            {
                int write_offset = (i - y1) * (x2 - x1 + 1) + x_start - x1;
                int read_offset = (int32_t)((i + m12) * source_w + x_start + m02);
                color_rgb565_t *pixel = (color_rgb565_t *)(uintptr_t)image_base + read_offset;

                int32_t j = 0;
                if (raster_region_start >= x_start)
                {
                    uint32_t len = (uint32_t)(raster_region_start - x_start + 1);
                    memcpy(writebuf + write_offset, (uint16_t *)(uintptr_t)pixel, len * 2);
                    write_offset += len;
                    pixel += len;
                    j += len;
                }
                // mve speed up: blend the source strip under a linear alpha ramp
                {
                    int16_t temp = _UI_MIN(raster_region_end, x_end);
#if RGB565_BLIT_USE_MVE
                    for (; j + 3 <= temp; j += 4)
                    {
                        uint32_t alpha_lane[4];
                        for (int32_t t = 0; t < 4; t++)
                        {
                            alpha_lane[t] =
                                (uint8_t)(0xff * (raster_region_end - (j + t)) / raster_region_length);
                        }
                        uint32x4_t fg_v = vldrhq_u32((const uint16_t *)pixel);
                        uint32x4_t bg_v = vldrhq_u32(&writebuf[write_offset]);
                        uint32x4_t out = rgb565_fast_blending_mve_x4(fg_v, bg_v, vld1q_u32(alpha_lane));
                        vstrhq_u32(&writebuf[write_offset], out);
                        write_offset += 4;
                        pixel += 4;
                    }
#endif
                    for (; j <= temp; j++)
                    {
                        uint8_t alpha = (uint8_t)(0xff * (raster_region_end - j) / raster_region_length);
                        writebuf[write_offset] = rgb565_fast_blending(pixel->color.rgb565, writebuf[write_offset],
                                                                      alpha);
                        write_offset++;
                        pixel++;
                    }
                }
            }
        }
        else if (image->blend_mode == IMG_RASTER_VERTICAL)
        {
            int16_t raster_region_length = (int16_t)(raster_width * dc->screen_height);
            int16_t raster_region_center = -raster_region_length / 2 + (int16_t)(prog *
                                                                                 (dc->screen_height + raster_region_length));
            int16_t raster_region_start = raster_region_center - raster_region_length / 2;
            int16_t raster_region_end = raster_region_center + raster_region_length / 2;
            uint32_t len = (uint32_t)(x_end - x_start + 1);

            for (int32_t i = y_start; i <= y_end; i++)
            {
                int32_t read_offset = (i + m12) * source_w + x_start + m02;
                int32_t write_offset = (i - y1) * (x2 - x1 + 1) + x_start - x1;
                color_rgb565_t *pixel = (color_rgb565_t *)(uintptr_t)image_base + read_offset;

                if (i <= raster_region_start)
                {
                    memcpy(writebuf + write_offset, (uint16_t *)(uintptr_t)pixel, len * 2);
                }
                else if (i <= raster_region_end)
                {
                    uint8_t alpha = (uint8_t)(0xff * (raster_region_end - i) /
                                              raster_region_length);
                    uint32_t j = 0;
#if RGB565_BLIT_USE_MVE
                    uint32x4_t alpha_v = vdupq_n_u32(alpha);
                    for (; j + 3 < len; j += 4)
                    {
                        uint32x4_t fg_v = vldrhq_u32((const uint16_t *)(pixel + j));
                        uint32x4_t bg_v = vldrhq_u32(&writebuf[write_offset + j]);
                        uint32x4_t out = rgb565_fast_blending_mve_x4(fg_v, bg_v, alpha_v);
                        vstrhq_u32(&writebuf[write_offset + j], out);
                    }
#endif
                    for (; j < len; j++)
                    {
                        writebuf[write_offset + j] = rgb565_fast_blending(pixel[j].color.rgb565,
                                                                          writebuf[write_offset + j],
                                                                          alpha);
                    }
                }
                else
                {
                    break;
                }
            }
        }
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
    else if (image->blend_mode == IMG_RASTER_HORIZONTAL || image->blend_mode == IMG_RASTER_VERTICAL)
    {
        rgb565_2_rgb565_1d_raster(image, dc, rect);
    }
    else
    {
        rgb565_2_rgb565_1d_bypass(image, dc, rect);
    }
}

static void rgb565_2_rgb565_2d_filter(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;
    if (opacity_value == 0)
    {
        return;// fully transparent, nothing to draw
    }
    else if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_2D(
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
        PROCESS_IMAGE_PIXEL_2D(
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

static void rgb565_2_rgb565_2d_filter_with_2x2_aa(draw_img_t *image, gui_dispdev_t *dc,
                                                  gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;
    if (opacity_value == 0)
    {
        return;// fully transparent, nothing to draw
    }
    else if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_2D_WITH_2X2_ANTI_ALIASING(
            color_rgb565_t,
        {
            writebuf[write_offset] = pixel_aliasing_2x2_without_alpha(
                pixel_00->color.rgb565,
                pixel_01->color.rgb565,
                pixel_10->color.rgb565,
                pixel_11->color.rgb565,
                (uint8_t)(xRatio * 255),
                (uint8_t)(yRatio * 255),
                writebuf[write_offset]);
        };
        );
    }
    else
    {
        PROCESS_IMAGE_PIXEL_2D_WITH_2X2_ANTI_ALIASING(
            color_rgb565_t,
        {
            writebuf[write_offset] = pixel_aliasing_2x2(
                pixel_00->color.rgb565,
                pixel_01->color.rgb565,
                pixel_10->color.rgb565,
                pixel_11->color.rgb565,
                opacity_value,
                opacity_value,
                opacity_value,
                opacity_value,
                (uint8_t)(xRatio * 255),
                (uint8_t)(yRatio * 255),
                writebuf[write_offset]);
        };
        );
    }
}

static void rgb565_2_rgb565_2d_bypass(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;
    if (opacity_value == 0)
    {
        return;// fully transparent, nothing to draw
    }
    else if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_2D(
            color_rgb565_t,
        {
            writebuf[write_offset] = pixel->color.rgb565;
        };
        );
    }
    else
    {
        PROCESS_IMAGE_PIXEL_2D(
            color_rgb565_t,
        {
            writebuf[write_offset] = rgb565_fast_blending(pixel->color.rgb565, writebuf[write_offset], opacity_value);
        };
        );
    }
}

static void rgb565_2_rgb565_3d_bypass(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;
    if (opacity_value == 0)
    {
        return;// fully transparent, nothing to draw
    }
    else if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_3D(
            color_rgb565_t,
        {
            writebuf[write_offset] = pixel->color.rgb565;
        };
        );
    }
    else
    {
        PROCESS_IMAGE_PIXEL_3D(
            color_rgb565_t,
        {
            writebuf[write_offset] = rgb565_fast_blending(pixel->color.rgb565, writebuf[write_offset], opacity_value);
        };
        );
    }
}

static void rgb565_2_rgb565_3d_filter(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;
    if (opacity_value == 0)
    {
        return;// fully transparent, nothing to draw
    }
    else if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_3D(
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
        PROCESS_IMAGE_PIXEL_3D(
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

static void rgb565_2_rgb565_2d(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    if (image->blend_mode == IMG_BYPASS_MODE)
    {
        rgb565_2_rgb565_2d_bypass(image, dc, rect);
    }
    else if (image->blend_mode == IMG_FILTER_BLACK)
    {
        if (image->high_quality == true)
        {
            rgb565_2_rgb565_2d_filter_with_2x2_aa(image, dc, rect);
        }
        else
        {
            rgb565_2_rgb565_2d_filter(image, dc, rect);
        }
    }
    else
    {
        GUI_ASSERT(NULL);
    }
}

static void rgb565_2_rgb565_3d(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    if (image->blend_mode == IMG_BYPASS_MODE)
    {
        rgb565_2_rgb565_3d_bypass(image, dc, rect);
    }
    else if (image->blend_mode == IMG_FILTER_BLACK)
    {
        rgb565_2_rgb565_3d_filter(image, dc, rect);
    }
    else
    {
        GUI_ASSERT(NULL);
    }
}


void rgb565_2_rgb565(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
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
        rgb565_2_rgb565_1d(image, dc, rect);
    }
    else if ((image->matrix.m[2][2] == 1) && (image->matrix.m[2][0] == 0) &&
             (image->matrix.m[2][1] == 0))
    {
        rgb565_2_rgb565_2d(image, dc, rect);
    }
    else
    {
        rgb565_2_rgb565_3d(image, dc, rect);
    }

}

