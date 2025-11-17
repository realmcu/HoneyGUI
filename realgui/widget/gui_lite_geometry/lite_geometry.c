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
    if (bg_a == 0)
    {
        uint8_t r = ((fg_color >> 16) & 0xFF) * alpha / 255;
        uint8_t g = ((fg_color >> 8) & 0xFF) * alpha / 255;
        uint8_t b = (fg_color & 0xFF) * alpha / 255;
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
    int  inner_radius_sq = (radius - 1) * (radius - 1);
    int  outer_radius_sq = (radius + 1) * (radius + 1);

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
                coverage = 1.0f - (dist - (radius - 0.7f)) / 1.4f;
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

// ==================== Arc drawing area calculation ====================
// arc angle range check
bool check_degree_arc_range(uint16_t angle, uint16_t start_angle, uint16_t end_angle,
                            bool is_closed)
{
    if (is_closed) { return true; }

    if (start_angle <= end_angle)
    {
        return angle >= start_angle && angle <= end_angle;
    }
    else
    {
        return angle >= start_angle || angle <= end_angle;
    }
}
void lg_draw_arc_get_area(int32_t x, int32_t y, uint16_t radius,  int32_t start_angle,
                          int32_t end_angle,
                          int32_t w, bool rounded, lg_area_t *area)
{
    int32_t rout = radius;
    int32_t start_angle_int = (int32_t) start_angle;
    int32_t end_angle_int = (int32_t) end_angle;

    /*Special case: full arc invalidation */
    if (end_angle_int == start_angle_int + 360 || start_angle_int == end_angle_int)
    {
        area->x1 = x - rout;
        area->y1 = y - rout;
        area->x2 = x + rout;
        area->y2 = y + rout;
        return;
    }

    if (start_angle_int > 360) { start_angle_int -= 360; }
    if (end_angle_int > 360) { end_angle_int -= 360; }


    int32_t rin = radius - w;
    int32_t extra_area = rounded ? w / 2 + 1 : 0;
    uint8_t start_quarter = start_angle_int / 90;
    uint8_t end_quarter = end_angle_int / 90;

    /*360 deg still counts as quarter 3 (360 / 90 would be 4)*/
    if (start_quarter == 4) { start_quarter = 3; }
    if (end_quarter == 4) { end_quarter = 3; }

    if (start_quarter == end_quarter && start_angle_int <= end_angle_int)
    {
        if (start_quarter == 0)
        {
            area->y1 = y + ((lg_trigo_sin(start_angle_int) * rin) >> LG_TRIGO_SHIFT) - extra_area;
            area->x2 = x + ((lg_trigo_sin(start_angle_int + 90) * rout) >> LG_TRIGO_SHIFT) + extra_area;

            area->y2 = y + ((lg_trigo_sin(end_angle_int) * rout) >> LG_TRIGO_SHIFT) + extra_area;
            area->x1 = x + ((lg_trigo_sin(end_angle_int + 90) * rin) >> LG_TRIGO_SHIFT) - extra_area;
        }
        else if (start_quarter == 1)
        {
            area->y2 = y + ((lg_trigo_sin(start_angle_int) * rout) >> LG_TRIGO_SHIFT) + extra_area;
            area->x2 = x + ((lg_trigo_sin(start_angle_int + 90) * rin) >> LG_TRIGO_SHIFT) + extra_area;

            area->y1 = y + ((lg_trigo_sin(end_angle_int) * rin) >> LG_TRIGO_SHIFT) - extra_area;
            area->x1 = x + ((lg_trigo_sin(end_angle_int + 90) * rout) >> LG_TRIGO_SHIFT) - extra_area;
        }
        else if (start_quarter == 2)
        {
            area->x1 = x + ((lg_trigo_sin(start_angle_int + 90) * rout) >> LG_TRIGO_SHIFT) - extra_area;
            area->y2 = y + ((lg_trigo_sin(start_angle_int) * rin) >> LG_TRIGO_SHIFT) + extra_area;

            area->y1 = y + ((lg_trigo_sin(end_angle_int) * rout) >> LG_TRIGO_SHIFT) - extra_area;
            area->x2 = x + ((lg_trigo_sin(end_angle_int + 90) * rin) >> LG_TRIGO_SHIFT) + extra_area;
        }
        else if (start_quarter == 3)
        {
            area->x1 = x + ((lg_trigo_sin(start_angle_int + 90) * rin) >> LG_TRIGO_SHIFT) - extra_area;
            area->y1 = y + ((lg_trigo_sin(start_angle_int) * rout) >> LG_TRIGO_SHIFT) - extra_area;

            area->x2 = x + ((lg_trigo_sin(end_angle_int + 90) * rout) >> LG_TRIGO_SHIFT) + extra_area;
            area->y2 = y + ((lg_trigo_sin(end_angle_int) * rin) >> LG_TRIGO_SHIFT) + extra_area;
        }
    }
    else if (start_quarter == 0 && end_quarter == 1)
    {
        area->x1 = x + ((lg_trigo_sin(end_angle_int + 90) * rout) >> LG_TRIGO_SHIFT) - extra_area;
        area->y1 = y + ((LG_MIN(lg_trigo_sin(end_angle_int),
                                lg_trigo_sin(start_angle_int))  * rin) >> LG_TRIGO_SHIFT) - extra_area;
        area->x2 = x + ((lg_trigo_sin(start_angle_int + 90) * rout) >> LG_TRIGO_SHIFT) + extra_area;
        area->y2 = y + rout + extra_area;
    }
    else if (start_quarter == 1 && end_quarter == 2)
    {
        area->x1 = x - rout - extra_area;
        area->y1 = y + ((lg_trigo_sin(end_angle_int) * rout) >> LG_TRIGO_SHIFT) - extra_area;
        area->x2 = x + ((LG_MAX(lg_trigo_sin(start_angle_int + 90),
                                lg_trigo_sin(end_angle_int + 90)) * rin) >> LG_TRIGO_SHIFT) + extra_area;
        area->y2 = y + ((lg_trigo_sin(start_angle_int) * rout) >> LG_TRIGO_SHIFT) + extra_area;
    }
    else if (start_quarter == 2 && end_quarter == 3)
    {
        area->x1 = x + ((lg_trigo_sin(start_angle_int + 90) * rout) >> LG_TRIGO_SHIFT) - extra_area;
        area->y1 = y - rout - extra_area;
        area->x2 = x + ((lg_trigo_sin(end_angle_int + 90) * rout) >> LG_TRIGO_SHIFT) + extra_area;
        area->y2 = y + (LG_MAX(lg_trigo_sin(end_angle_int) * rin,
                               lg_trigo_sin(start_angle_int) * rin) >> LG_TRIGO_SHIFT) + extra_area;
    }
    else if (start_quarter == 3 && end_quarter == 0)
    {
        area->x1 = x + ((LG_MIN(lg_trigo_sin(end_angle_int + 90),
                                lg_trigo_sin(start_angle_int + 90)) * rin) >> LG_TRIGO_SHIFT) - extra_area;
        area->y1 = y + ((lg_trigo_sin(start_angle_int) * rout) >> LG_TRIGO_SHIFT) - extra_area;
        area->x2 = x + rout + extra_area;
        area->y2 = y + ((lg_trigo_sin(end_angle_int) * rout) >> LG_TRIGO_SHIFT) + extra_area;

    }
    else
    {
        area->x1 = x - rout;
        area->y1 = y - rout;
        area->x2 = x + rout;
        area->y2 = y + rout;
    }
}
bool is_in_angle_range_quadrant(float dx, float dy, uint16_t start_angle, uint16_t end_angle)
{
    int quadrant;
    if (dx >= 0)
    {
        quadrant = (dy >= 0) ? 0 : 3;
    }
    else
    {
        quadrant = (dy >= 0) ? 1 : 2;
    }

    uint16_t quadrant_start = quadrant * 90;
    uint16_t quadrant_end = quadrant_start + 90;

    if (end_angle < start_angle)
    {
        //Crossing 0 degrees
        if (!(quadrant_start < end_angle || quadrant_end > start_angle))
        {
            return false;
        }
    }
    else
    {
        // continuous range
        if (quadrant_end < start_angle || quadrant_start > end_angle)
        {
            return false;
        }
    }

    uint16_t angle = lg_atan2(dy, dx);
    return check_degree_arc_range(angle, start_angle, end_angle, false);
}

static inline float compute_coverage(float dist_sq, float outer_radius,
                                     float inner_radius, float aa_width)
{
    float outer_aa_inner_sq = (outer_radius - aa_width) * (outer_radius - aa_width);
    float outer_aa_outer_sq = (outer_radius + aa_width) * (outer_radius + aa_width);
    float inner_aa_inner_sq = (inner_radius - aa_width) * (inner_radius - aa_width);
    float inner_aa_outer_sq = (inner_radius + aa_width) * (inner_radius + aa_width);

    if (dist_sq > outer_aa_inner_sq && dist_sq <= outer_aa_outer_sq)
    {

        float t = (dist_sq - outer_aa_inner_sq) / (outer_aa_outer_sq - outer_aa_inner_sq);
        float t_linear = t * t;
        return 1.0f - t_linear;
    }

    if (inner_radius > 0 && dist_sq >= inner_aa_inner_sq && dist_sq < inner_aa_outer_sq)
    {
        float t = (inner_aa_outer_sq - dist_sq) / (inner_aa_outer_sq - inner_aa_inner_sq);
        float t_linear = t * t;
        return 1.0f - t_linear;
    }

    return 1.0f;
}

typedef struct
{
    bool quadrants[4];  // Mark which quadrants contain arcs
    uint16_t start_quadrant;
    uint16_t end_quadrant;
} ArcQuadrantInfo;

typedef struct
{
    int start_x, end_x;
    int start_y, end_y;
} QuadrantBounds;

// Quadrants involved in pre-calculation of circular arcs
static void compute_arc_quadrants(uint16_t start_angle, uint16_t end_angle, ArcQuadrantInfo *info)
{
    memset(info->quadrants, 0, sizeof(info->quadrants));

    info->start_quadrant = start_angle / 90;
    info->end_quadrant = end_angle / 90;

    // Mark all involved quadrants
    if (end_angle >= start_angle)
    {
        // Not exceeding 0 degrees
        for (int quad = info->start_quadrant; quad <= info->end_quadrant; quad++)
        {
            info->quadrants[quad % 4] = true;
        }
    }
    else
    {
        // Crossing 0 degrees
        for (int quad = info->start_quadrant; quad < 4; quad++)
        {
            info->quadrants[quad] = true;
        }
        for (int quad = 0; quad <= info->end_quadrant; quad++)
        {
            info->quadrants[quad] = true;
        }
    }
}

// Calculate the bounding box for each quadrant
static void compute_quadrant_bounds(ArcQuadrantInfo *info, QuadrantBounds bounds[4],
                                    DrawContext *ctx, float center_x, float center_y,
                                    float outer_radius, float aa_width)
{
    float safety_margin = 1.0f;

    for (int quad = 0; quad < 4; quad++)
    {
        if (!info->quadrants[quad])
        {
            bounds[quad].start_x = 1;
            bounds[quad].end_x = 0;
            continue;
        }

        switch (quad)
        {
        case 0:
            bounds[quad].start_x = (int)(center_x + safety_margin);
            bounds[quad].end_x = (int)(center_x + outer_radius + aa_width - safety_margin);
            bounds[quad].start_y = (int)(center_y);
            bounds[quad].end_y = (int)(center_y + outer_radius + aa_width);
            break;
        case 1:
            bounds[quad].start_x = (int)(center_x - outer_radius - aa_width);
            bounds[quad].end_x = (int)(center_x);
            bounds[quad].start_y = (int)(center_y);
            bounds[quad].end_y = (int)(center_y + outer_radius + aa_width);
            break;
        case 2:
            bounds[quad].start_x = (int)(center_x - outer_radius - aa_width);
            bounds[quad].end_x = (int)(center_x);
            bounds[quad].start_y = (int)(center_y - outer_radius - aa_width);
            bounds[quad].end_y = (int)(center_y - safety_margin);
            break;
        case 3:
            bounds[quad].start_x = (int)(center_x + safety_margin);
            bounds[quad].end_x = (int)(center_x + outer_radius + aa_width);
            bounds[quad].start_y = (int)(center_y - outer_radius - aa_width);
            bounds[quad].end_y = (int)(center_y);
            break;
        }

        bounds[quad].start_x = LG_MAX(bounds[quad].start_x, 0);
        bounds[quad].end_x = LG_MIN(bounds[quad].end_x, ctx->width - 1);
        bounds[quad].start_y = LG_MAX(bounds[quad].start_y, 0);
        bounds[quad].end_y = LG_MIN(bounds[quad].end_y, ctx->height - 1);
    }
}
static void process_pixel_row(uint32_t *pixel_line, int start_x, int end_x,
                              float center_x, float dy, float dy_sq,
                              float outer_sq, float inner_sq,
                              float outer_aa_sq, float inner_aa_sq,
                              float outer_solid_sq, float inner_solid_sq,
                              uint16_t arc_start_deg, uint16_t arc_end_deg,
                              bool needs_angle_check, PixelColor stroke_color,
                              lg_color32_t fg)
{
    // Pre-calculate the dx relevance value of the current row
    for (int x = start_x; x <= end_x; x++)
    {
        float dx = (x + 0.5f) - center_x;
        float dist_sq = dx * dx + dy_sq;

        // Quick boundary check
        if (dist_sq > outer_aa_sq || (inner_sq > 0 && dist_sq < inner_aa_sq))
        {
            continue;
        }

        if (needs_angle_check && !is_in_angle_range_quadrant(dx, dy, arc_start_deg, arc_end_deg))
        {
            continue;
        }

        if (dist_sq <= outer_solid_sq && dist_sq >= inner_solid_sq)
        {
            // Solid area - fill directly
            pixel_line[x] = stroke_color;
            // if ( fg.alpha== 255) {
            //     pixel_line[x] = stroke_color;
            // } else if (fg.alpha > 0) {
            //     uint32_t bg_color = pixel_line[x];
            //     pixel_line[x] = blend_colors_argb8888(bg_color, stroke_color, fg.alpha);
            // }
        }
        else
        {
            // Anti-aliased edge area
            float coverage = compute_coverage(dist_sq,
                                              sqrtf(outer_sq), sqrtf(inner_sq), 2.0f);

            if (coverage > 0.001f)
            {
                uint8_t final_alpha = (uint8_t)(fg.alpha * coverage);
                if (final_alpha == 0) { continue; }

                uint32_t bg_color = pixel_line[x];

                if (final_alpha == 255)
                {
                    pixel_line[x] = stroke_color;
                }
                else
                {
                    uint16_t inv_alpha = 256 - final_alpha;

                    uint8_t bg_r = (bg_color >> 16) & 0xFF;
                    uint8_t bg_g = (bg_color >> 8)  & 0xFF;
                    uint8_t bg_b = bg_color & 0xFF;

                    pixel_line[x] = (0xFFU << 24) |
                                    ((((bg_r * inv_alpha + fg.red * final_alpha) >> 8) & 0xFF) << 16) |
                                    ((((bg_g * inv_alpha + fg.green * final_alpha) >> 8) & 0xFF) << 8) |
                                    (((bg_b * inv_alpha + fg.blue * final_alpha) >> 8) & 0xFF);
                }
            }

        }
    }
}
static void compute_row_x_range(QuadrantBounds *bounds, float center_x, float dy_sq,
                                float outer_aa_sq, float inner_aa_sq,
                                int *start_x, int *end_x)
{
    // Calculate the x-range of the current row within the annulus
    float max_dx_outer = sqrtf(fmaxf(outer_aa_sq - dy_sq, 0));
    float min_dx_inner = (inner_aa_sq > 0) ? sqrtf(fmaxf(inner_aa_sq - dy_sq, 0)) : 0;

    *start_x = LG_MAX(bounds->start_x, (int)(center_x - max_dx_outer));
    *end_x = LG_MIN(bounds->end_x, (int)(center_x + max_dx_outer));

    if (inner_aa_sq > 0 && dy_sq < inner_aa_sq)
    {
        int inner_start = (int)(center_x - min_dx_inner);
        int inner_end = (int)(center_x + min_dx_inner);

        if (inner_start <= *end_x && inner_end >= *start_x)
        {
            if (*start_x >= inner_start && *end_x <= inner_end)
            {
                // Entire row is within inner circle; set invalid range
                *start_x = *end_x + 1;
            }
            else if (*start_x < inner_start)
            {
                // Keep only the left side (before inner circle)
                *end_x = inner_start - 1;
            }
            else if (*end_x > inner_end)
            {
                // Keep only the right side (after inner circle)
                *start_x = inner_end + 1;
            }
            // First case (*start_x < inner_start && *end_x > inner_end) is already correct
            // as the row spans both sides of inner circle
        }
    }
}
static void process_quadrant_by_rows(DrawContext *ctx, int quad, QuadrantBounds *bounds,
                                     float center_x, float center_y,
                                     float outer_sq, float inner_sq,
                                     float outer_aa_sq, float inner_aa_sq,
                                     float outer_solid_sq, float inner_solid_sq,
                                     uint16_t arc_start_deg, uint16_t arc_end_deg,
                                     bool needs_angle_check, PixelColor stroke_color,
                                     lg_color32_t fg)
{
    LG_UNUSED(quad);
    float *dy_values = malloc((bounds->end_y - bounds->start_y + 1) * sizeof(float));
    float *dy_sq_values = malloc((bounds->end_y - bounds->start_y + 1) * sizeof(float));

    for (int y = bounds->start_y; y <= bounds->end_y; y++)
    {
        int idx = y - bounds->start_y;
        dy_values[idx] = (y + 0.5f) - center_y;
        dy_sq_values[idx] = dy_values[idx] * dy_values[idx];
    }

    for (int y = bounds->start_y; y <= bounds->end_y; y++)
    {
        int idx = y - bounds->start_y;
        float dy = dy_values[idx];
        float dy_sq = dy_sq_values[idx];

        uint32_t *pixel_line = (uint32_t *)(ctx->buffer + y * ctx->stride);

        int row_start_x, row_end_x;
        compute_row_x_range(bounds, center_x, dy_sq, outer_aa_sq, inner_aa_sq,
                            &row_start_x, &row_end_x);

        if (row_start_x > row_end_x) { continue; }

        process_pixel_row(pixel_line, row_start_x, row_end_x, center_x, dy, dy_sq,
                          outer_sq, inner_sq, outer_aa_sq, inner_aa_sq,
                          outer_solid_sq, inner_solid_sq,
                          arc_start_deg, arc_end_deg, needs_angle_check,
                          stroke_color, fg);
    }

    free(dy_values);
    free(dy_sq_values);
}
void draw_arc_df_aa(DrawContext *ctx, float center_x, float center_y,
                    float radius, float line_width,
                    float start_angle, float end_angle,
                    PixelColor stroke_color)
{
    if (radius <= 0 || line_width <= 0) { return; }
    uint16_t start_angle_deg = (uint16_t)start_angle;
    uint16_t end_angle_deg = (uint16_t)end_angle;

    while (start_angle_deg > 360) { start_angle_deg -= 360; }
    while (end_angle_deg > 360) { end_angle_deg -= 360; }

    uint16_t arc_start_deg = start_angle_deg;
    uint16_t arc_end_deg = end_angle_deg;
    if (arc_end_deg < arc_start_deg)
    {
        arc_end_deg += 360;
    }

    bool full_circle = (arc_end_deg - arc_start_deg >= 360);
    if (full_circle)
    {
        draw_arc_as_ring(ctx, center_x, center_y, radius, line_width, stroke_color);
        return;
    }

    float outer_radius = radius + line_width / 2;
    float inner_radius = fmaxf(radius - line_width / 2, 0);
    float aa_width = 2.0f;

    float outer_sq = outer_radius * outer_radius;
    float inner_sq = inner_radius * inner_radius;
    float outer_aa_sq = (outer_radius + aa_width) * (outer_radius + aa_width);
    float inner_aa_sq = fmaxf((inner_radius - aa_width) * (inner_radius - aa_width), 0);

    float outer_solid = outer_radius - aa_width;
    float inner_solid = inner_radius + aa_width;
    float outer_solid_sq = outer_solid * outer_solid;
    float inner_solid_sq = inner_solid * inner_solid;

    if (line_width > 1)
    {
        float start_rad = start_angle * M_PI / 180.0f;
        float end_rad = end_angle * M_PI / 180.0f;

        int start_cap_x = center_x + radius * cosf(start_rad);
        int start_cap_y = center_y + radius * sinf(start_rad);
        int end_cap_x = center_x + radius * cosf(end_rad);
        int end_cap_y = center_y + radius * sinf(end_rad);

        fill_circle_df_aa(ctx, start_cap_x, start_cap_y, (line_width + aa_width) / 2, stroke_color);
        fill_circle_df_aa(ctx, end_cap_x, end_cap_y, (line_width + aa_width) / 2, stroke_color);
    }

    ArcQuadrantInfo quadrant_info;
    compute_arc_quadrants(start_angle_deg, end_angle_deg, &quadrant_info);

    QuadrantBounds bounds[4];
    compute_quadrant_bounds(&quadrant_info, bounds, ctx, center_x, center_y, outer_radius, aa_width);

    lg_color32_t fg;
    fg.red   = (stroke_color >> 16) & 0xFF;
    fg.green = (stroke_color >> 8)  & 0xFF;
    fg.blue  = stroke_color & 0xFF;
    fg.alpha = (stroke_color >> 24) & 0xFF;

    uint16_t start_quadrant = start_angle_deg / 90;
    uint16_t end_quadrant = end_angle_deg / 90;

    for (int quad = 0; quad < 4; quad++)
    {
        if (!quadrant_info.quadrants[quad]) { continue; }

        bool needs_angle_check = (start_quadrant == end_quadrant && quad == start_quadrant) ||
                                 (start_quadrant != end_quadrant && (quad == start_quadrant ||
                                                                     quad == end_quadrant));

        process_quadrant_by_rows(ctx, quad, &bounds[quad], center_x, center_y,
                                 outer_sq, inner_sq, outer_aa_sq, inner_aa_sq,
                                 outer_solid_sq, inner_solid_sq,
                                 arc_start_deg, arc_end_deg,
                                 needs_angle_check, stroke_color,
                                 fg);
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
    else
    {
        if (radius <= 0 || line_width <= 0) { return; }

        float outer_radius = radius + line_width / 2;
        float inner_radius = fmaxf(radius - line_width / 2, 0);

        uint16_t start_angle_deg = (uint16_t)start_angle;
        uint16_t end_angle_deg = (uint16_t)end_angle;

        while (start_angle_deg >= 360) { start_angle_deg -= 360; }
        while (end_angle_deg >= 360) { end_angle_deg -= 360; }

        bool is_closed_arc = false;
        if (start_angle_deg == end_angle_deg)
        {
            is_closed_arc = true;
        }
        else
        {
            int angle_diff = (int)end_angle_deg - (int)start_angle_deg;
            if (angle_diff < 0) { angle_diff += 360; }
            is_closed_arc = (angle_diff >= 359);
        }

        uint16_t arc_start_deg = start_angle_deg;
        uint16_t arc_end_deg = end_angle_deg;

        if (arc_end_deg < arc_start_deg && !is_closed_arc)
        {
            arc_end_deg += 360;
        }
        lg_area_t draw_area;
        lg_draw_arc_get_area((int32_t)center_x, (int32_t)center_y, (uint16_t)outer_radius,
                             start_angle_deg, end_angle_deg, (int32_t)line_width, false, &draw_area);

        int start_x = LG_MAX(draw_area.x1, 0);
        int end_x = LG_MIN(draw_area.x2, ctx->width - 1);
        int start_y = LG_MAX(draw_area.y1, 0);
        int end_y = LG_MIN(draw_area.y2, ctx->height - 1);

        for (int py = start_y; py <= end_y; py++)
        {
            for (int px = start_x; px <= end_x; px++)
            {
                int dx = px - (int)center_x;
                int dy = py - (int)center_y;
                float dist_sq = dx * dx + dy * dy;

                if (dist_sq > outer_radius * outer_radius ||
                    dist_sq < inner_radius * inner_radius)
                {
                    continue;
                }

                if (!is_closed_arc)
                {
                    uint16_t angle_deg = lg_atan2(dy, dx);

                    bool in_angle_range = check_degree_arc_range(angle_deg, arc_start_deg, arc_end_deg, is_closed_arc);

                    if (!in_angle_range) { continue; }
                }
                add_pixel_to_context(ctx, px, py, stroke_color);
            }
        }
        if (line_width > 1 && !is_closed_arc)
        {
            float start_rad = start_angle * M_PI / 180.0f;
            float end_rad = end_angle * M_PI / 180.0f;

            float start_cap_x = center_x + radius * cosf(start_rad);
            float start_cap_y = center_y + radius * sinf(start_rad);
            float end_cap_x = center_x + radius * cosf(end_rad);
            float end_cap_y = center_y + radius * sinf(end_rad);

            fill_circle(ctx, start_cap_x, start_cap_y, line_width / 2, stroke_color);
            fill_circle(ctx, end_cap_x, end_cap_y, line_width / 2, stroke_color);
        }
    }
}
float calculate_coverage(float dist_sq, float outer_sq, float inner_sq,
                         float outer_aa_sq, float inner_aa_sq)
{
    if (dist_sq <= outer_sq && dist_sq >= inner_sq)
    {
        return 1.0f;
    }

    if (dist_sq > outer_sq && dist_sq <= outer_aa_sq)
    {
        float ratio = (dist_sq - outer_sq) / (outer_aa_sq - outer_sq);
        return 1.0f - LG_CLAMP(ratio, 0.0f, 1.0f);
    }

    if (dist_sq < inner_sq && dist_sq >= inner_aa_sq)
    {
        float ratio = (inner_sq - dist_sq) / (inner_sq - inner_aa_sq);
        return 1.0f - LG_CLAMP(ratio, 0.0f, 1.0f);
    }

    return 0.0f;
}

void draw_arc_as_ring(DrawContext *ctx, float center_x, float center_y,
                      float arc_radius, float line_width, PixelColor color)
{
    if (line_width <= 0) { return; }

    float outer_radius = arc_radius + line_width / 2;
    float inner_radius = fmaxf(arc_radius - line_width / 2, 0);

    float outer_sq = outer_radius * outer_radius;
    float inner_sq = inner_radius * inner_radius;
    float outer_aa_radius = outer_radius + 0.7f;
    float inner_aa_radius = fmaxf(inner_radius - 0.7f, 0);
    float outer_aa_sq = outer_aa_radius * outer_aa_radius;
    float inner_aa_sq = inner_aa_radius * inner_aa_radius;

    float outer_solid_radius = outer_radius - 0.7f;
    float inner_solid_radius = inner_radius + 0.7f;
    float outer_solid_sq = outer_solid_radius * outer_solid_radius;
    float inner_solid_sq = inner_solid_radius * inner_solid_radius;

    int max_y = (int)ceilf(outer_aa_radius);

    for (int y = 0; y <= max_y; y++)
    {
        float y_sq = y * y;

        int x_start = y;
        int x_end = (int)ceilf(sqrtf(outer_aa_sq - y_sq));

        if (x_start > x_end) { continue; }

        int inner_x_start = 0;
        if (y <= inner_aa_radius)
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

            if (dist_sq <= outer_solid_sq && dist_sq >= inner_solid_sq)
            {
                draw_symmetric_pixels(ctx, center_x, center_y, x, y, color, 1.0f);
            }
            else if (dist_sq <= outer_aa_sq && dist_sq >= inner_aa_sq)
            {
                float coverage = calculate_coverage(dist_sq, outer_sq, inner_sq, outer_aa_sq, inner_aa_sq);
                if (coverage > 0.001f)
                {
                    draw_symmetric_pixels(ctx, center_x, center_y, x, y, color, coverage);
                }
            }
        }
    }
}

void draw_symmetric_pixels(DrawContext *ctx, float cx, float cy, int x, int y,
                           PixelColor color, float coverage)
{
    // Define symmetrical points in 8 quadrants
    int coords[8][2];
    int point_count = 0;

    if (x == 0 && y == 0)
    {
        // Origin: There is only 1 point
        coords[point_count][0] = (int)(cx + 0); coords[point_count][1] = (int)(cy + 0); point_count++;
    }
    else if (y == 0)
    {
        // Plot 4 points on the x-axis (x, 0).
        coords[point_count][0] = (int)(cx + x); coords[point_count][1] = (int)(cy + 0); point_count++;
        coords[point_count][0] = (int)(cx - x); coords[point_count][1] = (int)(cy + 0); point_count++;
        coords[point_count][0] = (int)(cx + 0); coords[point_count][1] = (int)(cy + x); point_count++;
        coords[point_count][0] = (int)(cx + 0); coords[point_count][1] = (int)(cy - x); point_count++;
    }
    else if (x == y)
    {
        // Draw 4 points along the 45-degree diagonal.
        coords[point_count][0] = (int)(cx + x); coords[point_count][1] = (int)(cy + y); point_count++;
        coords[point_count][0] = (int)(cx - x); coords[point_count][1] = (int)(cy + y); point_count++;
        coords[point_count][0] = (int)(cx + x); coords[point_count][1] = (int)(cy - y); point_count++;
        coords[point_count][0] = (int)(cx - x); coords[point_count][1] = (int)(cy - y); point_count++;
    }
    else
    {
        // Standard points: 8 complete symmetrical points
        coords[point_count][0] = (int)(cx + x); coords[point_count][1] = (int)(cy + y); point_count++;
        coords[point_count][0] = (int)(cx - x); coords[point_count][1] = (int)(cy + y); point_count++;
        coords[point_count][0] = (int)(cx + x); coords[point_count][1] = (int)(cy - y); point_count++;
        coords[point_count][0] = (int)(cx - x); coords[point_count][1] = (int)(cy - y); point_count++;
        coords[point_count][0] = (int)(cx + y); coords[point_count][1] = (int)(cy + x); point_count++;
        coords[point_count][0] = (int)(cx - y); coords[point_count][1] = (int)(cy + x); point_count++;
        coords[point_count][0] = (int)(cx + y); coords[point_count][1] = (int)(cy - x); point_count++;
        coords[point_count][0] = (int)(cx - y); coords[point_count][1] = (int)(cy - x); point_count++;
    }

    // Plot all calculated points
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

