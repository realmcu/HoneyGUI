/**
 * @file trvg.c
 * @brief TRVG - Triton Vector Graphics Library Implementation
 *
 * Lightweight C vector graphics library, optimized for MCU and map rendering
 * Only includes ultra-fast drawing API, no anti-aliasing
 *
 * MVE (Helium) acceleration supported for ARM Cortex-M55/M85
 * Compiler: armclang 6.22+ with -march=armv8.1-m.main+dsp+mve.fp+fp.dp
 */

#include "trvg.h"
#include "trvg_mve.h"
#include <string.h>

/* ============================================================================
 * Helper Functions
 * ============================================================================ */

static inline int trvg_mini(int a, int b)
{
    return a < b ? a : b;
}
static inline int trvg_maxi(int a, int b)
{
    return a > b ? a : b;
}
static inline int trvg_absi(int v)
{
    return v < 0 ? -v : v;
}

/* ============================================================================
 * Context Management
 * ============================================================================ */

int trvg_bytes_per_pixel(trvg_pixel_format_t format)
{
    switch (format)
    {
    case TRVG_PIXEL_BGRA8888:
    case TRVG_PIXEL_RGBA8888:
        return 4;
    case TRVG_PIXEL_BGR565:
    case TRVG_PIXEL_RGB565:
        return 2;
    default:
        return 4;
    }
}

bool trvg_init(trvg_context_t *ctx, uint8_t *pixels, int width, int height,
               int stride, trvg_pixel_format_t format)
{
    if (!ctx || !pixels || width <= 0 || height <= 0)
    {
        return false;
    }

    memset(ctx, 0, sizeof(trvg_context_t));

    ctx->pixels = pixels;
    ctx->width = width;
    ctx->height = height;
    ctx->format = format;

    if (stride == 0)
    {
        ctx->stride = width * trvg_bytes_per_pixel(format);
    }
    else
    {
        ctx->stride = stride;
    }
    /* Initialize clipping region */
    trvg_reset_clip(ctx);

    return true;
}

void trvg_reset(trvg_context_t *ctx)
{
    if (!ctx)
    {
        return;
    }

    trvg_reset_clip(ctx);
    trvg_reset_stats(ctx);
}

/* ============================================================================
 * Clipping
 * ============================================================================ */

void trvg_set_clip(trvg_context_t *ctx, int x, int y, int w, int h)
{
    if (!ctx)
    {
        return;
    }
    ctx->clip.x = x;
    ctx->clip.y = y;
    ctx->clip.w = w;
    ctx->clip.h = h;
}

void trvg_reset_clip(trvg_context_t *ctx)
{
    if (!ctx)
    {
        return;
    }
    ctx->clip.x = 0;
    ctx->clip.y = 0;
    ctx->clip.w = ctx->width;
    ctx->clip.h = ctx->height;
}

/* ============================================================================
 * Statistics
 * ============================================================================ */

void trvg_get_stats(const trvg_context_t *ctx, uint32_t *pixels)
{
    if (pixels)
    {
        *pixels = ctx ? ctx->stats_pixels_drawn : 0;
    }
}

void trvg_reset_stats(trvg_context_t *ctx)
{
    if (ctx)
    {
        ctx->stats_pixels_drawn = 0;
    }
}

/* ============================================================================
 * Ultra-fast Drawing API - No anti-aliasing, optimized for MCU
 * ============================================================================ */

/* Fast pixel setting - inline optimized, no bounds check version */
static inline void trvg_pixel_unchecked(uint8_t *row, int x,
                                        trvg_pixel_format_t format,
                                        uint8_t r, uint8_t g, uint8_t b)
{
    switch (format)
    {
    case TRVG_PIXEL_RGBA8888:
        {
            uint8_t *p = row + x * 4;
            p[0] = r;
            p[1] = g;
            p[2] = b;
            p[3] = 255;
            break;
        }
    case TRVG_PIXEL_BGRA8888:
        {
            uint8_t *p = row + x * 4;
            p[0] = b;
            p[1] = g;
            p[2] = r;
            p[3] = 255;
            break;
        }
    case TRVG_PIXEL_RGB565:
        {
            uint16_t *p16 = (uint16_t *)(row + x * 2);
            *p16 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
            break;
        }
    case TRVG_PIXEL_BGR565:
        {
            uint16_t *p16 = (uint16_t *)(row + x * 2);
            *p16 = ((b >> 3) << 11) | ((g >> 2) << 5) | (r >> 3);
            break;
        }
    }
}

void trvg_fast_pixel(trvg_context_t *ctx, int x, int y,
                     uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels)
    {
        return;
    }
    if (x < ctx->clip.x || x >= ctx->clip.x + ctx->clip.w)
    {
        return;
    }
    if (y < ctx->clip.y || y >= ctx->clip.y + ctx->clip.h)
    {
        return;
    }

    uint8_t *row = ctx->pixels + y * ctx->stride;
    trvg_pixel_unchecked(row, x, ctx->format, r, g, b);
    ctx->stats_pixels_drawn++;
}

void trvg_fast_hline(trvg_context_t *ctx, int x0, int x1, int y,
                     uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels)
    {
        return;
    }
    if (y < ctx->clip.y || y >= ctx->clip.y + ctx->clip.h)
    {
        return;
    }

    if (x0 > x1)
    {
        int t = x0;
        x0 = x1;
        x1 = t;
    }

    int clip_x0 = ctx->clip.x;
    int clip_x1 = ctx->clip.x + ctx->clip.w - 1;

    if (x1 < clip_x0 || x0 > clip_x1)
    {
        return;
    }
    if (x0 < clip_x0)
    {
        x0 = clip_x0;
    }
    if (x1 > clip_x1)
    {
        x1 = clip_x1;
    }
    uint8_t *row = ctx->pixels + y * ctx->stride;
    int count = x1 - x0 + 1;

    /* Use MVE optimized fill functions */
    switch (ctx->format)
    {
    case TRVG_PIXEL_RGBA8888:
        trvg_mve_fill_rgba8888(row + x0 * 4, r, g, b, 255, count);
        break;
    case TRVG_PIXEL_BGRA8888:
        trvg_mve_fill_bgra8888(row + x0 * 4, r, g, b, 255, count);
        break;
    case TRVG_PIXEL_RGB565:
        trvg_mve_fill_rgb565((uint16_t *)(row + x0 * 2), r, g, b, count);
        break;
    case TRVG_PIXEL_BGR565:
        trvg_mve_fill_bgr565((uint16_t *)(row + x0 * 2), r, g, b, count);
        break;
    }

    ctx->stats_pixels_drawn += count;
}

void trvg_fast_vline(trvg_context_t *ctx, int x, int y0, int y1,
                     uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels)
    {
        return;
    }
    if (x < ctx->clip.x || x >= ctx->clip.x + ctx->clip.w)
    {
        return;
    }

    if (y0 > y1)
    {
        int t = y0;
        y0 = y1;
        y1 = t;
    }

    int clip_y0 = ctx->clip.y;
    int clip_y1 = ctx->clip.y + ctx->clip.h - 1;

    if (y1 < clip_y0 || y0 > clip_y1)
    {
        return;
    }
    if (y0 < clip_y0)
    {
        y0 = clip_y0;
    }
    if (y1 > clip_y1)
    {
        y1 = clip_y1;
    }

    for (int y = y0; y <= y1; y++)
    {
        uint8_t *row = ctx->pixels + y * ctx->stride;
        trvg_pixel_unchecked(row, x, ctx->format, r, g, b);
    }

    ctx->stats_pixels_drawn += (y1 - y0 + 1);
}

void trvg_fast_line(trvg_context_t *ctx,
                    int x0, int y0, int x1, int y1,
                    uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels)
    {
        return;
    }

    /* Bresenham line algorithm */
    int dx = trvg_absi(x1 - x0);
    int dy = trvg_absi(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    int clip_x0 = ctx->clip.x;
    int clip_y0 = ctx->clip.y;
    int clip_x1 = ctx->clip.x + ctx->clip.w - 1;
    int clip_y1 = ctx->clip.y + ctx->clip.h - 1;

    while (1)
    {
        if (x0 >= clip_x0 && x0 <= clip_x1 && y0 >= clip_y0 && y0 <= clip_y1)
        {
            uint8_t *row = ctx->pixels + y0 * ctx->stride;
            trvg_pixel_unchecked(row, x0, ctx->format, r, g, b);
            ctx->stats_pixels_drawn++;
        }

        if (x0 == x1 && y0 == y1)
        {
            break;
        }

        int e2 = err * 2;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void trvg_fast_thick_line(trvg_context_t *ctx,
                          int x0, int y0, int x1, int y1,
                          int thickness,
                          uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels || thickness <= 0)
    {
        return;
    }

    /* Use simple Bresenham for width 1 lines */
    if (thickness == 1)
    {
        trvg_fast_line(ctx, x0, y0, x1, y1, r, g, b);
        return;
    }

    int half_t = thickness / 2;

    /* Calculate line segment direction */
    int dx = x1 - x0;
    int dy = y1 - y0;
    int abs_dx = trvg_absi(dx);
    int abs_dy = trvg_absi(dy);
    /* Vertical or near-vertical line */
    if (abs_dx == 0 || (abs_dy != 0 && abs_dx < abs_dy / 2))
    {
        /* Draw thick vertical line - using horizontal scanlines */
        if (y0 > y1)
        {
            int t = y0;
            y0 = y1;
            y1 = t;
            t = x0;
            x0 = x1;
            x1 = t;
        }

        for (int y = y0; y <= y1; y++)
        {
            int cx;
            if (y1 != y0)
            {
                cx = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
            }
            else
            {
                cx = x0;
            }
            trvg_fast_hline(ctx, cx - half_t, cx + half_t, y, r, g, b);
        }
        return;
    }
    /* Horizontal or diagonal line - use vertical expansion */
    if (x0 > x1)
    {
        int t = x0;
        x0 = x1;
        x1 = t;
        t = y0;
        y0 = y1;
        y1 = t;
    }

    /* Use improved Bresenham to draw center line with vertical expansion */
    dx = x1 - x0;
    dy = y1 - y0;
    abs_dy = trvg_absi(dy);
    int sy = dy > 0 ? 1 : -1;
    int err = dx / 2;
    int y = y0;

    for (int x = x0; x <= x1; x++)
    {
        trvg_fast_vline(ctx, x, y - half_t, y + half_t, r, g, b);

        err -= abs_dy;
        if (err < 0)
        {
            y += sy;
            err += dx;
        }
    }
}

void trvg_fast_fill_rect(trvg_context_t *ctx,
                         int x, int y, int w, int h,
                         uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels)
    {
        return;
    }

    /* Clip to canvas */
    int x0 = trvg_maxi(x, ctx->clip.x);
    int y0 = trvg_maxi(y, ctx->clip.y);
    int x1 = trvg_mini(x + w, ctx->clip.x + ctx->clip.w);
    int y1 = trvg_mini(y + h, ctx->clip.y + ctx->clip.h);

    if (x0 >= x1 || y0 >= y1)
    {
        return;
    }

    for (int py = y0; py < y1; py++)
    {
        trvg_fast_hline(ctx, x0, x1 - 1, py, r, g, b);
    }
}

void trvg_fast_stroke_rect(trvg_context_t *ctx,
                           int x, int y, int w, int h,
                           uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels || w <= 0 || h <= 0)
    {
        return;
    }

    /* Four edges */
    trvg_fast_hline(ctx, x, x + w - 1, y, r, g, b);           /* Top */
    trvg_fast_hline(ctx, x, x + w - 1, y + h - 1, r, g, b);   /* Bottom */
    trvg_fast_vline(ctx, x, y, y + h - 1, r, g, b);           /* Left */
    trvg_fast_vline(ctx, x + w - 1, y, y + h - 1, r, g, b);   /* Right */
}

void trvg_fast_fill_circle(trvg_context_t *ctx,
                           int cx, int cy, int radius,
                           uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels || radius <= 0)
    {
        return;
    }

    /* Use Midpoint Circle Algorithm */
    int x = radius;
    int y = 0;
    int err = 1 - radius;

    while (x >= y)
    {
        /* Fill horizontal lines at 8 symmetric positions */
        trvg_fast_hline(ctx, cx - x, cx + x, cy + y, r, g, b);
        trvg_fast_hline(ctx, cx - x, cx + x, cy - y, r, g, b);
        trvg_fast_hline(ctx, cx - y, cx + y, cy + x, r, g, b);
        trvg_fast_hline(ctx, cx - y, cx + y, cy - x, r, g, b);

        y++;
        if (err < 0)
        {
            err += 2 * y + 1;
        }
        else
        {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

void trvg_fast_stroke_circle(trvg_context_t *ctx,
                             int cx, int cy, int radius,
                             uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels || radius <= 0)
    {
        return;
    }

    /* Use Midpoint Circle Algorithm */
    int x = radius;
    int y = 0;
    int err = 1 - radius;

    while (x >= y)
    {
        /* Draw 8 symmetric points */
        trvg_fast_pixel(ctx, cx + x, cy + y, r, g, b);
        trvg_fast_pixel(ctx, cx - x, cy + y, r, g, b);
        trvg_fast_pixel(ctx, cx + x, cy - y, r, g, b);
        trvg_fast_pixel(ctx, cx - x, cy - y, r, g, b);
        trvg_fast_pixel(ctx, cx + y, cy + x, r, g, b);
        trvg_fast_pixel(ctx, cx - y, cy + x, r, g, b);
        trvg_fast_pixel(ctx, cx + y, cy - x, r, g, b);
        trvg_fast_pixel(ctx, cx - y, cy - x, r, g, b);

        y++;
        if (err < 0)
        {
            err += 2 * y + 1;
        }
        else
        {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

void trvg_fast_fill_polygon(trvg_context_t *ctx,
                            const int16_t *points, int count,
                            uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels || !points || count < 3)
    {
        return;
    }

    /* Find Y range */
    int y_min = points[1], y_max = points[1];
    for (int i = 1; i < count; i++)
    {
        int y = points[i * 2 + 1];
        if (y < y_min)
        {
            y_min = y;
        }
        if (y > y_max)
        {
            y_max = y;
        }
    }

    int clip_y0 = ctx->clip.y;
    int clip_y1 = ctx->clip.y + ctx->clip.h - 1;

    if (y_min < clip_y0)
    {
        y_min = clip_y0;
    }
    if (y_max > clip_y1)
    {
        y_max = clip_y1;
    }
    /* Scan each row */
    for (int y = y_min; y <= y_max; y++)
    {
        /* Find x coordinates of all edges intersecting with current scanline */
        static int x_intersects[TRVG_MAX_INTERSECTS];
        int x_count = 0;

        for (int i = 0; i < count && x_count < TRVG_MAX_INTERSECTS; i++)
        {
            int j = (i + 1) % count;
            int y0 = points[i * 2 + 1];
            int y1 = points[j * 2 + 1];

            /* Skip horizontal edges */
            if (y0 == y1)
            {
                continue;
            }

            /* Check if edge intersects with scanline */
            if ((y >= y0 && y < y1) || (y >= y1 && y < y0))
            {
                int x0 = points[i * 2];
                int x1 = points[j * 2];
                /* Calculate intersection x coordinate */
                int x = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
                x_intersects[x_count++] = x;
            }
        }

        /* Sort intersections (simple bubble sort, suitable for small arrays) */
        for (int i = 0; i < x_count - 1; i++)
        {
            for (int j = i + 1; j < x_count; j++)
            {
                if (x_intersects[i] > x_intersects[j])
                {
                    int t = x_intersects[i];
                    x_intersects[i] = x_intersects[j];
                    x_intersects[j] = t;
                }
            }
        }

        /* Fill regions between pairs of intersections */
        for (int i = 0; i + 1 < x_count; i += 2)
        {
            trvg_fast_hline(ctx, x_intersects[i], x_intersects[i + 1], y, r, g, b);
        }
    }
}

void trvg_fast_stroke_polygon(trvg_context_t *ctx,
                              const int16_t *points, int count,
                              bool closed,
                              uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels || !points || count < 2)
    {
        return;
    }

    /* Draw all edges */
    for (int i = 0; i < count - 1; i++)
    {
        trvg_fast_line(ctx,
                       points[i * 2], points[i * 2 + 1],
                       points[(i + 1) * 2], points[(i + 1) * 2 + 1],
                       r, g, b);
    }

    /* Close polygon */
    if (closed && count >= 3)
    {
        trvg_fast_line(ctx,
                       points[(count - 1) * 2], points[(count - 1) * 2 + 1],
                       points[0], points[1],
                       r, g, b);
    }
}

void trvg_fast_thick_polygon(trvg_context_t *ctx,
                             const int16_t *points, int count,
                             bool closed, int thickness,
                             uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels || !points || count < 2 || thickness <= 0)
    {
        return;
    }

    /* Draw all edges */
    for (int i = 0; i < count - 1; i++)
    {
        trvg_fast_thick_line(ctx,
                             points[i * 2], points[i * 2 + 1],
                             points[(i + 1) * 2], points[(i + 1) * 2 + 1],
                             thickness,
                             r, g, b);
    }

    /* Close polygon */
    if (closed && count >= 3)
    {
        trvg_fast_thick_line(ctx,
                             points[(count - 1) * 2], points[(count - 1) * 2 + 1],
                             points[0], points[1],
                             thickness,
                             r, g, b);
    }
}

void trvg_fast_nav_path(trvg_context_t *ctx,
                        const int16_t *points, int count,
                        int thickness,
                        uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels || !points || count < 2 || thickness <= 0)
    {
        return;
    }

    /* Circle radius = half of line width */
    int radius = (thickness) / 2;
    if (radius < 1)
    {
        radius = 1;
    }
    int half_t = thickness / 2;

    /*
     * Strategy: first draw all circular join caps, then draw line segments
     * This ensures junction points won't be truncated due to segment endpoint deviations
     */

    /* 1. First draw circular join caps at all vertices */
    for (int i = 0; i < count; i++)
    {
        int cx = points[i * 2];
        int cy = points[i * 2 + 1];
        trvg_fast_fill_circle(ctx, cx, cy, radius, r, g, b);
    }

    /* 2. Then draw all line segments */
    for (int i = 0; i < count - 1; i++)
    {
        int x0 = points[i * 2];
        int y0 = points[i * 2 + 1];
        int x1 = points[(i + 1) * 2];
        int y1 = points[(i + 1) * 2 + 1];

        int dx = x1 - x0;
        int dy = y1 - y0;
        int abs_dx = trvg_absi(dx);
        int abs_dy = trvg_absi(dy);

        /* Skip zero-length segments */
        if (abs_dx == 0 && abs_dy == 0)
        {
            continue;
        }

        /* For completely vertical segments, use rectangle fill */
        if (abs_dx == 0)
        {
            int min_y = (y0 < y1) ? y0 : y1;
            int max_y = (y0 > y1) ? y0 : y1;
            trvg_fast_fill_rect(ctx, x0 - half_t, min_y - half_t,
                                thickness, max_y - min_y + thickness, r, g, b);
        }
        /* For completely horizontal segments, use rectangle fill */
        else if (abs_dy == 0)
        {
            int min_x = (x0 < x1) ? x0 : x1;
            int max_x = (x0 > x1) ? x0 : x1;
            trvg_fast_fill_rect(ctx, min_x - half_t, y0 - half_t,
                                max_x - min_x + thickness, thickness, r, g, b);
        }
        /* For near-vertical diagonal lines (dy > dx), use row-by-row scan to ensure no gaps */
        else if (abs_dy > abs_dx)
        {
            /* Ensure y0 < y1 for easier traversal */
            int sx0 = x0, sy0 = y0, sx1 = x1, sy1 = y1;
            if (sy0 > sy1)
            {
                int t = sx0;
                sx0 = sx1;
                sx1 = t;
                t = sy0;
                sy0 = sy1;
                sy1 = t;
            }
            /* Use fixed-point increment to avoid division in each loop iteration */
            int dx_fixed = ((sx1 - sx0) << 16) / (sy1 - sy0);  /* 16-bit fractional precision */
            int cx_fixed = sx0 << 16;
            for (int y = sy0; y <= sy1; y++)
            {
                int cx = cx_fixed >> 16;
                trvg_fast_hline(ctx, cx - half_t, cx + half_t, y, r, g, b);
                cx_fixed += dx_fixed;
            }
        }
        /* For near-horizontal diagonal lines (dx >= dy), use column-by-column scan to ensure no gaps */
        else
        {
            /* Ensure x0 < x1 for easier traversal */
            int sx0 = x0, sy0 = y0, sx1 = x1, sy1 = y1;
            if (sx0 > sx1)
            {
                int t = sx0;
                sx0 = sx1;
                sx1 = t;
                t = sy0;
                sy0 = sy1;
                sy1 = t;
            }
            /* Use fixed-point increment to avoid division in each loop iteration */
            int dy_fixed = ((sy1 - sy0) << 16) / (sx1 - sx0);  /* 16-bit fractional precision */
            int cy_fixed = sy0 << 16;
            for (int x = sx0; x <= sx1; x++)
            {
                int cy = cy_fixed >> 16;
                trvg_fast_vline(ctx, x, cy - half_t, cy + half_t, r, g, b);
                cy_fixed += dy_fixed;
            }
        }
    }
}

void trvg_fast_fill_triangle(trvg_context_t *ctx,
                             int x0, int y0,
                             int x1, int y1,
                             int x2, int y2,
                             uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels)
    {
        return;
    }

    /* Sort vertices by y: y0 <= y1 <= y2 */
    if (y0 > y1)
    {
        int t = y0;
        y0 = y1;
        y1 = t;
        t = x0;
        x0 = x1;
        x1 = t;
    }
    if (y1 > y2)
    {
        int t = y1;
        y1 = y2;
        y2 = t;
        t = x1;
        x1 = x2;
        x2 = t;
    }
    if (y0 > y1)
    {
        int t = y0;
        y0 = y1;
        y1 = t;
        t = x0;
        x0 = x1;
        x1 = t;
    }

    /* Degenerate triangle */
    if (y0 == y2)
    {
        int min_x = trvg_mini(x0, trvg_mini(x1, x2));
        int max_x = trvg_maxi(x0, trvg_maxi(x1, x2));
        trvg_fast_hline(ctx, min_x, max_x, y0, r, g, b);
        return;
    }

    /* Clipping */
    int clip_y0 = ctx->clip.y;
    int clip_y1 = ctx->clip.y + ctx->clip.h - 1;

    /* Upper half: y0 to y1 */
    if (y0 != y1)
    {
        int dy01 = y1 - y0;
        int dy02 = y2 - y0;

        for (int y = y0; y < y1; y++)
        {
            if (y < clip_y0 || y > clip_y1)
            {
                continue;
            }

            int xa = x0 + (x1 - x0) * (y - y0) / dy01;
            int xb = x0 + (x2 - x0) * (y - y0) / dy02;

            if (xa > xb)
            {
                int t = xa;
                xa = xb;
                xb = t;
            }
            trvg_fast_hline(ctx, xa, xb, y, r, g, b);
        }
    }

    /* Lower half: y1 to y2 */
    if (y1 != y2)
    {
        int dy12 = y2 - y1;
        int dy02 = y2 - y0;

        for (int y = y1; y <= y2; y++)
        {
            if (y < clip_y0 || y > clip_y1)
            {
                continue;
            }

            int xa = x1 + (x2 - x1) * (y - y1) / dy12;
            int xb = x0 + (x2 - x0) * (y - y0) / dy02;

            if (xa > xb)
            {
                int t = xa;
                xa = xb;
                xb = t;
            }
            trvg_fast_hline(ctx, xa, xb, y, r, g, b);
        }
    }
}

void trvg_fast_clear(trvg_context_t *ctx, uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels)
    {
        return;
    }

    int total = ctx->width * ctx->height;

    /* Use MVE optimized fill functions */
    switch (ctx->format)
    {
    case TRVG_PIXEL_RGBA8888:
        trvg_mve_fill_rgba8888(ctx->pixels, r, g, b, 255, total);
        break;
    case TRVG_PIXEL_BGRA8888:
        trvg_mve_fill_bgra8888(ctx->pixels, r, g, b, 255, total);
        break;
    case TRVG_PIXEL_RGB565:
        trvg_mve_fill_rgb565((uint16_t *)ctx->pixels, r, g, b, total);
        break;
    case TRVG_PIXEL_BGR565:
        trvg_mve_fill_bgr565((uint16_t *)ctx->pixels, r, g, b, total);
        break;
    }

    ctx->stats_pixels_drawn += total;
}

/* ============================================================================
 * Lane Arrow Fast Drawing - Pixel-Level Precise Drawing
 * ============================================================================ */

/* Straight arrow ↑ */
static void draw_arrow_straight(trvg_context_t *ctx, int cx, int cy, int size,
                                uint8_t r, uint8_t g, uint8_t b)
{
    int half = size / 2;
    int head_h = size * 40 / 100;
    int head_w = size * 45 / 100;
    int shaft_w = size * 22 / 100;

    int top_y = cy - half;
    int head_base_y = top_y + head_h;
    int bottom_y = cy + half * 60 / 100;

    /* Arrow head */
    trvg_fast_fill_triangle(ctx, cx, top_y,
                            cx - head_w, head_base_y, cx + head_w, head_base_y, r, g, b);
    /* Arrow shaft */
    trvg_fast_fill_rect(ctx, cx - shaft_w / 2, head_base_y,
                        shaft_w, bottom_y - head_base_y, r, g, b);
}

/* Left turn arrow ← */
static void draw_arrow_left(trvg_context_t *ctx, int cx, int cy, int size,
                            uint8_t r, uint8_t g, uint8_t b)
{
    int half = size / 2;
    int shaft_w = size * 18 / 100;
    int head_w = size * 35 / 100;

    int bottom_y = cy + half * 50 / 100;
    int turn_y = cy - half * 15 / 100;
    int arrow_x = cx - half * 55 / 100;
    int tip_x = arrow_x - head_w * 50 / 100;

    /* Vertical shaft */
    trvg_fast_fill_rect(ctx, cx - shaft_w / 2, turn_y, shaft_w, bottom_y - turn_y, r, g, b);
    /* Horizontal part */
    trvg_fast_fill_rect(ctx, arrow_x, turn_y - shaft_w / 2, cx - arrow_x + shaft_w / 2, shaft_w, r, g,
                        b);
    /* Arrow head */
    trvg_fast_fill_triangle(ctx, tip_x, turn_y,
                            arrow_x, turn_y - head_w, arrow_x, turn_y + head_w, r, g, b);
}

/* Right turn arrow → */
static void draw_arrow_right(trvg_context_t *ctx, int cx, int cy, int size,
                             uint8_t r, uint8_t g, uint8_t b)
{
    int half = size / 2;
    int shaft_w = size * 18 / 100;
    int head_w = size * 35 / 100;

    int bottom_y = cy + half * 50 / 100;
    int turn_y = cy - half * 15 / 100;
    int arrow_x = cx + half * 55 / 100;
    int tip_x = arrow_x + head_w * 50 / 100;

    /* Vertical shaft */
    trvg_fast_fill_rect(ctx, cx - shaft_w / 2, turn_y, shaft_w, bottom_y - turn_y, r, g, b);
    /* Horizontal part */
    trvg_fast_fill_rect(ctx, cx - shaft_w / 2, turn_y - shaft_w / 2, arrow_x - cx + shaft_w / 2,
                        shaft_w, r, g, b);
    /* Arrow head */
    trvg_fast_fill_triangle(ctx, tip_x, turn_y,
                            arrow_x, turn_y - head_w, arrow_x, turn_y + head_w, r, g, b);
}

/* Slight left arrow ↖ */
static void draw_arrow_slight_left(trvg_context_t *ctx, int cx, int cy, int size,
                                   uint8_t r, uint8_t g, uint8_t b)
{
    int half = size / 2;

    /* Reference: keep same vertical scale as straight arrow */
    int shaft_w = size * 22 / 100;
    int head_h = size * 40 / 100;
    int head_leg = size * 45 / 100;

    int top_y = cy - half;
    int bottom_y = cy + half * 60 / 100; /* Arrow shaft bottom aligned with straight arrow */

    /* Triangle tip offset (more like ↖) */
    int tip_offset_x = trvg_maxi(1, size * 10 / 100);
    int tip_offset_y = trvg_maxi(1, size * 4 / 100);

    /* Shaft position: offset right; Triangle tip: from shaft left edge toward upper left */
    int shaft_x = cx + half * 10 / 100;

    /* Triangle: right angle at tip (upper left), two legs toward right/down */
    int tip_x = (shaft_x - shaft_w / 2) - tip_offset_x;
    int tip_y = top_y - tip_offset_y;

    /* Triangle base edge (horizontal) midpoint: for "inserting" shaft */
    int base_mid_x = tip_x + head_leg / 2;
    int base_y = tip_y; /* Y of horizontal base edge */

    /* Let shaft top insert into triangle base middle, slightly higher */
    /* Too much insertion will "poke through", keep it shallow */
    int insert_h = trvg_maxi(1, size * 2 / 100);
    insert_h = 0;
    int shaft_top_y = base_y - insert_h;

    trvg_fast_fill_rect(ctx,
                        base_mid_x - shaft_w / 2 + trvg_maxi(1, size * 5 / 100),
                        shaft_top_y,
                        shaft_w,
                        bottom_y - shaft_top_y,
                        r, g, b);

    trvg_fast_fill_triangle(ctx,
                            tip_x, tip_y,
                            tip_x + head_leg, tip_y,
                            tip_x, tip_y + head_h,
                            r, g, b);
}

/* Slight right arrow ↗ */
static void draw_arrow_slight_right(trvg_context_t *ctx, int cx, int cy, int size,
                                    uint8_t r, uint8_t g, uint8_t b)
{
    int half = size / 2;

    int shaft_w = size * 22 / 100;
    int head_h = size * 40 / 100;
    int head_leg = size * 45 / 100;

    int top_y = cy - half;
    int bottom_y = cy + half * 60 / 100;

    int tip_offset_x = trvg_maxi(1, size * 10 / 100);
    int tip_offset_y = trvg_maxi(1, size * 4 / 100);

    int shaft_x = cx - half * 10 / 100;

    /* Triangle: right angle at tip (upper right), two legs toward left/down */
    int tip_x = (shaft_x + shaft_w / 2) + tip_offset_x;
    int tip_y = top_y - tip_offset_y;

    /* Triangle base edge (horizontal) midpoint */
    int base_mid_x = tip_x - head_leg / 2;
    int base_y = tip_y;

    /* Let shaft top insert into triangle base middle, slightly higher */
    /* Too much insertion will "poke through", keep it shallow */
    int insert_h = trvg_maxi(1, size * 3 / 100);
    int shaft_top_y = base_y - insert_h;

    trvg_fast_fill_rect(ctx,
                        base_mid_x - shaft_w / 2,
                        shaft_top_y,
                        shaft_w,
                        bottom_y - shaft_top_y,
                        r, g, b);

    trvg_fast_fill_triangle(ctx,
                            tip_x, tip_y,
                            tip_x - head_leg, tip_y,
                            tip_x, tip_y + head_h,
                            r, g, b);
}

/* Left U-turn arrow */
static void draw_arrow_uturn_left(trvg_context_t *ctx, int cx, int cy, int size,
                                  uint8_t r, uint8_t g, uint8_t b)
{
    int half = size / 2;
    int shaft_w = size * 14 / 100;
    int head_w = size * 28 / 100;
    int arc_w = half * 55 / 100;

    int right_x = cx + arc_w / 2;
    int left_x = cx - arc_w / 2;
    int top_y = cy - half * 35 / 100;
    int right_bottom_y = cy + half * 30 / 100;
    int left_bottom_y = cy + half * 55 / 100;
    int arrow_base_y = left_bottom_y - head_w * 50 / 100;

    /* Right vertical shaft */
    trvg_fast_fill_rect(ctx, right_x - shaft_w / 2, top_y, shaft_w, right_bottom_y - top_y, r, g, b);
    /* Top horizontal shaft */
    trvg_fast_fill_rect(ctx, left_x - shaft_w / 2, top_y - shaft_w / 2, arc_w + shaft_w, shaft_w, r, g,
                        b);
    /* Left vertical shaft */
    trvg_fast_fill_rect(ctx, left_x - shaft_w / 2, top_y, shaft_w, arrow_base_y - top_y, r, g, b);
    /* Arrow head */
    trvg_fast_fill_triangle(ctx, left_x, left_bottom_y,
                            left_x - head_w, arrow_base_y, left_x + head_w, arrow_base_y, r, g, b);
}

/* Right U-turn arrow */
static void draw_arrow_uturn_right(trvg_context_t *ctx, int cx, int cy, int size,
                                   uint8_t r, uint8_t g, uint8_t b)
{
    int half = size / 2;
    int shaft_w = size * 14 / 100;
    int head_w = size * 28 / 100;
    int arc_w = half * 55 / 100;

    int left_x = cx - arc_w / 2;
    int right_x = cx + arc_w / 2;
    int top_y = cy - half * 35 / 100;
    int left_bottom_y = cy + half * 30 / 100;
    int right_bottom_y = cy + half * 55 / 100;
    int arrow_base_y = right_bottom_y - head_w * 50 / 100;

    /* Left vertical shaft */
    trvg_fast_fill_rect(ctx, left_x - shaft_w / 2, top_y, shaft_w, left_bottom_y - top_y, r, g, b);
    /* Top horizontal shaft */
    trvg_fast_fill_rect(ctx, left_x - shaft_w / 2, top_y - shaft_w / 2, arc_w + shaft_w, shaft_w, r, g,
                        b);
    /* Right vertical shaft */
    trvg_fast_fill_rect(ctx, right_x - shaft_w / 2, top_y, shaft_w, arrow_base_y - top_y, r, g, b);
    /* Arrow head */
    trvg_fast_fill_triangle(ctx, right_x, right_bottom_y,
                            right_x - head_w, arrow_base_y, right_x + head_w, arrow_base_y, r, g, b);
}

/* Sharp left arrow (acute left turn) ↙ */
static void draw_arrow_sharp_left(trvg_context_t *ctx, int cx, int cy, int size,
                                  uint8_t r, uint8_t g, uint8_t b)
{
    int half = size / 2;
    int shaft_w = size * 20 / 100;
    int head_w = size * 40 / 100;

    int top_y = cy - half * 45 / 100;
    int bend_y = cy + half * 5 / 100;
    int tip_x = cx - half * 40 / 100;
    int tip_y = cy + half * 65 / 100;

    /* Upper vertical shaft */
    trvg_fast_fill_rect(ctx, cx - shaft_w / 2, top_y, shaft_w, bend_y - top_y, r, g, b);
    /* Diagonal shaft - toward lower left */
    trvg_fast_thick_line(ctx, cx, bend_y, tip_x, tip_y - head_w * 60 / 100, shaft_w, r, g, b);
    /* Arrow head - lower left direction */
    trvg_fast_fill_triangle(ctx, tip_x, tip_y,
                            tip_x + head_w * 80 / 100, tip_y - head_w * 50 / 100,
                            tip_x - head_w * 20 / 100, tip_y - head_w * 90 / 100, r, g, b);
}

/* Sharp right arrow (acute right turn) ↘ */
static void draw_arrow_sharp_right(trvg_context_t *ctx, int cx, int cy, int size,
                                   uint8_t r, uint8_t g, uint8_t b)
{
    int half = size / 2;
    int shaft_w = size * 20 / 100;
    int head_w = size * 40 / 100;

    int top_y = cy - half * 45 / 100;
    int bend_y = cy + half * 5 / 100;
    int tip_x = cx + half * 40 / 100;
    int tip_y = cy + half * 65 / 100;

    /* Upper vertical shaft */
    trvg_fast_fill_rect(ctx, cx - shaft_w / 2, top_y, shaft_w, bend_y - top_y, r, g, b);
    /* Diagonal shaft - toward lower right */
    trvg_fast_thick_line(ctx, cx, bend_y, tip_x, tip_y - head_w * 60 / 100, shaft_w, r, g, b);
    /* Arrow head - lower right direction */
    trvg_fast_fill_triangle(ctx, tip_x, tip_y,
                            tip_x - head_w * 80 / 100, tip_y - head_w * 50 / 100,
                            tip_x + head_w * 20 / 100, tip_y - head_w * 90 / 100, r, g, b);
}

/* Public API: draw single direction arrow */
void trvg_fast_lane_arrow(trvg_context_t *ctx, trvg_arrow_type_t type,
                          int cx, int cy, int size, uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels || size < 8)
    {
        return;
    }

    switch (type)
    {
    case TRVG_ARROW_STRAIGHT:
        draw_arrow_straight(ctx, cx, cy, size, r, g, b);
        break;
    case TRVG_ARROW_LEFT:
        draw_arrow_left(ctx, cx, cy, size, r, g, b);
        break;
    case TRVG_ARROW_RIGHT:
        draw_arrow_right(ctx, cx, cy, size, r, g, b);
        break;
    case TRVG_ARROW_SLIGHT_LEFT:
        draw_arrow_slight_left(ctx, cx, cy, size, r, g, b);
        break;
    case TRVG_ARROW_SLIGHT_RIGHT:
        draw_arrow_slight_right(ctx, cx, cy, size, r, g, b);
        break;
    case TRVG_ARROW_SHARP_LEFT:
        draw_arrow_sharp_left(ctx, cx, cy, size, r, g, b);
        break;
    case TRVG_ARROW_SHARP_RIGHT:
        draw_arrow_sharp_right(ctx, cx, cy, size, r, g, b);
        break;
    case TRVG_ARROW_UTURN_LEFT:
        draw_arrow_uturn_left(ctx, cx, cy, size, r, g, b);
        break;
    case TRVG_ARROW_UTURN_RIGHT:
        draw_arrow_uturn_right(ctx, cx, cy, size, r, g, b);
        break;
    default:
        draw_arrow_straight(ctx, cx, cy, size, r, g, b);
        break;
    }
}

/* ============================================================================
 * Combo Arrow Drawing
 * ============================================================================ */

/* Straight + Right turn combo */
static void draw_combo_straight_right(trvg_context_t *ctx, int cx, int cy, int size,
                                      uint8_t r, uint8_t g, uint8_t b)
{
    int half = size / 2;
    int shaft_w = size * 16 / 100;
    int head_w = size * 30 / 100;
    int head_h = size * 26 / 100;

    int bottom_y = cy + half * 45 / 100;
    int fork_y = cy - half * 5 / 100;
    int straight_top_y = cy - half * 72 / 100;
    int right_tip_x = cx + half * 52 / 100;
    int turn_y = fork_y - half * 12 / 100;

    /* Shared lower shaft */
    trvg_fast_fill_rect(ctx, cx - shaft_w / 2, fork_y, shaft_w, bottom_y - fork_y, r, g, b);
    /* Straight shaft */
    int straight_x = cx - half * 10 / 100;
    trvg_fast_fill_rect(ctx, straight_x - shaft_w / 2, straight_top_y + head_h,
                        shaft_w, fork_y - straight_top_y - head_h, r, g, b);
    /* Straight arrow head */
    trvg_fast_fill_triangle(ctx, straight_x, straight_top_y,
                            straight_x - head_w, straight_top_y + head_h, straight_x + head_w, straight_top_y + head_h, r, g,
                            b);
    /* Right turn horizontal part */
    int right_base_x = right_tip_x - head_w * 45 / 100;
    trvg_fast_fill_rect(ctx, cx, turn_y - shaft_w / 2, right_base_x - cx, shaft_w, r, g, b);
    /* Right turn arrow head */
    trvg_fast_fill_triangle(ctx, right_tip_x, turn_y,
                            right_base_x, turn_y - head_w, right_base_x, turn_y + head_w, r, g, b);
}

/* Straight + Left turn combo */
static void draw_combo_straight_left(trvg_context_t *ctx, int cx, int cy, int size,
                                     uint8_t r, uint8_t g, uint8_t b)
{
    int half = size / 2;
    int shaft_w = size * 16 / 100;
    int head_w = size * 30 / 100;
    int head_h = size * 26 / 100;

    int bottom_y = cy + half * 45 / 100;
    int fork_y = cy - half * 5 / 100;
    int straight_top_y = cy - half * 72 / 100;
    int left_tip_x = cx - half * 52 / 100;
    int turn_y = fork_y - half * 12 / 100;

    /* Shared lower shaft */
    trvg_fast_fill_rect(ctx, cx - shaft_w / 2, fork_y, shaft_w, bottom_y - fork_y, r, g, b);
    /* Straight shaft */
    int straight_x = cx + half * 10 / 100;
    trvg_fast_fill_rect(ctx, straight_x - shaft_w / 2, straight_top_y + head_h,
                        shaft_w, fork_y - straight_top_y - head_h, r, g, b);
    /* Straight arrow head */
    trvg_fast_fill_triangle(ctx, straight_x, straight_top_y,
                            straight_x - head_w, straight_top_y + head_h, straight_x + head_w, straight_top_y + head_h, r, g,
                            b);
    /* Left turn horizontal part */
    int left_base_x = left_tip_x + head_w * 45 / 100;
    trvg_fast_fill_rect(ctx, left_base_x, turn_y - shaft_w / 2, cx - left_base_x, shaft_w, r, g, b);
    /* Left turn arrow head */
    trvg_fast_fill_triangle(ctx, left_tip_x, turn_y,
                            left_base_x, turn_y - head_w, left_base_x, turn_y + head_w, r, g, b);
}

/* Left turn + Right turn combo */
static void draw_combo_left_right(trvg_context_t *ctx, int cx, int cy, int size,
                                  uint8_t r, uint8_t g, uint8_t b)
{
    int half = size / 2;
    int shaft_w = size * 16 / 100;
    int head_w = size * 28 / 100;

    int bottom_y = cy + half * 45 / 100;
    int fork_y = cy - half * 10 / 100;
    int turn_y = fork_y - half * 10 / 100;
    int left_tip_x = cx - half * 52 / 100;
    int right_tip_x = cx + half * 52 / 100;
    int left_base_x = left_tip_x + head_w * 45 / 100;
    int right_base_x = right_tip_x - head_w * 45 / 100;

    /* Lower shaft */
    trvg_fast_fill_rect(ctx, cx - shaft_w / 2, fork_y, shaft_w, bottom_y - fork_y, r, g, b);
    /* Horizontal shaft */
    trvg_fast_fill_rect(ctx, left_base_x, turn_y - shaft_w / 2, right_base_x - left_base_x, shaft_w, r,
                        g, b);
    /* Left turn arrow head */
    trvg_fast_fill_triangle(ctx, left_tip_x, turn_y,
                            left_base_x, turn_y - head_w, left_base_x, turn_y + head_w, r, g, b);
    /* Right turn arrow head */
    trvg_fast_fill_triangle(ctx, right_tip_x, turn_y,
                            right_base_x, turn_y - head_w, right_base_x, turn_y + head_w, r, g, b);
}

/* Straight + Left turn + Right turn combo */
static void draw_combo_straight_left_right(trvg_context_t *ctx, int cx, int cy, int size,
                                           uint8_t r, uint8_t g, uint8_t b)
{
    int half = size / 2;
    int shaft_w = size * 14 / 100;
    int head_w = size * 24 / 100;
    int head_h = size * 22 / 100;

    int bottom_y = cy + half * 45 / 100;
    int fork_y = cy - half * 5 / 100;
    int straight_top_y = cy - half * 72 / 100;
    int turn_y = fork_y - half * 10 / 100;
    int left_tip_x = cx - half * 50 / 100;
    int right_tip_x = cx + half * 50 / 100;
    int left_base_x = left_tip_x + head_w * 45 / 100;
    int right_base_x = right_tip_x - head_w * 45 / 100;

    /* Lower shaft */
    trvg_fast_fill_rect(ctx, cx - shaft_w / 2, fork_y, shaft_w, bottom_y - fork_y, r, g, b);
    /* Straight shaft */
    trvg_fast_fill_rect(ctx, cx - shaft_w / 2, straight_top_y + head_h, shaft_w,
                        fork_y - straight_top_y - head_h, r, g, b);
    /* Straight arrow head */
    trvg_fast_fill_triangle(ctx, cx, straight_top_y,
                            cx - head_w, straight_top_y + head_h, cx + head_w, straight_top_y + head_h, r, g, b);
    /* Horizontal shaft */
    trvg_fast_fill_rect(ctx, left_base_x, turn_y - shaft_w / 2, right_base_x - left_base_x, shaft_w, r,
                        g, b);
    /* Left turn arrow head */
    trvg_fast_fill_triangle(ctx, left_tip_x, turn_y,
                            left_base_x, turn_y - head_w, left_base_x, turn_y + head_w, r, g, b);
    /* Right turn arrow head */
    trvg_fast_fill_triangle(ctx, right_tip_x, turn_y,
                            right_base_x, turn_y - head_w, right_base_x, turn_y + head_w, r, g, b);
}

/* Straight + Slight left combo ↑↖ */
static void draw_combo_straight_slight_left(trvg_context_t *ctx, int cx, int cy, int size,
                                            uint8_t r, uint8_t g, uint8_t b)
{
    int half = size / 2;
    int shaft_w = size * 16 / 100;
    int head_w = size * 30 / 100;
    int head_h = size * 26 / 100;

    int bottom_y = cy + half * 45 / 100;
    int fork_y = cy - half * 5 / 100;
    int straight_top_y = cy - half * 72 / 100;

    /* Slight left arrow parameters */
    int slight_tip_x = cx - half * 35 / 100;
    int slight_tip_y = cy - half * 55 / 100;

    /* Shared lower shaft */
    trvg_fast_fill_rect(ctx, cx - shaft_w / 2, fork_y, shaft_w, bottom_y - fork_y, r, g, b);

    /* Straight shaft (slightly right) */
    int straight_x = cx + half * 8 / 100;
    trvg_fast_fill_rect(ctx, straight_x - shaft_w / 2, straight_top_y + head_h,
                        shaft_w, fork_y - straight_top_y - head_h, r, g, b);
    /* Straight arrow head */
    trvg_fast_fill_triangle(ctx, straight_x, straight_top_y,
                            straight_x - head_w, straight_top_y + head_h,
                            straight_x + head_w, straight_top_y + head_h, r, g, b);

    /* Slight left diagonal shaft */
    trvg_fast_thick_line(ctx, cx, fork_y, slight_tip_x, slight_tip_y + head_w * 50 / 100, shaft_w, r, g,
                         b);
    /* Slight left arrow head */
    trvg_fast_fill_triangle(ctx, slight_tip_x, slight_tip_y,
                            slight_tip_x + head_w * 80 / 100, slight_tip_y + head_w * 50 / 100,
                            slight_tip_x - head_w * 20 / 100, slight_tip_y + head_w * 90 / 100, r, g, b);
}

/* Straight + Slight right combo ↑↗ */
static void draw_combo_straight_slight_right(trvg_context_t *ctx, int cx, int cy, int size,
                                             uint8_t r, uint8_t g, uint8_t b)
{
    int half = size / 2;
    int shaft_w = size * 16 / 100;
    int head_w = size * 30 / 100;
    int head_h = size * 26 / 100;

    int bottom_y = cy + half * 45 / 100;
    int fork_y = cy - half * 5 / 100;
    int straight_top_y = cy - half * 72 / 100;

    /* Slight right arrow parameters */
    int slight_tip_x = cx + half * 35 / 100;
    int slight_tip_y = cy - half * 55 / 100;

    /* Shared lower shaft */
    trvg_fast_fill_rect(ctx, cx - shaft_w / 2, fork_y, shaft_w, bottom_y - fork_y, r, g, b);

    /* Straight shaft (slightly left) */
    int straight_x = cx - half * 8 / 100;
    trvg_fast_fill_rect(ctx, straight_x - shaft_w / 2, straight_top_y + head_h,
                        shaft_w, fork_y - straight_top_y - head_h, r, g, b);
    /* Straight arrow head */
    trvg_fast_fill_triangle(ctx, straight_x, straight_top_y,
                            straight_x - head_w, straight_top_y + head_h,
                            straight_x + head_w, straight_top_y + head_h, r, g, b);

    /* Slight right diagonal shaft */
    trvg_fast_thick_line(ctx, cx, fork_y, slight_tip_x, slight_tip_y + head_w * 50 / 100, shaft_w, r, g,
                         b);
    /* Slight right arrow head */
    trvg_fast_fill_triangle(ctx, slight_tip_x, slight_tip_y,
                            slight_tip_x - head_w * 80 / 100, slight_tip_y + head_w * 50 / 100,
                            slight_tip_x + head_w * 20 / 100, slight_tip_y + head_w * 90 / 100, r, g, b);
}

/* Public API: draw combo arrow */
void trvg_fast_combo_arrow(trvg_context_t *ctx, trvg_combo_arrow_type_t type,
                           int cx, int cy, int size, uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels || size < 8)
    {
        return;
    }

    switch (type)
    {
    case TRVG_COMBO_STRAIGHT_LEFT:
        draw_combo_straight_left(ctx, cx, cy, size, r, g, b);
        break;
    case TRVG_COMBO_STRAIGHT_RIGHT:
        draw_combo_straight_right(ctx, cx, cy, size, r, g, b);
        break;
    case TRVG_COMBO_LEFT_RIGHT:
        draw_combo_left_right(ctx, cx, cy, size, r, g, b);
        break;
    case TRVG_COMBO_STRAIGHT_LEFT_RIGHT:
        draw_combo_straight_left_right(ctx, cx, cy, size, r, g, b);
        break;
    case TRVG_COMBO_STRAIGHT_SLIGHT_LEFT:
        draw_combo_straight_slight_left(ctx, cx, cy, size, r, g, b);
        break;
    case TRVG_COMBO_STRAIGHT_SLIGHT_RIGHT:
        draw_combo_straight_slight_right(ctx, cx, cy, size, r, g, b);
        break;
    default:
        draw_arrow_straight(ctx, cx, cy, size, r, g, b);
        break;
    }
}

/* ============================================================================
 * GPS Marker Drawing Implementation
 * ============================================================================ */

/**
 * @brief Draw GPS marker (Google Maps style droplet shape)
 *
 * The marker consists of the following parts:
 * - Upper part: filled circle (radius is size * 0.35)
 * - Lower part: triangle tip pointing to (cx, cy)
 * - Inner: white small circle
 */
void trvg_fast_gps_marker(trvg_context_t *ctx,
                          int cx, int cy, int size,
                          uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels || size < 8)
    {
        return;
    }

    /* Calculate sizes of each part */
    /* Circle radius: half of marker width, about 35% of total height */
    int circle_radius = size * 35 / 100;
    if (circle_radius < 4)
    {
        circle_radius = 4;
    }

    /* Circle center position: above the tip, about size * 0.55 from the tip */
    int circle_cy = cy - size * 55 / 100;
    int circle_cx = cx;

    /* Triangle tip vertex */
    int tip_y = cy;  /* Tip pointing to cy */

    /* Two upper vertices of the triangle (tangent to the circle) */
    /* Offset down a bit from the circle center to make the connection smooth */
    int tri_top_y = circle_cy + circle_radius * 50 / 100;
    int tri_half_width = circle_radius * 70 / 100;

    /* 1. Draw upper part circle */
    trvg_fast_fill_circle(ctx, circle_cx, circle_cy, circle_radius, r, g, b);

    /* 2. Draw lower part triangle (tip pointing down) */
    trvg_fast_fill_triangle(ctx,
                            cx, tip_y,                          /* Bottom tip */
                            cx - tri_half_width, tri_top_y,     /* Upper left */
                            cx + tri_half_width, tri_top_y,     /* Upper right */
                            r, g, b);

    /* 3. Fill the gap between the circle and triangle (connect with a rectangle) */
    int fill_top = circle_cy;
    int fill_bottom = tri_top_y;
    int fill_left = cx - tri_half_width;
    int fill_right = cx + tri_half_width;
    if (fill_bottom > fill_top)
    {
        trvg_fast_fill_rect(ctx, fill_left, fill_top,
                            fill_right - fill_left, fill_bottom - fill_top,
                            r, g, b);
    }

    /* 4. Draw inner white small circle (highlight effect) */
    int inner_radius = circle_radius * 40 / 100;
    if (inner_radius < 2)
    {
        inner_radius = 2;
    }
    /* White small circle at the circle center */
    trvg_fast_fill_circle(ctx, circle_cx, circle_cy, inner_radius, 255, 255, 255);
}

/**
 * @brief Draw GPS marker with heading
 *
 * Draw a sector direction indicator around the standard GPS marker
 */
void trvg_fast_gps_marker_with_heading(trvg_context_t *ctx,
                                       int cx, int cy, int size,
                                       float heading,
                                       uint8_t r, uint8_t g, uint8_t b)
{
    if (!ctx || !ctx->pixels || size < 8)
    {
        return;
    }

    /* First draw the direction sector (light color, below the marker) */
    int arc_radius = size * 80 / 100;
    int circle_cy = cy - size * 55 / 100;

    /* Convert angle to radians (0 = North = up) */
    /* heading: 0=North, 90=East, 180=South, 270=West */
    /* Screen coordinate system: 0 degrees at right (3 o'clock), clockwise */
    /* Need to convert: screen_angle = 90 - heading (convert to math coordinate system) */
    float pi = 3.14159265f;
    float angle_rad = (90.0f - heading) * pi / 180.0f;

    /* Draw a direction triangle as heading indicator */
    int arrow_len = arc_radius;
    int arrow_width = size * 25 / 100;

    /* Calculate three vertices of the arrow */
    /* Tip: in the heading direction */
    float cos_a = (float)__builtin_cos(angle_rad);
    float sin_a = (float)__builtin_sin(angle_rad);

    int tip_x = circle_cy + (int)(arrow_len * cos_a);  /* Use circle_cy as reference */
    int tip_y_arrow = circle_cy - (int)(arrow_len * sin_a);  /* Screen Y axis down */

    /* Correction: use circle center as starting point */
    tip_x = cx + (int)(arrow_len * cos_a);
    tip_y_arrow = circle_cy - (int)(arrow_len * sin_a);

    /* Two side vertices (perpendicular to heading) */
    float perp_angle = angle_rad + pi / 2.0f;
    float cos_p = (float)__builtin_cos(perp_angle);
    float sin_p = (float)__builtin_sin(perp_angle);

    int side1_x = cx + (int)(arrow_width * cos_p);
    int side1_y = circle_cy - (int)(arrow_width * sin_p);
    int side2_x = cx - (int)(arrow_width * cos_p);
    int side2_y = circle_cy + (int)(arrow_width * sin_p);

    /* Draw the direction indicator triangle with lighter color */
    /* Mix color to make it lighter */
    uint8_t light_r = r + (255 - r) * 50 / 100;
    uint8_t light_g = g + (255 - g) * 50 / 100;
    uint8_t light_b = b + (255 - b) * 50 / 100;

    trvg_fast_fill_triangle(ctx, tip_x, tip_y_arrow,
                            side1_x, side1_y,
                            side2_x, side2_y,
                            light_r, light_g, light_b);

    /* Draw the standard GPS marker (on top of the direction indicator) */
    trvg_fast_gps_marker(ctx, cx, cy, size, r, g, b);
}
