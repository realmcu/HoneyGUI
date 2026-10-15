/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include <math.h>
#include <stddef.h>
#include "guidef.h"
#include "gui_fb.h"
#include "acc_api.h"
#include "gui_rect.h"
#include "lite_geometry.h"
#include "gui_geometry_cache.h"
#include "gui_geometry_common.h"
#include "gui_geometry_image.h"

/*============================================================================*
 *                           Configuration
 *============================================================================*/
// 1 = Enable Dither by default (Better quality on 565/666 screens)
// 0 = Disable Dither (Maximum speed, banding may appear on gradients)
#define GUI_RECT_ENABLE_DITHER  1

/**
 * 1 = store software-rasterised solid shapes as A8 coverage masks,
 * 0 = always use ARGB8888.
 *
 * A8 reduces cache memory and selects the RGB565 A8 blit path. Keep the switch
 * available because the gain is workload dependent.
 *
 * Overridable from the build:
 *   scons GUI_RECT_ENABLE_A8=0
 */
#ifndef GUI_RECT_ENABLE_A8
#define GUI_RECT_ENABLE_A8  1
#endif

/*============================================================================*
 *                           Types
 *============================================================================*/

GUI_GEOMETRY_DESC_SIZE_CHECK(rect_desc_t);

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/** Descriptor bytes to compare: solid parts stop before the gradient. */
static uint16_t rect_desc_len(const rect_desc_t *desc)
{
    if ((desc->flags & 0xFFFF0000u) == 0u)
    {
        return (uint16_t)offsetof(rect_desc_t, gradient);
    }
    return (uint16_t)sizeof(rect_desc_t);
}

/**
 * Whether this rect fills with a gradient.
 *
 * A solid rect without stroke can be stored as an A8 coverage mask: a mask
 * carries no RGB, so one payload serves every colour of the same shape --
 * which is the point, since "same size, different colour" is otherwise a full
 * cache miss.  A gradient or a stroke rules that out, since they need
 * per-pixel RGB.
 *
 * A translucent colour is fine: its alpha is folded into the mask values, not
 * carried in fg_color_set.  That matters because the two blit paths disagree
 * about fg_color_set's alpha -- acc_sw_raster multiplies it in, a8_2_rgb565
 * discards it -- so it is pinned at 255 and the mask carries everything.
 */
static bool rect_has_gradient(const gui_rounded_rect_t *rect)
{
    return rect->use_gradient && rect->gradient != NULL &&
           rect->gradient->stop_count >= 2;
}

/**
 * Fill in a descriptor for one cached payload.
 *
 * Zeroes first: gui_geometry_cache_acquire() compares descriptors byte for byte, so
 * a padding hole left uninitialised would make two identical rects miss.
 */
static void rect_desc_init(rect_desc_t *desc, const gui_rounded_rect_t *this,
                           gui_rect_part_t part, int size_a, int size_b, int radius,
                           bool is_a8)
{
    memset(desc, 0x00, sizeof(*desc));

    desc->part = (uint32_t)part;
    desc->size_a = size_a;
    desc->size_b = size_b;
    desc->radius = radius;
    desc->flags = this->enable_dither ? 1u : 0u;
    desc->stroke_width = this->stroke_width;
    if (this->stroke_width > 0.0f)
    {
        desc->stroke_color = this->stroke_color.color.argb_full;
    }

    /* RGB is deliberately left out of an A8 key -- that is what lets rects
     * differing only in colour share one mask.  Alpha stays in, because it is
     * folded into the mask values themselves. */
    if (is_a8)
    {
        desc->is_a8 = 1u;
        desc->color = this->color.color.rgba.a;
    }
    else
    {
        desc->color = this->color.color.argb_full;
    }

    /* Only the whole-rect part can carry a gradient; corners are always solid.
     * The direction lives in the high half so that rect_desc_len() can tell
     * from flags alone whether to compare it. */
    if (part == RECT_PART_ROUNDED && rect_has_gradient(this))
    {
        desc->flags |= ((uint32_t)this->gradient_dir + 1u) << 16;
        memcpy(&desc->gradient, this->gradient, sizeof(Gradient));
    }
}

static int get_effective_radius(const gui_rounded_rect_t *rect)
{
    if (rect->radius <= 0 || rect->base.w <= 0 || rect->base.h <= 0)
    {
        return 0;
    }

    int max_radius = _UI_MIN(rect->base.w, rect->base.h) / 2;
    return _UI_MIN(rect->radius, max_radius);
}

static float rounded_rect_distance(float px, float py,
                                   float x, float y, float w, float h, float radius)
{
    float half_w = w * 0.5f;
    float half_h = h * 0.5f;
    float r = LG_CLAMP(radius, 0.0f, LG_MIN(half_w, half_h));
    float qx = fabsf(px - (x + half_w)) - (half_w - r);
    float qy = fabsf(py - (y + half_h)) - (half_h - r);
    float outside_x = LG_MAX(qx, 0.0f);
    float outside_y = LG_MAX(qy, 0.0f);
    float inside = LG_MIN(LG_MAX(qx, qy), 0.0f);

    return inside + sqrtf(outside_x * outside_x + outside_y * outside_y) - r;
}

static float rect_coverage(float distance)
{
    return LG_CLAMP(0.5f - distance, 0.0f, 1.0f);
}

/**
 * Gradient sampled once per raster, so a pixel costs a table lookup instead of
 * a walk over the colour stops.  The gradient position t is linear in x and y
 * for every direction, so the table index is too: kx * x + ky * y + k0.
 */
typedef struct
{
    uint32_t *lut;              /**< NULL: no gradient, or no memory for the table. */
    int n;                      /**< Table entries. */
    int32_t kx;                 /**< Index step per pixel in x, 16.16 fixed point. */
    int32_t ky;                 /**< Index step per pixel in y, 16.16 fixed point. */
    int32_t k0;                 /**< Index at pixel (0, 0) plus rounding, 16.16. */
} rect_lut_t;

static void rect_lut_init(rect_lut_t *l, gui_rounded_rect_t *this, int w, int h)
{
    l->lut = NULL;
    if (!rect_has_gradient(this)) { return; }

    /* t per pixel step along each axis, and the t of pixel (0, 0). */
    float sx = (w > 1) ? 1.0f / (float)(w - 1) : 0.0f;
    float sy = (h > 1) ? 1.0f / (float)(h - 1) : 0.0f;
    float tx, ty, t0;
    int n;

    switch (this->gradient_dir)
    {
    case RECT_GRADIENT_VERTICAL:
        tx = 0.0f; ty = sy; t0 = 0.0f; n = h;
        break;
    case RECT_GRADIENT_DIAGONAL_TL_BR:
        tx = sx * 0.5f; ty = sy * 0.5f; t0 = 0.0f; n = w + h;
        break;
    case RECT_GRADIENT_DIAGONAL_TR_BL:
        tx = -sx * 0.5f; ty = sy * 0.5f; t0 = 0.5f; n = w + h;
        break;
    case RECT_GRADIENT_HORIZONTAL:
    default:
        tx = sx; ty = 0.0f; t0 = 0.0f; n = w;
        break;
    }

    /* One entry per pixel step is as fine as the gradient can show. */
    n = LG_CLAMP(n, 2, 2048);
    l->lut = gui_malloc((size_t)n * sizeof(uint32_t));
    if (l->lut == NULL) { return; }

    for (int i = 0; i < n; i++)
    {
        l->lut[i] = gradient_get_color(this->gradient, (float)i / (float)(n - 1));
    }
    l->n = n;
    l->kx = (int32_t)(tx * (float)(n - 1) * 65536.0f);
    l->ky = (int32_t)(ty * (float)(n - 1) * 65536.0f);
    l->k0 = (int32_t)((t0 * (float)(n - 1) + 0.5f) * 65536.0f);
}

/** Table entry for pixel (x, y), integer only. */
static inline uint32_t rect_lut_at(const rect_lut_t *l, int x, int y)
{
    int32_t i = (l->kx * x + l->ky * y + l->k0) >> 16;
    return l->lut[LG_CLAMP(i, 0, l->n - 1)];
}

static void rect_lut_free(rect_lut_t *l)
{
    if (l->lut != NULL)
    {
        gui_free(l->lut);
        l->lut = NULL;
    }
}

/** Fill colour at (x, y); @p l may be NULL, which samples the gradient directly. */
static uint32_t rect_fill_color_at(gui_rounded_rect_t *this, const rect_lut_t *l,
                                   int x, int y, int w, int h)
{
    if (!rect_has_gradient(this))
    {
        return this->color.color.argb_full;
    }

    uint32_t color;
    if (l != NULL && l->lut != NULL)
    {
        color = rect_lut_at(l, x, y);
    }
    else
    {
        float tx = (w > 1) ? (float)x / (float)(w - 1) : 0.0f;
        float ty = (h > 1) ? (float)y / (float)(h - 1) : 0.0f;
        float t = (this->gradient_dir == RECT_GRADIENT_VERTICAL) ? ty :
                  (this->gradient_dir == RECT_GRADIENT_DIAGONAL_TL_BR) ? (tx + ty) * 0.5f :
                  (this->gradient_dir == RECT_GRADIENT_DIAGONAL_TR_BL) ? ((1.0f - tx) + ty) * 0.5f :
                  tx;
        color = gradient_get_color(this->gradient, t);
    }

    return this->enable_dither ? gui_geometry_dither_argb8888_inline(color, x, y) : color;
}

static void rect_geom_init(rect_geom_t *g, const gui_rounded_rect_t *this, int radius)
{
    g->w = this->base.w;
    g->h = this->base.h;
    g->radius = radius;
    g->stroke = LG_CLAMP(this->stroke_width, 0.0f, LG_MIN((float)g->w, (float)g->h) * 0.5f);
    g->inner_w = (float)g->w - g->stroke * 2.0f;
    g->inner_h = (float)g->h - g->stroke * 2.0f;
    g->inner_radius = LG_MAX((float)radius - g->stroke, 0.0f);
    g->straight = (int)ceilf(LG_MAX((float)radius, g->stroke));
}

/**
 * Coverage of the outer edge and of the stroke's inner edge at (x, y).
 *
 * rounded_rect_distance() only takes a square root inside the corner arcs; on
 * the straight parts of the edges it reduces to a subtraction.
 */
static void rect_coverages(const rect_geom_t *g, int x, int y, float *c_out, float *c_in)
{
    float px = (float)x + 0.5f;
    float py = (float)y + 0.5f;

    *c_out = rect_coverage(rounded_rect_distance(px, py, 0.0f, 0.0f,
                                                 (float)g->w, (float)g->h, (float)g->radius));
    *c_in = *c_out;

    if (g->stroke > 0.0f && *c_out > 0.0f)
    {
        *c_in = 0.0f;
        if (g->inner_w > 0.0f && g->inner_h > 0.0f)
        {
            *c_in = LG_MIN(rect_coverage(rounded_rect_distance(px, py, g->stroke, g->stroke,
                                                               g->inner_w, g->inner_h,
                                                               g->inner_radius)), *c_out);
        }
    }
}

/** One pixel of the rect, fill and stroke merged by gui_geometry_mix_fill_stroke(). */
static uint32_t rect_pixel(gui_rounded_rect_t *this, const rect_geom_t *g,
                           const rect_lut_t *l, int x, int y)
{
    float c_out, c_in;
    rect_coverages(g, x, y, &c_out, &c_in);
    if (c_out <= 0.0f) { return 0u; }

    /* The stroke-only part needs no fill colour, which skips the gradient. */
    uint32_t fill = (c_in > 0.0f) ? rect_fill_color_at(this, l, x, y, g->w, g->h) : 0u;
    return gui_geometry_mix_fill_stroke(fill, this->stroke_color.color.argb_full, c_out, c_in);
}

/** One pixel from 0..255 coverages: integer except on the stroke/fill seam. */
static inline uint32_t rect_mix8(uint32_t fill, uint32_t stroke, uint32_t c_out, uint32_t c_in)
{
    if (c_in == c_out || c_in == 0u)
    {
        uint32_t color = c_in ? fill : stroke;
        if (c_out == 255u) { return color; }
        return ((((color >> 24) * c_out + 127u) / 255u) << 24) | (color & 0x00FFFFFFu);
    }
    return gui_geometry_mix_fill_stroke(fill, stroke, (float)c_out / 255.0f,
                                        (float)c_in / 255.0f);
}

/** Plain per-pixel raster, used only when a table could not be allocated. */
static void rect_raster_slow(gui_rounded_rect_t *this, const rect_geom_t *g,
                             const rect_lut_t *lut, uint8_t *body, bool is_a8,
                             int ox, int oy, int pw, int ph)
{
    uint32_t *pixels = (uint32_t *)body;

    for (int y = 0; y < ph; y++)
    {
        for (int x = 0; x < pw; x++)
        {
            uint32_t color = rect_pixel(this, g, lut, ox + x, oy + y);
            if (is_a8) { body[y * pw + x] = (uint8_t)(color >> 24); }
            else { pixels[y * pw + x] = color; }
        }
    }
}

static void rect_cov_store(const rect_geom_t *g, uint8_t *dst, int x, int y)
{
    float c_out, c_in;
    rect_coverages(g, x, y, &c_out, &c_in);
    dst[0] = (uint8_t)(c_out * 255.0f + 0.5f);
    dst[1] = (uint8_t)(c_in * 255.0f + 0.5f);
}

/**
 * Rasterise the window (ox, oy, pw x ph) of the rect into a payload body: the
 * whole rect for a single buffer, or one corner square for the split.
 *
 * The rect is symmetric about both centre lines, so every coverage it needs is
 * precomputed once as 0..255: an s x s table shared by the four corners and a
 * per-depth table shared by the four straight edges.  Each row is then walked
 * as up to three spans -- left band, middle, right band -- so the middle of a
 * row is a tight loop with no per-pixel classification: a plain fill inside
 * the rect, one constant coverage along the top and bottom bands.  The body
 * comes zeroed, so uncovered pixels are simply skipped.
 */
static void rect_raster(gui_rounded_rect_t *this, const rect_geom_t *g, uint8_t *body,
                        bool is_a8, int ox, int oy, int pw, int ph)
{
    int s = g->straight;
    int w = g->w;
    int h = g->h;
    uint32_t *pixels = (uint32_t *)body;
    uint32_t stroke = this->stroke_color.color.argb_full;
    uint32_t solid = this->color.color.argb_full;
    bool gradient = rect_has_gradient(this);
    bool dither = gradient && this->enable_dither;

    rect_lut_t lut;
    rect_lut_init(&lut, this, w, h);

    /* [0, s*s): corner, top-left orientation; [s*s, s*s + s): straight edges
     * by depth.  Two bytes per entry: outer coverage, then inner. */
    uint8_t *cov = (s > 0) ? gui_malloc((size_t)(s * s + s) * 2u) : NULL;

    if ((s > 0 && cov == NULL) || (gradient && lut.lut == NULL))
    {
        rect_raster_slow(this, g, &lut, body, is_a8, ox, oy, pw, ph);
        if (cov != NULL) { gui_free(cov); }
        rect_lut_free(&lut);
        return;
    }

    uint8_t *edge = (cov != NULL) ? cov + s * s * 2 : NULL;
    if (cov != NULL)
    {
        for (int y = 0; y < s; y++)
        {
            for (int x = 0; x < s; x++)
            {
                rect_cov_store(g, &cov[(y * s + x) * 2], x, y);
            }
        }
        /* Sampled on whichever side has a straight part; every side has the
         * same profile.  Unused when neither has one. */
        for (int i = 0; i < s; i++)
        {
            rect_cov_store(g, &edge[i * 2], (h > 2 * s) ? i : w / 2, (h > 2 * s) ? h / 2 : i);
        }
    }

    int32_t kx = gradient ? lut.kx : 0;
    int lut_max = gradient ? lut.n - 1 : 0;

    /* A full gradient without a stroke needs only the corner coverages. */
    if (gradient && g->stroke <= 0.0f && !is_a8 &&
        ox == 0 && oy == 0 && pw == w && ph == h &&
        s > 0 && w > 2 * s && h > 2 * s)
    {
        for (int y = 0; y < h; y++)
        {
            uint32_t *row = pixels + y * w;
            int dy = LG_MIN(y, h - 1 - y);
            int32_t gi_row = lut.ky * y + lut.k0;
            int32_t gi_left = gi_row;
            int32_t gi_right = gi_row + kx * (w - 1);

            for (int x = 0; x < s; x++, gi_left += kx, gi_right -= kx)
            {
                uint8_t cover = dy < s ? cov[(dy * s + x) * 2] : edge[x * 2];
                if (cover == 0u) { continue; }

                int left = LG_CLAMP(gi_left >> 16, 0, lut_max);
                int right_x = w - 1 - x;
                int right = LG_CLAMP(gi_right >> 16, 0, lut_max);
                uint32_t left_color = lut.lut[left];
                uint32_t right_color = lut.lut[right];
                if (dither)
                {
                    left_color = gui_geometry_dither_argb8888_inline(left_color, x, y);
                    right_color = gui_geometry_dither_argb8888_inline(right_color, right_x, y);
                }
                if (cover != 255u)
                {
                    left_color = ((((left_color >> 24) * cover + 127u) / 255u) << 24) |
                                 (left_color & 0x00FFFFFFu);
                    right_color = ((((right_color >> 24) * cover + 127u) / 255u) << 24) |
                                  (right_color & 0x00FFFFFFu);
                }
                row[x] = left_color;
                row[right_x] = right_color;
            }

            uint8_t center_cover = dy < s ? edge[dy * 2] : 255u;
            if (center_cover == 0u) { continue; }
            int32_t gi = gi_row + kx * s;
            if (center_cover == 255u)
            {
                for (int x = s; x < w - s; x++, gi += kx)
                {
                    uint32_t color = lut.lut[LG_CLAMP(gi >> 16, 0, lut_max)];
                    row[x] = dither ? gui_geometry_dither_argb8888_inline(color, x, y) : color;
                }
            }
            else
            {
                for (int x = s; x < w - s; x++, gi += kx)
                {
                    uint32_t color = lut.lut[LG_CLAMP(gi >> 16, 0, lut_max)];
                    if (dither) { color = gui_geometry_dither_argb8888_inline(color, x, y); }
                    row[x] = ((((color >> 24) * center_cover + 127u) / 255u) << 24) |
                             (color & 0x00FFFFFFu);
                }
            }
        }
        gui_free(cov);
        rect_lut_free(&lut);
        return;
    }

    for (int y = 0; y < ph; y++)
    {
        int ry = oy + y;
        int dy = LG_MIN(ry, h - 1 - ry);                /* depth from top or bottom */
        bool row_mid = dy >= s;
        uint32_t *row32 = pixels + y * pw;
        uint8_t *row8 = body + y * pw;
        int32_t gi_row = gradient ? lut.ky * ry + lut.k0 : 0;
        int x = 0;

        while (x < pw)
        {
            int rx = ox + x;
            int seg_end = (rx < s) ? s : (rx < w - s) ? w - s : w;
            seg_end = LG_MIN(seg_end, ox + pw) - ox;    /* back to window x */
            bool col_mid = rx >= s && rx < w - s;
            int32_t gi = gi_row + kx * rx;

            if (row_mid && col_mid)
            {
                /* Interior: plain fill */
                for (; x < seg_end; x++, rx++, gi += kx)
                {
                    uint32_t color = solid;
                    if (gradient)
                    {
                        color = lut.lut[LG_CLAMP(gi >> 16, 0, lut_max)];
                        if (dither) { color = gui_geometry_dither_argb8888_inline(color, rx, ry); }
                    }
                    if (is_a8) { row8[x] = (uint8_t)(color >> 24); }
                    else { row32[x] = color; }
                }
            }
            else if (col_mid)
            {
                /* Top or bottom band: one coverage for the whole span */
                uint32_t c_out = edge[dy * 2];
                uint32_t c_in = edge[dy * 2 + 1];

                if (c_out == 0u) { x = seg_end; continue; }

                for (; x < seg_end; x++, rx++, gi += kx)
                {
                    uint32_t fill = solid;
                    if (gradient && c_in != 0u)
                    {
                        fill = lut.lut[LG_CLAMP(gi >> 16, 0, lut_max)];
                        if (dither) { fill = gui_geometry_dither_argb8888_inline(fill, rx, ry); }
                    }
                    uint32_t color = rect_mix8(fill, stroke, c_out, c_in);
                    if (is_a8) { row8[x] = (uint8_t)(color >> 24); }
                    else { row32[x] = color; }
                }
            }
            else
            {
                /* Left or right band, or a corner: coverage per column */
                for (; x < seg_end; x++, rx++, gi += kx)
                {
                    int dx = LG_MIN(rx, w - 1 - rx);
                    const uint8_t *c = row_mid ? &edge[dx * 2] : &cov[(dy * s + dx) * 2];
                    if (c[0] == 0u) { continue; }

                    uint32_t fill = solid;
                    if (gradient && c[1] != 0u)
                    {
                        fill = lut.lut[LG_CLAMP(gi >> 16, 0, lut_max)];
                        if (dither) { fill = gui_geometry_dither_argb8888_inline(fill, rx, ry); }
                    }
                    uint32_t color = rect_mix8(fill, stroke, c[0], c[1]);
                    if (is_a8) { row8[x] = (uint8_t)(color >> 24); }
                    else { row32[x] = color; }
                }
            }
        }
    }

    if (cov != NULL)
    {
        gui_free(cov);
    }
    rect_lut_free(&lut);
}

static bool rect_add_solid(gui_rounded_rect_t *this, int x, int y, int w, int h,
                           uint32_t argb)
{
    return gui_geometry_parts_add_solid(&this->parts, GUI_BASE(this)->matrix,
                                        x, y, w, h, argb, this->opacity_value);
}

/**
 * Add a rasterised piece covering (x, y, pw x ph) of the rect, shared through
 * the geometry cache with every rect that would draw the same pixels.
 */
static bool rect_add_raster(gui_rounded_rect_t *this, const rect_geom_t *g,
                            gui_rect_part_t part, int corner_idx,
                            int x, int y, int pw, int ph, bool is_a8)
{
    /* A corner is keyed by its index and side, not by the rect size: every
     * rect with the same radius and stroke has the same corners. */
    rect_desc_t desc;
    rect_desc_init(&desc, this, part, pw, part == RECT_PART_CORNER ? corner_idx : ph,
                   g->radius, is_a8);

    bool is_new = false;
    uint8_t *buffer = gui_geometry_payload_acquire(&desc, rect_desc_len(&desc),
                                                   pw, ph, is_a8, &is_new);
    if (buffer == NULL) { return false; }

    if (is_new)
    {
        rect_raster(this, g, buffer + sizeof(gui_rgb_data_head_t), is_a8, x, y, pw, ph);
    }

    return gui_geometry_parts_add_image(&this->parts, GUI_BASE(this)->matrix, buffer,
                                        this->opacity_value, is_a8, this->color,
                                        x, y, 1, 0, 0, 1);
}

/**
 * Add one straight band as solid runs.
 *
 * Past the corners every row (or column) of a band is uniform along the edge,
 * so one sample per row gives its colour, and rows of equal colour merge into
 * a single rectangle: at most a stroke run, the stroke/fill edge row and a
 * fill run.
 *
 * @param rows  true for the top/bottom bands (runs of rows), false for the
 *              left/right ones (runs of columns).
 * @param first First row or column of the band.
 * @param count Rows or columns in the band.
 */
static bool rect_add_band(gui_rounded_rect_t *this, const rect_geom_t *g,
                          bool rows, int first, int count)
{
    int s = g->straight;
    int run_start = first;
    uint32_t run_color = 0u;

    for (int i = first; i <= first + count; i++)
    {
        uint32_t color = 0u;
        if (i < first + count)
        {
            color = rows ? rect_pixel(this, g, NULL, s, i) : rect_pixel(this, g, NULL, i, s);
        }

        if (i > first && (i == first + count || color != run_color))
        {
            bool ok = rows ?
                      rect_add_solid(this, s, run_start, g->w - 2 * s, i - run_start, run_color) :
                      rect_add_solid(this, run_start, s, i - run_start, g->h - 2 * s, run_color);
            if (!ok) { return false; }
            run_start = i;
        }
        run_color = color;
    }

    return true;
}

/**
 * Build every piece of the rect, see gui_geometry_parts_t.
 *
 * Split: four rasterised corners, solid runs along the four straight bands
 * and one solid centre -- the large areas stay rectangle fills.  Without a
 * stroke the left and right bands are fill colour like the centre, so the
 * three merge into one full-width strip: 4 corners + 3 strips.  Single: one
 * rasterised image of the whole rect, for what the split cannot express (a
 * gradient, a transform the pieces cannot follow, a rect too small to split).
 */
static bool rect_build_parts(gui_rounded_rect_t *this, const rect_geom_t *geom, bool single)
{
    const rect_geom_t g = *geom;
    int s = g.straight;
    uint32_t fill = rect_pixel(this, &g, NULL, g.w / 2, g.h / 2);

    if (single)
    {
        bool is_a8 = g.stroke <= 0.0f &&
                     gui_geometry_use_a8(GUI_RECT_ENABLE_A8, rect_has_gradient(this), false);
        return rect_add_raster(this, &g, RECT_PART_ROUNDED, 0, 0, 0, g.w, g.h, is_a8);
    }

    if (s > 0)
    {
        bool is_a8 = g.stroke <= 0.0f &&
                     gui_geometry_use_split_a8(gui_geometry_use_a8(GUI_RECT_ENABLE_A8, false, false),
                                               this->opacity_value,
                                               GUI_BASE(this)->parent != NULL &&
                                               GUI_BASE(this)->parent->opacity_value == UINT8_MAX);
        int corner_pos[4][2] = { {0, 0}, {g.w - s, 0}, {g.w - s, g.h - s}, {0, g.h - s} };

        for (int i = 0; i < 4; i++)
        {
            if (!rect_add_raster(this, &g, RECT_PART_CORNER, i,
                                 corner_pos[i][0], corner_pos[i][1], s, s, is_a8))
            {
                return false;
            }
        }

        if (!rect_add_band(this, &g, true, 0, s) ||
            !rect_add_band(this, &g, true, g.h - s, s))
        {
            return false;
        }

        if (g.stroke <= 0.0f)
        {
            return rect_add_solid(this, 0, s, g.w, g.h - 2 * s, fill);
        }

        if (!rect_add_band(this, &g, false, 0, s) ||
            !rect_add_band(this, &g, false, g.w - s, s))
        {
            return false;
        }
    }

    return rect_add_solid(this, s, s, g.w - 2 * s, g.h - 2 * s, fill);
}

static void gui_rect_prepare(gui_obj_t *obj)
{
    gui_rounded_rect_t *this = (gui_rounded_rect_t *)obj;
    uint32_t last = this->checksum;

    if (this->base.w <= 0 || this->base.h <= 0)
    {
        gui_geometry_parts_release(&this->parts);
        return;
    }

    int radius = get_effective_radius(this);

    // obj->matrix is already initialized by gui_obj_ctor
    // Don't reinitialize it - it may contain parent transformations (e.g., list scrolling)

    // Detect non-translate parent transform (rotation/scale/skew)
    bool parent_has_non_translate = !matrix_only_translate(obj->matrix);

    // Apply transformations if needed
    bool has_transform = (this->degrees != 0.0f || this->scale_x != 1.0f || this->scale_y != 1.0f ||
                          this->offset_x != 0.0f || this->offset_y != 0.0f);
    gui_matrix_t base_matrix;
    memcpy(&base_matrix, obj->matrix, sizeof(gui_matrix_t));

    if (has_transform)
    {
        float center_x = this->base.w / 2.0f;
        float center_y = this->base.h / 2.0f;

        matrix_translate(this->offset_x, this->offset_y, obj->matrix);
        matrix_translate(center_x, center_y, obj->matrix);
        matrix_scale(this->scale_x, this->scale_y, obj->matrix);
        matrix_rotate(this->degrees, obj->matrix);
        matrix_translate(-center_x, -center_y, obj->matrix);
    }

    /* Uniform scale without rotation: rasterise at the scaled size and blit it
     * 1:1, instead of letting the blit scale the bitmap.  The blit samples
     * nearest-neighbour, which turns the anti-aliased corners into steps.
     *
     * Only the image matrices see this.  For the rest of prepare the widget
     * temporarily takes the scaled size and a translate-only matrix, so the
     * untransformed split applies as is.  obj->matrix and the geometry are
     * restored at the end, so hit testing and children keep the scaled matrix
     * exactly as before. */
    bool bake_scale = false;
    gui_matrix_t scaled_matrix;
    int16_t saved_w = this->base.w;
    int16_t saved_h = this->base.h;
    int saved_radius = this->radius;
    float saved_stroke_width = this->stroke_width;

    if (has_transform && this->degrees == 0.0f && !parent_has_non_translate &&
        this->scale_x > 0.0f && fabsf(this->scale_x - this->scale_y) < 1e-4f &&
        this->scale_x != 1.0f)
    {
        float s = this->scale_x;
        float scaled_w = roundf((float)this->base.w * s);
        float scaled_h = roundf((float)this->base.h * s);

        if (scaled_w >= 1.0f && scaled_h >= 1.0f &&
            scaled_w <= (float)INT16_MAX && scaled_h <= (float)INT16_MAX)
        {
            bake_scale = true;
            memcpy(&scaled_matrix, obj->matrix, sizeof(gui_matrix_t));

            /* Same centre as the scaled matrix maps to, snapped to a whole
             * pixel so the split pieces stay seamless. */
            memcpy(obj->matrix, &base_matrix, sizeof(gui_matrix_t));
            matrix_translate(roundf(this->offset_x + (this->base.w - scaled_w) / 2.0f),
                             roundf(this->offset_y + (this->base.h - scaled_h) / 2.0f),
                             obj->matrix);

            this->base.w = (int16_t)scaled_w;
            this->base.h = (int16_t)scaled_h;
            this->radius = (int)roundf((float)this->radius * s);
            this->stroke_width *= s;
            radius = get_effective_radius(this);
        }
    }

    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LONG, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL, "touch");

    // Calculate checksum only for key properties (exclude pointers)
    uint32_t new_checksum = 2166136261u;
    new_checksum = gui_geometry_hash_update(new_checksum, &this->radius, sizeof(this->radius));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->color, sizeof(this->color));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->stroke_width,
                                            sizeof(this->stroke_width));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->stroke_color,
                                            sizeof(this->stroke_color));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->degrees, sizeof(this->degrees));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->scale_x, sizeof(this->scale_x));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->scale_y, sizeof(this->scale_y));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->offset_x, sizeof(this->offset_x));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->offset_y, sizeof(this->offset_y));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->use_gradient,
                                            sizeof(this->use_gradient));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->enable_dither,
                                            sizeof(this->enable_dither));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->gradient_dir,
                                            sizeof(this->gradient_dir));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->base.w, sizeof(this->base.w));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->base.h, sizeof(this->base.h));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->opacity_value,
                                            sizeof(this->opacity_value));

    // Handle bit-field hidden with temporary variable
    uint32_t hidden_val = obj->hidden;
    new_checksum = gui_geometry_hash_update(new_checksum, &hidden_val, sizeof(hidden_val));

    if (this->gradient != NULL)
    {
        new_checksum = gui_geometry_hash_update(new_checksum, this->gradient, sizeof(Gradient));
    }

    /* Single buffer for what the split cannot express: a gradient varies across
     * the centre, a transform would leave seams between the pieces, and below
     * 2 * straight per side the corners would overlap.  The size rule is a cost
     * trade-off -- see gui_geometry_common.h for why it is expressed in bytes. */
    rect_geom_t geom;
    rect_geom_init(&geom, this, radius);
    bool split_degenerate = (geom.w <= 2 * geom.straight) || (geom.h <= 2 * geom.straight);
    uint64_t rect_area = (uint64_t)(uint16_t)geom.w * (uint16_t)geom.h;
    bool payload_is_a8 = geom.stroke <= 0.0f && !rect_has_gradient(this) &&
                         GUI_RECT_ENABLE_A8;
    bool single = rect_has_gradient(this) || (has_transform && !bake_scale) ||
                  parent_has_non_translate ||
                  (geom.straight > 0 && (split_degenerate ||
                                         !gui_geometry_split_pays_off(rect_area, payload_is_a8)));
    new_checksum = gui_geometry_hash_update(new_checksum, &single, sizeof(single));

    /* Whether the split corners may be A8 also depends on the parent's
     * opacity, which none of the fields above see; hashing it rebuilds the
     * pieces when a fading parent crosses fully opaque. */
    bool parent_opaque = obj->parent != NULL && obj->parent->opacity_value == UINT8_MAX;
    new_checksum = gui_geometry_hash_update(new_checksum, &parent_opaque, sizeof(parent_opaque));

    // All pieces are rebuilt together, so they always match each other
    if (last != new_checksum || this->parts.count == 0)
    {
        gui_geometry_parts_release(&this->parts);
        if (!rect_build_parts(this, &geom, single))
        {
            gui_geometry_parts_release(&this->parts);
        }
        memcpy(&this->last_matrix, obj->matrix, sizeof(gui_matrix_t));
    }
    else if (memcmp(&this->last_matrix, obj->matrix, sizeof(gui_matrix_t)) != 0)
    {
        // Moved only (e.g. list scrolling): re-place the pieces, keep the pixels
        memcpy(&this->last_matrix, obj->matrix, sizeof(gui_matrix_t));
        gui_geometry_parts_place(&this->parts, obj->matrix);
    }

    this->checksum = new_checksum;

    if (bake_scale)
    {
        memcpy(obj->matrix, &scaled_matrix, sizeof(gui_matrix_t));
        this->base.w = saved_w;
        this->base.h = saved_h;
        this->radius = saved_radius;
        this->stroke_width = saved_stroke_width;
    }

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

/** Drawing phase processing */
static void gui_rect_draw(gui_obj_t *obj)
{
    gui_rounded_rect_t *this = (gui_rounded_rect_t *)obj;

    // Update opacity value to consider parent's opacity (like gui_img does)
    gui_geometry_parts_draw(&this->parts,
                            obj->parent->opacity_value * this->opacity_value / UINT8_MAX);
}

static void gui_rect_end(gui_obj_t *obj)
{
    gui_geometry_parts_end(&((gui_rounded_rect_t *)obj)->parts);
}

static void gui_rect_destroy(gui_obj_t *obj)
{
    gui_rounded_rect_t *this = (gui_rounded_rect_t *)obj;

    // Free gradient data
    if (this->gradient != NULL)
    {
        gui_free(this->gradient);
        this->gradient = NULL;
    }

    gui_geometry_parts_release(&this->parts);
}

static void gui_rect_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PRE_PROCESS:
            gui_rect_prepare(obj);
            break;
        case OBJ_PROCESS:
            gui_rect_draw(obj);
            break;
        case OBJ_POST_PROCESS:
            gui_rect_end(obj);
            break;
        case OBJ_DESTROY:
            gui_rect_destroy(obj);
            break;
        default:
            break;
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_rounded_rect_t *gui_rect_create(void *parent, const char *name, int x, int y,
                                    int w, int h,
                                    int radius, gui_color_t color)
{
    if (parent == NULL || !gui_geometry_i16_position_valid(x) || !gui_geometry_i16_position_valid(y) ||
        !gui_geometry_i16_size_valid(w, true) || !gui_geometry_i16_size_valid(h, true))
    {
        GUI_ASSERT(parent != NULL && gui_geometry_i16_position_valid(x) &&
                   gui_geometry_i16_position_valid(y) &&
                   gui_geometry_i16_size_valid(w, true) && gui_geometry_i16_size_valid(h, true));
        return NULL;
    }

    gui_rounded_rect_t *round_rect = gui_malloc(sizeof(gui_rounded_rect_t));
    if (round_rect == NULL)
    {
        GUI_ASSERT(round_rect != NULL);
        return NULL;
    }
    memset(round_rect, 0x00, sizeof(gui_rounded_rect_t));

    round_rect->opacity_value = UINT8_MAX;
    gui_obj_ctor((gui_obj_t *)round_rect, parent, name, x, y, w, h);
    GET_BASE(round_rect)->obj_cb = gui_rect_cb;
    GET_BASE(round_rect)->has_input_process_cb = true;
    GET_BASE(round_rect)->has_pre_process_cb = true;
    GET_BASE(round_rect)->has_process_cb = true;
    GET_BASE(round_rect)->has_post_process_cb = true;
    GET_BASE(round_rect)->has_destroy_cb = true;

    gui_list_init(&(GET_BASE(round_rect)->child_list));
    if ((GET_BASE(round_rect)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(round_rect)->parent)->child_list),
                               &(GET_BASE(round_rect)->brother_list));
    }
    GET_BASE(round_rect)->create_done = true;
    round_rect->radius = _UI_MAX(radius, 0);
    round_rect->color = color;
    round_rect->stroke_width = 0.0f;
    round_rect->stroke_color = gui_rgba(0, 0, 0, 0);
    round_rect->degrees = 0.0f;
    round_rect->scale_x = 1.0f;
    round_rect->scale_y = 1.0f;
    round_rect->offset_x = 0.0f;
    round_rect->offset_y = 0.0f;
    round_rect->gradient = NULL;
    round_rect->use_gradient = false;
    round_rect->enable_dither = (GUI_RECT_ENABLE_DITHER != 0);
    round_rect->gradient_dir = RECT_GRADIENT_HORIZONTAL;

    return round_rect;
}

void gui_rect_set_style(gui_rounded_rect_t *rect,
                        int x, int y, int w, int h,
                        int radius, gui_color_t color)
{
    GUI_ASSERT(rect != NULL);
    if (rect == NULL || !gui_geometry_i16_position_valid(x) || !gui_geometry_i16_position_valid(y) ||
        !gui_geometry_i16_size_valid(w, true) || !gui_geometry_i16_size_valid(h, true))
    {
        return;
    }

    rect->base.x = x;
    rect->base.y = y;
    rect->base.w = w;
    rect->base.h = h;
    rect->radius = _UI_MAX(radius, 0);
    rect->color = color;
}
void gui_rect_set_opacity(gui_rounded_rect_t *rect, uint8_t opacity)
{
    GUI_ASSERT(rect != NULL);
    if (rect == NULL)
    {
        return;
    }

    rect->opacity_value = opacity;
}
void gui_rect_set_position(gui_rounded_rect_t *rect, int x, int y)
{
    GUI_ASSERT(rect != NULL);
    if (rect == NULL || !gui_geometry_i16_position_valid(x) || !gui_geometry_i16_position_valid(y))
    {
        return;
    }

    rect->base.x = x;
    rect->base.y = y;
}

void gui_rect_set_size(gui_rounded_rect_t *rect, int w, int h)
{
    GUI_ASSERT(rect != NULL);
    if (rect == NULL || !gui_geometry_i16_size_valid(w, true) || !gui_geometry_i16_size_valid(h, true))
    {
        return;
    }

    rect->base.w = w;
    rect->base.h = h;
}

void gui_rect_set_radius(gui_rounded_rect_t *rect, int radius)
{
    GUI_ASSERT(rect != NULL);
    if (rect == NULL)
    {
        return;
    }

    rect->radius = _UI_MAX(radius, 0);
}

void gui_rect_set_color(gui_rounded_rect_t *rect, gui_color_t color)
{
    GUI_ASSERT(rect != NULL);
    if (rect == NULL)
    {
        return;
    }

    rect->color = color;
}

void gui_rect_set_stroke(gui_rounded_rect_t *rect, float width, gui_color_t color)
{
    GUI_ASSERT(rect != NULL);
    if (rect == NULL)
    {
        return;
    }

    rect->stroke_width = (width > 0.0f) ? width : 0.0f;
    rect->stroke_color = color;
}

void gui_rect_clear_stroke(gui_rounded_rect_t *rect)
{
    GUI_ASSERT(rect != NULL);
    if (rect == NULL)
    {
        return;
    }

    rect->stroke_width = 0.0f;
}

void gui_rect_on_click(gui_rounded_rect_t *rect, void *callback, void *parameter)
{
    gui_obj_add_event_cb((gui_obj_t *)rect, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED,
                         parameter);
}

void gui_rect_rotate(gui_rounded_rect_t *rect, float degrees)
{
    GUI_ASSERT(rect != NULL);
    rect->degrees = degrees;
}

void gui_rect_scale(gui_rounded_rect_t *rect, float scale_x, float scale_y)
{
    GUI_ASSERT(rect != NULL);
    rect->scale_x = scale_x;
    rect->scale_y = scale_y;
}

void gui_rect_translate(gui_rounded_rect_t *rect, float tx, float ty)
{
    GUI_ASSERT(rect != NULL);
    rect->offset_x = tx;
    rect->offset_y = ty;
}

void gui_rect_set_linear_gradient(gui_rounded_rect_t *rect, gui_rect_gradient_dir_t direction)
{
    GUI_ASSERT(rect != NULL);
    if (rect->gradient == NULL)
    {
        rect->gradient = gui_malloc(sizeof(Gradient));
        if (rect->gradient == NULL) { return; }
    }
    gradient_init(rect->gradient, GRADIENT_LINEAR);
    rect->gradient_dir = direction;
    rect->use_gradient = true;
}

void gui_rect_add_gradient_stop(gui_rounded_rect_t *rect, float position, gui_color_t color)
{
    GUI_ASSERT(rect != NULL);
    if (rect->gradient == NULL)
    {
        gui_rect_set_linear_gradient(rect, RECT_GRADIENT_HORIZONTAL);
    }
    if (rect->gradient != NULL)
    {
        gradient_add_stop(rect->gradient, position, color.color.argb_full);
    }
}

void gui_rect_clear_gradient(gui_rounded_rect_t *rect)
{
    GUI_ASSERT(rect != NULL);
    if (rect->gradient != NULL)
    {
        gui_free(rect->gradient);
        rect->gradient = NULL;
    }
    rect->use_gradient = false;
}

void gui_rect_set_dither(gui_rounded_rect_t *rect, bool enable)
{
    GUI_ASSERT(rect != NULL);
    if (rect == NULL)
    {
        return;
    }

    rect->enable_dither = enable;
}
