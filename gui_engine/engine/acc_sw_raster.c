/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_raster.c
  * @brief deal with no rle filter mode
  * @details input:rgba/rgb/rgb565;output:rgba/rgb565
  * @author howie_wang@realsil.com.cn
  * @date 2024/04/09
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
#include <acc_sw_raster.h>
#include <acc_sw_rle.h>

#pragma pack(1)
typedef struct _color_rgb888
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
} color_rgb888_t;

typedef struct _color_rgb565
{
    uint16_t b : 5;
    uint16_t g : 6;
    uint16_t r : 5;
} color_rgb565_t;

typedef struct _color_argb8565
{
    uint16_t b : 5;
    uint16_t g : 6;
    uint16_t r : 5;
    uint8_t a;
} color_argb8565_t;

typedef struct _color_argb88888
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
} color_argb8888_t;
#pragma pack()

static void do_raster_pixel(uint8_t *writebuf, int write_off, int image_base, uint32_t image_off,
                            char input_type, uint8_t dc_bytes_per_pixel, uint8_t opacity_value, uint32_t blend_mode)
{

    uint8_t source_red = 0;
    uint8_t source_green = 0;
    uint8_t source_blue = 0;
    uint8_t source_alpha = 0;

    uint8_t target_red = 0;
    uint8_t target_green = 0;
    uint8_t target_blue = 0;
    uint8_t target_alpha = 0;

    if (input_type == RGB565)
    {
        color_rgb565_t *pixel = (color_rgb565_t *)image_base + image_off;
        source_alpha = 0xff;
        source_red = pixel->r << 3;
        source_green = pixel->g << 2;
        source_blue = pixel->b << 3;
    }
    else if (input_type == RGB888)
    {
        color_rgb888_t *pixel = (color_rgb888_t *)image_base + image_off;
        source_alpha = 0xff;
        source_red = pixel->r;
        source_green = pixel->g;
        source_blue = pixel->b;
    }
    else if (input_type == ARGB8565)
    {
        color_argb8565_t *pixel = (color_argb8565_t *)image_base + image_off;
        source_alpha = pixel->a;
        source_red = pixel->r << 3;
        source_green = pixel->g << 2;
        source_blue = pixel->b << 3;
    }
    else if (input_type == RGBA8888)
    {
        color_argb8888_t *pixel = (color_argb8888_t *)image_base + image_off;
        source_alpha = pixel->a;
        source_red = pixel->r;
        source_green = pixel->g;
        source_blue = pixel->b;
    }


    if (dc_bytes_per_pixel == 2)
    {
        color_rgb565_t *pixel = (color_rgb565_t *)writebuf + write_off;
        target_alpha = 0xff;
        target_red = pixel->r << 3;
        target_green = pixel->g << 2;
        target_blue = pixel->b << 3;
    }
    else if (dc_bytes_per_pixel == 3)
    {
        color_rgb888_t *pixel = (color_rgb888_t *)writebuf + write_off;
        target_alpha = 0xff;
        target_red = pixel->r;
        target_green = pixel->g;
        target_blue = pixel->b;
    }
    else if (dc_bytes_per_pixel == 4)
    {
        color_argb8888_t *pixel = (color_argb8888_t *)writebuf + write_off;
        target_alpha = pixel->a;
        target_red = pixel->r;
        target_green = pixel->g;
        target_blue = pixel->b;
    }

    if (blend_mode == IMG_COVER_MODE)
    {
        target_alpha = source_alpha;
        target_red   = source_red;
        target_green = source_green;
        target_blue  = source_blue;
    }
    else if (blend_mode == IMG_BYPASS_MODE)
    {
        target_alpha = source_alpha * opacity_value / 255 + target_alpha * (255 - opacity_value) / 255;
        target_red   = source_red * opacity_value / 255 +   target_red * (255 - opacity_value) / 255;
        target_green = source_green * opacity_value / 255 + target_green * (255 - opacity_value) / 255;
        target_blue  = source_blue * opacity_value / 255 +  target_blue * (255 - opacity_value) / 255;
    }
    else if (blend_mode == IMG_FILTER_BLACK)
    {
        if ((source_alpha == 0) && (source_red == 0) && (source_green == 0) && (source_blue == 0))
        {
            return;
        }
        target_alpha = source_alpha * opacity_value / 255 + target_alpha * (255 - opacity_value) / 255;
        target_red   = source_red * opacity_value / 255 +   target_red * (255 - opacity_value) / 255;
        target_green = source_green * opacity_value / 255 + target_green * (255 - opacity_value) / 255;
        target_blue  = source_blue * opacity_value / 255 +  target_blue * (255 - opacity_value) / 255;

    }
    else if (blend_mode == IMG_SRC_OVER_MODE)
    {
        source_alpha = source_alpha * opacity_value / 255;

        target_alpha = ((255 - source_alpha) * target_alpha + source_alpha * source_alpha) / 255;
        target_red   = ((255 - source_alpha) * target_red + source_alpha * source_red) / 255;
        target_green = ((255 - source_alpha) * target_green + source_alpha * source_green) / 255;
        target_blue  = ((255 - source_alpha) * target_blue + source_alpha * target_blue) / 255;

    }
    else
    {
        GUI_ASSERT(NULL != NULL);
    }

    if (dc_bytes_per_pixel == 2)
    {
        color_rgb565_t *pixel = (color_rgb565_t *)writebuf + write_off;
        pixel->r = target_red >> 3;
        pixel->g = target_green >> 2;
        pixel->b = target_blue >> 3;
    }
    else if (dc_bytes_per_pixel == 3)
    {
        color_rgb888_t *pixel = (color_rgb888_t *)writebuf + write_off;
        pixel->r = target_red;
        pixel->g = target_green;
        pixel->b = target_blue;
    }
    else if (dc_bytes_per_pixel == 4)
    {
        color_argb8888_t *pixel = (color_argb8888_t *)writebuf + write_off;
        pixel->a = target_alpha;
        pixel->r = target_red;
        pixel->g = target_green;
        pixel->b = target_blue;

    }

}

void do_raster_no_rle(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;


    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_base = sizeof(gui_rgb_data_head_t) + (uint32_t)(image->data);
    gui_rgb_data_head_t *head = image->data;
    char input_type = head->type;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;

    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t source_bytes_per_pixel = gui_image_get_pixel(image);
    uint8_t opacity_value = image->opacity_value;

    uint8_t *writebuf = dc->frame_buf;
    uint32_t blend_mode = image->blend_mode;
    gui_matrix_t *inverse = image->inverse;


    for (uint32_t i = y_start; i <= y_end; i++)
    {
        for (uint32_t j = x_start; j <= x_end; j++)
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
            int image_off = y * source_w + x;
            int write_off = (i - dc->section.y1) * dc->fb_width + j;

            do_raster_pixel(writebuf, write_off, image_base, image_off, input_type, dc_bytes_per_pixel,
                            opacity_value,
                            blend_mode);
        }
    }

}

static void get_rle_pixel(draw_img_t *image, int x, int y, uint8_t *pixel)
{
    gui_img_file_t *file = (gui_img_file_t *)image->data;
    imdc_file_t *compressed = (imdc_file_t *)(&(file->data.idc_file));
    uint32_t line = (uint32_t)compressed + compressed->compressed_addr[y];
    int location = 0;
    imdc_rgb565_node_t *node = NULL;


    do
    {
        node = (imdc_rgb565_node_t *)line;
        location += node->len;
        line = line + sizeof(imdc_rgb565_node_t);
    }
    while (location < x);

    memcpy(pixel, &(node->pixel16), sizeof(node->pixel16));

}

void do_raster_use_rle(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    int16_t x_start = 0;
    int16_t x_end = 0;
    int16_t y_start = 0;
    int16_t y_end = 0;


    if (gui_image_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    gui_rgb_data_head_t *head = image->data;
    char input_type = head->type;
    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;

    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    uint8_t source_bytes_per_pixel = gui_image_get_pixel(image);
    uint8_t opacity_value = image->opacity_value;

    uint8_t *writebuf = dc->frame_buf;
    uint32_t blend_mode = image->blend_mode;
    gui_matrix_t *inverse = image->inverse;


    for (uint32_t i = y_start; i <= y_end; i++)
    {
        for (uint32_t j = x_start; j <= x_end; j++)
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
            int write_off = (i - dc->section.y1) * dc->fb_width + j;

            uint8_t rle_pixel[4];
            get_rle_pixel(image, x, y, rle_pixel);

            do_raster_pixel(writebuf, write_off, (int)rle_pixel, 0, input_type, dc_bytes_per_pixel,
                            opacity_value,
                            blend_mode);
        }
    }

}

