/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_rle_bypass.c
  * @brief Picture software decoding library:bypass mode
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author wenjing_jiang@realsil.com.cn
  * @date 2023/12/25
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
#include "acc_sw_rle.h"

static void rle_bypass_rgb565_draw(imdc_file_t *file, uint32_t line,  uint16_t *buf, int32_t img_x,
                                   int32_t remain_len)
{
    imdc_rgb565_node_t *img_line_start = (imdc_rgb565_node_t *)((uint8_t *)file +
                                                                file->compressed_addr[line]);
    uint16_t *pdc_draw = buf;
    int32_t img_draw_offset = ((img_x >= 0) ? 0 : (-1 * img_x));
    int32_t pixel_pass = -1 * img_draw_offset;
    // DBG_DIRECT("img_draw_offset %d remain_len %d", img_draw_offset, remain_len);

    imdc_rgb565_node_t *node_start;
    // find img draw start node by pixel
    for (node_start = img_line_start - 1; pixel_pass <= 0;)
    {
        node_start++;
        pixel_pass += node_start->len;
    }
    // DBG_DIRECT("pixel_pass %d node_start->len %d remain_len %d", pixel_pass, node_start->len, remain_len);
    // draw first node
    {
        uint32_t len = _UI_MIN(pixel_pass, remain_len);
        gui_memset16(pdc_draw, node_start->pixel16, len);
        pdc_draw += len;
        remain_len -= len;
        // DBG_DIRECT("len %d remain_len %d", len, remain_len);
    }

    // draw
    for (imdc_rgb565_node_t *node = node_start + 1; remain_len > 0;)
    {
        // DBG_DIRECT("node %d node->pixel16 0x%x  node->len %d linebuf %d remain_len %d", node, node->pixel16, node->len, linebuf, remain_len);
        gui_memset16(pdc_draw, node->pixel16, (node->len <= remain_len) ? node->len : remain_len);
        pdc_draw += node->len;
        remain_len -= node->len;
        node ++;
    }
}
void rle_bypass_blit_2_rgb565(draw_img_t *image, gui_dispdev_t *dc,
                              struct gui_rect *rect)
{
    int32_t x_start = 0;
    int32_t x_end = 0;
    int32_t y_start = 0;
    int32_t y_end = 0;

    if (draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(uintptr_t)(image->data);
    imdc_file_t *file = (imdc_file_t *)(uintptr_t)image_off;

    gui_matrix_t *inverse = &image->inverse;
    int x = x_start + inverse->m[0][2];

    for (uint32_t i = y_start; i <= y_end; i++)
    {
        int y = i + inverse->m[1][2];
        int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + x_start -
                        dc->section.x1;
        uint16_t *writebuf = (uint16_t *)dc->frame_buf;
        // memset line
        rle_bypass_rgb565_draw(file, y,  &(writebuf[write_off]), -x, (x_end - x_start + 1));
        write_off++;
    }
}
