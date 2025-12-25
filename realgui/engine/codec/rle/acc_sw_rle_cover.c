/**
*****************************************************************************************
*     Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_rle_cover.c
  * @brief Picture software decoding library:cover mode
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author wenjing_jiang@realsil.com.cn
  * @date 2023/12/25
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2023 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */
/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "draw_img.h"
#include <stdio.h>
#include <stdint.h>
#include "acc_sw_rle.h"

void rle_cover_blit_2_rgb565(draw_img_t *image, gui_dispdev_t *dc,
                             struct gui_rect *rect)
{
    int32_t x_start = 0;
    int32_t x_end = 0;
    int32_t y_start = 0;
    int32_t y_end = 0;
    int16_t source_w = image->img_w;

    if (draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_off = sizeof(gui_rgb_data_head_t) + (uintptr_t)(image->data);

    imdc_file_t *file = (imdc_file_t *)(uintptr_t)image_off;
    uint8_t line_buf[BYTE_PIXEL_RGB565 * source_w];
    gui_matrix_t *inverse = &image->inverse;

    for (int32_t i = y_start; i <= y_end; i++)
    {
        int y = i + inverse->m[1][2];
        int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + x_start -
                        dc->section.x1;
        uncompressed_rle_rgb565(file, y, line_buf);
        uint16_t *writebuf = (uint16_t *)dc->frame_buf;

        for (int32_t j = x_start; j <= x_end; j++)
        {
            int x = j + inverse->m[0][2];
            uint16_t pixel = *((uint16_t *)(uintptr_t)line_buf + x);
            writebuf[write_off] = pixel;
            write_off++;
        }
    }

}
