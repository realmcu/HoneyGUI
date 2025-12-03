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
#include "string.h"
#include "l3_sw_raster.h"
#include "l3_common.h"



void gui_get_source_color(uint8_t *source_red, uint8_t *source_green, uint8_t *source_blue,
                          uint8_t *source_alpha,
                          uintptr_t image_base, uint32_t image_off, char input_type)
{
    switch (input_type)
    {
    case L3_RGB565:
        {
            l3_color_rgb565_t *pixel = (l3_color_rgb565_t *)(uintptr_t)image_base + image_off;
            *source_alpha = 0xff;
            *source_red = pixel->r << 3;
            *source_green = pixel->g << 2;
            *source_blue = pixel->b << 3;
            break;
        }
    case L3_RGB888:
        {
            l3_color_rgb888_t *pixel = (l3_color_rgb888_t *)(uintptr_t)image_base + image_off;
            *source_alpha = 0xff;
            *source_red = pixel->r;
            *source_green = pixel->g;
            *source_blue = pixel->b;
            break;
        }
    case L3_ARGB8565:
        {
            l3_color_argb8565_t *pixel = (l3_color_argb8565_t *)(uintptr_t)image_base + image_off;
            *source_alpha = pixel->a;
            *source_red = pixel->r << 3;
            *source_green = pixel->g << 2;
            *source_blue = pixel->b << 3;
            break;
        }
    case L3_ARGB8888:
        {
            l3_color_argb8888_t *pixel = (l3_color_argb8888_t *)(uintptr_t)image_base + image_off;
            *source_alpha = pixel->a;
            *source_red = pixel->r;
            *source_green = pixel->g;
            *source_blue = pixel->b;
            break;
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
            l3_color_rgb565_t *pixel = (l3_color_rgb565_t *)(uintptr_t)writebuf + write_off;
            *target_alpha = 0xff;
            *target_red = pixel->r << 3;
            *target_green = pixel->g << 2;
            *target_blue = pixel->b << 3;
            break;
        }
    case 3:
        {
            l3_color_rgb888_t *pixel = (l3_color_rgb888_t *)(uintptr_t)writebuf + write_off;
            *target_alpha = 0xff;
            *target_red = pixel->r;
            *target_green = pixel->g;
            *target_blue = pixel->b;
            break;
        }
    case 4:
        {
            l3_color_argb8888_t *pixel = (l3_color_argb8888_t *)(uintptr_t)writebuf + write_off;
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
    case L3_IMG_COVER_MODE:
        {
            *target_alpha = source_alpha;
            *target_red = source_red;
            *target_green = source_green;
            *target_blue = source_blue;
            break;
        }
    case L3_IMG_BYPASS_MODE:
        {
            uint8_t blend_opacity = opacity_value;
            blend_colors(target_alpha, source_alpha, blend_opacity);
            blend_colors(target_red, source_red, blend_opacity);
            blend_colors(target_green, source_green, blend_opacity);
            blend_colors(target_blue, source_blue, blend_opacity);
            break;
        }
    case L3_IMG_FILTER_BLACK:
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
    case L3_IMG_SRC_OVER_MODE:
    case L3_IMG_RECT:
        {
            source_alpha = (source_alpha * opacity_value) / 255;
            blend_colors(target_alpha, source_alpha, source_alpha);
            blend_colors(target_red, source_red, source_alpha);
            blend_colors(target_green, source_green, source_alpha);
            blend_colors(target_blue, source_blue, source_alpha);
            break;
        }
    default:
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
            l3_color_rgb565_t *pixel = (l3_color_rgb565_t *)(uintptr_t)writebuf + write_off;
            pixel->r = target_red >> 3;
            pixel->g = target_green >> 2;
            pixel->b = target_blue >> 3;
            break;
        }
    case 3:
        {
            l3_color_rgb888_t *pixel = (l3_color_rgb888_t *)(uintptr_t)writebuf + write_off;
            pixel->r = target_red;
            pixel->g = target_green;
            pixel->b = target_blue;
            break;
        }
    case 4:
        {
            l3_color_argb8888_t *pixel = (l3_color_argb8888_t *)(uintptr_t)writebuf + write_off;
            pixel->a = target_alpha;
            pixel->r = target_red;
            pixel->g = target_green;
            pixel->b = target_blue;
            break;
        }
    }
}

void gui_get_rle_pixel(l3_draw_rect_img_t *image, int x, int y, uint8_t *pixel)//todo
{
    l3_img_file_t *file = (l3_img_file_t *)image->data;
    l3_img_head_t *head = image->data;
    char input_type = head->type;
    l3_imdc_file_t *compressed = (l3_imdc_file_t *)(&(file->data.imdc_file));
    uint32_t line = (uintptr_t)compressed + compressed->compressed_addr[y];
    int location = 0;
    switch (input_type)
    {
    case L3_RGB565:
        {
            l3_imdc_rgb565_node_t *node = NULL;
            do
            {
                node = (l3_imdc_rgb565_node_t *)(uintptr_t)line;
                location += node->len;
                line = line + sizeof(l3_imdc_rgb565_node_t);
            }
            while (location < (x + 1));
            memcpy(pixel, &(node->pixel16), sizeof(node->pixel16));
            break;
        }
    case L3_ARGB8565:
        {
            l3_imdc_argb8565_node_t *node = NULL;
            do
            {
                node = (l3_imdc_argb8565_node_t *)(uintptr_t)line;
                location += node->len;
                line = line + sizeof(l3_imdc_argb8565_node_t);
            }
            while (location < (x + 1));
            memcpy(pixel, &(node->pixel), sizeof(node->pixel));
            pixel[2] = node->alpha; // Assuming 'pixel' is at least 3 bytes, adjust index if necessary
            break;
        }
    case L3_RGB888:
        {
            l3_imdc_rgb888_node_t *node = NULL;
            do
            {
                node = (l3_imdc_rgb888_node_t *)(uintptr_t)line;
                location += node->len;
                line = line + sizeof(l3_imdc_rgb888_node_t);
            }
            while (location < (x + 1));
            pixel[2] = node->pixel_r;
            pixel[1] = node->pixel_g;
            pixel[0] = node->pixel_b;
            break;
        }
    case L3_ARGB8888:
        {
            l3_imdc_argb8888_node_t *node = NULL;
            do
            {
                node = (l3_imdc_argb8888_node_t *)(uintptr_t)line;
                location += node->len;
                line = line + sizeof(l3_imdc_argb8888_node_t);
            }
            while (location < (x + 1));
            memcpy(pixel, &(node->pixel32), sizeof(node->pixel32));
            break;
        }
    default:
        {
            return;
        }
    }
}

void do_raster_pixel(const gui_raster_params_t *params)
{
    uint8_t source_red, source_green, source_blue, source_alpha;
    uint8_t target_red, target_green, target_blue, target_alpha;

    gui_get_source_color(&source_red, &source_green, &source_blue, &source_alpha,
                         params->image_base, params->image_off, params->input_type);

    gui_get_target_color(&target_red, &target_green, &target_blue, &target_alpha,
                         params->writebuf, params->write_off, params->dc_bytes_per_pixel);

    gui_apply_blend_mode(&target_red, &target_green, &target_blue, &target_alpha,
                         source_red, source_green, source_blue, source_alpha,
                         params->opacity_value, params->blend_mode);

    gui_set_pixel_color(params->writebuf, params->write_off, params->dc_bytes_per_pixel,
                        target_red, target_green, target_blue, target_alpha);
}

bool l3_draw_img_target_area(l3_draw_rect_img_t *image, l3_canvas_t *dc, l3_rect_t *rect,
                             int32_t *x_start, int32_t *x_end, int32_t *y_start, int32_t *y_end)
{
    (void)rect;
    int16_t image_x = image->img_target_x;
    int16_t image_y = image->img_target_y;

    int16_t image_w = image->img_target_w ;
    int16_t image_h = image->img_target_h ;

    *x_start = _UI_MAX(image_x, dc->section.x1);
    *x_end = _UI_MIN(image_x + image_w - 1, dc->section.x2);

    *y_start = _UI_MAX(image_y, dc->section.y1);
    *y_end = _UI_MIN(image_y + image_h - 1, dc->section.y2);

    if ((*x_start >= *x_end) || (*y_start >= *y_end))
    {
        return false;
    }
    return true;
}




