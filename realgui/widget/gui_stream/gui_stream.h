/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_STREAM_H__
#define __GUI_STREAM_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "draw_img.h"
#include "gui_img.h"
#include "gui_fb.h"
#include "gui_video.h"          /* GUI_VIDEO_STATE                          */
#include "stream_transport.h"   /* stp_transport_t consumed by the widget   */
#include "msv1_decoder.h"
#include "cinepak_decoder.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/**
 * @brief  Codec of the encoded frames delivered through the transport.
 *
 * Fixed at creation time; the matching decoder is selected by a switch on this
 * tag (mirrors gui_lite_video / gui_video).  The frame geometry is taken from
 * the create() call -- there is no container header in a live stream, so the
 * decoder cannot learn dimensions on its own.
 */
typedef enum
{
    GUI_STREAM_CODEC_RAW = 0,   /**< Uncompressed, engine-native: copy only.     */
    GUI_STREAM_CODEC_JPEG,      /**< Baseline JPEG / MJPEG frame.                */
    GUI_STREAM_CODEC_MSV1,      /**< Microsoft Video 1, 16-bpp (RGB555) only.    */
    GUI_STREAM_CODEC_CINEPAK,   /**< Cinepak (CVID).                             */
    GUI_STREAM_CODEC_RLE,       /**< Reserved -- not implemented yet.            */
    GUI_STREAM_CODEC_H264,      /**< Placeholder -- not implemented yet.         */
} gui_stream_codec_t;

/**
 * @brief  Consumer-side frame-drop policy.
 *
 * Maps directly onto the transport primitives: NONE -> ::stp_acquire_oldest,
 * UNCONDITIONAL -> ::stp_acquire_latest.  The widget keeps its own policy and
 * never mutates the shared transport configuration.
 *
 * @note Unconditional drop is only safe for independently decodable frames
 *       (RAW / JPEG / intra-only).  For inter-coded streams (MSV1 / Cinepak)
 *       skipping frames corrupts decoder state, so the default is NONE.
 */
typedef enum
{
    GUI_STREAM_DROP_NONE = 0,        /**< Default: oldest-first, never drop.     */
    GUI_STREAM_DROP_UNCONDITIONAL,   /**< Jump to newest, recycle older frames.  */
} gui_stream_drop_mode_t;

/**
 * @brief  gui_stream widget -- receives encoded frames from a stream_transport
 *         and renders the decoded picture.
 *
 * Architecture (mirrors gui_lite_video):
 *  - Inherits gui_obj_t (base object).
 *  - Owns a child gui_img_t for on-screen rendering.
 *  - An internal timer fires every @c update_interval ms; when frames are
 *    waiting it flags a pull and marks the framebuffer dirty.
 *  - On draw the widget acquires one frame from the transport (oldest or
 *    newest per @c drop_mode), decodes / copies it into @c render_buf, hands
 *    that buffer to the child image, then releases the transport buffer back
 *    to its free pool.
 *
 * The transport is created and owned by the application; the widget only
 * references it and must not outlive it.  Destroying the widget does NOT
 * destroy the transport.
 */
typedef struct gui_stream
{
    gui_obj_t            base;
    gui_img_t           *img;           /**< child image widget for rendering  */

    stp_transport_t     *src;           /**< external transport (referenced)   */

    uint8_t              codec;         /**< gui_stream_codec_t                 */
    uint8_t              drop_mode;     /**< gui_stream_drop_mode_t             */
    uint8_t              state;         /**< GUI_VIDEO_STATE_*                  */
    uint8_t              raw_format;    /**< GUI_FormatType, RAW codec only     */

    gui_rgb_data_head_t  header;        /**< image header template             */
    uint8_t             *render_buf;    /**< owned: header + decoded pixels     */
    uint32_t             buf_pixel_sz;  /**< pixel-area bytes of render_buf     */
    uint8_t             *jpeg_cur;      /**< JPEG: buffer currently on screen   */

    union
    {
        struct { msv1_decoder_t    *dec; } msv1;     /**< 16-bpp, no palette    */
        struct { cinepak_decoder_t *dec; } cinepak;
    } u;

    uint32_t             update_interval; /**< ms between frame pulls           */
    bool                 pending_pull;    /**< timer requested a pull           */
    bool                 have_frame;      /**< at least one frame displayed     */
    bool                 placeholder_warned; /**< one-shot "not implemented" log */
} gui_stream_t;

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Create a stream widget bound to an existing transport.
 *
 * The widget size and codec are fixed here (a live stream carries no container
 * header to learn them from).  For MSV1 / Cinepak both @p w and @p h must be a
 * multiple of 4.
 *
 * @param parent  Parent GUI object (must not be NULL).
 * @param name    Widget name string (NULL -> "STREAM").
 * @param codec   Encoded frame format (::gui_stream_codec_t).
 * @param src     Transport to consume from (must not be NULL; app-owned).
 * @param x       Left edge relative to parent.
 * @param y       Top edge relative to parent.
 * @param w       Frame width in pixels.
 * @param h       Frame height in pixels.
 * @return        Widget pointer, or NULL on failure.
 */
gui_stream_t *gui_stream_create(void *parent, const char *name,
                                gui_stream_codec_t codec, stp_transport_t *src,
                                int16_t x, int16_t y, int16_t w, int16_t h);

/**
 * @brief Set the frame-pull interval in milliseconds (default 40 ms / 25 fps).
 *        Re-arms the internal timer immediately.
 */
void gui_stream_set_update_interval(gui_stream_t *this, uint32_t interval_ms);

/**
 * @brief Select the consumer-side drop policy (default ::GUI_STREAM_DROP_NONE).
 */
void gui_stream_set_drop_mode(gui_stream_t *this, gui_stream_drop_mode_t mode);

/**
 * @brief Set playback state.  Only ::GUI_VIDEO_STATE_PLAYING pulls frames;
 *        ::GUI_VIDEO_STATE_PAUSE freezes on the last decoded frame.
 */
void gui_stream_set_state(gui_stream_t *this, GUI_VIDEO_STATE state);

/**
 * @brief Get the current playback state.
 */
GUI_VIDEO_STATE gui_stream_get_state(gui_stream_t *this);

/**
 * @brief Set the pixel format of RAW frames (default ::RGB565).
 *
 * Only meaningful for ::GUI_STREAM_CODEC_RAW; ignored otherwise.  Reallocates
 * the render buffer when the bytes-per-pixel changes.  Call right after create,
 * before frames start flowing.
 */
void gui_stream_set_raw_format(gui_stream_t *this, GUI_FormatType format);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_STREAM_H__ */
