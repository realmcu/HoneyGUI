/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_GEOMETRY_EFFECT_H__
#define __GUI_GEOMETRY_EFFECT_H__
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
#include "gui_geometry_image.h"

/*============================================================================*
 *                         Constants
 *============================================================================*/

/**
 * Bumped whenever the mask contents would change for the same inputs: a rule's
 * falloff or compensation, its interior fill, or the block layout.  It is part
 * of every descriptor, so an upgrade invalidates old cache nodes instead of
 * letting two different masks answer to one key.
 */
#define GUI_GEOMETRY_EFFECT_ALGORITHM 2u

/**
 * Ceiling on the one-buffer path, in bytes of payload; 0 disables the check.
 *
 * The one-buffer mask is (w + 2s + 2m) x (h + 2s + 2m), so it grows with the
 * widget's area and with the spread.  Left at 0 the size is whatever the shape
 * needs -- the common case, since an effect is normally on one or two emphasis
 * elements rather than every list row.
 *
 * 0 is not a promise that the allocation fits: a platform enabling these
 * effects must set a limit its heap can spare and measure the total peak,
 * including cache capacity rounding and both layers on one widget.
 */
#if defined(GUI_GEOMETRY_GLOW_SINGLE_LIMIT) && !defined(GUI_GEOMETRY_EFFECT_SINGLE_LIMIT)
#define GUI_GEOMETRY_EFFECT_SINGLE_LIMIT GUI_GEOMETRY_GLOW_SINGLE_LIMIT
#endif
#ifndef GUI_GEOMETRY_EFFECT_SINGLE_LIMIT
#define GUI_GEOMETRY_EFFECT_SINGLE_LIMIT 0u
#endif

/*============================================================================*
 *                         Types
 *============================================================================*/

typedef enum
{
    GUI_GEOMETRY_EFFECT_SHAPE_RECT = 1,
    GUI_GEOMETRY_EFFECT_SHAPE_CIRCLE,
} gui_geometry_effect_shape_t;

/**
 * Which rule turns a distance into a mask value.
 *
 * GLOW is 0 so a caller that zeroes the input and fills the geometry keeps
 * asking for a glow, as every call site written before shadows existed does.
 *
 * This header knows only that the modes exist.  What each one puts in the mask
 * lives in gui_geometry_glow.c and gui_geometry_shadow.c; everything around it
 * -- the regional generator, the cache, the piece layout, the state machine --
 * is here and exists once.
 */
typedef enum
{
    GUI_GEOMETRY_EFFECT_MODE_GLOW = 0,
    GUI_GEOMETRY_EFFECT_MODE_SHADOW,
} gui_geometry_effect_mode_t;

/**
 * Everything a mask's pixels depend on.
 *
 * Zeroed and filled by the caller, then compared byte for byte against the copy
 * kept from the last build: position lives in the placement and the matrix,
 * colour in the descriptor-free draw path, so neither belongs here.  Geometry
 * arrives with any spread already folded in, so a spread shadow and a larger
 * plain body share a mask when their outlines agree.
 */
typedef struct
{
    uint32_t shape;             /**< gui_geometry_effect_shape_t. */
    uint32_t mode;              /**< gui_geometry_effect_mode_t. */
    uint32_t single;            /**< Non-zero: one buffer is required, because a
                                 *   transform stops the blocks following whole
                                 *   pixels.  Zero lets the size decide. */
    int32_t  w;                 /**< Body width in the coordinate space used. */
    int32_t  h;                 /**< Body height. */
    int32_t  radius;            /**< Corner radius, or the circle's radius. */
    uint32_t blur_q;            /**< Blur in 1/8 px, see
                                 *   gui_geometry_effect_blur_q(). */
    uint32_t edge_alpha;        /**< Body's edge alpha, 0..255.  The glow rule
                                 *   compensates for it; the shadow rule ignores
                                 *   it and callers pass 0. */
} gui_geometry_effect_input_t;

/**
 * One widget's effect layer: its pieces, the input that produced them, and the
 * screen area they last covered.
 *
 * Allocated on first use, so a rect or circle without an effect pays a pointer
 * and nothing else.  A widget holds up to two of these, one for the glow and
 * one for the shadow.
 */
typedef struct gui_geometry_effect
{
    gui_geometry_parts_t parts;  /**< Pieces, kept apart from the body's. */
    gui_geometry_effect_input_t built;          /**< Input the pieces were built from. */
    /** Input whose build failed; kept apart from @c built, which only ever
     *  holds a successful one.  Without it a failure looks like "input
     *  changed" on every frame and the retry backoff never engages. */
    gui_geometry_effect_input_t failed;
    /** Placement that failed alongside @c failed.  A placement the cold path
     *  refused is a distinct failure from a mask that would not allocate, and
     *  the wait must not be carried over when the caller fixes just the
     *  offset. */
    int32_t      failed_placement_x;
    int32_t      failed_placement_y;
    gui_matrix_t last_matrix;    /**< Matrix the pieces were last placed with. */
    int32_t      last_placement_x; /**< Extra local offset last placed with. */
    int32_t      last_placement_y;
    uint8_t     *band;           /**< Own reference on the band payload, so a
                                  *   width change can re-key it in place
                                  *   instead of allocating again. */
    uint8_t     *single;         /**< Own reference on the one-buffer payload,
                                  *   for the same reason: a size animation
                                  *   re-keys it in place rather than trading a
                                  *   free and an allocation every frame. */
    gui_rect_t   last_rect;      /**< Screen area drawn last time. */
    uint32_t     last_color;     /**< Colour and strength drawn last time. */
    uint8_t      built_valid : 1;
    uint8_t      suppressed : 1; /**< Over budget or out of memory. */
    uint8_t      matrix_valid : 1;
    uint8_t      placement_valid : 1;
    uint8_t      failed_valid : 1; /**< @c failed holds the input that failed. */
    /** Backoff interval in prepare cycles, doubling on each failure. */
    uint8_t      retry_interval;
    /** Cycles still to wait before the next attempt; counted down per frame. */
    uint8_t      retry_wait;
} gui_geometry_effect_t;

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Quantise a blur radius for use as a cache key.
 *
 * The mask is generated from the same value this returns, never from the raw
 * float: a key that rounds differently from the pixels it names would let two
 * different masks answer to one entry.  Clamped to 1.0 .. 32.0 px so shrinking
 * a widget cannot silently switch its effect off.
 *
 * @param blur_px Blur radius in local pixels.
 * @return Blur in 1/8 px units.
 */
uint32_t gui_geometry_effect_blur_q(float blur_px);

/**
 * @brief Build, rebuild or re-place one widget's effect layer.
 *
 * Call from prepare, inside the body's scale-baking window, so the effect sees
 * the same geometry and the same matrix the body's pieces were placed with.
 *
 * The glow and shadow wrappers in gui_geometry_glow.h and gui_geometry_shadow.h
 * call this; a widget normally goes through one of those rather than here.
 *
 * @param slot        Widget's layer pointer; allocated here on first use.
 * @param in          What the mask depends on, or NULL to switch the effect off.
 * @param matrix      Matrix the body's pieces were placed with.
 * @param placement_x Extra local offset for the pieces, added on top of the
 *                    margin origin.  Zero for a glow.
 * @param placement_y Likewise for y.
 * @param color       Effect colour; its alpha is the strength.
 */
void gui_geometry_effect_prepare(gui_geometry_effect_t **slot,
                                 const gui_geometry_effect_input_t *in,
                                 const gui_matrix_t *matrix,
                                 int32_t placement_x, int32_t placement_y,
                                 gui_color_t color);

/**
 * @brief Draw one effect layer.  Call before the body's own pieces.
 *
 * @param effect         Widget's layer, or NULL.
 * @param parent_opacity Parent's opacity value.
 * @param widget_opacity Widget's own opacity value.
 */
void gui_geometry_effect_draw(gui_geometry_effect_t *effect, uint8_t parent_opacity,
                              uint8_t widget_opacity);

/**
 * @brief Drop an effect layer and its pixels.
 *
 * @param slot Widget's layer pointer; cleared.
 */
void gui_geometry_effect_free(gui_geometry_effect_t **slot);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_GEOMETRY_EFFECT_H__ */
