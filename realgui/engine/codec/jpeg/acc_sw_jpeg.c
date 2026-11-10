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

void *gui_acc_jpeg_load(void *input, int len, int *w, int *h, int *channel)
{
    struct acc_engine *acc = gui_get_acc();
    if (acc != NULL && acc->jpeg != NULL && acc->jpeg->load != NULL)
    {
        return acc->jpeg->load(input, len, w, h, channel);
    }
    else
    {
        // Fallback to software decoding if no hardware acceleration is available
        unsigned char *img_data = gui_stbi_load_from_memory(input, len, w, h, channel, 0);
        if (img_data == NULL)
        {
            gui_log("Failed to load JPEG image from memory!\n");
            return NULL;
        }

        gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)img_data;
        memset(head, 0x00, sizeof(gui_rgb_data_head_t));
        head->type = RGB888; // Assuming RGB888 format for JPEG
        head->w = *w;
        head->h = *h;
        head->compress = false; // Not compressed
        head->jpeg = true;

        return img_data;
    }
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