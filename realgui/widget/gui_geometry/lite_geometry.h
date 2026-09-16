/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LITE_GEOMETRY_H
#define LITE_GEOMETRY_H

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#ifdef M_PI
#undef M_PI
#endif
#define M_PI 3.14159265358979323846f

#define LG_MIN(a, b) ((a) < (b) ? (a) : (b))
#define LG_MAX(a, b) ((a) > (b) ? (a) : (b))
#define LG_CLAMP(x, min_val, max_val) LG_MIN(LG_MAX(x, min_val), max_val)

typedef uint32_t PixelColor;

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
} Rect;

typedef enum
{
    PIXEL_FORMAT_RGB565,
    PIXEL_FORMAT_ARGB8888
} PixelFormat;

typedef enum
{
    GRADIENT_NONE = 0,
    GRADIENT_LINEAR,
    GRADIENT_RADIAL,
    GRADIENT_ANGULAR
} GradientType;

typedef struct
{
    float position;
    PixelColor color;
} GradientStop;

typedef struct
{
    GradientType type;
    GradientStop stops[8];
    int stop_count;
    float linear_x1;
    float linear_y1;
    float linear_x2;
    float linear_y2;
    float radial_cx;
    float radial_cy;
    float radial_r;
    float angular_cx;
    float angular_cy;
    float angular_start;
    float angular_end;
} Gradient;

typedef struct
{
    uint8_t *buffer;
    int width;
    int height;
    int stride;
    PixelFormat format;
    bool enable_aa;
    Rect clip_rect;
    bool enable_stroke_cap;
    Gradient *gradient;
} DrawContext;

PixelColor create_color_by_format(PixelFormat format, uint8_t r, uint8_t g,
                                  uint8_t b, uint8_t a);
PixelColor blend_colors_argb8888(PixelColor bg_color, PixelColor fg_color,
                                 uint8_t alpha);
PixelColor blend_colors_rgb565(PixelColor bg_color, PixelColor fg_color,
                               uint8_t alpha);

void init_draw_context(DrawContext *ctx, uint8_t *buffer, int width, int height,
                       PixelFormat format);
void fill_circle(DrawContext *ctx, float center_x, float center_y, float radius,
                 PixelColor fill_color);

void draw_arc_df_aa(DrawContext *ctx, float center_x, float center_y,
                    float radius, float line_width,
                    float start_angle, float end_angle,
                    PixelColor stroke_color);
void draw_arc_df_aa_gradient(DrawContext *ctx, float center_x, float center_y,
                             float radius, float line_width,
                             float start_angle, float end_angle,
                             Gradient *gradient);
void lg_arc_ink_bounds(float radius, float line_width,
                       float start_angle, float end_angle,
                       float *min_x, float *min_y, float *max_x, float *max_y);

void gradient_init(Gradient *grad, GradientType type);
void gradient_add_stop(Gradient *grad, float position, PixelColor color);
PixelColor gradient_get_color(Gradient *grad, float t);

#endif /* LITE_GEOMETRY_H */
