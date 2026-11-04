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
 * @param bpp         Bits per pixel (24 for RGB888, 32 for ARGB8888).
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
