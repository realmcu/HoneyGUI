/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_rle_source_over.c
  * @brief Picture software decoding library:source_over mode
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author wenjing_jiang@realsil.com.cn
  * @date 2023/12/28
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
#include "acc_sw_blend.h"

void rle_alpha_blend_blit_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
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
    uint8_t img_type = *((uint8_t *)image_off);
    uint8_t img_color_type = *((uint8_t *)image_off + 1);
    imdc_file_t *file = (imdc_file_t *)image_off;
    uint8_t opacity_value = image->opacity_value;
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;
    if (img_type == IMDC_SRC_RGB565)//rle_alpha_blend_565_2_565
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * source_w];

        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int line = i - image_y;

            uncompressed_rle_rgb565(file, line, line_buf);

            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint16_t pixel = *((uint16_t *)read_off + j);
                uint16_t *d = writebuf + (write_off + j);
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        *d = pixel;
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            *d = do_blending_acc_2_rgb565_opacity((uint32_t)pixel, (uint32_t) * d, opacity_value);
                        }
                    }
                    break;
                }
            }
        }
    }
    else if (img_type == IMDC_SRC_RGB888 && img_color_type == ARGB8565)
    {
        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * source_w];
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;
            int line = i - image_y;
            uncompressed_rle_argb8565(file, line, line_buf);
//            uint32_t read_off = (uint32_t)line_buf ;
            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint8_t *pixel = (uint8_t *)(line_buf + j * source_bytes_per_pixel);
                uint8_t alpha = pixel[2];
                uint16_t color_t = (uint16_t)((((uint16_t)(pixel[1] & 0xff)) << 8) + (pixel[0] & 0xff));
                uint16_t *d = writebuf + (write_off + j);
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        *d = do_blending_acc_2_rgb565_opacity((uint32_t)color_t, (uint32_t) * d, alpha);
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            // opacity_value = alpha * opacity_value / 255;
                            *d = do_blending_acc_2_rgb565_opacity((uint32_t)color_t, (uint32_t) * d, alpha);
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
    else if (img_type == IMDC_SRC_RGB888 && img_color_type == RGB888) //rle_alpha_blend_rgb888_2_rgb565
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

            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint8_t *pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                gui_color_t color = {.color.rgba.r = pixel[2],
                                     .color.rgba.g = pixel[1],
                                     .color.rgba.b = pixel[0],
                                     .color.rgba.a = 255,
                                    };
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        uint16_t *d = writebuf + (write_off + j);
                        do_blending_2_rgb565(d, &color);
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            uint16_t *d = writebuf + (write_off + j);
                            do_blending_2_rgb565_opacity(d, &color, opacity_value);
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
    else if (img_type == IMDC_SRC_ARGB8888) //rle_alpha_blend_argb8888_2_rgb565
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

            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint8_t *pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                gui_color_t color = {.color.rgba.r = pixel[2],
                                     .color.rgba.g = pixel[1],
                                     .color.rgba.b = pixel[0],
                                     .color.rgba.a = pixel[3],
                                    };
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        uint16_t *d = writebuf + (write_off + j);
                        do_blending_2_rgb565(d, &color);
                    }
                    break;
                default:
                    {
                        if (opacity_value < 255)
                        {
                            uint16_t *d = writebuf + (write_off + j);
                            do_blending_2_rgb565_opacity(d, &color, opacity_value);
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
void rle_alpha_blend_blit_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
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
    uint16_t img_type = *((uint16_t *)image_off + 1);
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;

    if (img_type == IMDC_SRC_ARGB8888)//rle_alpha_blend_rgba8888_rgb888
    {
        uint8_t source_bytes_per_pixel = 4;
        uint8_t line_buf[source_bytes_per_pixel * source_w];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
            int line = y_i - image_y;
            uncompressed_rle_argb8888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;
            uint8_t *pixel;
            for (uint32_t j = x_start; j < x_end; j++)
            {
                pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                gui_color_t color = {.color.rgba.r = pixel[0],
                                     .color.rgba.g = pixel[1],
                                     .color.rgba.b = pixel[2],
                                     .color.rgba.a = pixel[3],
                                    };
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_2_rgb888(d, &color);
                    }
                    break;
                default:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_2_rgb888_opacity(d, &color, opacity_value);
                    }
                    break;
                }
            }
        }
    }
    else if (img_type == IMDC_SRC_RGB888) //rle_alpha_blend_rgb888_2_rgb888
    {
        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * source_w];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;

            int line = y_i - image_y;
            uncompressed_rle_rgb888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;
            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint8_t *pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                gui_color_t color = {.color.rgba.r = pixel[0],
                                     .color.rgba.g = pixel[1],
                                     .color.rgba.b = pixel[2],
                                     .color.rgba.a = 255,
                                    };
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_2_rgb888(d, &color);
                    }
                    break;
                default:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_2_rgb888_opacity(d, &color, opacity_value);
                    }
                    break;
                }
            }

        }
    }
    else if (img_type == IMDC_SRC_ARGB8888) //rle_alpha_blend_rgb565_2_rgb888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * source_w];
        imdc_file_t *file = (imdc_file_t *)image_off;
        for (uint32_t y_i = y_start; y_i < y_end; y_i++)
        {
            int write_off = (y_i - dc->section.y1) * dc->fb_width ;
            int line = y_i - image_y;
            uncompressed_rle_rgb565(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            uint8_t *writebuf = dc->frame_buf;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint16_t pixel = *((uint16_t *)(read_off + j * source_bytes_per_pixel));
                gui_color_t color = {.color.rgba.a = 255,
                                     .color.rgba.r = (pixel & 0x001f) << 3,
                                     .color.rgba.g = ((pixel & 0x07e0) >> 5) << 2,
                                     .color.rgba.b = (pixel >> 11) << 3,
                                    };
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_2_rgb888(d, &color);
                    }
                    break;
                default:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_2_rgb888_opacity(d, &color, opacity_value);
                    }
                    break;
                }
            }
        }

    }
    return;
}
void rle_alpha_blend_blit_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
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
    uint16_t img_type = *((uint16_t *)image_off + 1);
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    imdc_file_t *file = (imdc_file_t *)image_off;
    uint8_t *writebuf = dc->frame_buf;
    if (img_type == IMDC_SRC_ARGB8888)//rle_alpha_rgba8888_rgba8888
    {
        uint8_t source_bytes_per_pixel = 4;
        uint8_t line_buf[source_bytes_per_pixel * source_w];
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;
            int line = i - image_y;
            uncompressed_rle_argb8888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;

            uint8_t *pixel;
            for (uint32_t j = x_start; j < x_end; j++)
            {
                pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                gui_color_t color = {.color.rgba.r = pixel[0],
                                     .color.rgba.g = pixel[1],
                                     .color.rgba.b = pixel[2],
                                     .color.rgba.a = pixel[3],
                                    };
                uint8_t opacity_value = (uint8_t)image->opacity_value;
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_2_argb8888(d, &color);
                    }
                    break;
                default:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_2_argb8888_opacity(d, &color, opacity_value);
                    }
                    break;
                }
            }
        }
    }
    else if (img_type == IMDC_SRC_RGB888) //rle_alpha_rgb888_rgba8888
    {
        uint8_t source_bytes_per_pixel = 3;
        uint8_t line_buf[source_bytes_per_pixel * source_w];
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;

            int line = i - image_y;
            uncompressed_rle_rgb888(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;
            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint8_t *pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
                gui_color_t color = {.color.rgba.r = pixel[0],
                                     .color.rgba.g = pixel[1],
                                     .color.rgba.b = pixel[2],
                                     .color.rgba.a = 255,
                                    };
                uint8_t opacity_value = (uint8_t)image->opacity_value;
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_2_argb8888(d, &color);
                    }
                    break;
                default:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_2_argb8888_opacity(d, &color, opacity_value);
                    }
                    break;
                }
            }

        }
    }
    else if (img_type == IMDC_SRC_RGB565) //rle_alpha_rgb565_rgba8888
    {
        uint8_t source_bytes_per_pixel = 2;
        uint8_t line_buf[source_bytes_per_pixel * source_w];
        for (uint32_t i = y_start; i < y_end; i++)
        {
            int write_off = (i - dc->section.y1) * dc->fb_width ;
            int line = i - image_y;
            uncompressed_rle_rgb565(file, line, line_buf);
            int read_off = (int)line_buf - source_bytes_per_pixel * image_x;

            for (uint32_t j = x_start; j < x_end; j++)
            {
                uint16_t pixel = *((uint16_t *)(read_off + j * source_bytes_per_pixel));
                gui_color_t color = {.color.rgba.r = (pixel & 0x001f) << 3,
                                     .color.rgba.g = ((pixel & 0x07e0) >> 5) << 2,
                                     .color.rgba.b = (pixel >> 11) << 3,
                                     .color.rgba.a = 255,
                                    };
                uint8_t opacity_value = (uint8_t)image->opacity_value;
                switch (opacity_value)
                {
                case 0:
                    break;
                case 255:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_2_argb8888(d, &color);
                    }
                    break;
                default:
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_2_argb8888_opacity(d, &color, opacity_value);
                    }
                    break;
                }
            }
        }

    }
    return;
}
