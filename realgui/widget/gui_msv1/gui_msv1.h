/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_MSV1_H__
#define __GUI_MSV1_H__

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
#include "gui_video.h"     /* re-use GUI_VIDEO_STATE / GUI_VIDEO_REPEAT_INFINITE */
#include "msv1_decoder.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/**
 * @brief  gui_msv1 widget -- plays an AVI file encoded with Microsoft Video 1
 *         (CRAM / MSVC codec, 8-bpp paletted or 16-bpp RGB555).
 *
 * Architecture mirrors gui_video:
 *  - Inherits gui_obj_t (base)
 *  - Creates a child gui_img_t for on-screen rendering
 *  - Internal timer advances frame_cur each frame_time ms
 *  - AVI chunks are indexed at creation time; each draw call reads one chunk,
 *    decodes it with msv1_decoder, converts to RGB565, and updates gui_img.
 */
typedef struct gui_msv1
{
    gui_obj_t            base;
    gui_img_t           *img;           /**< child image widget used for rendering      */

    void                *data;          /**< source: RAM pointer, FTL base, or FS path  */
    uint32_t             num_frame;     /**< total video frame count                    */
    uint8_t              storage_type;  /**< IMG_SRC_MEMADDR / IMG_SRC_FTL / IMG_SRC_FILESYS */

    gui_rgb_data_head_t  header;        /**< image header template (RGB565, w, h, ...)    */
    uint8_t             *render_buf;    /**< header + RGB565 pixels, allocated once      */

    msv1_decoder_t      *decoder;       /**< stateful MSV1 codec (retains prev frame)   */
    uint16_t             palette565[256]; /**< pre-built RGB565 palette (8-bpp only)    */
    uint8_t              bits_per_pixel;  /**< 8 or 16, from AVI BITMAPINFOHEADER       */

    uint32_t             frame_time;    /**< ms between frames (from AVI usec_per_frame) */
    uint32_t             frame_step;    /**< frames to advance per tick (default 1)     */
    int32_t              frame_cur;     /**< current frame index (-1 = not yet started) */
    int32_t              frame_last;    /**< last rendered frame (cache control)        */
    int32_t              repeat_cnt;    /**< remaining repeats; -1 = infinite           */

    uint8_t              state;         /**< GUI_VIDEO_STATE_*                          */

    void                *chunks;        /**< AviMoviChunk_t[], indexed at init          */
    uint32_t             chunk_num;     /**< number of entries in chunks[]              */
    uint32_t             frame_chunk_cur; /**< AVI chunk index of last non-empty V-frame */
} gui_msv1_t;

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Create an MSV1 widget from a memory buffer (AVI file in RAM/Flash).
 *
 * @param parent  Parent GUI object.
 * @param name    Widget name string.
 * @param addr    Pointer to the AVI data in memory.
 * @param x       Left edge position relative to parent.
 * @param y       Top edge position relative to parent.
 * @param w       Initial width hint (overwritten by the AVI frame size).
 * @param h       Initial height hint (overwritten by the AVI frame size).
 * @return        Pointer to the widget, or NULL on failure.
 */
gui_msv1_t *gui_msv1_create_from_mem(void *parent, const char *name, void *addr,
                                     int16_t x, int16_t y, int16_t w, int16_t h);

/**
 * @brief Create an MSV1 widget from the virtual filesystem.
 *
 * @param parent  Parent GUI object.
 * @param name    Widget name string.
 * @param path    File path string in the VFS (e.g. "/video/sample.avi").
 * @param x       Left edge position relative to parent.
 * @param y       Top edge position relative to parent.
 * @param w       Initial width hint.
 * @param h       Initial height hint.
 * @return        Pointer to the widget, or NULL on failure.
 */
gui_msv1_t *gui_msv1_create_from_fs(void *parent, const char *name, void *path,
                                    int16_t x, int16_t y, int16_t w, int16_t h);

/**
 * @brief Create an MSV1 widget from FTL (Flash Translation Layer) storage.
 *
 * @param parent  Parent GUI object.
 * @param name    Widget name string.
 * @param addr    FTL base address of the AVI data.
 * @param x       Left edge position relative to parent.
 * @param y       Top edge position relative to parent.
 * @param w       Initial width hint.
 * @param h       Initial height hint.
 * @return        Pointer to the widget, or NULL on failure.
 */
gui_msv1_t *gui_msv1_create_from_ftl(void *parent, const char *name, void *addr,
                                     int16_t x, int16_t y, int16_t w, int16_t h);

/**
 * @brief Set playback state.  Transitioning from STOP -> PLAYING resets to
 *        the first frame.
 *
 * @param this   Widget pointer.
 * @param state  Target state (GUI_VIDEO_STATE_PLAYING / _PAUSE / _STOP).
 */
void gui_msv1_set_state(gui_msv1_t *this, GUI_VIDEO_STATE state);

/**
 * @brief Get current playback state.
 */
GUI_VIDEO_STATE gui_msv1_get_state(gui_msv1_t *this);

/**
 * @brief Set the loop / repeat count.
 *
 * @param this  Widget pointer.
 * @param cnt   0 = play once (no extra repeats);
 *              >0 = play cnt additional times after the first;
 *              GUI_VIDEO_REPEAT_INFINITE = loop forever.
 */
void gui_msv1_set_repeat_count(gui_msv1_t *this, int32_t cnt);

/**
 * @brief Override playback frame rate.
 *
 * @param this  Widget pointer.
 * @param fps   Frames per second (must be > 0).
 */
void gui_msv1_set_frame_rate(gui_msv1_t *this, float fps);

/**
 * @brief Set the frame advance step (default 1).
 *
 * @param this  Widget pointer.
 * @param step  Number of frames to skip per tick.
 */
void gui_msv1_set_frame_step(gui_msv1_t *this, uint32_t step);

/**
 * @brief Apply a uniform scale to the rendered image.
 *
 * @param this     Widget pointer.
 * @param scale_x  Horizontal scale factor.
 * @param scale_y  Vertical scale factor.
 */
void gui_msv1_set_scale(gui_msv1_t *this, float scale_x, float scale_y);

/**
 * @brief Switch the video source at runtime.
 *
 * Handles all combinations of storage-type transitions
 * (MEMADDR <-> FTL <-> FILESYS).  The AVI container is re-parsed from
 * the new source, all widget state is updated, and playback restarts from
 * the first frame.
 *
 * Buffer management:
 *  - The render pixel buffer is freed and reallocated only when the new
 *    video's frame dimensions differ from the current ones.  When the
 *    dimensions are identical the existing buffer is reused (zero-filled).
 *    realloc is never used.
 *  - The existing msv1_decoder_t struct is reused (not destroyed / recreated).
 *
 * On failure the widget's current source and state are left unchanged.
 *
 * @param this         Widget pointer (must not be NULL).
 * @param src          New source: RAM/Flash pointer, FTL base address, or
 *                     NUL-terminated VFS path string.
 * @param storage_type IMG_SRC_MEMADDR, IMG_SRC_FTL, or IMG_SRC_FILESYS.
 */
void gui_msv1_set_src(gui_msv1_t *this, void *src, uint8_t storage_type);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_MSV1_H__ */
