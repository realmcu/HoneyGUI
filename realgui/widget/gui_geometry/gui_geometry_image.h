/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __GUI_GEOMETRY_IMAGE_H__
#define __GUI_GEOMETRY_IMAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "guidef.h"
#include "draw_img.h"

/**
 * Most pieces one shape is drawn from.  A stroked rect is the largest: four
 * corners, up to three runs on each side (stroke, stroke/fill edge row, fill)
 * and the centre, 4 + 4 * 3 + 1 = 17.  A circle needs at most 9.
 */
#define GUI_GEOMETRY_MAX_PARTS 17

/**
 * One piece of a split shape: an image placed inside the widget box.
 *
 * The piece maps its own pixel (u, v) to widget pixel
 * (x + xx * u + xy * v, y + yx * u + yy * v).  The 2x2 part is a signed
 * permutation, so a single payload can be drawn mirrored or transposed and
 * still land exactly on whole pixels.
 */
typedef struct
{
    draw_img_t *img;
    int16_t x;
    int16_t y;
    int8_t xx;
    int8_t xy;
    int8_t yx;
    int8_t yy;
} gui_geometry_part_t;

/**
 * The pieces a shape is drawn from.  They never overlap, so each pixel is
 * drawn exactly once and widget opacity applies to it once, the same as if the
 * whole shape were one image.
 */
typedef struct
{
    gui_geometry_part_t part[GUI_GEOMETRY_MAX_PARTS];
    uint8_t count;              /**< Pieces in use; 0 means nothing is built. */
} gui_geometry_parts_t;

void gui_geometry_data_head_set(gui_rgb_data_head_t *head, int16_t w, int16_t h,
                                GUI_FormatType type);

bool gui_geometry_image_buffer_size(int32_t w, int32_t h, uint32_t pixel_bytes,
                                    uint32_t *buffer_size);

void gui_geometry_img_bind(draw_img_t *img, uint8_t *payload, uint8_t opacity,
                           bool is_a8, gui_color_t color);

void gui_geometry_draw_img_release(draw_img_t **img);

uint32_t gui_geometry_dither_argb8888(uint32_t color, int x, int y);

static inline uint32_t gui_geometry_dither_argb8888_inline(uint32_t color, int x, int y)
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
    int r = ((color >> 16) & 0xFF) + d;
    int g = ((color >> 8) & 0xFF) + d;
    int b = (color & 0xFF) + d;

    r = r < 0 ? 0 : (r > 255 ? 255 : r);
    g = g < 0 ? 0 : (g > 255 ? 255 : g);
    b = b < 0 ? 0 : (b > 255 ? 255 : b);

    return ((uint32_t)a << 24) | ((uint32_t)r << 16) |
           ((uint32_t)g << 8) | (uint32_t)b;
}

bool gui_geometry_use_a8(bool enabled, bool has_gradient, bool is_stroke);

bool gui_geometry_use_split_a8(bool use_a8, uint8_t opacity, bool parent_opaque);

uint32_t gui_geometry_mix_fill_stroke(uint32_t fill, uint32_t stroke, float c_out, float c_in);

uint8_t *gui_geometry_payload_acquire(const void *desc, uint16_t desc_len,
                                      int w, int h, bool is_a8, bool *is_new);

bool gui_geometry_parts_add(gui_geometry_parts_t *parts, const gui_matrix_t *matrix,
                            draw_img_t *img, int x, int y,
                            int xx, int xy, int yx, int yy);

bool gui_geometry_parts_add_image(gui_geometry_parts_t *parts, const gui_matrix_t *matrix,
                                  uint8_t *payload, uint8_t opacity, bool is_a8,
                                  gui_color_t color, int x, int y,
                                  int xx, int xy, int yx, int yy);

bool gui_geometry_parts_add_solid(gui_geometry_parts_t *parts, const gui_matrix_t *matrix,
                                  int x, int y, int w, int h, uint32_t argb, uint8_t opacity);

void gui_geometry_parts_place(gui_geometry_parts_t *parts, const gui_matrix_t *matrix);

void gui_geometry_parts_draw(gui_geometry_parts_t *parts, uint8_t opacity);

void gui_geometry_parts_end(gui_geometry_parts_t *parts);

void gui_geometry_parts_release(gui_geometry_parts_t *parts);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_GEOMETRY_IMAGE_H__ */
