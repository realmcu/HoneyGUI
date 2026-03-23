/**
 * @file trvg.h
 * @brief TRVG - Triton Vector Graphics Library
 *
 * Lightweight C vector graphics library, optimized for MCU and map rendering
 * - Supports BGRA/BGR565 color formats
 * - Static memory allocation, avoids memory leaks
 * - No anti-aliasing, ultra-fast rendering
 * - Optimized for drawing large numbers of elements
 */

#ifndef TRVG_H
#define TRVG_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Configuration Macros
 * ============================================================================ */

/* Scanline intersection buffer size (increased for complex polygons like mountains) */
#ifndef TRVG_MAX_INTERSECTS
#define TRVG_MAX_INTERSECTS         1024
#endif

/* ============================================================================
 * Type Definitions
 * ============================================================================ */

/* Pixel format */
typedef enum
{
    TRVG_PIXEL_BGRA8888 = 0,    /* 32-bit BGRA (8 bits per channel) */
    TRVG_PIXEL_RGBA8888,        /* 32-bit RGBA (8 bits per channel) */
    TRVG_PIXEL_BGR565,          /* 16-bit BGR565 */
    TRVG_PIXEL_RGB565           /* 16-bit RGB565 */
} trvg_pixel_format_t;

/* Color structure (uses RGBA format internally) */
typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} trvg_color_t;

/* Rectangle region */
typedef struct
{
    int x;
    int y;
    int w;
    int h;
} trvg_rect_t;

/* Context structure - simplified version */
typedef struct
{
    /* Frame buffer */
    uint8_t *pixels;
    int width;
    int height;
    int stride;             /* Bytes per row */
    trvg_pixel_format_t format;

    /* Clipping region */
    trvg_rect_t clip;

    /* Statistics */
    uint32_t stats_pixels_drawn;
} trvg_context_t;

/* ============================================================================
 * Context Management
 * ============================================================================ */

/**
 * @brief Initialize context
 * @param ctx Context pointer
 * @param pixels Pixel buffer
 * @param width Width
 * @param height Height
 * @param stride Bytes per row (0 for auto-calculate)
 * @param format Pixel format
 * @return Returns true on success
 */
bool trvg_init(trvg_context_t *ctx, uint8_t *pixels, int width, int height,
               int stride, trvg_pixel_format_t format);

/**
 * @brief Reset context state
 */
void trvg_reset(trvg_context_t *ctx);

/**
 * @brief Get bytes per pixel
 */
int trvg_bytes_per_pixel(trvg_pixel_format_t format);

/* ============================================================================
 * Color Operations
 * ============================================================================ */

/**
 * @brief Create color
 */
static inline trvg_color_t trvg_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    trvg_color_t c = {r, g, b, a};
    return c;
}

static inline trvg_color_t trvg_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return trvg_rgba(r, g, b, 255);
}

/**
 * @brief Create color from 32-bit value (0xRRGGBBAA)
 */
static inline trvg_color_t trvg_color_from_u32(uint32_t c)
{
    return trvg_rgba((c >> 24) & 0xFF, (c >> 16) & 0xFF,
                     (c >> 8) & 0xFF, c & 0xFF);
}

/**
 * @brief Convert color to 32-bit value
 */
static inline uint32_t trvg_color_to_u32(trvg_color_t c)
{
    return ((uint32_t)c.r << 24) | ((uint32_t)c.g << 16) |
           ((uint32_t)c.b << 8) | c.a;
}

/* ============================================================================
 * Clipping
 * ============================================================================ */

/**
 * @brief Set clipping region
 */
void trvg_set_clip(trvg_context_t *ctx, int x, int y, int w, int h);

/**
 * @brief Reset clipping region to entire canvas
 */
void trvg_reset_clip(trvg_context_t *ctx);

/* ============================================================================
 * Ultra-fast Drawing API (no anti-aliasing, MCU optimized)
 * ============================================================================ */

/**
 * @brief Ultra-fast clear screen - uses optimized memory fill
 * @param ctx Context
 * @param r, g, b RGB color values
 */
void trvg_fast_clear(trvg_context_t *ctx, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Ultra-fast set single pixel
 * @param ctx Context
 * @param x, y Pixel coordinates
 * @param r, g, b RGB color values
 */
void trvg_fast_pixel(trvg_context_t *ctx, int x, int y,
                     uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Ultra-fast horizontal line
 * @param ctx Context
 * @param x0, x1 Start and end x coordinates
 * @param y Y coordinate
 * @param r, g, b RGB color values
 */
void trvg_fast_hline(trvg_context_t *ctx, int x0, int x1, int y,
                     uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Ultra-fast vertical line
 * @param ctx Context
 * @param x X coordinate
 * @param y0, y1 Start and end y coordinates
 * @param r, g, b RGB color values
 */
void trvg_fast_vline(trvg_context_t *ctx, int x, int y0, int y1,
                     uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Ultra-fast line drawing - uses Bresenham algorithm, no anti-aliasing
 * @param ctx Context
 * @param x0, y0 Start point (pixel coordinates)
 * @param x1, y1 End point (pixel coordinates)
 * @param r, g, b RGB color values
 */
void trvg_fast_line(trvg_context_t *ctx,
                    int x0, int y0, int x1, int y1,
                    uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Ultra-fast thick line drawing - uses Bresenham algorithm, no anti-aliasing
 * @param ctx Context
 * @param x0, y0 Start point (pixel coordinates)
 * @param x1, y1 End point (pixel coordinates)
 * @param thickness Line width (pixels)
 * @param r, g, b RGB color values
 */
void trvg_fast_thick_line(trvg_context_t *ctx,
                          int x0, int y0, int x1, int y1,
                          int thickness,
                          uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Ultra-fast fill rectangle - no anti-aliasing
 * @param ctx Context
 * @param x, y Top-left corner coordinates
 * @param w, h Width and height
 * @param r, g, b RGB color values
 */
void trvg_fast_fill_rect(trvg_context_t *ctx,
                         int x, int y, int w, int h,
                         uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Ultra-fast stroke rectangle - no anti-aliasing
 * @param ctx Context
 * @param x, y Top-left corner coordinates
 * @param w, h Width and height
 * @param r, g, b RGB color values
 */
void trvg_fast_stroke_rect(trvg_context_t *ctx,
                           int x, int y, int w, int h,
                           uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Ultra-fast fill circle - uses Midpoint Circle algorithm
 * @param ctx Context
 * @param cx, cy Center coordinates
 * @param radius Radius
 * @param r, g, b RGB color values
 */
void trvg_fast_fill_circle(trvg_context_t *ctx,
                           int cx, int cy, int radius,
                           uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Ultra-fast stroke circle - uses Midpoint Circle algorithm
 * @param ctx Context
 * @param cx, cy Center coordinates
 * @param radius Radius
 * @param r, g, b RGB color values
 */
void trvg_fast_stroke_circle(trvg_context_t *ctx,
                             int cx, int cy, int radius,
                             uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Ultra-fast fill polygon - uses scanline algorithm, no anti-aliasing
 * @param ctx Context
 * @param points Point array [x0, y0, x1, y1, ...]
 * @param count Number of points
 * @param r, g, b RGB color values
 */
void trvg_fast_fill_polygon(trvg_context_t *ctx,
                            const int16_t *points, int count,
                            uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Ultra-fast stroke polygon/polyline - no anti-aliasing
 * @param ctx Context
 * @param points Point array [x0, y0, x1, y1, ...]
 * @param count Number of points
 * @param closed Whether to close the polygon
 * @param r, g, b RGB color values
 */
void trvg_fast_stroke_polygon(trvg_context_t *ctx,
                              const int16_t *points, int count,
                              bool closed,
                              uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Ultra-fast thick polygon/polyline - no anti-aliasing
 * @param ctx Context
 * @param points Point array [x0, y0, x1, y1, ...]
 * @param count Number of points
 * @param closed Whether to close the polygon
 * @param thickness Line width
 * @param r, g, b RGB color values
 */
void trvg_fast_thick_polygon(trvg_context_t *ctx,
                             const int16_t *points, int count,
                             bool closed, int thickness,
                             uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Ultra-fast navigation path drawing - with round joints, optimized for navigation paths
 *
 * Solves the following problems with regular thick line drawing:
 * - Truncation: no smooth transition at line segment joints
 * - Discontinuity: gaps when diagonal lines connect to horizontal/vertical lines
 * - Sharp corners: no round joint processing
 *
 * @param ctx Context
 * @param points Point array [x0, y0, x1, y1, ...]
 * @param count Number of points
 * @param thickness Line width (pixels)
 * @param r, g, b RGB color values
 */
void trvg_fast_nav_path(trvg_context_t *ctx,
                        const int16_t *points, int count,
                        int thickness,
                        uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Ultra-fast fill triangle - uses scanline algorithm
 * @param ctx Context
 * @param x0, y0 First vertex
 * @param x1, y1 Second vertex
 * @param x2, y2 Third vertex
 * @param r, g, b RGB color values
 */
void trvg_fast_fill_triangle(trvg_context_t *ctx,
                             int x0, int y0,
                             int x1, int y1,
                             int x2, int y2,
                             uint8_t r, uint8_t g, uint8_t b);

/* ============================================================================
 * Utility Functions
 * ============================================================================ */

/**
 * @brief Get statistics
 */
void trvg_get_stats(const trvg_context_t *ctx, uint32_t *pixels);

/**
 * @brief Reset statistics
 */
void trvg_reset_stats(trvg_context_t *ctx);

/* ============================================================================
 * Lane Arrow Fast Drawing API (specialized for navigation lane display)
 * ============================================================================ */

/**
 * @brief Lane arrow direction types
 */
typedef enum
{
    TRVG_ARROW_STRAIGHT = 0,     /* Straight arrow ↑ */
    TRVG_ARROW_LEFT,             /* Left turn arrow ← */
    TRVG_ARROW_RIGHT,            /* Right turn arrow → */
    TRVG_ARROW_SLIGHT_LEFT,      /* Slight left ↖ */
    TRVG_ARROW_SLIGHT_RIGHT,     /* Slight right ↗ */
    TRVG_ARROW_SHARP_LEFT,       /* Sharp left (acute left turn) */
    TRVG_ARROW_SHARP_RIGHT,      /* Sharp right (acute right turn) */
    TRVG_ARROW_UTURN_LEFT,       /* U-turn left */
    TRVG_ARROW_UTURN_RIGHT       /* U-turn right */
} trvg_arrow_type_t;

/**
 * @brief Combination arrow types (for lanes with multiple directions)
 */
typedef enum
{
    TRVG_COMBO_STRAIGHT_LEFT = 0,    /* Straight + left ↑← */
    TRVG_COMBO_STRAIGHT_RIGHT,       /* Straight + right ↑→ */
    TRVG_COMBO_LEFT_RIGHT,           /* Left + right ←→ */
    TRVG_COMBO_STRAIGHT_LEFT_RIGHT,  /* Straight + left + right ←↑→ */
    TRVG_COMBO_STRAIGHT_SLIGHT_LEFT, /* Straight + slight left ↑↖ */
    TRVG_COMBO_STRAIGHT_SLIGHT_RIGHT /* Straight + slight right ↑↗ */
} trvg_combo_arrow_type_t;

/**
 * @brief Draw lane direction arrow - single direction
 */
void trvg_fast_lane_arrow(trvg_context_t *ctx,
                          trvg_arrow_type_t type,
                          int cx, int cy, int size,
                          uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Draw combination lane arrow - one lane with multiple directions
 */
void trvg_fast_combo_arrow(trvg_context_t *ctx,
                           trvg_combo_arrow_type_t type,
                           int cx, int cy, int size,
                           uint8_t r, uint8_t g, uint8_t b);

/* ============================================================================
 * GPS Position Marker Drawing API
 * ============================================================================ */

/**
 * @brief Draw GPS position marker (Google Maps style teardrop shape)
 *
 * Draws a Google Maps-like position marker consisting of:
 * - Upper half: circle
 * - Lower half: triangular point
 * - Interior: white dot
 *
 * @param ctx Context
 * @param cx Marker center x coordinate (tip position)
 * @param cy Marker center y coordinate (tip position)
 * @param size Marker size (total height, pixels)
 * @param r, g, b Main body RGB color values
 */
void trvg_fast_gps_marker(trvg_context_t *ctx,
                          int cx, int cy, int size,
                          uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Draw GPS position marker with direction indicator
 *
 * Adds direction indicator on top of standard GPS marker
 *
 * @param ctx Context
 * @param cx Marker center x coordinate (tip position)
 * @param cy Marker center y coordinate (tip position)
 * @param size Marker size (total height, pixels)
 * @param heading Direction angle (0=north, 90=east, 180=south, 270=west)
 * @param r, g, b Main body RGB color values
 */
void trvg_fast_gps_marker_with_heading(trvg_context_t *ctx,
                                       int cx, int cy, int size,
                                       float heading,
                                       uint8_t r, uint8_t g, uint8_t b);

#ifdef __cplusplus
}
#endif

#endif /* TRVG_H */
