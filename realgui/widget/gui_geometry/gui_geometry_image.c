/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include "gui_geometry_cache.h"
#include "gui_geometry_image.h"

void gui_geometry_data_head_set(gui_rgb_data_head_t *head, int16_t w, int16_t h,
                                GUI_FormatType type)
{
    GUI_ASSERT(head != NULL);

    memset(head, 0x00, sizeof(*head));
    head->type = (char)type;
    head->w = w;
    head->h = h;
}

bool gui_geometry_image_buffer_size(int32_t w, int32_t h, uint32_t pixel_bytes,
                                    uint32_t *buffer_size)
{
    if (buffer_size == NULL || w <= 0 || h <= 0 ||
        w > INT16_MAX || h > INT16_MAX || pixel_bytes == 0u)
    {
        return false;
    }

    uint64_t size = (uint64_t)(uint32_t)w * (uint32_t)h * pixel_bytes +
                    sizeof(gui_rgb_data_head_t);
    if (size > UINT32_MAX)
    {
        return false;
    }

    *buffer_size = (uint32_t)size;
    return true;
}

void gui_geometry_img_bind(draw_img_t *img, uint8_t *payload, uint8_t opacity,
                           bool is_a8, gui_color_t color)
{
    GUI_ASSERT(img != NULL);

    img->data = payload;
    img->opacity_value = opacity;
    img->high_quality = 1;

    if (is_a8)
    {
        img->blend_mode = IMG_2D_SW_FIX_A8_FG;
        img->fg_color_set = 0xFF000000u | (color.color.argb_full & 0x00FFFFFFu);
    }
    else
    {
        img->blend_mode = IMG_SRC_OVER_MODE;
    }
}

void gui_geometry_draw_img_release(draw_img_t **img)
{
    if (img == NULL || *img == NULL)
    {
        return;
    }

    if ((*img)->acc_user != NULL)
    {
        gui_free((*img)->acc_user);
        (*img)->acc_user = NULL;
    }

    if ((*img)->data != NULL)
    {
        if ((*img)->blend_mode == IMG_RECT)
        {
            gui_free((*img)->data);
        }
        else
        {
            gui_geometry_cache_release((*img)->data);
        }
    }

    gui_free(*img);
    *img = NULL;
}

uint32_t gui_geometry_dither_argb8888(uint32_t color, int x, int y)
{
    static const int8_t bayer4x4[16] =
    {
        -8,  0, -6,  2,
        4, -4,  6, -2,
        -5,  3, -7,  1,
        7, -1,  5, -3,
    };
    int d = bayer4x4[(y & 3) * 4 + (x & 3)];
    int a = (color >> 24) & 0xFF;
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;

    r += d;
    g += d;
    b += d;
    r = r < 0 ? 0 : (r > 255 ? 255 : r);
    g = g < 0 ? 0 : (g > 255 ? 255 : g);
    b = b < 0 ? 0 : (b > 255 ? 255 : b);

    return ((uint32_t)a << 24) | ((uint32_t)r << 16) |
           ((uint32_t)g << 8) | (uint32_t)b;
}

bool gui_geometry_use_a8(bool enabled, bool has_gradient, bool is_stroke)
{
    return enabled && (is_stroke || !has_gradient);
}

bool gui_geometry_use_split_a8(bool use_a8, uint8_t opacity, bool parent_opaque)
{
    return use_a8 && opacity == UINT8_MAX && parent_opaque;
}
