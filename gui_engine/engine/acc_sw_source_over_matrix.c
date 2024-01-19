/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_source_over_matrix.c
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
#include "acc_sw_blend.h"
#include "math.h"

void alpha_matrix_blit_argb8888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                           gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }


    struct gui_matrix *inverse = image->inverse;
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t *writebuf = dc->frame_buf;
    uint8_t source_bytes_per_pixel = 4;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;

    for (uint32_t i = y_start; i < y_end; i++)
    {

        int write_off = (i - dc->section.y1) * dc->fb_width ;


        for (uint32_t j = x_start; j < x_end; j++)
        {

            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }

            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);
            uint8_t opacity_value = image->opacity_value;

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
    return;
}
void alpha_matrix_blit_rgb888_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                         gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint8_t source_bytes_per_pixel = 3;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    struct gui_matrix *inverse = image->inverse;
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);

    for (uint32_t i = y_start; i < y_end; i++)
    {

        int write_off = (i - dc->section.y1) * dc->fb_width ;

        uint8_t *writebuf = dc->frame_buf;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }
            // pixel = (uint8_t *)(read_off + j * source_bytes_per_pixel);
            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);
            gui_color_t color = {.color.rgba.r = pixel[0],
                                 .color.rgba.g = pixel[1],
                                 .color.rgba.b = pixel[2],
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
    return;
}
void alpha_matrix_blit_rgb565_2_argb8888(draw_img_t *image, struct gui_dispdev *dc,
                                         gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint8_t *writebuf = dc->frame_buf;

    struct gui_matrix *inverse = image->inverse;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }
            uint16_t pixel = *((uint16_t *)image_off + y * source_w + x);
            gui_color_t color = {.color.rgba.r = (pixel & 0x001f) << 3,
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
    return;
}
void alpha_matrix_blit_argb8565_2_rgba8888(draw_img_t *image, struct gui_dispdev *dc,
                                           gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint8_t *writebuf = dc->frame_buf;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    struct gui_matrix *inverse = image->inverse;

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t opacity_value = image->opacity_value;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }
            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * 3);
            uint8_t alpha = pixel[2];
            uint16_t color_t = (uint16_t)(((uint16_t)(pixel[1]) << 8) + pixel[0]);
            gui_color_t color = {.color.rgba.b = (color_t >> 11) << 3,
                                 .color.rgba.g = ((color_t & 0x07e0) >> 5) << 2,
                                 .color.rgba.r = (color_t & 0x001f) << 3,
                                 .color.rgba.a = alpha,
                                };
            gui_color_t *d = (gui_color_t *)(writebuf + (write_off + j) * dc_bytes_per_pixel);
            switch (opacity_value)
            {
            case 0:
                break;
            case 255:
                {
                    do_blending_2_argb8888(d, &color);
                }
                break;
            default:
                {
                    if (opacity_value < 255)
                    {
                        opacity_value = alpha * opacity_value / 255;
                        do_blending_2_argb8888_opacity(d, &color, opacity_value);
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
void alpha_matrix_blit_rgb565_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                       gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint16_t *writebuf = (uint16_t *)dc->frame_buf;

    struct gui_matrix *inverse = image->inverse;
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t opacity_value = image->opacity_value;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }
            uint16_t pixel = *((uint16_t *)image_off + y * source_w + x);
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
    return;
}
void alpha_matrix_blit_argb8565_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                         gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint16_t *writebuf = (uint16_t *)dc->frame_buf;

    struct gui_matrix *inverse = image->inverse;


    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t opacity_value = image->opacity_value;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }
            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * 3);
            uint8_t alpha = pixel[2];
            uint16_t color_t = (uint16_t)(((uint16_t)(pixel[1]) << 8) + pixel[0]);
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
                        opacity_value = alpha * opacity_value / 255;
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
    return;
}
void alpha_matrix_blit_rgb888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                       gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;


    uint8_t source_bytes_per_pixel = 3;
    uint8_t opacity_value = image->opacity_value;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            float X = image->inverse->m[0][0] * j + image->inverse->m[0][1] * i + image->inverse->m[0][2];
            float Y = image->inverse->m[1][0] * j + image->inverse->m[1][1] * i + image->inverse->m[1][2];
            float Z = image->inverse->m[2][0] * j + image->inverse->m[2][1] * i + image->inverse->m[2][2];
            int x = round(X / Z);
            int y = round(Y / Z);

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }

            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);
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
    return;
}

void alpha_matrix_blit_rgba8888_2_rgb565(draw_img_t *image, struct gui_dispdev *dc,
                                         gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;


    uint8_t source_bytes_per_pixel = 4;
    uint8_t opacity_value = image->opacity_value;


    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            float X = image->inverse->m[0][0] * j + image->inverse->m[0][1] * i + image->inverse->m[0][2];
            float Y = image->inverse->m[1][0] * j + image->inverse->m[1][1] * i + image->inverse->m[1][2];
            float Z = image->inverse->m[2][0] * j + image->inverse->m[2][1] * i + image->inverse->m[2][2];
            int x = round(X / Z);
            int y = round(Y / Z);
            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }
            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);

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
    return;
}

void alpha_matrix_blit_rgb565_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                       gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t *writebuf = dc->frame_buf;


    uint8_t source_bytes_per_pixel = 2;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    struct gui_matrix *inverse = image->inverse;
    uint8_t opacity_value = image->opacity_value;
    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {
            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = X / Z;
            int y = Y / Z;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }
            uint16_t pixel = *(uint16_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);

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
    return;
}
void alpha_matrix_blit_rgb888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                       gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }
    struct gui_matrix *inverse = image->inverse;
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t *writebuf = dc->frame_buf;


    uint8_t source_bytes_per_pixel = 3;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {

            int x = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            int y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }

            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);
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
    return;
}
void alpha_matrix_blit_rgba8888_2_rgb888(draw_img_t *image, struct gui_dispdev *dc,
                                         gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }
    struct gui_matrix *inverse = image->inverse;
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    uint8_t *writebuf = dc->frame_buf;


    uint8_t source_bytes_per_pixel = 4;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t opacity_value = image->opacity_value;

    for (uint32_t i = y_start; i < y_end; i++)
    {
        int write_off = (i - dc->section.y1) * dc->fb_width ;

        for (uint32_t j = x_start; j < x_end; j++)
        {

            int x = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            int y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                continue;
            }

            uint8_t *pixel = (uint8_t *)(image_off + (y * source_w + x) * source_bytes_per_pixel);
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
    return;
}

