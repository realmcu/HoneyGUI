/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                             Header Files
 *============================================================================*/
#include <math.h>
#include <stddef.h>
#include <string.h>
#include "gui_geometry_raster.h"
#include "lite_geometry.h"

/*============================================================================*
 *                         Constants
 *============================================================================*/

/**
 * Coverage step per sub-scanline, so a fully covered pixel lands on 255.
 *
 * Coverage accumulates as a sum of per-sub-scanline fractions, each in 0..1, so
 * the total tops out at GUI_GEOMETRY_RASTER_SAMPLES.
 */
#define RASTER_ALPHA_SCALE (255.0f / (float)GUI_GEOMETRY_RASTER_SAMPLES)

/**
 * Boundaries per row that can move coverage: the two outer edges and the two
 * inner ones.
 */


/*============================================================================*
 *                         Types
 *============================================================================*/

/*============================================================================*
 *                         Functions
 *============================================================================*/


/**
 * @brief floor() and ceil() to int, without leaving the FPU.
 *
 * The library calls are twenty-odd cycles each on a Cortex-M, and the row
 * measurement makes a dozen of them per row, which is a large share of a row's
 * entire cost now that the column walk is short.  A cast truncates toward zero,
 * so the adjustment is only needed on the negative side of each.
 */
static int32_t raster_floor_int(float v)
{
    int32_t i = (int32_t)v;

    return (float)i > v ? i - 1 : i;
}

static int32_t raster_ceil_int(float v)
{
    int32_t i = (int32_t)v;

    return (float)i < v ? i + 1 : i;
}

/**
 * @brief Length of the overlap between two intervals on the x axis.
 *
 * Coverage is an area, so a boundary that falls strictly inside a pixel gives
 * that pixel a fractional value instead of rounding to covered or uncovered.
 * That is what removes the quantisation the supersampled version had.
 */
static float raster_overlap(float a, float b, float lo, float hi)
{
    float start = LG_MAX(a, lo);
    float end = LG_MIN(b, hi);
    float length = end - start;

    return length > 0.0f ? length : 0.0f;
}

/**
 * @brief Horizontal extent of a circle on one sub-scanline.
 */
static void raster_circle_bands(float cx, float cy, float outer_r, float inner_r,
                                float py, raster_bands_t *bands)
{
    float dy = py - cy;

    bands->has_outer = false;
    bands->has_inner = false;

    if (outer_r > 0.0f && fabsf(dy) <= outer_r)
    {
        float half = sqrtf((outer_r - dy) * (outer_r + dy));

        if (half > 0.0f)
        {
            bands->oa = cx - half;
            bands->ob = cx + half;
            bands->has_outer = true;
        }
    }

    if (inner_r > 0.0f && fabsf(dy) < inner_r)
    {
        float half = sqrtf((inner_r - dy) * (inner_r + dy));

        if (half > 0.0f)
        {
            bands->ia = cx - half;
            bands->ib = cx + half;
            bands->has_inner = true;
        }
    }
}

/**
 * @brief Horizontal extent of a rounded rectangle on one sub-scanline.
 *
 * Away from the corner bands the extent is the full box; inside them the
 * quarter circle cuts in by @c r - sqrt(r^2 - t^2), where @c t measures how far
 * the sub-scanline sits past the point the corner arc begins.  Taking the larger
 * of the two corner incursions covers the cases where both apply.
 */
static void raster_rect_bands(float x0, float y0, float w, float h, float r,
                              float py, float *out_a, float *out_b, bool *out_ok)
{
    *out_ok = false;

    if (w <= 0.0f || h <= 0.0f || py < y0 || py > y0 + h)
    {
        return;
    }

    r = LG_MIN(LG_MAX(r, 0.0f), LG_MIN(w, h) * 0.5f);

    float inset = 0.0f;
    float t = y0 + r - py;

    if (t > 0.0f)
    {
        t = LG_MIN(t, r);
        inset = r - sqrtf((r - t) * (r + t));
    }

    float t2 = py - (y0 + h - r);

    if (t2 > 0.0f)
    {
        t2 = LG_MIN(t2, r);
        float inset2 = r - sqrtf((r - t2) * (r + t2));

        if (inset2 > inset)
        {
            inset = inset2;
        }
    }

    *out_a = x0 + inset;
    *out_b = x0 + w - inset;
    *out_ok = *out_a < *out_b;
}

/**
 * @brief Bands for every sub-scanline of one pixel row.
 *
 * One boundary solve per sub-scanline replaces the former per-pixel sampling
 * loop: four square roots per row, independent of how wide the shape is.
 */
static void raster_row_bands(const gui_geometry_path_t *path, uint16_t y,
                             raster_bands_t *bands)
{
    float w = (float)path->width;
    float h = (float)path->height;
    float inset = path->inset;

    for (uint32_t k = 0u; k < GUI_GEOMETRY_RASTER_SAMPLES; k++)
    {
        float py = (float)y + ((float)k + 0.5f) / (float)GUI_GEOMETRY_RASTER_SAMPLES;
        raster_bands_t *b = &bands[k];

        if (path->type == GUI_GEOMETRY_PRIMITIVE_CIRCLE)
        {
            if (inset > 0.0f)
            {
                raster_circle_bands(w * 0.5f, h * 0.5f, path->radius,
                                    path->radius - inset, py, b);
            }
            else
            {
                // inset == 0: only outer circle, no inner circle
                raster_circle_bands(w * 0.5f, h * 0.5f, path->radius,
                                    path->radius, py, b);
            }
            continue;
        }

        raster_rect_bands(0.0f, 0.0f, w, h, path->radius, py,
                          &b->oa, &b->ob, &b->has_outer);

        if (!b->has_outer)
        {
            b->has_inner = false;
            continue;
        }

        b->has_inner = false;

        if (inset > 0.0f)
        {
            float ia = 0.0f;
            float ib = 0.0f;
            bool ok = false;

            raster_rect_bands(inset, inset, w - inset * 2.0f, h - inset * 2.0f,
                              path->radius - inset, py, &ia, &ib, &ok);

            if (ok)
            {
                b->ia = ia;
                b->ib = ib;
                b->has_inner = true;
            }
        }
    }
}

/**
 * @brief Add one zone, clipped to the columns the row actually visits.
 *
 * A zone that ends up empty is dropped rather than clamped to a single column,
 * so an unprovable row really does fall back to walking every column.
 */
static void raster_zone_add(raster_row_span_t *rs, float lo, float hi,
                            int32_t x_from, int32_t x_to)
{
    int32_t first = (int32_t)floorf(lo);
    int32_t last = (int32_t)ceilf(hi);
    uint32_t i;

    if (first < x_from)
    {
        first = x_from;
    }

    if (last > x_to)
    {
        last = x_to;
    }

    if (first > last || rs->zone_count >= RASTER_ZONE_MAX)
    {
        return;
    }

    /* Zones come out in construction order, which is not always ascending once
     * the outer and inner boundaries interleave; the emitter needs them sorted
     * so that every gap it probes lies inside a single uniform stretch. */
    i = rs->zone_count;

    while (i > 0u && rs->zone[i - 1u].lo > first)
    {
        rs->zone[i] = rs->zone[i - 1u];
        i--;
    }

    rs->zone[i].lo = first;
    rs->zone[i].hi = last;
    rs->zone_count++;
}

/**
 * @brief Work out which columns of a row need evaluating, and which can be skipped.
 *
 * Three facts about the row do most of the work: the outer spans bound the only
 * columns that can be painted, a column inside every sub-scanline's inner span
 * cannot be painted at all, and a column inside every outer span but outside all
 * inner spans is painted solid.
 *
 * Those facts also carve the row into stretches of constant coverage: each
 * sub-scanline's interval is constant for every column between two of its own
 * boundary crossings, so between the zones below nothing changes from column to
 * column and a single probe speaks for the whole stretch.  When the row is not
 * convex enough for that argument to hold -- a sub-scanline missing the shape
 * entirely, or the hole present on only some of them -- the whole row becomes
 * one zone and the probe is skipped.
 */
static bool raster_row_measure(const raster_bands_t *bands, uint16_t width,
                               raster_row_span_t *rs)
{
    bool any_outer = false;
    bool all_outer = true;
    bool have_inner = false;
    bool all_inner = true;
    float union_lo = 0.0f;
    float union_hi = 0.0f;
    float full_lo = -INFINITY;
    float full_hi = INFINITY;
    float inter_lo = 0.0f;
    float inter_hi = 0.0f;
    float gap_lo = INFINITY;
    float gap_hi = -INFINITY;

    for (uint32_t k = 0u; k < GUI_GEOMETRY_RASTER_SAMPLES; k++)
    {
        const raster_bands_t *b = &bands[k];

        if (b->has_outer)
        {
            if (!any_outer)
            {
                union_lo = b->oa;
                union_hi = b->ob;
                any_outer = true;
            }
            else
            {
                union_lo = LG_MIN(union_lo, b->oa);
                union_hi = LG_MAX(union_hi, b->ob);
            }

            full_lo = LG_MAX(full_lo, b->oa);
            full_hi = LG_MIN(full_hi, b->ob);
        }
        else
        {
            all_outer = false;
        }

        if (b->has_inner)
        {
            if (!have_inner)
            {
                inter_lo = b->ia;
                inter_hi = b->ib;
                have_inner = true;
            }
            else
            {
                inter_lo = LG_MAX(inter_lo, b->ia);
                inter_hi = LG_MIN(inter_hi, b->ib);
            }

            gap_lo = LG_MIN(gap_lo, b->ia);
            gap_hi = LG_MAX(gap_hi, b->ib);
        }
        else
        {
            all_inner = false;
        }
    }

    if (!any_outer)
    {
        return false;
    }

    int32_t x_from = raster_floor_int(union_lo);
    int32_t x_to = raster_ceil_int(union_hi) - 1;

    if (x_from < 0)
    {
        x_from = 0;
    }

    if (x_to > (int32_t)width - 1)
    {
        x_to = (int32_t)width - 1;
    }

    if (x_from > x_to)
    {
        return false;
    }

    rs->x_from = x_from;
    rs->x_to = x_to;
    rs->hole_from = 0;
    rs->hole_to = -1;

    /* A column lies wholly inside every inner span when it starts at or after
     * the innermost start and ends at or before the innermost end. */
    if (all_inner && have_inner && inter_lo < inter_hi)
    {
        int32_t from = (int32_t)ceilf(inter_lo);
        int32_t to = (int32_t)floorf(inter_hi) - 1;

        from = from < x_from ? x_from : from;
        to = to > x_to ? x_to : to;

        if (from <= to)
        {
            rs->hole_from = from;
            rs->hole_to = to;
        }
    }

    rs->fast_ok = all_outer;
    rs->full_lo = full_lo;
    rs->full_hi = full_hi;
    rs->gap_lo = have_inner ? gap_lo : INFINITY;
    rs->gap_hi = have_inner ? gap_hi : -INFINITY;

    rs->zone_count = 0u;

    if (!all_outer || (have_inner && !all_inner))
    {
        /* Not provably constant anywhere: keep the plain per-column walk. */
        rs->zone[0].lo = x_from;
        rs->zone[0].hi = x_to;
        rs->zone_count = 1u;

        return true;
    }

    /* Widened by one column on the low side so that a boundary landing exactly
     * on a pixel edge cannot fall outside its zone. */
    raster_zone_add(rs, union_lo - 1.0f, full_lo, x_from, x_to);

    if (have_inner)
    {
        raster_zone_add(rs, gap_lo - 1.0f, inter_lo, x_from, x_to);
        raster_zone_add(rs, inter_hi - 1.0f, gap_hi, x_from, x_to);
    }

    raster_zone_add(rs, full_hi - 1.0f, union_hi, x_from, x_to);

    if (rs->zone_count == 0u)
    {
        rs->zone[0].lo = x_from;
        rs->zone[0].hi = x_to;
        rs->zone_count = 1u;
    }

    return true;
}

/**
 * @brief Coverage of one column, as the mean of its sub-scanline overlaps.
 *
 * Exact along x, box filtered along y.  The outer span contributes area and the
 * inner span takes it away, which is the ring the shape describes.
 */
static uint8_t raster_column_alpha(const raster_bands_t *bands,
                                   const raster_row_span_t *rs, int32_t x)
{
    if (x >= rs->hole_from && x <= rs->hole_to)
    {
        return 0u;
    }

    float px0 = (float)x;
    float px1 = px0 + 1.0f;

    if (rs->fast_ok && px0 >= rs->full_lo && px1 <= rs->full_hi &&
        (px1 <= rs->gap_lo || px0 >= rs->gap_hi))
    {
        return 255u;
    }

    float acc = 0.0f;

    /* An edge zone sits well clear of the inner spans, so most columns never
     * need the subtraction.  Testing once for the whole loop keeps the branch
     * out of it. */
    if (px1 <= rs->gap_lo || px0 >= rs->gap_hi)
    {
        for (uint32_t k = 0u; k < GUI_GEOMETRY_RASTER_SAMPLES; k++)
        {
            const raster_bands_t *b = &bands[k];

            if (b->has_outer)
            {
                acc += raster_overlap(b->oa, b->ob, px0, px1);
            }
        }
    }
    else
    {
        for (uint32_t k = 0u; k < GUI_GEOMETRY_RASTER_SAMPLES; k++)
        {
            const raster_bands_t *b = &bands[k];

            if (!b->has_outer)
            {
                continue;
            }

            acc += raster_overlap(b->oa, b->ob, px0, px1);

            if (b->has_inner)
            {
                acc -= raster_overlap(b->ia, b->ib, px0, px1);
            }
        }
    }

    if (acc <= 0.0f)
    {
        return 0u;
    }

    if (acc >= (float)GUI_GEOMETRY_RASTER_SAMPLES)
    {
        return 255u;
    }

    return (uint8_t)(acc * RASTER_ALPHA_SCALE + 0.5f);
}

static void raster_run_store(const raster_run_builder_t *rb,
                             gui_geometry_span_run_t *runs, uint32_t index, bool write)
{
    if (!write)
    {
        return;
    }

    runs[index].x = (uint16_t)rb->x;
    runs[index].length = rb->length;
    runs[index].alpha = rb->alpha;
    runs[index].reserved = 0u;
}

/**
 * @brief End the run being accumulated, if any.
 *
 * @return false when the destination ran out of room, so callers stop early
 *         instead of writing past the payload.
 */
static bool raster_run_close(raster_run_builder_t *rb,
                             gui_geometry_span_run_t *runs,
                             uint32_t base, uint32_t capacity, bool write,
                             uint32_t *emitted, bool *overflow)
{
    if (rb->length == 0u)
    {
        return true;
    }

    if (write && base + *emitted >= capacity)
    {
        *overflow = true;
        return false;
    }

    raster_run_store(rb, runs, base + *emitted, write);
    (*emitted)++;
    rb->length = 0u;

    return true;
}

/**
 * @brief Fold a stretch of one coverage value into the open run.
 *
 * @param length Columns in the stretch; a row has at most 65535 of them, so the
 *               value always fits the field unmerged.
 */
static bool raster_run_add(raster_run_builder_t *rb,
                           gui_geometry_span_run_t *runs,
                           uint32_t base, uint32_t capacity, bool write,
                           uint32_t *emitted, int32_t x, int32_t length,
                           uint8_t alpha, bool *overflow)
{
    if (alpha == 0u)
    {
        return raster_run_close(rb, runs, base, capacity, write, emitted, overflow);
    }

    if (rb->length != 0u && rb->alpha == alpha &&
        (uint32_t)rb->length + (uint32_t)length <= UINT16_MAX)
    {
        rb->length = (uint16_t)((uint32_t)rb->length + (uint32_t)length);
        return true;
    }

    if (!raster_run_close(rb, runs, base, capacity, write, emitted, overflow))
    {
        return false;
    }

    rb->x = x;
    rb->length = (uint16_t)length;
    rb->alpha = alpha;

    return true;
}

/**
 * @brief Walk a row and emit one run per stretch of equal coverage.
 *
 * Columns inside a zone are evaluated one by one, since a boundary crossing
 * there can change the coverage from column to column.  Between zones nothing
 * changes, so one probe covers the whole gap and it collapses to a single run.
 * That is what keeps a row's cost proportional to its boundary count rather than
 * to its width -- the previous version evaluated every column of the row, which
 * for a 326 px ring meant 326 probes to produce eight runs.
 *
 * A run that would exceed the run-length field is closed and reopened instead of
 * failing the whole rasterisation.
 */
static uint32_t raster_emit_row(const raster_bands_t *bands,
                                const raster_row_span_t *rs,
                                gui_geometry_span_run_t *runs, uint32_t base,
                                uint32_t capacity, bool write, bool *overflow)
{
    raster_run_builder_t rb;
    uint32_t emitted = 0u;
    int32_t cursor = rs->x_from;

    rb.x = 0;
    rb.length = 0u;
    rb.alpha = 0u;

    for (uint32_t i = 0u; i < rs->zone_count; i++)
    {
        int32_t zone_lo = rs->zone[i].lo;
        int32_t zone_hi = rs->zone[i].hi;

        if (zone_hi < cursor)
        {
            continue;
        }

        if (zone_lo < cursor)
        {
            zone_lo = cursor;
        }

        if (zone_lo > cursor)
        {
            uint8_t alpha = raster_column_alpha(bands, rs, cursor);

            if (!raster_run_add(&rb, runs, base, capacity, write, &emitted,
                                cursor, zone_lo - cursor, alpha, overflow))
            {
                return emitted;
            }
        }

        for (int32_t x = zone_lo; x <= zone_hi; x++)
        {
            uint8_t alpha = raster_column_alpha(bands, rs, x);

            if (!raster_run_add(&rb, runs, base, capacity, write, &emitted, x, 1,
                                alpha, overflow))
            {
                return emitted;
            }
        }

        cursor = zone_hi + 1;
    }

    if (cursor <= rs->x_to)
    {
        uint8_t alpha = raster_column_alpha(bands, rs, cursor);

        if (!raster_run_add(&rb, runs, base, capacity, write, &emitted, cursor,
                            rs->x_to - cursor + 1, alpha, overflow))
        {
            return emitted;
        }
    }

    if (!raster_run_close(&rb, runs, base, capacity, write, &emitted, overflow))
    {
        return emitted;
    }

    return emitted;
}

/**
 * @brief Run the shared scan, either measuring or filling the destination.
 *
 * Both entry points go through here, so the count a caller sizes its payload
 * from is produced by exactly the same code that later fills it.  The previous
 * split between a counting pass and a building pass had to keep two copies of
 * the run-merging rules byte for byte identical or overrun the buffer.
 *
 * Rows are visited in ascending order, so the run store stays in row order and
 * a partial decode can read a band of rows as one contiguous stretch.  Rows come
 * in mirror pairs -- the shape is symmetric about its own centre row, and the
 * sub-scanline offsets are symmetric about a half, so row @c height-1-y
 * rasterises to the same runs as row @c y -- but exploiting that would mean
 * writing the upper half's runs before the lower half's, which is not worth
 * trading the layout for.
 */
static bool raster_sweep(const gui_geometry_path_t *path,
                         gui_geometry_span_t *spans, bool write,
                         uint32_t *out_runs)
{
    raster_bands_t bands[GUI_GEOMETRY_RASTER_SAMPLES];
    gui_geometry_span_run_t *runs = NULL;
    gui_geometry_span_row_t *rows = NULL;
    uint32_t capacity = 0u;
    uint32_t total = 0u;

    if (write && spans != NULL)
    {
        runs = gui_geometry_span_runs(spans);
        rows = gui_geometry_span_rows(spans);
        capacity = spans->run_count;
    }

    for (uint16_t y = 0u; y < path->height; y++)
    {
        raster_row_span_t rs;
        uint32_t first = total;
        uint32_t row_runs = 0u;

        raster_row_bands(path, y, bands);

        if (raster_row_measure(bands, path->width, &rs))
        {
            bool overflow = false;

            row_runs = raster_emit_row(bands, &rs, runs, first, capacity, write,
                                       &overflow);

            if (overflow || row_runs > UINT16_MAX)
            {
                return false;
            }
        }

        total += row_runs;

        if (rows != NULL)
        {
            rows[y].first_run = first;
            rows[y].run_count = (uint16_t)row_runs;
            rows[y].reserved = 0u;
        }
    }

    *out_runs = total;

    return true;
}

uint64_t gui_geometry_span_bytes(uint32_t run_count, uint16_t height)
{
    return sizeof(gui_geometry_span_t) +
           (uint64_t)height * sizeof(gui_geometry_span_row_t) +
           (uint64_t)run_count * sizeof(gui_geometry_span_run_t);
}

uint32_t gui_geometry_raster_run_count(const gui_geometry_path_t *path)
{
    uint32_t runs = 0u;

    if (path == NULL || path->width == 0u || path->height == 0u)
    {
        return 0u;
    }

    if (!raster_sweep(path, NULL, false, &runs))
    {
        return 0u;
    }

    return runs;
}

bool gui_geometry_raster_build(const gui_geometry_path_t *path,
                               gui_geometry_span_t *spans)
{
    uint32_t runs = 0u;

    if (path == NULL || spans == NULL || path->width == 0u || path->height == 0u)
    {
        return false;
    }

    spans->width = path->width;
    spans->height = path->height;

    if (!raster_sweep(path, spans, true, &runs))
    {
        return false;
    }

    spans->run_count = runs;
    return true;
}
