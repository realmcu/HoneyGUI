/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *  Cinepak video decoder - embedded C implementation
 *
 *  Algorithm reference:
 *    Published Cinepak bitstream specification.
 *    Behavioural details (strip rect override, delta codebook inheritance,
 *    flag-bit ordering, Sega CD heuristic) verified against the ref
 *    source in realgui/widget/gui_cinepak/refer/cinepak.cpp, which is
 *    licensed GPL-3.0 and used here ONLY as a specification reference.
 *    No ref source text has been copied into this file.
 *
 *  Output: RGB565, top-down row order, width * height * 2 bytes.
 *============================================================================*/

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "cinepak_decoder.h"
#include <stdlib.h>
#include <string.h>

/*============================================================================*
 *                        Memory Allocation Hooks
 *
 *  Override before compiling to redirect to gui_malloc / gui_free or any
 *  RTOS heap.
 *============================================================================*/
#ifndef CINEPAK_MALLOC
#  define CINEPAK_MALLOC(sz)     malloc(sz)
#endif
#ifndef CINEPAK_CALLOC
#  define CINEPAK_CALLOC(n, sz)  calloc((n), (sz))
#endif
#ifndef CINEPAK_FREE
#  define CINEPAK_FREE(p)        free(p)
#endif

/*============================================================================*
 *                        Private Types
 *============================================================================*/

/* Lightweight read-only byte stream (big-endian). */
typedef struct
{
    const uint8_t *data;
    size_t         size;
    size_t         pos;
} stream_t;

/*
 * One codebook entry.
 *
 * Without CINEPAK_PRECOMPUTE_COLORS (default):
 *   Stores raw YUV data - four luma samples (y[0..3]) arranged as a 2*2
 *   sub-block (y[0]=top-left, y[1]=top-right, y[2]=bottom-left,
 *   y[3]=bottom-right) plus shared chrominance offsets u (Cb) and v (Cr).
 *   YUV -> RGB565 conversion runs on every pixel lookup.
 *
 * With CINEPAK_PRECOMPUTE_COLORS:
 *   YUV -> RGB565 is computed once when the entry is loaded in load_codebook().
 *   px[0..3] hold the four pre-converted RGB565 values for the same sub-block
 *   positions.  Block decoding is then a direct copy with no arithmetic.
 */
#if defined(CINEPAK_PRECOMPUTE_COLORS)
typedef struct
{
    uint16_t px[4];  /* RGB565: px[0]=TL, px[1]=TR, px[2]=BL, px[3]=BR */
} cinepak_codebook_t;
#else
typedef struct
{
    uint8_t y[4];    /* luma: y[0]=TL, y[1]=TR, y[2]=BL, y[3]=BR */
    int8_t  u;       /* Cb chrominance offset */
    int8_t  v;       /* Cr chrominance offset */
} cinepak_codebook_t;
#endif

/* State for one horizontal strip of the frame. */
typedef struct
{
    uint16_t id;                         /* 0x1000=keyframe strip, 0x1100=delta */
    uint16_t top, left, bottom, right;   /* pixel coordinates */
    cinepak_codebook_t v1_codebook[256]; /* low-detail (V1) codebook */
    cinepak_codebook_t v4_codebook[256]; /* high-detail (V4) codebook */
} cinepak_strip_t;

/*
 * Maximum number of strips that can appear in a single frame.
 * Standard Cinepak rarely uses more than 3; pre-allocating 8 avoids
 * realloc when strip count grows mid-stream (e.g. cat_00.avi 1->2->3).
 */
#define CINEPAK_MAX_STRIPS 8u

/* Decoder context. */
struct cinepak_decoder
{
    uint16_t          width;
    uint16_t          height;
    uint16_t         *pixels;        /* RGB565 output buffer, caller-owned */
    cinepak_strip_t  *strips;        /* lazily allocated on first decode_frame */
    uint16_t          strips_alloc;  /* number of entries allocated in strips[] */
    uint16_t          prev_strip_count; /* strip count from the previous decoded frame */
};

/*============================================================================*
 *                        Private: Stream Helpers (big-endian)
 *============================================================================*/

static inline int stream_avail(const stream_t *s, size_t n)
{
    return (s->pos + n) <= s->size;
}

static inline uint8_t stream_read_u8(stream_t *s)
{
    return s->data[s->pos++];
}

static inline uint16_t stream_read_be16(stream_t *s)
{
    uint16_t v = ((uint16_t)s->data[s->pos] << 8) |
                 (uint16_t)s->data[s->pos + 1];
    s->pos += 2;
    return v;
}

static inline uint32_t stream_read_be32(stream_t *s)
{
    uint32_t v = ((uint32_t)s->data[s->pos]     << 24) |
                 ((uint32_t)s->data[s->pos + 1] << 16) |
                 ((uint32_t)s->data[s->pos + 2] <<  8) |
                 (uint32_t)s->data[s->pos + 3];
    s->pos += 4;
    return v;
}

/*============================================================================*
 *                        Private: YUV -> RGB565
 *
 *  Cinepak-specific approximation:
 *    R = clamp(Y + 2 dot V,           0, 255)
 *    G = clamp(Y - (U >> 1) - V,      0, 255)   (arithmetic right shift)
 *    B = clamp(Y + 2 dot U,           0, 255)
 *
 *  Y in [0, 255],  U, V in [-128, 127].
 *============================================================================*/

static inline uint8_t clip_u8(int32_t x)
{
    return (unsigned)x <= 255u ? (uint8_t)x : (x < 0 ? 0u : 255u);
}

static inline uint16_t yuv_to_rgb565(uint8_t y, int8_t u, int8_t v)
{
    uint8_t r = clip_u8((int32_t)y + 2  * (int32_t)v);
    uint8_t g = clip_u8((int32_t)y - ((int32_t)u >> 1) - (int32_t)v);
    uint8_t b = clip_u8((int32_t)y + 2  * (int32_t)u);
    return (uint16_t)(((uint16_t)(r & 0xF8u) << 8) |
                      ((uint16_t)(g & 0xFCu) << 3) |
                      (uint16_t)(b           >> 3));
}

/*============================================================================*
 *                        Private: 4*4 Block Decoders
 *
 *  @p dst    points to the top-left pixel of the 4*4 block (top-down layout).
 *  @p pitch  is the frame width in pixels (row stride).
 *============================================================================*/

/*
 * V1 block - one codebook entry covers the whole 4*4 block as four 2*2 cells:
 *
 *   y[0] y[0] | y[1] y[1]     (rows 0-1)
 *   ----------|----------
 *   y[2] y[2] | y[3] y[3]     (rows 2-3)
 *
 * All 16 pixels share the same u and v.
 */
static void decode_v1_block(const cinepak_codebook_t *cb,
                            uint16_t *dst, uint16_t pitch)
{
#if defined(CINEPAK_PRECOMPUTE_COLORS)
    uint16_t p0 = cb->px[0], p1 = cb->px[1];
    uint16_t p2 = cb->px[2], p3 = cb->px[3];
#else
    int8_t   u  = cb->u, v = cb->v;
    uint16_t p0 = yuv_to_rgb565(cb->y[0], u, v);
    uint16_t p1 = yuv_to_rgb565(cb->y[1], u, v);
    uint16_t p2 = yuv_to_rgb565(cb->y[2], u, v);
    uint16_t p3 = yuv_to_rgb565(cb->y[3], u, v);
#endif
    dst[0] = dst[1] = p0;  dst[2] = dst[3] = p1;  dst += pitch;
    dst[0] = dst[1] = p0;  dst[2] = dst[3] = p1;  dst += pitch;
    dst[0] = dst[1] = p2;  dst[2] = dst[3] = p3;  dst += pitch;
    dst[0] = dst[1] = p2;  dst[2] = dst[3] = p3;
}

/*
 * V4 block - four codebook entries, one per 2*2 quadrant:
 *   cb0=top-left, cb1=top-right, cb2=bottom-left, cb3=bottom-right.
 *
 * Within each 2*2 entry:
 *   y[0]=top-left, y[1]=top-right, y[2]=bottom-left, y[3]=bottom-right.
 *
 * Each entry has its own u and v.
 */
static void decode_v4_block(const cinepak_codebook_t *cb0,
                            const cinepak_codebook_t *cb1,
                            const cinepak_codebook_t *cb2,
                            const cinepak_codebook_t *cb3,
                            uint16_t *dst, uint16_t pitch)
{
#if defined(CINEPAK_PRECOMPUTE_COLORS)
    /* Row 0 */
    dst[0] = cb0->px[0];  dst[1] = cb0->px[1];
    dst[2] = cb1->px[0];  dst[3] = cb1->px[1];  dst += pitch;
    /* Row 1 */
    dst[0] = cb0->px[2];  dst[1] = cb0->px[3];
    dst[2] = cb1->px[2];  dst[3] = cb1->px[3];  dst += pitch;
    /* Row 2 */
    dst[0] = cb2->px[0];  dst[1] = cb2->px[1];
    dst[2] = cb3->px[0];  dst[3] = cb3->px[1];  dst += pitch;
    /* Row 3 */
    dst[0] = cb2->px[2];  dst[1] = cb2->px[3];
    dst[2] = cb3->px[2];  dst[3] = cb3->px[3];
#else
    /* Row 0 */
    dst[0] = yuv_to_rgb565(cb0->y[0], cb0->u, cb0->v);
    dst[1] = yuv_to_rgb565(cb0->y[1], cb0->u, cb0->v);
    dst[2] = yuv_to_rgb565(cb1->y[0], cb1->u, cb1->v);
    dst[3] = yuv_to_rgb565(cb1->y[1], cb1->u, cb1->v);
    dst += pitch;
    /* Row 1 */
    dst[0] = yuv_to_rgb565(cb0->y[2], cb0->u, cb0->v);
    dst[1] = yuv_to_rgb565(cb0->y[3], cb0->u, cb0->v);
    dst[2] = yuv_to_rgb565(cb1->y[2], cb1->u, cb1->v);
    dst[3] = yuv_to_rgb565(cb1->y[3], cb1->u, cb1->v);
    dst += pitch;
    /* Row 2 */
    dst[0] = yuv_to_rgb565(cb2->y[0], cb2->u, cb2->v);
    dst[1] = yuv_to_rgb565(cb2->y[1], cb2->u, cb2->v);
    dst[2] = yuv_to_rgb565(cb3->y[0], cb3->u, cb3->v);
    dst[3] = yuv_to_rgb565(cb3->y[1], cb3->u, cb3->v);
    dst += pitch;
    /* Row 3 */
    dst[0] = yuv_to_rgb565(cb2->y[2], cb2->u, cb2->v);
    dst[1] = yuv_to_rgb565(cb2->y[3], cb2->u, cb2->v);
    dst[2] = yuv_to_rgb565(cb3->y[2], cb3->u, cb3->v);
    dst[3] = yuv_to_rgb565(cb3->y[3], cb3->u, cb3->v);
#endif
}

/*============================================================================*
 *                        Private: Codebook Loader
 *
 *  Processes chunk IDs 0x20-0x27: updates V1 or V4 codebook entries.
 *
 *  chunk_id bit 0 - selective update: a 32-bit big-endian flag word precedes
 *    each group of 32 entries; a set bit means "update this entry".
 *
 *  chunk_id bit 2 - entry width: 0 = 6-byte (y[4] + int8 u + int8 v);
 *    1 = 4-byte (y[4] only, u = v = 0, greyscale/palettised mode).
 *
 *  @param chunk_start  Stream offset immediately after the 4-byte chunk header.
 *  @param chunk_size   Data bytes remaining in chunk (raw length - 4).
 *============================================================================*/
static void load_codebook(cinepak_codebook_t *codebook,
                          stream_t           *s,
                          size_t              chunk_start,
                          uint32_t            chunk_size,
                          uint8_t             chunk_id)
{
    uint32_t flag = 0, mask = 0;

    for (int i = 0; i < 256; i++)
    {
        /* Selective update: shift flag word, reading a new one when exhausted. */
        if ((chunk_id & 0x01u) && !(mask >>= 1))
        {
            if ((s->pos - chunk_start + 4u) > (size_t)chunk_size)
            {
                return;
            }
            flag = stream_read_be32(s);
            mask = 0x80000000u;
        }

        /* Update this entry only if: non-selective, or selective and bit is set. */
        if (!(chunk_id & 0x01u) || (flag & mask))
        {
            uint8_t n = (chunk_id & 0x04u) ? 4u : 6u;
            if ((s->pos - chunk_start + n) > (size_t)chunk_size)
            {
                return;
            }

            uint8_t y0 = stream_read_u8(s);
            uint8_t y1 = stream_read_u8(s);
            uint8_t y2 = stream_read_u8(s);
            uint8_t y3 = stream_read_u8(s);
            int8_t  cu = 0, cv = 0;
            if (n == 6u)
            {
                cu = (int8_t)stream_read_u8(s);
                cv = (int8_t)stream_read_u8(s);
            }

#if defined(CINEPAK_PRECOMPUTE_COLORS)
            /* Convert once at load time; block decode becomes a direct copy. */
            codebook[i].px[0] = yuv_to_rgb565(y0, cu, cv);
            codebook[i].px[1] = yuv_to_rgb565(y1, cu, cv);
            codebook[i].px[2] = yuv_to_rgb565(y2, cu, cv);
            codebook[i].px[3] = yuv_to_rgb565(y3, cu, cv);
#else
            codebook[i].y[0] = y0;
            codebook[i].y[1] = y1;
            codebook[i].y[2] = y2;
            codebook[i].y[3] = y3;
            codebook[i].u    = cu;
            codebook[i].v    = cv;
#endif
        }
    }
}

/*============================================================================*
 *                        Private: Vector (Image) Decoder
 *
 *  Processes chunk IDs 0x30-0x32: fills the frame with decoded pixels.
 *
 *  Outer flag  (chunk_id bit 0):  0 = all blocks updated; 1 = skip-flag per block.
 *  Inner flag  (chunk_id bit 1):  0 = V1/V4 flag per block; 1 = always V1.
 *
 *  Both flag types share a single 32-bit big-endian flag word, consumed MSB-
 *  first.  This is a direct C translation of ref's decodeVectorsTmpl().
 *============================================================================*/
static void decode_vectors(cinepak_decoder_t *ctx,
                           cinepak_strip_t   *strip,
                           stream_t          *s,
                           size_t             chunk_start,
                           uint32_t           chunk_size,
                           uint8_t            chunk_id)
{
    uint32_t flag = 0, mask = 0;
    uint16_t pitch = ctx->width;

    for (uint16_t y = strip->top; y < strip->bottom; y += 4u)
    {
        uint16_t *row_dst = ctx->pixels + (uint32_t)y * pitch + strip->left;

        for (uint16_t x = strip->left; x < strip->right; x += 4u, row_dst += 4u)
        {
            /* ---- Outer skip flag ---------------------------------------- */
            if ((chunk_id & 0x01u) && !(mask >>= 1))
            {
                if ((s->pos - chunk_start + 4u) > (size_t)chunk_size)
                {
                    return;
                }
                flag = stream_read_be32(s);
                mask = 0x80000000u;
            }

            if (!(chunk_id & 0x01u) || (flag & mask))
            {
                /* ---- Inner V1/V4 type flag ------------------------------ */
                if (!(chunk_id & 0x02u) && !(mask >>= 1))
                {
                    if ((s->pos - chunk_start + 4u) > (size_t)chunk_size)
                    {
                        return;
                    }
                    flag = stream_read_be32(s);
                    mask = 0x80000000u;
                }

                if ((chunk_id & 0x02u) || !(flag & mask))
                {
                    /* V1: one index selects a codebook entry for the 4*4 block */
                    if ((s->pos - chunk_start + 1u) > (size_t)chunk_size)
                    {
                        return;
                    }
                    uint8_t idx = stream_read_u8(s);
                    decode_v1_block(&strip->v1_codebook[idx], row_dst, pitch);
                }
                else if (flag & mask)
                {
                    /* V4: four indices, one per 2*2 quadrant */
                    if ((s->pos - chunk_start + 4u) > (size_t)chunk_size)
                    {
                        return;
                    }
                    uint8_t i0 = stream_read_u8(s);
                    uint8_t i1 = stream_read_u8(s);
                    uint8_t i2 = stream_read_u8(s);
                    uint8_t i3 = stream_read_u8(s);
                    decode_v4_block(&strip->v4_codebook[i0],
                                    &strip->v4_codebook[i1],
                                    &strip->v4_codebook[i2],
                                    &strip->v4_codebook[i3],
                                    row_dst, pitch);
                }
            }
        }
    }
}

/*============================================================================*
 *                        Public API
 *============================================================================*/

cinepak_decoder_t *cinepak_decoder_create(uint16_t width, uint16_t height,
                                          uint8_t *pixel_buf)
{
    if (!width || !height || (width % 4u) || (height % 4u))
    {
        return NULL;
    }
    if (!pixel_buf)
    {
        return NULL;
    }

    cinepak_decoder_t *ctx =
        (cinepak_decoder_t *)CINEPAK_MALLOC(sizeof(cinepak_decoder_t));
    if (!ctx)
    {
        return NULL;
    }

    ctx->width        = width;
    ctx->height       = height;
    ctx->pixels       = (uint16_t *)pixel_buf;
    ctx->strips       = NULL;
    ctx->strips_alloc = 0;
    return ctx;
}

const void *cinepak_decode_frame(cinepak_decoder_t *ctx,
                                 const uint8_t *src, size_t src_size)
{
    if (!ctx || !src || src_size < 10u)
    {
        return NULL;
    }

    stream_t s;
    s.data = src;
    s.size = src_size;
    s.pos  = 0;

    /* ---- 10-byte frame header (all fields big-endian) ------------------- */
    uint8_t  frame_flags  = stream_read_u8(&s);
    /* bit0 = 0 -> intra frame; bit0 = 1 -> inter frame */
    uint32_t frame_len    = (uint32_t)stream_read_u8(&s) << 16;
    frame_len            |= (uint32_t)stream_read_be16(&s);
    uint16_t frame_width  = stream_read_be16(&s);
    uint16_t frame_height = stream_read_be16(&s);
    uint16_t strip_count  = stream_read_be16(&s);

    /* Frame dimensions must match the context. */
    if (frame_width != ctx->width || frame_height != ctx->height)
    {
        return NULL;
    }
    if (strip_count == 0)
    {
        return NULL;
    }

    /* ---- Sega CD heuristic: skip extra preamble bytes ------------------- */
    if (frame_len != (uint32_t)src_size && s.pos + 2u <= s.size)
    {
        uint16_t peek = ((uint16_t)s.data[s.pos] << 8) | s.data[s.pos + 1];
        if (peek == 0xFE00u && s.pos + 6u <= s.size)
        {
            s.pos += 6u;    /* skip 2-byte marker + 4 extra bytes */
        }
    }

    /* ---- Allocate strip state on the first frame ------------------------ */
    if (!ctx->strips)
    {
        /* Always allocate CINEPAK_MAX_STRIPS so that frames whose strip count
         * grows later (e.g. 1->2->3 mid-stream) never hit the cap. */
        ctx->strips = (cinepak_strip_t *)CINEPAK_CALLOC(CINEPAK_MAX_STRIPS,
                                                        sizeof(cinepak_strip_t));
        if (!ctx->strips)
        {
            return NULL;
        }
        ctx->strips_alloc = (uint16_t)CINEPAK_MAX_STRIPS;
    }

    /* Never process more strips than allocated. */
    if (strip_count > ctx->strips_alloc)
    {
        strip_count = ctx->strips_alloc;
    }

    uint16_t cur_y = 0;

    /*
     * When the strip count grows compared with the previous frame, the encoder
     * encoded the new strip(s) relative to the PREVIOUS frame's last strip
     * codebook - not zeros.  Copy that codebook into the new slot(s) BEFORE
     * any strip in the current frame is decoded, so the source reflects the
     * previous frame's final state.
     */
    if (ctx->prev_strip_count > 0u && strip_count > ctx->prev_strip_count)
    {
        uint16_t src_idx = ctx->prev_strip_count - 1u;
        for (uint16_t i = ctx->prev_strip_count; i < strip_count; i++)
        {
            memcpy(ctx->strips[i].v1_codebook, ctx->strips[src_idx].v1_codebook,
                   256u * sizeof(cinepak_codebook_t));
            memcpy(ctx->strips[i].v4_codebook, ctx->strips[src_idx].v4_codebook,
                   256u * sizeof(cinepak_codebook_t));
        }
    }

    for (uint16_t i = 0; i < strip_count; i++)
    {
        /*
         * Intra frames (frame_flags bit0 = 0): codebook is shared across
         * strips within the same frame - each strip starts from the previous
         * strip's just-updated codebook.  (ref: if (i>0 && !(flags&1)))
         * Inter frames (frame_flags bit0 = 1): each strip is independent;
         * it keeps its own state from the previous frame (handled above for
         * new strips; unchanged for existing strips).
         */
        if (i > 0u && !(frame_flags & 0x01u))
        {
            memcpy(ctx->strips[i].v1_codebook, ctx->strips[i - 1].v1_codebook,
                   256u * sizeof(cinepak_codebook_t));
            memcpy(ctx->strips[i].v4_codebook, ctx->strips[i - 1].v4_codebook,
                   256u * sizeof(cinepak_codebook_t));
        }

        /* ---- 12-byte strip header --------------------------------------- */
        if (!stream_avail(&s, 12u))
        {
            break;
        }

        cinepak_strip_t *strip = &ctx->strips[i];
        strip->id              = stream_read_be16(&s);
        uint16_t raw_strip_len = stream_read_be16(&s);
        if (raw_strip_len < 12u)
        {
            break;
        }
        uint32_t strip_data_len = (uint32_t)raw_strip_len - 12u;

        /*
         * Strip rect: the Cinepak bitstream stores (top, left, HEIGHT, right).
         * The third field is the STRIP HEIGHT (relative), not the absolute
         * bottom coordinate.  top/left/right are always ignored; we compute
         * bottom from cur_y + strip_height.  (ref: bottom = _y + F2)
         */
        s.pos           += 2u;                    /* top   - ignored */
        s.pos           += 2u;                    /* left  - ignored */
        uint16_t strip_height = stream_read_be16(&s);
        s.pos           += 2u;                    /* right - ignored */

        strip->top    = cur_y;
        strip->left   = 0u;
        strip->bottom = cur_y + strip_height;
        if (strip->bottom > ctx->height)
        {
            strip->bottom = ctx->height;
        }
        /* Round down to a 4-row boundary to keep block writes in-bounds. */
        strip->bottom = strip->bottom & (uint16_t)~3u;
        strip->right  = ctx->width;

        size_t strip_start = s.pos;
        size_t strip_end   = strip_start + (size_t)strip_data_len;
        /* Clamp to actual data size, guard against integer overflow. */
        if (strip_end > s.size || strip_end < strip_start)
        {
            strip_end = s.size;
        }

        /* ---- Process chunks within this strip --------------------------- */
        while (s.pos + 4u <= strip_end)
        {
            uint8_t  chunk_id  = stream_read_u8(&s);
            uint32_t raw_csz   = (uint32_t)stream_read_u8(&s) << 16;
            raw_csz           |= (uint32_t)stream_read_be16(&s);

            if (raw_csz < 4u)
            {
                break;
            }
            uint32_t chunk_size  = raw_csz - 4u;
            size_t   chunk_start = s.pos;
            size_t   chunk_end   = chunk_start + (size_t)chunk_size;
            if (chunk_end > strip_end || chunk_end < chunk_start)
            {
                chunk_end = strip_end;
            }

            switch (chunk_id)
            {
            case 0x20: case 0x21: case 0x24: case 0x25:
                load_codebook(strip->v4_codebook, &s,
                              chunk_start, chunk_size, chunk_id);
                break;
            case 0x22: case 0x23: case 0x26: case 0x27:
                load_codebook(strip->v1_codebook, &s,
                              chunk_start, chunk_size, chunk_id);
                break;
            case 0x30: case 0x31: case 0x32:
                decode_vectors(ctx, strip, &s,
                               chunk_start, chunk_size, chunk_id);
                break;
            default:
                break;
            }

            /* Seek to the end of the chunk regardless of bytes consumed. */
            s.pos = chunk_end;
        }

        /* Seek to the end of the strip before starting the next one. */
        s.pos  = strip_end;
        cur_y  = strip->bottom;
    }

    ctx->prev_strip_count = strip_count;

    return ctx->pixels;
}

void cinepak_decoder_destroy(cinepak_decoder_t *ctx)
{
    if (ctx)
    {
        CINEPAK_FREE(ctx->strips);
        CINEPAK_FREE(ctx);
    }
}

int cinepak_decoder_reset(cinepak_decoder_t *ctx,
                          uint16_t width, uint16_t height,
                          uint8_t *pixel_buf)
{
    if (!ctx || !pixel_buf)
    {
        return -1;
    }
    if (!width || !height || (width % 4u) || (height % 4u))
    {
        return -1;
    }

    /* Free old strip state; next cinepak_decode_frame will reallocate. */
    CINEPAK_FREE(ctx->strips);
    ctx->strips            = NULL;
    ctx->strips_alloc      = 0u;
    ctx->prev_strip_count  = 0u;

    ctx->width  = width;
    ctx->height = height;
    ctx->pixels = (uint16_t *)pixel_buf;
    return 0;
}

uint16_t cinepak_decoder_get_width(const cinepak_decoder_t *ctx)
{
    return ctx ? ctx->width : 0u;
}

uint16_t cinepak_decoder_get_height(const cinepak_decoder_t *ctx)
{
    return ctx ? ctx->height : 0u;
}
