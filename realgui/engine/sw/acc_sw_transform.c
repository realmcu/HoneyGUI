/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_transform.c
  * @author howie_wang@realsil.com.cn
  * @date 2023/12/23
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
#include <math.h>

void sw_transform_for_rgb565(draw_img_t *image, gui_dispdev_t *dc,
                             gui_rect_t *rect)
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
    uint16_t *readbuf = (uint16_t *)(uintptr_t)image_base;

    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    gui_matrix_t *inverse = &image->inverse;

    for (int32_t i = y_start; i <= y_end; i++)
    {
        for (int32_t j = x_start; j <= x_end; j++)
        {
            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            int x = roundf(X);
            int y = roundf(Y);

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h)) { continue; }
            if (rect && ((x > rect->x2) || (x < rect->x1) || (y < rect->y1) || (y > rect->y2))) { continue; }

            int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + j - dc->section.x1;
            int image_off = (image->blend_mode == IMG_RECT) ? 0 : y * source_w + x;

            writebuf[write_off] = readbuf[image_off];

        }
    }
}
