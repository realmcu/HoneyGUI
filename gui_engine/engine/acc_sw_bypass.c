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
// #include "trace.h"

void bypass_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                          gui_rect_t *rect)
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
    uint8_t opacity_value = image->opacity_value;

    if (opacity_value == 0)
    {
        return;
    }
    if (img_type == RGB565)
    {
        int read_x_off = -_UI_MIN(image_x, 0) * BYTE_PIXEL_RGB565  + image_off;
        uint16_t write_off = (y_start - dc->section.y1) * dc->fb_width ;
        uint16_t *writebuf = (uint16_t *)dc->frame_buf + write_off;
        int read_buf = read_x_off + BYTE_PIXEL_RGB565 * ((y_start - image_y) * source_w) -
                       BYTE_PIXEL_RGB565 * x_start;
        uint16_t img_line_byte = source_w * BYTE_PIXEL_RGB565;

        // full screen background image, memcpy once
        if ((source_w == dc->fb_width) && (dc->fb_width == (x_end - x_start)) && (opacity_value == 255))
        {
            memcpy((writebuf + x_start), ((uint16_t *)read_buf + x_start),
                   BYTE_PIXEL_RGB565 * (x_end - x_start) * (y_end - y_start));
        }
        else
        {
            if (opacity_value == 255)
            {
                for (uint32_t i = y_start; i < y_end; i++)
                {
                    // memcpy line
                    memcpy((writebuf + x_start), ((uint16_t *)read_buf + x_start),
                           BYTE_PIXEL_RGB565 * (x_end - x_start));
                    // next line
                    writebuf += dc->fb_width;
                    read_buf += img_line_byte;
                }
            }
            else
            {
                for (uint32_t i = y_start; i < y_end; i++)
                {
                    for (uint32_t j = x_start; j < x_end; j++)
                    {
                        uint16_t pixel = (*((uint16_t *)read_buf + j));
                        writebuf[j] = ((((((pixel >> 11) << 3) * opacity_value + ((writebuf[j] >> 11) << 3) *
                                          (0xFF - opacity_value)) / 255) >> 3) << 11) +
                                      ((((((((pixel & 0x07e0) >> 5) << 2) * opacity_value) + (((writebuf[j] & 0x07e0) >> 5) <<
                                                                                              2) * (0xFF - opacity_value)) / 0xFF) >> 2) << 5) +
                                      ((((((pixel & 0x001f) << 3) * opacity_value) + ((writebuf[j]  & 0x001f) << 3) *
                                         (0xFF - opacity_value)) / 0xFF) >> 3);
                    }
                    // next line
                    writebuf += dc->fb_width;
                    read_buf += img_line_byte;
                }
            }
        }
    }
    else if (img_type == RGB888)
    {
        uint8_t source_bytes_per_pixel = 3;
        int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                           source_bytes_per_pixel * x_start;

            uint16_t *writebuf = (uint16_t *)dc->frame_buf;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint8_t *pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);

                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[write_off + j] = (uint16_t)(((pixel[2]) >> 3) << 11) + (uint16_t)(((pixel[1]) >> 2) << 5)
                                                  + (uint16_t)(((pixel[0]) >> 3)); //RGB  565
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            writebuf[write_off + j] = (((((uint16_t)(pixel[0]) * opacity_value) + ((
                                                                                                       writebuf[write_off + j]  & 0x001f) << 3) * (0xFF - opacity_value)) / 0xFF) >> 3) +
                                                      ((((((uint16_t)(pixel[1]) * opacity_value) + (((writebuf[write_off + j] & 0x07e0) >> 5) << 2) *
                                                          (0xFF - opacity_value)) / 0xFF) >> 2) << 5) +
                                                      ((((((uint16_t)(pixel[2]) * (opacity_value)) + ((writebuf[write_off + j] >> 11) << 3) *
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
    else if (img_type == RGBA8888)
    {
        uint8_t source_bytes_per_pixel = 4;
        int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                           source_bytes_per_pixel * x_start;

            uint16_t *writebuf = (uint16_t *)dc->frame_buf;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint8_t *pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);

                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[write_off + j] = (uint16_t)(((pixel[2]) >> 3) << 11) + (uint16_t)(((pixel[1]) >> 2) << 5)
                                                  + (uint16_t)(((pixel[0]) >> 3)); //RGB  565
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            uint8_t alpha = pixel[3] * opacity_value / 0xFF;
                            writebuf[write_off + j] = (((((uint16_t)(pixel[0]) * alpha) + ((writebuf[write_off + j]  & 0x001f)
                                                                                           << 3) * (0xFF - alpha)) / 0xFF) >> 3) +
                                                      ((((((uint16_t)(pixel[1]) * alpha) + (((writebuf[write_off + j] & 0x07e0) >> 5) << 2) *
                                                          (0xFF - alpha)) / 0xFF) >> 2) << 5) +
                                                      ((((((uint16_t)(pixel[2]) * (alpha)) + ((writebuf[write_off + j] >> 11) << 3) *
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
    return;
}
void bypass_blit_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                          gui_rect_t *rect)
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
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    struct gui_rgb_data_head *head = image->data;
    char img_type = head->type;
    uint8_t opacity_value = image->opacity_value;
    if (img_type == RGB565)
    {
        uint8_t source_bytes_per_pixel = 2;
        int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                           source_bytes_per_pixel * x_start;

            uint8_t *writebuf = dc->frame_buf;
            uint16_t pixel;
            for (uint32_t j = x_start; j < x_end; j++)
            {
                pixel = *((uint16_t *)read_off + j);
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;
                    }
                    break;
                default:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (((pixel >> 11) << 3) * opacity_value
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((((pixel & 0x07e0) >> 5) << 2) * opacity_value
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = (((pixel & 0x001f) << 3) * opacity_value
                                                                          + (writebuf[(write_off + j) * dc_bytes_per_pixel] * (255 - opacity_value))) / 255 ;
                    }
                    break;
                }
            }
        }
    }
    if (img_type == RGB888)
    {
        uint8_t source_bytes_per_pixel = 3;
        int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                           source_bytes_per_pixel * x_start;

            uint8_t *writebuf = dc->frame_buf;
            uint8_t *pixel;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = pixel[0];//b
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
                    }
                    break;
                default:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel[2] * opacity_value
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel[1]) * opacity_value
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = ((pixel[0]) * opacity_value
                                                                          + writebuf[(write_off + j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                    }
                    break;
                }

            }
        }
    }
    if (img_type == RGBA8888)
    {
        uint8_t source_bytes_per_pixel = 4;
        int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                           source_bytes_per_pixel * x_start;

            uint8_t *writebuf = dc->frame_buf;
            uint8_t *pixel;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = pixel[0];//b
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
                    }
                    break;
                default:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel[2] * opacity_value
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel[1]) * opacity_value
                                                                              + (writebuf[(write_off + j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = ((pixel[0]) * opacity_value
                                                                          + writebuf[(write_off + j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                    }
                    break;
                }

            }
        }
    }
    return;
}
void bypass_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                            gui_rect_t *rect)
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

    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    struct gui_rgb_data_head *head = image->data;
    char img_type = head->type;
    uint8_t *writebuf = dc->frame_buf;
    uint16_t pixel;
    uint8_t opacity_value = image->opacity_value;
    if (img_type == RGB565)
    {
        uint8_t source_bytes_per_pixel = 2;
        int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                           source_bytes_per_pixel * x_start;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                pixel = *((uint16_t *)read_off + j);
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = 0xFF;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = ((0xFF * opacity_value
                                                                                   + (writebuf[(write_off + j) * dc_bytes_per_pixel + 3] * (255 - opacity_value)))) / 255 ;
                            writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (((pixel >> 11) << 3) * opacity_value
                                                                                  + (writebuf[(write_off + j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((((pixel & 0x07e0) >> 5) << 2) * opacity_value
                                                                                  + (writebuf[(write_off + j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                            writebuf[(write_off + j) * dc_bytes_per_pixel] = (((pixel & 0x001f) << 3) * opacity_value
                                                                              + writebuf[(write_off + j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255;
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
    if (img_type == RGB888)
    {
        uint8_t source_bytes_per_pixel = 3;
        int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                           source_bytes_per_pixel * x_start;

            uint8_t *writebuf = dc->frame_buf;
            uint8_t *pixel;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + j) * dc_bytes_per_pixel] = pixel[0];//B
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1];//G
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //R
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = 0xFF; //A
                    }
                    break;
                default:
                    writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = ((0xFF * opacity_value
                                                                           + (writebuf[(write_off + j) * dc_bytes_per_pixel + 3] * (255 - opacity_value)))) / 255 ;
                    writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel[2] * opacity_value
                                                                          + (writebuf[(write_off + j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                    writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel[1]) * opacity_value
                                                                          + (writebuf[(write_off + j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                    writebuf[(write_off + j) * dc_bytes_per_pixel] = ((pixel[0]) * opacity_value
                                                                      + writebuf[(write_off + j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                    break;
                }
            }
        }
    }
    if (img_type == RGBA8888)
    {
        uint8_t source_bytes_per_pixel = 4;
        int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int read_off = ((i - image_y) * source_w) * source_bytes_per_pixel + read_x_off -
                           source_bytes_per_pixel * x_start;

            uint8_t *writebuf = dc->frame_buf;
            uint8_t *pixel;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {

                        writebuf[(write_off + j) * dc_bytes_per_pixel    ] = pixel[0]; //B
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1]; //G
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //R
                        writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = pixel[3]; //A
                    }
                    break;
                default:
                    writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel[2] * opacity_value
                                                                          + (writebuf[(write_off + j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                    writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel[1]) * opacity_value
                                                                          + (writebuf[(write_off + j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                    writebuf[(write_off + j) * dc_bytes_per_pixel] = ((pixel[0]) * opacity_value
                                                                      + writebuf[(write_off + j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                    writebuf[(write_off + j) * dc_bytes_per_pixel + 3] = (pixel[3] * opacity_value
                                                                          + (writebuf[(write_off + j) * dc_bytes_per_pixel + 3] * (255 - opacity_value))) / 255 ;
                    break;

                }
            }
        }
    }
    return;
}
