/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __ACC_SW_DEFINE_H__
#define __ACC_SW_DEFINE_H__

#include "draw_img.h"
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SETUP_DRAW_VARIABLES                                                                      \
    int32_t x_start = 0;                                                                               \
    int32_t x_end = 0;                                                                                 \
    int32_t y_start = 0;                                                                               \
    int32_t y_end = 0;                                                                                 \
    \
    if (draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)             \
    {                                                                                                  \
        return;                                                                                        \
    }                                                                                                  \
    \
    uintptr_t image_base = sizeof(gui_rgb_data_head_t) + (uintptr_t)((image)->data);            \
    uint16_t *writebuf = (uint16_t *)(dc)->frame_buf;                                                   \
    uint16_t *readbuf = (uint16_t *)(uintptr_t)image_base; GUI_UNUSED(readbuf);                                              \
    \
    int16_t source_w = (image)->img_w; GUI_UNUSED(source_w);                                                                 \
    int16_t source_h = (image)->img_h; GUI_UNUSED(source_h);                                                                \
    int16_t rel_x = (image)->rel_x; GUI_UNUSED(rel_x);                                                                 \
    int16_t rel_y = (image)->rel_y; GUI_UNUSED(rel_y);                                                                 \
    int16_t y1 = (dc)->section.y1; GUI_UNUSED(y1);                                                                  \
    int16_t y2 = (dc)->section.y2; GUI_UNUSED(y2);                                                                    \
    int16_t x1 = (dc)->section.x1; GUI_UNUSED(x1);                                                                    \
    int16_t x2 = (dc)->section.x2; GUI_UNUSED(x2);                                                                    \
    uint32_t fg = uint32_to_rgb565(image->fg_color_set); GUI_UNUSED(fg);                                              \
    uint32_t bg = uint32_to_rgb565(image->bg_color_fix); GUI_UNUSED(bg);                                              \
    uint8_t opacity_value = (image)->opacity_value; GUI_UNUSED(opacity_value);




#define PROCESS_IMAGE_PIXEL_1D(PIXEL_TYPE, PIXEL_ACTION)                                                       \
    do {                                                                                                    \
        for (int32_t i = y_start; i <= y_end; i++)                                                          \
        {                                                                                                   \
            int write_offset = (i - y1) * (x2 - x1 + 1) + x_start - x1;                                        \
            int read_offset = (int32_t)((i - rel_y) * source_w + x_start - rel_x);                                 \
            PIXEL_TYPE *pixel = (PIXEL_TYPE *)(uintptr_t)image_base + read_offset;                             \
            for (int32_t j = x_start; j <= x_end; j++)                                                      \
            {                                                                                               \
                PIXEL_ACTION;                                                                               \
                write_offset++;                                                                                \
                pixel++;                                                                                    \
            }                                                                                               \
        }                                                                                                   \
    } while(0)

static inline uint16_t rgb565_fast_blending(uint32_t fg, uint32_t bg, uint8_t alpha)
{
    // Alpha converted from [0..255] to [0..31] for 5-bit precision
    alpha = (alpha + 4) >> 3;

    // Unpack pixels into a 32-bit format for parallel operations
    // Converts  rrrrrggggggbbbbb  ->  00000gggggg00000rrrrr000000bbbbb
    // Mask: 0x07E0F81F -> 00000111111000001111100000011111
    bg = ((bg | (bg << 16)) & 0x07e0f81f);
    fg = ((fg | (fg << 16)) & 0x07e0f81f);

    // Perform parallel blending: result = ((fg - bg) * alpha) / 32 + bg
    uint32_t result = (((((fg - bg) * alpha) >> 5) + bg) & 0x07e0f81f);

    // Pack the result back into a 16-bit RGB565 value
    return (uint16_t)((result >> 16) | result);
}

static inline uint16_t uint32_to_rgb565(uint32_t argb)
{
    uint8_t b8 = (uint8_t)(argb & 0xFF);
    uint8_t g8 = (uint8_t)((argb >> 8) & 0xFF);
    uint8_t r8 = (uint8_t)((argb >> 16) & 0xFF);
    // Alpha (argb >> 24) not used in RGB565 conversion

    uint16_t r5 = (uint16_t)(r8 >> 3);
    uint16_t g6 = (uint16_t)(g8 >> 2);
    uint16_t b5 = (uint16_t)(b8 >> 3);

    return (uint16_t)((r5 << 11) | (g6 << 5) | b5);
}

#ifdef __cplusplus
}
#endif

#endif
