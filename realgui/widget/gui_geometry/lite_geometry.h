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

/* The collector type, for the run_sink field below.  This stays a plain
 * data-only header so the rasteriser can be exercised on a host. */
#include "gui_geometry_raster.h"

#ifdef M_PI
#undef M_PI
#endif
#define M_PI 3.14159265358979323846f

#define LG_MIN(a, b) ((a) < (b) ? (a) : (b))
#define LG_MAX(a, b) ((a) > (b) ? (a) : (b))
#define LG_CLAMP(x, min_val, max_val) LG_MIN(LG_MAX(x, min_val), max_val)

/**
 * Feather width of one stroke edge, in pixels.  Half a pixel: an edge inside a
 * pixel ramps it across 0..1 and no further.  Strokes narrower than
 * @c 2 * LG_ARC_AA_WIDTH have overlapping edges and take the thin profile
 * instead (see arc_is_thin()).
 */
#define LG_ARC_AA_WIDTH 0.5f

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
    PIXEL_FORMAT_ARGB8888,
    /**
     * One byte of coverage per pixel, no colour.
     *
     * A solid shape of a single colour needs only its coverage rasterised: the
     * colour travels beside the mask (the blit's fixed foreground), so one mask
     * serves every colour of the same geometry.  Coverage of a translucent
     * colour is folded into the mask, as gui_circle and gui_rect already do.
     */
    PIXEL_FORMAT_A8
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
    /**
     * Declares that this buffer holds one shape at a time.
     *
     * An arc's spans, AA edges and caps are one shape of one colour, so a pixel
     * that two of them cover is a raster overlap, not two objects stacking:
     * keeping the larger alpha deduplicates the passes, and it is what an A8
     * mask does regardless.  With the flag clear, overlapping writes composite
     * instead, which is right when the buffer collects several shapes -- the
     * particle widget's independent circles, or gui_arc_group's arcs.
     *
     * Set by the caller, because only the caller knows whether the buffer is
     * dedicated to one shape.  The rasterisers read it and never write it, so a
     * caller's setting survives every return path.  Defaults to false.
     */
    bool overlap_keeps_max_alpha;
    /**
     * Sparse destination, used instead of @c buffer when set.
     *
     * A shape that is empty nearly everywhere does not need a byte per pixel.
     * Pointing this at a collector makes the raster record only the covered
     * runs, which for a ring is a small fraction of its bounding box.  Every
     * sample then lands through gui_geometry_run_sink_put() and the bytes
     * written are the same ones @c buffer would have received, so the two
     * destinations stay interchangeable: the sparse one only leaves out the
     * zeros.
     *
     * Requires the raster to be the shape's sole output -- a collector holds
     * coverage and nothing else, so it cannot blend into what was there before.
     * NULL means write to @c buffer.  Defaults to NULL.
     */
    gui_geometry_run_sink_t *run_sink;
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

/* Update an opaque A8 arc in place. False leaves the buffer unchanged. */
bool lg_arc_update_a8(DrawContext *ctx, float center_x, float center_y,
                      float radius, float line_width, float start_angle,
                      float old_end_angle, float end_angle, PixelColor color);
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
