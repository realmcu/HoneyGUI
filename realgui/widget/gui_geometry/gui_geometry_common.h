/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __GUI_GEOMETRY_COMMON_H__
#define __GUI_GEOMETRY_COMMON_H__

#include <stddef.h>
#include <stdint.h>

#define GUI_GEOMETRY_HASH_INIT 2166136261u

static inline uint32_t gui_geometry_hash_update(uint32_t seed, const void *data, size_t len)
{
    const uint8_t *bytes = (const uint8_t *)data;

    for (size_t i = 0; i < len; i++)
    {
        seed ^= bytes[i];
        seed *= 16777619u;
    }

    return seed;
}

static inline bool gui_geometry_i16_position_valid(int64_t value)
{
    return value >= INT16_MIN && value <= INT16_MAX;
}

static inline bool gui_geometry_i16_size_valid(int64_t value, bool allow_zero)
{
    return value <= INT16_MAX && (allow_zero ? value >= 0 : value > 0);
}

/*---------------------------------------------------------------------------*
 * Split-rendering size budget
 *
 * The circle and rounded-rect widgets used to spell this as a bare pixel
 * count (10000) inside their local need_single_buffer expression.  That count
 * is not a memory budget: measured against the two representations, the
 * memory break-even between one w*h buffer and the split representation
 * (one centre/corner payload group plus five draw_img_t wrappers) lands
 * around 20x20 px, roughly 20x below the threshold.  What the threshold
 * actually buys is the fixed per-piece cost of the split path -- four matrix
 * inverses, five cache lookups and five blit submissions instead of one.
 *
 * It is expressed in bytes so the rule keeps its meaning when the payload is
 * not A8: those 10000 pixels cost 10 KB as an A8 coverage mask but 39 KB as
 * ARGB8888, and it is the bytes -- not the pixels -- that the single buffer
 * occupies.  With A8 enabled (the default for both widgets) the byte and
 * pixel formulations coincide, so the default build behaves as before; the
 * rule only tightens when a widget falls back to ARGB, where it now caps the
 * worst-case single allocation at the budget instead of 4x the budget.
 *---------------------------------------------------------------------------*/
#define GUI_GEOMETRY_SPLIT_BYTE_BUDGET 10000u

static inline uint32_t gui_geometry_payload_pixel_bytes(bool is_a8)
{
    return is_a8 ? 1u : 4u;
}

/* True when a payload of `area` pixels is large enough that splitting it is
 * worth the fixed per-piece overhead.  Callers pass the same `is_a8` their
 * payload constructor uses, so the rule and the payload agree. */
static inline bool gui_geometry_split_pays_off(uint64_t area, bool is_a8)
{
    return area * gui_geometry_payload_pixel_bytes(is_a8) >
           GUI_GEOMETRY_SPLIT_BYTE_BUDGET;
}

static inline bool gui_geometry_circle_bounds_valid(int center_x, int center_y,
                                                    int radius, bool allow_zero)
{
    int64_t base_x;
    int64_t base_y;
    int64_t diameter;

    if (!gui_geometry_i16_size_valid(radius, allow_zero) ||
        radius > INT16_MAX / 2)
    {
        return false;
    }

    base_x = (int64_t)center_x - radius;
    base_y = (int64_t)center_y - radius;
    diameter = (int64_t)radius * 2;
    return gui_geometry_i16_position_valid(base_x) &&
           gui_geometry_i16_position_valid(base_y) &&
           gui_geometry_i16_size_valid(diameter, true);
}

#endif /* __GUI_GEOMETRY_COMMON_H__ */
