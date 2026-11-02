/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Effect internals: the one thing the shared generator needs from each rule,
 * plus the entry points the regression test drives directly.
 *
 * Nothing in the firmware includes this.  The public surface is
 * gui_geometry_effect.h for the engine and gui_geometry_glow.h /
 * gui_geometry_shadow.h for the two wrappers.
 *
 * A rule owns exactly one decision: what a pixel at signed distance d is worth.
 * Everything else -- walking one corner or quadrant and mirroring it, filling
 * the interior runs, keying the cache, placing pieces -- is written once in
 * gui_geometry_effect.c and asks the rule for values.  Splitting the rule out
 * keeps the two sets of edge behaviour readable on their own; keeping the
 * traversal shared is what stops one of them being fixed and the other not.
 */

#ifndef __GUI_GEOMETRY_EFFECT_INTERNAL_H__
#define __GUI_GEOMETRY_EFFECT_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "gui_geometry_effect.h"

/**
 * The two things a mode contributes.  Everything else is the generator's.
 *
 * @c interior is what the generator stores where the body is solid and no
 * distance applies: a glow stores nothing there, a shadow stores the
 * full-strength value its offset will slide out past the body's edge.
 */
typedef struct
{
    uint8_t (*value)(float d, float blur_px, uint32_t edge_alpha);
    uint8_t interior;
} gui_geometry_effect_rule_t;

/**
 * @brief The rule for one mode, or NULL if the mode is unknown.
 */
const gui_geometry_effect_rule_t *gui_geometry_effect_rule(uint32_t mode);

/** Each mode's rule, defined next to the rule itself. */
const gui_geometry_effect_rule_t *gui_geometry_glow_rule(void);
const gui_geometry_effect_rule_t *gui_geometry_shadow_rule(void);

/**
 * @brief Mask value for one pixel, resolved through the mode's rule.
 */
uint8_t gui_geometry_effect_value(uint32_t mode, float d, float blur_px, uint32_t edge_alpha);

/**
 * @brief Padding the mask needs on each side, in pixels.
 * @param blur_q Blur radius in 1/8 px, as gui_geometry_effect_blur_q() returns.
 */
uint32_t gui_geometry_effect_margin(uint32_t blur_q);

/**
 * @brief Fill a mask from one corner, mirrors and copies.
 *
 * @return false when the shape cannot be expressed by the mirror layout, in
 *         which case @p body is left untouched.
 */
bool gui_geometry_effect_build_rect_regional(uint8_t *body, int w, int h,
                                             int body_w, int body_h,
                                             int radius, int m, uint32_t blur_q,
                                             uint32_t edge_alpha, uint32_t mode);

/**
 * @brief Fill a mask from one quadrant and mirrors.
 *
 * The buffer must be exactly (radius + margin) * 2 on both axes, so the centre
 * lands on the mirror axis.
 *
 * @return false when that does not hold, in which case @p body is untouched.
 */
bool gui_geometry_effect_build_circle_regional(uint8_t *body, int w, int h,
                                               int radius, int m,
                                               uint32_t blur_q, uint32_t edge_alpha,
                                               uint32_t mode);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_GEOMETRY_EFFECT_INTERNAL_H__ */
