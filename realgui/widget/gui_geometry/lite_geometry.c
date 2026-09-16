/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <math.h>
#include <string.h>

#include "lite_geometry.h"
#include "lite_geometry_math.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

static inline int get_pixel_size(PixelFormat format)
{
    return (format == PIXEL_FORMAT_RGB565) ? 2 : 4;
}
static inline float smoothstep(float edge0, float edge1, float x)
{
    float t = LG_CLAMP((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

PixelColor create_color_by_format(PixelFormat format, uint8_t r, uint8_t g,
                                  uint8_t b, uint8_t a)
{
    if (format == PIXEL_FORMAT_RGB565)
    {
        return ((r & 0xF8u) << 8) | ((g & 0xFCu) << 3) | (b >> 3);
    }
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

PixelColor blend_colors_rgb565(PixelColor bg_color, PixelColor fg_color,
                               uint8_t alpha)
{
    if (alpha == 0) { return bg_color; }
    if (alpha == 255) { return fg_color; }

    alpha = (alpha + 4) >> 3;
    bg_color = (bg_color | (bg_color << 16)) & 0x07E0F81F;
    fg_color = (fg_color | (fg_color << 16)) & 0x07E0F81F;
    uint32_t result = ((((fg_color - bg_color) * alpha) >> 5) + bg_color) & 0x07E0F81F;
    return (uint16_t)((result >> 16) | result);
}

PixelColor blend_colors_argb8888(PixelColor bg_color, PixelColor fg_color,
                                 uint8_t alpha)
{
    if (alpha == 0) { return bg_color; }
    if (alpha == 255) { return fg_color; }

    uint8_t bg_a = (bg_color >> 24) & 0xFF;
    uint32_t fg_r = (fg_color >> 16) & 0xFF;
    uint32_t fg_g = (fg_color >> 8) & 0xFF;
    uint32_t fg_b = fg_color & 0xFF;
    if (bg_a == 0)
    {
        return ((uint32_t)alpha << 24) | (fg_r << 16) | (fg_g << 8) | fg_b;
    }

    uint32_t inv_alpha = 255 - alpha;
    uint32_t bg_r = (bg_color >> 16) & 0xFF;
    uint32_t bg_g = (bg_color >> 8) & 0xFF;
    uint32_t bg_b = bg_color & 0xFF;
    uint32_t out_r = ((bg_r * inv_alpha + fg_r * alpha) * 257) >> 16;
    uint32_t out_g = ((bg_g * inv_alpha + fg_g * alpha) * 257) >> 16;
    uint32_t out_b = ((bg_b * inv_alpha + fg_b * alpha) * 257) >> 16;
    uint32_t out_a = bg_a + (((255 - bg_a) * alpha * 257) >> 16);
    return (out_a << 24) | (out_r << 16) | (out_g << 8) | out_b;
}

void init_draw_context(DrawContext *ctx, uint8_t *buffer, int width, int height,
                       PixelFormat format)
{
    ctx->buffer = buffer;
    ctx->width = width;
    ctx->height = height;
    ctx->format = format;
    ctx->stride = width * get_pixel_size(format);
}

static void add_pixel_aa(DrawContext *ctx, int x, int y, PixelColor color,
                         float coverage)
{
    int pixel_size = get_pixel_size(ctx->format);
    int offset = (y * ctx->width + x) * pixel_size;

    if (ctx->format == PIXEL_FORMAT_RGB565)
    {
        uint16_t *pixel = (uint16_t *)(ctx->buffer + offset);
        if (coverage > 0.999f)
        {
            *pixel = (uint16_t)color;
        }
        else
        {
            *pixel = (uint16_t)blend_colors_rgb565(*pixel, color,
                                                   (uint8_t)(coverage * 255.0f));
        }
        return;
    }

    uint8_t source_alpha = (color >> 24) & 0xFF;
    if (source_alpha == 0) { return; }

    uint8_t alpha = (uint8_t)(source_alpha * coverage);
    if (alpha == 0) { return; }

    uint32_t *pixel = (uint32_t *)(ctx->buffer + offset);
    if (alpha == 255)
    {
        *pixel = color;
    }
    else
    {
        *pixel = blend_colors_argb8888(*pixel, color, alpha);
    }
}

static inline bool is_in_angle_range_cross(float dx, float dy,
                                           float start_cos, float start_sin,
                                           float end_cos, float end_sin,
                                           bool wide_arc);

void fill_circle(DrawContext *ctx, float center_x, float center_y, float radius,
                 PixelColor fill_color)
{
    if (ctx == NULL || radius <= 0.0f) { return; }

    int min_x = LG_MAX((int)floorf(center_x - radius - 1.0f), ctx->clip_rect.x);
    int min_y = LG_MAX((int)floorf(center_y - radius - 1.0f), ctx->clip_rect.y);
    int max_x = LG_MIN((int)ceilf(center_x + radius + 1.0f),
                       ctx->clip_rect.x + ctx->clip_rect.w);
    int max_y = LG_MIN((int)ceilf(center_y + radius + 1.0f),
                       ctx->clip_rect.y + ctx->clip_rect.h);

    for (int y = min_y; y < max_y; y++)
    {
        for (int x = min_x; x < max_x; x++)
        {
            float dx = (float)x + 0.5f - center_x;
            float dy = (float)y + 0.5f - center_y;
            float coverage = radius + 0.5f - sqrtf(dx * dx + dy * dy);
            if (!ctx->enable_aa) { coverage = (coverage >= 0.5f) ? 1.0f : 0.0f; }
            add_pixel_aa(ctx, x, y, fill_color, LG_CLAMP(coverage, 0.0f, 1.0f));
        }
    }
}
// ==================== Arc Helper Functions ====================

// Compute coverage for anti-aliasing using squared distances (avoids sqrt)
// Uses smoothstep-like falloff for consistency with arc_sdf
static inline float compute_coverage_fast(float dist_sq, float outer_sq, float inner_sq,
                                          float outer_aa_sq, float inner_aa_sq,
                                          float outer_solid_sq, float inner_solid_sq)
{
    (void)outer_sq;
    // Solid region - full coverage
    if (dist_sq <= outer_solid_sq && dist_sq >= inner_solid_sq)
    {
        return 1.0f;
    }

    // Outer edge anti-aliasing - use smoothstep for consistency
    if (dist_sq > outer_solid_sq && dist_sq <= outer_aa_sq)
    {
        float t = (dist_sq - outer_solid_sq) / (outer_aa_sq - outer_solid_sq);
        // Smoothstep: 3t^2 - 2t^3
        t = t * t * (3.0f - 2.0f * t);
        return 1.0f - t;
    }

    // Inner edge anti-aliasing - use smoothstep for consistency
    if (inner_sq > 0 && dist_sq >= inner_aa_sq && dist_sq < inner_solid_sq)
    {
        float t = (inner_solid_sq - dist_sq) / (inner_solid_sq - inner_aa_sq);
        // Smoothstep: 3t^2 - 2t^3
        t = t * t * (3.0f - 2.0f * t);
        return 1.0f - t;
    }

    return 0.0f;
}

/**
 * Draw a complete ring (360 degree arc) - optimized for full circles
 * Uses unified AA width and smoothstep for consistency
 */
static void draw_arc_as_ring_symmetric(DrawContext *ctx, float center_x, float center_y,
                                       float arc_radius, float line_width, PixelColor color)
{
    if (line_width <= 0) { return; }

    float outer_radius = arc_radius + line_width / 2;
    float inner_radius = fmaxf(arc_radius - line_width / 2, 0);
    float aa_width = 1.5f;  // Unified AA width

    float outer_sq = outer_radius * outer_radius;
    float inner_sq = inner_radius * inner_radius;
    float outer_aa_sq = (outer_radius + aa_width) * (outer_radius + aa_width);
    float inner_aa_sq = fmaxf((inner_radius - aa_width) * (inner_radius - aa_width), 0);

    float outer_solid_sq = (outer_radius - aa_width) * (outer_radius - aa_width);
    float inner_solid_sq = (inner_radius + aa_width) * (inner_radius + aa_width);

    int max_y = (int)ceilf(outer_radius + aa_width);

    for (int y = 0; y <= max_y; y++)
    {
        float y_sq = y * y;

        int x_start = y;
        int x_end = (int)ceilf(sqrtf(outer_aa_sq - y_sq));

        if (x_start > x_end) { continue; }

        int inner_x_start = 0;
        if (y <= inner_radius + aa_width)
        {
            inner_x_start = (int)ceilf(sqrtf(fmaxf(inner_aa_sq - y_sq, 0)));
            if (inner_x_start > x_start && inner_x_start <= x_end)
            {
                x_start = inner_x_start;
            }
        }

        for (int x = x_start; x <= x_end; x++)
        {
            float dist_sq = x * x + y_sq;

            // Compute coverage using smoothstep for consistency
            float coverage = compute_coverage_fast(dist_sq, outer_sq, inner_sq,
                                                   outer_aa_sq, inner_aa_sq,
                                                   outer_solid_sq, inner_solid_sq);
            if (coverage < 0.001f) { continue; }

            // Draw 8-way symmetric pixels
            int coords[8][2];
            int point_count = 0;

            if (x == 0 && y == 0)
            {
                coords[point_count][0] = (int)(center_x + 0); coords[point_count][1] = (int)(center_y + 0);
                point_count++;
            }
            else if (y == 0)
            {
                coords[point_count][0] = (int)(center_x + x); coords[point_count][1] = (int)(center_y + 0);
                point_count++;
                coords[point_count][0] = (int)(center_x - x); coords[point_count][1] = (int)(center_y + 0);
                point_count++;
                coords[point_count][0] = (int)(center_x + 0); coords[point_count][1] = (int)(center_y + x);
                point_count++;
                coords[point_count][0] = (int)(center_x + 0); coords[point_count][1] = (int)(center_y - x);
                point_count++;
            }
            else if (x == y)
            {
                coords[point_count][0] = (int)(center_x + x); coords[point_count][1] = (int)(center_y + y);
                point_count++;
                coords[point_count][0] = (int)(center_x - x); coords[point_count][1] = (int)(center_y + y);
                point_count++;
                coords[point_count][0] = (int)(center_x + x); coords[point_count][1] = (int)(center_y - y);
                point_count++;
                coords[point_count][0] = (int)(center_x - x); coords[point_count][1] = (int)(center_y - y);
                point_count++;
            }
            else
            {
                coords[point_count][0] = (int)(center_x + x); coords[point_count][1] = (int)(center_y + y);
                point_count++;
                coords[point_count][0] = (int)(center_x - x); coords[point_count][1] = (int)(center_y + y);
                point_count++;
                coords[point_count][0] = (int)(center_x + x); coords[point_count][1] = (int)(center_y - y);
                point_count++;
                coords[point_count][0] = (int)(center_x - x); coords[point_count][1] = (int)(center_y - y);
                point_count++;
                coords[point_count][0] = (int)(center_x + y); coords[point_count][1] = (int)(center_y + x);
                point_count++;
                coords[point_count][0] = (int)(center_x - y); coords[point_count][1] = (int)(center_y + x);
                point_count++;
                coords[point_count][0] = (int)(center_x + y); coords[point_count][1] = (int)(center_y - x);
                point_count++;
                coords[point_count][0] = (int)(center_x - y); coords[point_count][1] = (int)(center_y - x);
                point_count++;
            }

            for (int i = 0; i < point_count; i++)
            {
                int px = coords[i][0];
                int py = coords[i][1];
                if (px >= 0 && px < ctx->width && py >= 0 && py < ctx->height)
                {
                    add_pixel_aa(ctx, px, py, color, coverage);
                }
            }
        }
    }
}

static void fill_ring_span_argb8888(DrawContext *ctx, int y, int start_x, int end_x,
                                    PixelColor color)
{
    if (start_x > end_x) { return; }

    int clip_x1 = ctx->clip_rect.x;
    int clip_x2 = ctx->clip_rect.x + ctx->clip_rect.w - 1;
    if (start_x < clip_x1) { start_x = clip_x1; }
    if (end_x > clip_x2) { end_x = clip_x2; }
    if (start_x > end_x) { return; }

    uint8_t alpha = (uint8_t)(color >> 24);
    if (alpha != UINT8_MAX)
    {
        for (int x = start_x; x <= end_x; x++)
        {
            add_pixel_aa(ctx, x, y, color, 1.0f);
        }
        return;
    }

    uint32_t *pixel = (uint32_t *)(ctx->buffer + y * ctx->stride) + start_x;
    int count = end_x - start_x + 1;
    while (count >= 4)
    {
        pixel[0] = color;
        pixel[1] = color;
        pixel[2] = color;
        pixel[3] = color;
        pixel += 4;
        count -= 4;
    }
    while (count-- > 0)
    {
        *pixel++ = color;
    }
}

static void draw_arc_as_ring(DrawContext *ctx, float center_x, float center_y,
                             float arc_radius, float line_width, PixelColor color)
{
    if (ctx->format != PIXEL_FORMAT_ARGB8888)
    {
        draw_arc_as_ring_symmetric(ctx, center_x, center_y, arc_radius, line_width, color);
        return;
    }

    if (line_width <= 0.0f) { return; }

    float outer_radius = arc_radius + line_width / 2.0f;
    float inner_radius = fmaxf(arc_radius - line_width / 2.0f, 0.0f);
    float aa_width = 1.5f;
    float outer_aa_radius = outer_radius + aa_width;
    float outer_solid_radius = fmaxf(outer_radius - aa_width, 0.0f);
    float inner_aa_radius = fmaxf(inner_radius - aa_width, 0.0f);
    float inner_solid_radius = inner_radius + aa_width;

    float outer_sq = outer_radius * outer_radius;
    float inner_sq = inner_radius * inner_radius;
    float outer_aa_sq = outer_aa_radius * outer_aa_radius;
    float inner_aa_sq = inner_aa_radius * inner_aa_radius;
    float outer_solid_sq = outer_solid_radius * outer_solid_radius;
    float inner_solid_sq = inner_solid_radius * inner_solid_radius;

    int clip_x1 = ctx->clip_rect.x;
    int clip_x2 = ctx->clip_rect.x + ctx->clip_rect.w - 1;
    int min_y = LG_MAX((int)floorf(center_y - outer_aa_radius), ctx->clip_rect.y);
    int max_y = LG_MIN((int)ceilf(center_y + outer_aa_radius),
                       ctx->clip_rect.y + ctx->clip_rect.h - 1);

    for (int y = min_y; y <= max_y; y++)
    {
        float dy = (float)y + 0.5f - center_y;
        float dy_sq = dy * dy;
        if (dy_sq > outer_aa_sq) { continue; }

        float outer_aa_x = sqrtf(outer_aa_sq - dy_sq);
        float outer_solid_x = (dy_sq < outer_solid_sq) ?
                              sqrtf(outer_solid_sq - dy_sq) : 0.0f;
        float inner_solid_x = (inner_radius > 0.0f && dy_sq < inner_solid_sq) ?
                              sqrtf(inner_solid_sq - dy_sq) : 0.0f;

        int outer_left_solid = (int)ceilf(center_x - outer_solid_x - 0.5f);
        int outer_right_solid = (int)floorf(center_x + outer_solid_x - 0.5f);
        int inner_left_solid = (int)floorf(center_x - inner_solid_x - 0.5f);
        int inner_right_solid = (int)ceilf(center_x + inner_solid_x - 0.5f);

        if (inner_radius > 0.0f && inner_solid_x > 0.0f)
        {
            fill_ring_span_argb8888(ctx, y, outer_left_solid, inner_left_solid, color);
            fill_ring_span_argb8888(ctx, y, inner_right_solid, outer_right_solid, color);
        }
        else
        {
            fill_ring_span_argb8888(ctx, y, outer_left_solid, outer_right_solid, color);
        }

        int outer_left_aa = (int)ceilf(center_x - outer_aa_x - 0.5f);
        int outer_right_aa = (int)floorf(center_x + outer_aa_x - 0.5f);
        int outer_left_end = LG_MIN(outer_left_solid - 1, clip_x2);
        for (int x = LG_MAX(outer_left_aa, clip_x1); x <= outer_left_end; x++)
        {
            float dx = (float)x + 0.5f - center_x;
            float coverage = compute_coverage_fast(dx * dx + dy_sq, outer_sq, inner_sq,
                                                   outer_aa_sq, inner_aa_sq,
                                                   outer_solid_sq, inner_solid_sq);
            if (coverage > 0.001f) { add_pixel_aa(ctx, x, y, color, coverage); }
        }
        int outer_right_start = LG_MAX(outer_right_solid + 1, clip_x1);
        for (int x = outer_right_start; x <= LG_MIN(outer_right_aa, clip_x2); x++)
        {
            float dx = (float)x + 0.5f - center_x;
            float coverage = compute_coverage_fast(dx * dx + dy_sq, outer_sq, inner_sq,
                                                   outer_aa_sq, inner_aa_sq,
                                                   outer_solid_sq, inner_solid_sq);
            if (coverage > 0.001f) { add_pixel_aa(ctx, x, y, color, coverage); }
        }

        if (inner_radius > 0.0f && dy_sq < inner_solid_sq)
        {
            float inner_aa_x = (dy_sq < inner_aa_sq) ? sqrtf(inner_aa_sq - dy_sq) : 0.0f;
            int inner_left_aa = (int)floorf(center_x - inner_aa_x - 0.5f);
            int inner_right_aa = (int)ceilf(center_x + inner_aa_x - 0.5f);

            int inner_left_start = LG_MAX(inner_left_solid + 1, clip_x1);
            for (int x = inner_left_start; x <= LG_MIN(inner_left_aa, clip_x2); x++)
            {
                float dx = (float)x + 0.5f - center_x;
                float coverage = compute_coverage_fast(dx * dx + dy_sq, outer_sq, inner_sq,
                                                       outer_aa_sq, inner_aa_sq,
                                                       outer_solid_sq, inner_solid_sq);
                if (coverage > 0.001f) { add_pixel_aa(ctx, x, y, color, coverage); }
            }
            int inner_right_end = LG_MIN(inner_right_solid - 1, clip_x2);
            for (int x = LG_MAX(inner_right_aa, clip_x1); x <= inner_right_end; x++)
            {
                float dx = (float)x + 0.5f - center_x;
                float coverage = compute_coverage_fast(dx * dx + dy_sq, outer_sq, inner_sq,
                                                       outer_aa_sq, inner_aa_sq,
                                                       outer_solid_sq, inner_solid_sq);
                if (coverage > 0.001f) { add_pixel_aa(ctx, x, y, color, coverage); }
            }
        }
    }
}

static void fill_arc_span_argb8888(DrawContext *ctx, int y, int start_x, int end_x,
                                   float center_x, float center_y,
                                   float start_cos, float start_sin,
                                   float end_cos, float end_sin,
                                   bool wide_arc, PixelColor color)
{
    int run_start = -1;

    for (int x = start_x; x <= end_x; x++)
    {
        float dx = (float)x + 0.5f - center_x;
        float dy = (float)y + 0.5f - center_y;
        bool in_angle = is_in_angle_range_cross(dx, dy,
                                                start_cos, start_sin,
                                                end_cos, end_sin,
                                                wide_arc);
        if (in_angle)
        {
            if (run_start < 0) { run_start = x; }
        }
        else if (run_start >= 0)
        {
            fill_ring_span_argb8888(ctx, y, run_start, x - 1, color);
            run_start = -1;
        }
    }

    if (run_start >= 0)
    {
        fill_ring_span_argb8888(ctx, y, run_start, end_x, color);
    }
}

static void draw_arc_cap_argb8888(DrawContext *ctx, float cap_x, float cap_y,
                                  float center_x, float center_y,
                                  float start_cos, float start_sin,
                                  float end_cos, float end_sin,
                                  bool wide_arc, float cap_radius,
                                  float aa_width, PixelColor color)
{
    int min_x = LG_MAX((int)floorf(cap_x - cap_radius - aa_width), ctx->clip_rect.x);
    int max_x = LG_MIN((int)ceilf(cap_x + cap_radius + aa_width),
                       ctx->clip_rect.x + ctx->clip_rect.w - 1);
    int min_y = LG_MAX((int)floorf(cap_y - cap_radius - aa_width), ctx->clip_rect.y);
    int max_y = LG_MIN((int)ceilf(cap_y + cap_radius + aa_width),
                       ctx->clip_rect.y + ctx->clip_rect.h - 1);

    for (int y = min_y; y <= max_y; y++)
    {
        float dy = (float)y + 0.5f - center_y;
        for (int x = min_x; x <= max_x; x++)
        {
            float dx = (float)x + 0.5f - center_x;
            if (is_in_angle_range_cross(dx, dy,
                                        start_cos, start_sin,
                                        end_cos, end_sin,
                                        wide_arc))
            {
                continue;
            }

            float cap_dx = (float)x + 0.5f - cap_x;
            float cap_dy = (float)y + 0.5f - cap_y;
            float distance = sqrtf(cap_dx * cap_dx + cap_dy * cap_dy) - cap_radius;
            float coverage = 1.0f - smoothstep(-aa_width, aa_width, distance);
            if (coverage > 0.001f)
            {
                add_pixel_aa(ctx, x, y, color, coverage);
            }
        }
    }
}

static bool draw_arc_scanline_argb8888(DrawContext *ctx, float center_x, float center_y,
                                       float radius, float line_width,
                                       float start_angle, float end_angle,
                                       PixelColor color)
{
    if (ctx->format != PIXEL_FORMAT_ARGB8888 || (uint8_t)(color >> 24) != UINT8_MAX)
    {
        return false;
    }

    float angle_span = end_angle - start_angle;
    if (angle_span <= 0.0f) { angle_span += 360.0f; }
    if (angle_span <= 0.0f || angle_span >= 359.9f)
    {
        return false;
    }

    float cap_radius = line_width / 2.0f;
    if (radius * angle_span * M_PI / 180.0f <= line_width * 1.5f)
    {
        return false;
    }

    float aa_width = 1.5f;
    float outer_radius = radius + cap_radius;
    float inner_radius = fmaxf(radius - cap_radius, 0.0f);
    float outer_aa_radius = outer_radius + aa_width;
    float outer_solid_radius = fmaxf(outer_radius - aa_width, 0.0f);
    float inner_aa_radius = fmaxf(inner_radius - aa_width, 0.0f);
    float inner_solid_radius = inner_radius + aa_width;

    float outer_sq = outer_radius * outer_radius;
    float inner_sq = inner_radius * inner_radius;
    float outer_aa_sq = outer_aa_radius * outer_aa_radius;
    float inner_aa_sq = inner_aa_radius * inner_aa_radius;
    float outer_solid_sq = outer_solid_radius * outer_solid_radius;
    float inner_solid_sq = inner_solid_radius * inner_solid_radius;

    float start_rad = start_angle * M_PI / 180.0f;
    float end_rad = end_angle * M_PI / 180.0f;
    float start_cos = cosf(start_rad);
    float start_sin = sinf(start_rad);
    float end_cos = cosf(end_rad);
    float end_sin = sinf(end_rad);
    bool wide_arc = angle_span > 180.0f;

    float start_cap_x = center_x + radius * start_cos;
    float start_cap_y = center_y + radius * start_sin;
    float end_cap_x = center_x + radius * end_cos;
    float end_cap_y = center_y + radius * end_sin;

    int clip_x1 = ctx->clip_rect.x;
    int clip_x2 = ctx->clip_rect.x + ctx->clip_rect.w - 1;
    int min_y = LG_MAX((int)floorf(center_y - outer_aa_radius), ctx->clip_rect.y);
    int max_y = LG_MIN((int)ceilf(center_y + outer_aa_radius),
                       ctx->clip_rect.y + ctx->clip_rect.h - 1);

    for (int y = min_y; y <= max_y; y++)
    {
        float dy = (float)y + 0.5f - center_y;
        float dy_sq = dy * dy;
        if (dy_sq > outer_aa_sq) { continue; }

        float outer_aa_x = sqrtf(outer_aa_sq - dy_sq);
        float outer_solid_x = (dy_sq < outer_solid_sq) ?
                              sqrtf(outer_solid_sq - dy_sq) : 0.0f;
        float inner_solid_x = (inner_radius > 0.0f && dy_sq < inner_solid_sq) ?
                              sqrtf(inner_solid_sq - dy_sq) : 0.0f;

        int outer_left_solid = (int)ceilf(center_x - outer_solid_x - 0.5f);
        int outer_right_solid = (int)floorf(center_x + outer_solid_x - 0.5f);
        int inner_left_solid = (int)floorf(center_x - inner_solid_x - 0.5f);
        int inner_right_solid = (int)ceilf(center_x + inner_solid_x - 0.5f);

        if (inner_radius > 0.0f && inner_solid_x > 0.0f)
        {
            fill_arc_span_argb8888(ctx, y, outer_left_solid, inner_left_solid,
                                   center_x, center_y, start_cos, start_sin,
                                   end_cos, end_sin, wide_arc, color);
            fill_arc_span_argb8888(ctx, y, inner_right_solid, outer_right_solid,
                                   center_x, center_y, start_cos, start_sin,
                                   end_cos, end_sin, wide_arc, color);
        }
        else
        {
            fill_arc_span_argb8888(ctx, y, outer_left_solid, outer_right_solid,
                                   center_x, center_y, start_cos, start_sin,
                                   end_cos, end_sin, wide_arc, color);
        }

        int outer_left_aa = (int)ceilf(center_x - outer_aa_x - 0.5f);
        int outer_left_end = LG_MIN(outer_left_solid - 1, clip_x2);
        for (int x = LG_MAX(outer_left_aa, clip_x1); x <= outer_left_end; x++)
        {
            float dx = (float)x + 0.5f - center_x;
            if (!is_in_angle_range_cross(dx, dy, start_cos, start_sin,
                                         end_cos, end_sin, wide_arc))
            {
                continue;
            }
            float coverage = compute_coverage_fast(dx * dx + dy_sq, outer_sq, inner_sq,
                                                   outer_aa_sq, inner_aa_sq,
                                                   outer_solid_sq, inner_solid_sq);
            if (coverage > 0.001f) { add_pixel_aa(ctx, x, y, color, coverage); }
        }

        int outer_right_aa = (int)floorf(center_x + outer_aa_x - 0.5f);
        int outer_right_start = LG_MAX(outer_right_solid + 1, clip_x1);
        for (int x = outer_right_start; x <= LG_MIN(outer_right_aa, clip_x2); x++)
        {
            float dx = (float)x + 0.5f - center_x;
            if (!is_in_angle_range_cross(dx, dy, start_cos, start_sin,
                                         end_cos, end_sin, wide_arc))
            {
                continue;
            }
            float coverage = compute_coverage_fast(dx * dx + dy_sq, outer_sq, inner_sq,
                                                   outer_aa_sq, inner_aa_sq,
                                                   outer_solid_sq, inner_solid_sq);
            if (coverage > 0.001f) { add_pixel_aa(ctx, x, y, color, coverage); }
        }

        if (inner_radius > 0.0f && dy_sq < inner_solid_sq)
        {
            float inner_aa_x = (dy_sq < inner_aa_sq) ? sqrtf(inner_aa_sq - dy_sq) : 0.0f;
            int inner_left_aa = (int)floorf(center_x - inner_aa_x - 0.5f);
            int inner_left_start = LG_MAX(inner_left_solid + 1, clip_x1);
            for (int x = inner_left_start; x <= LG_MIN(inner_left_aa, clip_x2); x++)
            {
                float dx = (float)x + 0.5f - center_x;
                if (!is_in_angle_range_cross(dx, dy, start_cos, start_sin,
                                             end_cos, end_sin, wide_arc))
                {
                    continue;
                }
                float coverage = compute_coverage_fast(dx * dx + dy_sq, outer_sq, inner_sq,
                                                       outer_aa_sq, inner_aa_sq,
                                                       outer_solid_sq, inner_solid_sq);
                if (coverage > 0.001f) { add_pixel_aa(ctx, x, y, color, coverage); }
            }

            int inner_right_aa = (int)ceilf(center_x + inner_aa_x - 0.5f);
            int inner_right_end = LG_MIN(inner_right_solid - 1, clip_x2);
            for (int x = LG_MAX(inner_right_aa, clip_x1); x <= inner_right_end; x++)
            {
                float dx = (float)x + 0.5f - center_x;
                if (!is_in_angle_range_cross(dx, dy, start_cos, start_sin,
                                             end_cos, end_sin, wide_arc))
                {
                    continue;
                }
                float coverage = compute_coverage_fast(dx * dx + dy_sq, outer_sq, inner_sq,
                                                       outer_aa_sq, inner_aa_sq,
                                                       outer_solid_sq, inner_solid_sq);
                if (coverage > 0.001f) { add_pixel_aa(ctx, x, y, color, coverage); }
            }
        }
    }

    draw_arc_cap_argb8888(ctx, start_cap_x, start_cap_y,
                          center_x, center_y, start_cos, start_sin,
                          end_cos, end_sin, wide_arc, cap_radius, aa_width, color);
    draw_arc_cap_argb8888(ctx, end_cap_x, end_cap_y,
                          center_x, center_y, start_cos, start_sin,
                          end_cos, end_sin, wide_arc, cap_radius, aa_width, color);

    return true;
}
// ==================== Optimized Arc Drawing ====================

// Normalize angle to [0, 360)
static inline float normalize_angle_arc(float angle)
{
    while (angle < 0) { angle += 360.0f; }
    while (angle > 360.0f) { angle -= 360.0f; }
    return angle;
}

// Check if angle is in arc range (handles cross-zero correctly)
static inline bool is_angle_in_arc_range(float angle, float start, float end)
{
    angle = normalize_angle_arc(angle);
    start = normalize_angle_arc(start);
    end = normalize_angle_arc(end);

    if (fabsf(end - start) >= 359.9f)
    {
        return true; // Full circle
    }

    if (start <= end)
    {
        return (angle >= start && angle <= end);
    }
    else
    {
        // Cross zero: e.g., start=350, end=10
        return (angle >= start || angle <= end);
    }
}

// Compute signed distance from point to arc (negative = inside)
// This provides seamless cap-to-arc transition
static inline float arc_sdf(float px, float py, float cx, float cy,
                            float radius, float line_width,
                            float start_angle, float end_angle)
{
    float dx = px - cx;
    float dy = py - cy;
    float dist = sqrtf(dx * dx + dy * dy);

    // Distance to ring
    float inner_r = radius - line_width / 2.0f;
    float outer_r = radius + line_width / 2.0f;
    float ring_dist;

    if (dist < inner_r)
    {
        ring_dist = inner_r - dist; // Inside inner circle
    }
    else if (dist > outer_r)
    {
        ring_dist = dist - outer_r; // Outside outer circle
    }
    else
    {
        ring_dist = -fminf(dist - inner_r, outer_r - dist); // Inside ring (negative)
    }

    // Check angle constraint
    if (fabsf(end_angle - start_angle) < 359.9f)
    {
        float angle = atan2f(dy, dx) * 180.0f / M_PI;
        if (!is_angle_in_arc_range(angle, start_angle, end_angle))
        {
            // Outside angle range - compute distance to arc endpoints (caps)
            float start_rad = start_angle * M_PI / 180.0f;
            float end_rad = end_angle * M_PI / 180.0f;

            float start_x = cx + radius * cosf(start_rad);
            float start_y = cy + radius * sinf(start_rad);
            float end_x = cx + radius * cosf(end_rad);
            float end_y = cy + radius * sinf(end_rad);

            float dist_to_start = sqrtf((px - start_x) * (px - start_x) + (py - start_y) * (py - start_y));
            float dist_to_end = sqrtf((px - end_x) * (px - end_x) + (py - end_y) * (py - end_y));

            // Cap SDF: distance to nearest cap center minus cap radius
            float cap_dist = fminf(dist_to_start, dist_to_end) - line_width / 2.0f;
            // Return the maximum of ring_dist and cap_dist for seamless blending
            return fmaxf(ring_dist, cap_dist);
        }
    }

    return ring_dist;
}

// ==================== Optimized Arc Drawing with Quadrant-based approach ====================

static inline bool is_in_angle_range_cross(float dx, float dy,
                                           float start_cos, float start_sin,
                                           float end_cos, float end_sin,
                                           bool wide_arc)
{
    float from_start = start_cos * dy - start_sin * dx;
    float to_end = dx * end_sin - dy * end_cos;
    return wide_arc ? (from_start >= 0.0f || to_end >= 0.0f) :
           (from_start >= 0.0f && to_end >= 0.0f);
}

/** Ink margin beyond the outer radius: the 1.5px AA band plus rounding slack. */
#define LG_ARC_AA_MARGIN 3.0f

/** Sweep tolerance for treating an arc as a complete ring. */
#define LG_ARC_FULL_EPS 0.01f

/**
 * Angular slack the rasterisers can ink past the requested end angle.
 *
 * draw_arc_df_aa() and draw_arc_df_aa_gradient() classify a pixel as inside or
 * outside the sweep from the precomputed start/end rays.  Pad the sweep by
 * that much so the allocation covers anti-aliased endpoint pixels.
 */
#define LG_ARC_ANGLE_GUARD 2.0f

/**
 * Axis-aligned bounds of the ink an arc lays down, relative to its centre.
 *
 * A 30-degree arc only inks a small sliver of its bounding square, so callers
 * that size a pixel buffer per arc want the sweep bounded, not the whole
 * circle.  The centreline arc is bounded first -- its two endpoints, plus
 * whichever of the four axis extremes the sweep actually crosses -- and then
 * inflated by half the line width.  That inflation also covers the round caps,
 * since they are centred on the centreline, and by the anti-aliasing margin the
 * draw_arc_* rasterisers feather over.
 *
 * Y grows downward, matching the rasterisers' screen-space convention.
 */
void lg_arc_ink_bounds(float radius, float line_width,
                       float start_angle, float end_angle,
                       float *min_x, float *min_y, float *max_x, float *max_y)
{
    float r = radius;
    float grow = line_width / 2.0f + LG_ARC_AA_MARGIN;
    float raw_span = end_angle - start_angle;

    /* Match draw_arc_df_aa()'s full-circle test: a 360-degree sweep normalizes
     * to a zero span, which it then treats as the complete ring.  The gradient
     * rasteriser also accepts near-zero and near-360 spans as a full circle, so
     * keep the allocation conservative for either renderer. */
    float sweep = fmodf(raw_span, 360.0f);
    if (sweep <= 0.0f) { sweep += 360.0f; }

    if (r <= 0.0f || line_width <= 0.0f)
    {
        *min_x = *min_y = *max_x = *max_y = 0.0f;
        return;
    }

    if (fabsf(raw_span) < 0.1f || fabsf(raw_span) >= 359.9f ||
        sweep + 2.0f * LG_ARC_ANGLE_GUARD >= 360.0f - LG_ARC_FULL_EPS)
    {
        *min_x = -r - grow; *max_x = r + grow;
        *min_y = -r - grow; *max_y = r + grow;
        return;
    }

    float a0 = fmodf(start_angle, 360.0f);
    if (a0 < 0.0f) { a0 += 360.0f; }
    a0 -= LG_ARC_ANGLE_GUARD;
    sweep += 2.0f * LG_ARC_ANGLE_GUARD;
    float a1 = a0 + sweep;                  /* left unwrapped, may exceed 360 */

    float c0 = cosf(a0 * M_PI / 180.0f);
    float s0 = sinf(a0 * M_PI / 180.0f);
    float c1 = cosf(a1 * M_PI / 180.0f);
    float s1 = sinf(a1 * M_PI / 180.0f);

    float lo_x = LG_MIN(c0, c1) * r, hi_x = LG_MAX(c0, c1) * r;
    float lo_y = LG_MIN(s0, s1) * r, hi_y = LG_MAX(s0, s1) * r;

    /* An axis extreme only counts when the sweep actually passes over it. */
    for (int k = 0; k < 4; k++)
    {
        float ak = 90.0f * (float)k;
        if (!((ak >= a0 && ak <= a1) || (ak + 360.0f >= a0 && ak + 360.0f <= a1)))
        {
            continue;
        }
        switch (k)
        {
        case 0: hi_x =  r; break;           /* 0 deg   -> rightmost */
        case 1: hi_y =  r; break;           /* 90 deg  -> bottommost */
        case 2: lo_x = -r; break;           /* 180 deg -> leftmost */
        default: lo_y = -r; break;          /* 270 deg -> topmost */
        }
    }

    *min_x = lo_x - grow; *max_x = hi_x + grow;
    *min_y = lo_y - grow; *max_y = hi_y + grow;
}

/**
 * Draw arc with anti-aliasing - hybrid approach for performance + quality
 * Key features:
 * 1. Fast path for arc body using squared distance
 * 2. arc_sdf only for cap regions (seamless transition)
 * 3. Smooth anti-aliasing using smoothstep
 */
void draw_arc_df_aa(DrawContext *ctx, float center_x, float center_y,
                    float radius, float line_width,
                    float start_angle, float end_angle,
                    PixelColor stroke_color)
{
    if (radius <= 0 || line_width <= 0) { return; }

    // Normalize angles
    start_angle = normalize_angle_arc(start_angle);
    end_angle = normalize_angle_arc(end_angle);

    // Calculate angle span
    float angle_span = end_angle - start_angle;
    if (angle_span <= 0) { angle_span += 360.0f; }

    // Check for full circle
    const float EPSILON = 0.001f;
    bool is_full_circle = (fabsf(angle_span - 360.0f) < EPSILON);

    // Use optimized ring drawing for full circles
    if (is_full_circle)
    {
        draw_arc_as_ring(ctx, center_x, center_y, radius, line_width, stroke_color);
        return;
    }

    if (draw_arc_scanline_argb8888(ctx, center_x, center_y, radius, line_width,
                                   start_angle, end_angle, stroke_color))
    {
        return;
    }

    // Pre-calculate radii and squared values
    float outer_radius = radius + line_width / 2.0f;
    float inner_radius = fmaxf(radius - line_width / 2.0f, 0);
    float aa_width = 1.5f;  // Unified AA width for both arc body and caps

    float outer_sq = outer_radius * outer_radius;
    float inner_sq = inner_radius * inner_radius;
    float outer_aa_sq = (outer_radius + aa_width) * (outer_radius + aa_width);
    float inner_aa_sq = fmaxf((inner_radius - aa_width) * (inner_radius - aa_width), 0);

    // Solid region boundaries (no AA needed)
    float outer_solid = outer_radius - aa_width;
    float inner_solid = inner_radius + aa_width;
    float outer_solid_sq = outer_solid * outer_solid;
    float inner_solid_sq = inner_solid * inner_solid;

    // Convert angles to integer degrees for fast comparison
    uint16_t start_angle_deg = (uint16_t)start_angle;
    uint16_t end_angle_deg = (uint16_t)end_angle;
    while (start_angle_deg >= 360) { start_angle_deg -= 360; }
    while (end_angle_deg >= 360) { end_angle_deg -= 360; }

    // Pre-calculate cap positions
    float start_rad = start_angle * M_PI / 180.0f;
    float end_rad = end_angle * M_PI / 180.0f;
    float start_cos = cosf(start_rad);
    float start_sin = sinf(start_rad);
    float end_cos = cosf(end_rad);
    float end_sin = sinf(end_rad);
    float start_cap_x = center_x + radius * start_cos;
    float start_cap_y = center_y + radius * start_sin;
    float end_cap_x = center_x + radius * end_cos;
    float end_cap_y = center_y + radius * end_sin;
    bool wide_arc = angle_span > 180.0f;

    float cap_r = line_width / 2.0f;
    float cap_check_sq = (cap_r + aa_width + 2.0f) * (cap_r + aa_width + 2.0f);

    // Determine which quadrants contain the arc
    bool quadrants[4] = {false, false, false, false};
    uint16_t start_quadrant = start_angle_deg / 90;
    uint16_t end_quadrant = end_angle_deg / 90;

    if (end_angle_deg >= start_angle_deg)
    {
        for (int q = start_quadrant; q <= end_quadrant; q++)
        {
            quadrants[q % 4] = true;
        }
    }
    else
    {
        // Cross zero
        for (int q = start_quadrant; q < 4; q++)
        {
            quadrants[q] = true;
        }
        for (int q = 0; q <= end_quadrant; q++)
        {
            quadrants[q] = true;
        }
    }

    // Determine which quadrant should draw each cap (to avoid double-drawing)
    // Start cap quadrant - based on start angle
    int start_cap_quad = start_angle_deg / 90;

    // End cap quadrant - based on end angle
    int end_cap_quad = end_angle_deg / 90;

    // Extract color components
    uint8_t color_a = (stroke_color >> 24) & 0xFF;
    uint8_t color_r = (stroke_color >> 16) & 0xFF;
    uint8_t color_g = (stroke_color >> 8) & 0xFF;
    uint8_t color_b = stroke_color & 0xFF;

    if (color_a == 0) { return; }

    // Direct pixel access
    int pixel_size = (ctx->format == PIXEL_FORMAT_RGB565) ? 2 : 4;
    int stride = ctx->width * pixel_size;

    // Process each quadrant with strict non-overlapping boundaries
    for (int quad = 0; quad < 4; quad++)
    {
        if (!quadrants[quad]) { continue; }

        // Calculate quadrant bounds with strict boundaries to avoid overlap
        // Use center_x/center_y as exclusive boundaries between quadrants
        int qx_start, qx_end, qy_start, qy_end;
        float cap_expand = cap_r + aa_width + 1.0f;
        int center_x_int = (int)center_x;
        int center_y_int = (int)center_y;

        // Check if start/end cap is in this quadrant and needs boundary extension
        bool has_start_cap = (start_cap_quad == quad);
        bool has_end_cap = (end_cap_quad == quad);

        switch (quad)
        {
        case 0: // Q0: x >= center, y >= center (angles 0-90)
            qx_start = center_x_int;
            qx_end = (int)(center_x + outer_radius + aa_width + cap_expand);
            qy_start = center_y_int;
            qy_end = (int)(center_y + outer_radius + aa_width + cap_expand);
            // Extend boundaries only if cap belongs to this quadrant (by angle, not position)
            if (has_start_cap)
            {
                qx_start = LG_MIN(qx_start, (int)(start_cap_x - cap_expand));
                qy_start = LG_MIN(qy_start, (int)(start_cap_y - cap_expand));
            }
            if (has_end_cap)
            {
                qx_start = LG_MIN(qx_start, (int)(end_cap_x - cap_expand));
                qy_start = LG_MIN(qy_start, (int)(end_cap_y - cap_expand));
            }
            break;
        case 1: // Q1: x < center, y >= center (angles 90-180)
            qx_start = (int)(center_x - outer_radius - aa_width - cap_expand);
            qx_end = center_x_int - 1;
            qy_start = center_y_int;
            qy_end = (int)(center_y + outer_radius + aa_width + cap_expand);
            // Extend boundaries only if cap belongs to this quadrant
            if (has_start_cap)
            {
                qx_end = LG_MAX(qx_end, (int)(start_cap_x + cap_expand));
                qy_start = LG_MIN(qy_start, (int)(start_cap_y - cap_expand));
            }
            if (has_end_cap)
            {
                qx_end = LG_MAX(qx_end, (int)(end_cap_x + cap_expand));
                qy_start = LG_MIN(qy_start, (int)(end_cap_y - cap_expand));
            }
            break;
        case 2: // Q2: x < center, y < center (angles 180-270)
            qx_start = (int)(center_x - outer_radius - aa_width - cap_expand);
            qx_end = center_x_int - 1;
            qy_start = (int)(center_y - outer_radius - aa_width - cap_expand);
            qy_end = center_y_int - 1;
            // Extend boundaries only if cap belongs to this quadrant
            if (has_start_cap)
            {
                qx_end = LG_MAX(qx_end, (int)(start_cap_x + cap_expand));
                qy_end = LG_MAX(qy_end, (int)(start_cap_y + cap_expand));
            }
            if (has_end_cap)
            {
                qx_end = LG_MAX(qx_end, (int)(end_cap_x + cap_expand));
                qy_end = LG_MAX(qy_end, (int)(end_cap_y + cap_expand));
            }
            break;
        case 3: // Q3: x >= center, y < center (angles 270-360)
            qx_start = center_x_int;
            qx_end = (int)(center_x + outer_radius + aa_width + cap_expand);
            qy_start = (int)(center_y - outer_radius - aa_width - cap_expand);
            qy_end = center_y_int - 1;
            // Extend boundaries only if cap belongs to this quadrant
            if (has_start_cap)
            {
                qx_start = LG_MIN(qx_start, (int)(start_cap_x - cap_expand));
                qy_end = LG_MAX(qy_end, (int)(start_cap_y + cap_expand));
            }
            if (has_end_cap)
            {
                qx_start = LG_MIN(qx_start, (int)(end_cap_x - cap_expand));
                qy_end = LG_MAX(qy_end, (int)(end_cap_y + cap_expand));
            }
            break;
        }

        // Clip to context bounds
        qx_start = LG_MAX(qx_start, ctx->clip_rect.x);
        qx_end = LG_MIN(qx_end, ctx->clip_rect.x + ctx->clip_rect.w - 1);
        qy_start = LG_MAX(qy_start, ctx->clip_rect.y);
        qy_end = LG_MIN(qy_end, ctx->clip_rect.y + ctx->clip_rect.h - 1);

        if (qx_start > qx_end || qy_start > qy_end) { continue; }

        // Process rows in this quadrant
        for (int py = qy_start; py <= qy_end; py++)
        {
            float dy = (py + 0.5f) - center_y;
            float dy_sq = dy * dy;
            uint8_t *row = ctx->buffer + py * stride;

            for (int px = qx_start; px <= qx_end; px++)
            {
                float dx = (px + 0.5f) - center_x;
                float dist_sq = dx * dx + dy_sq;

                // Quick rejection: outside AA region
                if (dist_sq > outer_aa_sq) { continue; }
                if (inner_sq > 0 && dist_sq < inner_aa_sq) { continue; }

                float coverage;

                // Check if in angle range
                bool in_angle = is_in_angle_range_cross(dx, dy,
                                                        start_cos, start_sin,
                                                        end_cos, end_sin,
                                                        wide_arc);

                if (!in_angle)
                {
                    // Outside angle range - check if in cap region
                    float sample_x = px + 0.5f;
                    float sample_y = py + 0.5f;
                    float dx_start = sample_x - start_cap_x;
                    float dy_start = sample_y - start_cap_y;
                    float dx_end = sample_x - end_cap_x;
                    float dy_end = sample_y - end_cap_y;

                    float dist_start_sq = dx_start * dx_start + dy_start * dy_start;
                    float dist_end_sq = dx_end * dx_end + dy_end * dy_end;

                    // Quick rejection for cap region
                    if (dist_start_sq > cap_check_sq && dist_end_sq > cap_check_sq)
                    {
                        continue;
                    }

                    // Use arc_sdf for seamless cap transition
                    float sdf = arc_sdf(sample_x, sample_y, center_x, center_y,
                                        radius, line_width, start_angle, end_angle);
                    coverage = 1.0f - smoothstep(-aa_width, aa_width, sdf);
                }
                else
                {
                    // In angle range - use fast coverage calculation
                    coverage = compute_coverage_fast(dist_sq, outer_sq, inner_sq,
                                                     outer_aa_sq, inner_aa_sq,
                                                     outer_solid_sq, inner_solid_sq);
                }

                if (coverage < 0.001f) { continue; }

                // Compute final alpha
                uint8_t final_alpha = (uint8_t)(color_a * coverage);
                if (final_alpha == 0) { continue; }

                // Write pixel
                int byte_offset = px * pixel_size;

                if (ctx->format == PIXEL_FORMAT_ARGB8888)
                {
                    uint32_t *pixel_ptr = (uint32_t *)(row + byte_offset);

                    if (final_alpha == 255)
                    {
                        *pixel_ptr = stroke_color;
                    }
                    else
                    {
                        uint32_t bg = *pixel_ptr;
                        uint8_t bg_a = (bg >> 24) & 0xFF;

                        if (bg_a == 0)
                        {
                            *pixel_ptr = (final_alpha << 24) | (color_r << 16) | (color_g << 8) | color_b;
                        }
                        else
                        {
                            // Check if background is same color (to avoid double-draw artifacts)
                            uint8_t bg_r = (bg >> 16) & 0xFF;
                            uint8_t bg_g = (bg >> 8) & 0xFF;
                            uint8_t bg_b = bg & 0xFF;

                            // If same color, use MAX alpha instead of ADD to prevent transparency stacking
                            if (bg_r == color_r && bg_g == color_g && bg_b == color_b)
                            {
                                uint8_t max_alpha = (final_alpha > bg_a) ? final_alpha : bg_a;
                                *pixel_ptr = (max_alpha << 24) | (color_r << 16) | (color_g << 8) | color_b;
                            }
                            else
                            {
                                // Different color, use standard alpha blending
                                uint16_t inv_alpha = 256 - final_alpha;

                                uint8_t out_r = ((bg_r * inv_alpha + color_r * final_alpha) >> 8) & 0xFF;
                                uint8_t out_g = ((bg_g * inv_alpha + color_g * final_alpha) >> 8) & 0xFF;
                                uint8_t out_b = ((bg_b * inv_alpha + color_b * final_alpha) >> 8) & 0xFF;
                                uint8_t out_a = bg_a + (((255 - bg_a) * final_alpha) >> 8);

                                *pixel_ptr = (out_a << 24) | (out_r << 16) | (out_g << 8) | out_b;
                            }
                        }
                    }
                }
                else if (ctx->format == PIXEL_FORMAT_RGB565)
                {
                    uint16_t *pixel_ptr = (uint16_t *)(row + byte_offset);

                    if (final_alpha == 255)
                    {
                        *pixel_ptr = (uint16_t)stroke_color;
                    }
                    else
                    {
                        uint16_t bg = *pixel_ptr;
                        uint16_t inv_alpha = 256 - final_alpha;

                        uint8_t bg_r = (bg >> 11) & 0x1F;
                        uint8_t bg_g = (bg >> 5) & 0x3F;
                        uint8_t bg_b = bg & 0x1F;

                        uint8_t fg_r = (color_r >> 3) & 0x1F;
                        uint8_t fg_g = (color_g >> 2) & 0x3F;
                        uint8_t fg_b = (color_b >> 3) & 0x1F;

                        uint8_t out_r = ((bg_r * inv_alpha + fg_r * final_alpha) >> 8) & 0x1F;
                        uint8_t out_g = ((bg_g * inv_alpha + fg_g * final_alpha) >> 8) & 0x3F;
                        uint8_t out_b = ((bg_b * inv_alpha + fg_b * final_alpha) >> 8) & 0x1F;

                        *pixel_ptr = (out_r << 11) | (out_g << 5) | out_b;
                    }
                }
            }
        }
    }
}

// ==================== Gradient Functions ====================

/**
 * Initialize a gradient structure
 */
void gradient_init(Gradient *grad, GradientType type)
{
    if (grad == NULL) { return; }

    /* Descriptors compare the complete Gradient byte-for-byte.  Clear inactive
     * stops as well as active fields so equivalent gradients share cache data. */
    memset(grad, 0x00, sizeof(*grad));
    grad->type = type;
    grad->angular_end = 360;
}

/**
 * Add a color stop to gradient
 */
void gradient_add_stop(Gradient *grad, float position, PixelColor color)
{
    if (grad == NULL || grad->stop_count >= 8) { return; }

    // Clamp position to [0, 1]
    position = LG_CLAMP(position, 0.0f, 1.0f);

    // Insert stop in sorted order
    int insert_idx = grad->stop_count;
    for (int i = 0; i < grad->stop_count; i++)
    {
        if (position < grad->stops[i].position)
        {
            insert_idx = i;
            break;
        }
    }

    // Shift existing stops
    for (int i = grad->stop_count; i > insert_idx; i--)
    {
        grad->stops[i] = grad->stops[i - 1];
    }

    // Insert new stop
    grad->stops[insert_idx].position = position;
    grad->stops[insert_idx].color = color;
    grad->stop_count++;
}

/**
 * Interpolate color between two stops
 */
static PixelColor interpolate_color(PixelColor c1, PixelColor c2, float t)
{
    int a1 = (c1 >> 24) & 0xFF;
    int r1 = (c1 >> 16) & 0xFF;
    int g1 = (c1 >> 8) & 0xFF;
    int b1 = c1 & 0xFF;

    int a2 = (c2 >> 24) & 0xFF;
    int r2 = (c2 >> 16) & 0xFF;
    int g2 = (c2 >> 8) & 0xFF;
    int b2 = c2 & 0xFF;

    // Use int for interpolation to avoid overflow
    int a = (int)(a1 + (a2 - a1) * t);
    int r = (int)(r1 + (r2 - r1) * t);
    int g = (int)(g1 + (g2 - g1) * t);
    int b = (int)(b1 + (b2 - b1) * t);

    // Clamp to [0, 255]
    a = (a < 0) ? 0 : ((a > 255) ? 255 : a);
    r = (r < 0) ? 0 : ((r > 255) ? 255 : r);
    g = (g < 0) ? 0 : ((g > 255) ? 255 : g);
    b = (b < 0) ? 0 : ((b > 255) ? 255 : b);

    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

/**
 * Get color from gradient at position t (0.0 to 1.0)
 */
PixelColor gradient_get_color(Gradient *grad, float t)
{
    if (grad == NULL || grad->stop_count == 0)
    {
        return 0xFF000000; // Black
    }

    if (grad->stop_count == 1)
    {
        return grad->stops[0].color;
    }

    // Clamp t to [0, 1]
    t = LG_CLAMP(t, 0.0f, 1.0f);

    // Find the two stops to interpolate between
    if (t <= grad->stops[0].position)
    {
        return grad->stops[0].color;
    }

    if (t >= grad->stops[grad->stop_count - 1].position)
    {
        return grad->stops[grad->stop_count - 1].color;
    }

    for (int i = 0; i < grad->stop_count - 1; i++)
    {
        if (t >= grad->stops[i].position && t <= grad->stops[i + 1].position)
        {
            float pos1 = grad->stops[i].position;
            float pos2 = grad->stops[i + 1].position;
            float local_t = (t - pos1) / (pos2 - pos1);

            return interpolate_color(grad->stops[i].color, grad->stops[i + 1].color, local_t);
        }
    }

    return grad->stops[grad->stop_count - 1].color;
}

/**
 * Get color from gradient at specific angle (for angular gradients)
 * Improved version with correct angle handling for full circles
 */
PixelColor gradient_get_color_at_angle(Gradient *grad, float angle)
{
    if (grad == NULL || grad->type != GRADIENT_ANGULAR)
    {
        return 0xFF000000;
    }

    if (grad->stop_count == 0)
    {
        return 0xFF000000;
    }

    if (grad->stop_count == 1)
    {
        return grad->stops[0].color;
    }

    // Get gradient angle range (use original values, don't normalize)
    float start = grad->angular_start;
    float end = grad->angular_end;

    // Calculate angle span
    float span = end - start;
    if (span <= 0.0f) { span += 360.0f; }

    // Normalize input angle to be relative to start
    float angle_offset = angle - start;

    // Handle wrap-around
    while (angle_offset < 0.0f) { angle_offset += 360.0f; }
    while (angle_offset >= 360.0f) { angle_offset -= 360.0f; }

    // Calculate t (0.0 to 1.0)
    float t = angle_offset / span;

    // Clamp t to [0, 1] range
    if (t < 0.0f) { t = 0.0f; }
    if (t > 1.0f) { t = 1.0f; }

    return gradient_get_color(grad, t);
}

/**
 * Draw arc with gradient - optimized version using LUT
 * Performance optimizations:
 * 1. Pre-computed angle-to-color lookup table (LUT)
 * 2. Fast integer angle calculation using atan2 approximation
 * 3. Squared distance comparison (no sqrt)
 * 4. Minimized branching in hot loop
 */
void draw_arc_df_aa_gradient(DrawContext *ctx, float center_x, float center_y,
                             float radius, float line_width,
                             float start_angle, float end_angle,
                             Gradient *gradient)
{
    if (radius <= 0 || line_width <= 0 || gradient == NULL) { return; }

    // Read gradient angles from struct using union to handle potential alignment issues
    volatile unsigned char *bytes = (volatile unsigned char *)gradient;
    union { unsigned char b[4]; float f; } start_union, end_union;
    start_union.b[0] = bytes[108]; start_union.b[1] = bytes[109];
    start_union.b[2] = bytes[110]; start_union.b[3] = bytes[111];
    end_union.b[0] = bytes[112]; end_union.b[1] = bytes[113];
    end_union.b[2] = bytes[114]; end_union.b[3] = bytes[115];
    float gradient_start = start_union.f;
    float gradient_end = end_union.f;

    // Save original angles for cap calculation
    float original_start_angle = start_angle;
    float original_end_angle = end_angle;

    // CRITICAL: Calculate original angle span BEFORE normalization
    // This preserves the intent of full circle (e.g., 270-630 = 360? span)
    float original_span = original_end_angle - original_start_angle;
    bool is_full_circle = (original_span >= 359.9f || original_span <= -359.9f ||
                           fabsf(original_span) < 0.1f);  // 270-270 case

    // For full circle, treat as 0-360 internally
    if (is_full_circle)
    {
        start_angle = 0.0f;
        end_angle = 360.0f;
    }
    else
    {
        // Normalize angles
        start_angle = normalize_angle_arc(start_angle);
        end_angle = normalize_angle_arc(end_angle);
    }

    // Calculate angle span
    float angle_span = end_angle - start_angle;
    if (angle_span <= 0) { angle_span += 360.0f; }

    // Calculate bounding box
    float outer_r = radius + line_width / 2.0f + 2.0f;
    int min_x = (int)(center_x - outer_r);
    int max_x = (int)(center_x + outer_r) + 1;
    int min_y = (int)(center_y - outer_r);
    int max_y = (int)(center_y + outer_r) + 1;

    // Clip to context bounds
    min_x = LG_MAX(min_x, ctx->clip_rect.x);
    max_x = LG_MIN(max_x, ctx->clip_rect.x + ctx->clip_rect.w);
    min_y = LG_MAX(min_y, ctx->clip_rect.y);
    max_y = LG_MIN(max_y, ctx->clip_rect.y + ctx->clip_rect.h);

    if (min_x >= max_x || min_y >= max_y) { return; }

    // Direct pixel access
    int pixel_size = (ctx->format == PIXEL_FORMAT_RGB565) ? 2 : 4;
    uint8_t *buffer = ctx->buffer;
    int stride = ctx->width * pixel_size;

    // ========== OPTIMIZATION 1: Pre-compute color LUT ==========
    // Build 360-entry lookup table for angle-to-color mapping
    PixelColor color_lut[360];
    for (int i = 0; i < 360; i++)
    {
        color_lut[i] = gradient_get_color_at_angle(gradient, (float)i);
    }

    // ========== OPTIMIZATION 2: Pre-calculate constants ==========
    // Check if gradient angle span exceeds 360 degrees
    float gradient_span = gradient_end - gradient_start;
    if (gradient_span <= 0.0f) { gradient_span += 360.0f; }
    bool gradient_over_360 = (gradient_span > 360.0f);

    bool has_caps = (angle_span < 359.9f);

    // CRITICAL: When gradient > 360?, we need to draw end cap even for full circle
    bool draw_end_cap = gradient_over_360 && !has_caps;

    float start_cap_x = 0, start_cap_y = 0, end_cap_x = 0, end_cap_y = 0;
    PixelColor start_color = gradient->stops[0].color;
    PixelColor end_color = gradient->stops[gradient->stop_count - 1].color;

    // Pre-normalize angles for range checking
    int norm_start_i = (int)start_angle;
    int norm_end_i = (int)end_angle;
    if (norm_start_i < 0) { norm_start_i += 360; }
    if (norm_start_i >= 360) { norm_start_i -= 360; }
    if (norm_end_i < 0) { norm_end_i += 360; }
    if (norm_end_i >= 360) { norm_end_i -= 360; }

    if (has_caps || draw_end_cap)
    {
        float start_rad = original_start_angle * M_PI / 180.0f;
        float end_rad = original_end_angle * M_PI / 180.0f;
        start_cap_x = center_x + radius * cosf(start_rad);
        start_cap_y = center_y + radius * sinf(start_rad);
        end_cap_x = center_x + radius * cosf(end_rad);
        end_cap_y = center_y + radius * sinf(end_rad);
    }

    // ========== OPTIMIZATION 3: Pre-calculate ring parameters ==========
    float inner_r = radius - line_width / 2.0f;
    float outer_radius = radius + line_width / 2.0f;
    float aa_width = 1.5f;  // Unified AA width for both arc body and caps

    // Pre-calculate squared distances for fast comparison
    float outer_sq = outer_radius * outer_radius;
    float inner_sq = inner_r * inner_r;
    float outer_aa_sq = (outer_radius + aa_width) * (outer_radius + aa_width);
    float inner_aa_sq = fmaxf((inner_r - aa_width) * (inner_r - aa_width), 0);

    // Solid region boundaries (no AA needed)
    float outer_solid = outer_radius - aa_width;
    float inner_solid = inner_r + aa_width;
    float outer_solid_sq = outer_solid * outer_solid;
    float inner_solid_sq = inner_solid * inner_solid;

    // ========== OPTIMIZED PATH FOR FULL CIRCLE ==========
    if (is_full_circle && !draw_end_cap)
    {
        // Full circle gradient - simplified loop without angle range check
        for (int py = min_y; py < max_y; py++)
        {
            float dy = py + 0.5f - center_y;
            float dy_sq = dy * dy;
            uint8_t *row = buffer + py * stride;

            for (int px = min_x; px < max_x; px++)
            {
                float dx = px + 0.5f - center_x;
                float dist_sq = dx * dx + dy_sq;

                // Quick rejection
                if (dist_sq > outer_aa_sq) { continue; }
                if (dist_sq < inner_aa_sq && inner_r > 2.0f) { continue; }

                // Compute coverage
                float coverage = compute_coverage_fast(dist_sq, outer_sq, inner_sq,
                                                       outer_aa_sq, inner_aa_sq,
                                                       outer_solid_sq, inner_solid_sq);
                if (coverage < 0.001f) { continue; }

                // Fast angle calculation
                int angle_i = lg_atan2((int)dy, (int)dx);
                if (angle_i < 0) { angle_i += 360; }
                if (angle_i >= 360) { angle_i -= 360; }

                // Direct LUT lookup for full circle
                PixelColor stroke_color = color_lut[angle_i];

                uint8_t color_a = (stroke_color >> 24) & 0xFF;
                if (color_a == 0) { continue; }

                uint8_t color_r = (stroke_color >> 16) & 0xFF;
                uint8_t color_g = (stroke_color >> 8) & 0xFF;
                uint8_t color_b = stroke_color & 0xFF;

                uint8_t final_alpha = (uint8_t)(color_a * coverage);
                if (final_alpha == 0) { continue; }

                int byte_offset = px * pixel_size;

                if (ctx->format == PIXEL_FORMAT_ARGB8888)
                {
                    uint32_t *pixel_ptr = (uint32_t *)(row + byte_offset);
                    if (final_alpha == 255)
                    {
                        *pixel_ptr = stroke_color;
                    }
                    else
                    {
                        uint32_t bg = *pixel_ptr;
                        uint8_t bg_a = (bg >> 24) & 0xFF;
                        if (bg_a == 0)
                        {
                            *pixel_ptr = (final_alpha << 24) | (color_r << 16) | (color_g << 8) | color_b;
                        }
                        else
                        {
                            uint16_t inv_alpha = 256 - final_alpha;
                            uint8_t bg_r = (bg >> 16) & 0xFF;
                            uint8_t bg_g = (bg >> 8) & 0xFF;
                            uint8_t bg_b = bg & 0xFF;
                            uint8_t out_r = ((bg_r * inv_alpha + color_r * final_alpha) >> 8) & 0xFF;
                            uint8_t out_g = ((bg_g * inv_alpha + color_g * final_alpha) >> 8) & 0xFF;
                            uint8_t out_b = ((bg_b * inv_alpha + color_b * final_alpha) >> 8) & 0xFF;
                            uint8_t out_a = bg_a + (((255 - bg_a) * final_alpha) >> 8);
                            *pixel_ptr = (out_a << 24) | (out_r << 16) | (out_g << 8) | out_b;
                        }
                    }
                }
                else if (ctx->format == PIXEL_FORMAT_RGB565)
                {
                    uint16_t *pixel_ptr = (uint16_t *)(row + byte_offset);
                    if (final_alpha == 255)
                    {
                        *pixel_ptr = (uint16_t)stroke_color;
                    }
                    else
                    {
                        uint16_t bg = *pixel_ptr;
                        uint16_t inv_alpha = 256 - final_alpha;
                        uint8_t bg_r = (bg >> 11) & 0x1F;
                        uint8_t bg_g = (bg >> 5) & 0x3F;
                        uint8_t bg_b = bg & 0x1F;
                        uint8_t fg_r = (color_r >> 3) & 0x1F;
                        uint8_t fg_g = (color_g >> 2) & 0x3F;
                        uint8_t fg_b = (color_b >> 3) & 0x1F;
                        uint8_t out_r = ((bg_r * inv_alpha + fg_r * final_alpha) >> 8) & 0x1F;
                        uint8_t out_g = ((bg_g * inv_alpha + fg_g * final_alpha) >> 8) & 0x3F;
                        uint8_t out_b = ((bg_b * inv_alpha + fg_b * final_alpha) >> 8) & 0x1F;
                        *pixel_ptr = (out_r << 11) | (out_g << 5) | out_b;
                    }
                }
            }
        }
        return;
    }

    // ========== Main rendering loop for partial arcs ==========
    // Use hybrid approach: fast path for arc body, arc_sdf only for cap regions
    float cap_r = line_width / 2.0f;
    float cap_check_sq = (cap_r + aa_width + 2.0f) * (cap_r + aa_width + 2.0f);

    for (int py = min_y; py < max_y; py++)
    {
        float dy = py + 0.5f - center_y;
        float dy_sq = dy * dy;
        uint8_t *row = buffer + py * stride;

        for (int px = min_x; px < max_x; px++)
        {
            float dx = px + 0.5f - center_x;
            float dist_sq = dx * dx + dy_sq;

            // Quick rejection: outside outer circle or inside inner circle
            if (dist_sq > outer_aa_sq) { continue; }
            if (dist_sq < inner_aa_sq && inner_r > 2.0f) { continue; }

            // ========== Fast angle calculation for color lookup ==========
            int angle_i = lg_atan2((int)dy, (int)dx);
            if (angle_i < 0) { angle_i += 360; }
            if (angle_i >= 360) { angle_i -= 360; }

            // ========== Angle range check ==========
            bool in_arc_range = true;
            if (has_caps)
            {
                if (norm_start_i <= norm_end_i)
                {
                    in_arc_range = (angle_i >= norm_start_i && angle_i <= norm_end_i);
                }
                else
                {
                    in_arc_range = (angle_i >= norm_start_i || angle_i <= norm_end_i);
                }
            }

            float coverage;
            PixelColor stroke_color;
            float sample_x = px + 0.5f;
            float sample_y = py + 0.5f;

            if (has_caps && !in_arc_range)
            {
                // In cap region - check distance to caps
                float dx_start = sample_x - start_cap_x;
                float dy_start = sample_y - start_cap_y;
                float dx_end = sample_x - end_cap_x;
                float dy_end = sample_y - end_cap_y;

                float dist_start_sq = dx_start * dx_start + dy_start * dy_start;
                float dist_end_sq = dx_end * dx_end + dy_end * dy_end;

                // Quick rejection for cap region
                if (dist_start_sq > cap_check_sq && dist_end_sq > cap_check_sq)
                {
                    continue;
                }

                // Use arc_sdf only for cap region pixels (seamless transition)
                float sdf = arc_sdf(sample_x, sample_y, center_x, center_y,
                                    radius, line_width, start_angle, end_angle);
                coverage = 1.0f - smoothstep(-aa_width, aa_width, sdf);

                if (coverage < 0.001f) { continue; }

                // Determine cap color
                if (gradient_over_360 && dist_end_sq < dist_start_sq)
                {
                    stroke_color = gradient_get_color_at_angle(gradient, (float)angle_i);
                }
                else
                {
                    stroke_color = (dist_start_sq < dist_end_sq) ? start_color : end_color;
                }
            }
            else
            {
                // In arc range - use fast coverage calculation
                coverage = compute_coverage_fast(dist_sq, outer_sq, inner_sq,
                                                 outer_aa_sq, inner_aa_sq,
                                                 outer_solid_sq, inner_solid_sq);
                if (coverage < 0.001f) { continue; }

                // Use LUT for color lookup
                stroke_color = color_lut[angle_i];
            }

            // Extract color components
            uint8_t color_a = (stroke_color >> 24) & 0xFF;
            if (color_a == 0) { continue; }

            uint8_t color_r = (stroke_color >> 16) & 0xFF;
            uint8_t color_g = (stroke_color >> 8) & 0xFF;
            uint8_t color_b = stroke_color & 0xFF;

            // Compute final alpha
            uint8_t final_alpha = (uint8_t)(color_a * coverage);
            if (final_alpha == 0) { continue; }

            // Write pixel
            int byte_offset = px * pixel_size;

            if (ctx->format == PIXEL_FORMAT_ARGB8888)
            {
                uint32_t *pixel_ptr = (uint32_t *)(row + byte_offset);

                if (final_alpha == 255)
                {
                    *pixel_ptr = stroke_color;
                }
                else
                {
                    uint32_t bg = *pixel_ptr;
                    uint8_t bg_a = (bg >> 24) & 0xFF;

                    if (bg_a == 0)
                    {
                        *pixel_ptr = (final_alpha << 24) | (color_r << 16) | (color_g << 8) | color_b;
                    }
                    else
                    {
                        uint16_t inv_alpha = 256 - final_alpha;
                        uint8_t bg_r = (bg >> 16) & 0xFF;
                        uint8_t bg_g = (bg >> 8) & 0xFF;
                        uint8_t bg_b = bg & 0xFF;

                        uint8_t out_r = ((bg_r * inv_alpha + color_r * final_alpha) >> 8) & 0xFF;
                        uint8_t out_g = ((bg_g * inv_alpha + color_g * final_alpha) >> 8) & 0xFF;
                        uint8_t out_b = ((bg_b * inv_alpha + color_b * final_alpha) >> 8) & 0xFF;
                        uint8_t out_a = bg_a + (((255 - bg_a) * final_alpha) >> 8);

                        *pixel_ptr = (out_a << 24) | (out_r << 16) | (out_g << 8) | out_b;
                    }
                }
            }
            else if (ctx->format == PIXEL_FORMAT_RGB565)
            {
                uint16_t *pixel_ptr = (uint16_t *)(row + byte_offset);

                if (final_alpha == 255)
                {
                    *pixel_ptr = (uint16_t)stroke_color;
                }
                else
                {
                    uint16_t bg = *pixel_ptr;
                    uint16_t inv_alpha = 256 - final_alpha;

                    uint8_t bg_r = (bg >> 11) & 0x1F;
                    uint8_t bg_g = (bg >> 5) & 0x3F;
                    uint8_t bg_b = bg & 0x1F;

                    uint8_t fg_r = (color_r >> 3) & 0x1F;
                    uint8_t fg_g = (color_g >> 2) & 0x3F;
                    uint8_t fg_b = (color_b >> 3) & 0x1F;

                    uint8_t out_r = ((bg_r * inv_alpha + fg_r * final_alpha) >> 8) & 0x1F;
                    uint8_t out_g = ((bg_g * inv_alpha + fg_g * final_alpha) >> 8) & 0x3F;
                    uint8_t out_b = ((bg_b * inv_alpha + fg_b * final_alpha) >> 8) & 0x1F;

                    *pixel_ptr = (out_r << 11) | (out_g << 5) | out_b;
                }
            }
        }
    }

    // ========== SPECIAL: Draw end cap for gradient > 360? on full circle ==========
    // When arc is full circle (0-360) but gradient exceeds 360? (e.g., 0-361),
    // we need to draw the end cap separately since arc_sdf doesn't handle it
    // The end cap should protrude OUTWARD from the arc at the end angle position
    if (draw_end_cap)
    {
        float cap_radius = line_width / 2.0f;

        // Calculate end cap center position
        // The cap center should be on the arc's center line (at radius distance from center)
        // at the gradient end angle position
        // Use gradient_end from gradient struct (already read at function start)
        float end_angle_rad = gradient_end * M_PI / 180.0f;
        float cap_center_x = center_x + radius * cosf(end_angle_rad);
        float cap_center_y = center_y + radius * sinf(end_angle_rad);

        int cap_min_x = (int)(cap_center_x - cap_radius - 2);
        int cap_max_x = (int)(cap_center_x + cap_radius + 2);
        int cap_min_y = (int)(cap_center_y - cap_radius - 2);
        int cap_max_y = (int)(cap_center_y + cap_radius + 2);

        // Clip to context bounds
        cap_min_x = LG_MAX(cap_min_x, ctx->clip_rect.x);
        cap_max_x = LG_MIN(cap_max_x, ctx->clip_rect.x + ctx->clip_rect.w);
        cap_min_y = LG_MAX(cap_min_y, ctx->clip_rect.y);
        cap_max_y = LG_MIN(cap_max_y, ctx->clip_rect.y + ctx->clip_rect.h);

        for (int py = cap_min_y; py < cap_max_y; py++)
        {
            float dy_cap = py + 0.5f - cap_center_y;
            uint8_t *row = buffer + py * stride;

            for (int px = cap_min_x; px < cap_max_x; px++)
            {
                float dx_cap = px + 0.5f - cap_center_x;

                // Distance from cap center
                float dist_cap = sqrtf(dx_cap * dx_cap + dy_cap * dy_cap);

                // SDF for circle cap
                float sdf = dist_cap - cap_radius;
                float coverage = 1.0f - smoothstep(-aa_width, aa_width, sdf);
                if (coverage < 0.001f) { continue; }

                // Use the end color of the gradient for the cap
                PixelColor stroke_color = end_color;

                uint8_t color_a = (stroke_color >> 24) & 0xFF;
                if (color_a == 0) { continue; }

                uint8_t color_r = (stroke_color >> 16) & 0xFF;
                uint8_t color_g = (stroke_color >> 8) & 0xFF;
                uint8_t color_b = stroke_color & 0xFF;

                uint8_t final_alpha = (uint8_t)(color_a * coverage);
                if (final_alpha == 0) { continue; }

                int byte_offset = px * pixel_size;

                if (ctx->format == PIXEL_FORMAT_ARGB8888)
                {
                    uint32_t *pixel_ptr = (uint32_t *)(row + byte_offset);
                    if (final_alpha == 255)
                    {
                        *pixel_ptr = stroke_color;
                    }
                    else
                    {
                        uint32_t bg = *pixel_ptr;
                        uint8_t bg_a = (bg >> 24) & 0xFF;
                        if (bg_a == 0)
                        {
                            *pixel_ptr = (final_alpha << 24) | (color_r << 16) | (color_g << 8) | color_b;
                        }
                        else
                        {
                            uint16_t inv_alpha = 256 - final_alpha;
                            uint8_t bg_r = (bg >> 16) & 0xFF;
                            uint8_t bg_g = (bg >> 8) & 0xFF;
                            uint8_t bg_b = bg & 0xFF;
                            uint8_t out_r = ((bg_r * inv_alpha + color_r * final_alpha) >> 8) & 0xFF;
                            uint8_t out_g = ((bg_g * inv_alpha + color_g * final_alpha) >> 8) & 0xFF;
                            uint8_t out_b = ((bg_b * inv_alpha + color_b * final_alpha) >> 8) & 0xFF;
                            uint8_t out_a = bg_a + (((255 - bg_a) * final_alpha) >> 8);
                            *pixel_ptr = (out_a << 24) | (out_r << 16) | (out_g << 8) | out_b;
                        }
                    }
                }
                else if (ctx->format == PIXEL_FORMAT_RGB565)
                {
                    uint16_t *pixel_ptr = (uint16_t *)(row + byte_offset);
                    if (final_alpha == 255)
                    {
                        *pixel_ptr = (uint16_t)stroke_color;
                    }
                    else
                    {
                        uint16_t bg = *pixel_ptr;
                        uint16_t inv_alpha = 256 - final_alpha;
                        uint8_t bg_r = (bg >> 11) & 0x1F;
                        uint8_t bg_g = (bg >> 5) & 0x3F;
                        uint8_t bg_b = bg & 0x1F;
                        uint8_t fg_r = (color_r >> 3) & 0x1F;
                        uint8_t fg_g = (color_g >> 2) & 0x3F;
                        uint8_t fg_b = (color_b >> 3) & 0x1F;
                        uint8_t out_r = ((bg_r * inv_alpha + fg_r * final_alpha) >> 8) & 0x1F;
                        uint8_t out_g = ((bg_g * inv_alpha + fg_g * final_alpha) >> 8) & 0x3F;
                        uint8_t out_b = ((bg_b * inv_alpha + fg_b * final_alpha) >> 8) & 0x1F;
                        *pixel_ptr = (out_r << 11) | (out_g << 5) | out_b;
                    }
                }
            }
        }
    }
}
