/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "msv1_decoder.h"
#include <stdlib.h>
#include <string.h>

/*============================================================================*
 *                        Memory Allocation Hooks
 *
 *  Override these macros before including this translation unit to redirect
 *  allocation to gui_malloc / gui_free or any RTOS heap.
 *============================================================================*/
#ifndef MSV1_MALLOC
#  define MSV1_MALLOC(sz)     malloc(sz)
#endif
#ifndef MSV1_CALLOC
#  define MSV1_CALLOC(n, sz)  calloc((n), (sz))
#endif
#ifndef MSV1_FREE
#  define MSV1_FREE(p)        free(p)
#endif

/*============================================================================*
 *                        Private Types
 *============================================================================*/

/* Lightweight read-only byte stream -- replaces Common::SeekableReadStream */
typedef struct
{
    const uint8_t *data;
    size_t         size;
    size_t         pos;
} stream_t;

/* Decoder context -- owns the persistent pixel buffer */
struct msv1_decoder
{
    uint16_t width;
    uint16_t height;
    uint8_t  bits_per_pixel;  /* 8 or 16 */
    void    *pixels;          /* bottom-up frame buffer, updated in-place */
};

/*============================================================================*
 *                        Private: Stream Helpers
 *============================================================================*/

/* Returns non-zero if at least n bytes remain from the current position. */
static inline int stream_avail(const stream_t *s, size_t n)
{
    return (s->pos + n) <= s->size;
}

static inline uint8_t stream_read_u8(stream_t *s)
{
    return s->data[s->pos++];
}

static inline uint16_t stream_read_u16le(stream_t *s)
{
    uint16_t v = (uint16_t)s->data[s->pos] |
                 ((uint16_t)s->data[s->pos + 1] << 8);
    s->pos += 2;
    return v;
}

/*============================================================================*
 *                        Private: 8-bpp Decoder
 *
 *  Decodes palette-index frames (CRAM 8-bit variant).
 *  Output: uint8_t[width * height], bottom-up row order.
 *
 *  Block encoding modes (per 4x4 block):
 *    Skip   -- keep previous frame pixels
 *    1-color -- fill block with one palette index
 *    2-color -- two palette indices selected per pixel via a 16-bit flags word
 *    8-color -- eight palette indices, quad-selected per pixel via flags word
 *============================================================================*/
static void decode8(msv1_decoder_t *ctx, stream_t *s)
{
    uint8_t  colors[8];
    uint8_t *pixels     = (uint8_t *)ctx->pixels;
    uint16_t stride     = ctx->width;

    int      skip_blocks  = 0;
    uint16_t blocks_wide  = ctx->width  / 4;
    uint16_t blocks_high  = ctx->height / 4;
    uint32_t total_blocks = (uint32_t)blocks_wide * blocks_high;
    uint32_t block_inc    = 4;

    /*
     * row_dec is applied after writing 4 pixels to the right:
     *   pixel_ptr has advanced by +4, then we subtract (stride + 4)
     *   -> net movement = -stride  (one row upward in memory)
     */
    uint32_t row_dec = (uint32_t)stride + 4u;

    for (uint16_t block_y = blocks_high; block_y > 0; block_y--)
    {
        /* Start at the bottom row of this 4-row block (bottom-up layout). */
        uint32_t block_ptr = (uint32_t)(block_y * 4u - 1u) * stride;

        for (uint16_t block_x = blocks_wide; block_x > 0; block_x--)
        {
            /* Skip code: leave previous pixels in place. */
            if (skip_blocks > 0)
            {
                block_ptr += block_inc;
                skip_blocks--;
                total_blocks--;
                continue;
            }

            uint32_t pixel_ptr = block_ptr;

            if (!stream_avail(s, 2))
            {
                return;
            }
            uint8_t byte_a = stream_read_u8(s);
            uint8_t byte_b = stream_read_u8(s);

            /* End-of-stream sentinel. */
            if (byte_a == 0 && byte_b == 0 && total_blocks == 0)
            {
                return;
            }
            else if ((byte_b & 0xFCu) == 0x84u)
            {
                /* Skip N blocks (not counting the current one). */
                skip_blocks = (int)(((uint32_t)(byte_b - 0x84u) << 8) + byte_a) - 1;
            }
            else if (byte_b < 0x80u)
            {
                /* 2-color encoding: 16-bit flags select color[0] or color[1]. */
                uint16_t flags = ((uint16_t)byte_b << 8) | byte_a;

                if (!stream_avail(s, 2))
                {
                    return;
                }
                colors[0] = stream_read_u8(s);
                colors[1] = stream_read_u8(s);

                for (uint8_t py = 0; py < 4; py++)
                {
                    for (uint8_t px = 0; px < 4; px++, flags >>= 1)
                    {
                        pixels[pixel_ptr++] = colors[(flags & 0x1u) ^ 1u];
                    }
                    pixel_ptr -= row_dec;
                }
            }
            else if (byte_b >= 0x90u)
            {
                /* 8-color encoding: flag bit + 2-bit quad position -> one of 8 colors. */
                uint16_t flags = ((uint16_t)byte_b << 8) | byte_a;

                if (!stream_avail(s, 8))
                {
                    return;
                }
                for (uint8_t i = 0; i < 8; i++)
                {
                    colors[i] = stream_read_u8(s);
                }

                for (uint8_t py = 0; py < 4; py++)
                {
                    for (uint8_t px = 0; px < 4; px++, flags >>= 1)
                    {
                        pixels[pixel_ptr++] =
                            colors[((py & 0x2u) << 1) + (px & 0x2u) +
                                                ((flags & 0x1u) ^ 1u)];
                    }
                    pixel_ptr -= row_dec;
                }
            }
            else
            {
                /* 1-color encoding: fill the entire 4x4 block. */
                colors[0] = byte_a;

                for (uint8_t py = 0; py < 4; py++)
                {
                    for (uint8_t px = 0; px < 4; px++)
                    {
                        pixels[pixel_ptr++] = colors[0];
                    }
                    pixel_ptr -= row_dec;
                }
            }

            block_ptr += block_inc;
            total_blocks--;
        }
    }
}

/*============================================================================*
 *                        Private: 16-bpp Decoder
 *
 *  Decodes RGB555 frames (CRAM 16-bit variant).
 *  Output: uint16_t[width * height], bottom-up row order, little-endian.
 *
 *  The 8-color mode is detected by bit 15 of the first color word being set.
 *============================================================================*/
static void decode16(msv1_decoder_t *ctx, stream_t *s)
{
    uint16_t  colors[8];
    uint16_t *pixels    = (uint16_t *)ctx->pixels;
    int32_t   stride    = (int32_t)ctx->width;

    int32_t skip_blocks  = 0;
    int32_t blocks_wide  = (int32_t)(ctx->width  / 4);
    int32_t blocks_high  = (int32_t)(ctx->height / 4);
    int32_t total_blocks = blocks_wide * blocks_high;
    int32_t block_inc    = 4;
    int32_t row_dec      = stride + 4;

    for (int32_t block_y = blocks_high; block_y > 0; block_y--)
    {
        int32_t block_ptr = (block_y * 4 - 1) * stride;

        for (int32_t block_x = blocks_wide; block_x > 0; block_x--)
        {
            if (skip_blocks)
            {
                block_ptr += block_inc;
                skip_blocks--;
                total_blocks--;
                continue;
            }

            int32_t pixel_ptr = block_ptr;

            if (!stream_avail(s, 2))
            {
                return;
            }
            uint8_t byte_a = stream_read_u8(s);
            uint8_t byte_b = stream_read_u8(s);

            if ((byte_a == 0) && (byte_b == 0) && (total_blocks == 0))
            {
                return;
            }
            else if ((byte_b & 0xFCu) == 0x84u)
            {
                skip_blocks = (int32_t)(((uint32_t)(byte_b - 0x84u) << 8) + byte_a) - 1;
            }
            else if (byte_b < 0x80u)
            {
                uint16_t flags = ((uint16_t)byte_b << 8) | byte_a;

                if (!stream_avail(s, 4))
                {
                    return;
                }
                colors[0] = stream_read_u16le(s);
                colors[1] = stream_read_u16le(s);

                if (colors[0] & 0x8000u)
                {
                    /* 8-color encoding (bit 15 of first color is the discriminant). */
                    if (!stream_avail(s, 12))
                    {
                        return;
                    }
                    colors[2] = stream_read_u16le(s);
                    colors[3] = stream_read_u16le(s);
                    colors[4] = stream_read_u16le(s);
                    colors[5] = stream_read_u16le(s);
                    colors[6] = stream_read_u16le(s);
                    colors[7] = stream_read_u16le(s);

                    colors[0] = (uint16_t)(((colors[0] & 0x7FE0u) << 1u) | (colors[0] & 0x001Fu));
                    colors[1] = (uint16_t)(((colors[1] & 0x7FE0u) << 1u) | (colors[1] & 0x001Fu));
                    colors[2] = (uint16_t)(((colors[2] & 0x7FE0u) << 1u) | (colors[2] & 0x001Fu));
                    colors[3] = (uint16_t)(((colors[3] & 0x7FE0u) << 1u) | (colors[3] & 0x001Fu));
                    colors[4] = (uint16_t)(((colors[4] & 0x7FE0u) << 1u) | (colors[4] & 0x001Fu));
                    colors[5] = (uint16_t)(((colors[5] & 0x7FE0u) << 1u) | (colors[5] & 0x001Fu));
                    colors[6] = (uint16_t)(((colors[6] & 0x7FE0u) << 1u) | (colors[6] & 0x001Fu));
                    colors[7] = (uint16_t)(((colors[7] & 0x7FE0u) << 1u) | (colors[7] & 0x001Fu));

                    for (int py = 0; py < 4; py++)
                    {
                        for (int px = 0; px < 4; px++, flags >>= 1)
                        {
                            pixels[pixel_ptr++] =
                                colors[((py & 0x2) << 1) + (px & 0x2) +
                                                   ((flags & 0x1u) ^ 1u)];
                        }
                        pixel_ptr -= row_dec;
                    }
                }
                else
                {
                    /* 2-color encoding. */
                    colors[0] = (uint16_t)(((colors[0] & 0x7FE0u) << 1u) | (colors[0] & 0x001Fu));
                    colors[1] = (uint16_t)(((colors[1] & 0x7FE0u) << 1u) | (colors[1] & 0x001Fu));
                    for (int py = 0; py < 4; py++)
                    {
                        for (int px = 0; px < 4; px++, flags >>= 1)
                        {
                            pixels[pixel_ptr++] = colors[(flags & 0x1u) ^ 1u];
                        }
                        pixel_ptr -= row_dec;
                    }
                }
            }
            else
            {
                /* 1-color encoding. */
                colors[0] = ((uint16_t)byte_b << 8) | byte_a;
                colors[0] = (uint16_t)(((colors[0] & 0x7FE0u) << 1u) | (colors[0] & 0x001Fu));

                for (int py = 0; py < 4; py++)
                {
                    for (int px = 0; px < 4; px++)
                    {
                        pixels[pixel_ptr++] = colors[0];
                    }
                    pixel_ptr -= row_dec;
                }
            }

            block_ptr += block_inc;
            total_blocks--;
        }
    }
}

/*============================================================================*
 *                        Public API
 *============================================================================*/

msv1_decoder_t *msv1_decoder_create(uint16_t width, uint16_t height,
                                    uint8_t bits_per_pixel, uint8_t *pixel_buf)
{
    if (bits_per_pixel != 8 && bits_per_pixel != 16)
    {
        return NULL;
    }
    if ((width % 4) != 0 || (height % 4) != 0 || width == 0 || height == 0)
    {
        return NULL;
    }

    msv1_decoder_t *ctx = (msv1_decoder_t *)MSV1_MALLOC(sizeof(msv1_decoder_t));
    if (!ctx)
    {
        return NULL;
    }

    ctx->pixels = pixel_buf;  /* caller owns buffer; decoder writes directly into it */
    if (!ctx->pixels)
    {
        MSV1_FREE(ctx);
        return NULL;
    }

    ctx->width          = width;
    ctx->height         = height;
    ctx->bits_per_pixel = bits_per_pixel;

    return ctx;
}

const void *msv1_decode_frame(msv1_decoder_t *ctx,
                              const uint8_t *src, size_t src_size)
{
    if (!ctx || !src || src_size == 0)
    {
        return NULL;
    }

    stream_t s;
    s.data = src;
    s.size = src_size;
    s.pos  = 0;

    if (ctx->bits_per_pixel == 8)
    {
        decode8(ctx, &s);
    }
    else
    {
        decode16(ctx, &s);
    }

    return ctx->pixels;
}

void msv1_decoder_destroy(msv1_decoder_t *ctx)
{
    if (ctx)
    {
        // MSV1_FREE(ctx->pixels);
        MSV1_FREE(ctx);
    }
}

int msv1_decode_frame_rgb565(msv1_decoder_t *ctx,
                             const uint8_t  *src,
                             size_t          src_size,
                             uint16_t       *dst,
                             const uint16_t *palette)
{
    if (!ctx || !src || src_size == 0 || !dst)
    {
        return -1;
    }
    if (ctx->bits_per_pixel == 8 && !palette)
    {
        return -1;
    }

    const void *raw = msv1_decode_frame(ctx, src, src_size);
    if (!raw)
    {
        return -1;
    }

    uint32_t n = (uint32_t)ctx->width * ctx->height;

    if (ctx->bits_per_pixel == 16)
    {
        /* decode16() already wrote RGB565 directly into ctx->pixels (= dst).
         * No conversion or copy needed -- the data is ready in-place. */
        (void)raw;
        (void)n;
    }
    else /* 8 bpp */
    {
        const uint8_t *sp = (const uint8_t *)raw;
        for (uint32_t i = 0; i < n; i++)
        {
            dst[i] = palette[sp[i]];
        }
    }

    return 0;
}

uint16_t msv1_decoder_get_width(const msv1_decoder_t *ctx)
{
    return ctx ? ctx->width : 0;
}

uint16_t msv1_decoder_get_height(const msv1_decoder_t *ctx)
{
    return ctx ? ctx->height : 0;
}

uint8_t msv1_decoder_get_bpp(const msv1_decoder_t *ctx)
{
    return ctx ? ctx->bits_per_pixel : 0;
}
