/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_argb8565.c
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

static void argb8565_2_rgb565_1d(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    argb8565_2_rgb565_1d_src_over(image, dc, rect);
}

static void argb8565_2_rgb565_2d_src_over(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    SETUP_DRAW_VARIABLES;
    if (opacity_value == 0)
    {
        return;// fully transparent, nothing to draw
    }
    else if (opacity_value == 255)
    {
        PROCESS_IMAGE_PIXEL_2D(
            color_argb8565_t,
        {
            writebuf[write_offset] = rgb565_fast_blending(pixel->color.rgb565, writebuf[write_offset], pixel->a);
        };
        );
    }
    else
    {
        PROCESS_IMAGE_PIXEL_2D(
            color_argb8565_t,
        {
            writebuf[write_offset] = rgb565_fast_blending(pixel->color.rgb565, writebuf[write_offset], pixel->a * opacity_value >> 8);
        };
        );
    }
}



static void argb8565_2_rgb565_2d(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    argb8565_2_rgb565_2d_src_over(image, dc, rect);
}

static void argb8565_2_rgb565_3d(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    GUI_UNUSED(image);
    GUI_UNUSED(dc);
    GUI_UNUSED(rect);
    GUI_ASSERT(0);
}


void argb8565_2_rgb565(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
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
        argb8565_2_rgb565_1d(image, dc, rect);
    }
    else if ((image->matrix.m[2][2] == 1) && (image->matrix.m[2][0] == 0) &&
             (image->matrix.m[2][1] == 0))
    {
        argb8565_2_rgb565_2d(image, dc, rect);
    }
    else
    {
        argb8565_2_rgb565_3d(image, dc, rect);
    }

}

