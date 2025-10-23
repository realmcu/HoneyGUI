/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file acc_sw_transform.c
  * @author howie_wang@realsil.com.cn
  * @date 2023/12/23
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <draw_img.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

static uint16_t do_blending_acc_2_rgb565_opacity(uint32_t fg, uint32_t bg, uint8_t alpha)
{
    // Alpha converted from [0..255] to [0..31]
    // Converts  0000000000000000rrrrrggggggbbbbb
    //     into  00000gggggg00000rrrrr000000bbbbb
    // with mask 00000111111000001111100000011111
    // This is useful because it makes space for a parallel fixed-point multiply

    alpha = (alpha + 4) >> 3;
    bg = ((bg | (bg << 16)) & 0x07e0f81f);
    fg = ((fg | (fg << 16)) & 0x07e0f81f);
    uint32_t result = (((((fg - bg) * alpha) >> 5) + bg) & 0x07e0f81f);

    return (uint16_t)((result >> 16) | result);
}

#ifdef HONEYGUI_4XAA
static color_argb8565_t do_average_acc_2_argb8565_opacity(uint32_t fg, uint32_t bg,
                                                          uint8_t alpha_fg, uint8_t alpha_bg, uint8_t ratio)
{
    // Alpha converted from [0..255] to [0..31]
    // Converts  0000000000000000rrrrrggggggbbbbb
    //     into  00000gggggg00000rrrrr000000bbbbb
    // with mask 00000111111000001111100000011111
    // This is useful because it makes space for a parallel fixed-point multiply

    uint8_t rgb_ratio = (ratio + 4) >> 3;
    bg = ((bg | (bg << 16)) & 0x07e0f81f);
    fg = ((fg | (fg << 16)) & 0x07e0f81f);
    uint32_t result = (((((fg - bg) * rgb_ratio) >> 5) + bg) & 0x07e0f81f);
    color_argb8565_t pixel = {.rgb = (uint16_t)((result >> 16) | result), .a = ((alpha_fg * ratio + alpha_bg * (255 - ratio)) >> 8)};
    return pixel;
}
#endif

static uint16_t do_average_blend_acc_2_argb8565_opacity(uint32_t fg, uint32_t bg, uint32_t write,
                                                        uint8_t alpha_fg, uint8_t alpha_bg, uint8_t ratio)
{
    // Alpha converted from [0..255] to [0..31]
    // Converts  0000000000000000rrrrrggggggbbbbb
    //     into  00000gggggg00000rrrrr000000bbbbb
    // with mask 00000111111000001111100000011111
    // This is useful because it makes space for a parallel fixed-point multiply

    uint8_t rgb_ratio = (ratio + 4) >> 3;
    bg = ((bg | (bg << 16)) & 0x07e0f81f);
    fg = ((fg | (fg << 16)) & 0x07e0f81f);
    uint32_t result = (((((fg - bg) * rgb_ratio) >> 5) + bg) & 0x07e0f81f);
    uint8_t alpha = (alpha_fg * ratio + alpha_bg * (255 - ratio)) >> 8;
    alpha = (alpha + 4) >> 3;
    write = ((write | (write << 16)) & 0x07e0f81f);
    result = (((((result - write) * alpha) >> 5) + write) & 0x07e0f81f);
    return (uint16_t)((result >> 16) | result);
}

void sw_transform_for_rgb565(draw_img_t *image, gui_dispdev_t *dc,
                             gui_rect_t *rect)
{
    int32_t x_start = 0;
    int32_t x_end = 0;
    int32_t y_start = 0;
    int32_t y_end = 0;

    if (draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_base = sizeof(gui_rgb_data_head_t) + (uint32_t)(uintptr_t)(image->data);
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;
    uint16_t *readbuf = (uint16_t *)(uintptr_t)image_base;

    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    gui_matrix_t *inverse = &image->inverse;

    float m00 = inverse->m[0][0];
    float m01 = inverse->m[0][1];
    float m02 = inverse->m[0][2];
    float m10 = inverse->m[1][0];
    float m11 = inverse->m[1][1];
    float m12 = inverse->m[1][2];


    for (int32_t i = y_start; i <= y_end; i++)
    {
        float detalX = m01 * i + m02;
        float detalY = m11 * i + m12;
        float X = m00 * x_start + detalX;
        float Y = m10 * x_start + detalY;

        int write_offset = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) - dc->section.x1;

        for (int32_t j = x_start; j <= x_end; j++)
        {
            // int x = roundf(X);
            // int y = roundf(Y);

            int x = (int)X;
            int y = (int)Y;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                X += m00;
                Y += m10;
                continue;
            }

            int write_off = write_offset + j;
            int image_off = (image->blend_mode == IMG_RECT) ? 0 : y * source_w + x;

            writebuf[write_off] = readbuf[image_off];
            X += m00;
            Y += m10;
        }
    }
}


void sw_transform_for_argb8565(draw_img_t *image, gui_dispdev_t *dc,
                               gui_rect_t *rect)
{
    int32_t x_start = 0;
    int32_t x_end = 0;
    int32_t y_start = 0;
    int32_t y_end = 0;

    if (draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_base = sizeof(gui_rgb_data_head_t) + (uint32_t)(uintptr_t)(image->data);
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;

    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    gui_matrix_t *inverse = &image->inverse;

    float m00 = inverse->m[0][0];
    float m01 = inverse->m[0][1];
    float m02 = inverse->m[0][2];
    float m10 = inverse->m[1][0];
    float m11 = inverse->m[1][1];
    float m12 = inverse->m[1][2];

    float detalX = m01 * y_start + m02;
    float detalY = m11 * y_start + m12;
    for (int32_t i = y_start; i <= y_end; i++)
    {
        float X = m00 * x_start + detalX;
        float Y = m10 * x_start + detalY;

        int write_offset = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + dc->section.x1;

        for (int32_t j = x_start; j <= x_end; j++)
        {
            // int x = roundf(X);
            // int y = roundf(Y);

            int x = (int)X;
            int y = (int)Y;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                X += m00;
                Y += m10;
                continue;
            }

            int write_off = write_offset + j;
            int image_off = y * source_w + x;

            color_argb8565_t *pixel = (color_argb8565_t *)(uintptr_t)image_base + image_off;

            writebuf[write_off] = do_blending_acc_2_rgb565_opacity(pixel->color.rgb565, writebuf[write_off],
                                                                   pixel->a);

            X += m00;
            Y += m10;
        }
        detalX += m01;
        detalY += m11;
    }
}

static inline int fast_floor(float x)
{
    return (x >= 0) ? (int)x : (int)(x - 1);
}

void sw_transform_for_argb8565_aa(draw_img_t *image, gui_dispdev_t *dc,
                                  gui_rect_t *rect)
{
    int32_t x_start = 0;
    int32_t x_end = 0;
    int32_t y_start = 0;
    int32_t y_end = 0;

    if (draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_base = sizeof(gui_rgb_data_head_t) + (uint32_t)(uintptr_t)(image->data);
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;

    uint16_t source_w = image->img_w;
    uint16_t source_h = image->img_h;
    gui_matrix_t *inverse = &image->inverse;

    float m00 = inverse->m[0][0];
    float m01 = inverse->m[0][1];
    float m02 = inverse->m[0][2];
    float m10 = inverse->m[1][0];
    float m11 = inverse->m[1][1];
    float m12 = inverse->m[1][2];

    const float m00xx = m00 * x_start;
    const float m10xx = m10 * x_start;
    const int fb_width = (dc->section.x2 - dc->section.x1 + 1);
    const int offset_constant = dc->section.y1 * fb_width + dc->section.x1;
    float detalX = m01 * y_start + m02;
    float detalY = m11 * y_start + m12;
    for (int32_t i = y_start; i <= y_end; i++)
    {
        float X = m00xx + detalX;
        float Y = m10xx + detalY;

        int write_offset = i * fb_width - offset_constant;

        for (int32_t j = x_start; j <= x_end; j++)
        {
            // int x = roundf(X);
            // int y = roundf(Y);

            int x = fast_floor(X);
            int y = (int)Y;

            if ((uint32_t)x >= source_w || (uint32_t)y >= source_h)
            {
                X += m00;
                Y += m10;
                continue;
            }

            uint8_t xRatio = (1 + x - X) * 0xFF;
            color_argb8565_t pixel1 = {0}, pixel2 = {0};
#ifdef HONEYGUI_4XAA
            uint8_t yRatio = (1 + y - Y) * 0xFF;
            color_argb8565_t pixel3 = {0}, pixel4 = {0};
#endif
            int write_off = write_offset + j;
            int image_off = y * source_w + x;

            pixel1 = *((color_argb8565_t *)(uintptr_t)image_base + image_off);

#ifdef HONEYGUI_4XAA
            if (y >= -1)
            {
                pixel3 = *((color_argb8565_t *)(uintptr_t)image_base + image_off + source_w);
            }

            if (x < source_w - 1 && y >= 0)
#else
            if (x < source_w - 1)
#endif
            {
                pixel2 = *((color_argb8565_t *)(uintptr_t)image_base + image_off + 1);
            }
#ifdef HONEYGUI_4XAA
            if (y < source_h - 1)
            {
                pixel4 = *((color_argb8565_value_t *)(uintptr_t)image_base + image_off + source_w + 1);
            }
#endif
#ifdef HONEYGUI_4XAA
            pixel2 = do_average_acc_2_argb8565_opacity(pixel1.rgb, pixel2.rgb, pixel1.a, pixel2.a, xRatio);
            pixel4 = do_average_acc_2_argb8565_opacity(pixel3.rgb, pixel4.rgb, pixel3.a, pixel4.a, xRatio);
            writebuf[write_off] = do_average_blend_acc_2_argb8565_opacity(pixel2.rgb, pixel4.rgb,
                                                                          writebuf[write_off], pixel2.a, pixel4.a, yRatio);
#else
            writebuf[write_off] = do_average_blend_acc_2_argb8565_opacity(pixel1.color.rgb565,
                                                                          pixel2.color.rgb565,
                                                                          writebuf[write_off], pixel1.a, pixel2.a, xRatio);
#endif
            X += m00;
            Y += m10;
        }
        detalX += m01;
        detalY += m11;
    }
}

void sw_transform_for_alpha_aa(draw_img_t *image, gui_dispdev_t *dc,
                               gui_rect_t *rect)
{
    int32_t x_start = 0;
    int32_t x_end = 0;
    int32_t y_start = 0;
    int32_t y_end = 0;

    if (draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_base = sizeof(gui_rgb_data_head_t) + (uint32_t)(uintptr_t)(image->data);
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;

    uint16_t source_w = image->img_w;
    uint16_t source_h = image->img_h;
    gui_matrix_t *inverse = &image->inverse;

    float m00 = inverse->m[0][0];
    float m01 = inverse->m[0][1];
    float m02 = inverse->m[0][2];
    float m10 = inverse->m[1][0];
    float m11 = inverse->m[1][1];
    float m12 = inverse->m[1][2];

    uint32_t argb_value = image->fg_color_set;
    uint8_t b8 = (uint8_t)(argb_value & 0xFF);
    uint8_t g8 = (uint8_t)((argb_value >> 8) & 0xFF);
    uint8_t r8 = (uint8_t)((argb_value >> 16) & 0xFF);
    // Alpha (argb >> 24) not used in RGB565 conversion

    uint16_t r5 = (uint16_t)(r8 >> 3);
    uint16_t g6 = (uint16_t)(g8 >> 2);
    uint16_t b5 = (uint16_t)(b8 >> 3);

    uint16_t fg_color = (uint16_t)((r5 << 11) | (g6 << 5) | b5);
    const float m00xx = m00 * x_start;
    const float m10xx = m10 * x_start;
    const int fb_width = (dc->section.x2 - dc->section.x1 + 1);
    const int offset_constant = dc->section.y1 * fb_width + dc->section.x1;
    float detalX = m01 * y_start + m02;
    float detalY = m11 * y_start + m12;
    for (int32_t i = y_start; i <= y_end; i++)
    {
        float X = m00xx + detalX;
        float Y = m10xx + detalY;

        int write_offset = i * fb_width - offset_constant;

        for (int32_t j = x_start; j <= x_end; j++)
        {
            // int x = roundf(X);
            // int y = roundf(Y);

            int x = fast_floor(X);
            int y = (int)Y;

            if ((uint32_t)x >= source_w || (uint32_t)y >= source_h)
            {
                X += m00;
                Y += m10;
                continue;
            }
            uint8_t xRatio = (X - x) * 0xFF;
            uint8_t alpha1 = 0, alpha2 = 0;
#ifdef HONEYGUI_4XAA
            uint8_t yRatio = (Y - y) * 0xFF;
            uint8_t alpha3 = 0, alpha4 = 0;
#endif
            int write_off = write_offset + j;
            int image_off = y * source_w + x;

            alpha1 = *((uint8_t *)(uintptr_t)image_base + image_off);
#ifdef HONEYGUI_4XAA
            if (y >= -1)
            {
                alpha3 = *((uint8_t *)(uintptr_t)mage_base + image_off + source_w);
            }

            if (x < source_w - 1 && y >= 0)
#else
            if (x < source_w - 1)
#endif
            {
                alpha2 = *((uint8_t *)(uintptr_t)image_base + image_off + 1);
            }
#ifdef HONEYGUI_4XAA
            if (y < source_h - 1)
            {
                alpha4 = *((uint8_t *)(uintptr_t)image_base + image_off + source_w + 1);
            }
#endif
#ifdef HONEYGUI_4XAA
            alpha1 = ((alpha1 * (0xFF - xRatio) + alpha2 * xRatio) >> 8);
            alpha3 = ((alpha3 * (0xFF - xRatio) + alpha4 * xRatio) >> 8);
            alpha1 = ((alpha1 * (0xFF - yRatio) + alpha3 * yRatio) >> 8);
#else
            alpha1 = ((alpha1 * (0xFF - xRatio) + alpha2 * xRatio) >> 8);
#endif
            writebuf[write_off] = do_blending_acc_2_rgb565_opacity(fg_color, writebuf[write_off], alpha1);
            X += m00;
            Y += m10;
        }
        detalX += m01;
        detalY += m11;
    }
}

void sw_transform_for_alpha(draw_img_t *image, gui_dispdev_t *dc,
                            gui_rect_t *rect)
{
    int32_t x_start = 0;
    int32_t x_end = 0;
    int32_t y_start = 0;
    int32_t y_end = 0;

    if (draw_img_target_area(image, dc, rect, &x_start, &x_end, &y_start, &y_end) == false)
    {
        return;
    }

    uint32_t image_base = sizeof(gui_rgb_data_head_t) + (uint32_t)(uintptr_t)(image->data);
    uint16_t *writebuf = (uint16_t *)dc->frame_buf;

    int16_t source_w = image->img_w;
    int16_t source_h = image->img_h;
    gui_matrix_t *inverse = &image->inverse;

    float m00 = inverse->m[0][0];
    float m01 = inverse->m[0][1];
    float m02 = inverse->m[0][2];
    float m10 = inverse->m[1][0];
    float m11 = inverse->m[1][1];
    float m12 = inverse->m[1][2];

    uint32_t argb_value = image->fg_color_set;
    uint8_t b8 = (uint8_t)(argb_value & 0xFF);
    uint8_t g8 = (uint8_t)((argb_value >> 8) & 0xFF);
    uint8_t r8 = (uint8_t)((argb_value >> 16) & 0xFF);
    // Alpha (argb >> 24) not used in RGB565 conversion

    uint16_t r5 = (uint16_t)(r8 >> 3);
    uint16_t g6 = (uint16_t)(g8 >> 2);
    uint16_t b5 = (uint16_t)(b8 >> 3);

    uint16_t fg_color = (uint16_t)((r5 << 11) | (g6 << 5) | b5);
    for (int32_t i = y_start; i <= y_end; i++)
    {
        float detalX = m01 * i + m02;
        float detalY = m11 * i + m12;
        float X = m00 * x_start + detalX;
        float Y = m10 * x_start + detalY;

        int write_offset = (i - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) - dc->section.x1;

        for (int32_t j = x_start; j <= x_end; j++)
        {
            // int x = roundf(X);
            // int y = roundf(Y);

            int x = (int)X;
            int y = (int)Y;

            if ((x >= source_w) || (x < 0) || (y < 0) || (y >= source_h))
            {
                X += m00;
                Y += m10;
                continue;
            }

            int write_off = write_offset + j;
            int image_off = y * source_w + x;

            uint8_t alpha = *((uint8_t *)(uintptr_t)image_base + image_off);

            writebuf[write_off] = do_blending_acc_2_rgb565_opacity(fg_color, writebuf[write_off], alpha);

            X += m00;
            Y += m10;
        }
    }
}
