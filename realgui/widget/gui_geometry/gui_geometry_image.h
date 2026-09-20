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

void gui_geometry_data_head_set(gui_rgb_data_head_t *head, int16_t w, int16_t h,
                                GUI_FormatType type);

bool gui_geometry_image_buffer_size(int32_t w, int32_t h, uint32_t pixel_bytes,
                                    uint32_t *buffer_size);

void gui_geometry_img_bind(draw_img_t *img, uint8_t *payload, uint8_t opacity,
                           bool is_a8, gui_color_t color);

void gui_geometry_draw_img_release(draw_img_t **img);

uint32_t gui_geometry_dither_argb8888(uint32_t color, int x, int y);

bool gui_geometry_use_a8(bool enabled, bool has_gradient, bool is_stroke);

bool gui_geometry_use_split_a8(bool use_a8, uint8_t opacity, bool parent_opaque);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_GEOMETRY_IMAGE_H__ */
