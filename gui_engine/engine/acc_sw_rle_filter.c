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

void rle_filter_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                              struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = image->img_w;
    int image_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = *((uint8_t *)image_off);
    imdc_file_t *file = (imdc_file_t *)image_off;
    uint8_t opacity_value = image->opacity_value;

    if (img_type == 4)//rle_filter_565_2_565
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * image_w];

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
                if (pixel != 0)
                {
                    switch (opacity_value)
                    {
                    case 0:
                        break;
                    case 255:
                        {
                            writebuf[write_off + j] = pixel;
                        }
                        break;
                    default:
                        {
                            if (opacity_value < 255)
                            {
                                writebuf[write_off + j] = ((((((pixel >> 11) << 3) * opacity_value + ((
                                                                                                          writebuf[write_off + j] >> 11) << 3) * (0xFF - opacity_value)) / 255) >> 3) << 11) +
                                                          ((((((((pixel & 0x07e0) >> 5) << 2) * opacity_value) + (((writebuf[write_off + j] & 0x07e0) >> 5) <<
                                                                  2) * (0xFF - opacity_value)) / 0xFF) >> 2) << 5) +
                                                          ((((((pixel & 0x001f) << 3) * opacity_value) + ((writebuf[write_off + j]  & 0x001f) << 3) *
                                                             (0xFF - opacity_value)) / 0xFF) >> 3);
                            }
                            else
                            {
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    else if (img_type == 68) //rle_filter_rgb888_2_rgb565
    {
        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
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
                if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
                {
                    switch (opacity_value)
                    {
                    case 0:
                        break;
                    case 255:
                        {
                            writebuf[write_off + x_j] = (uint16_t)(((pixel[2]) >> 3) << 11) + (uint16_t)(((pixel[1]) >> 2) << 5)
                                                        + (uint16_t)(((pixel[0]) >> 3)); //RGB  565
                        }
                        break;
                    default:
                        {
                            if (opacity_value < 255)
                            {
                                writebuf[write_off + x_j] = (((((uint16_t)(pixel[0]) * opacity_value) + ((
                                                                                                             writebuf[write_off + x_j]  & 0x001f)
                                                                                                         << 3) * (0xFF - opacity_value)) / 0xFF) >> 3) +
                                                            ((((((uint16_t)(pixel[1]) * opacity_value) + (((writebuf[write_off + x_j] & 0x07e0) >> 5) << 2) *
                                                                (0xFF - opacity_value)) / 0xFF) >> 2) << 5) +
                                                            ((((((uint16_t)(pixel[2]) * (opacity_value)) + ((writebuf[write_off + x_j] >> 11) << 3) *
                                                                (0xFF - opacity_value)) / 0xFF) >> 3) << 11);
                            }
                            else
                            {
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    else if (img_type == 132) //rle_filter_argb8888_2_rgb565
    {
        uint8_t source_bytes_per_pixel = 4;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
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
                if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
                {
                    switch (opacity_value)
                    {
                    case 0:
                        break;
                    case 255:
                        {
                            writebuf[write_off + x_j] = (uint16_t)(((pixel[2]) >> 3) << 11) + (uint16_t)(((pixel[1]) >> 2) << 5)
                                                        + (uint16_t)(((pixel[0]) >> 3)); //RGB  565
                        }
                        break;
                    default:
                        {
                            if (opacity_value < 255)
                            {
                                uint8_t alpha = pixel[3] * opacity_value / 0xFF;
                                writebuf[write_off + x_j] = (((((uint16_t)(pixel[0]) * alpha) + ((writebuf[write_off + x_j]  &
                                                                                                  0x001f)
                                                                                                 << 3) * (0xFF - alpha)) / 0xFF) >> 3) +
                                                            ((((((uint16_t)(pixel[1]) * alpha) + (((writebuf[write_off + x_j] & 0x07e0) >> 5) << 2) *
                                                                (0xFF - alpha)) / 0xFF) >> 2) << 5) +
                                                            ((((((uint16_t)(pixel[2]) * (alpha)) + ((writebuf[write_off + x_j] >> 11) << 3) *
                                                                (0xFF - alpha)) / 0xFF) >> 3) << 11);
                            }
                            else
                            {
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    return;
}
void rle_filter_blit_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                              struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = image->img_w;
    int image_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = *((uint8_t *)image_off);
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;

    if (img_type == 132)//rle_filter_rgba888_rgb888
    {
        uint8_t source_bytes_per_pixel = 4;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
            int line = y_i - image_y;
            uncompressed_rle_argb8888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;
            uint8_t *pixel;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                pixel = (uint8_t *)(read_off + x_j * source_bytes_per_pixel);
                if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
                {
                    switch (opacity_value)
                    {
                    case 0:
                        break;
                    case 255:
                        {
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel] = pixel[0];//b
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
                        }
                        break;
                    default:
                        {
                            opacity_value = (pixel[3] * opacity_value) / 255;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (pixel[2] * opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((pixel[1]) * opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel] = ((pixel[0]) * opacity_value
                                                                                + writebuf[(write_off + x_j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                        }
                        break;
                    }
                }
            }
        }
    }
    else if (img_type == 68) //rle_filter_rgb888_2_rgb888
    {
        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;

            int line = y_i - image_y;
            uncompressed_rle_rgb888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                uint8_t *pixel = (uint8_t *)(read_off + x_j * source_bytes_per_pixel);
                if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
                {
                    switch (opacity_value)
                    {
                    case 0:
                        break;
                    case 255:
                        {
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel] = pixel[0];//b
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
                        }
                        break;
                    default:
                        {
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (pixel[2] * opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((pixel[1]) * opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel] = ((pixel[0]) * opacity_value
                                                                                + writebuf[(write_off + x_j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                        }
                        break;
                    }
                }
            }

        }
    }
    else if (img_type == 4) //rle_filter_rgb565_2_rgb888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
            int line = y_i - image_y;
            uncompressed_rle_rgb565(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;

            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                uint16_t pixel = *((uint16_t *)(read_off + x_j * source_bytes_per_pixel));
                if (pixel != 0)
                {
                    switch (opacity_value)
                    {
                    case 0:
                        break;
                    case 255:
                        {
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;
                        }
                        break;
                    default:
                        {
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (((pixel >> 11) << 3) *
                                                                                    opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((((pixel & 0x07e0) >> 5) << 2) *
                                                                                    opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel] = (((pixel & 0x001f) << 3) *
                                                                                opacity_value
                                                                                + writebuf[(write_off + x_j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                        }
                        break;
                    }
                }
            }
        }

    }
    return;
}
void rle_filter_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = image->img_w;
    int image_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = *((uint8_t *)image_off);
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;

    if (img_type == 132)
    {
        uint8_t source_bytes_per_pixel = 4;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
            int line = y_i - image_y;
            uncompressed_rle_argb8888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;
            uint8_t *pixel;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                pixel = (uint8_t *)(read_off + x_j * source_bytes_per_pixel);
                if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
                {
                    switch (opacity_value)
                    {
                    case 0:
                        break;
                    case 255:
                        {
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel] = pixel[0];//b
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = pixel[3]; //a
                        }
                        break;
                    default:
                        {
                            opacity_value = (pixel[3] * opacity_value) / 255;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = (pixel[3] * opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (pixel[2] * opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((pixel[1]) * opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel] = ((pixel[0]) * opacity_value
                                                                                + writebuf[(write_off + x_j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                        }
                        break;
                    }
                }
            }
        }
    }
    else if (img_type == 68) //rle_filter_rgb888_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;

            int line = y_i - image_y;
            uncompressed_rle_rgb888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                uint8_t *pixel = (uint8_t *)(read_off + x_j * source_bytes_per_pixel);
                if (pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
                {
                    switch (opacity_value)
                    {
                    case 0:
                        break;
                    case 255:
                        {
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel] = pixel[0];//b
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = 0xFF; //a
                        }
                        break;
                    default:
                        {
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = (0xFF * opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (pixel[2] * opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((pixel[1]) * opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel] = ((pixel[0]) * opacity_value
                                                                                + writebuf[(write_off + x_j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                        }
                        break;
                    }
                }
            }

        }
    }
    else if (img_type == 4) //rgb565_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
            int line = y_i - image_y;
            uncompressed_rle_rgb565(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;

            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                uint16_t pixel = *((uint16_t *)(read_off + x_j * source_bytes_per_pixel));
                if (pixel != 0)
                {
                    switch (opacity_value)
                    {
                    case 0:
                        break;
                    case 255:
                        {
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = 0xFF; //a
                        }
                        break;
                    default:
                        {
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = (0xFF * opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (((pixel >> 11) << 3) *
                                                                                    opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((((pixel & 0x07e0) >> 5) << 2) *
                                                                                    opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel] = (((pixel & 0x001f) << 3) *
                                                                                opacity_value
                                                                                + writebuf[(write_off + x_j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                        }
                        break;
                    }
                }
            }
        }

    }
    return;
}