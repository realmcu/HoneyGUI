/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_rle_filter.c
  * @brief Picture software decoding library:filter mode
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
#include "acc_engine.h"
#include "acc_sw_rle.h"

static void rle_filter_rgb565_draw(imdc_file_t *file, uint32_t line,  uint16_t *buf, int32_t img_x,
                                   int32_t remain_len)
{
    imdc_rgb565_node_t *img_line_start = (imdc_rgb565_node_t *)((uint8_t *)file +
                                                                file->compressed_addr[line]);
    uint16_t *pdc_draw = buf;
    // img_x: img location, img_draw_offset: img source x offset
    int32_t img_draw_offset = ((img_x >= 0) ? 0 : (-1 * img_x));
    // for calculate, init negative value
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
        int32_t len = _UI_MIN(pixel_pass, remain_len);
        if (node_start->pixel16 != 0)
        {
            gui_memset16(pdc_draw, node_start->pixel16, len);
        }
        pdc_draw += len;
        remain_len -= len;
        // DBG_DIRECT("len %d remain_len %d", len, remain_len);
    }

    // draw
    for (imdc_rgb565_node_t *node = node_start + 1; remain_len > 0;)
    {
        // DBG_DIRECT("node %d node->pixel16 0x%x  node->len %d linebuf %d remain_len %d", node, node->pixel16, node->len, linebuf, remain_len);
        if (node->pixel16 != 0)
        {
            // draw end
            gui_memset16(pdc_draw, node->pixel16, (node->len <= remain_len) ? node->len : remain_len);
        }
        pdc_draw += node->len;
        remain_len -= node->len;
        node ++;
    }
}


void rle_filter_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                              struct gui_rect *rect)
{
    int16_t image_x = rect->x1;
    int16_t image_y = rect->y1;
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = ((struct gui_rgb_data_head *)(image->data))->type;
    imdc_file_t *file = (imdc_file_t *)image_off;
    uint8_t opacity_value = image->opacity_value;

    if (opacity_value == 0)
    {
        return;
    }

    if (img_type == RGB565)//rle_filter_565_2_565
    {
        if (opacity_value == 255)
        {
            for (uint32_t i = y_start; i <= y_end; i++)
            {
                int line = i - image_y;
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                uint16_t *writebuf = (uint16_t *)dc->frame_buf;
                rle_filter_rgb565_draw(file, line,  &(writebuf[write_off + x_start]), image_x, (x_end - x_start));
            }
        }
        else
        {
            for (uint32_t i = y_start; i <= y_end; i++)
            {
                int line = i - image_y;
                int write_off = (i - dc->section.y1) * dc->fb_width ;
                uint16_t *writebuf = (uint16_t *)dc->frame_buf;
                uint8_t line_buf[BYTE_PIXEL_RGB565 * source_w];
                uncompressed_rle_rgb565(file, line, line_buf);
                int read_off = (int)line_buf - BYTE_PIXEL_RGB565 * image_x;

                for (uint32_t j = x_start; j <= x_end; j++)
                {
                    uint16_t pixel = *((uint16_t *)read_off + j);
                    if (pixel != 0)
                    {
                        writebuf[write_off + j] = ((((((pixel >> 11) << 3) * opacity_value +
                                                      ((writebuf[write_off + j] >> 11) << 3) * (0xFF - opacity_value)) / 255) >> 3) << 11) +
                                                  ((((((((pixel & 0x07e0) >> 5) << 2) * opacity_value) + (((writebuf[write_off + j] & 0x07e0) >> 5) <<
                                                          2) * (0xFF - opacity_value)) / 0xFF) >> 2) << 5) +
                                                  ((((((pixel & 0x001f) << 3) * opacity_value) + ((writebuf[write_off + j]  & 0x001f) << 3) *
                                                     (0xFF - opacity_value)) / 0xFF) >> 3);
                    }
                }
            }
        }
    }
}
