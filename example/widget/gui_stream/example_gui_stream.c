/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file  example_gui_stream.c
 * @brief gui_stream widget demo -- a producer feeds a transport, a widget
 *        (the consumer) renders it.  One source is chosen at compile time.
 *
 * The example is split into two self-contained halves so each end can be read
 * and reused on its own:
 *
 *   PRODUCER SIDE  (search "PRODUCER SIDE")
 *       Owns the encoded byte source, cuts it into frames and pushes them into
 *       the transport with the two producer-only calls:
 *           stp_acquire_free()  -> borrow a free pool buffer
 *           stp_commit()        -> publish the filled buffer
 *       producer_post_frame() is the canonical "send one frame" pattern.
 *       This side never touches the widget.
 *
 *   CONSUMER SIDE  (search "CONSUMER SIDE")
 *       Creates and configures the gui_stream widget, which pulls frames out of
 *       the transport and renders them.  consumer_create_widget() is all an app
 *       needs.  This side never touches the producer.
 *
 * The ONLY thing the two ends share is the stp_transport_t* handle: app_init()
 * creates it once via stp_instance_create() and hands the same pointer to each
 * end.  Start order does not matter -- the transport is an SPSC queue with its
 * own buffering and back-pressure.  Tap the video to toggle Play / Pause.
 *
 * Sources:
 *   STREAM_DEMO_DUCK  -- duck.avi    -> Microsoft Video 1, 272x272 (inter-frame)
 *   STREAM_DEMO_EARTH -- earth MJPEG -> baseline JPEG, 410x502     (intra-frame)
 *
 * ===========================================================================
 *  Where the transport comes from
 * ===========================================================================
 *  The application owns the transport lifecycle.  app_init() fills an
 *  stp_config_t, calls stp_instance_create() (which allocates the frame pool
 *  internally via gui_malloc, sized from the size classes), hands the handle to
 *  the widget, and on teardown would call stp_instance_destroy():
 *
 *      stp_config_t cfg;            // size classes + drop policy
 *           |  stp_instance_create(&cfg)
 *           v
 *      stp_transport_t *tp  ----> gui_stream_create(.., tp, ..)   (consumer)
 *                           \---> producer thread                 (producer)
 *
 * ===========================================================================
 *  stp_config_t field reference / tuning
 * ===========================================================================
 *  align               Buffer start alignment in bytes (0/1 = none).  8 suits
 *                      most DMA / decoder needs.
 *  classes,            One or more { buf_size, buf_count } size classes, listed
 *  class_count         in ascending buf_size.  The pool is sized from these
 *                      (sum of buf_size * buf_count + alignment slack).  Use ONE
 *                      class for the common "every frame <= one max size" case.
 *    .buf_size         Must be >= the largest frame passed to stp_commit;
 *                      bigger frames are dropped (here MAX_FRAME).
 *    .buf_count        Ring depth = frames allowed in flight (produced but not
 *                      yet rendered).  Deeper = fewer drops under jitter, more
 *                      RAM.  >= 2; 4..16 is typical.
 *  drop_mode           Match it to the codec:
 *                        UNCONDITIONAL  intra-only (JPEG/MJPEG): jump to newest
 *                                       ready frame.  Used by EARTH.
 *                        CONDITIONAL    inter-frame (MSV1/Cinepak): catch up
 *                                       only to a key frame so prediction stays
 *                                       valid.  Recommended for DUCK.
 *                        NONE           never drop; strict oldest-first.
 *  allow_oversize_fit  When the best-fit class is empty, borrow from a larger
 *                      class instead of dropping.  Usually true.
 *  keep_cond,          CONDITIONAL only: predicate marking a safe catch-up
 *  keep_cond_user      point.  When NULL the stp_frame_t::is_keyframe hint is
 *                      used instead.
 *
 *  This demo uses a single MAX_FRAME class at depth APP_STREAM_BUF_COUNT.  Its
 *  transport drop_mode follows the codec; the widget keeps its own catch-up
 *  policy (see consumer_drop_mode_for).
 */

#include <string.h>
#include "guidef.h"
#include "gui_obj.h"
#include "gui_api.h"
#include "gui_components_init.h"
#include "gui_obj_event.h"
#include "gui_stream.h"
#include "stream_transport.h"   /* stp_config_t + stp_instance_create() */

/*============================================================================*
 *                         Demo stream selection
 *
 * Set STREAM_DEMO_SELECTION to exactly one of the values below.
 * Define it on the compiler command line to override the default.
 *============================================================================*/

#define STREAM_DEMO_DUCK    1   /**< MSV1 via AVI  (inter-frame, order-sensitive) */
#define STREAM_DEMO_EARTH   2   /**< JPEG via MJPEG (intra-frame, drop-safe)      */

#ifndef STREAM_DEMO_SELECTION
#define STREAM_DEMO_SELECTION  STREAM_DEMO_EARTH
#endif

#if (STREAM_DEMO_SELECTION != STREAM_DEMO_DUCK) && \
    (STREAM_DEMO_SELECTION != STREAM_DEMO_EARTH)
#error "STREAM_DEMO_SELECTION must be exactly STREAM_DEMO_DUCK or STREAM_DEMO_EARTH"
#endif

/*============================================================================*
 *                           Binary resource binding
 *============================================================================*/

#if (STREAM_DEMO_SELECTION == STREAM_DEMO_DUCK)

#ifdef _HONEYGUI_SIMULATOR_
extern const unsigned char _binary_duck_avi_start[];
#else
#define STREAM_DEMO_FLASH_ADDR   0x240F400UL
#endif

#else /* STREAM_DEMO_EARTH */

#ifdef _HONEYGUI_SIMULATOR_
extern const unsigned char _binary_earth_420_410_502_40_lq_mjpg_start[];
extern const unsigned char _binary_earth_420_410_502_40_lq_mjpg_end[];
#else
#define STREAM_DEMO_FLASH_ADDR   0x7004D100UL
#define STREAM_DEMO_FLASH_SIZE   5574973UL
#endif

#endif /* STREAM_DEMO_SELECTION */

/*============================================================================*
 *                             Demo configuration
 *============================================================================*/

#define DEMO_SCREEN_H   502u    /* matches DRV_LCD_HEIGHT in SConscript  */

#if (STREAM_DEMO_SELECTION == STREAM_DEMO_EARTH)
#define EARTH_W         410u
#define EARTH_H         502u
#define EARTH_FPS       30u
#endif

#define MAX_FRAME       (64u * 1024u)   /* per-buffer cap in the OS-port pool */

/*============================================================================*
 *                         Shared stream contract
 *
 * The read-only description both ends agree on.  The producer paces its output
 * at this fps; the consumer lays out w/h, selects the codec decoder and paces
 * its own pulls from the same fps.  Neither end mutates it, so it is NOT shared
 * mutable state -- it is a contract passed by const pointer to each end.
 *============================================================================*/

typedef struct
{
    uint16_t           w;
    uint16_t           h;
    uint32_t           fps_num;
    uint32_t           fps_den;
    gui_stream_codec_t codec;
} stream_spec_t;

/* Per-frame interval in ms derived from the spec fps (clamped to >= 1).  Both
 * ends call this to compute their own pacing independently. */
static uint32_t spec_interval_ms(const stream_spec_t *spec)
{
    uint32_t ms = (1000u * spec->fps_den) / spec->fps_num;
    return (ms == 0u) ? 1u : ms;
}

/*============================================================================*
 *                                                                            *
 *                              PRODUCER SIDE                                 *
 *                                                                            *
 *  Reads an encoded byte source, cuts it into frames and pushes them into    *
 *  the transport.  Depends only on stp_transport_t (stp_acquire_free /        *
 *  stp_commit) -- it has no knowledge of the gui_stream widget.              *
 *                                                                            *
 *============================================================================*/

/*--------------------- AVI demux helpers (DUCK source) ----------------------*/

#if (STREAM_DEMO_SELECTION == STREAM_DEMO_DUCK)

#define FOURCC(a, b, c, d) \
    ((uint32_t)(uint8_t)(a)         | ((uint32_t)(uint8_t)(b) << 8) | \
     ((uint32_t)(uint8_t)(c) << 16) | ((uint32_t)(uint8_t)(d) << 24))

#define CC_RIFF   FOURCC('R','I','F','F')
#define CC_AVI    FOURCC('A','V','I',' ')
#define CC_LIST   FOURCC('L','I','S','T')
#define CC_hdrl   FOURCC('h','d','r','l')
#define CC_movi   FOURCC('m','o','v','i')
#define CC_strh   FOURCC('s','t','r','h')
#define CC_strf   FOURCC('s','t','r','f')
#define CC_vids   FOURCC('v','i','d','s')
#define CC_00dc   FOURCC('0','0','d','c')
#define CC_CRAM   FOURCC('C','R','A','M')
#define CC_MSVC   FOURCC('M','S','V','C')
#define CC_wmsv   FOURCC('w','m','s','v')
#define CC_CVID   FOURCC('C','V','I','D')
#define CC_cvid   FOURCC('c','v','i','d')

static uint32_t rd32(const uint8_t *p)
{
    uint32_t v;
    memcpy(&v, p, sizeof(v));
    return v;
}

/* Intermediate parse result: geometry, fps and the 'movi' payload region.
 * Consumed by source_open() to fill the spec and the producer's source. */
typedef struct
{
    const uint8_t *movi;
    uint32_t       movi_size;
    uint16_t       w;
    uint16_t       h;
    uint32_t       fourcc;
    uint32_t       fps_num;
    uint32_t       fps_den;
} avi_info_t;

static bool avi_parse(const uint8_t *data, avi_info_t *out)
{
    if (rd32(data) != CC_RIFF || rd32(data + 8) != CC_AVI)
    {
        return false;
    }

    uint32_t file_size = rd32(data + 4) + 8u;
    uint32_t strh_off = 0, strf_off = 0, movi_data = 0, movi_size = 0;
    uint32_t pos = 12;

    while (pos + 8u <= file_size)
    {
        uint32_t cc = rd32(data + pos);
        uint32_t sz = rd32(data + pos + 4);

        if (cc == CC_LIST)
        {
            uint32_t lt = rd32(data + pos + 8);

            if (lt == CC_movi)
            {
                movi_data = pos + 12u;
                movi_size = (sz >= 4u) ? (sz - 4u) : 0u;
            }
            else if (lt == CC_hdrl)
            {
                uint32_t hdrl_end = pos + 8u + sz;
                uint32_t q = pos + 12u;

                while (q + 8u <= hdrl_end && q + 8u <= file_size)
                {
                    uint32_t c2 = rd32(data + q);
                    uint32_t s2 = rd32(data + q + 4);

                    if (c2 == CC_LIST)
                    {
                        q += 12u;
                        continue;
                    }
                    if (c2 == CC_strh && strh_off == 0 &&
                        rd32(data + q + 8) == CC_vids)
                    {
                        strh_off = q + 8u;
                    }
                    else if (c2 == CC_strf && strf_off == 0 && strh_off != 0)
                    {
                        strf_off = q + 8u;
                    }
                    q += 8u + s2 + (s2 & 1u);
                }
            }
        }
        pos += 8u + sz + (sz & 1u);
    }

    if (movi_data == 0 || strf_off == 0)
    {
        return false;
    }

    out->movi      = data + movi_data;
    out->movi_size = movi_size;
    out->w         = (uint16_t)rd32(data + strf_off + 4);
    out->h         = (uint16_t)rd32(data + strf_off + 8);
    out->fourcc    = rd32(data + strf_off + 16);

    if (strh_off != 0)
    {
        uint32_t scale = rd32(data + strh_off + 20);
        uint32_t rate  = rd32(data + strh_off + 24);
        out->fps_num = (rate  != 0) ? rate  : 30u;
        out->fps_den = (scale != 0) ? scale : 1u;
    }
    else
    {
        out->fps_num = 30u;
        out->fps_den = 1u;
    }
    return true;
}

static gui_stream_codec_t avi_codec(uint32_t fourcc)
{
    if (fourcc == CC_MSVC || fourcc == CC_CRAM || fourcc == CC_wmsv)
    {
        return GUI_STREAM_CODEC_MSV1;
    }
    if (fourcc == CC_cvid || fourcc == CC_CVID)
    {
        return GUI_STREAM_CODEC_CINEPAK;
    }
    return GUI_STREAM_CODEC_RAW;
}

#endif /* STREAM_DEMO_DUCK */

/*--------------------------- Producer runtime -------------------------------*/

/* Producer-private state handed to the producer thread.  src/src_len point at
 * the encoded byte source; tp + interval_ms drive how frames are emitted. */
typedef struct
{
    stp_transport_t *tp;          /* shared transport (producer endpoint)     */
    const uint8_t   *src;         /* encoded source bytes                     */
    uint32_t         src_len;     /* length of the encoded source             */
    uint32_t         interval_ms; /* inter-frame pacing                       */
    volatile bool    running;     /* producer lifecycle flag                  */
} stream_producer_t;

static stream_producer_t s_producer;

/*
 * Canonical "send one encoded frame" primitive -- the only transport contact a
 * producer needs:
 *
 *     stp_acquire_free() -> memcpy payload -> stp_commit()
 *
 * If every pool buffer is still in flight (consumer behind), wait one frame
 * interval and retry so committed data is never overwritten.  Bails out if the
 * producer was asked to stop.
 */
static void producer_post_frame(stream_producer_t *prod, const uint8_t *payload,
                                uint32_t sz, bool keyframe)
{
    stp_frame_t f;

    while (prod->running)
    {
        if (stp_acquire_free(prod->tp, sz, &f))
        {
            memcpy(f.addr, payload, sz);              /* fill the buffer      */
            stp_commit(prod->tp, &f, sz, keyframe);   /* publish to consumer  */
            return;
        }
        gui_thread_mdelay(prod->interval_ms);         /* back-pressure        */
    }
}

/*------------------------ Source-specific producer --------------------------*/

#if (STREAM_DEMO_SELECTION == STREAM_DEMO_DUCK)

/* Walk the AVI 'movi' region, emitting each '00dc' chunk as one frame.  The
 * first chunk after a (re)start is tagged as a key frame. */
static void avi_producer_entry(void *param)
{
    stream_producer_t *prod = (stream_producer_t *)param;
    const uint8_t     *movi = prod->src;
    const uint8_t     *end  = movi + prod->src_len;
    const uint8_t     *cur  = movi;
    bool               keyframe = true;

    while (prod->running)
    {
        if (cur + 8u > end)
        {
            cur = movi;
            keyframe = true;
            continue;
        }

        uint32_t cc = rd32(cur);
        uint32_t sz = rd32(cur + 4);

        if (cc == CC_LIST)
        {
            cur += 12u;
            continue;
        }

        if (cc == CC_00dc && sz > 0u && sz <= MAX_FRAME)
        {
            producer_post_frame(prod, cur + 8u, sz, keyframe);
            keyframe = false;
            gui_thread_mdelay(prod->interval_ms);
        }

        cur += 8u + sz + (sz & 1u);
    }
}

/* Locate the AVI bytes, demux geometry/fps/codec into the spec and stash the
 * 'movi' payload region as the producer's source. */
static bool source_open(stream_producer_t *prod, stream_spec_t *spec)
{
#ifdef _HONEYGUI_SIMULATOR_
    const uint8_t *avi = (const uint8_t *)_binary_duck_avi_start;
#else
    const uint8_t *avi = (const uint8_t *)STREAM_DEMO_FLASH_ADDR;
#endif

    avi_info_t info;
    if (!avi_parse(avi, &info))
    {
        gui_log("stream demo: AVI parse failed\n");
        return false;
    }

    gui_stream_codec_t codec = avi_codec(info.fourcc);
    if (codec == GUI_STREAM_CODEC_RAW)
    {
        gui_log("stream demo: unsupported fourcc 0x%08X\n", (unsigned)info.fourcc);
        return false;
    }

    spec->w       = info.w;
    spec->h       = info.h;
    spec->fps_num = info.fps_num;
    spec->fps_den = info.fps_den;
    spec->codec   = codec;

    prod->src     = info.movi;
    prod->src_len = info.movi_size;
    return true;
}

#define PRODUCER_ENTRY  avi_producer_entry
#define DEMO_NAME       "stream_msv1"
#define DEMO_LABEL      "duck-MSV1"

#else /* STREAM_DEMO_EARTH */

#define JPEG_MARK   0xFF
#define JPEG_SOI    0xD8
#define JPEG_EOI    0xD9

/* Scan the MJPEG blob for SOI(FFD8)..EOI(FFD9) boundaries, emitting each JPEG
 * image as one (always-decodable) key frame. */
static void mjpeg_producer_entry(void *param)
{
    stream_producer_t *prod = (stream_producer_t *)param;
    const uint8_t     *base = prod->src;
    const uint8_t     *end  = base + prod->src_len;
    const uint8_t     *cur  = base;

    while (prod->running)
    {
        while (cur + 1 < end && !(cur[0] == JPEG_MARK && cur[1] == JPEG_SOI))
        {
            cur++;
        }
        if (cur + 1 >= end)
        {
            cur = base;
            continue;
        }

        const uint8_t *soi = cur;
        const uint8_t *q   = soi + 2;
        while (q + 1 < end && !(q[0] == JPEG_MARK && q[1] == JPEG_EOI))
        {
            q++;
        }
        if (q + 1 >= end)
        {
            cur = base;
            continue;
        }

        uint32_t sz = (uint32_t)((q + 2) - soi);
        cur = q + 2;

        if (sz == 0u || sz > MAX_FRAME)
        {
            continue;
        }

        producer_post_frame(prod, soi, sz, true);
        gui_thread_mdelay(prod->interval_ms);
    }
}

/* Locate the MJPEG blob and fill the (fixed) spec + the producer's source. */
static bool source_open(stream_producer_t *prod, stream_spec_t *spec)
{
#ifdef _HONEYGUI_SIMULATOR_
    const uint8_t *blob = (const uint8_t *)_binary_earth_420_410_502_40_lq_mjpg_start;
    /* Convert to integers before subtracting: the linker emits start/end as two
     * distinct symbols, so a direct pointer subtraction trips the static
     * analyzer's comparePointers (UB) check. */
    uint32_t       blen = (uint32_t)((uintptr_t)_binary_earth_420_410_502_40_lq_mjpg_end -
                                     (uintptr_t)_binary_earth_420_410_502_40_lq_mjpg_start);
#else
    const uint8_t *blob = (const uint8_t *)STREAM_DEMO_FLASH_ADDR;
    uint32_t       blen = (uint32_t)STREAM_DEMO_FLASH_SIZE;
#endif

    if (!blob || blen == 0u)
    {
        gui_log("stream demo: empty MJPEG blob\n");
        return false;
    }

    spec->w       = EARTH_W;
    spec->h       = EARTH_H;
    spec->fps_num = EARTH_FPS;
    spec->fps_den = 1u;
    spec->codec   = GUI_STREAM_CODEC_JPEG;

    prod->src     = blob;
    prod->src_len = blen;
    return true;
}

#define PRODUCER_ENTRY  mjpeg_producer_entry
#define DEMO_NAME       "stream_jpeg"
#define DEMO_LABEL      "earth-MJPEG"

#endif /* STREAM_DEMO_SELECTION */

/* Bind the producer to the transport and launch its feeder thread.  prod->src /
 * prod->src_len must already be set by source_open(). */
static void producer_start(stream_producer_t *prod, stp_transport_t *tp,
                           const stream_spec_t *spec, void (*entry)(void *),
                           const char *name)
{
    prod->tp          = tp;
    prod->interval_ms = spec_interval_ms(spec);   /* emit at the source fps   */
    prod->running     = true;

    if (!gui_thread_create(name, entry, prod, 1024 * 8, 5))
    {
        gui_log("stream demo: %s producer thread create failed\n", name);
        prod->running = false;
    }
}

/*============================================================================*
 *                                                                            *
 *                              CONSUMER SIDE                                 *
 *                                                                            *
 *  Creates and configures the gui_stream widget.  The widget pulls frames    *
 *  out of the transport and renders them; it references the transport but    *
 *  does not own it (it must not outlive it).  Depends only on the widget API *
 *  plus the transport handle -- it has no knowledge of the producer.         *
 *                                                                            *
 *============================================================================*/

/* Tap to toggle Play / Pause on the widget. */
static void stream_click_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(e);
    gui_stream_t *st = (gui_stream_t *)obj;

    if (gui_stream_get_state(st) == GUI_VIDEO_STATE_PLAYING)
    {
        gui_stream_set_state(st, GUI_VIDEO_STATE_PAUSE);
    }
    else
    {
        gui_stream_set_state(st, GUI_VIDEO_STATE_PLAYING);
    }
}

/* Pick a drop policy from the codec's decodability, independent of which demo
 * is built: intra-only frames are safe to skip to the newest; inter-frame
 * codecs must keep strict order so prediction is never broken mid-GOP. */
static gui_stream_drop_mode_t consumer_drop_mode_for(gui_stream_codec_t codec)
{
    if (codec == GUI_STREAM_CODEC_JPEG)
    {
        return GUI_STREAM_DROP_UNCONDITIONAL;   /* intra-only: skip to newest */
    }
    return GUI_STREAM_DROP_NONE;                /* inter-frame: keep order    */
}

/*
 * Build the widget that consumes the transport.  This is everything the
 * consumer end needs:
 *
 *     gui_stream_create()             -> attach the widget to the transport
 *     gui_stream_set_update_interval()-> how often to pull + render
 *     gui_stream_set_drop_mode()      -> catch-up policy for the codec
 *     gui_obj_add_event_cb()          -> tap-to-toggle interaction
 */
static gui_stream_t *consumer_create_widget(stp_transport_t *tp,
                                            const stream_spec_t *spec,
                                            const char *name)
{
    int16_t y = (int16_t)((DEMO_SCREEN_H > spec->h)
                          ? (DEMO_SCREEN_H - spec->h) / 2u : 0u);

    gui_stream_t *st = gui_stream_create(gui_obj_get_root(), name, spec->codec, tp,
                                         0, y,
                                         (int16_t)spec->w, (int16_t)spec->h);
    if (!st)
    {
        gui_log("stream demo: %s gui_stream_create failed\n", name);
        return NULL;
    }

    gui_stream_set_update_interval(st, spec_interval_ms(spec));
    gui_stream_set_drop_mode(st, consumer_drop_mode_for(spec->codec));
    gui_obj_add_event_cb(st, stream_click_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    return st;
}

/*============================================================================*
 *                         Transport sizing (app-owned)
 *
 *  stp_instance_create() allocates the frame pool from these size classes
 *  (gui_malloc, sized as sum of buf_size * buf_count + alignment slack).  One
 *  class big enough for the largest encoded frame (MAX_FRAME, the producer's
 *  per-frame cap) at a depth that absorbs producer/consumer jitter covers both
 *  demo sources.
 *============================================================================*/

#define APP_STREAM_BUF_COUNT  16u   /* ring depth: frames allowed in flight */

static const stp_class_cfg_t s_stream_classes[] =
{
    { .buf_size = MAX_FRAME, .buf_count = APP_STREAM_BUF_COUNT },
};

/*============================================================================*
 *                                                                            *
 *                            APPLICATION GLUE                                *
 *                                                                            *
 *  Creates the one transport the app owns and wires the two independent ends *
 *  to it.  The handle is the only thing they share; start order is           *
 *  irrelevant.                                                               *
 *                                                                            *
 *============================================================================*/

static int app_init(void)
{
    gui_log("GUI Stream Widget Example Start (source=%s)\n", DEMO_LABEL);

    /* (1) Open the encoded source first: it fills the read-only stream spec
     *     (the consumer needs it) and stashes the producer-private pointers. */
    stream_spec_t spec;
    if (!source_open(&s_producer, &spec))
    {
        return 0;  /* reason already logged */
    }

    /* (2) Create the one transport this app owns.  The pool is allocated inside
     *     stp_instance_create() from s_stream_classes; the drop policy follows
     *     the codec.  The handle is the only thing the two ends share. */
    stp_config_t cfg;
    stp_config_default(&cfg);
    cfg.align       = 8;
    cfg.classes     = s_stream_classes;
    cfg.class_count = 1;
    cfg.drop_mode   = (spec.codec == GUI_STREAM_CODEC_JPEG)
                      ? STP_DROP_UNCONDITIONAL : STP_DROP_NONE;

    stp_transport_t *tp = stp_instance_create(&cfg);
    if (!tp)
    {
        gui_log("stream demo: stp_instance_create failed\n");
        return 0;
    }

    /* (3) CONSUMER end -- the widget that pulls + renders frames.
     *     Needs only tp + spec; knows nothing about the producer. */
    consumer_create_widget(tp, &spec, DEMO_NAME);

    /* (4) PRODUCER end -- feed encoded frames into the same transport.
     *     Needs only tp + source; knows nothing about the widget. */
    producer_start(&s_producer, tp, &spec, PRODUCER_ENTRY, DEMO_NAME);

    gui_log("stream demo: %s %ux%u @ %u/%u fps, codec %d\n",
            DEMO_NAME, (unsigned)spec.w, (unsigned)spec.h,
            (unsigned)spec.fps_num, (unsigned)spec.fps_den, (int)spec.codec);
    return 0;
}
GUI_INIT_APP_EXPORT(app_init);
