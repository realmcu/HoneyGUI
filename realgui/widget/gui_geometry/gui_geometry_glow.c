/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * The glow rule, and the glow entry point.
 *
 * A glow is the ring just outside the body: empty inside, falling off outside,
 * and divided by the complement of the coverage the body will apply on top, so
 * the two edges compose instead of multiplying twice.  That compensation is
 * what makes this a separate rule from the shadow's, which needs none of it.
 *
 * Everything around the rule -- walking one corner or quadrant, mirroring it,
 * filling the interior runs, the cache, the pieces, the state machine -- lives
 * in gui_geometry_effect.c and is shared with the shadow.
 */

#include <math.h>
#include "gui_geometry_distance.h"
#include "gui_geometry_effect_internal.h"
#include "gui_geometry_glow.h"

/* 8 fractional bits: enough for 256 mask levels across blur_px pixels. */
#define GUI_GEOMETRY_EFFECT_FIX_BITS 8
#define GUI_GEOMETRY_EFFECT_FIX_ONE  (1u << GUI_GEOMETRY_EFFECT_FIX_BITS)

/* F(t) = (1 - t)^2, t = d / blur; zero at and beyond blur. */
static uint32_t glow_falloff(float d, float blur_px)
{
    if (!(d > 0.0f) || blur_px <= 0.0f)
    {
        return 255u;
    }
    if (d >= blur_px)
    {
        return 0u;
    }

    uint32_t t = (uint32_t)(d * (float)GUI_GEOMETRY_EFFECT_FIX_ONE / blur_px);
    if (t >= GUI_GEOMETRY_EFFECT_FIX_ONE)
    {
        return 0u;
    }

    uint32_t u = GUI_GEOMETRY_EFFECT_FIX_ONE - t;
    uint32_t v = (u * u) >> GUI_GEOMETRY_EFFECT_FIX_BITS;
    return v > 255u ? 255u : v;
}

/**
 * Mask value for one pixel; body coverage is divided out so the glow
 * composites correctly under SRC_OVER: (1 - c*a) * mask = (1 - c) * F.
 */
static uint8_t glow_value(float d, float blur_px, uint32_t edge_alpha)
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

    uint32_t f = glow_falloff(d, blur_px);
    if (f == 0u)
    {
        return 0u;
    }

    uint32_t c_q8 = (uint32_t)(c * (float)GUI_GEOMETRY_EFFECT_FIX_ONE + 0.5f);
    if (c_q8 >= GUI_GEOMETRY_EFFECT_FIX_ONE)
    {
        return 0u;
    }

    uint32_t one_minus_c = GUI_GEOMETRY_EFFECT_FIX_ONE - c_q8;
    uint32_t ca_q8 = (c_q8 * edge_alpha) / 255u;
    uint32_t divisor = GUI_GEOMETRY_EFFECT_FIX_ONE - ca_q8;

    if (divisor == 0u)
    {
        return 0u;
    }

    uint32_t value = (f * one_minus_c) / divisor;
    return (uint8_t)(value > 255u ? 255u : value);
}

void gui_geometry_glow_prepare(gui_geometry_effect_t **slot,
                               const gui_geometry_effect_input_t *in,
                               const gui_matrix_t *matrix, gui_color_t color)
{
    gui_geometry_effect_prepare(slot, in, matrix, 0, 0, color);
}

/* The rule table lives with the mode it describes, not with the generator, so
 * adding a mode means adding a file rather than editing a switch in the middle
 * of the traversal code. */
const gui_geometry_effect_rule_t *gui_geometry_glow_rule(void)
{
    static const gui_geometry_effect_rule_t rule =
    {
        .value = glow_value,
        .interior = 0u,
    };
    return &rule;
}
