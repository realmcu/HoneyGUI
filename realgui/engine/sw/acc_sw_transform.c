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

    float m00 = inverse->m[0][0];
    float m01 = inverse->m[0][1];
    float m02 = inverse->m[0][2];
    float m10 = inverse->m[1][0];
    float m11 = inverse->m[1][1];
    float m12 = inverse->m[1][2];


    for (int32_t i = y_start; i <= y_end; i++)
    {
        float detalX = m01 * i + m02;
        float detalY = m11 * i + m12;
        float X = m00 * x_start + detalX;
        float Y = m10 * x_start + detalY;

        int write_offset = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) - dc->section.x1;

        for (int32_t j = x_start; j <= x_end; j++)
        {
            //int x = roundf(X);
            //int y = roundf(Y);

            if ((X >= source_w) || (X < 0) || (Y < 0) || (Y >= source_h))
            {
                X += m00;
                Y += m10;
                continue;
            }

            int write_off = write_offset + j;
            int image_off = (image->blend_mode == IMG_RECT) ? 0 : Y * source_w + X;

            writebuf[write_off] = readbuf[image_off];
            X += m00;
            Y += m10;
        }
    }
}
