/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "gui_stream.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_obj_event.h"
#include "gui_api.h"
#include "acc_sw_jpeg.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/

#define STREAM_DEFAULT_FPS   25u
#define STREAM_HEAD_SZ       ((uint32_t)sizeof(gui_rgb_data_head_t))

/*============================================================================*
 *                    H.264 streaming decoder (weak stubs)
 *
 *  Resolved to the real implementation in realgui/3rd/h264bsd/src/gui_h264bsd.c
 *  when CONFIG_REALTEK_H264BSD is enabled; otherwise these no-op stubs let the
 *  widget link and the H264 path degrades gracefully (create fails / decode
 *  returns nonzero -> keep the previous frame).  Distinct names from the
 *  file-oriented gui_h264bsd_* weak stubs in gui_video.c so the two never clash.
 *============================================================================*/
__attribute__((weak)) void *gui_h264bsd_create_decoder_stream(void)
{
    gui_log("h264bsd decoder is not enabled\n");
    return NULL;
}
__attribute__((weak)) int gui_h264bsd_decode_au(void *gui_decoder, const uint8_t *au,
                                                uint32_t au_len, uint8_t *frame_buff,
                                                uint32_t buff_size)
{
    GUI_UNUSED(gui_decoder); GUI_UNUSED(au); GUI_UNUSED(au_len);
    GUI_UNUSED(frame_buff);  GUI_UNUSED(buff_size);
    return -1;
}
__attribute__((weak)) int gui_h264bsd_destroy_stream(void *gui_decoder)
{
    GUI_UNUSED(gui_decoder);
    return 0;
}
extern void *gui_h264bsd_create_decoder_stream(void);
extern int gui_h264bsd_decode_au(void *gui_decoder, const uint8_t *au, uint32_t au_len,
                                 uint8_t *frame_buff, uint32_t buff_size);
extern int gui_h264bsd_destroy_stream(void *gui_decoder);

/*============================================================================*
 *                    Private: Helpers
 *============================================================================*/

/* Bytes per pixel for a render target of the given engine format. */
static uint8_t stream_format_bytes(uint8_t fmt)
{
    switch (fmt)
    {
    case RGB565:      return 2;
    case ARGB8565:    return 3;
    case RGB888:      return 3;
    case ARGB8888:    return 4;
    case XRGB8888:    return 4;
    case GRAY:        return 1;
    case RTKARGB8565: return 3;   /* planar A8 + RGB565 = 3 bytes / pixel */
    default:          return 2;
    }
}

/* Buffer that is currently meant to be on screen (black placeholder until the
 * first frame; the JPEG path tracks its own decoder allocation). */
static const uint8_t *stream_disp_buf(gui_stream_t *this)
{
    if (this->codec == GUI_STREAM_CODEC_JPEG && this->jpeg_cur)
    {
        return this->jpeg_cur;
    }
    return this->render_buf;
}

static void stream_blit_current(gui_stream_t *this)
{
    const uint8_t *disp = stream_disp_buf(this);
    gui_img_set_src(this->img, disp, IMG_SRC_MEMADDR);
    gui_img_refresh_draw_data(this->img);
}

/*============================================================================*
 *                    Private: Decode one acquired frame
 *
 *  Reads the encoded bytes out of @p f into the widget-owned buffer.  By the
 *  time this returns the transport buffer is no longer needed and is released
 *  by the caller.  For JPEG the decoded buffer is returned via @p out_jpeg
 *  (a fresh allocation that becomes the new on-screen buffer).
 *
 *  Returns true when a displayable frame was produced.
 *============================================================================*/
static bool stream_decode_frame(gui_stream_t *this, const stp_frame_t *f,
                                uint8_t **out_jpeg)
{
    *out_jpeg = NULL;

    switch (this->codec)
    {
    case GUI_STREAM_CODEC_RAW:
        {
            uint32_t n = (f->used < this->buf_pixel_sz) ? f->used : this->buf_pixel_sz;
            memcpy(this->render_buf + STREAM_HEAD_SZ, f->addr, n);
            return true;
        }

    case GUI_STREAM_CODEC_MSV1:
        {
            uint16_t *dst = (uint16_t *)(this->render_buf + STREAM_HEAD_SZ);
            /* 16-bpp source -> RGB565, no palette (see gui_stream_codec_t). */
            return (msv1_decode_frame_rgb565(this->u.msv1.dec,
                                             (const uint8_t *)f->addr, f->used,
                                             dst, NULL) == 0);
        }

    case GUI_STREAM_CODEC_CINEPAK:
        {
            const void *px = cinepak_decode_frame(this->u.cinepak.dec,
                                                  (const uint8_t *)f->addr, f->used);
            return (px != NULL);
        }

    case GUI_STREAM_CODEC_JPEG:
        {
            int jw = 0, jh = 0, ch = 0;
            uint8_t *out = (uint8_t *)gui_acc_jpeg_load(f->addr, (int)f->used,
                                                        &jw, &jh, &ch);
            if (!out)
            {
                return false;
            }
            /* The loader leaves a valid image header in front of the pixels.
             * Fix it up for already-decoded data, mirroring gui_video: the
             * acc backend yields RGB565, the software fallback yields RGB888. */
            {
                gui_rgb_data_head_t *ph = (gui_rgb_data_head_t *)out;
                ph->type = (gui_get_acc() && gui_get_acc()->jpeg_load)
                           ? (char)RGB565 : (char)RGB888;
                ph->jpeg = false;
            }
            *out_jpeg = out;
            return true;
        }

    case GUI_STREAM_CODEC_H264:
        {
            int r = gui_h264bsd_decode_au(this->u.h264.dec,
                                          (const uint8_t *)f->addr, f->used,
                                          this->render_buf + STREAM_HEAD_SZ,
                                          this->buf_pixel_sz);
            return (r == 0);
        }

    case GUI_STREAM_CODEC_RLE:
    default:
        if (!this->placeholder_warned)
        {
            gui_log("gui_stream: codec %u not implemented yet\n",
                    (unsigned)this->codec);
            this->placeholder_warned = true;
        }
        return false;
    }
}

/*============================================================================*
 *                    Private: Widget Lifecycle Callbacks
 *============================================================================*/

/* Frame-pull timer -- runs on the GUI task (same thread as draw). */
static void stream_timer_cb(void *p)
{
    gui_stream_t *this = (gui_stream_t *)p;

    if (this->state != GUI_VIDEO_STATE_PLAYING) { return; }
    if (!this->src) { return; }
    if (stp_ready_count(this->src) == 0) { return; }  /* nothing waiting */

    this->pending_pull = true;
    gui_fb_change();
}

/* Prepare pass: arm touch hit-testing so the application's registered event
 * callbacks actually fire.  gui_obj_enable_event() relies on the live touch
 * info, so it must run every prepare (not once at construction), and it is a
 * no-op for events without a registered handler -- enabling the common touch
 * set here is therefore safe regardless of what the app attached via
 * gui_obj_add_event_cb().  Mirrors gui_lite_video's ltv_prepare(). */
static void stream_prepare(gui_obj_t *obj)
{
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED,        "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_DOUBLE_CLICKED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSING,       "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED,       "touch");
}

static void stream_draw(gui_obj_t *obj)
{
    gui_stream_t *this = (gui_stream_t *)obj;
    stp_frame_t   f;
    bool          got;
    bool          ok;
    uint8_t      *new_jpeg = NULL;

    /* No new frame this pass: re-emit whatever is already on screen. */
    if (!this->pending_pull)
    {
        stream_blit_current(this);
        return;
    }
    this->pending_pull = false;

    memset(&f, 0, sizeof(f));
    got = (this->drop_mode == GUI_STREAM_DROP_UNCONDITIONAL)
          ? stp_acquire_latest(this->src, 0, &f)
          : stp_acquire_oldest(this->src, 0, &f);
    if (!got)
    {
        stream_blit_current(this);
        return;
    }

    ok = stream_decode_frame(this, &f, &new_jpeg);

    /* Requirement: free the queue buffer once decode/copy is done -- the bytes
     * have been consumed into render_buf (or a fresh JPEG buffer) by now. */
    stp_release(this->src, &f);

    if (!ok)
    {
        stream_blit_current(this);   /* keep the previous good frame */
        return;
    }

    if (this->codec == GUI_STREAM_CODEC_JPEG)
    {
        gui_img_set_src(this->img, new_jpeg, IMG_SRC_MEMADDR);
        gui_img_refresh_draw_data(this->img);
        /* JPEG buffers come from the (possibly HW) loader: match its free. */
        if (this->jpeg_cur) { gui_acc_jpeg_free(this->jpeg_cur); }
        this->jpeg_cur = new_jpeg;
    }
    else
    {
        gui_img_set_src(this->img, this->render_buf, IMG_SRC_MEMADDR);
        gui_img_refresh_draw_data(this->img);
    }

    if (!this->have_frame)   /* one line per widget lifetime: decode is live */
    {
        gui_log("gui_stream '%s': first frame decoded (%dx%d, codec %u)\n",
                this->base.name ? this->base.name : "?",
                (int)this->header.w, (int)this->header.h,
                (unsigned)this->codec);
    }
    this->have_frame = true;
}

static void stream_destroy(gui_obj_t *obj)
{
    gui_stream_t *this = (gui_stream_t *)obj;

    if (this->codec == GUI_STREAM_CODEC_MSV1)
    {
        if (this->u.msv1.dec)
        {
            msv1_decoder_destroy(this->u.msv1.dec);
            this->u.msv1.dec = NULL;
        }
    }
    else if (this->codec == GUI_STREAM_CODEC_CINEPAK)
    {
        if (this->u.cinepak.dec)
        {
            cinepak_decoder_destroy(this->u.cinepak.dec);
            this->u.cinepak.dec = NULL;
        }
    }
    else if (this->codec == GUI_STREAM_CODEC_H264)
    {
        if (this->u.h264.dec)
        {
            gui_h264bsd_destroy_stream(this->u.h264.dec);
            this->u.h264.dec = NULL;
        }
    }

    if (this->jpeg_cur)
    {
        gui_acc_jpeg_free(this->jpeg_cur);
        this->jpeg_cur = NULL;
    }
    if (this->render_buf)
    {
        gui_free(this->render_buf);
        this->render_buf = NULL;
    }
    /* this->src is owned by the application; do not destroy it here. */
}

static void stream_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (!obj) { return; }

    switch (cb_type)
    {
    case OBJ_PREPARE: stream_prepare(obj); break;
    case OBJ_DRAW:    stream_draw(obj);    break;
    case OBJ_DESTROY: stream_destroy(obj); break;
    default: break;
    }
}

/*============================================================================*
 *                    Private: Constructor
 *============================================================================*/

static gui_stream_t *stream_ctor(gui_obj_t *parent, const char *name,
                                 uint8_t codec, stp_transport_t *src,
                                 int16_t x, int16_t y, int16_t w, int16_t h)
{
    gui_stream_t *this = (gui_stream_t *)gui_malloc(sizeof(gui_stream_t));
    if (!this) { gui_log("gui_stream: alloc failed\n"); return NULL; }
    memset(this, 0, sizeof(gui_stream_t));

    gui_obj_t *root = (gui_obj_t *)this;
    gui_obj_ctor(root, parent, name, x, y, w, h);
    root->type           = IMAGE_FROM_MEM;
    root->obj_cb         = stream_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb    = true;
    root->has_destroy_cb = true;

    this->src             = src;
    this->codec           = codec;
    this->drop_mode       = GUI_STREAM_DROP_NONE;
    this->state           = GUI_VIDEO_STATE_PLAYING;
    this->raw_format      = RGB565;
    this->update_interval = 1000u / STREAM_DEFAULT_FPS;

    /* Render buffer: header + pixels.  For JPEG this is only a black
     * placeholder (the decoder allocates its own output buffer per frame). */
    uint8_t bpp   = (codec == GUI_STREAM_CODEC_RAW)
                    ? stream_format_bytes(this->raw_format) : 2u;
    char    htype = (codec == GUI_STREAM_CODEC_RAW)
                    ? (char)this->raw_format : (char)RGB565;

    this->buf_pixel_sz = (uint32_t)w * (uint32_t)h * bpp;
    this->render_buf   = (uint8_t *)gui_malloc(STREAM_HEAD_SZ + this->buf_pixel_sz);
    if (!this->render_buf)
    {
        gui_log("gui_stream: render buf alloc failed\n");
        gui_free(this);
        return NULL;
    }
    memset(this->render_buf, 0, STREAM_HEAD_SZ + this->buf_pixel_sz);

    memset(&this->header, 0, sizeof(gui_rgb_data_head_t));
    this->header.type = htype;
    this->header.w    = w;
    this->header.h    = h;
    memcpy(this->render_buf, &this->header, sizeof(gui_rgb_data_head_t));

    /* Codec decoders that carry state across frames write into render_buf. */
    uint8_t *pixel_buf = this->render_buf + STREAM_HEAD_SZ;
    if (codec == GUI_STREAM_CODEC_MSV1)
    {
        this->u.msv1.dec = msv1_decoder_create((uint16_t)w, (uint16_t)h, 16, pixel_buf);
        if (!this->u.msv1.dec)
        {
            gui_log("gui_stream: MSV1 decoder create failed\n");
            gui_free(this->render_buf);
            gui_free(this);
            return NULL;
        }
    }
    else if (codec == GUI_STREAM_CODEC_CINEPAK)
    {
        this->u.cinepak.dec = cinepak_decoder_create((uint16_t)w, (uint16_t)h, pixel_buf);
        if (!this->u.cinepak.dec)
        {
            gui_log("gui_stream: Cinepak decoder create failed\n");
            gui_free(this->render_buf);
            gui_free(this);
            return NULL;
        }
    }
    else if (codec == GUI_STREAM_CODEC_H264)
    {
        /* Persistent decoder fed one Access Unit per frame.  It converts into
         * render_buf + head on each decode, so (unlike MSV1 / Cinepak) it is
         * not bound to pixel_buf at creation time. */
        this->u.h264.dec = gui_h264bsd_create_decoder_stream();
        if (!this->u.h264.dec)
        {
            gui_log("gui_stream: H264 decoder create failed\n");
            gui_free(this->render_buf);
            gui_free(this);
            return NULL;
        }
    }

    /* Register in the parent widget tree. */
    gui_list_init(&root->child_list);
    if (root->parent)
    {
        gui_list_insert_before(&root->parent->child_list, &root->brother_list);
    }

    /* Child image widget shows render_buf (black) until the first frame. */
    this->img = gui_img_create_from_mem(this, "STREAM_IMG",
                                        (void *)&this->header,
                                        0, 0, w, h);
    gui_img_set_mode(this->img, IMG_BYPASS_MODE);
    gui_img_set_src(this->img, this->render_buf, IMG_SRC_MEMADDR);
    gui_img_refresh_size(this->img);

    /* Frame-pull timer. */
    gui_obj_create_timer(root, this->update_interval, true, stream_timer_cb);

    return this;
}

/*============================================================================*
 *                        Public API
 *============================================================================*/

gui_stream_t *gui_stream_create(void *parent, const char *name,
                                gui_stream_codec_t codec, stp_transport_t *src,
                                int16_t x, int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (!name) { name = "STREAM"; }
    if (!src)  { gui_log("gui_stream: NULL transport\n"); return NULL; }

    if (codec == GUI_STREAM_CODEC_MSV1 || codec == GUI_STREAM_CODEC_CINEPAK)
    {
        if ((w & 3) || (h & 3))
        {
            gui_log("gui_stream: %s needs w/h multiple of 4\n",
                    (codec == GUI_STREAM_CODEC_MSV1) ? "MSV1" : "Cinepak");
            return NULL;
        }
    }

    gui_stream_t *widget = stream_ctor((gui_obj_t *)parent, name,
                                       (uint8_t)codec, src, x, y, w, h);
    if (widget) { GET_BASE(widget)->create_done = true; }
    return widget;
}

void gui_stream_set_update_interval(gui_stream_t *this, uint32_t interval_ms)
{
    if (!this || interval_ms == 0) { return; }
    this->update_interval = interval_ms;
    gui_obj_create_timer((gui_obj_t *)this, interval_ms, true, stream_timer_cb);
}

void gui_stream_set_drop_mode(gui_stream_t *this, gui_stream_drop_mode_t mode)
{
    if (!this) { return; }
    this->drop_mode = (uint8_t)mode;
}

void gui_stream_set_state(gui_stream_t *this, GUI_VIDEO_STATE state)
{
    if (!this) { return; }
    this->state = (uint8_t)state;
}

GUI_VIDEO_STATE gui_stream_get_state(gui_stream_t *this)
{
    if (!this) { return GUI_VIDEO_STATE_ERR; }
    return (GUI_VIDEO_STATE)this->state;
}

void gui_stream_set_raw_format(gui_stream_t *this, GUI_FormatType format)
{
    if (!this) { return; }
    if (this->codec != GUI_STREAM_CODEC_RAW)
    {
        gui_log("gui_stream: set_raw_format ignored (codec is not RAW)\n");
        return;
    }

    gui_obj_t *obj     = (gui_obj_t *)this;
    uint8_t    bpp     = stream_format_bytes((uint8_t)format);
    uint32_t   new_sz  = (uint32_t)obj->w * (uint32_t)obj->h * bpp;

    if (new_sz != this->buf_pixel_sz)
    {
        uint8_t *nb = (uint8_t *)gui_malloc(STREAM_HEAD_SZ + new_sz);
        if (!nb)
        {
            gui_log("gui_stream: raw buf realloc failed\n");
            return;
        }
        memset(nb, 0, STREAM_HEAD_SZ + new_sz);
        gui_free(this->render_buf);
        this->render_buf   = nb;
        this->buf_pixel_sz = new_sz;
    }

    this->raw_format  = (uint8_t)format;
    this->header.type = (char)format;
    this->header.w    = obj->w;
    this->header.h    = obj->h;
    memcpy(this->render_buf, &this->header, sizeof(gui_rgb_data_head_t));
    this->have_frame  = false;

    if (this->img)
    {
        gui_img_set_src(this->img, this->render_buf, IMG_SRC_MEMADDR);
        gui_img_refresh_size(this->img);
    }
}
