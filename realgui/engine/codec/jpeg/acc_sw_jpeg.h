/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __ACC_SW_JPEG_H__
#define __ACC_SW_JPEG_H__

#include "guidef.h"


#ifdef __cplusplus
extern "C" {
#endif

void *gui_acc_jpeg_load(void *input, int len, int *w, int *h, int *channel);
void gui_acc_jpeg_release(void *decoded);

/**
 * @brief Encode a pixel buffer into JPEG.
 *
 * THE RETURNED SHAPE DEPENDS ON THE BUILD, mirroring how the encoder itself was
 * compiled:
 *
 *   GUI_STBIW_JPEG_HEADER == 0    a bare .jpg. The pointer is the stream, it
 *                                 opens with SOI, and *out_len is its length.
 *                                 Write it to disk, feed it to a decoder, or
 *                                 wrap it in a container yourself.
 *
 *   GUI_STBIW_JPEG_HEADER == 16   a gui_jpeg_file_head_t as def_file.h
 *                                 declares it, with jpeg[] at +16 and *out_len
 *                                 counting the head. This is what
 *                                 gui_img_create_from_mem() consumes directly:
 *                                 draw_img.c sees img_header.type == JPEG and
 *                                 decodes it itself.
 *
 * The header macro lives in acc_sw_jpeg.c and is currently 0, so callers today
 * get a bare JPEG. Read that definition rather than assuming.
 *
 * @param pixels      Source pixels, laid out as @p bpp describes.
 * @param w           Width in pixels.
 * @param h           Height in pixels.
 * @param bpp         Bits per pixel:
 *                      16  packed RGB565, one 16-bit little-endian word per
 *                          pixel (r:5 g:6 b:5). This is the layout the display
 *                          controller and gui_img_tree_convert_to_img() produce,
 *                          so a screenshot can be encoded without first being
 *                          expanded to 24/32 bpp.
 *                      24  RGB888
 *                      32  ARGB8888
 *                    Anything else is rejected.
 * @param quality     1..100. Values outside the range are rejected.
 * @param buff_sz     Hard ceiling on the returned block, in bytes. 0 means "no
 *                    ceiling". With a container shape, a non-zero value that
 *                    cannot hold gui_jpeg_file_head_t is rejected. If the encode
 *                    needs more than the ceiling the call returns NULL rather
 *                    than a partial block.
 * @param out_len     Optional, receives the size of the returned block. Set to 0
 *                    on failure.
 * @return Encoded block, or NULL on failure. Free with gui_acc_jpeg_free()
 *         or, for the container shape, give ownership to a gui_img.
 */
void *gui_acc_jpeg_compress(void *input, int w, int h, int bpp,
                            int quality, int buf_len, int *out_len);

/**
 * @brief Release a block returned by gui_acc_jpeg_compress().
 */
void gui_acc_jpeg_free(void *encoded);

#ifdef __cplusplus
}
#endif

#endif
