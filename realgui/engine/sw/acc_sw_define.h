/**
 * @file acc_sw_bypass.h
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2025-10-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __ACC_SW_DEFINE_H__
#define __ACC_SW_DEFINE_H__

#include <draw_img.h>
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
    uint32_t image_base = sizeof(gui_rgb_data_head_t) + (uint32_t)(uintptr_t)((image)->data);            \
    uint16_t *writebuf = (uint16_t *)(dc)->frame_buf;                                                   \
    uint16_t *readbuf = (uint16_t *)(uintptr_t)image_base; GUI_UNUSED(readbuf);                                              \
    \
    int16_t source_w = (image)->img_w; GUI_UNUSED(source_w);                                                                 \
    int16_t source_h = (image)->img_h; GUI_UNUSED(source_h);                                                                \
    gui_matrix_t *inverse = &(image)->inverse;                                                         \
    \
    float m00 = inverse->m[0][0]; GUI_UNUSED(m00);  \
    float m01 = inverse->m[0][1]; GUI_UNUSED(m01);  \
    float m02 = inverse->m[0][2]; GUI_UNUSED(m02);  \
    float m10 = inverse->m[1][0]; GUI_UNUSED(m10);  \
    float m11 = inverse->m[1][1]; GUI_UNUSED(m11);  \
    float m12 = inverse->m[1][2]; GUI_UNUSED(m12);  \
    \
    int16_t y1 = (dc)->section.y1; GUI_UNUSED(y1);                                                                    \
    int16_t y2 = (dc)->section.y2; GUI_UNUSED(y2);                                                                    \
    int16_t x1 = (dc)->section.x1; GUI_UNUSED(x1);                                                                    \
    int16_t x2 = (dc)->section.x2; GUI_UNUSED(x2);                                                                    \
    uint32_t fg = uint32_to_rgb565(image->fg_color_set); GUI_UNUSED(fg);                                              \
    uint32_t bg = uint32_to_rgb565(image->bg_color_fix); GUI_UNUSED(bg);                                              \
    uint8_t mix_alpha = (image)->alpha_mix; GUI_UNUSED(mix_alpha);                                                             \
    uint8_t opacity_value = (image)->opacity_value; GUI_UNUSED(opacity_value);




#define PROCESS_IMAGE_PIXEL_1D(PIXEL_TYPE, PIXEL_ACTION)                                                       \
    do {                                                                                                    \
        for (int32_t i = y_start; i <= y_end; i++)                                                          \
        {                                                                                                   \
            int write_offset = (i - y1) * (x2 - x1 + 1) + x_start - x1;                                        \
            int read_offset = (int32_t)((i + m12) * source_w + x_start + m02);                                 \
            PIXEL_TYPE *pixel = (PIXEL_TYPE *)(uintptr_t)image_base + read_offset;                             \
            for (int32_t j = x_start; j <= x_end; j++)                                                      \
            {                                                                                               \
                PIXEL_ACTION;                                                                               \
                write_offset++;                                                                                \
                pixel++;                                                                                    \
            }                                                                                               \
        }                                                                                                   \
    } while(0)

#define PROCESS_IMAGE_PIXEL_2D(PIXEL_TYPE, PIXEL_ACTION)                                                    \
    do {                                                                                                    \
        for (int32_t i = y_start; i <= y_end; i++)                                                          \
        {                                                                                                   \
            float detalX = m01 * i + m02;   \
            float detalY = m11 * i + m12;   \
            float X = m00 * x_start + detalX;   \
            float Y = m10 * x_start + detalY;   \
            int write_off = (i - y1) * (x2 - x1 + 1) - x1;                                               \
            \
            for (int32_t j = x_start; j <= x_end; j++)                                                      \
            {                                                                                               \
                int x = (int)X; /*roundf(X);*/    \
                int y = (int)Y; /*roundf(Y);*/    \
                if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))                   \
                {                                                                               \
                    X += m00;   \
                    Y += m10;   \
                    continue;   \
                }   \
                int write_offset = write_off + j;                                                           \
                int read_offset = y * source_w + x;                                                   \
                X += m00; \
                Y += m10; \
                PIXEL_TYPE *pixel = (PIXEL_TYPE *)(uintptr_t)image_base + read_offset;               \
                PIXEL_ACTION;                                                                               \
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

static inline uint16_t pixel_aliasing_1x2(uint32_t fg_left_rgb, uint32_t fg_right_rgb,
                                          uint8_t fg_left_alpha, uint8_t fg_right_alpha, uint8_t ratio, uint32_t bg)
{
    uint8_t rgb_ratio = (ratio + 4) >> 3;
    fg_left_rgb = ((fg_left_rgb | (fg_left_rgb << 16)) & 0x07e0f81f);
    fg_right_rgb = ((fg_right_rgb | (fg_right_rgb << 16)) & 0x07e0f81f);
    bg = ((bg | (bg << 16)) & 0x07e0f81f);
    uint32_t fg_rgb = (((((fg_right_rgb - fg_left_rgb) * rgb_ratio) >> 5) + fg_left_rgb) & 0x07e0f81f);
    uint8_t fg_alpha = (fg_left_alpha * (255 - ratio) + fg_right_alpha * ratio) >> 8;
    fg_alpha = (fg_alpha + 4) >> 3;
    uint32_t result = (((((fg_rgb - bg) * fg_alpha) >> 5) + bg) & 0x07e0f81f);
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
