/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <math.h>
#include <string.h>

#include "lite_geometry.h"
#include "lite_geometry_math.h"

/**
 * Accumulate into an A8 mask.
 * Overlapping writes take max coverage, matching ARGB behavior (overlap_keeps_max_alpha).
 */
static inline void a8_accumulate(uint8_t *pixel, uint8_t value)
{
    if (value > *pixel)
    {
        *pixel = value;
    }
}

/** Hermite ramp 3t^2 - 2t^3 over t in [0, 1]. */
static inline float smooth01(float t)
{
    return t * t * (3.0f - 2.0f * t);
}

/** Coverage of a signed distance (negative inside), feathered over +-aa. */
static inline float sdf_coverage(float d, float aa)
{
    return 1.0f - smooth01(LG_CLAMP((d + aa) / (aa + aa), 0.0f, 1.0f));
}

/**
 * Source-over of one ARGB8888 pixel, as the arc rasterisers do it.
 *
 * This is the 8-bit-shift blend, not blend_colors_argb8888(): the two round
 * differently by up to one level, and the arc output is kept as it was.
 * @p keep_max_on_same_rgb takes the larger alpha when the destination already
 * holds the same colour, so a pixel two raster passes cover does not darken.
 */
static inline void arc_blend_argb8888(uint32_t *pixel, PixelColor color, uint8_t alpha,
                                      bool keep_max_on_same_rgb)
{
    if (alpha == 255)
    {
        *pixel = color;
        return;
    }

    uint32_t bg = *pixel;
    uint32_t bg_a = bg >> 24;
    uint32_t rgb = color & 0x00FFFFFFu;
    if (bg_a == 0)
    {
        *pixel = ((uint32_t)alpha << 24) | rgb;
        return;
    }
    if (keep_max_on_same_rgb && (bg & 0x00FFFFFFu) == rgb)
    {
        *pixel = ((uint32_t)LG_MAX(alpha, bg_a) << 24) | rgb;
        return;
    }

    uint32_t inv_alpha = 256u - alpha;
    uint32_t out_r = ((((bg >> 16) & 0xFFu) * inv_alpha + ((color >> 16) & 0xFFu) * alpha) >> 8) &
                     0xFFu;
    uint32_t out_g = ((((bg >> 8) & 0xFFu) * inv_alpha + ((color >> 8) & 0xFFu) * alpha) >> 8) & 0xFFu;
    uint32_t out_b = (((bg & 0xFFu) * inv_alpha + (color & 0xFFu) * alpha) >> 8) & 0xFFu;
    uint32_t out_a = bg_a + (((255u - bg_a) * alpha) >> 8);
    *pixel = (out_a << 24) | (out_r << 16) | (out_g << 8) | out_b;
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
    ctx->stride = width * (format == PIXEL_FORMAT_A8 ? 1 : 4);
    ctx->overlap_keeps_max_alpha = false;
    ctx->run_sink = NULL;
}

static void add_pixel_aa(DrawContext *ctx, int x, int y, PixelColor color,
                         float coverage)
{
    /* Sparse destination: store coverage only. */
    if (ctx->run_sink != NULL)
    {
        uint8_t source_alpha = (color >> 24) & 0xFF;
        if (source_alpha == 0) { return; }

        gui_geometry_run_sink_put(ctx->run_sink, x, y, (uint8_t)(source_alpha * coverage));
        return;
    }

    if (ctx->format == PIXEL_FORMAT_A8)
    {
        uint8_t source_alpha = (color >> 24) & 0xFF;
        if (source_alpha == 0) { return; }

        uint8_t alpha = (uint8_t)(source_alpha * coverage);
        a8_accumulate(ctx->buffer + y * ctx->stride + x, alpha);
        return;
    }

    uint8_t source_alpha = (color >> 24) & 0xFF;
    if (source_alpha == 0) { return; }

    uint8_t alpha = (uint8_t)(source_alpha * coverage);
    if (alpha == 0) { return; }

    uint32_t *pixel = (uint32_t *)(ctx->buffer + y * ctx->stride) + x;
    if (alpha == 255)
    {
        *pixel = color;
    }
    else
    {
        /* Keep max alpha for same-color overlaps if buffer is dedicated to one shape. */
        uint32_t bg = *pixel;
        uint8_t bg_alpha = (bg >> 24) & 0xFF;
        if (ctx->overlap_keeps_max_alpha && bg_alpha != 0 &&
            (bg & 0x00FFFFFFu) == (color & 0x00FFFFFFu))
        {
            if (alpha > bg_alpha)
            {
                *pixel = ((uint32_t)alpha << 24) | (color & 0x00FFFFFFu);
            }
        }
        else
        {
            *pixel = blend_colors_argb8888(bg, color, alpha);
        }
    }
}

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

// ==================== Arc helpers ====================

/** Stroke too thin for the two-band coverage model (outer and inner bands overlap). */
static inline bool arc_is_thin(float line_width)
{
    return line_width < 2.0f * LG_ARC_AA_WIDTH;
}

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

/** Box-filtered coverage for thin strokes. Callers must avoid caps. */
static inline float arc_stroke_coverage(float d, float half)
{
    return LG_CLAMP(half + 0.5f - fabsf(d), 0.0f, 1.0f);
}

/** Coverage using squared distances (avoids sqrt) with smoothstep falloff. */
static inline float compute_coverage_fast(float dist_sq, float inner_sq,
                                          float outer_aa_sq, float inner_aa_sq,
                                          float outer_solid_sq, float inner_solid_sq)
{
    if (dist_sq <= outer_solid_sq && dist_sq >= inner_solid_sq)
    {
        return 1.0f;
    }

    if (dist_sq > outer_solid_sq && dist_sq <= outer_aa_sq)
    {
        return 1.0f - smooth01((dist_sq - outer_solid_sq) / (outer_aa_sq - outer_solid_sq));
    }

    if (inner_sq > 0 && dist_sq >= inner_aa_sq && dist_sq < inner_solid_sq)
    {
        return 1.0f - smooth01((inner_solid_sq - dist_sq) / (inner_solid_sq - inner_aa_sq));
    }

    return 0.0f;
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

    if (ctx->run_sink != NULL)
    {
        if (alpha == 0) { return; }

        for (int x = start_x; x <= end_x; x++)
        {
            gui_geometry_run_sink_put(ctx->run_sink, x, y, alpha);
        }
        return;
    }

    /* Fully covered A8 span is simply a memset of alpha. */
    if (ctx->format == PIXEL_FORMAT_A8)
    {
        memset(ctx->buffer + y * ctx->stride + start_x, alpha, (size_t)(end_x - start_x + 1));
        return;
    }

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

/** Draw a complete thin ring via distance profile. */
static void draw_arc_as_ring_thin(DrawContext *ctx, float center_x, float center_y,
                                  float arc_radius, float line_width, PixelColor color)
{
    float half = line_width * 0.5f;
    float reach = half + 0.5f;

    int clip_x1 = ctx->clip_rect.x;
    int clip_x2 = ctx->clip_rect.x + ctx->clip_rect.w - 1;
    int min_y = LG_MAX((int)floorf(center_y - arc_radius - reach), ctx->clip_rect.y);
    int max_y = LG_MIN((int)ceilf(center_y + arc_radius + reach),
                       ctx->clip_rect.y + ctx->clip_rect.h - 1);

    for (int y = min_y; y <= max_y; y++)
    {
        float dy = (float)y + 0.5f - center_y;
        float dy_sq = dy * dy;

        float band_sq = (arc_radius + reach) * (arc_radius + reach);
        if (dy_sq > band_sq) { continue; }

        float outer_x = sqrtf(band_sq - dy_sq);

        float inner_reach = arc_radius - reach;
        float inner_x = (inner_reach > 0.0f && dy_sq < inner_reach * inner_reach) ?
                        sqrtf(inner_reach * inner_reach - dy_sq) : 0.0f;

        int left_from = LG_MAX((int)ceilf(center_x - outer_x - 0.5f), clip_x1);
        int left_to = LG_MIN((int)floorf(center_x - inner_x - 0.5f), clip_x2);
        int right_from = LG_MAX((int)ceilf(center_x + inner_x - 0.5f), clip_x1);
        int right_to = LG_MIN((int)floorf(center_x + outer_x - 0.5f), clip_x2);

        if (inner_reach <= 0.0f)
        {
            left_to = LG_MIN((int)floorf(center_x - 0.5f), clip_x2);
            right_from = LG_MAX((int)ceilf(center_x - 0.5f), clip_x1);
        }

        for (int pass = 0; pass < 2; pass++)
        {
            int from = (pass == 0) ? left_from : right_from;
            int to = (pass == 0) ? left_to : right_to;

            for (int x = from; x <= to; x++)
            {
                float dx = (float)x + 0.5f - center_x;
                float dist = sqrtf(dx * dx + dy_sq);
                float coverage = arc_stroke_coverage(dist - arc_radius, half);

                if (coverage > 0.001f)
                {
                    add_pixel_aa(ctx, x, y, color, coverage);
                }
            }
        }
    }
}

static void draw_arc_as_ring(DrawContext *ctx, float center_x, float center_y,
                             float arc_radius, float line_width, PixelColor color)
{
    if (line_width <= 0.0f) { return; }

    if (arc_is_thin(line_width))
    {
        draw_arc_as_ring_thin(ctx, center_x, center_y, arc_radius, line_width, color);
        return;
    }

    float outer_radius = arc_radius + line_width / 2.0f;
    float inner_radius = fmaxf(arc_radius - line_width / 2.0f, 0.0f);
    float aa_width = LG_ARC_AA_WIDTH;
    float outer_aa_radius = outer_radius + aa_width;
    float outer_solid_radius = fmaxf(outer_radius - aa_width, 0.0f);
    float inner_aa_radius = fmaxf(inner_radius - aa_width, 0.0f);
    float inner_solid_radius = inner_radius + aa_width;

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
        int outer_left_aa = (int)ceilf(center_x - outer_aa_x - 0.5f);
        int outer_right_aa = (int)floorf(center_x + outer_aa_x - 0.5f);

        // Row entirely in outer AA band: no solid span, prevents center spike.
        if (dy_sq >= outer_solid_sq)
        {
            outer_left_solid = outer_right_aa + 1;
            outer_right_solid = outer_right_aa;
        }

        if (inner_radius > 0.0f && inner_solid_x > 0.0f)
        {
            fill_ring_span_argb8888(ctx, y, outer_left_solid, inner_left_solid, color);
            fill_ring_span_argb8888(ctx, y, inner_right_solid, outer_right_solid, color);
        }
        else
        {
            fill_ring_span_argb8888(ctx, y, outer_left_solid, outer_right_solid, color);
        }

        int outer_left_end = LG_MIN(outer_left_solid - 1, clip_x2);
        for (int x = LG_MAX(outer_left_aa, clip_x1); x <= outer_left_end; x++)
        {
            float dx = (float)x + 0.5f - center_x;
            float coverage = compute_coverage_fast(dx * dx + dy_sq, inner_sq,
                                                   outer_aa_sq, inner_aa_sq,
                                                   outer_solid_sq, inner_solid_sq);
            if (coverage > 0.001f) { add_pixel_aa(ctx, x, y, color, coverage); }
        }
        int outer_right_start = LG_MAX(outer_right_solid + 1, clip_x1);
        for (int x = outer_right_start; x <= LG_MIN(outer_right_aa, clip_x2); x++)
        {
            float dx = (float)x + 0.5f - center_x;
            float coverage = compute_coverage_fast(dx * dx + dy_sq, inner_sq,
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
                float coverage = compute_coverage_fast(dx * dx + dy_sq, inner_sq,
                                                       outer_aa_sq, inner_aa_sq,
                                                       outer_solid_sq, inner_solid_sq);
                if (coverage > 0.001f) { add_pixel_aa(ctx, x, y, color, coverage); }
            }
            int inner_right_end = LG_MIN(inner_right_solid - 1, clip_x2);
            for (int x = LG_MAX(inner_right_aa, clip_x1); x <= inner_right_end; x++)
            {
                float dx = (float)x + 0.5f - center_x;
                float coverage = compute_coverage_fast(dx * dx + dy_sq, inner_sq,
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
            float coverage = sdf_coverage(distance, aa_width);
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
    if ((uint8_t)(color >> 24) != UINT8_MAX)
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

    if (arc_is_thin(line_width))
    {
        return false;
    }

    float aa_width = LG_ARC_AA_WIDTH;
    float outer_radius = radius + cap_radius;
    float inner_radius = fmaxf(radius - cap_radius, 0.0f);
    float outer_aa_radius = outer_radius + aa_width;
    float outer_solid_radius = fmaxf(outer_radius - aa_width, 0.0f);
    float inner_aa_radius = fmaxf(inner_radius - aa_width, 0.0f);
    float inner_solid_radius = inner_radius + aa_width;

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
        int outer_left_aa = (int)ceilf(center_x - outer_aa_x - 0.5f);
        int outer_right_aa = (int)floorf(center_x + outer_aa_x - 0.5f);

        // Row entirely in outer AA band: no solid span (prevents 90/270 spike).
        if (dy_sq >= outer_solid_sq)
        {
            outer_left_solid = outer_right_aa + 1;
            outer_right_solid = outer_right_aa;
        }

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

        int outer_left_end = LG_MIN(outer_left_solid - 1, clip_x2);
        for (int x = LG_MAX(outer_left_aa, clip_x1); x <= outer_left_end; x++)
        {
            float dx = (float)x + 0.5f - center_x;
            if (!is_in_angle_range_cross(dx, dy, start_cos, start_sin,
                                         end_cos, end_sin, wide_arc))
            {
                continue;
            }
            float coverage = compute_coverage_fast(dx * dx + dy_sq, inner_sq,
                                                   outer_aa_sq, inner_aa_sq,
                                                   outer_solid_sq, inner_solid_sq);
            if (coverage > 0.001f) { add_pixel_aa(ctx, x, y, color, coverage); }
        }

        int outer_right_start = LG_MAX(outer_right_solid + 1, clip_x1);
        for (int x = outer_right_start; x <= LG_MIN(outer_right_aa, clip_x2); x++)
        {
            float dx = (float)x + 0.5f - center_x;
            if (!is_in_angle_range_cross(dx, dy, start_cos, start_sin,
                                         end_cos, end_sin, wide_arc))
            {
                continue;
            }
            float coverage = compute_coverage_fast(dx * dx + dy_sq, inner_sq,
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
                float coverage = compute_coverage_fast(dx * dx + dy_sq, inner_sq,
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
                float coverage = compute_coverage_fast(dx * dx + dy_sq, inner_sq,
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

// ==================== Arc drawing ====================

static inline float normalize_angle_arc(float angle)
{
    while (angle < 0) { angle += 360.0f; }
    while (angle > 360.0f) { angle -= 360.0f; }
    return angle;
}

static inline bool is_angle_in_arc_range(float angle, float start, float end)
{
    angle = normalize_angle_arc(angle);
    start = normalize_angle_arc(start);
    end = normalize_angle_arc(end);

    if (fabsf(end - start) >= 359.9f)
    {
        return true;
    }

    if (start <= end)
    {
        return (angle >= start && angle <= end);
    }
    else
    {
        return (angle >= start || angle <= end);
    }
}

/** Signed distance to arc. Negative = inside. */
static inline float arc_sdf(float px, float py, float cx, float cy,
                            float radius, float line_width,
                            float start_angle, float end_angle)
{
    float dx = px - cx;
    float dy = py - cy;
    float dist = sqrtf(dx * dx + dy * dy);

    float inner_r = radius - line_width / 2.0f;
    float outer_r = radius + line_width / 2.0f;
    float ring_dist;

    if (dist < inner_r)
    {
        ring_dist = inner_r - dist;
    }
    else if (dist > outer_r)
    {
        ring_dist = dist - outer_r;
    }
    else
    {
        /* Signed distance to centreline, prevents sign flip. */
        ring_dist = fabsf(dist - radius) - line_width / 2.0f;
    }

    if (fabsf(end_angle - start_angle) < 359.9f)
    {
        float angle = atan2f(dy, dx) * 180.0f / M_PI;
        if (!is_angle_in_arc_range(angle, start_angle, end_angle))
        {
            float start_rad = start_angle * M_PI / 180.0f;
            float end_rad = end_angle * M_PI / 180.0f;

            float start_x = cx + radius * cosf(start_rad);
            float start_y = cy + radius * sinf(start_rad);
            float end_x = cx + radius * cosf(end_rad);
            float end_y = cy + radius * sinf(end_rad);

            float dist_to_start = sqrtf((px - start_x) * (px - start_x) + (py - start_y) * (py - start_y));
            float dist_to_end = sqrtf((px - end_x) * (px - end_x) + (py - end_y) * (py - end_y));

            float cap_dist = fminf(dist_to_start, dist_to_end) - line_width / 2.0f;
            return fmaxf(ring_dist, cap_dist);
        }
    }

    return ring_dist;
}

/** Draw a partial thin arc using a single distance field walk. */
static void arc_draw_thin_outline(DrawContext *ctx, float center_x, float center_y,
                                  float radius, float line_width,
                                  float start_angle, float end_angle,
                                  PixelColor color)
{
    float half = line_width * 0.5f;
    float reach = half + 0.5f;
    float outer = radius + reach;

    int min_x = LG_MAX((int)floorf(center_x - outer), ctx->clip_rect.x);
    int max_x = LG_MIN((int)ceilf(center_x + outer), ctx->clip_rect.x + ctx->clip_rect.w - 1);
    int min_y = LG_MAX((int)floorf(center_y - outer), ctx->clip_rect.y);
    int max_y = LG_MIN((int)ceilf(center_y + outer), ctx->clip_rect.y + ctx->clip_rect.h - 1);

    for (int y = min_y; y <= max_y; y++)
    {
        float dy = (float)y + 0.5f - center_y;

        for (int x = min_x; x <= max_x; x++)
        {
            float dx = (float)x + 0.5f - center_x;

            if (dx * dx + dy * dy > outer * outer) { continue; }

            float sdf = arc_sdf((float)x + 0.5f, (float)y + 0.5f, center_x, center_y,
                                radius, line_width, start_angle, end_angle);
            float coverage = sdf_coverage(sdf, LG_ARC_AA_WIDTH);

            if (coverage > 0.001f)
            {
                add_pixel_aa(ctx, x, y, color, coverage);
            }
        }
    }
}

// ==================== Arc drawing, quadrants ====================

#define LG_ARC_AA_MARGIN 3.0f
#define LG_ARC_FULL_EPS 0.01f
#define LG_ARC_ANGLE_GUARD 2.0f

/** Compute axis-aligned ink bounds for an arc, including AA margin and caps. */
void lg_arc_ink_bounds(float radius, float line_width,
                       float start_angle, float end_angle,
                       float *min_x, float *min_y, float *max_x, float *max_y)
{
    float r = radius;
    float grow = line_width / 2.0f + LG_ARC_AA_MARGIN;
    float raw_span = end_angle - start_angle;

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
    float a1 = a0 + sweep;

    float c0 = cosf(a0 * M_PI / 180.0f);
    float s0 = sinf(a0 * M_PI / 180.0f);
    float c1 = cosf(a1 * M_PI / 180.0f);
    float s1 = sinf(a1 * M_PI / 180.0f);

    float lo_x = LG_MIN(c0, c1) * r, hi_x = LG_MAX(c0, c1) * r;
    float lo_y = LG_MIN(s0, s1) * r, hi_y = LG_MAX(s0, s1) * r;

    for (int k = 0; k < 4; k++)
    {
        float ak = 90.0f * (float)k;
        if (!((ak >= a0 && ak <= a1) || (ak + 360.0f >= a0 && ak + 360.0f <= a1)))
        {
            continue;
        }
        switch (k)
        {
        case 0: hi_x =  r; break;
        case 1: hi_y =  r; break;
        case 2: lo_x = -r; break;
        default: lo_y = -r; break;
        }
    }

    *min_x = lo_x - grow; *max_x = hi_x + grow;
    *min_y = lo_y - grow; *max_y = hi_y + grow;
}

/** Hybrid arc drawer: fast raster for body, arc_sdf for caps. */
void draw_arc_df_aa(DrawContext *ctx, float center_x, float center_y,
                    float radius, float line_width,
                    float start_angle, float end_angle,
                    PixelColor stroke_color)
{
    if (radius <= 0 || line_width <= 0) { return; }

    start_angle = normalize_angle_arc(start_angle);
    end_angle = normalize_angle_arc(end_angle);

    float angle_span = end_angle - start_angle;
    if (angle_span <= 0) { angle_span += 360.0f; }

    const float EPSILON = 0.001f;
    bool is_full_circle = (fabsf(angle_span - 360.0f) < EPSILON);

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

    if (arc_is_thin(line_width))
    {
        arc_draw_thin_outline(ctx, center_x, center_y, radius, line_width,
                              start_angle, end_angle, stroke_color);
        return;
    }

    float outer_radius = radius + line_width / 2.0f;
    float inner_radius = fmaxf(radius - line_width / 2.0f, 0);
    float aa_width = LG_ARC_AA_WIDTH;

    float inner_sq = inner_radius * inner_radius;
    float outer_aa_sq = (outer_radius + aa_width) * (outer_radius + aa_width);
    float inner_aa_sq = fmaxf((inner_radius - aa_width) * (inner_radius - aa_width), 0);

    float outer_solid = outer_radius - aa_width;
    float inner_solid = inner_radius + aa_width;
    float outer_solid_sq = outer_solid * outer_solid;
    float inner_solid_sq = inner_solid * inner_solid;

    uint16_t start_angle_deg = (uint16_t)start_angle;
    uint16_t end_angle_deg = (uint16_t)end_angle;
    while (start_angle_deg >= 360) { start_angle_deg -= 360; }
    while (end_angle_deg >= 360) { end_angle_deg -= 360; }

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
        for (int q = start_quadrant; q < 4; q++)
        {
            quadrants[q] = true;
        }
        for (int q = 0; q <= end_quadrant; q++)
        {
            quadrants[q] = true;
        }
    }

    int start_cap_quad = start_angle_deg / 90;
    int end_cap_quad = end_angle_deg / 90;

    uint8_t color_a = (stroke_color >> 24) & 0xFF;
    if (color_a == 0) { return; }

    for (int quad = 0; quad < 4; quad++)
    {
        if (!quadrants[quad]) { continue; }

        int qx_start, qx_end, qy_start, qy_end;
        float cap_expand = cap_r + aa_width + 1.0f;
        int center_x_int = (int)center_x;
        int center_y_int = (int)center_y;

        bool has_start_cap = (start_cap_quad == quad);
        bool has_end_cap = (end_cap_quad == quad);

        switch (quad)
        {
        case 0:
            qx_start = center_x_int;
            qx_end = (int)(center_x + outer_radius + aa_width + cap_expand);
            qy_start = center_y_int;
            qy_end = (int)(center_y + outer_radius + aa_width + cap_expand);
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
        case 1:
            qx_start = (int)(center_x - outer_radius - aa_width - cap_expand);
            qx_end = center_x_int - 1;
            qy_start = center_y_int;
            qy_end = (int)(center_y + outer_radius + aa_width + cap_expand);
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
        case 2:
            qx_start = (int)(center_x - outer_radius - aa_width - cap_expand);
            qx_end = center_x_int - 1;
            qy_start = (int)(center_y - outer_radius - aa_width - cap_expand);
            qy_end = center_y_int - 1;
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
        case 3:
            qx_start = center_x_int;
            qx_end = (int)(center_x + outer_radius + aa_width + cap_expand);
            qy_start = (int)(center_y - outer_radius - aa_width - cap_expand);
            qy_end = center_y_int - 1;
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

        qx_start = LG_MAX(qx_start, ctx->clip_rect.x);
        qx_end = LG_MIN(qx_end, ctx->clip_rect.x + ctx->clip_rect.w - 1);
        qy_start = LG_MAX(qy_start, ctx->clip_rect.y);
        qy_end = LG_MIN(qy_end, ctx->clip_rect.y + ctx->clip_rect.h - 1);

        if (qx_start > qx_end || qy_start > qy_end) { continue; }

        for (int py = qy_start; py <= qy_end; py++)
        {
            float dy = (py + 0.5f) - center_y;
            float dy_sq = dy * dy;
            uint8_t *row = ctx->buffer + py * ctx->stride;

            for (int px = qx_start; px <= qx_end; px++)
            {
                float dx = (px + 0.5f) - center_x;
                float dist_sq = dx * dx + dy_sq;

                if (dist_sq > outer_aa_sq) { continue; }
                if (inner_sq > 0 && dist_sq < inner_aa_sq) { continue; }

                float coverage;

                bool in_angle = is_in_angle_range_cross(dx, dy,
                                                        start_cos, start_sin,
                                                        end_cos, end_sin,
                                                        wide_arc);

                if (!in_angle)
                {
                    float sample_x = px + 0.5f;
                    float sample_y = py + 0.5f;
                    float dx_start = sample_x - start_cap_x;
                    float dy_start = sample_y - start_cap_y;
                    float dx_end = sample_x - end_cap_x;
                    float dy_end = sample_y - end_cap_y;

                    float dist_start_sq = dx_start * dx_start + dy_start * dy_start;
                    float dist_end_sq = dx_end * dx_end + dy_end * dy_end;

                    if (dist_start_sq > cap_check_sq && dist_end_sq > cap_check_sq)
                    {
                        continue;
                    }

                    float sdf = arc_sdf(sample_x, sample_y, center_x, center_y,
                                        radius, line_width, start_angle, end_angle);
                    coverage = sdf_coverage(sdf, aa_width);
                }
                else
                {
                    coverage = compute_coverage_fast(dist_sq, inner_sq,
                                                     outer_aa_sq, inner_aa_sq,
                                                     outer_solid_sq, inner_solid_sq);
                }

                if (coverage < 0.001f) { continue; }

                uint8_t final_alpha = (uint8_t)(color_a * coverage);
                if (final_alpha == 0) { continue; }

                if (ctx->format == PIXEL_FORMAT_A8)
                {
                    a8_accumulate(row + px, final_alpha);
                }
                else
                {
                    arc_blend_argb8888((uint32_t *)row + px, stroke_color, final_alpha, true);
                }
            }
        }
    }
}

bool lg_arc_update_a8(DrawContext *ctx, float center_x, float center_y,
                      float radius, float line_width, float start_angle,
                      float old_end_angle, float end_angle, PixelColor color)
{
    if (ctx == NULL || ctx->buffer == NULL || ctx->format != PIXEL_FORMAT_A8 ||
        ctx->run_sink != NULL || (uint8_t)(color >> 24) != UINT8_MAX ||
        !isfinite(radius) || !isfinite(line_width) || radius <= 0.0f ||
        line_width < 2.0f * LG_ARC_AA_WIDTH ||
        !isfinite(start_angle) || !isfinite(old_end_angle) || !isfinite(end_angle) ||
        start_angle < 0.0f || start_angle >= 360.0f ||
        old_end_angle < 0.0f || old_end_angle > 720.0f ||
        end_angle < 0.0f || end_angle > 720.0f ||
        ctx->width <= 0 || ctx->height <= 0 || ctx->width > INT16_MAX ||
        ctx->height > INT16_MAX || !isfinite(center_x) || !isfinite(center_y))
    {
        return false;
    }

    float old_span = fmodf(old_end_angle - start_angle, 360.0f);
    float span = fmodf(end_angle - start_angle, 360.0f);
    if (old_span <= 0.0f) { old_span += 360.0f; }
    if (span <= 0.0f) { span += 360.0f; }

    float min_span = LG_MIN(old_span, span);
    float max_span = LG_MAX(old_span, span);
    if (max_span >= 359.9f || max_span - min_span > 90.0f ||
        (min_span <= 180.0f && max_span >= 180.0f) ||
        radius * min_span * M_PI / 180.0f <= line_width * 1.5f)
    {
        return false;
    }
    if (old_span == span) { return true; }

    float min_x, min_y, max_x, max_y;
    lg_arc_ink_bounds(radius, line_width, start_angle + min_span, start_angle + max_span,
                      &min_x, &min_y, &max_x, &max_y);
    int x0 = LG_MAX(0, ctx->clip_rect.x);
    int y0 = LG_MAX(0, ctx->clip_rect.y);
    int x1 = LG_MIN(ctx->width, ctx->clip_rect.x + ctx->clip_rect.w);
    int y1 = LG_MIN(ctx->height, ctx->clip_rect.y + ctx->clip_rect.h);
    x0 = LG_MAX(x0, (int)floorf(center_x + min_x));
    y0 = LG_MAX(y0, (int)floorf(center_y + min_y));
    x1 = LG_MIN(x1, (int)ceilf(center_x + max_x));
    y1 = LG_MIN(y1, (int)ceilf(center_y + max_y));
    if (x0 >= x1 || y0 >= y1) { return true; }
    if ((int64_t)(x1 - x0) * (y1 - y0) * 2 >= (int64_t)ctx->width * ctx->height)
    {
        return false;
    }

    Rect saved_clip = ctx->clip_rect;
    ctx->clip_rect = (Rect)
    {
        (int16_t)x0, (int16_t)y0,
        (int16_t)(x1 - x0), (int16_t)(y1 - y0)
    };
    for (int y = y0; y < y1; y++)
    {
        memset(ctx->buffer + (size_t)y * ctx->width + x0, 0, (size_t)(x1 - x0));
    }
    draw_arc_df_aa(ctx, center_x, center_y, radius, line_width,
                   start_angle, end_angle, color);
    ctx->clip_rect = saved_clip;
    return true;
}

// ==================== Gradient Functions ====================

/** Initialize gradient. Zeroes memory for cache byte-comparison. */
void gradient_init(Gradient *grad, GradientType type)
{
    if (grad == NULL) { return; }

    memset(grad, 0x00, sizeof(*grad));
    grad->type = type;
    grad->angular_end = 360;
}

void gradient_add_stop(Gradient *grad, float position, PixelColor color)
{
    if (grad == NULL || grad->stop_count >= 8) { return; }

    position = LG_CLAMP(position, 0.0f, 1.0f);

    int insert_idx = grad->stop_count;
    for (int i = 0; i < grad->stop_count; i++)
    {
        if (position < grad->stops[i].position)
        {
            insert_idx = i;
            break;
        }
    }

    for (int i = grad->stop_count; i > insert_idx; i--)
    {
        grad->stops[i] = grad->stops[i - 1];
    }

    grad->stops[insert_idx].position = position;
    grad->stops[insert_idx].color = color;
    grad->stop_count++;
}

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

    int a = (int)(a1 + (a2 - a1) * t);
    int r = (int)(r1 + (r2 - r1) * t);
    int g = (int)(g1 + (g2 - g1) * t);
    int b = (int)(b1 + (b2 - b1) * t);

    a = (a < 0) ? 0 : ((a > 255) ? 255 : a);
    r = (r < 0) ? 0 : ((r > 255) ? 255 : r);
    g = (g < 0) ? 0 : ((g > 255) ? 255 : g);
    b = (b < 0) ? 0 : ((b > 255) ? 255 : b);

    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

PixelColor gradient_get_color(Gradient *grad, float t)
{
    if (grad == NULL || grad->stop_count == 0)
    {
        return 0xFF000000;
    }

    if (grad->stop_count == 1)
    {
        return grad->stops[0].color;
    }

    t = LG_CLAMP(t, 0.0f, 1.0f);

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

    float start = grad->angular_start;
    float end = grad->angular_end;

    float span = end - start;
    if (span <= 0.0f) { span += 360.0f; }

    float angle_offset = angle - start;

    while (angle_offset < 0.0f) { angle_offset += 360.0f; }
    while (angle_offset >= 360.0f) { angle_offset -= 360.0f; }

    float t = angle_offset / span;

    if (t < 0.0f) { t = 0.0f; }
    if (t > 1.0f) { t = 1.0f; }

    return gradient_get_color(grad, t);
}

/**
 * Draw a gradient arc.
 * Uses 360-entry LUT for angle-to-color mapping.
 */
void draw_arc_df_aa_gradient(DrawContext *ctx, float center_x, float center_y,
                             float radius, float line_width,
                             float start_angle, float end_angle,
                             Gradient *gradient)
{
    if (radius <= 0 || line_width <= 0 || gradient == NULL) { return; }

    /* Gradients cannot be expressed as A8 coverage masks. */
    if (ctx->format == PIXEL_FORMAT_A8) { return; }

    volatile unsigned char *bytes = (volatile unsigned char *)gradient;
    union { unsigned char b[4]; float f; } start_union, end_union;
    start_union.b[0] = bytes[108]; start_union.b[1] = bytes[109];
    start_union.b[2] = bytes[110]; start_union.b[3] = bytes[111];
    end_union.b[0] = bytes[112]; end_union.b[1] = bytes[113];
    end_union.b[2] = bytes[114]; end_union.b[3] = bytes[115];
    float gradient_start = start_union.f;
    float gradient_end = end_union.f;

    float original_start_angle = start_angle;
    float original_end_angle = end_angle;

    float original_span = original_end_angle - original_start_angle;
    bool is_full_circle = (original_span >= 359.9f || original_span <= -359.9f ||
                           fabsf(original_span) < 0.1f);

    if (is_full_circle)
    {
        start_angle = 0.0f;
        end_angle = 360.0f;
    }
    else
    {
        start_angle = normalize_angle_arc(start_angle);
        end_angle = normalize_angle_arc(end_angle);
    }

    float angle_span = end_angle - start_angle;
    if (angle_span <= 0) { angle_span += 360.0f; }

    float outer_r = radius + line_width / 2.0f + 2.0f;
    int min_x = (int)(center_x - outer_r);
    int max_x = (int)(center_x + outer_r) + 1;
    int min_y = (int)(center_y - outer_r);
    int max_y = (int)(center_y + outer_r) + 1;

    min_x = LG_MAX(min_x, ctx->clip_rect.x);
    max_x = LG_MIN(max_x, ctx->clip_rect.x + ctx->clip_rect.w);
    min_y = LG_MAX(min_y, ctx->clip_rect.y);
    max_y = LG_MIN(max_y, ctx->clip_rect.y + ctx->clip_rect.h);

    if (min_x >= max_x || min_y >= max_y) { return; }

    uint8_t *buffer = ctx->buffer;
    int stride = ctx->stride;

    PixelColor color_lut[360];
    for (int i = 0; i < 360; i++)
    {
        color_lut[i] = gradient_get_color_at_angle(gradient, (float)i);
    }

    float gradient_span = gradient_end - gradient_start;
    if (gradient_span <= 0.0f) { gradient_span += 360.0f; }
    bool gradient_over_360 = (gradient_span > 360.0f);

    bool has_caps = (angle_span < 359.9f);

    bool draw_end_cap = gradient_over_360 && !has_caps;

    float start_cap_x = 0, start_cap_y = 0, end_cap_x = 0, end_cap_y = 0;

    /* Cap colors follow the gradient at the arc's endpoints. */
    float cap_start_offset = original_start_angle - gradient_start;
    while (cap_start_offset < 0.0f) { cap_start_offset += 360.0f; }
    while (cap_start_offset >= 360.0f) { cap_start_offset -= 360.0f; }
    float cap_end_offset = original_end_angle - gradient_start;
    while (cap_end_offset <= 0.0f) { cap_end_offset += 360.0f; }
    while (cap_end_offset > 360.0f) { cap_end_offset -= 360.0f; }
    float cap_start_t = LG_MIN(cap_start_offset / gradient_span, 1.0f);
    float cap_end_t = LG_MIN(cap_end_offset / gradient_span, 1.0f);
    PixelColor start_color = gradient_get_color(gradient, cap_start_t);
    PixelColor end_color = gradient_get_color(gradient, cap_end_t);

    float range_start_cos = cosf(start_angle * M_PI / 180.0f);
    float range_start_sin = sinf(start_angle * M_PI / 180.0f);
    float range_end_cos = cosf(end_angle * M_PI / 180.0f);
    float range_end_sin = sinf(end_angle * M_PI / 180.0f);
    bool wide_arc = angle_span > 180.0f;

    if (has_caps || draw_end_cap)
    {
        float start_rad = original_start_angle * M_PI / 180.0f;
        float end_rad = original_end_angle * M_PI / 180.0f;
        start_cap_x = center_x + radius * cosf(start_rad);
        start_cap_y = center_y + radius * sinf(start_rad);
        end_cap_x = center_x + radius * cosf(end_rad);
        end_cap_y = center_y + radius * sinf(end_rad);
    }

    float inner_r = radius - line_width / 2.0f;
    float outer_radius = radius + line_width / 2.0f;
    float aa_width = LG_ARC_AA_WIDTH;

    /* Max distance from centerline that inks. Prevents negative inner bound for thick strokes. */
    float reach = LG_MIN(line_width / 2.0f + 0.5f, radius);

    float inner_sq = inner_r * inner_r;
    float outer_aa_sq = (outer_radius + aa_width) * (outer_radius + aa_width);
    float inner_aa_radius = radius - reach;
    float inner_aa_sq = (inner_aa_radius > 0.0f) ? inner_aa_radius * inner_aa_radius : 0.0f;

    float outer_solid = outer_radius - aa_width;
    float inner_solid = inner_r + aa_width;
    float outer_solid_sq = outer_solid * outer_solid;
    float inner_solid_sq = inner_solid * inner_solid;

    if (is_full_circle && !draw_end_cap)
    {
        for (int py = min_y; py < max_y; py++)
        {
            float dy = py + 0.5f - center_y;
            float dy_sq = dy * dy;
            uint8_t *row = buffer + py * stride;

            for (int px = min_x; px < max_x; px++)
            {
                float dx = px + 0.5f - center_x;
                float dist_sq = dx * dx + dy_sq;

                if (dist_sq > outer_aa_sq) { continue; }
                if (dist_sq < inner_aa_sq && inner_r > 2.0f) { continue; }

                float coverage = compute_coverage_fast(dist_sq, inner_sq,
                                                       outer_aa_sq, inner_aa_sq,
                                                       outer_solid_sq, inner_solid_sq);
                if (coverage < 0.001f) { continue; }

                int angle_i = lg_atan2((int)dy, (int)dx);
                if (angle_i < 0) { angle_i += 360; }
                if (angle_i >= 360) { angle_i -= 360; }

                PixelColor stroke_color = color_lut[angle_i];

                uint8_t color_a = (stroke_color >> 24) & 0xFF;
                if (color_a == 0) { continue; }

                uint8_t final_alpha = (uint8_t)(color_a * coverage);
                if (final_alpha == 0) { continue; }

                arc_blend_argb8888((uint32_t *)row + px, stroke_color, final_alpha, false);
            }
        }
        return;
    }

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

            if (dist_sq > outer_aa_sq) { continue; }
            if (dist_sq < inner_aa_sq && inner_r > 2.0f) { continue; }

            int angle_i = lg_atan2((int)dy, (int)dx);
            if (angle_i < 0) { angle_i += 360; }
            if (angle_i >= 360) { angle_i -= 360; }

            bool in_arc_range = true;
            if (has_caps)
            {
                in_arc_range = is_in_angle_range_cross(dx, dy,
                                                       range_start_cos, range_start_sin,
                                                       range_end_cos, range_end_sin,
                                                       wide_arc);
            }

            float coverage;
            PixelColor stroke_color;
            float sample_x = px + 0.5f;
            float sample_y = py + 0.5f;

            if (has_caps && !in_arc_range)
            {
                float dx_start = sample_x - start_cap_x;
                float dy_start = sample_y - start_cap_y;
                float dx_end = sample_x - end_cap_x;
                float dy_end = sample_y - end_cap_y;

                float dist_start_sq = dx_start * dx_start + dy_start * dy_start;
                float dist_end_sq = dx_end * dx_end + dy_end * dy_end;

                if (dist_start_sq > cap_check_sq && dist_end_sq > cap_check_sq)
                {
                    continue;
                }

                float sdf = arc_sdf(sample_x, sample_y, center_x, center_y,
                                    radius, line_width, start_angle, end_angle);
                coverage = sdf_coverage(sdf, aa_width);

                if (coverage < 0.001f) { continue; }

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
                coverage = compute_coverage_fast(dist_sq, inner_sq,
                                                 outer_aa_sq, inner_aa_sq,
                                                 outer_solid_sq, inner_solid_sq);
                if (coverage < 0.001f) { continue; }

                stroke_color = color_lut[angle_i];
            }

            uint8_t color_a = (stroke_color >> 24) & 0xFF;
            if (color_a == 0) { continue; }

            uint8_t final_alpha = (uint8_t)(color_a * coverage);
            if (final_alpha == 0) { continue; }

            arc_blend_argb8888((uint32_t *)row + px, stroke_color, final_alpha, false);
        }
    }

    if (draw_end_cap)
    {
        float cap_radius = line_width / 2.0f;

        float end_angle_rad = gradient_end * M_PI / 180.0f;
        float cap_center_x = center_x + radius * cosf(end_angle_rad);
        float cap_center_y = center_y + radius * sinf(end_angle_rad);

        int cap_min_x = (int)(cap_center_x - cap_radius - 2);
        int cap_max_x = (int)(cap_center_x + cap_radius + 2);
        int cap_min_y = (int)(cap_center_y - cap_radius - 2);
        int cap_max_y = (int)(cap_center_y + cap_radius + 2);

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

                float dist_cap = sqrtf(dx_cap * dx_cap + dy_cap * dy_cap);

                float sdf = dist_cap - cap_radius;
                float coverage = sdf_coverage(sdf, aa_width);
                if (coverage < 0.001f) { continue; }

                PixelColor stroke_color = end_color;

                uint8_t color_a = (stroke_color >> 24) & 0xFF;
                if (color_a == 0) { continue; }

                uint8_t final_alpha = (uint8_t)(color_a * coverage);
                if (final_alpha == 0) { continue; }

                arc_blend_argb8888((uint32_t *)row + px, stroke_color, final_alpha, false);
            }
        }
    }
}