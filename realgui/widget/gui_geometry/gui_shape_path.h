/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __GUI_SHAPE_PATH_H__
#define __GUI_SHAPE_PATH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "guidef.h"
#include "gui_matrix.h"

typedef enum
{
    GUI_SHAPE_PATH_CIRCLE = 1,
    GUI_SHAPE_PATH_ROUNDED_RECT,
} gui_shape_path_type_t;

typedef struct
{
    uint32_t magic;
    uint32_t type;
    uint16_t width;
    uint16_t height;
    float radius;
    float inset;
    uint32_t samples;
} gui_shape_path_t;

typedef struct gui_shape_span_data gui_shape_span_data_t;

void gui_shape_path_init_circle(gui_shape_path_t *path, int radius, float inset);
void gui_shape_path_init_rounded_rect(gui_shape_path_t *path, int width, int height,
                                      int radius, float inset);

gui_shape_span_data_t *gui_shape_path_acquire(const gui_shape_path_t *path);
void gui_shape_path_release(const gui_shape_span_data_t *spans);

bool gui_shape_path_can_draw(const gui_matrix_t *matrix);
bool gui_shape_path_draw(const gui_shape_span_data_t *spans,
                         const gui_matrix_t *matrix,
                         gui_color_t color, uint8_t opacity,
                         gui_dispdev_t *dc);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_SHAPE_PATH_H__ */
