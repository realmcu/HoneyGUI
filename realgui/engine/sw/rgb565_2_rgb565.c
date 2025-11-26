/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file rgb565_2_rgb565.c
  * @brief
  * @author howie_wang@realsil.com.cn
  * @date 2025/10/20
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>
#include "acc_sw_define.h"

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

