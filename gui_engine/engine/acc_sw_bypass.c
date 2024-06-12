/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_bypass.c
  * @brief deal with no rle bypass mode
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

void bypass_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                          gui_rect_t *rect)
{
    int16_t image_x = image->img_target_x;
    int16_t image_y = image->img_target_y;
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    if (draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(uintptr_t)(image->data);
    uint8_t opacity_value = image->opacity_value;

    int read_x_off = -_UI_MIN(image_x, 0) * BYTE_PIXEL_RGB565  + image_off;
    uint16_t write_off = (y_start - dc->section.y1) * dc->fb_width ;
    uint16_t *writebuf = (uint16_t *)dc->frame_buf + write_off;
    int read_buf = read_x_off + BYTE_PIXEL_RGB565 * ((y_start - image_y) * source_w) -
                   BYTE_PIXEL_RGB565 * x_start;
    uint16_t img_line_byte = source_w * BYTE_PIXEL_RGB565;

    // full screen background image, memcpy once
    if ((source_w == dc->fb_width) && (dc->fb_width == (x_end - x_start)) && (opacity_value == 255))
    {
        memcpy((writebuf + x_start), ((uint16_t *)(uintptr_t)read_buf + x_start),
               BYTE_PIXEL_RGB565 * (x_end - x_start) * (y_end - y_start));
    }
    else
    {
        for (uint32_t i = y_start; i <= y_end; i++)
        {
            // memcpy line
            memcpy((writebuf + x_start), ((uint16_t *)(uintptr_t)read_buf + x_start),
                   BYTE_PIXEL_RGB565 * (x_end - x_start));
            // next line
            writebuf += dc->fb_width;
            read_buf += img_line_byte;
        }
    }

    return;
}
