/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CINEPAK_DECODER_H
#define CINEPAK_DECODER_H

/*============================================================================*
 *                        Cinepak Video Decoder - Public API
 *
 *  Decodes individual Cinepak-compressed frames into packed RGB565 pixels.
 *  Output layout: top-down rows, width * height * 2 bytes.
 *
 *  Interface mirrors gui_msv1/msv1_decoder.h.
 *
 *  Typical usage:
 *
 *    uint8_t *buf = malloc(width * height * 2u);
 *    cinepak_decoder_t *ctx = cinepak_decoder_create(width, height, buf);
 *
 *    // per-frame:
 *    const uint16_t *px = cinepak_decode_frame(ctx, frame_data, frame_size);
 *
 *    cinepak_decoder_destroy(ctx);
 *    free(buf);
 *
 *  Constraints:
 *    - width and height must be > 0 and divisible by 4.
 *    - pixel_buf must be 2-byte aligned and hold at least width*height*2 bytes.
 *    - The codebook state persists between frames; do NOT free the context
 *      between frames (delta frames depend on previous codebooks).
 *    - Call cinepak_decoder_reset() when switching video sources or seeking
 *      to a random position, to clear stale codebook state.
 *============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/*============================================================================*
 *  Compile-time configuration macros
 *
 *  Define these (e.g. in a project-wide config header or via -D compiler flag)
 *  BEFORE including this header.
 *
 *  CINEPAK_PRECOMPUTE_COLORS
 *    When defined, YUV - RGB565 conversion is performed once when each
 *    codebook entry is loaded / updated.  The codebook stores four
 *    pre-converted RGB565 values instead of raw Y/U/V bytes.  Block
 *    decoding then becomes a pure 4-pixel copy with no per-pixel
 *    arithmetic, which is faster on MCUs without hardware multiply.
 *    Trade-off: sizeof(cinepak_codebook_t) grows from 6 to 8 bytes, and
 *    codebook memcpy (for strip inheritance) copies slightly more data.
 *    Enable this macro when decode throughput matters more than RAM.
 *============================================================================*/
#define CINEPAK_PRECOMPUTE_COLORS


/** Opaque decoder context. */
typedef struct cinepak_decoder cinepak_decoder_t;

/**
 * @brief Create a Cinepak decoder context.
 *
 * @param width      Frame width in pixels (> 0, multiple of 4).
 * @param height     Frame height in pixels (> 0, multiple of 4).
 * @param pixel_buf  Caller-allocated RGB565 output buffer,
 *                   at least width * height * 2 bytes, 2-byte aligned.
 *                   The decoder writes directly into this buffer; the caller
 *                   owns and must free it.
 * @return Non-NULL context on success, NULL on invalid args or alloc failure.
 */
cinepak_decoder_t *cinepak_decoder_create(uint16_t width, uint16_t height,
                                          uint8_t *pixel_buf);

/**
 * @brief Decode one Cinepak frame.
 *
 * Writes RGB565 pixels top-down into the buffer supplied at creation time.
 * The codebook state is preserved across calls; delta frames reference
 * codebooks from the previous frame.
 *
 * @param ctx       Decoder context.
 * @param src       Pointer to one complete Cinepak frame blob (big-endian).
 * @param src_size  Byte length of @p src.
 * @return Pointer to the RGB565 pixel buffer on success, NULL on error.
 *         The pointer remains valid until the context is destroyed or reset.
 */
const void *cinepak_decode_frame(cinepak_decoder_t *ctx,
                                 const uint8_t *src, size_t src_size);

/**
 * @brief Destroy a decoder context and free internal resources.
 *
 * The caller-supplied pixel buffer is NOT freed.
 *
 * @param ctx  Context to destroy (NULL is safe - no-op).
 */
void cinepak_decoder_destroy(cinepak_decoder_t *ctx);

/**
 * @brief Reset a decoder to new dimensions and/or pixel buffer.
 *
 * All internal codebook state is cleared.  The next call to
 * cinepak_decode_frame() should supply a keyframe for correct output.
 *
 * @param ctx        Existing decoder context (must not be NULL).
 * @param width      New frame width (> 0, multiple of 4).
 * @param height     New frame height (> 0, multiple of 4).
 * @param pixel_buf  New pixel buffer (caller-owned, see cinepak_decoder_create).
 * @return 0 on success, -1 on invalid arguments (context unchanged).
 */
int cinepak_decoder_reset(cinepak_decoder_t *ctx,
                          uint16_t width, uint16_t height,
                          uint8_t *pixel_buf);

/**
 * @brief Return the configured frame width.
 * @param ctx  Decoder context (NULL - 0).
 */
uint16_t cinepak_decoder_get_width(const cinepak_decoder_t *ctx);

/**
 * @brief Return the configured frame height.
 * @param ctx  Decoder context (NULL - 0).
 */
uint16_t cinepak_decoder_get_height(const cinepak_decoder_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* CINEPAK_DECODER_H */
