/**
 * @file acc_sw_define.h
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
#include <math.h>

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
            float deltaX = m01 * i + m02;   \
            float deltaY = m11 * i + m12;   \
            float X = m00 * x_start + deltaX;   \
            float Y = m10 * x_start + deltaY;   \
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

#define PROCESS_IMAGE_PIXEL_2D_WITH_2X2_ANTI_ALIASING(PIXEL_TYPE, PIXEL_ACTION)                             \
    do {                                                                                                    \
        for (int32_t i = y_start; i <= y_end; i++)                                                          \
        {                                                                                                   \
            float deltaX = m01 * i + m02;   \
            float deltaY = m11 * i + m12;   \
            float X = m00 * x_start + deltaX;  \
            float Y = m10 * x_start + deltaY;  \
            int write_off = (i - y1) * (x2 - x1 + 1) - x1;                                               \
            \
            for (int32_t j = x_start; j <= x_end; j++)                                                      \
            {                 \
                if (!(X > -1.0f && Y > -1.0f && X < (float)source_w && Y < (float)source_h))   \
                { \
                    X += m00; \
                    Y += m10; \
                    continue; \
                } \
                int x = floorf(X);   \
                int y = floorf(Y);   \
                float xRatio = X - (float)x;  GUI_UNUSED(xRatio);\
                float yRatio = Y - (float)y;  GUI_UNUSED(yRatio); \
                int write_offset = write_off + j;                                                           \
                \
                PIXEL_TYPE pixel_dummy = {0};                                                    \
                PIXEL_TYPE *pixel_00 = &pixel_dummy; \
                PIXEL_TYPE *pixel_01 = &pixel_dummy; \
                PIXEL_TYPE *pixel_10 = &pixel_dummy; \
                PIXEL_TYPE *pixel_11 = &pixel_dummy; \
                \
                if ((x != -1) && (y != -1)) {  \
                    pixel_00 = (PIXEL_TYPE *)(uintptr_t)image_base + y * source_w + x;               \
                    if (x + 1 < source_w) { \
                        pixel_01 = pixel_00 + 1;               \
                    } \
                    if ((y + 1 < source_h)) { \
                        pixel_10 = pixel_00 + source_w;               \
                    } \
                    if ((x + 1 < source_w) && (y + 1 < source_h)) { \
                        pixel_11 = pixel_10 + 1;               \
                    } \
                } \
                else if(x != -1) { \
                    pixel_10 = (PIXEL_TYPE *)(uintptr_t)image_base + (y + 1) * source_w + x;               \
                    if ((x + 1 < source_w) && (y + 1 < source_h)) { \
                        pixel_11 = pixel_10 + 1;               \
                    } \
                } \
                else if(y != -1) { \
                    pixel_01 = (PIXEL_TYPE *)(uintptr_t)image_base + y * source_w + x + 1;               \
                    if ((x + 1 < source_w) && (y + 1 < source_h)) { \
                        pixel_11 = pixel_01 + source_w;               \
                    } \
                } \
                else { \
                    pixel_11 = (PIXEL_TYPE *)(uintptr_t)image_base + (y + 1) * source_w + x + 1;           \
                } \
                PIXEL_ACTION;                                                                               \
                X += m00; \
                Y += m10; \
            }                                                                                               \
        }                                                                                                   \
    } while(0)

#define PROCESS_IMAGE_PIXEL_2D_WITH_1X2_ANTI_ALIASING(PIXEL_TYPE, PIXEL_ACTION)                             \
    do {                                                                                                    \
        for (int32_t i = y_start; i <= y_end; i++)                                                          \
        {                                                                                                   \
            float deltaX = m01 * i + m02;   \
            float deltaY = m11 * i + m12;   \
            float X = m00 * x_start + deltaX;  \
            float Y = m10 * x_start + deltaY;  \
            int write_off = (i - y1) * (x2 - x1 + 1) - x1;                                               \
            \
            for (int32_t j = x_start; j <= x_end; j++)                                                      \
            {                 \
                int x = (X >= 0) ? (int)X : (int)(X - 1);   \
                int y = (int)(Y);   \
                if ((uint32_t)x >= (uint16_t)source_w || (uint32_t)y >= (uint16_t)source_h)   \
                { \
                    X += m00; \
                    Y += m10; \
                    continue; \
                } \
                float xRatio = X - (float)x;  GUI_UNUSED(xRatio);\
                int write_offset = write_off + j;                                                           \
                \
                PIXEL_TYPE pixel_dummy = {0};                                                    \
                PIXEL_TYPE *pixel_00 = &pixel_dummy; \
                PIXEL_TYPE *pixel_01 = &pixel_dummy; \
                \
                int image_offset = y * source_w + x;\
                pixel_00 = (PIXEL_TYPE *)(uintptr_t)image_base + image_offset;  \
                if (x < source_w - 1) {\
                    pixel_01 = (PIXEL_TYPE *)(uintptr_t)image_base + image_offset + 1; \
                } \
                PIXEL_ACTION;                                                                               \
                X += m00; \
                Y += m10; \
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

static inline uint16_t pixel_aliasing_1x2(uint32_t pixel1_rgb, uint32_t pixel2_rgb,
                                          uint8_t pixel1_alpha, uint8_t pixel2_alpha, uint8_t ratio, uint32_t bg)
{
    uint8_t rgb_ratio = (ratio + 4) >> 3;
    pixel1_rgb = ((pixel1_rgb | (pixel1_rgb << 16)) & 0x07e0f81f);
    pixel2_rgb = ((pixel2_rgb | (pixel2_rgb << 16)) & 0x07e0f81f);
    bg = ((bg | (bg << 16)) & 0x07e0f81f);
    uint32_t fg_rgb = (((((pixel2_rgb - pixel1_rgb) * rgb_ratio) >> 5) + pixel1_rgb) & 0x07e0f81f);
    uint8_t fg_alpha = (pixel1_alpha * (255 - ratio) + pixel2_alpha * ratio) >> 8;
    fg_alpha = (fg_alpha + 4) >> 3;
    uint32_t result = (((((fg_rgb - bg) * fg_alpha) >> 5) + bg) & 0x07e0f81f);
    return (uint16_t)((result >> 16) | result);
}
static inline uint16_t pixel_aliasing_2x2(uint32_t pixel00_rgb, uint32_t pixel01_rgb,
                                          uint32_t pixel10_rgb, uint32_t pixel11_rgb,
                                          uint8_t pixel00_alpha, uint8_t pixel01_alpha,
                                          uint8_t pixel10_alpha, uint8_t pixel11_alpha,
                                          uint8_t x_ratio, uint8_t y_ratio, uint32_t bg)
{
    uint8_t xr = (x_ratio + 4) >> 3;
    uint8_t yr = (y_ratio + 4) >> 3;

    pixel00_rgb = ((pixel00_rgb | (pixel00_rgb << 16)) & 0x07e0f81f);
    pixel01_rgb = ((pixel01_rgb | (pixel01_rgb << 16)) & 0x07e0f81f);
    pixel10_rgb = ((pixel10_rgb | (pixel10_rgb << 16)) & 0x07e0f81f);
    pixel11_rgb = ((pixel11_rgb | (pixel11_rgb << 16)) & 0x07e0f81f);
    bg = ((bg | (bg << 16)) & 0x07e0f81f);

    uint32_t top_rgb = (((((pixel01_rgb - pixel00_rgb) * xr) >> 5) + pixel00_rgb) & 0x07e0f81f);
    uint32_t bottom_rgb = (((((pixel11_rgb - pixel10_rgb) * xr) >> 5) + pixel10_rgb) & 0x07e0f81f);
    uint32_t fg_rgb = (((((bottom_rgb - top_rgb) * yr) >> 5) + top_rgb) & 0x07e0f81f);

    uint8_t fg_alpha = (pixel00_alpha * (255 - xr) * (255 - yr) +
                        pixel01_alpha * xr * (255 - yr) +
                        pixel10_alpha * (255 - xr) * yr +
                        pixel11_alpha * xr * yr) >> 16;
    fg_alpha = (fg_alpha + 4) >> 3;

    uint32_t result = (((((fg_rgb - bg) * fg_alpha) >> 5) + bg) & 0x07e0f81f);
    return (uint16_t)((result >> 16) | result);
}

static inline uint16_t pixel_aliasing_a8_1x2(uint32_t fg, uint32_t bg, uint8_t pixel00_alpha,
                                             uint8_t pixel01_alpha, uint8_t ratio)
{
    fg = ((fg | (fg << 16)) & 0x07e0f81f);
    bg = ((bg | (bg << 16)) & 0x07e0f81f);
    uint8_t fg_alpha = (pixel00_alpha * (255 - ratio) + pixel01_alpha * ratio) >> 8;
    fg_alpha = (fg_alpha + 4) >> 3;
    uint32_t result = (((((fg - bg) * fg_alpha) >> 5) + bg) & 0x07e0f81f);
    return (uint16_t)((result >> 16) | result);
}

static inline uint16_t pixel_aliasing_a8_2x2(uint32_t fg, uint32_t bg, uint8_t pixel00_alpha,
                                             uint8_t pixel01_alpha, uint8_t pixel10_alpha, uint8_t pixel11_alpha, uint8_t x_ratio,
                                             uint8_t y_ratio)
{
    uint8_t xr = x_ratio;
    uint8_t yr = y_ratio;
    fg = ((fg | (fg << 16)) & 0x07e0f81f);
    bg = ((bg | (bg << 16)) & 0x07e0f81f);
    uint8_t fg_alpha = (pixel00_alpha * (255 - xr) * (255 - yr) +
                        pixel01_alpha * xr * (255 - yr) +
                        pixel10_alpha * (255 - xr) * yr +
                        pixel11_alpha * xr * yr) >> 16;

    fg_alpha = (fg_alpha + 4) >> 3;
    uint32_t result = (((((fg - bg) * fg_alpha) >> 5) + bg) & 0x07e0f81f);
    return (uint16_t)((result >> 16) | result);
}

static inline uint16_t pixel_aliasing_2x2_without_alpha(uint32_t pixel00_rgb, uint32_t pixel01_rgb,
                                                        uint32_t pixel10_rgb, uint32_t pixel11_rgb,
                                                        uint8_t x_ratio, uint8_t y_ratio, uint32_t bg)
{
    uint8_t xr = (x_ratio + 4) >> 3;
    uint8_t yr = (y_ratio + 4) >> 3;

    pixel00_rgb = ((pixel00_rgb | (pixel00_rgb << 16)) & 0x07e0f81f);
    pixel01_rgb = ((pixel01_rgb | (pixel01_rgb << 16)) & 0x07e0f81f);
    pixel10_rgb = ((pixel10_rgb | (pixel10_rgb << 16)) & 0x07e0f81f);
    pixel11_rgb = ((pixel11_rgb | (pixel11_rgb << 16)) & 0x07e0f81f);
    bg = ((bg | (bg << 16)) & 0x07e0f81f);

    uint32_t top_rgb = (((((pixel01_rgb - pixel00_rgb) * xr) >> 5) + pixel00_rgb) & 0x07e0f81f);
    uint32_t bottom_rgb = (((((pixel11_rgb - pixel10_rgb) * xr) >> 5) + pixel10_rgb) & 0x07e0f81f);
    uint32_t fg_rgb = (((((bottom_rgb - top_rgb) * yr) >> 5) + top_rgb) & 0x07e0f81f);

    uint32_t result = (((((fg_rgb - bg) * 31) >> 5) + bg) & 0x07e0f81f);
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
