/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_source_over.c
  * @brief Picture software decoding library
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/19
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


void alpha_blend_blit_rgb565_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                      gui_rect_t *rect)
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

    uint8_t source_bytes_per_pixel = 2;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
    uint8_t opacity_value = image ->opacity_value;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;
        uint16_t *writebuf = (uint16_t *)dc->frame_buf;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            uint16_t pixel = *((uint16_t *)read_off + j);
            gui_color_t color = {.color.rgba.a = 255,
                                 .color.rgba.b = (pixel & 0x001f) << 3,
                                 .color.rgba.g = ((pixel & 0x07e0) >> 5) << 2,
                                 .color.rgba.r = (pixel >> 11) << 3,
                                };
            switch (opacity_value)
            {
            case 0:
                break;
            case 255:
                {
                    uint16_t *d = writebuf + (write_off + j);
                    do_blending_rgb565_2_rgb565(d, &color);
                }
                break;
            default:
                {
                    if (opacity_value < 255)
                    {
                        uint16_t *d = writebuf + (write_off + j);
                        do_blending_rgb565_2_rgb565_opacity(d, &color, opacity_value);
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
    return;
}
void alpha_blend_blit_rgb888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                      gui_rect_t *rect)
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
    uint8_t source_bytes_per_pixel = 3;
    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
    uint8_t opacity_value = image->opacity_value;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint16_t *writebuf = (uint16_t *)dc->frame_buf;

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
                    do_blending_rgb888_2_rgb565(d, &color);
                }
                break;
            default:
                {
                    if (opacity_value < 255)
                    {
                        uint16_t *d = writebuf + (write_off + j);
                        do_blending_rgb888_2_rgb565_opacity(d, &color, opacity_value);
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
    return;
}
void alpha_blend_blit_argb8888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                        gui_rect_t *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = image->img_w;
    int image_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);

    uint8_t source_bytes_per_pixel = 4;
    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
    uint8_t opacity_value = image->opacity_value;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint16_t *writebuf = (uint16_t *)dc->frame_buf;

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
                    do_blending_argb8888_2_rgb565(d, &color);
                }
                break;
            default:
                {
                    if (opacity_value < 255)
                    {
                        uint16_t *d = writebuf + (write_off + j);
                        do_blending_argb8888_2_rgb565_opacity(d, &color, opacity_value);
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
    return;
}

void alpha_blend_blit_rgb565_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                      gui_rect_t *rect)
{
    int image_x = rect->x1;
    int image_y = rect->y1;
    int image_w = image->img_w;
    int image_h = image->img_h;

    int x_start = _UI_MAX(image_x, 0);
    int x_end = _UI_MIN(image_x + image_w, dc->fb_width);
    int y_start = _UI_MAX(dc->section.y1, image_y);
    int y_end = _UI_MIN(dc->section.y2, image_y + image_h);
    uint8_t opacity_value = image->opacity_value;

    if ((x_start >= x_end) || (y_start >= y_end))
    {
        return;
    }

    uint8_t source_bytes_per_pixel = 2;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint8_t *writebuf = dc->frame_buf;
        uint16_t pixel;
        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = *((uint16_t *)read_off + j);
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
                    do_blending_rgb565_2_rgb888(d, &color);
                }
                break;
            default:
                {
                    gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                    do_blending_rgb565_2_rgb888_opacity(d, &color, opacity_value);
                }
                break;
            }
        }
    }
    return;
}
void alpha_blend_blit_rgb888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                      gui_rect_t *rect)
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

    uint8_t source_bytes_per_pixel = 3;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
    uint8_t opacity_value = image->opacity_value;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint8_t *writebuf = dc->frame_buf;
        uint8_t *pixel;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
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
                    do_blending_rgb888_2_rgb888(d, &color);
                }
                break;
            default:
                {
                    gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                    do_blending_rgb888_2_rgb888_opacity(d, &color, opacity_value);
                }
                break;
            }
        }
    }
    return;
}
void alpha_blend_blit_rgba8888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                        gui_rect_t *rect)
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

    uint8_t source_bytes_per_pixel = 4;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
    uint8_t opacity_value = image->opacity_value;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

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
                    do_blending_argb8888_2_rgb888(d, &color);
                }
                break;
            default:
                {
                    gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                    do_blending_argb8888_2_rgb888_opacity(d, &color, opacity_value);
                }
                break;
            }
        }
    }
    return;
}
void alpha_blend_blit_rgb565_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                        gui_rect_t *rect)
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

    uint8_t source_bytes_per_pixel = 2;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;
    uint8_t *writebuf = dc->frame_buf;
    uint16_t pixel;
    uint8_t opacity_value = image->opacity_value;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = *((uint16_t *)read_off + j);
            gui_color_t color = {.color.rgba.r = (pixel & 0x001f) << 3 | (pixel & 0x001f) >> 2,
                                 .color.rgba.g = ((pixel & 0x07e0) >> 5) << 2,
                                 .color.rgba.b = (pixel >> 11) << 3,
                                 .color.rgba.a = 255,
                                };

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
                    if (opacity_value < 255)
                    {
                        gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
                        do_blending_rgb565_2_argb8888_opacity(d, &color, opacity_value);
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
    return;
}

void alpha_blend_blit_rgb888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                        gui_rect_t *rect)
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

    uint8_t source_bytes_per_pixel = 3;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

        uint8_t *writebuf = dc->frame_buf;
        uint8_t *pixel;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
            gui_color_t color = {.color.rgba.b = pixel[2],
                                 .color.rgba.g = pixel[1],
                                 .color.rgba.r = pixel[0],
                                 .color.rgba.a = 255,
                                };
            uint8_t opacity_value = image->opacity_value;
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
    return;
}
void alpha_blend_blit_argb8888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                          gui_rect_t *rect)
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

    uint8_t source_bytes_per_pixel = 4;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    int read_x_off = -_UI_MIN(image_x, 0) * source_bytes_per_pixel  + image_off;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        int read_off = ((i - image_y) * image_w) * source_bytes_per_pixel + read_x_off -
                       source_bytes_per_pixel * x_start;

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
            uint8_t opacity_value = image->opacity_value;
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
    return;
}
