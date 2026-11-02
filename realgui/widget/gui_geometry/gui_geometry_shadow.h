/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_GEOMETRY_SHADOW_H__
#define __GUI_GEOMETRY_SHADOW_H__
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

/** Largest spread the shadow API accepts, in pixels per side. */
#define GUI_GEOMETRY_SHADOW_MAX_SPREAD 32

/** Largest offset length the direction helper accepts, in pixels. */
#define GUI_GEOMETRY_SHADOW_MAX_DISTANCE 64

/*============================================================================*
 *                         Types
 *============================================================================*/

/**
 * A drop shadow: colour, offset, blur and spread.
 *
 * The colour's alpha is the strength.  The offset is a local integer vector, so
 * the shadow turns with the widget the way a drop shadow does in a drawing
 * tool; a caller that wants a fixed screen direction converts it itself.
 */
typedef struct
{
    gui_color_t color;          /**< Shadow colour; alpha is the strength. */
    int16_t     dx;             /**< Local offset in x, whole pixels. */
    int16_t     dy;             /**< Local offset in y, whole pixels. */
    uint8_t     blur;           /**< Blur radius 1..32 px; 0 disables. */
    uint8_t     spread;         /**< Inflate each side by this much; 0..32. */
} gui_shadow_t;

/** The nine preset directions, matching the usual drawing-tool palette. */
typedef enum
{
    GUI_SHADOW_DIR_TOP_LEFT = 0,
    GUI_SHADOW_DIR_TOP,
    GUI_SHADOW_DIR_TOP_RIGHT,
    GUI_SHADOW_DIR_LEFT,
    GUI_SHADOW_DIR_CENTER,
    GUI_SHADOW_DIR_RIGHT,
    GUI_SHADOW_DIR_BOTTOM_LEFT,
    GUI_SHADOW_DIR_BOTTOM,
    GUI_SHADOW_DIR_BOTTOM_RIGHT,
} gui_shadow_dir_t;

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Fill a shadow configuration's offset from a preset direction.
 *
 * Only touches @p dx and @p dy; the widget keeps the copy it was given, so a
 * caller must call the widget's setter again after changing the configuration.
 *
 * The diagonal component is rounded with integer arithmetic, which makes the
 * result a whole number of pixels -- a fractional offset would push every
 * piece into the 2D inverse-map blit path for a difference nothing can see.
 *
 * @param shadow   Configuration to update.
 * @param dir      One of the nine presets; an unknown value leaves it alone.
 * @param distance Offset length in pixels.
 */
void gui_shadow_set_direction(gui_shadow_t *shadow, gui_shadow_dir_t dir, uint8_t distance);

/**
 * @brief The configuration a widget starts with.
 *
 * Returned by value rather than exposed as a constant, because gui_rgba() is a
 * function in this tree and cannot initialise a file-scope structure.
 */
gui_shadow_t gui_shadow_default(void);

/**
 * @brief Build, rebuild or re-place one widget's shadow.
 *
 * A thin wrapper over gui_geometry_effect_prepare().  The offset and the spread
 * never reach the mask: the spread is already folded into @p in's geometry by
 * the caller, and the offset only moves the pieces, so one mask serves every
 * direction and every distance.
 *
 * @param slot        Widget's shadow pointer; allocated here on first use.
 * @param in          Geometry the mask depends on, or NULL to switch the shadow
 *                    off.
 * @param matrix      Matrix the body's pieces were placed with.
 * @param placement_x Extra local offset for the pieces, typically
 *                    (-spread + dx); the margin offset is added on top.
 * @param placement_y Likewise for y.
 * @param color       Shadow colour; its alpha is the strength.
 */
void gui_geometry_shadow_prepare(gui_geometry_effect_t **slot,
                                 const gui_geometry_effect_input_t *in,
                                 const gui_matrix_t *matrix,
                                 int32_t placement_x, int32_t placement_y,
                                 gui_color_t color);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_GEOMETRY_SHADOW_H__ */
