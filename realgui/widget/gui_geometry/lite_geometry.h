#ifndef LITE_GEOMETRY_H
#define LITE_GEOMETRY_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

#include "lite_geometry_math.h"

#define LG_MIN(a, b) ((a) < (b) ? (a) : (b))
#define LG_MAX(a, b) ((a) > (b) ? (a) : (b))
#define LG_CLAMP(x, min_val, max_val) LG_MIN(LG_MAX(x, min_val), max_val)
#define LG_ABS(x) ((x) > 0 ? (x) : -(x))

#ifdef M_PI
#undef M_PI
#endif
#define M_PI 3.14159265358979323846f

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440f /* 1/sqrt(2) */
#endif

#define LG_UNUSED(x) ((void)x)

#define CANVAS_WIDTH   410
#define CANVAS_HEIGHT  502

typedef uint32_t PixelColor;

typedef struct
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
} lg_color32_t;

typedef struct
{
    int x;
    int y;
    PixelColor color;
} PixelData;
typedef struct
{
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
} lg_area_t;
typedef struct
{
    PixelData *data;
    int count;
} PixelBuffer;

typedef struct
{
    int16_t x, y;
    int16_t w, h;
} Rect;

typedef struct
{
    int32_t x;
    int32_t y;
} lg_point_t;

typedef enum
{
    PIXEL_FORMAT_RGB565,
    PIXEL_FORMAT_ARGB8888
} PixelFormat;

// Gradient types
typedef enum
{
    GRADIENT_NONE = 0,
    GRADIENT_LINEAR,
    GRADIENT_RADIAL,
    GRADIENT_ANGULAR
} GradientType;

// Gradient stop structure
typedef struct
{
    float position;      // 0.0 to 1.0
    PixelColor color;
} GradientStop;

// Gradient definition
typedef struct
{
    GradientType type;
    GradientStop stops[8];  // Support up to 8 color stops
    int stop_count;

    // Linear gradient parameters
    float linear_x1, linear_y1;  // Start point
    float linear_x2, linear_y2;  // End point

    // Radial gradient parameters
    float radial_cx, radial_cy;  // Center point
    float radial_r;              // Radius

    // Angular gradient parameters (for arc)
    float angular_cx, angular_cy;  // Center point
    float angular_start;           // Start angle in degrees
    float angular_end;             // End angle in degrees
} Gradient;

typedef struct
{
    uint8_t *buffer;
    int width;
    int height;
    int stride;
    PixelFormat format;     // pixel format
    bool enable_aa; //anti-aliasing
    Rect clip_rect;
    bool enable_stroke_cap; //enable stroke cap
    Gradient *gradient;     // Optional gradient for fills/strokes
} DrawContext;

void add_pixel_with_coverage_rgb565(DrawContext *ctx, int x, int y, PixelColor color,
                                    float coverage);
void add_pixel_with_coverage_argb8888(DrawContext *ctx, int x, int y, PixelColor color,
                                      float coverage);

static void add_pixel_with_coverage(DrawContext *ctx, int x, int y, PixelColor color,
                                    float coverage);
static inline void write_pixel_at_offset(DrawContext *ctx, int byte_offset, PixelColor color);
static inline void write_pixel_rgb565(DrawContext *ctx, int byte_offset, PixelColor color);
static inline void write_pixel_argb8888(DrawContext *ctx, int byte_offset, PixelColor color);
// static inline PixelColor get_pixel_color(DrawContext *ctx, int x, int y);
PixelColor blend_colors_argb8888(PixelColor bg_color, PixelColor fg_color,
                                 uint8_t alpha);
PixelColor blend_colors_rgb565(PixelColor bg_color, PixelColor fg_color,
                               uint8_t alpha);

void add_pixel_aa(DrawContext *ctx, int x, int y, PixelColor color, float coverage);
void add_pixel_to_context(DrawContext *ctx, int x, int y, PixelColor color);
void init_draw_context(DrawContext *ctx, uint8_t *buffer, int width, int height,
                       PixelFormat format);

void fill_rect(DrawContext *ctx, const Rect *rect, PixelColor fill_color);
void fill_rounded_rect(DrawContext *ctx,
                       float x, float y, float width, float height,
                       float radius, PixelColor fill_color);
void fill_rounded_rect_df_aa(DrawContext *ctx,
                             int x, int y, int width, int height,
                             int radius, PixelColor fill_color);
void fill_rounded_rect_no_aa(DrawContext *ctx,
                             float x, float y, float width, float height,
                             float radius, PixelColor fill_color);
void fill_circle(DrawContext *ctx, float center_x, float center_y, float radius,
                 PixelColor fill_color);
void fill_circle_no_aa(DrawContext *ctx, int center_x, int center_y, int radius, PixelColor color);
void fill_circle_df_aa(DrawContext *ctx, int center_x, int center_y, int radius,
                       PixelColor fill_color);

void draw_arc(DrawContext *ctx, float center_x, float center_y,
              float radius, float line_width,
              float start_angle, float end_angle,
              PixelColor stroke_color);
void draw_arc_df_aa(DrawContext *ctx, float center_x, float center_y,
                    float radius, float line_width,
                    float start_angle, float end_angle,
                    PixelColor stroke_color);
void draw_arc_as_ring(DrawContext *ctx, float center_x, float center_y,
                      float arc_radius, float line_width, PixelColor color);
void draw_arc_df_aa_gradient(DrawContext *ctx, float center_x, float center_y,
                             float radius, float line_width,
                             float start_angle, float end_angle,
                             Gradient *gradient);
void draw_symmetric_pixels(DrawContext *ctx, float cx, float cy, int x, int y,
                           PixelColor color, float coverage);
PixelColor create_color_by_format(PixelFormat format, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
PixelColor RGBA_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

// Gradient functions
void gradient_init(Gradient *grad, GradientType type);
void gradient_add_stop(Gradient *grad, float position, PixelColor color);
PixelColor gradient_get_color(Gradient *grad, float t);
PixelColor gradient_get_color_at_point(Gradient *grad, float x, float y);
PixelColor gradient_get_color_at_angle(Gradient *grad, float angle);

// #define COLOR_RED     create_color_by_format(PIXEL_FORMAT_ARGB8888, 255, 0, 0, 255)
// #define COLOR_GREEN   create_color_by_format(PIXEL_FORMAT_ARGB8888, 0, 255, 0, 255)
// #define COLOR_BLUE    create_color_by_format(PIXEL_FORMAT_ARGB8888, 0, 0, 255, 255)
// #define COLOR_WHITE   create_color_by_format(PIXEL_FORMAT_ARGB8888, 255, 255, 255, 255)
// #define COLOR_BLACK   create_color_by_format(PIXEL_FORMAT_ARGB8888, 0, 0, 0, 255)
// #define COLOR_YELLOW  create_color_by_format(PIXEL_FORMAT_ARGB8888, 255, 255, 0, 255)
// #define COLOR_CYAN    create_color_by_format(PIXEL_FORMAT_ARGB8888, 0, 255, 255, 255)
// #define COLOR_MAGENTA create_color_by_format(PIXEL_FORMAT_ARGB8888, 255, 0, 255, 255)

// // opacity 50%
// #define COLOR_RED_50     create_color_by_format(PIXEL_FORMAT_ARGB8888, 255, 0, 0, 128)
// #define COLOR_GREEN_50   create_color_by_format(PIXEL_FORMAT_ARGB8888, 0, 255, 0, 128)
// #define COLOR_BLUE_50    create_color_by_format(PIXEL_FORMAT_ARGB8888, 0, 0, 255, 128)
// #define COLOR_WHITE_50   create_color_by_format(PIXEL_FORMAT_ARGB8888, 255, 255, 255, 128)
// #define COLOR_BLACK_50   create_color_by_format(PIXEL_FORMAT_ARGB8888, 0, 0, 0, 128)
// #define COLOR_MAGENTA_50 create_color_by_format(PIXEL_FORMAT_ARGB8888, 255, 0, 255, 128)
#endif // LITE_GEOMETRY_H