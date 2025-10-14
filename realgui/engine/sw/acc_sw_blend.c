/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_blend.c
  * @brief Picture software decoding library blend
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/19
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>
#include "acc_sw_blend.h"

static uint16_t do_blending_acc_2_rgb565_opacity(uint32_t fg, uint32_t bg, uint8_t alpha)
{
    // Alpha converted from [0..255] to [0..31]
    // Converts  0000000000000000rrrrrggggggbbbbb
    //     into  00000gggggg00000rrrrr000000bbbbb
    // with mask 00000111111000001111100000011111
    // This is useful because it makes space for a parallel fixed-point multiply

    alpha = (alpha + 4) >> 3;
    bg = ((bg | (bg << 16)) & 0x07e0f81f);
    fg = ((fg | (fg << 16)) & 0x07e0f81f);
    uint32_t result = (((((fg - bg) * alpha) >> 5) + bg) & 0x07e0f81f);

    return (uint16_t)((result >> 16) | result);
}

void src_over_blit_2_rgb565(draw_img_t *image, gui_dispdev_t *dc,
                            gui_rect_t *rect)////S * Sa + (1 - Sa) * D
{
    int32_t x_start = 0;
    int32_t x_end = 0;
    int32_t y_start = 0;
    int32_t y_end = 0;

    if (draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_base = sizeof(gui_rgb_data_head_t) + (uint32_t)(uintptr_t)(image->data);
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;

    int16_t source_w = image->img_w;

    gui_matrix_t *inverse = &image->inverse;
    float m12 = inverse->m[1][2];
    float m02 = inverse->m[0][2];

    int16_t y1 = dc->section.y1;
    int16_t x1 = dc->section.x1;
    int16_t x2 = dc->section.x2;
    uint8_t opacity_value = image->opacity_value;
    if (opacity_value == 0)
    {
        // fully transparent, nothing to draw
        return;
    }
    else if (opacity_value == 255)
    {
        for (int32_t i = y_start; i <= y_end; i++)
        {
            int write_off = (i - y1) * (x2 - x1 + 1) + x_start - x1;
            color_argb8565_value_t *pixel = (color_argb8565_value_t *)(uintptr_t)image_base + (uint32_t)((
                                                i + m12) * source_w + x_start + m02);

            for (int32_t j = x_start; j <= x_end; j++)
            {
                writebuf[write_off] = do_blending_acc_2_rgb565_opacity(pixel->rgb, writebuf[write_off], pixel->a);
                write_off++;
                pixel++;
            }
        }
    }
    else
    {
        for (int32_t i = y_start; i <= y_end; i++)
        {
            int write_off = (i - y1) * (x2 - x1 + 1) + x_start - x1;
            color_argb8565_value_t *pixel = (color_argb8565_value_t *)(uintptr_t)image_base + (uint32_t)((
                                                i + m12) * source_w + x_start + m02);

            for (int32_t j = x_start; j <= x_end; j++)
            {
                uint8_t alpha = pixel->a;
                alpha = (alpha * opacity_value) >> 8;
                writebuf[write_off] = do_blending_acc_2_rgb565_opacity(pixel->rgb, writebuf[write_off], alpha);

                write_off++;
                pixel++;
            }
        }
    }



}

#include <stdint.h>

static inline uint16_t argb8888_to_rgb565(uint32_t argb)
{
    uint8_t b8 = (uint8_t)(argb & 0xFF);
    uint8_t g8 = (uint8_t)((argb >> 8) & 0xFF);
    uint8_t r8 = (uint8_t)((argb >> 16) & 0xFF);
    // Alpha (argb >> 24) not used in RGB565 conversion

    uint16_t r5 = (uint16_t)(r8 >> 3);
    uint16_t g6 = (uint16_t)(g8 >> 2);
    uint16_t b5 = (uint16_t)(b8 >> 3);

    return (uint16_t)((r5 << 11) | (g6 << 5) | b5);
}


void preconfig_a8_fgbg(draw_img_t *image, gui_dispdev_t *dc,
                       gui_rect_t *rect)////fixed fg and bg
{
    int32_t x_start = 0;
    int32_t x_end = 0;
    int32_t y_start = 0;
    int32_t y_end = 0;

    if (draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_base = sizeof(gui_rgb_data_head_t) + (uint32_t)(uintptr_t)(image->data);
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;

    int16_t source_w = image->img_w;

    gui_matrix_t *inverse = &image->inverse;
    float m12 = inverse->m[1][2];
    float m02 = inverse->m[0][2];

    int16_t y1 = dc->section.y1;
    int16_t x1 = dc->section.x1;
    int16_t x2 = dc->section.x2;


    uint32_t fg_rgb565 = argb8888_to_rgb565(image->fg_color_mix);
    uint32_t bg_rgb565 = argb8888_to_rgb565(image->bg_color_mix);
    uint8_t opacity_value = image->opacity_value;

    if (opacity_value == 0)
    {
        // fully transparent, nothing to draw
        return;
    }
    else if (opacity_value == 255)
    {
        for (int32_t i = y_start; i <= y_end; i++)
        {
            int write_off = (i - y1) * (x2 - x1 + 1) + x_start - x1;
            color_a8_t *a = (color_a8_t *)(uintptr_t)image_base + (uint32_t)((i + m12) * source_w + x_start +
                                                                             m02);

            for (int32_t j = x_start; j <= x_end; j++)
            {
                writebuf[write_off] = do_blending_acc_2_rgb565_opacity(fg_rgb565, bg_rgb565, a->a);
                write_off++;
                a++;
            }
        }
    }
    else
    {
        for (int32_t i = y_start; i <= y_end; i++)
        {
            int write_off = (i - y1) * (x2 - x1 + 1) + x_start - x1;
            color_a8_t *a = (color_a8_t *)(uintptr_t)image_base + (uint32_t)((i + m12) * source_w + x_start +
                                                                             m02);

            for (int32_t j = x_start; j <= x_end; j++)
            {
                uint8_t alpha = a->a;
                alpha = (alpha * opacity_value) >> 8;
                writebuf[write_off] = do_blending_acc_2_rgb565_opacity(fg_rgb565, bg_rgb565, alpha);

                write_off++;
                a++;
            }
        }
    }


}


void preconfig_a8_fg(draw_img_t *image, gui_dispdev_t *dc,
                     gui_rect_t *rect)////fixed fg
{
    int32_t x_start = 0;
    int32_t x_end = 0;
    int32_t y_start = 0;
    int32_t y_end = 0;

    if (draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_base = sizeof(gui_rgb_data_head_t) + (uint32_t)(uintptr_t)(image->data);
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;

    int16_t source_w = image->img_w;

    gui_matrix_t *inverse = &image->inverse;
    float m12 = inverse->m[1][2];
    float m02 = inverse->m[0][2];

    int16_t y1 = dc->section.y1;
    int16_t x1 = dc->section.x1;
    int16_t x2 = dc->section.x2;


    uint32_t fg_rgb565 = argb8888_to_rgb565(image->fg_color_mix);
    uint8_t opacity_value = image->opacity_value;

    if (opacity_value == 0)
    {
        // fully transparent, nothing to draw
        return;
    }
    else if (opacity_value == 255)
    {
        for (int32_t i = y_start; i <= y_end; i++)
        {
            int write_off = (i - y1) * (x2 - x1 + 1) + x_start - x1;
            color_a8_t *a = (color_a8_t *)(uintptr_t)image_base + (uint32_t)((i + m12) * source_w + x_start +
                                                                             m02);

            for (int32_t j = x_start; j <= x_end; j++)
            {
                writebuf[write_off] = do_blending_acc_2_rgb565_opacity(fg_rgb565, writebuf[write_off], a->a);
                write_off++;
                a++;
            }
        }
    }
    else
    {
        for (int32_t i = y_start; i <= y_end; i++)
        {
            int write_off = (i - y1) * (x2 - x1 + 1) + x_start - x1;
            color_a8_t *a = (color_a8_t *)(uintptr_t)image_base + (uint32_t)((i + m12) * source_w + x_start +
                                                                             m02);

            for (int32_t j = x_start; j <= x_end; j++)
            {
                uint8_t alpha = a->a;
                alpha = (alpha * opacity_value) >> 8;
                writebuf[write_off] = do_blending_acc_2_rgb565_opacity(fg_rgb565, writebuf[write_off], alpha);

                write_off++;
                a++;
            }
        }
    }

}

