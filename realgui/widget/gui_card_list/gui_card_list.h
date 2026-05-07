/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __GUI_CARD_LIST_H__
#define __GUI_CARD_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_obj.h"


#ifdef M_PI
#undef M_PI
#endif
#define M_PI 3.14159265358979323846f

/*============================================================================*
 *                         Types
 *============================================================================*/

/**
 * @brief Internal slot object - one pool entry holding one card's contents.
 *
 * Each slot is a fixed object whose children are created / destroyed each
 * time the slot is assigned to a new card_id.  Slots are always parented to
 * the card_list and sit at position (0, 0).  All visual placement (scale,
 * rotation, translation) is encoded in the slot's matrix inside OBJ_PREPARE.
 */
typedef struct gui_card_slot
{
    gui_obj_t base;         /**< Must be the first member (C inheritance). */
    int       virtual_id;   /**< Logical slot index; CARD_LIST_SLOT_FREE_ID = unassigned. */
    int       card_id;      /**< Data index into user's card array; -1 = unassigned. */
} gui_card_slot_t;

/**
 * @brief 3-D carousel card-list widget.
 *
 * Displays a fixed pool of card slots arranged in a perspective carousel.
 * The centre slot is the selected card; cards to each side are scaled down,
 * dimmed, and rotated around the Y-axis (horizontal mode) or X-axis
 * (vertical mode) to create a 2.5-D depth effect.
 *
 * Typical usage:
 * @code
 *   gui_card_list_t *cl = gui_card_list_create(parent, "cards",
 *                                               0, 0, 480, 360,
 *                                               200, 280, 5,
 *                                               HORIZONTAL,
 *                                               my_card_design, NULL);
 *   gui_card_list_set_card_count(cl, 10);
 * @endcode
 */
typedef struct gui_card_list
{
    gui_obj_t base;                 /**< Must be the first member. */

    /* ---- layout --------------------------------------------------------- */
    uint32_t  dir              : 1; /**< 0 = HORIZONTAL, 1 = VERTICAL.      */

    /* ---- behaviour flags ------------------------------------------------ */
    uint32_t  loop             : 1; /**< Enable circular wrapping.           */
    uint32_t  inertia          : 1; /**< Enable inertia (fling) scrolling.   */
    uint32_t  scroll_to_active : 1; /**< Programmatic scroll in progress.    */
    uint32_t  enable_scroll    : 1; /**< Master on/off for touch scrolling.  */
    uint32_t  face_inward      : 1; /**< Side cards face the centre (inward) */
    uint32_t  is_scrolling     : 1; /**< True while carousel is in motion.   */
    uint32_t  _reserved        : 25;

    /* ---- card data ------------------------------------------------------ */
    uint16_t  card_count;           /**< Total number of data cards.         */
    uint8_t   display_count;        /**< Visible slots (always odd, >= 3).   */
    uint8_t   slot_count;           /**< Pool size = display_count + 2.      */

    /* ---- geometry ------------------------------------------------------- */
    uint16_t  card_w;               /**< Pixel width  of one card (logical). */
    uint16_t  card_h;               /**< Pixel height of one card (logical). */
    int       slot_size;            /**< Offset delta per card step (pixels).*/

    /* ---- 3-D effect parameters ----------------------------------------- */
    float     screen_gap;           /**< Centre-to-centre visual distance.   */
    float     min_scale;            /**< Minimum scale for off-centre cards. */
    float     scale_step;           /**< Scale reduction per slot unit.      */
    float     min_opacity;          /**< Minimum opacity (0..1).             */
    float     opacity_step;         /**< Opacity reduction per slot unit.    */
    float     angle_per_slot;       /**< rotateY/X degrees per slot unit.    */
    float     y_arc_per_slot;       /**< Vertical arc offset per slot unit.  */
    float     z_per_slot;           /**< Z-depth per slot unit.              */
    float     fov_depth;            /**< Perspective eye distance.           */

    /* ---- scroll state --------------------------------------------------- */
    int       offset;               /**< Current scroll offset (pixels).     */
    int       hold;                 /**< Offset at last finger-down/up.      */
    int       scroll_target;        /**< Target offset for programmatic anim.*/
    int16_t   speed;                /**< Current inertia speed.              */
    int16_t   record[5];            /**< Recent delta ring-buffer for speed. */
    float     factor;               /**< Inertia decay factor (0..1).        */

    /* ---- user callbacks ------------------------------------------------- */
    void (*card_design)(gui_obj_t *obj, int card_id, void *param);
    void     *design_param;

    /* ---- pool ----------------------------------------------------------- */
    gui_card_slot_t *slots;         /**< Heap-allocated pool array.          */

    /* ---- checksum for dirty-flag --------------------------------------- */
    uint8_t   checksum;
} gui_card_list_t;

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief  Create a gui_card_list widget.
 *
 * @param parent        Parent widget (must not be NULL).
 * @param name          Widget name string.
 * @param x, y          Position relative to parent.
 * @param w, h          Widget size (0 = fill to screen edge).
 * @param card_w        Width  of a single card content area.
 * @param card_h        Height of a single card content area.
 * @param display_count Visible cards (must be odd >= 3; clamped internally).
 * @param dir           0 = HORIZONTAL, 1 = VERTICAL.
 * @param card_design   Callback invoked whenever a slot needs new content.
 *                      The callback should populate @p obj (a slot widget)
 *                      with child widgets for card index @p card_id.
 * @param param         Opaque pointer forwarded to @p card_design.
 * @return              Pointer to the new widget, or NULL on allocation failure.
 */
gui_card_list_t *gui_card_list_create(void       *parent,
                                      const char *name,
                                      int16_t     x,
                                      int16_t     y,
                                      int16_t     w,
                                      int16_t     h,
                                      uint16_t    card_w,
                                      uint16_t    card_h,
                                      uint8_t     display_count,
                                      uint8_t     dir,
                                      void (*card_design)(gui_obj_t *obj, int card_id, void *param),
                                      void       *param);

/**
 * @brief  Set the total number of data cards.
 *
 * Resets the pool and scroll position.  Safe to call at any time.
 */
void gui_card_list_set_card_count(gui_card_list_t *cl, uint16_t count);

/** @brief Enable or disable circular wrapping. */
void gui_card_list_enable_loop(gui_card_list_t *cl, bool enable);

/** @brief Enable or disable all touch scrolling. */
void gui_card_list_enable_scroll(gui_card_list_t *cl, bool enable);

/**
 * @brief  Control whether side cards face the centre (inward) or face away
 *         (outward).
 *
 * When @p inward is @c true (default) each off-centre card is rotated so that
 * its front face is directed toward the carousel centre, matching a standard
 * cover-flow look.  Set to @c false for the mirror effect where cards show
 * their "back" edge toward the centre.
 *
 * This setting affects both HORIZONTAL (rotateY) and VERTICAL (rotateX)
 * orientations.
 */
void gui_card_list_set_face_inward(gui_card_list_t *cl, bool inward);

/** @brief Set inertia decay factor (0 = no decay, 1 = instant stop). */
void gui_card_list_set_factor(gui_card_list_t *cl, float factor);

/**
 * @brief  Override all 3-D effect parameters in one call.
 *
 * Pass 0.0f for any parameter to keep its current value.
 */
void gui_card_list_set_effect_params(gui_card_list_t *cl,
                                     float screen_gap,
                                     float min_scale,
                                     float scale_step,
                                     float min_opacity,
                                     float opacity_step,
                                     float angle_per_slot,
                                     float y_arc_per_slot,
                                     float z_per_slot,
                                     float fov_depth);

/**
 * @brief  Return the data index of the currently centred (selected) card.
 *
 * Returns -1 if card_count is 0.
 */
int gui_card_list_get_current_index(gui_card_list_t *cl);

/**
 * @brief  Return the widget handle of the currently centred (selected) card.
 *
 * Returns the first child of the centred slot - i.e. the top-level widget
 * created by the @p card_design callback for the active card.
 * Returns NULL if no card is active or no children have been created.
 */
gui_obj_t *gui_card_list_get_current_card(gui_card_list_t *cl);

/**
 * @brief  Return the widget handle of the card at the given data index.
 *
 * Searches the live slot pool for a slot currently assigned to @p idx and
 * returns its first child widget (the one created by @p card_design).
 * Returns NULL if the card at @p idx is not currently loaded in the pool
 * (i.e. it is outside the visible window), @p idx is out of range, or the
 * slot has no children yet.
 */
gui_obj_t *gui_card_list_get_card_by_index(gui_card_list_t *cl, int idx);

/**
 * @brief  Animate (ease) to the given card index.
 *
 * In loop mode the shortest circular path is taken.
 */
void gui_card_list_scroll_to_card(gui_card_list_t *cl, int idx);

/**
 * @brief  Jump instantly (no animation) to the given card index.
 */
void gui_card_list_jump_to_card(gui_card_list_t *cl, int idx);

/**
 * @brief  Query whether the carousel is currently in motion.
 *
 * Returns @c true whenever the carousel is actively moving - the user is
 * pressing and dragging, inertia is decaying, or the auto-align snap
 * animation has not yet completed.  Returns @c false when the carousel is
 * fully settled on a card (speed == 0, not being touched, not running a
 * programmatic animation, and offset is exactly on a snap position).
 *
 * Typical use: poll this from a timer or touch callback to defer actions
 * (e.g. loading card content) until the carousel has stopped moving.
 *
 * @param  cl  Pointer to the card-list widget.
 * @return     @c true if scrolling, @c false if idle.
 */
bool gui_card_list_is_scrolling(gui_card_list_t *cl);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_CARD_LIST_H__ */
