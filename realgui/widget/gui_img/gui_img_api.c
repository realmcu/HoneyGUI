/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_fb.h"

uint8_t *gui_rle_solid_color_create(uint16_t width, uint16_t height, gui_color_t color)
{
    /* RLE compression algorithm constants (compress/rle.ts: COMPRESS_RLE = 0) */
#define PURE_RLE_ALGORITHM   0
#define PURE_RLE_FEATURE_1   1   /* RLECompression default runLength1 = 1 */
#define PURE_RLE_FEATURE_2   0   /* default runLength2 = 0 */
#define PURE_RLE_MAX_RUN     255 /* max repeat count per RLE node */

    /* gui_rgb_data_head_t flags byte: set compress bit only (bit4) */
#define PURE_RLE_HEAD_FLAGS  0x10

    gui_dispdev_t *dc = gui_get_dc();
    uint8_t bpp = dc->bit_depth / 8;
    bpp = bpp > 3 ? 3 : bpp;

    size_t total = 0;
    {
        /* width identical pixels per row -> ceil(width/255) RLE nodes */
        size_t nodes_per_line = ((size_t)width + (PURE_RLE_MAX_RUN - 1)) / PURE_RLE_MAX_RUN;
        size_t bytes_per_line = nodes_per_line * (size_t)(1 + bpp);

        size_t header_bytes = 8                                   /* gui_rgb_data_head_t */
                              + 12                                /* imdc_file_header_t  */
                              + (size_t)(height + 1) * 4;         /* row offset table    */

        total = header_bytes + bytes_per_line * (size_t)height;
    }

    void *buffer = gui_malloc(total);
    GUI_ASSERT(buffer != NULL);
    uint8_t *p = buffer;

    /* ---- 1. gui_rgb_data_head_t (8B) ---- */
    p[0] = PURE_RLE_HEAD_FLAGS;             /* flags: compress=1 */
    p[1] = bpp == 2 ? 0 : 3;                /* type               */
    p[2] = (uint8_t)(width & 0xFF);         /* width  (int16 LE)  */
    p[3] = (uint8_t)((width >> 8) & 0xFF);
    p[4] = (uint8_t)(height & 0xFF);        /* height (int16 LE)  */
    p[5] = (uint8_t)((height >> 8) & 0xFF);
    p[6] = 0;                               /* version            */
    p[7] = 0;                               /* rsvd2              */
    p += 8;

    /* ---- 2. imdc_file_header_t (12B) ---- */
    p[0] = (uint8_t)((PURE_RLE_ALGORITHM & 0x03)
                     | ((PURE_RLE_FEATURE_1 & 0x03) << 2)
                     | ((PURE_RLE_FEATURE_2 & 0x03) << 4)
                     | (((bpp - 2) & 0x03) << 6));
    p[1] = 0;                       /* reserved[0] */
    p[2] = 0;                       /* reserved[1] */
    p[3] = 0;                       /* reserved[2] */
    *((uint32_t *)(p + 4)) = (uint32_t)(width);    /* raw_pic_width  */
    *((uint32_t *)(p + 8)) = (uint32_t)(height);   /* raw_pic_height */
    p += 12;

    /* ---- 3. Row offset table (height+1) * 4B ----
     * offset base = start of imdc header (file offset 8), imdcOffset = 12 + (height+1)*4 */
    size_t nodes_per_line = ((size_t)width + (PURE_RLE_MAX_RUN - 1)) / PURE_RLE_MAX_RUN;
    size_t bytes_per_line = nodes_per_line * (size_t)(1 + bpp);
    uint32_t imdc_offset = (uint32_t)(12 + (size_t)(height + 1) * 4);

    for (uint16_t line = 0; line < height; line++)
    {
        /* All rows have the same byte count: lineOffset = line * bytes_per_line */
        *((uint32_t *)p) = imdc_offset + (uint32_t)((size_t)line * bytes_per_line);
        p += 4;
    }
    /* Last entry = imdc_offset + total compressed data length (end of file) */
    *((uint32_t *)p) = imdc_offset + (uint32_t)(bytes_per_line * (size_t)height);
    p += 4;

    /* ---- 4. RLE compressed data ---- */
    uint8_t pix[3] = {0};
    uint8_t r = color.color.rgba.r;
    uint8_t g = color.color.rgba.g;
    uint8_t b = color.color.rgba.b;
    switch (bpp)
    {
    case 2:
        *((uint16_t *)pix) = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
        break;
    case 3:  /* BGR */
        pix[0] = b;
        pix[1] = g;
        pix[2] = r;
        break;
    default:
        break;
    }

    for (uint16_t line = 0; line < height; line++)
    {
        uint16_t remain = width;
        while (remain > 0)
        {
            uint8_t run = (remain > PURE_RLE_MAX_RUN)
                          ? (uint8_t)PURE_RLE_MAX_RUN
                          : (uint8_t)remain;
            *p++ = run;                      /* [len] */
            memcpy(p, pix, (size_t)bpp);     /* [pixel] */
            p += bpp;
            remain = (uint16_t)(remain - run);
        }
    }

    return buffer;
}


