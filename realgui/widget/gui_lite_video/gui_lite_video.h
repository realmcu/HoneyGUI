/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_LITE_VIDEO_H__
#define __GUI_LITE_VIDEO_H__

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
#include "gui_video.h"       /* GUI_VIDEO_STATE / GUI_VIDEO_REPEAT_INFINITE */
#include "msv1_decoder.h"
#include "cinepak_decoder.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/**
 * @brief  Codec type tag stored in gui_lite_video_t::codec.
 */
typedef enum
{
    LITE_VIDEO_CODEC_UNKNOWN = 0,   /**< Not yet detected or parse failed.     */
    LITE_VIDEO_CODEC_MSV1,          /**< Microsoft Video 1 (CRAM / MSVC).     */
    LITE_VIDEO_CODEC_CINEPAK,       /**< Cinepak (CVID).                       */
} lite_video_codec_t;

/**
 * @brief  gui_lite_video widget -- plays an AVI file encoded with either
 *         Microsoft Video 1 (MSV1) or Cinepak (CVID).
 *
 *  The codec is detected automatically when the AVI source is parsed.
 *  The widget interface is identical for both codecs; no codec-specific
 *  code is required in application code.
 *
 * Architecture:
 *  - Inherits gui_obj_t (base object)
 *  - Creates a child gui_img_t for on-screen rendering
 *  - Internal timer advances frame_cur every frame_time ms
 *  - AVI chunks are indexed at creation time; each draw call reads one
 *    chunk, decodes it with the appropriate codec, and updates gui_img.
 *  - Frame-skip protection ensures the codec state machine stays
 *    consistent when the render rate falls behind the play rate.
 */
typedef struct gui_lite_video
{
    gui_obj_t            base;
    gui_img_t           *img;           /**< child image widget for rendering    */

    void                *data;          /**< source: RAM ptr, FTL base, or path  */
    uint32_t             num_frame;     /**< total video frame count             */
    uint8_t              storage_type;  /**< IMG_SRC_MEMADDR/FTL/FILESYS         */

    gui_rgb_data_head_t  header;        /**< RGB565 image header template        */
    uint8_t             *render_buf;    /**< header + RGB565 pixels              */

    uint8_t              codec;         /**< lite_video_codec_t, set at parse    */

    union
    {
        struct
        {
            msv1_decoder_t  *dec;
            uint16_t         palette565[256]; /**< pre-built palette (8-bpp only)  */
            uint8_t          bpp;             /**< 8 or 16, from BITMAPINFOHEADER  */
        } msv1;

        struct
        {
            cinepak_decoder_t *dec;
        } cinepak;
    } u;

    uint32_t             frame_time;    /**< ms between frames                   */
    uint32_t             frame_step;    /**< frames to advance per tick          */
    int32_t              frame_cur;     /**< current frame index (-1 = unstarted) */
    int32_t              frame_last;    /**< last rendered frame                 */
    int32_t              repeat_cnt;    /**< remaining repeats; -1 = infinite    */

    uint8_t              state;         /**< GUI_VIDEO_STATE_*                   */

    void                *chunks;        /**< LTV_chunk_t[], indexed at init       */
    uint32_t             chunk_num;     /**< entry count in chunks[]             */
    uint32_t             frame_chunk_cur; /**< chunk index of last decoded frame */
} gui_lite_video_t;

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Create a lite-video widget from a memory buffer (AVI file in RAM/Flash).
 *
 * The codec (MSV1 or Cinepak) is detected automatically from the AVI header.
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
gui_lite_video_t *gui_lite_video_create_from_mem(void *parent, const char *name,
                                                 void *addr,
                                                 int16_t x, int16_t y,
                                                 int16_t w, int16_t h);

/**
 * @brief Create a lite-video widget from the virtual filesystem.
 *
 * @param parent  Parent GUI object.
 * @param name    Widget name string.
 * @param path    NUL-terminated VFS path (e.g. "/video/sample.avi").
 * @param x       Left edge position relative to parent.
 * @param y       Top edge position relative to parent.
 * @param w       Initial width hint.
 * @param h       Initial height hint.
 * @return        Pointer to the widget, or NULL on failure.
 */
gui_lite_video_t *gui_lite_video_create_from_fs(void *parent, const char *name,
                                                void *path,
                                                int16_t x, int16_t y,
                                                int16_t w, int16_t h);

/**
 * @brief Create a lite-video widget from FTL (Flash Translation Layer) storage.
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
gui_lite_video_t *gui_lite_video_create_from_ftl(void *parent, const char *name,
                                                 void *addr,
                                                 int16_t x, int16_t y,
                                                 int16_t w, int16_t h);

/**
 * @brief Set playback state.  STOP -> PLAYING resets to the first frame.
 */
void gui_lite_video_set_state(gui_lite_video_t *this, GUI_VIDEO_STATE state);

/**
 * @brief Get current playback state.
 */
GUI_VIDEO_STATE gui_lite_video_get_state(gui_lite_video_t *this);

/**
 * @brief Set the loop / repeat count.
 *
 * @param cnt  0 = play once; >0 = play cnt additional times;
 *             GUI_VIDEO_REPEAT_INFINITE (-1) = loop forever.
 */
void gui_lite_video_set_repeat_count(gui_lite_video_t *this, int32_t cnt);

/**
 * @brief Override playback frame rate.
 *
 * @param fps  Frames per second (must be > 0).
 */
void gui_lite_video_set_frame_rate(gui_lite_video_t *this, float fps);

/**
 * @brief Set the frame advance step (default 1).
 *
 * @param step  Number of frames to advance per timer tick.
 */
void gui_lite_video_set_frame_step(gui_lite_video_t *this, uint32_t step);

/**
 * @brief Apply a uniform scale to the rendered image.
 */
void gui_lite_video_set_scale(gui_lite_video_t *this, float scale_x, float scale_y);

/**
 * @brief Switch the video source at runtime.
 *
 * Supports any codec-to-codec transition (MSV1<->Cinepak<->same).
 * The new AVI is parsed, the appropriate decoder is reset or replaced,
 * and playback restarts from frame 0.
 *
 * The render buffer is freed and reallocated only when the new video's
 * frame dimensions differ.  realloc is never used.
 *
 * On any failure the widget's current source and state are unchanged.
 *
 * @param this         Widget pointer (must not be NULL).
 * @param src          New source: RAM/Flash pointer, FTL base address, or
 *                     NUL-terminated VFS path string.
 * @param storage_type IMG_SRC_MEMADDR, IMG_SRC_FTL, or IMG_SRC_FILESYS.
 */
void gui_lite_video_set_src(gui_lite_video_t *this, void *src, uint8_t storage_type);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_LITE_VIDEO_H__ */
