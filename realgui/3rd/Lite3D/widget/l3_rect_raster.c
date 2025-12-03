/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_raster.c
  * @brief deal with no rle filter mode
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author howie_wang@realsil.com.cn
  * @date 2024/04/09
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "l3_sw_raster.h"
#include "l3_common.h"

void (*l3_draw_rect_img_to_canvas_imp)(l3_draw_rect_img_t *image, l3_canvas_t *dc,
                                       l3_rect_t *rect) = NULL;

void l3_draw_rect_img_to_canvas(l3_draw_rect_img_t *image, l3_canvas_t *dc, l3_rect_t *rect)
{
    if (l3_draw_rect_img_to_canvas_imp)
    {
        l3_draw_rect_img_to_canvas_imp(image, dc, rect);
        return;
    }
    l3_img_head_t *head = image->data;
    if (head->compress)
    {
        int32_t x_start = 0, x_end = 0, y_start = 0, y_end = 0;
        if (!l3_draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end)) { return; }

        char input_type = head->type;
        int16_t source_w = image->img_w, source_h = image->img_h;
        uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3, opacity_value = image->opacity_value;
        uint8_t *writebuf = dc->frame_buf;
        uint32_t blend_mode = image->blend_mode;
        l3_3x3_matrix_t *inverse = &image->inverse;

        gui_raster_params_t params = { writebuf, 0, 0, 0, input_type, dc_bytes_per_pixel, opacity_value, blend_mode };
        uint8_t rle_pixel[4];

        for (int32_t i = y_start; i <= y_end; i++)
        {
            for (int32_t j = x_start; j <= x_end; j++)
            {
                float X = inverse->u.m[0][0] * j + inverse->u.m[0][1] * i + inverse->u.m[0][2];
                float Y = inverse->u.m[1][0] * j + inverse->u.m[1][1] * i + inverse->u.m[1][2];
                float Z = inverse->u.m[2][0] * j + inverse->u.m[2][1] * i + inverse->u.m[2][2];
                int x = roundf(X / Z);
                int y = roundf(Y / Z);

                if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h)) { continue; }
                if (rect && ((x >= rect->x2) || (x < rect->x1) || (y < rect->y1) || (y >= rect->y2))) { continue; }

                int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + j - dc->section.x1;

                gui_get_rle_pixel(image, x, y, rle_pixel);
                params.image_base = (uintptr_t)rle_pixel;
                params.write_off = write_off;

                do_raster_pixel(&params);
            }
        }
    }
    else
    {
        int32_t x_start = 0, x_end = 0, y_start = 0, y_end = 0;
        if (!l3_draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end)) { return; }

        uint32_t image_base = sizeof(l3_img_head_t) + (uintptr_t)(image->data);

        char input_type = head->type;
        int16_t source_w = image->img_w, source_h = image->img_h;
        uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3, opacity_value = image->opacity_value;
        uint8_t *writebuf = dc->frame_buf;
        uint32_t blend_mode = image->blend_mode;
        l3_3x3_matrix_t *inverse = &image->inverse;

        gui_raster_params_t params = { writebuf, 0, image_base, 0, input_type, dc_bytes_per_pixel, opacity_value, blend_mode };

        for (int32_t i = y_start; i <= y_end; i++)
        {
            for (int32_t j = x_start; j <= x_end; j++)
            {
                float X = inverse->u.m[0][0] * j + inverse->u.m[0][1] * i + inverse->u.m[0][2];
                float Y = inverse->u.m[1][0] * j + inverse->u.m[1][1] * i + inverse->u.m[1][2];
                float Z = inverse->u.m[2][0] * j + inverse->u.m[2][1] * i + inverse->u.m[2][2];
                int x = round(X / Z);
                int y = round(Y / Z);

                if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h)) { continue; }
                if (rect && ((x > rect->x2) || (x < rect->x1) || (y < rect->y1) || (y > rect->y2))) { continue; }

                int read_off = (image->blend_mode == L3_IMG_RECT) ? 0 : y * source_w + x;
                params.image_off = read_off;
                params.write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + j -
                                   dc->section.x1;

                do_raster_pixel(&params);
            }
        }
    }

}
