/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_GEOMETRY_GLOW_H__
#define __GUI_GEOMETRY_GLOW_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include "guidef.h"
#include "gui_obj.h"
#include "gui_geometry_effect.h"

/*============================================================================*
 *                         Constants
 *============================================================================*/

/** Largest blur radius the API accepts, in pixels. */
#define GUI_GEOMETRY_GLOW_MAX_BLUR 32

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Build, rebuild or re-place one widget's glow.
 *
 * A thin wrapper over gui_geometry_effect_prepare(): it sets the glow mode and
 * the zero placement, then hands over.  The mask itself comes from the glow
 * rule in gui_geometry_glow.c, which is the only part of this file that differs
 * from the shadow's.
 *
 * @param slot   Widget's glow pointer; allocated here on first use.
 * @param in     Geometry the mask depends on, or NULL to switch the glow off.
 * @param matrix Matrix the body's pieces were placed with.
 * @param color  Glow colour; its alpha is the strength.
 */
void gui_geometry_glow_prepare(gui_geometry_effect_t **slot,
                               const gui_geometry_effect_input_t *in,
                               const gui_matrix_t *matrix, gui_color_t color);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_GEOMETRY_GLOW_H__ */
