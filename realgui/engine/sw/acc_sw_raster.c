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
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "draw_img.h"

typedef struct
{
    uint8_t *writebuf;
    int write_off;
    uint32_t image_base;
    uint32_t image_off;
    char input_type;
    uint8_t dc_bytes_per_pixel;
    uint8_t opacity_value;
    uint32_t blend_mode;

    uint8_t *palette_data;
    uint8_t *palette_index;
} gui_raster_params_t;

static void gui_get_source_color(uint8_t *source_red, uint8_t *source_green, uint8_t *source_blue,
                                 uint8_t *source_alpha,
                                 uint32_t image_base, uint32_t image_off, char input_type, uint8_t *palette_data,
                                 uint8_t *palette_index)
{
    switch (input_type)
    {
    case RGB565:
        {
            color_rgb565_t *pixel = (color_rgb565_t *)(uintptr_t)image_base + image_off;
            *source_alpha = 0xff;
            *source_red = pixel->r << 3;
            *source_green = pixel->g << 2;
            *source_blue = pixel->b << 3;
            break;
        }
    case RGB888:
        {
            color_rgb888_t *pixel = (color_rgb888_t *)(uintptr_t)image_base + image_off;
            *source_alpha = 0xff;
            *source_red = pixel->r;
            *source_green = pixel->g;
            *source_blue = pixel->b;
            break;
        }
    case ARGB8565:
        {
            color_argb8565_t *pixel = (color_argb8565_t *)(uintptr_t)image_base + image_off;
            *source_alpha = pixel->a;
            *source_red = pixel->r << 3;
            *source_green = pixel->g << 2;
            *source_blue = pixel->b << 3;
            break;
        }
    case ARGB8888:
        {
            color_argb8888_t *pixel = (color_argb8888_t *)(uintptr_t)image_base + image_off;
            *source_alpha = pixel->a;
            *source_red = pixel->r;
            *source_green = pixel->g;
            *source_blue = pixel->b;
            break;
        }
    case PALETTE:
        {
            uint8_t index = palette_index[image_off];
            uint8_t *color = palette_data + index * 3; // Assuming palette_data is RGB triplets
            *source_alpha = 0xff;
            *source_red = color[0];
            // *source_red = 0xFF;
            *source_green = color[1];
            *source_blue = color[2];
            break;
        }
    default:
        {
            GUI_ASSERT(NULL != NULL); // Ensure an error is noticed
        }
    }
}

static void gui_get_target_color(uint8_t *target_red, uint8_t *target_green, uint8_t *target_blue,
                                 uint8_t *target_alpha,
                                 uint8_t *writebuf, int write_off, uint8_t dc_bytes_per_pixel)
{
    switch (dc_bytes_per_pixel)
    {
    case 2:
        {
            color_rgb565_t *pixel = (color_rgb565_t *)(uintptr_t)writebuf + write_off;
            *target_alpha = 0xff;
            *target_red = pixel->r << 3;
            *target_green = pixel->g << 2;
            *target_blue = pixel->b << 3;
            break;
        }
    case 3:
        {
            color_rgb888_t *pixel = (color_rgb888_t *)(uintptr_t)writebuf + write_off;
            *target_alpha = 0xff;
            *target_red = pixel->r;
            *target_green = pixel->g;
            *target_blue = pixel->b;
            break;
        }
    case 4:
        {
            color_argb8888_t *pixel = (color_argb8888_t *)(uintptr_t)writebuf + write_off;
            *target_alpha = pixel->a;
            *target_red = pixel->r;
            *target_green = pixel->g;
            *target_blue = pixel->b;
            break;
        }
    }
}

static void blend_colors(uint8_t *target_channel, uint8_t source_channel, uint8_t source_alpha)
{
    *target_channel = ((255 - source_alpha) * *target_channel + source_alpha * source_channel) / 255;
}

static void gui_apply_blend_mode(uint8_t *target_red, uint8_t *target_green, uint8_t *target_blue,
                                 uint8_t *target_alpha,
                                 uint8_t source_red, uint8_t source_green, uint8_t source_blue, uint8_t source_alpha,
                                 uint8_t opacity_value, uint32_t blend_mode)
{

    switch (blend_mode)
    {
    case IMG_COVER_MODE:
        {
            *target_alpha = source_alpha;
            *target_red = source_red;
            *target_green = source_green;
            *target_blue = source_blue;
            break;
        }
    case IMG_BYPASS_MODE:
        {
            uint8_t blend_opacity = opacity_value;
            blend_colors(target_alpha, source_alpha, blend_opacity);
            blend_colors(target_red, source_red, blend_opacity);
            blend_colors(target_green, source_green, blend_opacity);
            blend_colors(target_blue, source_blue, blend_opacity);
            break;
        }
    case IMG_FILTER_BLACK:
        {
            if (source_red == 0 && source_green == 0 && source_blue == 0)
            {
                return; // If source is black, do nothing
            }
            source_alpha = (source_alpha * opacity_value) / 255;
            blend_colors(target_alpha, source_alpha, source_alpha);
            blend_colors(target_red, source_red, source_alpha);
            blend_colors(target_green, source_green, source_alpha);
            blend_colors(target_blue, source_blue, source_alpha);
            break;
        }
    case IMG_SRC_OVER_MODE:
    case IMG_RECT:
        {
            source_alpha = (source_alpha * opacity_value) / 255;
            blend_colors(target_alpha, source_alpha, source_alpha);
            blend_colors(target_red, source_red, source_alpha);
            blend_colors(target_green, source_green, source_alpha);
            blend_colors(target_blue, source_blue, source_alpha);
            break;
        }
    default:
        GUI_ASSERT(NULL != NULL); // Ensure an error is noticed
        break;
    }
}

static void gui_set_pixel_color(uint8_t *writebuf, int write_off, uint8_t dc_bytes_per_pixel,
                                uint8_t target_red, uint8_t target_green, uint8_t target_blue, uint8_t target_alpha)
{
    switch (dc_bytes_per_pixel)
    {
    case 2:
        {
            color_rgb565_t *pixel = (color_rgb565_t *)(uintptr_t)writebuf + write_off;
            pixel->r = target_red >> 3;
            pixel->g = target_green >> 2;
            pixel->b = target_blue >> 3;
            break;
        }
    case 3:
        {
            color_rgb888_t *pixel = (color_rgb888_t *)(uintptr_t)writebuf + write_off;
            pixel->r = target_red;
            pixel->g = target_green;
            pixel->b = target_blue;
            break;
        }
    case 4:
        {
            color_argb8888_t *pixel = (color_argb8888_t *)(uintptr_t)writebuf + write_off;
            pixel->a = target_alpha;
            pixel->r = target_red;
            pixel->g = target_green;
            pixel->b = target_blue;
            break;
        }
    }
}
static void do_raster_pixel(const gui_raster_params_t *params)
{
    uint8_t source_red, source_green, source_blue, source_alpha;
    uint8_t target_red, target_green, target_blue, target_alpha;

    gui_get_source_color(&source_red, &source_green, &source_blue, &source_alpha,
                         params->image_base, params->image_off, params->input_type, params->palette_data,
                         params->palette_index);

    gui_get_target_color(&target_red, &target_green, &target_blue, &target_alpha,
                         params->writebuf, params->write_off, params->dc_bytes_per_pixel);

    gui_apply_blend_mode(&target_red, &target_green, &target_blue, &target_alpha,
                         source_red, source_green, source_blue, source_alpha,
                         params->opacity_value, params->blend_mode);

    gui_set_pixel_color(params->writebuf, params->write_off, params->dc_bytes_per_pixel,
                        target_red, target_green, target_blue, target_alpha);
}

static void gui_get_rle_pixel(draw_img_t *image, int x, int y, uint8_t *pixel)
{
    static int x_record = -1;
    static int y_record = -1;
    static uint32_t line = 0;
    static int location = 0;
    static draw_img_t *image_record = NULL;
    gui_img_file_t *file = (gui_img_file_t *)image->data;
    gui_rgb_data_head_t *head = image->data;
    char input_type = head->type;
    imdc_file_t *compressed = (imdc_file_t *)(&(file->data.imdc_file));
    if (y != y_record || x < x_record || image_record != image)
    {
        x_record = x;
        y_record = y;
        image_record = image;
        location = 0;
        line = (uint32_t)(uintptr_t)compressed + compressed->compressed_addr[y];
    }
    switch (input_type)
    {
    case RGB565:
        {
            imdc_rgb565_node_t *node = NULL;
            do
            {
                node = (imdc_rgb565_node_t *)(uintptr_t)line;
                location += node->len;
                line = line + sizeof(imdc_rgb565_node_t);
            }
            while (location < (x + 1));
            location -= node->len;
            line -= sizeof(imdc_rgb565_node_t);
            memcpy(pixel, &(node->pixel16), sizeof(node->pixel16));
            break;
        }
    case ARGB8565:
        {
            imdc_argb8565_node_t *node = NULL;
            do
            {
                node = (imdc_argb8565_node_t *)(uintptr_t)line;
                location += node->len;
                line = line + sizeof(imdc_argb8565_node_t);
            }
            while (location < (x + 1));
            location -= node->len;
            line -= sizeof(imdc_argb8565_node_t);
            memcpy(pixel, &(node->pixel), sizeof(node->pixel));
            pixel[2] = node->alpha; // Assuming 'pixel' is at least 3 bytes, adjust index if necessary
            break;
        }
    case RGB888:
        {
            imdc_rgb888_node_t *node = NULL;
            do
            {
                node = (imdc_rgb888_node_t *)(uintptr_t)line;
                location += node->len;
                line = line + sizeof(imdc_rgb888_node_t);
            }
            while (location < (x + 1));
            location -= node->len;
            line -= sizeof(imdc_rgb888_node_t);
            pixel[2] = node->pixel_r;
            pixel[1] = node->pixel_g;
            pixel[0] = node->pixel_b;
            break;
        }
    case ARGB8888:
        {
            imdc_argb8888_node_t *node = NULL;
            do
            {
                node = (imdc_argb8888_node_t *)(uintptr_t)line;
                location += node->len;
                line = line + sizeof(imdc_argb8888_node_t);
            }
            while (location < (x + 1));
            location -= node->len;
            line -= sizeof(imdc_argb8888_node_t);
            memcpy(pixel, &(node->pixel32), sizeof(node->pixel32));
            break;
        }
    default:
        {
            gui_log("Unsupported image type in RLE: %d\n", input_type);
            return;
        }
    }
}


void do_raster_no_rle(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    int32_t x_start = 0, x_end = 0, y_start = 0, y_end = 0;
    if (!draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end)) { return; }

    uint32_t image_base = sizeof(gui_rgb_data_head_t) + (uint32_t)(uintptr_t)(image->data);
    gui_rgb_data_head_t *head = image->data;
    char input_type = head->type;
    int16_t source_w = image->img_w, source_h = image->img_h;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3, opacity_value = image->opacity_value;
    uint8_t *writebuf = dc->frame_buf;
    uint32_t blend_mode = image->blend_mode;
    gui_matrix_t *inverse = &image->inverse;

    gui_raster_params_t params = { writebuf, 0, image_base, 0, input_type, dc_bytes_per_pixel, opacity_value, blend_mode };
    params.palette_index = ((gui_palette_file_t *)head)->palette_index;
    params.palette_data = ((gui_palette_file_t *)head)->palette_data;

    for (int32_t i = y_start; i <= y_end; i++)
    {
        for (int32_t j = x_start; j <= x_end; j++)
        {
            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = roundf(X / Z);
            int y = roundf(Y / Z);

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h)) { continue; }
            if (rect && ((x > rect->x2) || (x < rect->x1) || (y < rect->y1) || (y > rect->y2))) { continue; }

            int read_off = (image->blend_mode == IMG_RECT) ? 0 : y * source_w + x;
            params.image_off = read_off;
            params.write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + j -
                               dc->section.x1;

            do_raster_pixel(&params);
        }
    }
}


void do_raster_use_rle(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    int32_t x_start = 0, x_end = 0, y_start = 0, y_end = 0;
    if (!draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end)) { return; }

    gui_rgb_data_head_t *head = image->data;
    char input_type = head->type;
    int16_t source_w = image->img_w, source_h = image->img_h;
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3, opacity_value = image->opacity_value;
    uint8_t *writebuf = dc->frame_buf;
    uint32_t blend_mode = image->blend_mode;
    gui_matrix_t *inverse = &image->inverse;

    gui_raster_params_t params = { writebuf, 0, 0, 0, input_type, dc_bytes_per_pixel, opacity_value, blend_mode };
    uint8_t rle_pixel[4];

    for (int32_t i = y_start; i <= y_end; i++)
    {
        for (int32_t j = x_start; j <= x_end; j++)
        {
            float X = inverse->m[0][0] * j + inverse->m[0][1] * i + inverse->m[0][2];
            float Y = inverse->m[1][0] * j + inverse->m[1][1] * i + inverse->m[1][2];
            float Z = inverse->m[2][0] * j + inverse->m[2][1] * i + inverse->m[2][2];
            int x = roundf(X / Z);
            int y = roundf(Y / Z);

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h)) { continue; }
            if (rect && ((x >= rect->x2) || (x < rect->x1) || (y < rect->y1) || (y >= rect->y2))) { continue; }

            int write_off = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + j - dc->section.x1;

            gui_get_rle_pixel(image, x, y, rle_pixel);
            params.image_base = (uint32_t)(uintptr_t)rle_pixel;
            params.write_off = write_off;

            do_raster_pixel(&params);
        }
    }
}
