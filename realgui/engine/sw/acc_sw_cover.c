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

void cover_blit_2_rgb565(draw_img_t *image, gui_dispdev_t *dc,
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

    int16_t source_w = image->img_w;
    gui_matrix_t *inverse = &image->inverse;
    uint32_t len = x_end - x_start + 1;

    for (uint32_t i = y_start; i <= y_end; i++)
    {
        uint32_t source_off = (i + inverse->m[1][2]) * source_w + x_start + inverse->m[0][2];
        uint32_t des_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + x_start -
                           dc->section.x1;
        memcpy(writebuf + des_off, (uint16_t *)(uintptr_t)image_base + source_off, 2 * len);
    }
}

