/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <string.h>
#include "acc_sw_jpeg.h"
#include "draw_img.h"
#include "gui_api.h"

#define GUI_STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_NO_PNG
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STBI_NO_GIF
#define STBI_NO_PIC
#define STBI_NO_THREAD_LOCALS

#include "gui_api.h"
#define STBI_MALLOC(sz)           gui_malloc(sz)
#define STBI_REALLOC(p,newsz)     gui_realloc(p, newsz)
#define STBI_FREE(p)              gui_free(p)
#include "gui_stb_image.h"

/*============================================================================*
 *                        Software decode backends
 *
 *  JPEG_SW_BACKEND selects who decodes when no hardware jpeg ops are
 *  registered:
 *
 *      0   stb_image -- float IDCT
 *      1   tjpgd     -- integer IDCT, ~4.7 KB of code and 3.5 KB of workspace;
 *                       decodes MCU row by MCU row, so no full-size component
 *                       planes are ever materialised (default)
 *
 *  tjpgd only understands baseline JPEG. Anything it refuses (progressive,
 *  arithmetic coding, a broken stream) falls back to stb in the caller, so no
 *  image that used to draw can disappear because of this switch.
 *============================================================================*/
#define JPEG_SW_BACKEND   1

#if JPEG_SW_BACKEND == 1
#include "tjpgd.h"
#endif

/*============================================================================*
 *                    Output pixel format of the software path
 *
 *  JD_FORMAT in tjpgdcnf.h is the single switch, and it is a build-time one:
 *
 *    JD_FORMAT 1  -> RGB565, 2 bytes/px (default). HoneyGUI's color_rgb565_t
 *                    keeps b in the low bits, tjpgd packs R5G6B5, they agree.
 *    JD_FORMAT 0  -> RGB888, 3 bytes/px. HoneyGUI's RGB888 is BGR in memory
 *                    (core/def_color.h: { b, g, r }) and tjpgd stores B,G,R.
 *
 *  Override with -DJD_FORMAT=0 instead of editing the header. Either way the
 *  packing happens inside the decoder -- tjpgd folds RGB565 in mcu_output()'s
 *  scratch buffer -- so nothing re-walks the finished frame. The stb fallback
 *  converts only when tjpgd refused the stream.
 *============================================================================*/
#if (JPEG_SW_BACKEND == 1) && defined(JD_FORMAT) && (JD_FORMAT == 1)
#define JPEG_SW_DEFAULT_565  1
#else
#define JPEG_SW_DEFAULT_565  0
#endif

/* Resolved once, so both backends and the image header always agree. */
#define JPEG_SW_FORMAT   ((JPEG_SW_DEFAULT_565 != 0) ? RGB565 : RGB888)
#define JPEG_SW_BPP      ((JPEG_SW_DEFAULT_565 != 0) ? 2u : 3u)

/* Format of every buffer this file returns, in the same vocabulary as the
 * image header. Callers ask instead of inferring it from "is a hardware jpeg op
 * registered", which stops holding once the backend can be either format. */
char gui_acc_jpeg_get_output(void)
{
    return (JPEG_SW_DEFAULT_565 != 0) ? RGB565 : RGB888;
}

#if JPEG_SW_BACKEND == 1

typedef struct
{
    const uint8_t *src;      /* encoded stream passed to gui_acc_jpeg_load() */
    size_t         len;
    size_t         pos;      /* read cursor inside src                        */
    uint8_t       *dst;      /* first output pixel, i.e. just past the header  */
    uint32_t       stride;   /* bytes per output row                           */
    uint8_t        bpp;      /* 2 for RGB565, 3 for RGB888, per frame         */
} jpeg_tjpgd_io_t;

/* tjpgd pulls the bitstream through this in 512-byte gulps (JD_SZBUF). A short
 * read is how it learns the stream ended. */
static size_t jpeg_tjpgd_in(JDEC *jd, uint8_t *buff, size_t nbyte)
{
    jpeg_tjpgd_io_t *io = (jpeg_tjpgd_io_t *)jd->device;
    size_t left = io->len - io->pos;

    if (nbyte > left)
    {
        nbyte = left;
    }
    if (buff != NULL && nbyte > 0u)
    {
        memcpy(buff, io->src + io->pos, nbyte);
    }
    io->pos += nbyte;
    return nbyte;
}

/* One MCU rectangle per call. Rows go to the final stride, not the MCU width,
 * so a partially covered rect at the right edge still lands correctly. */
static int jpeg_tjpgd_out(JDEC *jd, void *bitmap, JRECT *rect)
{
    jpeg_tjpgd_io_t *io = (jpeg_tjpgd_io_t *)jd->device;
    const uint8_t *src = (const uint8_t *)bitmap;
    uint32_t row_bytes = (uint32_t)(rect->right - rect->left + 1u) * io->bpp;
    uint8_t *dst = io->dst + (uint32_t)rect->top * io->stride +
                   (uint32_t)rect->left * io->bpp;
    uint32_t y;

    for (y = rect->top; y <= rect->bottom; y++)
    {
        /* The decoder already emits the layout the engine expects (see the
         * format notes above), so rows are plain copies. */
        memcpy(dst, src, row_bytes);
        dst += io->stride;
        src += row_bytes;
    }
    return 1;   /* non-zero: keep decoding */
}

static void *jpeg_load_tjpgd(void *input, int len, int *w, int *h, int *channel)
{
    /* tjpgd only needs the pool between jd_prepare and jd_decomp. Keeping it
     * static puts the 3.5 KB of tables in DTCM instead of the heap, and avoids
     * a 3.5 KB stack frame in the GUI task. One decoder runs at a time. */
    static uint32_t pool[(TJPGD_WORKSPACE_SIZE + 3) / 4];
    JDEC     jd;
    jpeg_tjpgd_io_t io;
    JRESULT  rc;
    uint8_t *img;
    gui_rgb_data_head_t *head;
    size_t   px_bytes;
    char     format;

    if (input == NULL || len <= 0)
    {
        return NULL;
    }

    memset(&jd, 0, sizeof(jd));
    memset(&io, 0, sizeof(io));
    io.src = (const uint8_t *)input;
    io.len = (size_t)len;
    format = JPEG_SW_FORMAT;
    io.bpp = JPEG_SW_BPP;

    rc = jd_prepare(&jd, jpeg_tjpgd_in, pool, sizeof(pool), &io);
    if (rc != JDR_OK || jd.width == 0u || jd.height == 0u)
    {
        /* One-shot: if the stream is a sampling layout tjpgd cannot handle
         * (4:4:0, progressive, ...) every frame lands here, and the caller
         * falls back to stb. Printing once avoids drowning the log at 25 fps. */
        static bool warned = false;
        if (!warned)
        {
            warned = true;
            gui_log("tjpgd: prepare failed rc=%d -- falling back to stb\n", (int)rc);
        }
        return NULL;
    }

    px_bytes = (size_t)jd.width * (size_t)jd.height * io.bpp;
    img = (uint8_t *)gui_malloc(sizeof(gui_rgb_data_head_t) + px_bytes);
    if (img == NULL)
    {
        gui_log("tjpgd: out of memory (%u bytes)\n",
                (unsigned)(sizeof(gui_rgb_data_head_t) + px_bytes));
        return NULL;
    }

    io.dst    = img + sizeof(gui_rgb_data_head_t);
    io.stride = (uint32_t)jd.width * io.bpp;

    rc = jd_decomp(&jd, jpeg_tjpgd_out, 0);
    if (rc != JDR_OK)
    {
        static bool warned = false;
        if (!warned)
        {
            warned = true;
            gui_log("tjpgd: decomp failed rc=%d (%ux%u)\n", (int)rc, jd.width, jd.height);
        }
        gui_free(img);
        return NULL;
    }

    head = (gui_rgb_data_head_t *)img;
    memset(head, 0x00, sizeof(*head));
    head->type = format;
    head->w    = (short)jd.width;
    head->h    = (short)jd.height;
    head->jpeg = true;

    if (w)       { *w = (int)jd.width; }
    if (h)       { *h = (int)jd.height; }
    if (channel) { *channel = (int)io.bpp; }

    return img;
}
#endif /* JPEG_SW_BACKEND */

/* stb_image keeps its own allocation discipline: it reserves the header bytes
 * for us (STB_GUI_IMG_HEADER), so the buffer can be handed out as-is. */
static void *jpeg_load_stb(void *input, int len, int *w, int *h, int *channel)
{
    unsigned char *img_data;
    gui_rgb_data_head_t *head;

    img_data = gui_stbi_load_from_memory(input, len, w, h, channel, 0);
    if (img_data == NULL)
    {
        gui_log("Failed to load JPEG image from memory!\n");
        return NULL;
    }

    /* When RGB565 is requested but stb only produces RGB888 (B,G,R on this
     * fork), pack downwards in place, last pixel first: the destination index
     * (2i) always trails the source index (3i), so nothing is overwritten
     * before it is read. This runs only when tjpgd rejected the stream, so the
     * hot path never pays for it. */
    if ((JPEG_SW_FORMAT == RGB565) && *w > 0 && *h > 0)
    {
        uint8_t *base = img_data + sizeof(gui_rgb_data_head_t);
        size_t   n = (size_t)(*w) * (size_t)(*h);
        uint8_t       *s = base + n * 3u;
        uint8_t       *d = base + n * 2u;

        while (n--)
        {
            uint8_t b, g, r;

            s -= 3;
            b = s[0];
            g = s[1];
            r = s[2];
            d -= 2;
            d[0] = (uint8_t)((b >> 3) | ((g & 0x1Cu) << 3));          /* GGGBBBBB */
            d[1] = (uint8_t)(((g & 0xE0u) >> 5) | ((r & 0xF8u) >> 0)); /* RRRRRGGG */
        }
        if (channel)
        {
            *channel = 2;
        }
    }

    head = (gui_rgb_data_head_t *)img_data;
    memset(head, 0x00, sizeof(gui_rgb_data_head_t));
    head->type = JPEG_SW_FORMAT;
    head->w = (short) * w;
    head->h = (short) * h;
    head->compress = false; // Not compressed
    head->jpeg = true;

    return img_data;
}

void *gui_acc_jpeg_load(void *input, int len, int *w, int *h, int *channel)
{
    struct acc_engine *acc = gui_get_acc();

    if (acc != NULL && acc->jpeg != NULL && acc->jpeg->load != NULL)
    {
        return acc->jpeg->load(input, len, w, h, channel);
    }

#if JPEG_SW_BACKEND == 1
    {
        void *img = jpeg_load_tjpgd(input, len, w, h, channel);
        if (img != NULL)
        {
            return img;
        }
    }
#endif

    /* stb_image: the historical path, and the safety net for streams tjpgd
     * refuses (it is baseline-only). */
    return jpeg_load_stb(input, len, w, h, channel);
}

void gui_acc_jpeg_release(void *decoded)
{
    struct acc_engine *acc = gui_get_acc();
    if (acc != NULL && acc->jpeg != NULL && acc->jpeg->release != NULL)
    {
        acc->jpeg->release(decoded);
    }
    else
    {
        // Fallback to software memory deallocation
        if (decoded != NULL)
        {
            gui_stbi_image_free(decoded);
        }
    }
}

#define GUI_STB_IMAGE_WRITE_IMPLEMENTATION

#define GUI_STBI_WRITE_NO_STDIO

#define GUI_STBIW_MALLOC(sz)            gui_malloc(sz)
#define GUI_STBIW_REALLOC(p, newsz)     gui_realloc(p, newsz)
#define GUI_STBIW_FREE(p)               gui_free(p)
#define GUI_STBIW_REALLOC_SIZED(p, oldsz, newsz)  gui_realloc(p, newsz)

#define GUI_STBIW_ASSERT(x)             GUI_ASSERT(x)

#define GUI_STBIW_JPEG_HEADER           0

#include "gui_stb_image_write.h"

typedef struct
{
    unsigned char *buf;  /**< Buffer being filled. On entry this is the block
                          *   gui_acc_jpeg_compress() allocated; it is swapped for
                          *   the realloc() result when the encode outgrows it.
                          *   Never NULL once the caller has passed its checks. */
    int len;             /**< Bytes written so far, i.e. the write cursor. Starts
                          *   at 0 even when the encoder begins by emitting the
                          *   reserved resource head -- that head is a normal part
                          *   of the stream, so the caller must NOT pre-advance
                          *   this to GUI_STBIW_JPEG_HEADER. On return it is the
                          *   total stream length, head included. */
    int cap;             /**< Current capacity of buf, in bytes, including any
                          *   growth that has happened. */
    int oom;             /**< Set when gui_realloc() returned NULL. */
} jpeg_write_sink_t;

static void jpeg_write_sink_cb(void *context, void *data, int size)
{
    jpeg_write_sink_t *m = (jpeg_write_sink_t *)context;

    if (m == NULL || m->oom || data == NULL || size <= 0)
    {
        return;
    }

    if (m->len + size > m->cap)
    {
        int cap = m->cap > 0 ? m->cap : 4096;
        unsigned char *grown = NULL;

        while (cap < m->len + size)
        {
            cap *= 2;
        }
        grown = gui_realloc(m->buf, (size_t)cap);
        if (grown == NULL)
        {
            m->oom = 1;
            return;
        }
        m->buf = grown;
        m->cap = cap;
    }
    memcpy(m->buf + m->len, data, (size_t)size);
    m->len += size;
}

void *gui_acc_jpeg_compress(void *input, int w, int h, int bpp,
                            int quality, int buf_len, int *out_len)
{
    struct acc_engine *acc = gui_get_acc();
    if (acc != NULL && acc->jpeg != NULL && acc->jpeg->compress != NULL)
    {
        return acc->jpeg->compress(input, w, h, bpp, quality, buf_len, out_len);
    }

    int channel = 0;
    int src565 = 0;
    size_t cap;
    size_t limit;
    unsigned char *buf;
    jpeg_write_sink_t sink;
    const int JPEG_HEAD_SIZE = (int)sizeof(gui_jpeg_file_head_t) -
                               (int)sizeof(((gui_jpeg_file_head_t *)0)->jpeg);

    if (bpp == 16)
    {
        /* 2 bytes per pixel; channel only drives the size estimate below. */
        src565 = 1;
        channel = 2;
    }
    else
    {
        channel = (bpp == 24) ? 3 : ((bpp == 32) ? 4 : 0);
    }

    if (input == NULL || w <= 0 || h <= 0 || channel == 0 || out_len == NULL)
    {
        gui_log("gui_acc_jpeg_write: bad arguments (color depth=%d w=%d h=%d out_len=%d)\n",
                (int)bpp, w, h, out_len);
        return NULL;
    }

    if (buf_len < 0)
    {
        gui_log("gui_acc_jpeg_compress: buf_len %d is negative\n", buf_len);
        return NULL;
    }
    if (GUI_STBIW_JPEG_HEADER > 0 && buf_len > 0 && buf_len <= JPEG_HEAD_SIZE)
    {
        gui_log("gui_acc_jpeg_compress: buf_len %d cannot hold the %d-byte resource head\n",
                buf_len, JPEG_HEAD_SIZE);
        return NULL;
    }

    quality = (quality < 1) ? 1 : ((quality > 100) ? 100 : quality);
    *out_len = 0;
    limit = (size_t)buf_len;

    cap = (size_t)w * (size_t)h * (size_t)channel + 4096u;

    buf = gui_malloc(cap);
    if (buf == NULL)
    {
        gui_log("gui_acc_jpeg_compress: out of memory (%u bytes)\n", (unsigned)cap);
        return NULL;
    }

    memset(&sink, 0, sizeof(sink));
    sink.buf = buf;
    sink.cap = (int)cap;

    const int min_len = (GUI_STBIW_JPEG_HEADER > 0) ? JPEG_HEAD_SIZE : 2;

    if (!gui_stbi_write_jpg_to_func(jpeg_write_sink_cb, &sink,
                                    w, h, src565 ? GUI_STBIW_JPG_SRC_RGB565 : channel,
                                    input, quality)
        || sink.oom || sink.len < min_len)
    {
        gui_log("gui_acc_jpeg_compress: encode failed\n");
        gui_free(sink.buf);
        return NULL;
    }

    if (limit > 0 && (size_t)sink.len > limit)
    {
        gui_log("gui_acc_jpeg_compress: result %d bytes exceeds buf_len %d\n",
                sink.len, buf_len);
        gui_free(sink.buf);
        return NULL;
    }

    if (GUI_STBIW_JPEG_HEADER == 0)
    {
        /* Bare stream: it has to open with SOI, or it is not a JPEG at all. */
        if (sink.len < 2 || sink.buf[0] != 0xFF || sink.buf[1] != 0xD8)
        {
            gui_free(sink.buf);
            return NULL;
        }
        if (out_len != NULL)
        {
            *out_len = sink.len;
        }
        return sink.buf;
    }
    else if (GUI_STBIW_JPEG_HEADER == JPEG_HEAD_SIZE)
    {
        gui_jpeg_file_head_t *res = (gui_jpeg_file_head_t *)sink.buf;

        memset(res, 0x00, (size_t)JPEG_HEAD_SIZE);
        res->img_header.type = JPEG;
        res->img_header.jpeg = 0;
        res->img_header.w = (short)w;
        res->img_header.h = (short)h;
        res->size = (uint32_t)(sink.len - JPEG_HEAD_SIZE);
        res->dummy = 0;

        if (out_len != NULL)
        {
            *out_len = sink.len;
        }
        return res;
    }

    gui_free(sink.buf);
    return NULL;
}

void gui_acc_jpeg_free(void *encoded)
{
    struct acc_engine *acc = gui_get_acc();
    if (acc != NULL && acc->jpeg != NULL && acc->jpeg->free != NULL)
    {
        acc->jpeg->free(encoded);
    }
    else
    {
        // Fallback to software memory deallocation
        if (encoded != NULL)
        {
            gui_free(encoded);
        }
    }
}