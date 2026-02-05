/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "lite_geometry.h"
PixelColor RGB565_color(uint8_t r, uint8_t g, uint8_t b)
{

    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);

}
PixelColor RGBA_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{

    return (a << 24) | (r << 16) | (g << 8) | b;

}
PixelColor create_color_by_format(PixelFormat format, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    switch (format)
    {
    case PIXEL_FORMAT_RGB565:
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    case PIXEL_FORMAT_ARGB8888:
    default:
        return (a << 24) | (r << 16) | (g << 8) | b;
    }
}

static inline int get_pixel_size(PixelFormat format)
{
    switch (format)
    {
    case PIXEL_FORMAT_RGB565: return 2;
    case PIXEL_FORMAT_ARGB8888: return 4;
    default: return 4;
    }
}

// ==================== Pixel drawing function ====================

void add_pixel_to_context(DrawContext *ctx, int x, int y, PixelColor color)
{
    add_pixel_with_coverage(ctx, x, y, color, 1.0f);
}

void add_pixel_aa(DrawContext *ctx, int x, int y, PixelColor color, float coverage)
{
    add_pixel_with_coverage(ctx, x, y, color, coverage);
}
static inline PixelColor get_pixel_color_rgb565(DrawContext *ctx, int byte_offset)
{
    return *(uint16_t *)(ctx->buffer + byte_offset);
}

static inline PixelColor get_pixel_color_argb8888(DrawContext *ctx, int byte_offset)
{
    return *(uint32_t *)(ctx->buffer + byte_offset);
}
static inline void write_pixel_at_offset(DrawContext *ctx, int byte_offset, PixelColor color)
{
    switch (ctx->format)
    {
    case PIXEL_FORMAT_RGB565:
        *(uint16_t *)(ctx->buffer + byte_offset) = (uint16_t)color;
        break;
    case PIXEL_FORMAT_ARGB8888:
        *(uint32_t *)(ctx->buffer + byte_offset) = color;
        break;
    default:
        memcpy(ctx->buffer + byte_offset, &color, get_pixel_size(ctx->format));
        break;
    }
}
static inline void write_pixel_rgb565(DrawContext *ctx, int byte_offset, PixelColor color)
{
    *(uint16_t *)(ctx->buffer + byte_offset) = (uint16_t)color;
}

static inline void write_pixel_argb8888(DrawContext *ctx, int byte_offset, PixelColor color)
{
    *(uint32_t *)(ctx->buffer + byte_offset) = color;
}

static void add_pixel_with_coverage(DrawContext *ctx, int x, int y, PixelColor color,
                                    float coverage)
{
//    if (x < 0 || x >= ctx->width || y < 0 || y >= ctx->height || coverage < 0.01f)
//    {
//        return;
//    }

    int pixel_size = get_pixel_size(ctx->format);
    int byte_offset = (y * ctx->width + x) * pixel_size;

    switch (ctx->format)
    {
    case PIXEL_FORMAT_RGB565:
        {
            if (coverage > 0.999f)
            {
                write_pixel_rgb565(ctx, byte_offset, color);
            }
            else
            {
                uint8_t alpha = (uint8_t)(coverage * 255);
                PixelColor bg_color = get_pixel_color_rgb565(ctx, byte_offset);
                PixelColor blended_color = blend_colors_rgb565(bg_color, color, alpha);
                write_pixel_rgb565(ctx, byte_offset, blended_color);
            }
            break;
        }

    case PIXEL_FORMAT_ARGB8888:
        {
            uint8_t original_alpha = (color >> 24) & 0xFF;
            if (original_alpha == 0) { return; }

            uint8_t final_alpha = (uint8_t)(original_alpha * coverage);
            if (final_alpha == 0) { return; }

            if (final_alpha == 255)
            {
                write_pixel_argb8888(ctx, byte_offset, color);
            }
            else
            {
                PixelColor bg_color = get_pixel_color_argb8888(ctx, byte_offset);
                PixelColor blended_color = blend_colors_argb8888(bg_color, color, final_alpha);
                write_pixel_argb8888(ctx, byte_offset, blended_color);
            }
            break;
        }

    default:
        write_pixel_at_offset(ctx, byte_offset, color);
        break;
    }
}
void add_pixel_with_coverage_rgb565(DrawContext *ctx, int x, int y, PixelColor color,
                                    float coverage)
{
    int pixel_size = get_pixel_size(ctx->format); // 2 Byte
    int byte_offset = (y * ctx->width + x) * pixel_size;

    if (coverage > 0.999f)
    {
        write_pixel_rgb565(ctx, byte_offset, color);
    }
    else
    {
        uint8_t alpha = (uint8_t)(coverage * 255);
        PixelColor bg_color = get_pixel_color_rgb565(ctx, byte_offset);
        PixelColor blended_color = blend_colors_rgb565(bg_color, color, alpha);
        write_pixel_rgb565(ctx, byte_offset, blended_color);
    }
}
void add_pixel_with_coverage_argb8888(DrawContext *ctx, int x, int y, PixelColor color,
                                      float coverage)
{
    int pixel_size = get_pixel_size(ctx->format); // 4 Byte
    int byte_offset = (y * ctx->width + x) * pixel_size;

    uint8_t original_alpha = (color >> 24) & 0xFF;
    if (original_alpha == 0) { return; }

    uint8_t final_alpha = (uint8_t)(original_alpha * coverage);
    if (final_alpha == 0) { return; }

    if (final_alpha == 255)
    {
        write_pixel_argb8888(ctx, byte_offset, color);
    }
    else
    {
        PixelColor bg_color = get_pixel_color_argb8888(ctx, byte_offset);
        PixelColor blended_color = blend_colors_argb8888(bg_color, color, final_alpha);
        write_pixel_argb8888(ctx, byte_offset, blended_color);
    }
}

PixelColor blend_colors_rgb565(PixelColor bg_color, PixelColor fg_color,
                               uint8_t alpha)
{

    /* Source: https://stackoverflow.com/a/50012418/1999969*/
    if (alpha == 0) { return bg_color; }
    if (alpha == 255) { return fg_color; }
    // Converts  0000000000000000rrrrrggggggbbbbb
    //     into  00000gggggg00000rrrrr000000bbbbb
    // with mask 00000111111000001111100000011111
    alpha = (alpha + 4) >> 3;
    bg_color = (bg_color | (bg_color << 16)) & 0x07E0F81F;
    fg_color = (fg_color | (fg_color << 16)) & 0x07E0F81F;
    uint32_t result = ((((fg_color - bg_color) * alpha) >> 5) + bg_color) & 0x07E0F81F;
    return (uint16_t)((result >> 16) | result);
}

PixelColor blend_colors_argb8888(PixelColor bg_color, PixelColor fg_color, uint8_t alpha)
{
    if (alpha == 0) { return bg_color; }
    if (alpha == 255) { return fg_color; }

    uint8_t bg_a = (bg_color >> 24) & 0xFF;

    // CRITICAL FIX: When blending onto transparent background, output straight alpha
    // to avoid black edges. Do NOT premultiply RGB components!
    if (bg_a == 0)
    {
        uint8_t r = (fg_color >> 16) & 0xFF;
        uint8_t g = (fg_color >> 8) & 0xFF;
        uint8_t b = fg_color & 0xFF;
        // Keep RGB unchanged, only set alpha channel
        return (alpha << 24) | (r << 16) | (g << 8) | b;
    }

    uint32_t inv_alpha = 255 - alpha;

    uint32_t bg_r = (bg_color >> 16) & 0xFF;
    uint32_t bg_g = (bg_color >> 8) & 0xFF;
    uint32_t bg_b = bg_color & 0xFF;

    uint32_t fg_r = (fg_color >> 16) & 0xFF;
    uint32_t fg_g = (fg_color >> 8) & 0xFF;
    uint32_t fg_b = fg_color & 0xFF;

    // ((bg_r * inv_alpha + fg_r * alpha) * 257) >> 16 equivalent to ((bg_r * inv_alpha + fg_r * alpha) + 127) / 255
    uint32_t out_r = ((bg_r * inv_alpha + fg_r * alpha) * 257) >> 16;
    uint32_t out_g = ((bg_g * inv_alpha + fg_g * alpha) * 257) >> 16;
    uint32_t out_b = ((bg_b * inv_alpha + fg_b * alpha) * 257) >> 16;

    uint32_t out_a = bg_a + (((255 - bg_a) * alpha * 257) >> 16);

    return (out_a << 24) | (out_r << 16) | (out_g << 8) | out_b;
}

// static inline PixelColor get_pixel_color(DrawContext *ctx, int x, int y)
// {
//     if (x < 0 || x >= ctx->width || y < 0 || y >= ctx->height)
//     {
//         return 0;
//     }

//     int pixel_size = get_pixel_size(ctx->format);
//     int byte_offset = (y * ctx->width + x) * pixel_size;
//     PixelColor color = 0;

//     switch (ctx->format)
//     {
//     case PIXEL_FORMAT_RGB565:
//         {
//             uint16_t color_16bit;
//             memcpy(&color_16bit, ctx->buffer + byte_offset, 2);
//             color = color_16bit;
//         }
//         break;
//     case PIXEL_FORMAT_ARGB8888:
//         memcpy(&color, ctx->buffer + byte_offset, 4);
//         break;
//     default:
//         memcpy(&color, ctx->buffer + byte_offset, pixel_size);
//         break;
//     }
//     return color;
// }

static void add_pixels_hline(DrawContext *ctx, int start_x, int end_x, int y, PixelColor color)
{
    if (y < 0 || y >= ctx->height) { return; }

    start_x = (start_x < 0) ? 0 : start_x;
    end_x = (end_x >= ctx->width) ? ctx->width - 1 : end_x;

    int pixel_size = get_pixel_size(ctx->format);
    uint8_t alpha = (ctx->format == PIXEL_FORMAT_RGB565) ? 255 : ((color >> 24) & 0xFF);

    bool is_opaque = (alpha == 255);

    for (int x = start_x; x <= end_x; x++)
    {
        int byte_offset = (y * ctx->width + x) * pixel_size;
        PixelColor final_color;

        if (is_opaque)
        {
            final_color = color;
        }
        else
        {
            PixelColor bg_color;

            if (ctx->format == PIXEL_FORMAT_RGB565)
            {
                uint16_t *pixel_ptr = (uint16_t *)(ctx->buffer + byte_offset);
                bg_color = *pixel_ptr;
                final_color = blend_colors_rgb565(bg_color, color, alpha);
            }
            else if (ctx->format == PIXEL_FORMAT_ARGB8888)
            {
                uint32_t *pixel_ptr = (uint32_t *)(ctx->buffer + byte_offset);
                bg_color = *pixel_ptr;
                final_color = blend_colors_argb8888(bg_color, color, alpha);
            }
        }

        // wirte pixel
        if (ctx->format == PIXEL_FORMAT_RGB565)
        {
            uint16_t *pixel_ptr = (uint16_t *)(ctx->buffer + byte_offset);
            *pixel_ptr = (uint16_t)final_color;
        }
        else if (ctx->format == PIXEL_FORMAT_ARGB8888)
        {
            uint32_t *pixel_ptr = (uint32_t *)(ctx->buffer + byte_offset);
            *pixel_ptr = final_color;
        }
        else
        {
            memcpy(ctx->buffer + byte_offset, &final_color, pixel_size);
        }

    }
}

void init_draw_context(DrawContext *ctx, uint8_t *buffer, int width, int height, PixelFormat format)
{
    ctx->buffer = buffer;
    ctx->width = width;
    ctx->height = height;
    ctx->format = format;
    ctx->stride = width * get_pixel_size(format);
}

static void fill_rect_blend_argb8888(DrawContext *ctx, int start_x, int start_y,
                                     int width, int height, PixelColor fill_color)
{
    uint32_t *dest_buf = (uint32_t *)ctx->buffer;
    int dest_stride = ctx->width;
    uint32_t color32 = fill_color;
// gui_log("[fill_rect_blend_argb8888]start_x %d ,start_y %d,width %d,height %d \n",start_x,start_y,width,height);
    uint8_t alpha = (fill_color >> 24) & 0xFF;
    uint32_t *dest_start = &dest_buf[start_y * dest_stride + start_x];
    if (alpha == 255)
    {
        for (int y = 0; y < height; y++)
        {

            uint32_t *dest_row = dest_start;
            // 8 pixels per loop unrolling
            int x = 0;
            for (; x <= width - 8; x += 8)
            {
                dest_row[x + 0] = color32;
                dest_row[x + 1] = color32;
                dest_row[x + 2] = color32;
                dest_row[x + 3] = color32;
                dest_row[x + 4] = color32;
                dest_row[x + 5] = color32;
                dest_row[x + 6] = color32;
                dest_row[x + 7] = color32;
            }
            // remaining pixels
            for (; x < width; x++)
            {
                dest_row[x] = color32;
            }
            dest_start += dest_stride;
        }
    }
    else
    {
        // Alpha blending path
        for (int y = 0; y < height; y++)
        {
            uint32_t *dest_row = dest_start;

            for (int x = 0; x < width; x++)
            {
                dest_row[x] = blend_colors_argb8888(dest_row[x], fill_color, alpha);
            }
            dest_start += dest_stride;
        }
    }
}

static void fill_rect_blend_rgb565(DrawContext *ctx, int start_x, int start_y,
                                   int width, int height, PixelColor fill_color)
{
    uint16_t *dest_buf = (uint16_t *)ctx->buffer;
    int dest_stride = ctx->width;
    uint16_t color16 = (uint16_t)fill_color;
    uint16_t *dest_start = &dest_buf[start_y * dest_stride + start_x];
    for (int y = 0; y < height; y++)
    {

        uint16_t *dest_row = dest_start;
        // loop unrolling for performance
        int x = 0;
        for (; x <= width - 16; x += 16)
        {
            dest_row[x + 0] = color16;
            dest_row[x + 1] = color16;
            dest_row[x + 2] = color16;
            dest_row[x + 3] = color16;
            dest_row[x + 4] = color16;
            dest_row[x + 5] = color16;
            dest_row[x + 6] = color16;
            dest_row[x + 7] = color16;
            dest_row[x + 8] = color16;
            dest_row[x + 9] = color16;
            dest_row[x + 10] = color16;
            dest_row[x + 11] = color16;
            dest_row[x + 12] = color16;
            dest_row[x + 13] = color16;
            dest_row[x + 14] = color16;
            dest_row[x + 15] = color16;
        }
        // remaining pixels
        for (; x < width; x++)
        {
            dest_row[x] = color16;
        }
        dest_start += dest_stride;
    }
}

static void fill_rect_blend_other(DrawContext *ctx, int start_x, int start_y,
                                  int width, int height, PixelColor fill_color)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            add_pixel_to_context(ctx, start_x + x, start_y + y, fill_color);
        }
    }
}

// ==================== Basic rectangle fill function ====================
void fill_rect(DrawContext *ctx, const Rect *rect, PixelColor fill_color)
{
    int start_x = rect->x;
    int end_x = (rect->x + rect->w);
    int start_y = rect->y;
    int end_y = rect->y + rect->h;

    start_x = LG_MAX(start_x, 0);
    end_x = LG_MIN(end_x, ctx->width - 1);
    start_y = LG_MAX(start_y, 0);
    end_y = LG_MIN(end_y, ctx->height - 1);

    int width = end_x - start_x + 1;
    int height = end_y - start_y + 1;

    if (width <= 0 || height <= 0) { return; }

    switch (ctx->format)
    {
    case PIXEL_FORMAT_ARGB8888:
        fill_rect_blend_argb8888(ctx, start_x, start_y, width, height, fill_color);
        break;
    case PIXEL_FORMAT_RGB565:
        fill_rect_blend_rgb565(ctx, start_x, start_y, width, height, fill_color);
        break;
    default:
        fill_rect_blend_other(ctx, start_x, start_y, width, height, fill_color);
        break;
    }
}

static void fill_rounded_corner(DrawContext *ctx,
                                int start_x, int start_y, int end_x, int end_y,
                                float center_x, float center_y, float radius,
                                PixelColor fill_color)
{
    float radius_sq = radius * radius;

    for (int py = start_y; py <= end_y; py++)
    {
        for (int px = start_x; px <= end_x; px++)
        {
            float dx = (px + 0.5f) - center_x;
            float dy = (py + 0.5f) - center_y;
            if (dx * dx + dy * dy <= radius_sq)
            {
                add_pixel_to_context(ctx, px, py, fill_color);
            }
        }
    }
}
void fill_rounded_rect_no_aa(DrawContext *ctx,
                             float x, float y, float width, float height,
                             float radius, PixelColor fill_color)
{
    if (width <= 0 || height <= 0) { return; }

    float max_radius = (width < height) ? width / 2.0f : height / 2.0f;
    if (radius > max_radius) { radius = max_radius; }
    if (radius < 0) { radius = 0; }

    //Quick path: No rounded corners
    if (radius < 0.5f)
    {
        fill_rect(ctx, &(Rect) {x, y, width, height}, fill_color);
        return;
    }

    int start_x = (int)floor(x);
    int end_x = (int)ceil(x + width);
    int start_y = (int)floor(y);
    int end_y = (int)ceil(y + height);

    start_x = LG_MAX(start_x, 0);
    end_x = LG_MIN(end_x, ctx->width - 1);
    start_y = LG_MAX(start_y, 0);
    end_y = LG_MIN(end_y, ctx->height - 1);

    // Calculate corner dimensions
    int corner_size = (int)ceil(radius);
    int inner_start_x = start_x + corner_size;
    int inner_end_x = end_x - corner_size;
    int inner_start_y = start_y + corner_size;
    int inner_end_y = end_y - corner_size;

    // Ensure the inner rectangle is valid
    if (inner_start_x > inner_end_x) { inner_start_x = inner_end_x = (start_x + end_x) / 2; }
    if (inner_start_y > inner_end_y) { inner_start_y = inner_end_y = (start_y + end_y) / 2; }

    int vertical_area = 0;    // Vertical merge: left, center, right --- a rectangle
    int horizontal_area = 0;  // Horizontal merging: Top, middle, bottom --- a rectangle

    if (inner_start_y <= inner_end_y)
    {
        vertical_area = (end_x - start_x + 1) * (inner_end_y - inner_start_y + 1);
    }

    if (inner_start_x <= inner_end_x)
    {
        horizontal_area = (inner_end_x - inner_start_x + 1) * (end_y - start_y + 1);
    }

    if (vertical_area >= horizontal_area && vertical_area > 0)
    {
        Rect vertical_rect =
        {
            (float)start_x,
            (float)inner_start_y,
            (float)(end_x - start_x + 1),
            (float)(inner_end_y - inner_start_y + 1)
        };
        fill_rect(ctx, &vertical_rect, fill_color);

        if (start_y < inner_start_y)
        {
            Rect top_strip =
            {
                (float)inner_start_x,
                (float)start_y,
                (float)(inner_end_x - inner_start_x + 1),
                (float)(inner_start_y - start_y)
            };
            fill_rect(ctx, &top_strip, fill_color);
        }

        if (inner_end_y < end_y)
        {
            Rect bottom_strip =
            {
                (float)inner_start_x,
                (float)(inner_end_y + 1),
                (float)(inner_end_x - inner_start_x + 1),
                (float)(end_y - inner_end_y)
            };
            fill_rect(ctx, &bottom_strip, fill_color);
        }
    }
    else if (horizontal_area > 0)
    {
        Rect horizontal_rect =
        {
            (float)inner_start_x,
            (float)start_y,
            (float)(inner_end_x - inner_start_x + 1),
            (float)(end_y - start_y + 1)
        };
        fill_rect(ctx, &horizontal_rect, fill_color);

        if (start_x < inner_start_x)
        {
            Rect left_strip =
            {
                (float)start_x,
                (float)inner_start_y,
                (float)(inner_start_x - start_x),
                (float)(inner_end_y - inner_start_y + 1)
            };
            fill_rect(ctx, &left_strip, fill_color);
        }

        if (inner_end_x < end_x)
        {
            Rect right_strip =
            {
                (float)(inner_end_x + 1),
                (float)inner_start_y,
                (float)(end_x - inner_end_x),
                (float)(inner_end_y - inner_start_y + 1)
            };
            fill_rect(ctx, &right_strip, fill_color);
        }
    }

    // Four corner areas
    fill_rounded_corner(ctx, start_x, start_y, inner_start_x - 1, inner_start_y - 1,
                        x + radius, y + radius, radius, fill_color);
    fill_rounded_corner(ctx, inner_end_x + 1, start_y, end_x, inner_start_y - 1,
                        x + width - radius, y + radius, radius, fill_color);
    fill_rounded_corner(ctx, inner_end_x + 1, inner_end_y + 1, end_x, end_y,
                        x + width - radius, y + height - radius, radius, fill_color);
    fill_rounded_corner(ctx, start_x, inner_end_y + 1, inner_start_x - 1, end_y,
                        x + radius, y + height - radius, radius, fill_color);
}
void fill_rounded_rect(DrawContext *ctx,
                       float x, float y, float width, float height,
                       float radius, PixelColor fill_color)
{
    if (ctx->enable_aa)
    {
        fill_rounded_rect_df_aa(ctx, x, y, width, height, radius, fill_color);
    }
    else
    {
        fill_rounded_rect_no_aa(ctx, x, y, width, height, radius, fill_color);
    }
}

void fill_circle(DrawContext *ctx, float center_x, float center_y, float radius,
                 PixelColor fill_color)
{
    if (ctx->enable_aa)
    {
        fill_circle_df_aa(ctx, center_x, center_y, radius, fill_color);
    }
    else
    {
        fill_circle_no_aa(ctx, center_x, center_y, radius, fill_color);
    }
}
// ==================== Anti-aliased drawing functions ====================

static void fill_single_corner_aa(DrawContext *ctx,
                                  int corner_idx,
                                  int start_x, int start_y, int end_x, int end_y,
                                  float corner_x, float corner_y, float radius,
                                  PixelColor fill_color)
{
    float aa_range = 1.0f;
    for (int py = start_y; py <= end_y; py++)
    {
        float sample_y = py + 0.5f;
        for (int px = start_x; px <= end_x; px++)
        {
            float sample_x = px + 0.5f;
            float dx = sample_x - corner_x;
            float dy = sample_y - corner_y;

            float dist;
            if ((corner_idx == 0 && dx <= 0 && dy <= 0) ||
                (corner_idx == 1 && dx >= 0 && dy <= 0) ||
                (corner_idx == 2 && dx >= 0 && dy >= 0) ||
                (corner_idx == 3 && dx <= 0 && dy >= 0))
            {
                dist = sqrtf(dx * dx + dy * dy) - radius;
            }
            else if ((corner_idx == 0 && dy > 0) ||
                     (corner_idx == 3 && dy < 0))
            {
                dist = dx;
            }
            else if ((corner_idx == 1 && dx < 0) ||
                     (corner_idx == 2 && dx > 0))
            {
                dist = dy;
            }
            else
            {
                continue;
            }

            float coverage = 1.0f - smoothstep(-aa_range, aa_range, dist);

            if (coverage > 0.001f)
            {
                add_pixel_aa(ctx, px, py, fill_color, coverage);
            }
        }
    }
}

void fill_rounded_rect_df_aa(DrawContext *ctx,
                             int x, int y, int width, int height,
                             int radius, PixelColor fill_color)
{
    if (width <= 0 || height <= 0) { return; }

    int max_radius = (width < height) ? width / 2 : height / 2;
    if (radius > max_radius) { radius = max_radius; }
    if (radius < 0) { radius = 0; }

    // Fast path: No rounded corners or too small radius
    if (radius < 1)
    {
        fill_rect(ctx, &(Rect) {x, y, width, height}, fill_color);
        return;
    }

    int left = x;
    int right = x + width;
    int top = y;
    int bottom = y + height;

    // Inner rectangular boundary (area without rounded corners)
    int inner_left = left + radius;
    int inner_right = right - radius;
    int inner_top = top + radius;
    int inner_bottom = bottom - radius;

    int start_x = LG_MAX(left, 0);
    int end_x = LG_MIN(right - 1, ctx->width - 1);
    int start_y = LG_MAX(top, 0);
    int end_y = LG_MIN(bottom - 1, ctx->height - 1);

    int inner_px_start = LG_MAX(inner_left, start_x);
    int inner_px_end = LG_MIN(inner_right - 1, end_x);
    int inner_py_start = LG_MAX(inner_top, start_y);
    int inner_py_end = LG_MIN(inner_bottom - 1, end_y);

    float corner_centers[4][2] =
    {
        {left + radius, top + radius},
        {right - radius, top + radius},
        {right - radius, bottom - radius},
        {left + radius, bottom - radius}
    };

    int vertical_area = 0;
    int horizontal_area = 0;

    if (inner_py_start <= inner_py_end)
    {
        vertical_area = (end_x - start_x + 1) * (inner_py_end - inner_py_start + 1);
    }

    if (inner_px_start <= inner_px_end)
    {
        horizontal_area = (inner_px_end - inner_px_start + 1) * (end_y - start_y + 1);
    }

    if (vertical_area >= horizontal_area && vertical_area > 0)
    {
        Rect vertical_rect =
        {
            start_x,
            inner_py_start,
            (end_x - start_x),
            (inner_py_end - inner_py_start)
        };
        fill_rect(ctx, &vertical_rect, fill_color);

        if (start_y < inner_py_start)
        {
            Rect top_strip =
            {
                inner_px_start,
                start_y,
                (inner_px_end - inner_px_start),
                (inner_py_start - start_y - 1)
            };
            fill_rect(ctx, &top_strip, fill_color);
        }

        if (inner_py_end < end_y)
        {
            Rect bottom_strip =
            {
                inner_px_start,
                (inner_py_end + 1),
                (inner_px_end - inner_px_start),
                (end_y - inner_py_end - 1)
            };
            fill_rect(ctx, &bottom_strip, fill_color);
        }
    }
    else if (horizontal_area > 0)
    {
        Rect horizontal_rect =
        {
            inner_px_start,
            start_y,
            (inner_px_end - inner_px_start),
            (end_y - start_y)
        };
        fill_rect(ctx, &horizontal_rect, fill_color);

        if (start_x < inner_px_start)
        {
            Rect left_strip =
            {
                start_x,
                inner_py_start,
                (inner_px_start - start_x - 1),
                (inner_py_end - inner_py_start)
            };
            fill_rect(ctx, &left_strip, fill_color);
        }

        if (inner_px_end < end_x)
        {
            Rect right_strip =
            {
                (inner_px_end + 1),
                inner_py_start,
                (end_x - inner_px_end - 1),
                (inner_py_end - inner_py_start)
            };
            fill_rect(ctx, &right_strip, fill_color);
        }
    }

    fill_single_corner_aa(ctx, 0,
                          start_x, start_y,
                          inner_px_start - 1, inner_py_start - 1,
                          corner_centers[0][0], corner_centers[0][1],
                          radius, fill_color);

    fill_single_corner_aa(ctx, 1,
                          inner_px_end + 1, start_y,
                          end_x, inner_py_start - 1,
                          corner_centers[1][0], corner_centers[1][1],
                          radius, fill_color);

    fill_single_corner_aa(ctx, 2,
                          inner_px_end + 1, inner_py_end + 1,
                          end_x, end_y,
                          corner_centers[2][0], corner_centers[2][1],
                          radius, fill_color);

    fill_single_corner_aa(ctx, 3,
                          start_x, inner_py_end + 1,
                          inner_px_start - 1, end_y,
                          corner_centers[3][0], corner_centers[3][1],
                          radius, fill_color);
}

void fill_circle_df_aa(DrawContext *ctx, int center_x, int center_y, int radius,
                       PixelColor fill_color)
{
    if (radius > 50)
    {
        fill_circle_no_aa(ctx, center_x, center_y, radius, fill_color);
        return;
    }
    if (radius <= 0) { return; }

    // Use same AA width as arc drawing for consistent appearance
    float aa_width = 2.0f;

    int inner_radius = radius * M_SQRT1_2 - 2;
    int inner_size = inner_radius * 2;

    Rect inner_rect =
    {
        center_x - inner_radius,
        center_y - inner_radius,
        inner_size,
        inner_size
    };

    if (inner_rect.w > 0 && inner_rect.h > 0)
    {
        fill_rect(ctx, &inner_rect, fill_color);
    }

    // Anti-aliasing is applied only to the edge areas.
    float inner_radius_sq = (radius - aa_width) * (radius - aa_width);
    float outer_radius_sq = (radius + aa_width) * (radius + aa_width);

    // Calculate the boundary region to be processed (excluding inscribed rectangles).
    int start_x = (int)(center_x - radius - 2);
    int end_x = (int)(center_x + radius + 2);
    int start_y = (int)(center_y - radius - 2);
    int end_y = (int)(center_y + radius + 2);

    start_x = LG_MAX(LG_MAX(start_x, 0), ctx->clip_rect.x);
    end_x = LG_MIN(LG_MIN(end_x, ctx->width - 1), ctx->clip_rect.x + ctx->clip_rect.w - 1);
    start_y = LG_MAX(LG_MAX(start_y, 0), ctx->clip_rect.y);
    end_y = LG_MIN(LG_MIN(end_y, ctx->height - 1), ctx->clip_rect.y + ctx->clip_rect.h - 1);

    if (start_x > end_x || start_y > end_y)
    {
        return;
    }

    // Boundary of the inscribed rectangle
    int inner_start_x = (center_x - inner_radius);
    int inner_end_x = (center_x + inner_radius);
    int inner_start_y = (center_y - inner_radius);
    int inner_end_y = (center_y + inner_radius);

    for (int py = start_y; py <= end_y; py++)
    {
        for (int px = start_x; px <= end_x; px++)
        {
            // Skip the inscribed rectangle area (which is already filled).
            if (px >= inner_start_x && px <= inner_end_x &&
                py >= inner_start_y && py <= inner_end_y)
            {
                continue;
            }

            float dx = (px + 0.5f) - center_x;
            float dy = (py + 0.5f) - center_y;
            float dist_sq = dx * dx + dy * dy;

            float coverage;

            if (dist_sq <= inner_radius_sq)
            {
                coverage = 1.0f;  // completely internal
            }
            else if (dist_sq >= outer_radius_sq)
            {
                coverage = 0.0f;  // completely external
            }
            else
            {
                float dist = sqrtf(dist_sq);
                coverage = 1.0f - (dist - (radius - aa_width)) / (aa_width * 2.0f);
                coverage = fmaxf(0.0f, fminf(1.0f, coverage));
            }

            if (coverage > 0.001f)
            {
                add_pixel_aa(ctx, px, py, fill_color, coverage);
            }
        }
    }
}
void fill_circle_no_aa(DrawContext *ctx, int center_x, int center_y, int radius, PixelColor color)
{
    if (radius <= 0) { return; }

    int cx = center_x;
    int cy = center_y;
    int r = radius;

    int inner_size = (int)(radius * M_SQRT1_2);
    if (inner_size > 0)
    {
        Rect inner_rect =
        {
            cx - inner_size,
            cy - inner_size,
            inner_size * 2,
            inner_size * 2
        };
        fill_rect(ctx, &inner_rect, color);
    }

    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        if (x > inner_size)
        {
            int start_x = cx + inner_size + 1;
            int end_x = cx + x;
            if (start_x < end_x)
            {
                add_pixels_hline(ctx, start_x, end_x, cy + y, color);
                add_pixels_hline(ctx, start_x, end_x, cy - y - 1, color);
            }
        }

        if (x > inner_size)
        {
            int start_x = cx - x;
            int end_x = cx - inner_size - 1;
            if (start_x <= end_x)
            {
                add_pixels_hline(ctx, start_x, end_x, cy + y, color);
                add_pixels_hline(ctx, start_x, end_x, cy - y - 1, color);
            }
        }

        y++;
        err += 1 + 2 * y;
        if (2 * (err - x) + 1 > 0)
        {
            x--;
            err += 1 - 2 * x;
        }
    }
    // int top_edge = cy - r;
    int top_inner = cy - inner_size;
    int bottom_inner = cy + inner_size;
    // int bottom_edge = cy + r;

    for (int offset = 0; offset <= (r - inner_size); offset++)
    {
        int current_y_top = top_inner - offset - 1;
        int current_y_bottom = bottom_inner + offset + 1;

        int dy = offset + inner_size;
        int x_radius = (int)lg_sqrt32(r * r - dy * dy);

        int start_x = cx - x_radius;
        int end_x = cx + x_radius;

        add_pixels_hline(ctx, start_x, end_x, current_y_top, color);
        add_pixels_hline(ctx, start_x, end_x, current_y_bottom, color);
    }
}

// ==================== Arc Helper Functions ====================

// Compute coverage for anti-aliasing using squared distances (avoids sqrt)
// Uses smoothstep-like falloff for consistency with arc_sdf
static inline float compute_coverage_fast(float dist_sq, float outer_sq, float inner_sq,
                                          float outer_aa_sq, float inner_aa_sq,
                                          float outer_solid_sq, float inner_solid_sq)
{
    (void)outer_sq;
    // Solid region - full coverage
    if (dist_sq <= outer_solid_sq && dist_sq >= inner_solid_sq)
    {
        return 1.0f;
    }

    // Outer edge anti-aliasing - use smoothstep for consistency
    if (dist_sq > outer_solid_sq && dist_sq <= outer_aa_sq)
    {
        float t = (dist_sq - outer_solid_sq) / (outer_aa_sq - outer_solid_sq);
        // Smoothstep: 3t^2 - 2t^3
        t = t * t * (3.0f - 2.0f * t);
        return 1.0f - t;
    }

    // Inner edge anti-aliasing - use smoothstep for consistency
    if (inner_sq > 0 && dist_sq >= inner_aa_sq && dist_sq < inner_solid_sq)
    {
        float t = (inner_solid_sq - dist_sq) / (inner_solid_sq - inner_aa_sq);
        // Smoothstep: 3t^2 - 2t^3
        t = t * t * (3.0f - 2.0f * t);
        return 1.0f - t;
    }

    return 0.0f;
}

/**
 * Draw a complete ring (360 degree arc) - optimized for full circles
 * Uses unified AA width and smoothstep for consistency
 */
void draw_arc_as_ring(DrawContext *ctx, float center_x, float center_y,
                      float arc_radius, float line_width, PixelColor color)
{
    if (line_width <= 0) { return; }

    float outer_radius = arc_radius + line_width / 2;
    float inner_radius = fmaxf(arc_radius - line_width / 2, 0);
    float aa_width = 1.5f;  // Unified AA width

    float outer_sq = outer_radius * outer_radius;
    float inner_sq = inner_radius * inner_radius;
    float outer_aa_sq = (outer_radius + aa_width) * (outer_radius + aa_width);
    float inner_aa_sq = fmaxf((inner_radius - aa_width) * (inner_radius - aa_width), 0);

    float outer_solid_sq = (outer_radius - aa_width) * (outer_radius - aa_width);
    float inner_solid_sq = (inner_radius + aa_width) * (inner_radius + aa_width);

    int max_y = (int)ceilf(outer_radius + aa_width);

    for (int y = 0; y <= max_y; y++)
    {
        float y_sq = y * y;

        int x_start = y;
        int x_end = (int)ceilf(sqrtf(outer_aa_sq - y_sq));

        if (x_start > x_end) { continue; }

        int inner_x_start = 0;
        if (y <= inner_radius + aa_width)
        {
            inner_x_start = (int)ceilf(sqrtf(fmaxf(inner_aa_sq - y_sq, 0)));
            if (inner_x_start > x_start && inner_x_start <= x_end)
            {
                x_start = inner_x_start;
            }
        }

        for (int x = x_start; x <= x_end; x++)
        {
            float dist_sq = x * x + y_sq;

            // Compute coverage using smoothstep for consistency
            float coverage = compute_coverage_fast(dist_sq, outer_sq, inner_sq,
                                                   outer_aa_sq, inner_aa_sq,
                                                   outer_solid_sq, inner_solid_sq);
            if (coverage < 0.001f) { continue; }

            // Draw 8-way symmetric pixels
            int coords[8][2];
            int point_count = 0;

            if (x == 0 && y == 0)
            {
                coords[point_count][0] = (int)(center_x + 0); coords[point_count][1] = (int)(center_y + 0);
                point_count++;
            }
            else if (y == 0)
            {
                coords[point_count][0] = (int)(center_x + x); coords[point_count][1] = (int)(center_y + 0);
                point_count++;
                coords[point_count][0] = (int)(center_x - x); coords[point_count][1] = (int)(center_y + 0);
                point_count++;
                coords[point_count][0] = (int)(center_x + 0); coords[point_count][1] = (int)(center_y + x);
                point_count++;
                coords[point_count][0] = (int)(center_x + 0); coords[point_count][1] = (int)(center_y - x);
                point_count++;
            }
            else if (x == y)
            {
                coords[point_count][0] = (int)(center_x + x); coords[point_count][1] = (int)(center_y + y);
                point_count++;
                coords[point_count][0] = (int)(center_x - x); coords[point_count][1] = (int)(center_y + y);
                point_count++;
                coords[point_count][0] = (int)(center_x + x); coords[point_count][1] = (int)(center_y - y);
                point_count++;
                coords[point_count][0] = (int)(center_x - x); coords[point_count][1] = (int)(center_y - y);
                point_count++;
            }
            else
            {
                coords[point_count][0] = (int)(center_x + x); coords[point_count][1] = (int)(center_y + y);
                point_count++;
                coords[point_count][0] = (int)(center_x - x); coords[point_count][1] = (int)(center_y + y);
                point_count++;
                coords[point_count][0] = (int)(center_x + x); coords[point_count][1] = (int)(center_y - y);
                point_count++;
                coords[point_count][0] = (int)(center_x - x); coords[point_count][1] = (int)(center_y - y);
                point_count++;
                coords[point_count][0] = (int)(center_x + y); coords[point_count][1] = (int)(center_y + x);
                point_count++;
                coords[point_count][0] = (int)(center_x - y); coords[point_count][1] = (int)(center_y + x);
                point_count++;
                coords[point_count][0] = (int)(center_x + y); coords[point_count][1] = (int)(center_y - x);
                point_count++;
                coords[point_count][0] = (int)(center_x - y); coords[point_count][1] = (int)(center_y - x);
                point_count++;
            }

            for (int i = 0; i < point_count; i++)
            {
                int px = coords[i][0];
                int py = coords[i][1];
                if (px >= 0 && px < ctx->width && py >= 0 && py < ctx->height)
                {
                    add_pixel_aa(ctx, px, py, color, coverage);
                }
            }
        }
    }
}

// ==================== Optimized Arc Drawing ====================

// Normalize angle to [0, 360)
static inline float normalize_angle_arc(float angle)
{
    while (angle < 0) { angle += 360.0f; }
    while (angle > 360.0f) { angle -= 360.0f; }
    return angle;
}

// Check if angle is in arc range (handles cross-zero correctly)
static inline bool is_angle_in_arc_range(float angle, float start, float end)
{
    angle = normalize_angle_arc(angle);
    start = normalize_angle_arc(start);
    end = normalize_angle_arc(end);

    if (fabsf(end - start) >= 359.9f)
    {
        return true; // Full circle
    }

    if (start <= end)
    {
        return (angle >= start && angle <= end);
    }
    else
    {
        // Cross zero: e.g., start=350, end=10
        return (angle >= start || angle <= end);
    }
}

// Compute signed distance from point to arc (negative = inside)
// This provides seamless cap-to-arc transition
static inline float arc_sdf(float px, float py, float cx, float cy,
                            float radius, float line_width,
                            float start_angle, float end_angle)
{
    float dx = px - cx;
    float dy = py - cy;
    float dist = sqrtf(dx * dx + dy * dy);

    // Distance to ring
    float inner_r = radius - line_width / 2.0f;
    float outer_r = radius + line_width / 2.0f;
    float ring_dist;

    if (dist < inner_r)
    {
        ring_dist = inner_r - dist; // Inside inner circle
    }
    else if (dist > outer_r)
    {
        ring_dist = dist - outer_r; // Outside outer circle
    }
    else
    {
        ring_dist = -fminf(dist - inner_r, outer_r - dist); // Inside ring (negative)
    }

    // Check angle constraint
    if (fabsf(end_angle - start_angle) < 359.9f)
    {
        float angle = atan2f(dy, dx) * 180.0f / M_PI;
        if (!is_angle_in_arc_range(angle, start_angle, end_angle))
        {
            // Outside angle range - compute distance to arc endpoints (caps)
            float start_rad = start_angle * M_PI / 180.0f;
            float end_rad = end_angle * M_PI / 180.0f;

            float start_x = cx + radius * cosf(start_rad);
            float start_y = cy + radius * sinf(start_rad);
            float end_x = cx + radius * cosf(end_rad);
            float end_y = cy + radius * sinf(end_rad);

            float dist_to_start = sqrtf((px - start_x) * (px - start_x) + (py - start_y) * (py - start_y));
            float dist_to_end = sqrtf((px - end_x) * (px - end_x) + (py - end_y) * (py - end_y));

            // Cap SDF: distance to nearest cap center minus cap radius
            float cap_dist = fminf(dist_to_start, dist_to_end) - line_width / 2.0f;
            // Return the maximum of ring_dist and cap_dist for seamless blending
            return fmaxf(ring_dist, cap_dist);
        }
    }

    return ring_dist;
}

// ==================== Optimized Arc Drawing with Quadrant-based approach ====================

// Fast angle check using quadrant pre-filtering and lg_atan2
static inline bool is_in_angle_range_fast(float dx, float dy, uint16_t start_angle,
                                          uint16_t end_angle)
{
    // Quick quadrant check using pixel center position
    // Use > instead of >= for consistent boundary handling
    int quadrant;
    if (dx > 0)
    {
        quadrant = (dy >= 0) ? 0 : 3;  // Q1 or Q4
    }
    else if (dx < 0)
    {
        quadrant = (dy >= 0) ? 1 : 2;  // Q2 or Q3
    }
    else
    {
        // dx == 0: on Y axis
        quadrant = (dy >= 0) ? 0 : 3;  // Assign to Q1 or Q4
    }

    uint16_t quadrant_start = quadrant * 90;
    uint16_t quadrant_end = quadrant_start + 89;  // Use 89 instead of 90 to avoid boundary overlap

    // Check if current quadrant is in arc range
    if (end_angle < start_angle)
    {
        // Cross zero
        if (!(quadrant_start <= end_angle || quadrant_end >= start_angle))
        {
            return false;
        }
    }
    else
    {
        // Continuous range
        if (quadrant_end < start_angle || quadrant_start > end_angle)
        {
            return false;
        }
    }

    // Quadrant check passed, use fast atan2 for precise check
    uint16_t angle = lg_atan2((int)dy, (int)dx);

    // Inline angle range check
    if (end_angle >= start_angle)
    {
        return (angle >= start_angle && angle <= end_angle);
    }
    else
    {
        // Cross zero: e.g., start=350, end=10
        return (angle >= start_angle || angle <= end_angle);
    }
}

/**
 * Draw arc with anti-aliasing - hybrid approach for performance + quality
 * Key features:
 * 1. Fast path for arc body using squared distance
 * 2. arc_sdf only for cap regions (seamless transition)
 * 3. Smooth anti-aliasing using smoothstep
 */
void draw_arc_df_aa(DrawContext *ctx, float center_x, float center_y,
                    float radius, float line_width,
                    float start_angle, float end_angle,
                    PixelColor stroke_color)
{
    if (radius <= 0 || line_width <= 0) { return; }

    // Normalize angles
    start_angle = normalize_angle_arc(start_angle);
    end_angle = normalize_angle_arc(end_angle);

    // Calculate angle span
    float angle_span = end_angle - start_angle;
    if (angle_span <= 0) { angle_span += 360.0f; }

    // Check for full circle
    const float EPSILON = 0.001f;
    bool is_full_circle = (fabsf(angle_span - 360.0f) < EPSILON);

    // Use optimized ring drawing for full circles
    if (is_full_circle)
    {
        draw_arc_as_ring(ctx, center_x, center_y, radius, line_width, stroke_color);
        return;
    }

    // Pre-calculate radii and squared values
    float outer_radius = radius + line_width / 2.0f;
    float inner_radius = fmaxf(radius - line_width / 2.0f, 0);
    float aa_width = 1.5f;  // Unified AA width for both arc body and caps

    float outer_sq = outer_radius * outer_radius;
    float inner_sq = inner_radius * inner_radius;
    float outer_aa_sq = (outer_radius + aa_width) * (outer_radius + aa_width);
    float inner_aa_sq = fmaxf((inner_radius - aa_width) * (inner_radius - aa_width), 0);

    // Solid region boundaries (no AA needed)
    float outer_solid = outer_radius - aa_width;
    float inner_solid = inner_radius + aa_width;
    float outer_solid_sq = outer_solid * outer_solid;
    float inner_solid_sq = inner_solid * inner_solid;

    // Convert angles to integer degrees for fast comparison
    uint16_t start_angle_deg = (uint16_t)start_angle;
    uint16_t end_angle_deg = (uint16_t)end_angle;
    while (start_angle_deg >= 360) { start_angle_deg -= 360; }
    while (end_angle_deg >= 360) { end_angle_deg -= 360; }

    // Pre-calculate cap positions
    float start_rad = start_angle * M_PI / 180.0f;
    float end_rad = end_angle * M_PI / 180.0f;
    float start_cap_x = center_x + radius * cosf(start_rad);
    float start_cap_y = center_y + radius * sinf(start_rad);
    float end_cap_x = center_x + radius * cosf(end_rad);
    float end_cap_y = center_y + radius * sinf(end_rad);

    float cap_r = line_width / 2.0f;
    float cap_check_sq = (cap_r + aa_width + 2.0f) * (cap_r + aa_width + 2.0f);

    // Determine which quadrants contain the arc
    bool quadrants[4] = {false, false, false, false};
    uint16_t start_quadrant = start_angle_deg / 90;
    uint16_t end_quadrant = end_angle_deg / 90;

    if (end_angle_deg >= start_angle_deg)
    {
        for (int q = start_quadrant; q <= end_quadrant; q++)
        {
            quadrants[q % 4] = true;
        }
    }
    else
    {
        // Cross zero
        for (int q = start_quadrant; q < 4; q++)
        {
            quadrants[q] = true;
        }
        for (int q = 0; q <= end_quadrant; q++)
        {
            quadrants[q] = true;
        }
    }

    // Determine which quadrant should draw each cap (to avoid double-drawing)
    // Start cap quadrant - based on start angle
    int start_cap_quad = start_angle_deg / 90;

    // End cap quadrant - based on end angle
    int end_cap_quad = end_angle_deg / 90;

    // Extract color components
    uint8_t color_a = (stroke_color >> 24) & 0xFF;
    uint8_t color_r = (stroke_color >> 16) & 0xFF;
    uint8_t color_g = (stroke_color >> 8) & 0xFF;
    uint8_t color_b = stroke_color & 0xFF;

    if (color_a == 0) { return; }

    // Direct pixel access
    int pixel_size = (ctx->format == PIXEL_FORMAT_RGB565) ? 2 : 4;
    int stride = ctx->width * pixel_size;

    // Process each quadrant with strict non-overlapping boundaries
    for (int quad = 0; quad < 4; quad++)
    {
        if (!quadrants[quad]) { continue; }

        // Calculate quadrant bounds with strict boundaries to avoid overlap
        // Use center_x/center_y as exclusive boundaries between quadrants
        int qx_start, qx_end, qy_start, qy_end;
        float cap_expand = cap_r + aa_width + 1.0f;
        int center_x_int = (int)center_x;
        int center_y_int = (int)center_y;

        // Check if start/end cap is in this quadrant and needs boundary extension
        bool has_start_cap = (start_cap_quad == quad);
        bool has_end_cap = (end_cap_quad == quad);

        switch (quad)
        {
        case 0: // Q0: x >= center, y >= center (angles 0-90)
            qx_start = center_x_int;
            qx_end = (int)(center_x + outer_radius + aa_width + cap_expand);
            qy_start = center_y_int;
            qy_end = (int)(center_y + outer_radius + aa_width + cap_expand);
            // Extend boundaries only if cap belongs to this quadrant (by angle, not position)
            if (has_start_cap)
            {
                qx_start = LG_MIN(qx_start, (int)(start_cap_x - cap_expand));
                qy_start = LG_MIN(qy_start, (int)(start_cap_y - cap_expand));
            }
            if (has_end_cap)
            {
                qx_start = LG_MIN(qx_start, (int)(end_cap_x - cap_expand));
                qy_start = LG_MIN(qy_start, (int)(end_cap_y - cap_expand));
            }
            break;
        case 1: // Q1: x < center, y >= center (angles 90-180)
            qx_start = (int)(center_x - outer_radius - aa_width - cap_expand);
            qx_end = center_x_int - 1;
            qy_start = center_y_int;
            qy_end = (int)(center_y + outer_radius + aa_width + cap_expand);
            // Extend boundaries only if cap belongs to this quadrant
            if (has_start_cap)
            {
                qx_end = LG_MAX(qx_end, (int)(start_cap_x + cap_expand));
                qy_start = LG_MIN(qy_start, (int)(start_cap_y - cap_expand));
            }
            if (has_end_cap)
            {
                qx_end = LG_MAX(qx_end, (int)(end_cap_x + cap_expand));
                qy_start = LG_MIN(qy_start, (int)(end_cap_y - cap_expand));
            }
            break;
        case 2: // Q2: x < center, y < center (angles 180-270)
            qx_start = (int)(center_x - outer_radius - aa_width - cap_expand);
            qx_end = center_x_int - 1;
            qy_start = (int)(center_y - outer_radius - aa_width - cap_expand);
            qy_end = center_y_int - 1;
            // Extend boundaries only if cap belongs to this quadrant
            if (has_start_cap)
            {
                qx_end = LG_MAX(qx_end, (int)(start_cap_x + cap_expand));
                qy_end = LG_MAX(qy_end, (int)(start_cap_y + cap_expand));
            }
            if (has_end_cap)
            {
                qx_end = LG_MAX(qx_end, (int)(end_cap_x + cap_expand));
                qy_end = LG_MAX(qy_end, (int)(end_cap_y + cap_expand));
            }
            break;
        case 3: // Q3: x >= center, y < center (angles 270-360)
            qx_start = center_x_int;
            qx_end = (int)(center_x + outer_radius + aa_width + cap_expand);
            qy_start = (int)(center_y - outer_radius - aa_width - cap_expand);
            qy_end = center_y_int - 1;
            // Extend boundaries only if cap belongs to this quadrant
            if (has_start_cap)
            {
                qx_start = LG_MIN(qx_start, (int)(start_cap_x - cap_expand));
                qy_end = LG_MAX(qy_end, (int)(start_cap_y + cap_expand));
            }
            if (has_end_cap)
            {
                qx_start = LG_MIN(qx_start, (int)(end_cap_x - cap_expand));
                qy_end = LG_MAX(qy_end, (int)(end_cap_y + cap_expand));
            }
            break;
        }

        // Clip to context bounds
        qx_start = LG_MAX(qx_start, ctx->clip_rect.x);
        qx_end = LG_MIN(qx_end, ctx->clip_rect.x + ctx->clip_rect.w - 1);
        qy_start = LG_MAX(qy_start, ctx->clip_rect.y);
        qy_end = LG_MIN(qy_end, ctx->clip_rect.y + ctx->clip_rect.h - 1);

        if (qx_start > qx_end || qy_start > qy_end) { continue; }

        // Process rows in this quadrant
        for (int py = qy_start; py <= qy_end; py++)
        {
            float dy = (py + 0.5f) - center_y;
            float dy_sq = dy * dy;
            uint8_t *row = ctx->buffer + py * stride;

            for (int px = qx_start; px <= qx_end; px++)
            {
                float dx = (px + 0.5f) - center_x;
                float dist_sq = dx * dx + dy_sq;

                // Quick rejection: outside AA region
                if (dist_sq > outer_aa_sq) { continue; }
                if (inner_sq > 0 && dist_sq < inner_aa_sq) { continue; }

                float coverage;

                // Check if in angle range
                bool in_angle = is_in_angle_range_fast(dx, dy, start_angle_deg, end_angle_deg);

                if (!in_angle)
                {
                    // Outside angle range - check if in cap region
                    float sample_x = px + 0.5f;
                    float sample_y = py + 0.5f;
                    float dx_start = sample_x - start_cap_x;
                    float dy_start = sample_y - start_cap_y;
                    float dx_end = sample_x - end_cap_x;
                    float dy_end = sample_y - end_cap_y;

                    float dist_start_sq = dx_start * dx_start + dy_start * dy_start;
                    float dist_end_sq = dx_end * dx_end + dy_end * dy_end;

                    // Quick rejection for cap region
                    if (dist_start_sq > cap_check_sq && dist_end_sq > cap_check_sq)
                    {
                        continue;
                    }

                    // Use arc_sdf for seamless cap transition
                    float sdf = arc_sdf(sample_x, sample_y, center_x, center_y,
                                        radius, line_width, start_angle, end_angle);
                    coverage = 1.0f - smoothstep(-aa_width, aa_width, sdf);
                }
                else
                {
                    // In angle range - use fast coverage calculation
                    coverage = compute_coverage_fast(dist_sq, outer_sq, inner_sq,
                                                     outer_aa_sq, inner_aa_sq,
                                                     outer_solid_sq, inner_solid_sq);
                }

                if (coverage < 0.001f) { continue; }

                // Compute final alpha
                uint8_t final_alpha = (uint8_t)(color_a * coverage);
                if (final_alpha == 0) { continue; }

                // Write pixel
                int byte_offset = px * pixel_size;

                if (ctx->format == PIXEL_FORMAT_ARGB8888)
                {
                    uint32_t *pixel_ptr = (uint32_t *)(row + byte_offset);

                    if (final_alpha == 255)
                    {
                        *pixel_ptr = stroke_color;
                    }
                    else
                    {
                        uint32_t bg = *pixel_ptr;
                        uint8_t bg_a = (bg >> 24) & 0xFF;

                        if (bg_a == 0)
                        {
                            *pixel_ptr = (final_alpha << 24) | (color_r << 16) | (color_g << 8) | color_b;
                        }
                        else
                        {
                            // Check if background is same color (to avoid double-draw artifacts)
                            uint8_t bg_r = (bg >> 16) & 0xFF;
                            uint8_t bg_g = (bg >> 8) & 0xFF;
                            uint8_t bg_b = bg & 0xFF;

                            // If same color, use MAX alpha instead of ADD to prevent transparency stacking
                            if (bg_r == color_r && bg_g == color_g && bg_b == color_b)
                            {
                                uint8_t max_alpha = (final_alpha > bg_a) ? final_alpha : bg_a;
                                *pixel_ptr = (max_alpha << 24) | (color_r << 16) | (color_g << 8) | color_b;
                            }
                            else
                            {
                                // Different color, use standard alpha blending
                                uint16_t inv_alpha = 256 - final_alpha;

                                uint8_t out_r = ((bg_r * inv_alpha + color_r * final_alpha) >> 8) & 0xFF;
                                uint8_t out_g = ((bg_g * inv_alpha + color_g * final_alpha) >> 8) & 0xFF;
                                uint8_t out_b = ((bg_b * inv_alpha + color_b * final_alpha) >> 8) & 0xFF;
                                uint8_t out_a = bg_a + (((255 - bg_a) * final_alpha) >> 8);

                                *pixel_ptr = (out_a << 24) | (out_r << 16) | (out_g << 8) | out_b;
                            }
                        }
                    }
                }
                else if (ctx->format == PIXEL_FORMAT_RGB565)
                {
                    uint16_t *pixel_ptr = (uint16_t *)(row + byte_offset);

                    if (final_alpha == 255)
                    {
                        *pixel_ptr = (uint16_t)stroke_color;
                    }
                    else
                    {
                        uint16_t bg = *pixel_ptr;
                        uint16_t inv_alpha = 256 - final_alpha;

                        uint8_t bg_r = (bg >> 11) & 0x1F;
                        uint8_t bg_g = (bg >> 5) & 0x3F;
                        uint8_t bg_b = bg & 0x1F;

                        uint8_t fg_r = (color_r >> 3) & 0x1F;
                        uint8_t fg_g = (color_g >> 2) & 0x3F;
                        uint8_t fg_b = (color_b >> 3) & 0x1F;

                        uint8_t out_r = ((bg_r * inv_alpha + fg_r * final_alpha) >> 8) & 0x1F;
                        uint8_t out_g = ((bg_g * inv_alpha + fg_g * final_alpha) >> 8) & 0x3F;
                        uint8_t out_b = ((bg_b * inv_alpha + fg_b * final_alpha) >> 8) & 0x1F;

                        *pixel_ptr = (out_r << 11) | (out_g << 5) | out_b;
                    }
                }
            }
        }
    }
}

void draw_arc(DrawContext *ctx, float center_x, float center_y,
              float radius, float line_width,
              float start_angle, float end_angle,
              PixelColor stroke_color)
{
    if (ctx->enable_aa)
    {
        draw_arc_df_aa(ctx, center_x, center_y, radius, line_width, start_angle, end_angle, stroke_color);
    }
    // No non-AA version for now, always use AA
}

// ==================== Gradient Functions ====================

/**
 * Initialize a gradient structure
 */
void gradient_init(Gradient *grad, GradientType type)
{
    if (grad == NULL) { return; }

    grad->type = type;
    grad->stop_count = 0;
    grad->linear_x1 = 0;
    grad->linear_y1 = 0;
    grad->linear_x2 = 0;
    grad->linear_y2 = 0;
    grad->radial_cx = 0;
    grad->radial_cy = 0;
    grad->radial_r = 0;
    grad->angular_cx = 0;
    grad->angular_cy = 0;
    grad->angular_start = 0;
    grad->angular_end = 360;
}

/**
 * Add a color stop to gradient
 */
void gradient_add_stop(Gradient *grad, float position, PixelColor color)
{
    if (grad == NULL || grad->stop_count >= 8) { return; }

    // Clamp position to [0, 1]
    position = LG_CLAMP(position, 0.0f, 1.0f);

    // Insert stop in sorted order
    int insert_idx = grad->stop_count;
    for (int i = 0; i < grad->stop_count; i++)
    {
        if (position < grad->stops[i].position)
        {
            insert_idx = i;
            break;
        }
    }

    // Shift existing stops
    for (int i = grad->stop_count; i > insert_idx; i--)
    {
        grad->stops[i] = grad->stops[i - 1];
    }

    // Insert new stop
    grad->stops[insert_idx].position = position;
    grad->stops[insert_idx].color = color;
    grad->stop_count++;
}

/**
 * Interpolate color between two stops
 */
static PixelColor interpolate_color(PixelColor c1, PixelColor c2, float t)
{
    int a1 = (c1 >> 24) & 0xFF;
    int r1 = (c1 >> 16) & 0xFF;
    int g1 = (c1 >> 8) & 0xFF;
    int b1 = c1 & 0xFF;

    int a2 = (c2 >> 24) & 0xFF;
    int r2 = (c2 >> 16) & 0xFF;
    int g2 = (c2 >> 8) & 0xFF;
    int b2 = c2 & 0xFF;

    // Use int for interpolation to avoid overflow
    int a = (int)(a1 + (a2 - a1) * t);
    int r = (int)(r1 + (r2 - r1) * t);
    int g = (int)(g1 + (g2 - g1) * t);
    int b = (int)(b1 + (b2 - b1) * t);

    // Clamp to [0, 255]
    a = (a < 0) ? 0 : ((a > 255) ? 255 : a);
    r = (r < 0) ? 0 : ((r > 255) ? 255 : r);
    g = (g < 0) ? 0 : ((g > 255) ? 255 : g);
    b = (b < 0) ? 0 : ((b > 255) ? 255 : b);

    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

/**
 * Get color from gradient at position t (0.0 to 1.0)
 */
PixelColor gradient_get_color(Gradient *grad, float t)
{
    if (grad == NULL || grad->stop_count == 0)
    {
        return 0xFF000000; // Black
    }

    if (grad->stop_count == 1)
    {
        return grad->stops[0].color;
    }

    // Clamp t to [0, 1]
    t = LG_CLAMP(t, 0.0f, 1.0f);

    // Find the two stops to interpolate between
    if (t <= grad->stops[0].position)
    {
        return grad->stops[0].color;
    }

    if (t >= grad->stops[grad->stop_count - 1].position)
    {
        return grad->stops[grad->stop_count - 1].color;
    }

    for (int i = 0; i < grad->stop_count - 1; i++)
    {
        if (t >= grad->stops[i].position && t <= grad->stops[i + 1].position)
        {
            float pos1 = grad->stops[i].position;
            float pos2 = grad->stops[i + 1].position;
            float local_t = (t - pos1) / (pos2 - pos1);

            return interpolate_color(grad->stops[i].color, grad->stops[i + 1].color, local_t);
        }
    }

    return grad->stops[grad->stop_count - 1].color;
}

/**
 * Get color from gradient at specific point (for linear/radial gradients)
 */
PixelColor gradient_get_color_at_point(Gradient *grad, float x, float y)
{
    if (grad == NULL) { return 0xFF000000; }

    float t = 0.0f;

    switch (grad->type)
    {
    case GRADIENT_LINEAR:
        {
            // Calculate projection onto gradient line
            float dx = grad->linear_x2 - grad->linear_x1;
            float dy = grad->linear_y2 - grad->linear_y1;
            float len_sq = dx * dx + dy * dy;

            if (len_sq < 0.0001f)
            {
                t = 0.0f;
            }
            else
            {
                float px = x - grad->linear_x1;
                float py = y - grad->linear_y1;
                t = (px * dx + py * dy) / len_sq;
            }
        }
        break;

    case GRADIENT_RADIAL:
        {
            // Calculate distance from center
            float dx = x - grad->radial_cx;
            float dy = y - grad->radial_cy;
            float dist = sqrtf(dx * dx + dy * dy);

            if (grad->radial_r > 0.0001f)
            {
                t = dist / grad->radial_r;
            }
            else
            {
                t = 0.0f;
            }
        }
        break;

    default:
        t = 0.0f;
        break;
    }

    return gradient_get_color(grad, t);
}

/**
 * Get color from gradient at specific angle (for angular gradients)
 * Improved version with correct angle handling for full circles
 */
PixelColor gradient_get_color_at_angle(Gradient *grad, float angle)
{
    if (grad == NULL || grad->type != GRADIENT_ANGULAR)
    {
        return 0xFF000000;
    }

    if (grad->stop_count == 0)
    {
        return 0xFF000000;
    }

    if (grad->stop_count == 1)
    {
        return grad->stops[0].color;
    }

    // Get gradient angle range (use original values, don't normalize)
    float start = grad->angular_start;
    float end = grad->angular_end;

    // Calculate angle span
    float span = end - start;
    if (span <= 0.0f) { span += 360.0f; }

    // Normalize input angle to be relative to start
    float angle_offset = angle - start;

    // Handle wrap-around
    while (angle_offset < 0.0f) { angle_offset += 360.0f; }
    while (angle_offset >= 360.0f) { angle_offset -= 360.0f; }

    // Calculate t (0.0 to 1.0)
    float t = angle_offset / span;

    // Clamp t to [0, 1] range
    if (t < 0.0f) { t = 0.0f; }
    if (t > 1.0f) { t = 1.0f; }

    return gradient_get_color(grad, t);
}

/**
 * Draw arc with gradient - optimized version using LUT
 * Performance optimizations:
 * 1. Pre-computed angle-to-color lookup table (LUT)
 * 2. Fast integer angle calculation using atan2 approximation
 * 3. Squared distance comparison (no sqrt)
 * 4. Minimized branching in hot loop
 */
void draw_arc_df_aa_gradient(DrawContext *ctx, float center_x, float center_y,
                             float radius, float line_width,
                             float start_angle, float end_angle,
                             Gradient *gradient)
{
    if (radius <= 0 || line_width <= 0 || gradient == NULL) { return; }

    // Read gradient angles from struct using union to handle potential alignment issues
    volatile unsigned char *bytes = (volatile unsigned char *)gradient;
    union { unsigned char b[4]; float f; } start_union, end_union;
    start_union.b[0] = bytes[108]; start_union.b[1] = bytes[109];
    start_union.b[2] = bytes[110]; start_union.b[3] = bytes[111];
    end_union.b[0] = bytes[112]; end_union.b[1] = bytes[113];
    end_union.b[2] = bytes[114]; end_union.b[3] = bytes[115];
    float gradient_start = start_union.f;
    float gradient_end = end_union.f;

    // Save original angles for cap calculation
    float original_start_angle = start_angle;
    float original_end_angle = end_angle;

    // CRITICAL: Calculate original angle span BEFORE normalization
    // This preserves the intent of full circle (e.g., 270-630 = 360° span)
    float original_span = original_end_angle - original_start_angle;
    bool is_full_circle = (original_span >= 359.9f || original_span <= -359.9f ||
                           fabsf(original_span) < 0.1f);  // 270-270 case

    // For full circle, treat as 0-360 internally
    if (is_full_circle)
    {
        start_angle = 0.0f;
        end_angle = 360.0f;
    }
    else
    {
        // Normalize angles
        start_angle = normalize_angle_arc(start_angle);
        end_angle = normalize_angle_arc(end_angle);
    }

    // Calculate angle span
    float angle_span = end_angle - start_angle;
    if (angle_span <= 0) { angle_span += 360.0f; }

    // Calculate bounding box
    float outer_r = radius + line_width / 2.0f + 2.0f;
    int min_x = (int)(center_x - outer_r);
    int max_x = (int)(center_x + outer_r) + 1;
    int min_y = (int)(center_y - outer_r);
    int max_y = (int)(center_y + outer_r) + 1;

    // Clip to context bounds
    min_x = LG_MAX(min_x, ctx->clip_rect.x);
    max_x = LG_MIN(max_x, ctx->clip_rect.x + ctx->clip_rect.w);
    min_y = LG_MAX(min_y, ctx->clip_rect.y);
    max_y = LG_MIN(max_y, ctx->clip_rect.y + ctx->clip_rect.h);

    if (min_x >= max_x || min_y >= max_y) { return; }

    // Direct pixel access
    int pixel_size = (ctx->format == PIXEL_FORMAT_RGB565) ? 2 : 4;
    uint8_t *buffer = ctx->buffer;
    int stride = ctx->width * pixel_size;

    // ========== OPTIMIZATION 1: Pre-compute color LUT ==========
    // Build 360-entry lookup table for angle-to-color mapping
    PixelColor color_lut[360];
    for (int i = 0; i < 360; i++)
    {
        color_lut[i] = gradient_get_color_at_angle(gradient, (float)i);
    }

    // ========== OPTIMIZATION 2: Pre-calculate constants ==========
    // Check if gradient angle span exceeds 360 degrees
    float gradient_span = gradient_end - gradient_start;
    if (gradient_span <= 0.0f) { gradient_span += 360.0f; }
    bool gradient_over_360 = (gradient_span > 360.0f);

    bool has_caps = (angle_span < 359.9f);

    // CRITICAL: When gradient > 360°, we need to draw end cap even for full circle
    bool draw_end_cap = gradient_over_360 && !has_caps;

    float start_cap_x = 0, start_cap_y = 0, end_cap_x = 0, end_cap_y = 0;
    PixelColor start_color = gradient->stops[0].color;
    PixelColor end_color = gradient->stops[gradient->stop_count - 1].color;

    // Pre-normalize angles for range checking
    int norm_start_i = (int)start_angle;
    int norm_end_i = (int)end_angle;
    if (norm_start_i < 0) { norm_start_i += 360; }
    if (norm_start_i >= 360) { norm_start_i -= 360; }
    if (norm_end_i < 0) { norm_end_i += 360; }
    if (norm_end_i >= 360) { norm_end_i -= 360; }

    if (has_caps || draw_end_cap)
    {
        float start_rad = original_start_angle * M_PI / 180.0f;
        float end_rad = original_end_angle * M_PI / 180.0f;
        start_cap_x = center_x + radius * cosf(start_rad);
        start_cap_y = center_y + radius * sinf(start_rad);
        end_cap_x = center_x + radius * cosf(end_rad);
        end_cap_y = center_y + radius * sinf(end_rad);
    }

    // ========== OPTIMIZATION 3: Pre-calculate ring parameters ==========
    float inner_r = radius - line_width / 2.0f;
    float outer_radius = radius + line_width / 2.0f;
    float aa_width = 1.5f;  // Unified AA width for both arc body and caps

    // Pre-calculate squared distances for fast comparison
    float outer_sq = outer_radius * outer_radius;
    float inner_sq = inner_r * inner_r;
    float outer_aa_sq = (outer_radius + aa_width) * (outer_radius + aa_width);
    float inner_aa_sq = fmaxf((inner_r - aa_width) * (inner_r - aa_width), 0);

    // Solid region boundaries (no AA needed)
    float outer_solid = outer_radius - aa_width;
    float inner_solid = inner_r + aa_width;
    float outer_solid_sq = outer_solid * outer_solid;
    float inner_solid_sq = inner_solid * inner_solid;

    // ========== OPTIMIZED PATH FOR FULL CIRCLE ==========
    if (is_full_circle && !draw_end_cap)
    {
        // Full circle gradient - simplified loop without angle range check
        for (int py = min_y; py < max_y; py++)
        {
            float dy = py + 0.5f - center_y;
            float dy_sq = dy * dy;
            uint8_t *row = buffer + py * stride;

            for (int px = min_x; px < max_x; px++)
            {
                float dx = px + 0.5f - center_x;
                float dist_sq = dx * dx + dy_sq;

                // Quick rejection
                if (dist_sq > outer_aa_sq) { continue; }
                if (dist_sq < inner_aa_sq && inner_r > 2.0f) { continue; }

                // Compute coverage
                float coverage = compute_coverage_fast(dist_sq, outer_sq, inner_sq,
                                                       outer_aa_sq, inner_aa_sq,
                                                       outer_solid_sq, inner_solid_sq);
                if (coverage < 0.001f) { continue; }

                // Fast angle calculation
                int angle_i = lg_atan2((int)dy, (int)dx);
                if (angle_i < 0) { angle_i += 360; }
                if (angle_i >= 360) { angle_i -= 360; }

                // Direct LUT lookup for full circle
                PixelColor stroke_color = color_lut[angle_i];

                uint8_t color_a = (stroke_color >> 24) & 0xFF;
                if (color_a == 0) { continue; }

                uint8_t color_r = (stroke_color >> 16) & 0xFF;
                uint8_t color_g = (stroke_color >> 8) & 0xFF;
                uint8_t color_b = stroke_color & 0xFF;

                uint8_t final_alpha = (uint8_t)(color_a * coverage);
                if (final_alpha == 0) { continue; }

                int byte_offset = px * pixel_size;

                if (ctx->format == PIXEL_FORMAT_ARGB8888)
                {
                    uint32_t *pixel_ptr = (uint32_t *)(row + byte_offset);
                    if (final_alpha == 255)
                    {
                        *pixel_ptr = stroke_color;
                    }
                    else
                    {
                        uint32_t bg = *pixel_ptr;
                        uint8_t bg_a = (bg >> 24) & 0xFF;
                        if (bg_a == 0)
                        {
                            *pixel_ptr = (final_alpha << 24) | (color_r << 16) | (color_g << 8) | color_b;
                        }
                        else
                        {
                            uint16_t inv_alpha = 256 - final_alpha;
                            uint8_t bg_r = (bg >> 16) & 0xFF;
                            uint8_t bg_g = (bg >> 8) & 0xFF;
                            uint8_t bg_b = bg & 0xFF;
                            uint8_t out_r = ((bg_r * inv_alpha + color_r * final_alpha) >> 8) & 0xFF;
                            uint8_t out_g = ((bg_g * inv_alpha + color_g * final_alpha) >> 8) & 0xFF;
                            uint8_t out_b = ((bg_b * inv_alpha + color_b * final_alpha) >> 8) & 0xFF;
                            uint8_t out_a = bg_a + (((255 - bg_a) * final_alpha) >> 8);
                            *pixel_ptr = (out_a << 24) | (out_r << 16) | (out_g << 8) | out_b;
                        }
                    }
                }
                else if (ctx->format == PIXEL_FORMAT_RGB565)
                {
                    uint16_t *pixel_ptr = (uint16_t *)(row + byte_offset);
                    if (final_alpha == 255)
                    {
                        *pixel_ptr = (uint16_t)stroke_color;
                    }
                    else
                    {
                        uint16_t bg = *pixel_ptr;
                        uint16_t inv_alpha = 256 - final_alpha;
                        uint8_t bg_r = (bg >> 11) & 0x1F;
                        uint8_t bg_g = (bg >> 5) & 0x3F;
                        uint8_t bg_b = bg & 0x1F;
                        uint8_t fg_r = (color_r >> 3) & 0x1F;
                        uint8_t fg_g = (color_g >> 2) & 0x3F;
                        uint8_t fg_b = (color_b >> 3) & 0x1F;
                        uint8_t out_r = ((bg_r * inv_alpha + fg_r * final_alpha) >> 8) & 0x1F;
                        uint8_t out_g = ((bg_g * inv_alpha + fg_g * final_alpha) >> 8) & 0x3F;
                        uint8_t out_b = ((bg_b * inv_alpha + fg_b * final_alpha) >> 8) & 0x1F;
                        *pixel_ptr = (out_r << 11) | (out_g << 5) | out_b;
                    }
                }
            }
        }
        return;
    }

    // ========== Main rendering loop for partial arcs ==========
    // Use hybrid approach: fast path for arc body, arc_sdf only for cap regions
    float cap_r = line_width / 2.0f;
    float cap_check_sq = (cap_r + aa_width + 2.0f) * (cap_r + aa_width + 2.0f);

    for (int py = min_y; py < max_y; py++)
    {
        float dy = py + 0.5f - center_y;
        float dy_sq = dy * dy;
        uint8_t *row = buffer + py * stride;

        for (int px = min_x; px < max_x; px++)
        {
            float dx = px + 0.5f - center_x;
            float dist_sq = dx * dx + dy_sq;

            // Quick rejection: outside outer circle or inside inner circle
            if (dist_sq > outer_aa_sq) { continue; }
            if (dist_sq < inner_aa_sq && inner_r > 2.0f) { continue; }

            // ========== Fast angle calculation for color lookup ==========
            int angle_i = lg_atan2((int)dy, (int)dx);
            if (angle_i < 0) { angle_i += 360; }
            if (angle_i >= 360) { angle_i -= 360; }

            // ========== Angle range check ==========
            bool in_arc_range = true;
            if (has_caps)
            {
                if (norm_start_i <= norm_end_i)
                {
                    in_arc_range = (angle_i >= norm_start_i && angle_i <= norm_end_i);
                }
                else
                {
                    in_arc_range = (angle_i >= norm_start_i || angle_i <= norm_end_i);
                }
            }

            float coverage;
            PixelColor stroke_color;
            float sample_x = px + 0.5f;
            float sample_y = py + 0.5f;

            if (has_caps && !in_arc_range)
            {
                // In cap region - check distance to caps
                float dx_start = sample_x - start_cap_x;
                float dy_start = sample_y - start_cap_y;
                float dx_end = sample_x - end_cap_x;
                float dy_end = sample_y - end_cap_y;

                float dist_start_sq = dx_start * dx_start + dy_start * dy_start;
                float dist_end_sq = dx_end * dx_end + dy_end * dy_end;

                // Quick rejection for cap region
                if (dist_start_sq > cap_check_sq && dist_end_sq > cap_check_sq)
                {
                    continue;
                }

                // Use arc_sdf only for cap region pixels (seamless transition)
                float sdf = arc_sdf(sample_x, sample_y, center_x, center_y,
                                    radius, line_width, start_angle, end_angle);
                coverage = 1.0f - smoothstep(-aa_width, aa_width, sdf);

                if (coverage < 0.001f) { continue; }

                // Determine cap color
                if (gradient_over_360 && dist_end_sq < dist_start_sq)
                {
                    stroke_color = gradient_get_color_at_angle(gradient, (float)angle_i);
                }
                else
                {
                    stroke_color = (dist_start_sq < dist_end_sq) ? start_color : end_color;
                }
            }
            else
            {
                // In arc range - use fast coverage calculation
                coverage = compute_coverage_fast(dist_sq, outer_sq, inner_sq,
                                                 outer_aa_sq, inner_aa_sq,
                                                 outer_solid_sq, inner_solid_sq);
                if (coverage < 0.001f) { continue; }

                // Use LUT for color lookup
                stroke_color = color_lut[angle_i];
            }

            // Extract color components
            uint8_t color_a = (stroke_color >> 24) & 0xFF;
            if (color_a == 0) { continue; }

            uint8_t color_r = (stroke_color >> 16) & 0xFF;
            uint8_t color_g = (stroke_color >> 8) & 0xFF;
            uint8_t color_b = stroke_color & 0xFF;

            // Compute final alpha
            uint8_t final_alpha = (uint8_t)(color_a * coverage);
            if (final_alpha == 0) { continue; }

            // Write pixel
            int byte_offset = px * pixel_size;

            if (ctx->format == PIXEL_FORMAT_ARGB8888)
            {
                uint32_t *pixel_ptr = (uint32_t *)(row + byte_offset);

                if (final_alpha == 255)
                {
                    *pixel_ptr = stroke_color;
                }
                else
                {
                    uint32_t bg = *pixel_ptr;
                    uint8_t bg_a = (bg >> 24) & 0xFF;

                    if (bg_a == 0)
                    {
                        *pixel_ptr = (final_alpha << 24) | (color_r << 16) | (color_g << 8) | color_b;
                    }
                    else
                    {
                        uint16_t inv_alpha = 256 - final_alpha;
                        uint8_t bg_r = (bg >> 16) & 0xFF;
                        uint8_t bg_g = (bg >> 8) & 0xFF;
                        uint8_t bg_b = bg & 0xFF;

                        uint8_t out_r = ((bg_r * inv_alpha + color_r * final_alpha) >> 8) & 0xFF;
                        uint8_t out_g = ((bg_g * inv_alpha + color_g * final_alpha) >> 8) & 0xFF;
                        uint8_t out_b = ((bg_b * inv_alpha + color_b * final_alpha) >> 8) & 0xFF;
                        uint8_t out_a = bg_a + (((255 - bg_a) * final_alpha) >> 8);

                        *pixel_ptr = (out_a << 24) | (out_r << 16) | (out_g << 8) | out_b;
                    }
                }
            }
            else if (ctx->format == PIXEL_FORMAT_RGB565)
            {
                uint16_t *pixel_ptr = (uint16_t *)(row + byte_offset);

                if (final_alpha == 255)
                {
                    *pixel_ptr = (uint16_t)stroke_color;
                }
                else
                {
                    uint16_t bg = *pixel_ptr;
                    uint16_t inv_alpha = 256 - final_alpha;

                    uint8_t bg_r = (bg >> 11) & 0x1F;
                    uint8_t bg_g = (bg >> 5) & 0x3F;
                    uint8_t bg_b = bg & 0x1F;

                    uint8_t fg_r = (color_r >> 3) & 0x1F;
                    uint8_t fg_g = (color_g >> 2) & 0x3F;
                    uint8_t fg_b = (color_b >> 3) & 0x1F;

                    uint8_t out_r = ((bg_r * inv_alpha + fg_r * final_alpha) >> 8) & 0x1F;
                    uint8_t out_g = ((bg_g * inv_alpha + fg_g * final_alpha) >> 8) & 0x3F;
                    uint8_t out_b = ((bg_b * inv_alpha + fg_b * final_alpha) >> 8) & 0x1F;

                    *pixel_ptr = (out_r << 11) | (out_g << 5) | out_b;
                }
            }
        }
    }

    // ========== SPECIAL: Draw end cap for gradient > 360° on full circle ==========
    // When arc is full circle (0-360) but gradient exceeds 360° (e.g., 0-361),
    // we need to draw the end cap separately since arc_sdf doesn't handle it
    // The end cap should protrude OUTWARD from the arc at the end angle position
    if (draw_end_cap)
    {
        float cap_radius = line_width / 2.0f;

        // Calculate end cap center position
        // The cap center should be on the arc's center line (at radius distance from center)
        // at the gradient end angle position
        // Use gradient_end from gradient struct (already read at function start)
        float end_angle_rad = gradient_end * M_PI / 180.0f;
        float cap_center_x = center_x + radius * cosf(end_angle_rad);
        float cap_center_y = center_y + radius * sinf(end_angle_rad);

        int cap_min_x = (int)(cap_center_x - cap_radius - 2);
        int cap_max_x = (int)(cap_center_x + cap_radius + 2);
        int cap_min_y = (int)(cap_center_y - cap_radius - 2);
        int cap_max_y = (int)(cap_center_y + cap_radius + 2);

        // Clip to context bounds
        cap_min_x = LG_MAX(cap_min_x, ctx->clip_rect.x);
        cap_max_x = LG_MIN(cap_max_x, ctx->clip_rect.x + ctx->clip_rect.w);
        cap_min_y = LG_MAX(cap_min_y, ctx->clip_rect.y);
        cap_max_y = LG_MIN(cap_max_y, ctx->clip_rect.y + ctx->clip_rect.h);

        for (int py = cap_min_y; py < cap_max_y; py++)
        {
            float dy_cap = py + 0.5f - cap_center_y;
            uint8_t *row = buffer + py * stride;

            for (int px = cap_min_x; px < cap_max_x; px++)
            {
                float dx_cap = px + 0.5f - cap_center_x;

                // Distance from cap center
                float dist_cap = sqrtf(dx_cap * dx_cap + dy_cap * dy_cap);

                // SDF for circle cap
                float sdf = dist_cap - cap_radius;
                float coverage = 1.0f - smoothstep(-aa_width, aa_width, sdf);
                if (coverage < 0.001f) { continue; }

                // Use the end color of the gradient for the cap
                PixelColor stroke_color = end_color;

                uint8_t color_a = (stroke_color >> 24) & 0xFF;
                if (color_a == 0) { continue; }

                uint8_t color_r = (stroke_color >> 16) & 0xFF;
                uint8_t color_g = (stroke_color >> 8) & 0xFF;
                uint8_t color_b = stroke_color & 0xFF;

                uint8_t final_alpha = (uint8_t)(color_a * coverage);
                if (final_alpha == 0) { continue; }

                int byte_offset = px * pixel_size;

                if (ctx->format == PIXEL_FORMAT_ARGB8888)
                {
                    uint32_t *pixel_ptr = (uint32_t *)(row + byte_offset);
                    if (final_alpha == 255)
                    {
                        *pixel_ptr = stroke_color;
                    }
                    else
                    {
                        uint32_t bg = *pixel_ptr;
                        uint8_t bg_a = (bg >> 24) & 0xFF;
                        if (bg_a == 0)
                        {
                            *pixel_ptr = (final_alpha << 24) | (color_r << 16) | (color_g << 8) | color_b;
                        }
                        else
                        {
                            uint16_t inv_alpha = 256 - final_alpha;
                            uint8_t bg_r = (bg >> 16) & 0xFF;
                            uint8_t bg_g = (bg >> 8) & 0xFF;
                            uint8_t bg_b = bg & 0xFF;
                            uint8_t out_r = ((bg_r * inv_alpha + color_r * final_alpha) >> 8) & 0xFF;
                            uint8_t out_g = ((bg_g * inv_alpha + color_g * final_alpha) >> 8) & 0xFF;
                            uint8_t out_b = ((bg_b * inv_alpha + color_b * final_alpha) >> 8) & 0xFF;
                            uint8_t out_a = bg_a + (((255 - bg_a) * final_alpha) >> 8);
                            *pixel_ptr = (out_a << 24) | (out_r << 16) | (out_g << 8) | out_b;
                        }
                    }
                }
                else if (ctx->format == PIXEL_FORMAT_RGB565)
                {
                    uint16_t *pixel_ptr = (uint16_t *)(row + byte_offset);
                    if (final_alpha == 255)
                    {
                        *pixel_ptr = (uint16_t)stroke_color;
                    }
                    else
                    {
                        uint16_t bg = *pixel_ptr;
                        uint16_t inv_alpha = 256 - final_alpha;
                        uint8_t bg_r = (bg >> 11) & 0x1F;
                        uint8_t bg_g = (bg >> 5) & 0x3F;
                        uint8_t bg_b = bg & 0x1F;
                        uint8_t fg_r = (color_r >> 3) & 0x1F;
                        uint8_t fg_g = (color_g >> 2) & 0x3F;
                        uint8_t fg_b = (color_b >> 3) & 0x1F;
                        uint8_t out_r = ((bg_r * inv_alpha + fg_r * final_alpha) >> 8) & 0x1F;
                        uint8_t out_g = ((bg_g * inv_alpha + fg_g * final_alpha) >> 8) & 0x3F;
                        uint8_t out_b = ((bg_b * inv_alpha + fg_b * final_alpha) >> 8) & 0x1F;
                        *pixel_ptr = (out_r << 11) | (out_g << 5) | out_b;
                    }
                }
            }
        }
    }
}
