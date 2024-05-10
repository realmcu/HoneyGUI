/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw.c
  * @brief deal with no rle filter mode
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
void filter_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                          gui_rect_t *rect)
{
    int16_t image_x = image->img_target_x;
    int16_t image_y = image->img_target_y;
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;

    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(uintptr_t)(image->data);
    int read_x_off = -_UI_MIN(image_x, 0) * BYTE_PIXEL_RGB565  + image_off;

    uint32_t pixel;
    uint32_t line_len = x_end - x_start + 1;
    for (uint32_t i = y_start; i <= y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;
        int read_off = read_x_off + ((i - image_y) * source_w) * BYTE_PIXEL_RGB565;
        uint16_t *writebuf = (uint16_t *)dc->frame_buf + write_off + x_start;

        for (uint32_t j = 0; j < line_len;)
        {
            pixel = *((uint32_t *)((uint16_t *)(uintptr_t)read_off + j));
            if (pixel)
            {
                if ((uint16_t)pixel)
                {
                    writebuf[j] = pixel;
                }
                j++;
                if ((uint16_t)(pixel >> 16) && (j < line_len))
                {
                    writebuf[j] = pixel >> 16;
                }
                j++;
                continue;
            }
            j += 2;
        }
    }
}



