/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Glow internals shared with the tests under tests/.
 *
 * Nothing in the firmware includes this: the public surface is
 * gui_geometry_glow.h.  It exists so the regression test can call the two mask
 * generators directly instead of going through a widget, which would drag the
 * object tree into a test that only cares about pixel values.
 */

#ifndef __GUI_GEOMETRY_GLOW_INTERNAL_H__
#define __GUI_GEOMETRY_GLOW_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "gui_geometry_glow.h"

/**
 * @brief Mask value for one pixel of the glow.
 *
 * The falloff and the compensation for the body's own edge coverage, in one
 * place so the regional generator and the test's reference generator cannot
 * drift apart on the formula while still differing on how they walk the buffer.
 *
 * @param d Signed distance to the body's outer outline, positive outside.
 * @param blur_px Blur radius in pixels.
 * @param edge_alpha The body's edge colour alpha, 0..255.
 */
uint8_t gui_geometry_glow_mask_value(float d, float blur_px, uint32_t edge_alpha);

/**
 * @brief Padding the mask needs on each side, in pixels.
 * @param blur_q Blur radius in 1/8 px, as gui_geometry_glow_blur_q() returns.
 */
uint32_t gui_geometry_glow_margin(uint32_t blur_q);

/**
 * @brief Fill a mask from one corner, mirrors and copies.
 *
 * @return false when the shape cannot be expressed by the mirror layout, in
 *         which case @p body is left untouched.
 */
bool gui_geometry_glow_build_rect_regional(uint8_t *body, int w, int h,
                                           int body_w, int body_h,
                                           int radius, int m, uint32_t blur_q,
                                           uint32_t edge_alpha);

/**
 * @brief Fill a mask from one quadrant and mirrors.
 *
 * The buffer must be exactly (radius + margin) * 2 on both axes, so the centre
 * lands on the mirror axis.
 *
 * @return false when that does not hold, in which case @p body is untouched.
 */
bool gui_geometry_glow_build_circle_regional(uint8_t *body, int w, int h,
                                             int radius, int m,
                                             uint32_t blur_q, uint32_t edge_alpha);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_GEOMETRY_GLOW_INTERNAL_H__ */
