/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_GEOMETRY_DISTANCE_H__
#define __GUI_GEOMETRY_DISTANCE_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include "lite_geometry.h"

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Signed distance from a point to a rounded rectangle's outline.
 *
 * Negative inside, positive outside, zero on the outline.  This is the same
 * field gui_rect's anti-aliasing samples, so a mask built from it lines up with
 * the body's own edge coverage instead of sitting half a pixel off.
 *
 * Only the corner arcs take a square root; along the straight edges the
 * expression collapses to a subtraction.
 *
 * Lives here rather than in gui_rect.c because the glow layer needs the same
 * numbers: two copies of this formula would drift the moment either side
 * changed how the radius is clamped.
 *
 * @param px,py Point in the rect's own coordinate space, corner at (0, 0).
 * @param x,y   Rect origin; the widget always passes (0, 0).
 * @param w,h   Rect size.
 * @param radius Corner radius, clamped to at most half the shorter side.
 */
static inline float gui_geometry_rounded_rect_distance(float px, float py,
                                                       float x, float y,
                                                       float w, float h,
                                                       float radius)
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

/**
 * @brief Coverage the body's outer edge has at a given signed distance.
 *
 * One pixel either side of the outline is a ramp; further in it is solid, and
 * further out it is empty.  The glow's own mask divides by the complement of
 * this, so the two edges compose instead of multiplying twice.
 */
static inline float gui_geometry_edge_coverage(float distance)
{
    return LG_CLAMP(0.5f - distance, 0.0f, 1.0f);
}

#ifdef __cplusplus
}
#endif

#endif /* __GUI_GEOMETRY_DISTANCE_H__ */
