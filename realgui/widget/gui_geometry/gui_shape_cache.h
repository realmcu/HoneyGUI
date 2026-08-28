/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_SHAPE_CACHE_H__
#define __GUI_SHAPE_CACHE_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>

/*============================================================================*
 *                         Constants
 *============================================================================*/

/**
 * @brief Reference-counted store for rasterised geometry pixel data.
 *
 * gui_rect, gui_circle and gui_arc each rasterise their shape into an
 * ARGB8888 buffer once and then blit it every frame.  That buffer's contents
 * depend only on the parameters used to generate it -- size, radius, colour,
 * gradient -- and never on where the shape sits, because position lives in the
 * draw_img matrix.  So a screen with sixteen identical dots holds sixteen
 * byte-identical buffers.
 *
 * A widget therefore describes the buffer it wants instead of allocating one:
 * pass a descriptor, get back either an existing buffer with its reference
 * count bumped, or a fresh allocation to fill in.  Identity is decided by an
 * exact memcmp of the descriptor, not by a hash, so a hash collision can never
 * hand back the wrong pixels.
 *
 * Only the pixel payload is shared.  Each widget keeps its own draw_img_t: the
 * matrix, inverse, target area and accelerator scratch data are all per
 * instance and per frame.
 *
 * All entry points must be called from the GUI task -- there is no locking,
 * matching the rest of the widget layer.
 */

/**
 * Descriptor ceiling: a geometry key plus a full Gradient (116 bytes).
 *
 * Only a sanity bound -- a node stores the descriptor length it was given, so a
 * solid shape does not pay for the space a gradient would need.  Per-node
 * bookkeeping is 24 bytes plus the descriptor, which is still enough that only
 * payloads holding real pixels are worth caching: sharing a bare image header
 * costs far more than it saves.
 */
#define GUI_SHAPE_DESC_MAX 160

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Take a reference on the buffer matching @p desc, allocating if absent.
 *
 * On a miss the payload is allocated but left uninitialised, and @p is_new is
 * set so the caller knows it must rasterise into it.  On a hit the caller must
 * not write to the payload: other widgets are drawing from it.
 *
 * @param desc     Parameters that fully determine the payload's contents.
 *                 Must not contain padding holes or uninitialised bytes -- zero
 *                 the descriptor struct before filling it, or two runs of the
 *                 same shape will miss each other.
 * @param desc_len Descriptor length in bytes, at most GUI_SHAPE_DESC_MAX.
 * @param size     Payload size in bytes (image header plus pixels).
 * @param is_new   Set true when the payload needs rasterising, false on a hit.
 * @return Payload pointer, or NULL if allocation failed.
 */
uint8_t *gui_shape_cache_acquire(const void *desc, uint16_t desc_len,
                                 uint32_t size, bool *is_new);

/**
 * @brief Refresh a mutable payload with spare capacity for geometry changes.
 *
 * The cache is searched once. A hit replaces the caller's current reference.
 * Otherwise an unshared current payload is re-keyed in place when it has enough
 * capacity. A replacement allocation reserves a geometric size class, allowing
 * later rekeys to grow without reallocating on every small size change.
 *
 * @param current  Payload currently owned by the caller, or NULL.
 * @param desc     Parameters that fully determine the payload's contents.
 * @param desc_len Descriptor length in bytes, at most GUI_SHAPE_DESC_MAX.
 * @param size     Current logical payload size in bytes.
 * @param must_render Set true when the returned payload needs rasterising.
 * @return Payload pointer, or NULL if allocation failed.
 */
uint8_t *gui_shape_cache_refresh_resizable(const void *current,
                                           const void *desc, uint16_t desc_len,
                                           uint32_t size, bool *must_render);

/**
 * @brief Take a reference on a matching payload, without allocating one.
 *
 * For callers that hold a private payload they could re-render in place: sharing
 * an existing one is better still, but only if it already exists.
 *
 * @param desc     Parameters that fully determine the payload's contents.
 * @param desc_len Descriptor length in bytes, at most GUI_SHAPE_DESC_MAX.
 * @param size     Payload size in bytes.
 * @return Payload pointer with its reference count bumped, or NULL on a miss.
 */
uint8_t *gui_shape_cache_find(const void *desc, uint16_t desc_len, uint32_t size);

/**
 * @brief Take an extra reference on a payload already held by the caller.
 *
 * For the case where one widget blits the same pixels through several
 * draw_img_t structures, so that every structure can be released the same way.
 *
 * @param data Payload previously returned by gui_shape_cache_acquire().
 */
void gui_shape_cache_addref(const void *data);

/**
 * @brief Drop a reference, freeing the payload when the last one goes.
 *
 * @param data Payload previously returned by gui_shape_cache_acquire(), or NULL.
 */
void gui_shape_cache_release(const void *data);

/**
 * @brief Re-describe a payload in place when the caller is its only user.
 *
 * An animating shape changes its parameters every frame.  Releasing and
 * re-acquiring would free and re-allocate each time, so when nobody else shares
 * the payload it is re-keyed to @p desc and reused, keeping its allocation.
 * The caller must then rasterise into it, exactly as for a miss.
 *
 * @param data     Payload previously returned by gui_shape_cache_acquire().
 * @param desc     New descriptor.
 * @param desc_len New descriptor length, at most GUI_SHAPE_DESC_MAX.
 * @param size     New payload size; must fit the existing allocation.
 * @return true if re-keyed and safe to overwrite, false if the caller must
 *         release and acquire instead.
 */
bool gui_shape_cache_rekey(const void *data, const void *desc, uint16_t desc_len,
                           uint32_t size);

/**
 * @brief Report cache occupancy, for tuning and leak hunting.
 *
 * @param payload_bytes Out, total payload bytes held, or NULL.
 * @param shared_bytes  Out, payload bytes that would have been duplicated
 *                      without sharing, or NULL.
 * @return Number of distinct payloads held.
 */
uint32_t gui_shape_cache_stats(uint32_t *payload_bytes, uint32_t *shared_bytes);

/**
 * @brief Return the total payload capacity reserved by cache nodes.
 *
 * This can exceed the logical payload bytes reported by
 * gui_shape_cache_stats() when resizable nodes keep growth headroom.
 */
uint32_t gui_shape_cache_capacity_bytes(void);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_SHAPE_CACHE_H__ */
