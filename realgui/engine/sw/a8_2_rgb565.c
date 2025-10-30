/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file a8_2_rgb565.c
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

    opacity_value = image->opacity_value * image->alpha_mix / 255;

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

static void a8_2_rgb565_2d_fix_bg(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;
    if (opacity_value == 0)
    {
        return;// fully transparent, nothing to draw
    }
    else if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_2D(
            color_a8_t,
        {
            writebuf[write_offset] = rgb565_fast_blending(fg, bg, pixel->a);
        };
        );
    }
    else
    {
        PROCESS_IMAGE_PIXEL_2D(
            color_a8_t,
        {
            writebuf[write_offset] = rgb565_fast_blending(fg, bg, pixel->a * opacity_value >> 8);
        };
        );
    }
}

static void a8_2_rgb565_2d_use_bg(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;
    if (opacity_value == 0)
    {
        return;// fully transparent, nothing to draw
    }
    else if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_2D(
            color_a8_t,
        {
            writebuf[write_offset] = rgb565_fast_blending(fg, writebuf[write_offset], pixel->a);
        };
        );
    }
    else
    {
        PROCESS_IMAGE_PIXEL_2D(
            color_a8_t,
        {
            writebuf[write_offset] = rgb565_fast_blending(fg, writebuf[write_offset], pixel->a * opacity_value >> 8);
        };
        );
    }
}

static void a8_2_rgb565_2d_use_bg_with_aa(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;
    if (opacity_value == 0)
    {
        return;// fully transparent, nothing to draw
    }
    else if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_2D_WITH_1X2_ANTI_ALIASING(
            color_a8_t,
        {
            writebuf[write_offset] = pixel_aliasing_a8_1x2(fg, writebuf[write_offset], pixel_00->a, pixel_01->a, xRatio);
        };
        );
    }
    else
    {
        PROCESS_IMAGE_PIXEL_2D_WITH_1X2_ANTI_ALIASING(
            color_a8_t,
        {
            writebuf[write_offset] = pixel_aliasing_a8_1x2(fg, writebuf[write_offset], pixel_00->a * opacity_value >> 8, pixel_01->a * opacity_value >> 8, xRatio);
        };
        );
    }
}



static void a8_2_rgb565_2d(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    if (image->blend_mode == IMG_2D_SW_FIX_A8_FG)
    {
        if (image->high_quality == true)
        {
            a8_2_rgb565_2d_use_bg_with_aa(image, dc, rect);
        }
        else
        {
            a8_2_rgb565_2d_use_bg(image, dc, rect);
        }
    }
    else if (image->blend_mode == IMG_2D_SW_FIX_A8_BGFG)
    {
        a8_2_rgb565_2d_fix_bg(image, dc, rect);
    }
}

static void a8_2_rgb565_3d(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    GUI_UNUSED(image);
    GUI_UNUSED(dc);
    GUI_UNUSED(rect);
    GUI_ASSERT(0);
}


void a8_2_rgb565(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
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
        a8_2_rgb565_1d(image, dc, rect);
    }
    else if ((image->matrix.m[2][2] == 1) && (image->matrix.m[2][0] == 0) &&
             (image->matrix.m[2][1] == 0))
    {
        a8_2_rgb565_2d(image, dc, rect);
    }
    else
    {
        a8_2_rgb565_3d(image, dc, rect);
    }

}

