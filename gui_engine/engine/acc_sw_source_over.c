/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_source_over.c
  * @brief Picture software decoding library
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
#include "acc_engine.h"
#include "acc_sw_blend.h"

void rect_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                   gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;

    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    gui_color_t reverse =
    {
        .color.rgba.a = ((gui_rect_file_head_t *)(image->data))->color.color.rgba.a,
        .color.rgba.r = ((gui_rect_file_head_t *)(image->data))->color.color.rgba.b,
        .color.rgba.b = ((gui_rect_file_head_t *)(image->data))->color.color.rgba.r,
        .color.rgba.g = ((gui_rect_file_head_t *)(image->data))->color.color.rgba.g
    };

    for (uint32_t i = y_start; i <= y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;


        uint8_t *writebuf = dc->frame_buf;

        for (uint32_t j = x_start; j <= x_end; j++)
        {

            uint8_t opacity_value = image->opacity_value;
            switch (opacity_value)
            {
            case 0:
                break;
            case 255:
                {
                    uint16_t *d = (uint16_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);

                    do_blending_2_rgb565(d, &reverse);
                }
                break;
            default:
                {
                    uint16_t *d = (uint16_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                    do_blending_2_rgb565_opacity(d, &reverse, opacity_value);
                }
                break;
            }
        }
    }
    return;
}


void rect_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                     gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;

    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    gui_color_t reverse =
    {
        .color.rgba.a = ((gui_rect_file_head_t *)(image->data))->color.color.rgba.a,
        .color.rgba.r = ((gui_rect_file_head_t *)(image->data))->color.color.rgba.b,
        .color.rgba.b = ((gui_rect_file_head_t *)(image->data))->color.color.rgba.r,
        .color.rgba.g = ((gui_rect_file_head_t *)(image->data))->color.color.rgba.g
    };

    for (uint32_t i = y_start; i <= y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;


        uint8_t *writebuf = dc->frame_buf;

        for (uint32_t j = x_start; j <= x_end; j++)
        {

            uint8_t opacity_value = image->opacity_value;
            switch (opacity_value)
            {
            case 0:
                break;
            case 255:
                {
                    gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);

                    do_blending_2_argb8888(d, &reverse);
                }
                break;
            default:
                {
                    gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                    do_blending_2_argb8888_opacity(d, &reverse, opacity_value);
                }
                break;
            }
        }
    }
    return;
}
