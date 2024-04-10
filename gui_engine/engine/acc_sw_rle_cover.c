/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_rle_cover.c
  * @brief Picture software decoding library:cover mode
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

void rle_cover_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                             struct gui_rect *rect)
{
    int16_t image_x = rect->x1;
    int16_t image_y = rect->y1;
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
//    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = ((struct gui_rgb_data_head *)(image->data))->type;
    imdc_file_t *file = (imdc_file_t *)image_off;

    if (img_type == RGB565)//rle_cover_565_2_565
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * source_w];

        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int line = i - image_y;

            uncompressed_rle_rgb565(file, line, line_buf);

            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;

            uint16_t *writebuf = (uint16_t *)dc->frame_buf;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint16_t pixel = *((uint16_t *)read_off + j);
                writebuf[write_off + j] = pixel;
            }
        }
    }
    else if (img_type == RGB888) //rle_cover_rgb888_2_rgb565
    {
        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * source_w];
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int line = i - image_y;
            uncompressed_rle_rgb888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;

            uint16_t *writebuf = (uint16_t *) dc->frame_buf;

            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                uint8_t *pixel = (uint8_t *)(read_off + x_j * source_bytes_per_pixel);
                writebuf[write_off + x_j] = (uint16_t)(((pixel[2]) >> 3) << 11) + (uint16_t)(((pixel[1]) >> 2) << 5)
                                            + (uint16_t)(((pixel[0]) >> 3)); //RGB  565
            }
        }
    }
    else if (img_type == RGBA8888) //rle_cover_argb8888_2_rgb565
    {
        uint8_t source_bytes_per_pixel = 4;
        uint8_t line_buf[source_bytes_per_pixel * source_w];
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int line = i - image_y;
            uncompressed_rle_argb8888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;

            uint16_t *writebuf = (uint16_t *) dc->frame_buf;

            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                uint8_t *pixel = (uint8_t *)(read_off + x_j * source_bytes_per_pixel);
                writebuf[write_off + x_j] = (uint16_t)(((pixel[2]) >> 3) << 11) + (uint16_t)(((pixel[1]) >> 2) << 5)
                                            + (uint16_t)(((pixel[0]) >> 3)); //RGB  565
            }
        }
    }
    return;
}
void rle_cover_blit_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                             struct gui_rect *rect)
{
    int16_t image_x = rect->x1;
    int16_t image_y = rect->y1;
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
//    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = ((struct gui_rgb_data_head *)(image->data))->type;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    if (img_type == RGBA8888)//rle_cover_rgba8888_rgb888
    {
        uint8_t source_bytes_per_pixel = 4;
        uint8_t line_buf[source_bytes_per_pixel * source_w];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;
            int line = i - image_y;
            uncompressed_rle_argb8888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;
            uint8_t *pixel;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                pixel = (uint8_t *)(read_off + x_j * source_bytes_per_pixel);
                writebuf[(write_off + x_j) * dc_bytes_per_pixel] = pixel[0];//b
                writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
            }
        }
    }
    else if (img_type == RGB888) //rle_cover_rgb888_2_rgb888
    {
        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * source_w];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int line = i - image_y;
            uncompressed_rle_rgb888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                uint8_t *pixel = (uint8_t *)(read_off + x_j * source_bytes_per_pixel);

                writebuf[(write_off + x_j) * dc_bytes_per_pixel] = pixel[0];//b
                writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
            }

        }
    }
    else if (img_type == RGB565) //rle_cover_rgb565_2_rgb888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * source_w];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;
            int line = i - image_y;
            uncompressed_rle_rgb565(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;

            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                uint16_t pixel = *((uint16_t *)(read_off + x_j * source_bytes_per_pixel));
                writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
                writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
                writebuf[(write_off + x_j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;

            }
        }

    }
    return;
}
void rle_cover_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                               struct gui_rect *rect)
{
    int16_t image_x = rect->x1;
    int16_t image_y = rect->y1;
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
//    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = ((struct gui_rgb_data_head *)(image->data))->type;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    if (img_type == RGBA8888)
    {
        uint8_t source_bytes_per_pixel = 4;
        uint8_t line_buf[source_bytes_per_pixel * source_w];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;
            int line = i - image_y;
            uncompressed_rle_argb8888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;
            uint8_t *pixel;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                pixel = (uint8_t *)(read_off + x_j * source_bytes_per_pixel);
                writebuf[(write_off + x_j) * dc_bytes_per_pixel] = pixel[0];//b
                writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
                writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = pixel[3]; //a
            }
        }
    }
    else if (img_type == RGB888) //rle_cover_rgb888_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * source_w];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int line = i - image_y;
            uncompressed_rle_rgb888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                uint8_t *pixel = (uint8_t *)(read_off + x_j * source_bytes_per_pixel);
                writebuf[(write_off + x_j) * dc_bytes_per_pixel] = pixel[0];//b
                writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
                writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = 0xFF; //a
            }

        }
    }
    else if (img_type == RGB565) //rle_cover_rgb565_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * source_w];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;
            int line = i - image_y;
            uncompressed_rle_rgb565(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;

            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                uint16_t pixel = *((uint16_t *)(read_off + x_j * source_bytes_per_pixel));
                writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
                writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
                writebuf[(write_off + x_j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;
                writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = 0xFF; //a

            }
        }

    }
    return;
}
