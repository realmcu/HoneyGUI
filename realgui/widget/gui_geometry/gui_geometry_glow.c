/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include "gui_fb.h"
#include "acc_api.h"
#include "gui_geometry_cache.h"
#include "gui_geometry_common.h"
#include "gui_geometry_distance.h"
#include "gui_geometry_glow_internal.h"
#include "gui_geometry_glow.h"

/*============================================================================*
 *                           Configuration
 *============================================================================*/

/* 1 = split glow into blocks; 0 = one buffer (default, faster).
 * Split saves memory (blocks depend on radius/blur, not area) but costs extra
 * blits. Enable when a one-buffer mask (w+2m)x(h+2m) will not fit. */
#ifndef GUI_GEOMETRY_GLOW_ENABLE_SPLIT
#define GUI_GEOMETRY_GLOW_ENABLE_SPLIT 0
#endif

/*============================================================================*
 *                         Constants
 *============================================================================*/

/* Descriptor tags, clear of RECT_PART_* and CIRCLE_PART_*. */
#define GUI_GEOMETRY_GLOW_PART_RECT_CORNER  0x101u
#define GUI_GEOMETRY_GLOW_PART_RECT_BAND_E  0x102u
#define GUI_GEOMETRY_GLOW_PART_RECT_BAND_S  0x103u
#define GUI_GEOMETRY_GLOW_PART_RECT_FULL    0x104u
#define GUI_GEOMETRY_GLOW_PART_CIRCLE_STRIP 0x105u
#define GUI_GEOMETRY_GLOW_PART_CIRCLE_FULL  0x106u

#define GUI_GEOMETRY_GLOW_PIXEL_BYTES 1u

/* 8 fractional bits: enough for 256 mask levels across blur_px pixels. */
#define GUI_GEOMETRY_GLOW_FIX_BITS 8
#define GUI_GEOMETRY_GLOW_FIX_ONE  (1u << GUI_GEOMETRY_GLOW_FIX_BITS)

/*============================================================================*
 *                            Types
 *============================================================================*/

/* Cache key for one glow payload. Excludes position and colour so widgets
 * with the same shape share a mask. */
typedef struct
{
    uint32_t part;        /* GUI_GEOMETRY_GLOW_PART_* */
    int32_t  size_a;      /* payload width */
    int32_t  size_b;      /* payload height */
    int32_t  radius;      /* corner radius or circle radius; bands 0 */
    uint32_t blur_q;      /* blur in 1/8 px */
    uint32_t edge_alpha;  /* body edge alpha; bands 0 */
    uint32_t algorithm;   /* GUI_GEOMETRY_GLOW_ALGORITHM */
} glow_desc_t;

GUI_GEOMETRY_DESC_SIZE_CHECK(glow_desc_t);

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

uint32_t gui_geometry_glow_margin(uint32_t blur_q)
{
    float blur_px = (float)blur_q / 8.0f;
    return (uint32_t)ceilf(blur_px + 0.5f);
}

/* F(t) = (1 - t)^2, t = d / blur; zero at and beyond blur. */
static uint32_t gui_geometry_glow_falloff(float d, float blur_px)
{
    if (!(d > 0.0f) || blur_px <= 0.0f)
    {
        return 255u;
    }
    if (d >= blur_px)
    {
        return 0u;
    }

    uint32_t t = (uint32_t)(d * (float)GUI_GEOMETRY_GLOW_FIX_ONE / blur_px);
    if (t >= GUI_GEOMETRY_GLOW_FIX_ONE)
    {
        return 0u;
    }

    uint32_t u = GUI_GEOMETRY_GLOW_FIX_ONE - t;
    uint32_t v = (u * u) >> GUI_GEOMETRY_GLOW_FIX_BITS;
    return v > 255u ? 255u : v;
}

/* Mask value for one pixel; body coverage is divided out so the glow
 * composites correctly under SRC_OVER: (1 - c*a) * mask = (1 - c) * F. */
uint8_t gui_geometry_glow_mask_value(float d, float blur_px, uint32_t edge_alpha)
{
    if (d <= -0.5f)
    {
        return 0u;
    }

    float c = gui_geometry_edge_coverage(d);
    if (c >= 1.0f)
    {
        return 0u;
    }

    uint32_t f = gui_geometry_glow_falloff(d, blur_px);
    if (f == 0u)
    {
        return 0u;
    }

    uint32_t c_q8 = (uint32_t)(c * (float)GUI_GEOMETRY_GLOW_FIX_ONE + 0.5f);
    if (c_q8 >= GUI_GEOMETRY_GLOW_FIX_ONE)
    {
        return 0u;
    }

    uint32_t one_minus_c = GUI_GEOMETRY_GLOW_FIX_ONE - c_q8;
    uint32_t ca_q8 = (c_q8 * edge_alpha) / 255u;
    uint32_t divisor = GUI_GEOMETRY_GLOW_FIX_ONE - ca_q8;

    if (divisor == 0u)
    {
        return 0u;
    }

    uint32_t value = (f * one_minus_c) / divisor;
    return (uint8_t)(value > 255u ? 255u : value);
}

/* Value of a band row lying (rows - row_from_outside) - 0.5 outside the body. */
static uint8_t gui_geometry_glow_band_row_value(uint32_t row_from_outside, uint32_t rows,
                                                float blur_px)
{
    return gui_geometry_glow_mask_value((float)(rows - row_from_outside) - 0.5f, blur_px, 255u);
}

static void gui_geometry_glow_desc_init(glow_desc_t *desc, uint32_t part, int size_a, int size_b,
                                        int radius, uint32_t blur_q, uint32_t edge_alpha)
{
    memset(desc, 0x00, sizeof(*desc));
    desc->part = part;
    desc->size_a = size_a;
    desc->size_b = size_b;
    desc->radius = radius;
    desc->blur_q = blur_q;
    desc->edge_alpha = edge_alpha;
    desc->algorithm = GUI_GEOMETRY_GLOW_ALGORITHM;
}

/* Acquire an A8 payload; is_new set when caller must fill it. */
static uint8_t *gui_geometry_glow_acquire(glow_desc_t *desc, int w, int h, bool *is_new)
{
    return gui_geometry_payload_acquire(desc, (uint16_t)sizeof(*desc), w, h, true, is_new);
}

/* Fill a rect band: each row is one value (distance depends only on row). */
static void gui_geometry_glow_fill_rect_band(uint8_t *body, int w, int h, uint32_t blur_q)
{
    float blur_px = (float)blur_q / 8.0f;

    for (int y = 0; y < h; y++)
    {
        uint8_t value = gui_geometry_glow_band_row_value((uint32_t)y, (uint32_t)h, blur_px);
        memset(body + (size_t)y * (size_t)w, value, (size_t)w);
    }
}

/* Fill the circle pinwheel strip; local centre is (r + m, r + m). */
static void gui_geometry_glow_fill_circle_strip(uint8_t *body, int w, int h, int radius,
                                                int m, uint32_t blur_q, uint32_t edge_alpha)
{
    float blur_px = (float)blur_q / 8.0f;
    float cx = (float)(radius + m);
    float far = (float)(radius + m);

    for (int y = 0; y < h; y++)
    {
        uint8_t *row = body + (size_t)y * (size_t)w;

        for (int x = 0; x < w; x++)
        {
            float dx = (float)x + 0.5f - cx;
            float dy = (float)y + 0.5f - cx;
            uint8_t value = 0u;

            if (dx * dx + dy * dy < far * far)
            {
                value = gui_geometry_glow_mask_value(sqrtf(dx * dx + dy * dy) - (float)radius,
                                                     blur_px, edge_alpha);
            }
            row[x] = value;
        }
    }
}

/* Reverse one row of n bytes. */
static void gui_geometry_glow_reverse_row(uint8_t *row, int n)
{
    for (int i = 0, j = n - 1; i < j; i++, j--)
    {
        uint8_t t = row[i];
        row[i] = row[j];
        row[j] = t;
    }
}

/* Fill one rect corner; other three are mirrors. stride = row length of the
 * destination buffer (corner width for split, full width for single). */
static void gui_geometry_glow_fill_rect_corner(uint8_t *base, int stride, int size,
                                               int radius, uint32_t blur_q, uint32_t edge_alpha)
{
    float blur_px = (float)blur_q / 8.0f;
    int m = (int)gui_geometry_glow_margin(blur_q);
    float side = (float)(2 * (m + radius));

    for (int y = 0; y < size; y++)
    {
        uint8_t *row = base + (size_t)y * (size_t)stride;

        for (int x = 0; x < size; x++)
        {
            float d = gui_geometry_rounded_rect_distance((float)x + 0.5f - (float)m,
                                                         (float)y + 0.5f - (float)m,
                                                         0.0f, 0.0f, side, side,
                                                         (float)radius);
            row[x] = gui_geometry_glow_mask_value(d, blur_px, edge_alpha);
        }
    }
}

/* Mask value for the middle run of a row outside the body; 0 inside. */
static uint8_t gui_geometry_glow_run_value(int row_outside, float blur_px)
{
    if (row_outside <= 0)
    {
        return 0u;
    }

    return gui_geometry_glow_mask_value((float)row_outside - 0.5f, blur_px, 255u);
}

/* Build whole-buffer rect glow: top-left corner + mirrors + row copies.
 * Buffer is (w+2m) x (h+2m), body inset by m. Returns false if corners overlap. */
bool gui_geometry_glow_build_rect_regional(uint8_t *body, int w, int h,
                                           int body_w, int body_h,
                                           int radius, int m, uint32_t blur_q,
                                           uint32_t edge_alpha)
{
    float blur_px = (float)blur_q / 8.0f;
    int c = radius + m;
    int run = body_w - 2 * radius;

    if (c <= 0 || c * 2 > w || c * 2 > h || run < 0)
    {
        return false;
    }

    gui_geometry_glow_fill_rect_corner(body, w, c, radius, blur_q, edge_alpha);

    /* Top band: rows with a corner on each side. */
    for (int y = 0; y < c; y++)
    {
        uint8_t *row = body + (size_t)y * (size_t)w;
        int outside = (y < m) ? (m - y) : 0;

        memset(row + c, gui_geometry_glow_run_value(outside, blur_px), (size_t)run);
        memcpy(row + c + run, row, (size_t)c);
        gui_geometry_glow_reverse_row(row + c + run, c);
    }

    /* Middle rows: distance depends on x alone, so build one and copy. */
    if (h - 2 * c > 0)
    {
        uint8_t *mid = body + (size_t)c * (size_t)w;

        for (int x = 0; x < c; x++)
        {
            float d = gui_geometry_rounded_rect_distance((float)x + 0.5f - (float)m,
                                                         (float)radius + 0.5f,
                                                         0.0f, 0.0f,
                                                         (float)body_w, (float)body_h,
                                                         (float)radius);
            mid[x] = gui_geometry_glow_mask_value(d, blur_px, edge_alpha);
        }
        memset(mid + c, 0, (size_t)run);
        memcpy(mid + c + run, mid, (size_t)c);
        gui_geometry_glow_reverse_row(mid + c + run, c);

        for (int y = c + 1; y < h - c; y++)
        {
            memcpy(body + (size_t)y * (size_t)w, mid, (size_t)w);
        }
    }

    /* Bottom band: mirror of the top. */
    for (int y = h - c; y < h; y++)
    {
        memcpy(body + (size_t)y * (size_t)w, body + (size_t)(h - 1 - y) * (size_t)w,
               (size_t)w);
    }

    return true;
}

/* Build whole-buffer circle glow from one quadrant + mirrors.
 * Requires the buffer centred on the buffer centre line (c*2 == w == h). */
bool gui_geometry_glow_build_circle_regional(uint8_t *body, int w, int h,
                                             int radius, int m,
                                             uint32_t blur_q, uint32_t edge_alpha)
{
    float blur_px = (float)blur_q / 8.0f;
    int half_w = w / 2;
    int half_h = h / 2;
    int c = radius + m;
    float outer = (float)radius + blur_px;
    float inner = (float)radius - 0.5f;

    if (c * 2 != w || c * 2 != h)
    {
        return false;
    }

    for (int y = 0; y < half_h; y++)
    {
        float dy = (float)y + 0.5f - (float)c;
        float dy2 = dy * dy;
        uint8_t *row = body + (size_t)y * (size_t)w;

        /* Radii bounds, widened by one pixel to avoid skipping pixels. */
        int xo = (int)sqrtf(LG_MAX(0.0f, outer * outer - dy2)) + 1;
        int xi = (int)sqrtf(LG_MAX(0.0f, inner * inner - dy2)) - 1;

        if (xo > half_w) { xo = half_w; }
        if (xi < 0) { xi = 0; }

        memset(row, 0, (size_t)w);

        int x_from = half_w - 1 - xo;
        int x_to = half_w - 1 - xi;

        if (x_from < 0) { x_from = 0; }

        for (int x = x_from; x <= x_to; x++)
        {
            float dx = (float)x + 0.5f - (float)c;

            row[x] = gui_geometry_glow_mask_value(sqrtf(dx * dx + dy2) - (float)radius,
                                                  blur_px, edge_alpha);
        }

        for (int x = half_w; x < w; x++)
        {
            row[x] = row[w - 1 - x];
        }

        memcpy(body + (size_t)(h - 1 - y) * (size_t)w, row, (size_t)w);
    }

    return true;
}

/* Attach a cached payload to the piece list; takes over the caller's ref. */
static bool gui_geometry_glow_add_piece(gui_geometry_glow_t *glow, const gui_matrix_t *matrix,
                                        uint8_t *payload, gui_color_t color, uint8_t opacity,
                                        int x, int y, int xx, int xy, int yx, int yy)
{
    return gui_geometry_parts_add_image(&glow->parts, matrix, payload, opacity,
                                        true, color, x, y, xx, xy, yx, yy);
}

/* Add one piece taking its own reference: every owner holds a ref. */
static bool gui_geometry_glow_add_piece_ref(gui_geometry_glow_t *glow, const gui_matrix_t *matrix,
                                            uint8_t *payload, gui_color_t color, uint8_t opacity,
                                            int x, int y, int xx, int xy, int yx, int yy)
{
    gui_geometry_cache_addref(payload);
    return gui_geometry_glow_add_piece(glow, matrix, payload, color, opacity, x, y, xx, xy, yx, yy);
}

/* Lay out rect glow: one corner + horizontal band + optional vertical band,
 * each mirrored/turned by its piece matrix. Positions relative to body. */
static bool gui_geometry_glow_build_rect(gui_geometry_glow_t *glow,
                                         const gui_geometry_glow_input_t *in,
                                         const gui_matrix_t *matrix, gui_color_t color, uint8_t opacity)
{
    int m = (int)gui_geometry_glow_margin(in->blur_q);
    int r = in->radius;
    int corner = r + m;
    int band_h = in->w - 2 * r;
    int band_v = in->h - 2 * r;
    uint8_t *band = NULL;
    bool band_owned = false;

    if (corner > 0)
    {
        glow_desc_t desc;
        bool is_new = false;

        gui_geometry_glow_desc_init(&desc, GUI_GEOMETRY_GLOW_PART_RECT_CORNER, corner, corner, r,
                                    in->blur_q, in->edge_alpha);
        uint8_t *payload = gui_geometry_glow_acquire(&desc, corner, corner, &is_new);
        if (payload == NULL)
        {
            return false;
        }
        if (is_new)
        {
            gui_geometry_glow_fill_rect_corner(payload + sizeof(gui_rgb_data_head_t),
                                               corner, corner, r,
                                               in->blur_q, in->edge_alpha);
        }

        /* TL, TR, BR, BL. */
        const int t[4][6] =
        {
            {-m,                -m,                 1, 0, 0,  1},
            {in->w + m - 1,     -m,                -1, 0, 0,  1},
            {in->w + m - 1,      in->h + m - 1,    -1, 0, 0, -1},
            {-m,                 in->h + m - 1,     1, 0, 0, -1},
        };

        for (int i = 0; i < 4; i++)
        {
            if (!gui_geometry_glow_add_piece_ref(glow, matrix, payload, color, opacity,
                                                 t[i][0], t[i][1], t[i][2], t[i][3], t[i][4], t[i][5]))
            {
                gui_geometry_cache_release(payload);
                return false;
            }
        }

        gui_geometry_cache_release(payload);
    }

    if (band_h > 0)
    {
        glow_desc_t desc;
        bool must_render = false;

        gui_geometry_glow_desc_init(&desc, GUI_GEOMETRY_GLOW_PART_RECT_BAND_E, band_h, m, 0,
                                    in->blur_q, 0u);

        uint32_t size = (uint32_t)band_h * (uint32_t)m * GUI_GEOMETRY_GLOW_PIXEL_BYTES +
                        (uint32_t)sizeof(gui_rgb_data_head_t);

        band = gui_geometry_cache_refresh_resizable(glow->band, &desc,
                                                    (uint16_t)sizeof(desc), size,
                                                    &must_render);
        if (band == NULL)
        {
            return false;
        }
        if (must_render)
        {
            gui_geometry_data_head_set((gui_rgb_data_head_t *)band, (int16_t)band_h,
                                       (int16_t)m, A8);
            gui_geometry_glow_fill_rect_band(band + sizeof(gui_rgb_data_head_t), band_h, m,
                                             in->blur_q);
        }

        glow->band = band;
        band_owned = true;

        if (!gui_geometry_glow_add_piece_ref(glow, matrix, band, color, opacity,
                                             r, -m, 1, 0, 0, 1) ||
            !gui_geometry_glow_add_piece_ref(glow, matrix, band, color, opacity,
                                             r, in->h + m - 1, 1, 0, 0, -1))
        {
            return false;
        }
    }

    if (band_v > 0)
    {
        uint8_t *vertical;
        bool vertical_owned = false;

        if (band_owned && band_v == band_h)
        {
            vertical = band;
        }
        else
        {
            glow_desc_t desc;
            bool is_new = false;

            gui_geometry_glow_desc_init(&desc, GUI_GEOMETRY_GLOW_PART_RECT_BAND_S, band_v, m, 0,
                                        in->blur_q, 0u);
            vertical = gui_geometry_glow_acquire(&desc, band_v, m, &is_new);
            if (vertical == NULL)
            {
                return false;
            }
            if (is_new)
            {
                gui_geometry_glow_fill_rect_band(vertical + sizeof(gui_rgb_data_head_t), band_v, m,
                                                 in->blur_q);
            }
            vertical_owned = true;
        }

        bool ok = gui_geometry_glow_add_piece_ref(glow, matrix, vertical, color, opacity,
                                                  -m, r, 0, 1, 1, 0) &&
                  gui_geometry_glow_add_piece_ref(glow, matrix, vertical, color, opacity,
                                                  in->w + m - 1, r, 0, -1, 1, 0);

        if (vertical_owned)
        {
            gui_geometry_cache_release(vertical);
        }

        if (!ok)
        {
            return false;
        }
    }

    return true;
}

/* Lay out circle glow as one strip turned four times (empty centre). */
static bool gui_geometry_glow_build_circle(gui_geometry_glow_t *glow,
                                           const gui_geometry_glow_input_t *in,
                                           const gui_matrix_t *matrix, gui_color_t color, uint8_t opacity)
{
    int m = (int)gui_geometry_glow_margin(in->blur_q);
    int r = in->radius;
    int half = (int)floorf(((float)r - 0.5f) * 0.70710678118654752440f);
    int band = r + m - half;
    int strip_w = 2 * half + band;
    int outer = 2 * r + m - 1;
    bool is_new = false;
    glow_desc_t desc;

    if (half < 0 || band <= 0 || strip_w <= 0)
    {
        return false;
    }

    gui_geometry_glow_desc_init(&desc, GUI_GEOMETRY_GLOW_PART_CIRCLE_STRIP, strip_w, band, r,
                                in->blur_q, in->edge_alpha);
    uint8_t *payload = gui_geometry_glow_acquire(&desc, strip_w, band, &is_new);
    if (payload == NULL)
    {
        return false;
    }
    if (is_new)
    {
        gui_geometry_glow_fill_circle_strip(payload + sizeof(gui_rgb_data_head_t), strip_w, band,
                                            r, m, in->blur_q, in->edge_alpha);
    }

    /* Top strip, then quarter turns about the centre. */
    const int t[4][6] =
    {
        {-m,    -m,     1,  0, 0,  1},
        {outer, -m,     0, -1, 1,  0},
        {outer, outer, -1,  0, 0, -1},
        {-m,    outer,  0,  1, -1, 0},
    };

    for (int i = 0; i < 4; i++)
    {
        if (!gui_geometry_glow_add_piece_ref(glow, matrix, payload, color, opacity,
                                             t[i][0], t[i][1], t[i][2], t[i][3], t[i][4], t[i][5]))
        {
            gui_geometry_cache_release(payload);
            return false;
        }
    }

    gui_geometry_cache_release(payload);

    return true;
}

/* Single-buffer path: for transforms the pieces cannot follow, or small shapes. */
static bool gui_geometry_glow_build_single(gui_geometry_glow_t *glow,
                                           const gui_geometry_glow_input_t *in,
                                           const gui_matrix_t *matrix, gui_color_t color, uint8_t opacity)
{
    int m = (int)gui_geometry_glow_margin(in->blur_q);
    int w = in->w + 2 * m;
    int h = in->h + 2 * m;
    glow_desc_t desc;
    uint32_t size = 0;

    /* The radius arrives already clamped by prepare(); every builder and the
     * descriptor read the same value from there. */
    int radius = in->radius;

    if (!gui_geometry_image_buffer_size(w, h, GUI_GEOMETRY_GLOW_PIXEL_BYTES, &size))
    {
        return false;
    }

#if GUI_GEOMETRY_GLOW_SINGLE_LIMIT > 0
    if (size > GUI_GEOMETRY_GLOW_SINGLE_LIMIT)
    {
        return false;
    }
#endif

    gui_geometry_glow_desc_init(&desc, in->shape == GUI_GEOMETRY_GLOW_SHAPE_CIRCLE ?
                                GUI_GEOMETRY_GLOW_PART_CIRCLE_FULL : GUI_GEOMETRY_GLOW_PART_RECT_FULL,
                                w, h, radius, in->blur_q, in->edge_alpha);

    bool must_render = false;
    uint8_t *payload;

    if (glow->single == NULL)
    {
        bool is_new = false;

        payload = gui_geometry_glow_acquire(&desc, w, h, &is_new);
        must_render = is_new;
    }
    else
    {
        payload = gui_geometry_cache_refresh_resizable(glow->single, &desc,
                                                       (uint16_t)sizeof(desc), size,
                                                       &must_render);
    }

    if (payload == NULL)
    {
        return false;
    }
    glow->single = payload;

    if (must_render)
    {
        gui_geometry_data_head_set((gui_rgb_data_head_t *)payload, (int16_t)w,
                                   (int16_t)h, A8);

        uint8_t *body = payload + sizeof(gui_rgb_data_head_t);
        bool filled = (in->shape == GUI_GEOMETRY_GLOW_SHAPE_CIRCLE) ?
                      gui_geometry_glow_build_circle_regional(body, w, h, radius, m,
                                                              in->blur_q, in->edge_alpha) :
                      gui_geometry_glow_build_rect_regional(body, w, h, in->w, in->h,
                                                            radius, m, in->blur_q,
                                                            in->edge_alpha);

        if (!filled)
        {
            return false;
        }
    }

    return gui_geometry_glow_add_piece_ref(glow, matrix, payload, color, opacity,
                                           -m, -m, 1, 0, 0, 1);
}

/* Drop the pieces, keeping the payload refs glow itself owns (for rebuild). */
static void gui_geometry_glow_release_pieces(gui_geometry_glow_t *glow)
{
    gui_geometry_parts_release(&glow->parts);
}

/* Drop the pieces and the payloads this widget holds. */
static void gui_geometry_glow_release_all(gui_geometry_glow_t *glow)
{
    gui_geometry_parts_release(&glow->parts);
    gui_geometry_cache_release(glow->band);
    gui_geometry_cache_release(glow->single);
    glow->band = NULL;
    glow->single = NULL;
}

/* Union of every piece's target rectangle, empty when there is none. */
static gui_rect_t gui_geometry_glow_target_rect(const gui_geometry_glow_t *glow)
{
    gui_rect_t rect = {0, 0, -1, -1};

    for (uint8_t i = 0; i < glow->parts.count; i++)
    {
        const draw_img_t *img = glow->parts.part[i].img;

        if (img == NULL || img->img_target_w <= 0 || img->img_target_h <= 0)
        {
            continue;
        }

        int16_t x1 = img->img_target_x;
        int16_t y1 = img->img_target_y;
        int16_t x2 = (int16_t)(x1 + img->img_target_w - 1);
        int16_t y2 = (int16_t)(y1 + img->img_target_h - 1);

        if (rect.x2 < rect.x1)
        {
            rect = (gui_rect_t) {x1, y1, x2, y2};
            continue;
        }

        rect.x1 = _UI_MIN(rect.x1, x1);
        rect.y1 = _UI_MIN(rect.y1, y1);
        rect.x2 = _UI_MAX(rect.x2, x2);
        rect.y2 = _UI_MAX(rect.y2, y2);
    }

    return rect;
}

static bool gui_geometry_glow_rect_valid(const gui_rect_t *rect)
{
    return rect->x2 >= rect->x1 && rect->y2 >= rect->y1;
}

static void gui_geometry_glow_forget_last_rect(gui_rect_t *rect)
{
    *rect = (gui_rect_t) {0, 0, -1, -1};
}

/* Track the old rect but request a full frame: the dirty-region path is not
 * exercised elsewhere in this tree, and enabling it for glow alone drops bodies. */
static void gui_geometry_glow_dirty_last_rect(gui_rect_t *rect)
{
    gui_geometry_glow_forget_last_rect(rect);
}

/* Push the current colour into every piece's fg_color_set. */
static void gui_geometry_glow_apply_color(gui_geometry_glow_t *glow, gui_color_t color)
{
    uint32_t fg = 0xFF000000u | (color.color.argb_full & 0x00FFFFFFu);

    for (uint8_t i = 0; i < glow->parts.count; i++)
    {
        if (glow->parts.part[i].img != NULL)
        {
            glow->parts.part[i].img->fg_color_set = fg;
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

uint32_t gui_geometry_glow_blur_q(float blur_px)
{
    long q = lroundf(blur_px * 8.0f);

    if (q < 8)
    {
        q = 8;
    }
    else if (q > (long)GUI_GEOMETRY_GLOW_MAX_BLUR * 8l)
    {
        q = (long)GUI_GEOMETRY_GLOW_MAX_BLUR * 8l;
    }

    return (uint32_t)q;
}

void gui_geometry_glow_prepare(gui_geometry_glow_t **slot, const gui_geometry_glow_input_t *in,
                               const gui_matrix_t *matrix, bool moved, gui_color_t color)
{
    if (slot == NULL)
    {
        return;
    }

    gui_geometry_glow_t *glow = *slot;

    if (in == NULL)
    {
        if (glow != NULL)
        {
            bool had_area = gui_geometry_glow_rect_valid(&glow->last_rect);

            gui_geometry_glow_dirty_last_rect(&glow->last_rect);
            gui_geometry_glow_release_all(glow);
            glow->built_valid = 0;
            glow->suppressed = 0;

            if (had_area)
            {
                gui_fb_change();
            }
        }
        return;
    }

    if (glow == NULL)
    {
        glow = gui_malloc(sizeof(*glow));
        if (glow == NULL)
        {
            return;
        }
        memset(glow, 0x00, sizeof(*glow));
        glow->last_rect = (gui_rect_t) {0, 0, -1, -1};
        *slot = glow;
    }

    /* Normalise the radius once, here, so all three builders and the cached
     * `built` copy work from the same value.  The widget clamps its own radius
     * already; this catches a caller that bypassed that, and without it the
     * split layout can place four corners that overlap each other. */
    gui_geometry_glow_input_t clamped = *in;

    if (clamped.radius < 0)
    {
        clamped.radius = 0;
    }
    if (clamped.radius * 2 > _UI_MIN(clamped.w, clamped.h))
    {
        clamped.radius = _UI_MIN(clamped.w, clamped.h) / 2;
    }

    in = &clamped;

    bool changed = !glow->built_valid || memcmp(&glow->built, in, sizeof(*in)) != 0;
    bool color_changed = glow->last_color != color.color.argb_full;

    if (!changed && glow->suppressed)
    {
        if (moved)
        {
            gui_geometry_parts_place(&glow->parts, matrix);
        }
        return;
    }

    bool rebuilt = false;

    glow->last_color = color.color.argb_full;

    if (changed)
    {
        bool want_single;

#if GUI_GEOMETRY_GLOW_ENABLE_SPLIT
        want_single = (in->single != 0u);
#else
        want_single = true;
#endif

        gui_geometry_glow_dirty_last_rect(&glow->last_rect);
        gui_geometry_glow_release_pieces(glow);

        if (!want_single)
        {
            gui_geometry_cache_release(glow->single);
            glow->single = NULL;
        }
        else
        {
            gui_geometry_cache_release(glow->band);
            glow->band = NULL;
        }

        glow->suppressed = 0;

        bool ok;
#if GUI_GEOMETRY_GLOW_ENABLE_SPLIT
        if (!in->single)
        {
            ok = (in->shape == GUI_GEOMETRY_GLOW_SHAPE_CIRCLE) ?
                 gui_geometry_glow_build_circle(glow, in, matrix, color, UINT8_MAX) :
                 gui_geometry_glow_build_rect(glow, in, matrix, color, UINT8_MAX);
        }
        else
#endif
        {
            ok = gui_geometry_glow_build_single(glow, in, matrix, color, UINT8_MAX);
        }

        glow->built = *in;
        glow->built_valid = 1;
        rebuilt = ok;

        if (!ok)
        {
            gui_geometry_glow_release_pieces(glow);
            glow->suppressed = 1;
        }
    }
    else
    {
        if (moved)
        {
            gui_geometry_parts_place(&glow->parts, matrix);
        }
        if (color_changed)
        {
            gui_geometry_glow_apply_color(glow, color);
        }
    }

    gui_rect_t next = gui_geometry_glow_target_rect(glow);
    if (gui_geometry_glow_rect_valid(&next) &&
        (!gui_geometry_glow_rect_valid(&glow->last_rect) ||
         glow->last_rect.x1 != next.x1 || glow->last_rect.y1 != next.y1 ||
         glow->last_rect.x2 != next.x2 || glow->last_rect.y2 != next.y2))
    {
        gui_geometry_glow_dirty_last_rect(&glow->last_rect);
        glow->last_rect = next;
    }

    if (rebuilt || color_changed)
    {
        gui_fb_change();
    }
}

void gui_geometry_glow_draw(gui_geometry_glow_t *glow, uint8_t parent_opacity,
                            uint8_t widget_opacity)
{
    if (glow == NULL || glow->parts.count == 0)
    {
        return;
    }
    uint32_t op = (uint32_t)parent_opacity * (uint32_t)widget_opacity *
                  (uint32_t)((glow->last_color >> 24) & 0xFFu);

    if (op == 0u)
    {
        return;
    }

    gui_geometry_parts_draw(&glow->parts, (uint8_t)((op + 32512u) / 65025u));
}

void gui_geometry_glow_free(gui_geometry_glow_t **slot)
{
    if (slot == NULL || *slot == NULL)
    {
        return;
    }

    gui_geometry_glow_t *glow = *slot;
    bool had_area = gui_geometry_glow_rect_valid(&glow->last_rect);

    gui_geometry_glow_dirty_last_rect(&glow->last_rect);
    gui_geometry_glow_release_all(glow);

    gui_free(glow);
    *slot = NULL;
    if (had_area)
    {
        gui_fb_change();
    }
}