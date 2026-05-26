/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef MSV1_DECODER_H
#define MSV1_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/**
 * Opaque decoder context.
 *
 * The context owns an internal pixel buffer that is updated in-place on every
 * decoded frame.  Skip-blocks leave pixels from the previous frame untouched,
 * so the buffer MUST persist between calls to msv1_decode_frame().
 */
typedef struct msv1_decoder msv1_decoder_t;

/**
 * Create an MSV1 decoder instance.
 *
 * @param width          Frame width in pixels.  Must be a multiple of 4.
 * @param height         Frame height in pixels.  Must be a multiple of 4.
 * @param bits_per_pixel Colour depth: 8 (palette indices) or 16 (RGB555 LE).
 *
 * @return Pointer to the new context, or NULL on allocation failure or
 *         invalid arguments.
 */
// msv1_decoder_t *msv1_decoder_create(uint16_t width, uint16_t height,
//                                     uint8_t bits_per_pixel);
msv1_decoder_t *msv1_decoder_create(uint16_t width, uint16_t height,
                                    uint8_t bits_per_pixel, uint8_t *pixel_buf);
/**
 * Decode one compressed MSV1 frame.
 *
 * The internal pixel buffer is updated in-place and a pointer to it is
 * returned.  The caller MUST NOT free this pointer.  The buffer remains valid
 * until the next call to msv1_decode_frame() or msv1_decoder_destroy().
 *
 * Output memory layout (bottom-up, BMP convention):
 *   - 8 bpp  : uint8_t  [width * height]   -- palette indices (0-255)
 *   - 16 bpp : uint16_t [width * height]   -- RGB555, little-endian
 *
 * Memory offset 0 corresponds to the BOTTOM row of the visual image.
 * Row n starts at offset (n * width) for 8 bpp, (n * width * 2) for 16 bpp.
 *
 * @param ctx      Decoder context created by msv1_decoder_create().
 * @param src      Pointer to the compressed frame data.
 * @param src_size Byte length of the compressed data.
 *
 * @return Pointer to the decoded pixel buffer, or NULL if ctx/src is NULL or
 *         src_size is 0.
 */
const void *msv1_decode_frame(msv1_decoder_t *ctx,
                              const uint8_t *src, size_t src_size);

/**
 * Destroy the decoder and release all associated memory.
 *
 * @param ctx  Decoder context, or NULL (no-op).
 */
void msv1_decoder_destroy(msv1_decoder_t *ctx);

/**
 * Decode one frame and write the result as packed RGB565 into dst.
 *
 *  - 16-bpp source: each RGB555 pixel is widened to RGB565 (G LSB padded to 0).
 *  - 8-bpp source:  each palette index is looked up in palette[] and the
 *                   resulting RGB565 value is written.  palette must hold
 *                   exactly 256 entries.
 *
 * This is a convenience wrapper around msv1_decode_frame() that avoids
 * an extra conversion pass in the caller.
 *
 * @param ctx      Decoder context.
 * @param src      Compressed frame data.
 * @param src_size Byte count of src.
 * @param dst      Output buffer; caller must allocate (width * height * 2) bytes.
 * @param palette  RGB565 lookup table with 256 entries; used only for 8-bpp
 *                 sources; must be NULL for 16-bpp sources (ignored but
 *                 passing NULL makes the intent explicit).
 *
 * @return 0 on success, -1 on invalid arguments.
 */
int msv1_decode_frame_rgb565(msv1_decoder_t *ctx,
                             const uint8_t  *src,
                             size_t          src_size,
                             uint16_t       *dst,
                             const uint16_t *palette);

/**
 * Return the frame width stored in the context (pixels).
 */
uint16_t msv1_decoder_get_width(const msv1_decoder_t *ctx);

/**
 * Return the frame height stored in the context (pixels).
 */
uint16_t msv1_decoder_get_height(const msv1_decoder_t *ctx);

/**
 * Return the bit depth stored in the context (8 or 16).
 */
uint8_t msv1_decoder_get_bpp(const msv1_decoder_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* MSV1_DECODER_H */
