/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_cover.c
  * @brief deal with no rle direct cover mode
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author wenjing_jiang@realsil.com.cn
  * @date 2023/12/23
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

void cover_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                         gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;

    if (draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }
    uint32_t image_base = sizeof(gui_rgb_data_head_t) + (uint32_t)(uintptr_t)(image->data);
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;

    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;

    for (uint32_t i = y_start; i <= y_end; i++)
    {
        for (uint32_t j = x_start; j <= x_end; j++)
        {
            if ((j >= source_w) || (j < 0) || (i < 0) || (i >= source_h))
            {
                continue;
            }
            if (rect != NULL)
            {
                if ((j >= rect->x2) || (j < rect->x1) || (i < rect->y1) || (i >= rect->y2))
                {
                    continue;
                }
            }

            int read_off = i * source_w + j;
            int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + j - dc->section.x1;

            uint16_t pixel = *((uint16_t *)(uintptr_t)image_base + read_off);
            writebuf[write_off] = pixel;
        }
    }
}

