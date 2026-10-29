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
#include "gui_geometry_image.h"

/*============================================================================*
 *                         Constants
 *============================================================================*/

/**
 * Bumped whenever the mask contents would change for the same inputs: the
 * falloff curve, the edge-alpha compensation, or the block layout.  It is part
 * of every descriptor, so an upgrade invalidates old cache nodes instead of
 * letting two different masks answer to one key.
 */
#define GUI_GEOMETRY_GLOW_ALGORITHM 1u

/**
 * Ceiling on the one-buffer path, in bytes of payload; 0 disables the check.
 *
 * The one-buffer mask is (w + 2m) x (h + 2m), so it grows with the widget's
 * area: a full-screen rect can ask for a few hundred kilobytes.  Left at 0 the
 * size is whatever the shape needs -- the common case, since a glow is normally
 * on one or two emphasis elements rather than every list row.
 *
 * Set it on a platform whose heap cannot spare that much, or turn on
 * GUI_GEOMETRY_GLOW_ENABLE_SPLIT (in gui_geometry_glow.c) so the mask is cut
 * into blocks whose size does not follow the widget's area.
 *
 * This is a per-instance cap, not a budget for every glow on the screen -- two
 * adjacent cards are two.
 */
#ifndef GUI_GEOMETRY_GLOW_SINGLE_LIMIT
#define GUI_GEOMETRY_GLOW_SINGLE_LIMIT 0u
#endif

/** Largest blur radius the API accepts, in pixels. */
#define GUI_GEOMETRY_GLOW_MAX_BLUR 32

/*============================================================================*
 *                         Types
 *============================================================================*/

typedef enum
{
    GUI_GEOMETRY_GLOW_SHAPE_RECT = 1,
    GUI_GEOMETRY_GLOW_SHAPE_CIRCLE,
} gui_glow_shape_t;

/**
 * Everything the mask pixels depend on.
 *
 * Zeroed and filled by the widget, then compared byte for byte against the
 * copy kept from the last build: position lives in the matrix and colour in
 * the descriptor-free draw path, so neither belongs here.
 */
typedef struct
{
    uint32_t shape;             /**< gui_glow_shape_t. */
    uint32_t single;            /**< Non-zero: one buffer is required, because a
                                 *   transform stops the blocks following whole
                                 *   pixels.  Zero lets the size decide. */
    int32_t  w;                 /**< Body width in the coordinate space used. */
    int32_t  h;                 /**< Body height. */
    int32_t  radius;            /**< Corner radius, or the circle's radius. */
    uint32_t blur_q;            /**< Blur in 1/8 px, see gui_geometry_glow_blur_q(). */
    uint32_t edge_alpha;        /**< Body's edge alpha, 0..255; see the
                                 *   compensation in gui_geometry_glow.c. */
} gui_geometry_glow_input_t;

/**
 * One widget's glow: its pieces, the input that produced them, and the screen
 * area they last covered.
 *
 * Allocated on first use, so a rect or circle without a glow pays a pointer
 * and nothing else.
 */
typedef struct gui_geometry_glow
{
    gui_geometry_parts_t parts;  /**< Pieces, kept apart from the body's. */
    gui_geometry_glow_input_t built;          /**< Input the pieces were built from. */
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
    uint8_t      suppressed : 1; /**< Over budget; retried when input changes. */
} gui_geometry_glow_t;

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Quantise a blur radius for use as a cache key.
 *
 * The mask is generated from the same value this returns, never from the raw
 * float: a key that rounds differently from the pixels it names would let two
 * different masks answer to one entry.  Clamped to 1.0 .. 32.0 px so shrinking
 * a widget cannot silently switch its glow off.
 *
 * @param blur_px Blur radius in local pixels.
 * @return Blur in 1/8 px units.
 */
uint32_t gui_geometry_glow_blur_q(float blur_px);

/**
 * @brief Build, rebuild or re-place one widget's glow.
 *
 * Call from prepare, inside the body's scale-baking window, so the glow sees
 * the same geometry and the same matrix the body's pieces were placed with.
 *
 * @param slot   Widget's glow pointer; allocated here on first use.
 * @param in     What the mask depends on, or NULL to switch the glow off.
 * @param matrix Matrix the body's pieces were placed with.
 * @param moved  The body only moved this frame, nothing else changed.
 * @param color  Glow colour; its alpha is the strength.
 */
void gui_geometry_glow_prepare(gui_geometry_glow_t **slot, const gui_geometry_glow_input_t *in,
                               const gui_matrix_t *matrix, bool moved, gui_color_t color);

/**
 * @brief Draw the glow.  Call before the body's own pieces.
 *
 * @param glow           Widget's glow, or NULL.
 * @param parent_opacity Parent's opacity value.
 * @param widget_opacity Widget's own opacity value.
 */
void gui_geometry_glow_draw(gui_geometry_glow_t *glow, uint8_t parent_opacity,
                            uint8_t widget_opacity);

/**
 * @brief Drop the glow, marking the area it covered as dirty.
 *
 * @param slot Widget's glow pointer; cleared.
 */
void gui_geometry_glow_free(gui_geometry_glow_t **slot);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_GEOMETRY_GLOW_H__ */
