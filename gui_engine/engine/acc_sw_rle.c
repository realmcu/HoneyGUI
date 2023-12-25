/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_rle.c
  * @brief Picture software decoding library
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author wenjing_jiang@realsil.com.cn
  * @date 2023/12/08
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
#include "acc_sw_blend.h"
#include "acc_sw.h"
#include "acc_sw_rle.h"
#if defined ( __CC_ARM ) && !defined(RTL8763EP)
#define __FPU_PRESENT                  1            /* FPU present                                                             */
#include "arm_math.h"
#else
#include "math.h"
#endif

static void gui_memset16(uint16_t *addr, uint16_t pixel, uint32_t len) //rgb565
{
#if defined ( __CC_ARM ) && !defined(RTL8763EP)
    arm_fill_q15(pixel, (int16_t *)addr, len);
#endif
#if defined(_MSC_VER) || (defined(__GNUC__))
    for (uint32_t i = 0; i < len; i++)
    {
        addr[i] = pixel;
    }
#endif
}
static void gui_memset32(uint32_t *addr, uint32_t pixel, uint32_t len)  //argb8888
{
#if defined ( __CC_ARM ) && !defined(RTL8763EP)
    arm_fill_q31(pixel, (int32_t *)addr, len);
#endif
#if defined(_MSC_VER) || (defined(__GNUC__))
    for (uint32_t i = 0; i < len; i++)
    {
        addr[i] = pixel;
    }
#endif
}
static void uncompressed_rle_rgb565(rtzip_file_t *file, uint32_t line,  uint8_t *buf)
{
    //rtzip_file_header_t *header = (rtzip_file_header_t *)file;
    uint32_t start = (uint32_t)file + file->compressed_addr[line];
    uint32_t end = (uint32_t)file + file->compressed_addr[line + 1];
    uint16_t *linebuf = (uint16_t *)buf;

    for (uint32_t addr = start; addr < end;)
    {
        rtzip_rgb565_node_t *node = (rtzip_rgb565_node_t *)addr;
        gui_memset16(linebuf, node->pixel16, node->len);

        addr = addr + sizeof(rtzip_rgb565_node_t);
        linebuf = linebuf + node->len;
    }
}
void uncompressed_rle_rgb888(rtzip_file_t *file, uint32_t line,  uint8_t *buf)
{
    //rtzip_file_header_t *header = (rtzip_file_header_t *)file;
    uint32_t start = (uint32_t)file + file->compressed_addr[line];
    uint32_t end = (uint32_t)file + file->compressed_addr[line + 1];
    uint8_t *linebuf = (uint8_t *)buf;
    //gui_log("linebuf %o\n",linebuf);
    for (uint32_t addr = start; addr < end;)
    {
        rtzip_rgb888_node_t *node = (rtzip_rgb888_node_t *)addr;
        for (uint32_t i = 0; i < node->len; i++)
        {
            linebuf[i * 3]     = node->pixel_b;
            linebuf[i * 3 + 1] = node->pixel_g;
            linebuf[i * 3 + 2] = node->pixel_r;
        }
        addr = addr + sizeof(rtzip_rgb888_node_t);
        linebuf = linebuf + node->len * 3;
    }
}
void uncompressed_rle_argb8888(rtzip_file_t *file, uint32_t line,  uint8_t *buf)
{
    //rtzip_file_header_t *header = (rtzip_file_header_t *)file;
    uint32_t start = (uint32_t)file + file->compressed_addr[line];
    uint32_t end = (uint32_t)file + file->compressed_addr[line + 1];
    uint32_t *linebuf = (uint32_t *)buf;

    for (uint32_t addr = start; addr < end;)
    {
        rtzip_argb8888_node_t *node = (rtzip_argb8888_node_t *)addr;
        gui_memset32(linebuf, node->pixel32, node->len);

        addr = addr + sizeof(rtzip_argb8888_node_t);
        linebuf = linebuf + node->len;
    }
}

void rle_bypass_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
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
    rtzip_file_t *file = (rtzip_file_t *)image_off;
    uint8_t opacity_value = image->opacity_value;

    if (img_type == 4)//rle_bypass_565_2_565
    {
        // gui_log("rle 565_2_565\n");
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
    else if (img_type == 68) //rle_bypass_rgb888_2_rgb565
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
    else if (img_type == 132) //rle_bypass_argb8888_2_rgb565
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
    return;
}
void rle_bypass_blit_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
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

    if (img_type == 132)//rle_bypass_rgba8888_rgb888
    {
        uint8_t source_bytes_per_pixel = 4;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        rtzip_file_t *file = (rtzip_file_t *)image_off;
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
                uint8_t opacity_value = image->opacity_value;
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
    else if (img_type == 68) //rle_bypass_rgb888_2_rgb888
    {
        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
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
                uint8_t opacity_value = image->opacity_value;
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
    else if (img_type == 4) //rle_bypass_rgb565_2_rgb888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        rtzip_file_t *file = (rtzip_file_t *)image_off;
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
                uint8_t opacity_value = image->opacity_value;
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
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (((pixel >> 11) << 3) * opacity_value
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((((pixel & 0x07e0) >> 5) << 2) *
                                                                                opacity_value
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel] = (((pixel & 0x001f) << 3) * opacity_value
                                                                            + writebuf[(write_off + x_j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                    }
                    break;
                }

            }
        }

    }
    return;
}
void rle_bypass_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
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

    if (img_type == 132)
    {
        uint8_t source_bytes_per_pixel = 4;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        rtzip_file_t *file = (rtzip_file_t *)image_off;
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
                uint8_t opacity_value = image->opacity_value;
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel] = pixel[0];//b
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = pixel[1];//g
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = pixel[2]; //r
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = pixel[3]; //r
                    }
                    break;
                default:
                    {
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
    else if (img_type == 68) //rle_bypass_rgb888_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
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
                uint8_t opacity_value = image->opacity_value;
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
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
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
    else if (img_type == 4) //rle_bypass_rgb565_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        rtzip_file_t *file = (rtzip_file_t *)image_off;
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
                uint8_t opacity_value = image->opacity_value;
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
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] = (((pixel >> 11) << 3) * opacity_value
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] = ((((pixel & 0x07e0) >> 5) << 2) *
                                                                                opacity_value
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 1] * (255 - opacity_value))) / 255 ;
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel] = (((pixel & 0x001f) << 3) * opacity_value
                                                                            + writebuf[(write_off + x_j) * dc_bytes_per_pixel] * (255 - opacity_value)) / 255 ;
                    }
                    break;
                }

            }
        }

    }
    return;
}
void rle_bypass_matrix_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                     struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;

    int image_w = image->target_w + 1;
    int image_h = image->target_h + 1;
    int source_w = image->img_w;
    int source_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    struct rtgui_matrix *inverse = image->inverse;
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;
    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = *((uint8_t *)image_off);



    int32_t y1, y2, z1, z2;
//        x1 = round(inverse->m[0][0] * dc->section.x1 + inverse->m[0][1] * dc->section.y1 +
//                   inverse->m[0][2]);
    y1 = round(inverse->m[1][0] * dc->section.x1 + inverse->m[1][1] * dc->section.y1 +
               inverse->m[1][2]);
    z1 = round(inverse->m[2][0] * dc->section.x1 + inverse->m[2][1] * dc->section.y1 +
               inverse->m[2][2]);

//        x2 = round(inverse->m[0][0] * dc->section.x2 + inverse->m[0][1] * (dc->section.y2 - 1) +
//                   inverse->m[0][2]);
    y2 = round(inverse->m[1][0] * dc->section.x2 + inverse->m[1][1] * (dc->section.y2 - 1) +
               inverse->m[1][2]);
    z2 = round(inverse->m[2][0] * dc->section.x2 + inverse->m[2][1] * (dc->section.y2 - 1) +
               inverse->m[2][2]);

    y1 = y1 / z1;
    y2 = y2 / z2;

    uint32_t start_line = 0, end_line = image->img_h - 1;
    if (y1 < 0)
    {
        start_line = 0;
    }
    else if (y1 >= image->img_h)
    {
        return;
    }
    else
    {
        start_line = y1;
    }

    if (y2 < 0)
    {
        return;
    }
    else if (y2 + 4 >= image->img_h)
    {
        end_line = image->img_h - 1;
    }
    else
    {
        end_line = y2 + 4;
    }
    //TODO: y2 + ceil(1 / scale_y_ratio)
    if (img_type == 4)//rle_bypass_matrix_rgb565_2_rgb565
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb565(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);

                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint16_t pixel = *((uint16_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                                source_bytes_per_pixel));
                uint8_t opacity_value = image->opacity_value;
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        writebuf[write_off + x_j] = pixel;
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            writebuf[write_off + x_j] = ((((((pixel >> 11) << 3) * opacity_value + ((
                                                                                                        writebuf[write_off + x_j] >> 11) << 3) * (0xFF - opacity_value)) / 255) >> 3) << 11) +
                                                        ((((((((pixel & 0x07e0) >> 5) << 2) * opacity_value) + (((writebuf[write_off + x_j] & 0x07e0) >> 5)
                                                                << 2) * (0xFF - opacity_value)) / 0xFF) >> 2) << 5) +
                                                        ((((((pixel & 0x001f) << 3) * opacity_value) + ((writebuf[write_off + x_j]  & 0x001f) << 3) *
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
    else if (img_type == 68) //rle_bypass_matrix_rgb888_2_rgb565
    {
        uint8_t source_bytes_per_pixel = 3;

        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);

                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                             source_bytes_per_pixel);
                uint8_t opacity_value = image->opacity_value;
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
                                                                                                         writebuf[write_off + x_j]  & 0x001f) << 3) * (0xFF - opacity_value)) / 0xFF) >> 3) +
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
    else if (img_type == 132) //rle_bypass_matrix_argb8888_2_rgb565
    {
        uint8_t source_bytes_per_pixel = 4;

        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_argb8888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);

                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                             source_bytes_per_pixel);
                uint8_t opacity_value = image->opacity_value;
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
    return;
}

void rle_bypass_matrix_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                       struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;

    int image_w = image->target_w + 1;
    int image_h = image->target_h + 1;
    int source_w = image->img_w;
    int source_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    struct rtgui_matrix *inverse = image->inverse;

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = *((uint8_t *)image_off);
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;
    int32_t  y1, y2, z1, z2;
//        x1 = round(inverse->m[0][0] * dc->section.x1 + inverse->m[0][1] * dc->section.y1 +
//                   inverse->m[0][2]);
    y1 = round(inverse->m[1][0] * dc->section.x1 + inverse->m[1][1] * dc->section.y1 +
               inverse->m[1][2]);
    z1 = round(inverse->m[2][0] * dc->section.x1 + inverse->m[2][1] * dc->section.y1 +
               inverse->m[2][2]);

//        x2 = round(inverse->m[0][0] * dc->section.x2 + inverse->m[0][1] * (dc->section.y2 - 1) +
//                   inverse->m[0][2]);
    y2 = round(inverse->m[1][0] * dc->section.x2 + inverse->m[1][1] * (dc->section.y2 - 1) +
               inverse->m[1][2]);
    z2 = round(inverse->m[2][0] * dc->section.x2 + inverse->m[2][1] * (dc->section.y2 - 1) +
               inverse->m[2][2]);

    y1 = y1 / z1;
    y2 = y2 / z2;

    uint32_t start_line = 0, end_line = image->img_h - 1;
    if (y1 < 0)
    {
        start_line = 0;
    }
    else if (y1 >= image->img_h)
    {
        return;
    }
    else
    {
        start_line = y1;
    }

    if (y2 < 0)
    {
        return;
    }
    else if (y2 + 4 >= image->img_h)
    {
        end_line = image->img_h - 1;
    }
    else
    {
        end_line = y2 + 4;
    }
    if (img_type == 132)//rle_bypass_matric_rgba8888_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 4;

        //TODO: y2 + ceil(1 / scale_y_ratio)

        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_argb8888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);
// gui_log("matric_rgba8888_2_argb8888:z%d\n",x_matric);
                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                             source_bytes_per_pixel);

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
                        writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = (pixel[3] * opacity_value
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
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
    else if (img_type == 68) //rle_bypass_matrix_rgb888_2_argb8888
    {

        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);
// gui_log("matric_rgb565_2_argb8888:z%d\n",x);
                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                             source_bytes_per_pixel);
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
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
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
    else if (img_type == 4) //rle_bypass_matrix_rgb565_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;

            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb565(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);

                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint16_t pixel = *(uint16_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                               source_bytes_per_pixel);
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
                                                                                + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
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
    return;
}
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
    rtzip_file_t *file = (rtzip_file_t *)image_off;
    uint8_t opacity_value = image->opacity_value;

    if (img_type == 4)//rle_filter_565_2_565
    {
        // gui_log("rle 565_2_565\n");
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
        rtzip_file_t *file = (rtzip_file_t *)image_off;
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
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
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
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
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
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
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
        rtzip_file_t *file = (rtzip_file_t *)image_off;
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
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
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
void rle_filter_matrix_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                     struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;

    int image_w = image->target_w + 1;
    int image_h = image->target_h + 1;
    int source_w = image->img_w;
    int source_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    struct rtgui_matrix *inverse = image->inverse;
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;
    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = *((uint8_t *)image_off);



    int32_t y1, y2, z1, z2;
//        x1 = round(inverse->m[0][0] * dc->section.x1 + inverse->m[0][1] * dc->section.y1 +
//                   inverse->m[0][2]);
    y1 = round(inverse->m[1][0] * dc->section.x1 + inverse->m[1][1] * dc->section.y1 +
               inverse->m[1][2]);
    z1 = round(inverse->m[2][0] * dc->section.x1 + inverse->m[2][1] * dc->section.y1 +
               inverse->m[2][2]);

//        x2 = round(inverse->m[0][0] * dc->section.x2 + inverse->m[0][1] * (dc->section.y2 - 1) +
//                   inverse->m[0][2]);
    y2 = round(inverse->m[1][0] * dc->section.x2 + inverse->m[1][1] * (dc->section.y2 - 1) +
               inverse->m[1][2]);
    z2 = round(inverse->m[2][0] * dc->section.x2 + inverse->m[2][1] * (dc->section.y2 - 1) +
               inverse->m[2][2]);

    y1 = y1 / z1;
    y2 = y2 / z2;

    uint32_t start_line = 0, end_line = image->img_h - 1;
    if (y1 < 0)
    {
        start_line = 0;
    }
    else if (y1 >= image->img_h)
    {
        return;
    }
    else
    {
        start_line = y1;
    }

    if (y2 < 0)
    {
        return;
    }
    else if (y2 + 4 >= image->img_h)
    {
        end_line = image->img_h - 1;
    }
    else
    {
        end_line = y2 + 4;
    }
    //TODO: y2 + ceil(1 / scale_y_ratio)
    if (img_type == 4)//rle_filter_matrix_rgb565_2_rgb565
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb565(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);

                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint16_t pixel = *((uint16_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                                source_bytes_per_pixel));
                uint8_t opacity_value = image->opacity_value;
                if (pixel != 0)
                {
                    switch (opacity_value)
                    {
                    case 0:
                        break;
                    case 255:
                        {
                            writebuf[write_off + x_j] = pixel;
                        }
                        break;
                    default:
                        {
                            if (opacity_value < 255)
                            {
                                writebuf[write_off + x_j] = ((((((pixel >> 11) << 3) * opacity_value + ((
                                                                                                            writebuf[write_off + x_j] >> 11) << 3) * (0xFF - opacity_value)) / 255) >> 3) << 11) +
                                                            ((((((((pixel & 0x07e0) >> 5) << 2) * opacity_value) + (((writebuf[write_off + x_j] & 0x07e0) >> 5)
                                                                    << 2) * (0xFF - opacity_value)) / 0xFF) >> 2) << 5) +
                                                            ((((((pixel & 0x001f) << 3) * opacity_value) + ((writebuf[write_off + x_j]  & 0x001f) << 3) *
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
    else if (img_type == 68) //rle_filter_matrix_rgb888_2_rgb565
    {
        uint8_t source_bytes_per_pixel = 3;

        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);

                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                             source_bytes_per_pixel);
                uint8_t opacity_value = image->opacity_value;
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
                                                                                                             writebuf[write_off + x_j]  & 0x001f) << 3) * (0xFF - opacity_value)) / 0xFF) >> 3) +
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
    else if (img_type == 132) //rle_filter_matrix_argb8888_2_rgb565
    {
        uint8_t source_bytes_per_pixel = 4;

        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_argb8888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);

                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                             source_bytes_per_pixel);
                uint8_t opacity_value = image->opacity_value;
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

void rle_filter_matrix_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                       struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;

    int image_w = image->target_w + 1;
    int image_h = image->target_h + 1;
    int source_w = image->img_w;
    int source_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    struct rtgui_matrix *inverse = image->inverse;

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = *((uint8_t *)image_off);
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;
    int32_t  y1, y2, z1, z2;
//        x1 = round(inverse->m[0][0] * dc->section.x1 + inverse->m[0][1] * dc->section.y1 +
//                   inverse->m[0][2]);
    y1 = round(inverse->m[1][0] * dc->section.x1 + inverse->m[1][1] * dc->section.y1 +
               inverse->m[1][2]);
    z1 = round(inverse->m[2][0] * dc->section.x1 + inverse->m[2][1] * dc->section.y1 +
               inverse->m[2][2]);

//        x2 = round(inverse->m[0][0] * dc->section.x2 + inverse->m[0][1] * (dc->section.y2 - 1) +
//                   inverse->m[0][2]);
    y2 = round(inverse->m[1][0] * dc->section.x2 + inverse->m[1][1] * (dc->section.y2 - 1) +
               inverse->m[1][2]);
    z2 = round(inverse->m[2][0] * dc->section.x2 + inverse->m[2][1] * (dc->section.y2 - 1) +
               inverse->m[2][2]);

    y1 = y1 / z1;
    y2 = y2 / z2;

    uint32_t start_line = 0, end_line = image->img_h - 1;
    if (y1 < 0)
    {
        start_line = 0;
    }
    else if (y1 >= image->img_h)
    {
        return;
    }
    else
    {
        start_line = y1;
    }

    if (y2 < 0)
    {
        return;
    }
    else if (y2 + 4 >= image->img_h)
    {
        end_line = image->img_h - 1;
    }
    else
    {
        end_line = y2 + 4;
    }
    if (img_type == 132)//matric_rgba8888_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 4;

        //TODO: y2 + ceil(1 / scale_y_ratio)

        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_argb8888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);
// gui_log("matric_rgba8888_2_argb8888:z%d\n",x_matric);
                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                             source_bytes_per_pixel);

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
                            writebuf[(write_off + x_j) * dc_bytes_per_pixel + 3] = (pixel[3] * opacity_value
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
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
    else if (img_type == 68) //matrix_rgb888_2_argb8888
    {

        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);
// gui_log("matric_rgb565_2_argb8888:z%d\n",x);
                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                             source_bytes_per_pixel);
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
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
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
    else if (img_type == 4) //matrix_rgb565_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;

            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb565(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);

                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint16_t pixel = *(uint16_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                               source_bytes_per_pixel);
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
                                                                                    + (writebuf[(write_off + x_j) * dc_bytes_per_pixel + 2] * (255 - opacity_value))) / 255 ;
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
void rle_alpha_matrix_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                      struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;

    int image_w = image->target_w + 1;
    int image_h = image->target_h + 1;
    int source_w = image->img_w;
    int source_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    struct rtgui_matrix *inverse = image->inverse;

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = *((uint8_t *)image_off);
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    if (img_type == 132)//alpha_matric_rgb8888_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 4;
        int32_t  y1, y2, z1, z2;
//        x1 = round(inverse->m[0][0] * dc->section.x1 + inverse->m[0][1] * dc->section.y1 +
//                   inverse->m[0][2]);
        y1 = round(inverse->m[1][0] * dc->section.x1 + inverse->m[1][1] * dc->section.y1 +
                   inverse->m[1][2]);
        z1 = round(inverse->m[2][0] * dc->section.x1 + inverse->m[2][1] * dc->section.y1 +
                   inverse->m[2][2]);

//        x2 = round(inverse->m[0][0] * dc->section.x2 + inverse->m[0][1] * (dc->section.y2 - 1) +
//                   inverse->m[0][2]);
        y2 = round(inverse->m[1][0] * dc->section.x2 + inverse->m[1][1] * (dc->section.y2 - 1) +
                   inverse->m[1][2]);
        z2 = round(inverse->m[2][0] * dc->section.x2 + inverse->m[2][1] * (dc->section.y2 - 1) +
                   inverse->m[2][2]);

        y1 = y1 / z1;
        y2 = y2 / z2;

        uint32_t start_line = 0, end_line = image->img_h - 1;
        if (y1 < 0)
        {
            start_line = 0;
        }
        else if (y1 >= image->img_h)
        {
            return;
        }
        else
        {
            start_line = y1;
        }

        if (y2 < 0)
        {
            return;
        }
        else if (y2 + 4 >= image->img_h)
        {
            end_line = image->img_h - 1;
        }
        else
        {
            end_line = y2 + 4;
        }
        //TODO: y2 + ceil(1 / scale_y_ratio)
        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;

            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_argb8888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);

                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                             source_bytes_per_pixel);

                gui_color_t color = {.channel.blue = pixel[0],
                                     .channel.green = pixel[1],
                                     .channel.red = pixel[2],
                                     .channel.alpha = pixel[3],
                                    };
                uint8_t opacity_value = (uint8_t)image->opacity_value;
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + x_j) * dc_bytes_per_pixel);
                        do_blending_argb8888_2_argb8888(d, &color);
                    }
                    break;
                default:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + x_j) * dc_bytes_per_pixel);
                        do_blending_argb8888_2_argb8888_opacity(d, &color, opacity_value);
                    }
                    break;
                }

            }
        }
    }
}

void rle_bypass_matrix_blit_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                     struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;

    int image_w = image->target_w + 1;
    int image_h = image->target_h + 1;
    int source_w = image->img_w;
    int source_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    struct rtgui_matrix *inverse = image->inverse;

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = *((uint8_t *)image_off);
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;
    int32_t  y1, y2, z1, z2;
//        x1 = round(inverse->m[0][0] * dc->section.x1 + inverse->m[0][1] * dc->section.y1 +
//                   inverse->m[0][2]);
    y1 = round(inverse->m[1][0] * dc->section.x1 + inverse->m[1][1] * dc->section.y1 +
               inverse->m[1][2]);
    z1 = round(inverse->m[2][0] * dc->section.x1 + inverse->m[2][1] * dc->section.y1 +
               inverse->m[2][2]);

//        x2 = round(inverse->m[0][0] * dc->section.x2 + inverse->m[0][1] * (dc->section.y2 - 1) +
//                   inverse->m[0][2]);
    y2 = round(inverse->m[1][0] * dc->section.x2 + inverse->m[1][1] * (dc->section.y2 - 1) +
               inverse->m[1][2]);
    z2 = round(inverse->m[2][0] * dc->section.x2 + inverse->m[2][1] * (dc->section.y2 - 1) +
               inverse->m[2][2]);

    y1 = y1 / z1;
    y2 = y2 / z2;

    uint32_t start_line = 0, end_line = image->img_h - 1;
    if (y1 < 0)
    {
        start_line = 0;
    }
    else if (y1 >= image->img_h)
    {
        return;
    }
    else
    {
        start_line = y1;
    }

    if (y2 < 0)
    {
        return;
    }
    else if (y2 + 4 >= image->img_h)
    {
        end_line = image->img_h - 1;
    }
    else
    {
        end_line = y2 + 4;
    }
    if (img_type == 132)//rle_bypass_matric_rgba8888_2_rgb888
    {
        uint8_t source_bytes_per_pixel = 4;

        //TODO: y2 + ceil(1 / scale_y_ratio)

        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_argb8888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);
// gui_log("matric_rgba8888_2_argb8888:z%d\n",x_matric);
                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                             source_bytes_per_pixel);

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
    else if (img_type == 68) //rle_bypass_matrix_rgb888_2_rgb888
    {

        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);

                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                             source_bytes_per_pixel);
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
    else if (img_type == 4) //rle_bypass_matrix_rgb565_2_rgb888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;

            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb565(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);

                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint16_t pixel = *(uint16_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                               source_bytes_per_pixel);
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
        rtzip_file_t *file = (rtzip_file_t *)image_off;
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
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
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
        rtzip_file_t *file = (rtzip_file_t *)image_off;
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
void rle_filter_matrix_blit_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                     struct rtgui_rect *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;

    int image_w = image->target_w + 1;
    int image_h = image->target_h + 1;
    int source_w = image->img_w;
    int source_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    struct rtgui_matrix *inverse = image->inverse;

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t img_type = *((uint8_t *)image_off);
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;
    int32_t  y1, y2, z1, z2;
//        x1 = round(inverse->m[0][0] * dc->section.x1 + inverse->m[0][1] * dc->section.y1 +
//                   inverse->m[0][2]);
    y1 = round(inverse->m[1][0] * dc->section.x1 + inverse->m[1][1] * dc->section.y1 +
               inverse->m[1][2]);
    z1 = round(inverse->m[2][0] * dc->section.x1 + inverse->m[2][1] * dc->section.y1 +
               inverse->m[2][2]);

//        x2 = round(inverse->m[0][0] * dc->section.x2 + inverse->m[0][1] * (dc->section.y2 - 1) +
//                   inverse->m[0][2]);
    y2 = round(inverse->m[1][0] * dc->section.x2 + inverse->m[1][1] * (dc->section.y2 - 1) +
               inverse->m[1][2]);
    z2 = round(inverse->m[2][0] * dc->section.x2 + inverse->m[2][1] * (dc->section.y2 - 1) +
               inverse->m[2][2]);

    y1 = y1 / z1;
    y2 = y2 / z2;

    uint32_t start_line = 0, end_line = image->img_h - 1;
    if (y1 < 0)
    {
        start_line = 0;
    }
    else if (y1 >= image->img_h)
    {
        return;
    }
    else
    {
        start_line = y1;
    }

    if (y2 < 0)
    {
        return;
    }
    else if (y2 + 4 >= image->img_h)
    {
        end_line = image->img_h - 1;
    }
    else
    {
        end_line = y2 + 4;
    }
    if (img_type == 132)//rle_filter_matric_rgba8888_2_rgb888
    {
        uint8_t source_bytes_per_pixel = 4;

        //TODO: y2 + ceil(1 / scale_y_ratio)

        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_argb8888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);
// gui_log("matric_rgba8888_2_argb8888:z%d\n",x_matric);
                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                             source_bytes_per_pixel);

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
    else if (img_type == 68) //rle_filter_matrix_rgb888_2_rgb888
    {

        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
//            int line = y_i - image_y;
            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb888(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);
// gui_log("matric_rgb565_2_argb8888:z%d\n",x);
                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint8_t *pixel = (uint8_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                             source_bytes_per_pixel);
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
    else if (img_type == 4) //rle_filter_matrix_rgb565_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * source_w * (end_line - start_line + 1)];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;

            for (int k = start_line; k <= end_line; k++)
            {
                uncompressed_rle_rgb565(file, k, line_buf + (k - start_line) * source_w * source_bytes_per_pixel);
            }
            uint32_t read_off = (uint32_t)line_buf ;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t x_j = x_start; x_j < x_end; x_j++)
            {
                float X = image->inverse->m[0][0] * x_j + image->inverse->m[0][1] * y_i + image->inverse->m[0][2];
                float Y = image->inverse->m[1][0] * x_j + image->inverse->m[1][1] * y_i + image->inverse->m[1][2];
                float Z = image->inverse->m[2][0] * x_j + image->inverse->m[2][1] * y_i + image->inverse->m[2][2];
                int x_matric = round(X / Z);
                int y_matric = round(Y / Z);

                if ((x_matric >= source_w - 1) || (x_matric < 0) || (y_matric < 0) || (y_matric >= source_h - 1))
                {
                    continue;
                }
                uint16_t pixel = *(uint16_t *)(read_off + ((y_matric - start_line) * source_w + x_matric) *
                                               source_bytes_per_pixel);
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
void rle_alpha_blend_blit_argb8888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
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

    if (img_type == 132)
    {
        uint8_t source_bytes_per_pixel = 4;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;
            int line = i - image_y;
            uncompressed_rle_argb8888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;
            uint8_t *pixel;
            for (uint32_t j = x_start; j < x_end; j++)
            {
                pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                gui_color_t color = {.channel.blue = pixel[0],
                                     .channel.green = pixel[1],
                                     .channel.red = pixel[2],
                                     .channel.alpha = pixel[3],
                                    };
                uint8_t opacity_value = (uint8_t)image->opacity_value;
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_argb8888_2_argb8888(d, &color);
                    }
                    break;
                default:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_argb8888_2_argb8888_opacity(d, &color, opacity_value);
                    }
                    break;
                }
            }
        }
    }
    else if (img_type == 68) //rle_rgb888_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int line = i - image_y;
            uncompressed_rle_rgb888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint8_t *pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                gui_color_t color = {.channel.blue = pixel[0],
                                     .channel.green = pixel[1],
                                     .channel.red = pixel[2],
                                     .channel.alpha = 255,
                                    };
                uint8_t opacity_value = (uint8_t)image->opacity_value;
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_rgb888_2_argb8888(d, &color);
                    }
                    break;
                default:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_rgb888_2_argb8888_opacity(d, &color, opacity_value);
                    }
                    break;
                }
            }

        }
    }
    else if (img_type == 4) //rgb565_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;
            int line = i - image_y;
            uncompressed_rle_rgb565(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint16_t pixel = *((uint16_t *)(read_off + j * source_bytes_per_pixel));
                gui_color_t color = {.channel.blue = (pixel & 0x001f) << 3,
                                     .channel.green = ((pixel & 0x07e0) >> 5) << 2,
                                     .channel.red = (pixel >> 11) << 3,
                                     .channel.alpha = 255,
                                    };
                uint8_t opacity_value = (uint8_t)image->opacity_value;
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_rgb565_2_argb8888(d, &color);
                    }
                    break;
                default:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_rgb565_2_argb8888_opacity(d, &color, opacity_value);
                    }
                    break;
                }
            }
        }

    }
    return;
}
void rle_alpha_blend_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
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

    if (img_type == 132)//rle_alpha_rgba8888_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 4;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;
            int line = i - image_y;
            uncompressed_rle_argb8888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;
            uint8_t *pixel;
            for (uint32_t j = x_start; j < x_end; j++)
            {
                pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                gui_color_t color = {.channel.blue = pixel[0],
                                     .channel.green = pixel[1],
                                     .channel.red = pixel[2],
                                     .channel.alpha = pixel[3],
                                    };
                uint8_t opacity_value = (uint8_t)image->opacity_value;
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_argb8888_2_argb8888(d, &color);
                    }
                    break;
                default:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_argb8888_2_argb8888_opacity(d, &color, opacity_value);
                    }
                    break;
                }
            }
        }
    }
    else if (img_type == 68) //rle_alpha_rgb888_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        memset(line_buf, 0, source_bytes_per_pixel * image_w);
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int line = i - image_y;
            uncompressed_rle_rgb888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint8_t *pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                //gui_log("piexl_%d %d %d\n",pixel[0],pixel[1],pixel[2]);
                writebuf[(write_off + j) * dc_bytes_per_pixel]     = pixel[0]; //B
                writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = pixel[1]; //G
                writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = pixel[2]; //R
            }

        }
    }
    else if (img_type == 4) //rle_alpha_rgb565_2_argb8888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * image_w];
        rtzip_file_t *file = (rtzip_file_t *)image_off;
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;
            int line = i - image_y;
            uncompressed_rle_rgb565(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint16_t pixel = *((uint16_t *)(read_off + j * source_bytes_per_pixel));
                writebuf[(write_off + j) * dc_bytes_per_pixel + 2] = (pixel >> 11) << 3;
                writebuf[(write_off + j) * dc_bytes_per_pixel + 1] = ((pixel & 0x07e0) >> 5) << 2;
                writebuf[(write_off + j) * dc_bytes_per_pixel] = (pixel & 0x001f) << 3;
            }
        }

    }
    return;
}
void rle(draw_img_t *image, struct gui_dispdev *dc,
         struct rtgui_rect *rect)
{
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    rtgui_matrix_t *matrix = image->matrix;
    bool identity = true;
    if (
        (matrix->m[0][0] == 1) && \
        (matrix->m[1][1] == 1) && \
        (matrix->m[2][2] == 1) && \
        (matrix->m[0][1] == 0) && \
        (matrix->m[1][0] == 0) && \
        (matrix->m[2][0] == 0) && \
        (matrix->m[2][1] == 0)
    )
    {
        identity = true;
    }
    else
    {
        identity = false;
    }
    if (image->blend_mode == IMG_BYPASS_MODE && (identity == true))//no matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            rle_bypass_blit_2_rgb565(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 3)
        {
            rle_bypass_blit_2_rgb888(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 4)
        {
            rle_bypass_blit_2_argb8888(image, dc, rect);
        }

    }
    if (image->blend_mode == IMG_BYPASS_MODE && (identity == false))//matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            rle_bypass_matrix_blit_2_rgb565(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 3)
        {
            rle_bypass_matrix_blit_2_rgb888(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 4)
        {
            rle_bypass_matrix_blit_2_argb8888(image, dc, rect);
        }

    }
    else if (image->blend_mode == IMG_FILTER_BLACK && (identity == true))//no matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            rle_filter_blit_2_rgb565(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 3)
        {
            rle_filter_blit_2_rgb888(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 4)
        {
            rle_filter_blit_2_argb8888(image, dc, rect);
        }
    }
    else if (image->blend_mode == IMG_FILTER_BLACK && (identity == false))//matrix
    {
        // gui_log("test\n");
        if (dc_bytes_per_pixel == 2)
        {
            rle_filter_matrix_blit_2_rgb565(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 3)
        {
            rle_filter_matrix_blit_2_rgb888(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 4)
        {
            rle_filter_matrix_blit_2_argb8888(image, dc, rect);
        }
    }
    else if (image->blend_mode == IMG_SRC_OVER_MODE && (identity == false))//matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            //rle_alpha_matrix_blit_2_rgb565(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 3)
        {
            //rle_alpha_matrix_blit_2_rgb888(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 4)
        {
            rle_alpha_matrix_blit_2_argb8888(image, dc, rect);
        }

    }
    else if (image->blend_mode == IMG_SRC_OVER_MODE && (identity == true))//no matrix
    {
        if (dc_bytes_per_pixel == 2)
        {
            //rle_alpha_blend_blit_2_rgb565(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 3)
        {
            // rle_alpha_blend_blit_2_rgb888(image, dc, rect);
        }
        else if (dc_bytes_per_pixel == 4)
        {
            rle_alpha_blend_blit_2_argb8888(image, dc, rect);
        }

    }


}

