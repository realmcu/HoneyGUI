/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "draw_font.h"
#include "font_ttf.h"
#include "font_mem.h"
#include "font_lib_manager.h"
#include <stddef.h>
#include <math.h>
#include "font_rendering_utils.h"
#include "gui_vfs.h"


/*============================================================================*
 *                           Types
 *============================================================================*/
typedef struct line
{
    int16_t y0;
    int16_t y1;
    float x0;
    float dxy;
} LINE_T;

/* Glyph bitmap bounds and the sub-pixel offset passed to the rasterizer.
 * Positions use the coordinate space supplied to ttf_split_placement(). */
typedef struct
{
    int out_x0;
    int out_y0;
    int out_w;
    int out_h;
    int render_w;
    int render_h;
    float sub_x;
    float sub_y;
} ttf_raster_geo_t;


/*============================================================================*
 *                           Constants
 *============================================================================*/
/* No lookup tables here on purpose: the prec 2/4/8 downsample paths compute
 * all population counts of a word in parallel with SWAR operations instead. */


/*============================================================================*
 *                            Macros
 *============================================================================*/

/*MVE*/
#define GUI_ENABLE_MVE      1
#ifdef __ARM_FEATURE_MVE
#if GUI_ENABLE_MVE
#define FONT_TTF_USE_MVE
/* Bit 0 of __ARM_FEATURE_MVE marks the integer MVE subset, bit 1 the floating
 * point one. Targets built with +mve instead of +mve.fp only provide the
 * integer subset, where float32x4_t and the f32 intrinsics do not exist. */
#if (__ARM_FEATURE_MVE & 2)
#define FONT_TTF_USE_MVE_FP
#endif
#include "arm_mve.h"
#endif
#endif

/* Copy packed winding data from flash to RAM before transformation.
 * Keep enabled unless disassembly confirms that the compiler uses scalar loads. */
#define FIX_AUTO_VECTORIZE   1

/* Per-kernel explicit MVE switches. Keep these separate so a target platform
 * can benchmark each implementation independently. A value of 1 means the
 * MVE path is enabled when FONT_TTF_USE_MVE is available. */
#define FONT_TTF_MVE_EMBOLDEN          1
#define FONT_TTF_MVE_ADJUST_PRECISION  1
/* Needs FONT_TTF_USE_MVE_FP, not just FONT_TTF_USE_MVE. */
#define FONT_TTF_MVE_WINDING_TRANSFORM 1
/* The prec 1 MVE kernel is retained for other targets, but disabled here:
 * repeated end-to-end benchmarks are slower than the scalar nibble LUT. */
#define FONT_TTF_MVE_DOWNSAMPLE_PREC1  0
#define FONT_TTF_MVE_DOWNSAMPLE_PREC2  1
#define FONT_TTF_MVE_DOWNSAMPLE_PREC4  1
#define FONT_TTF_MVE_DOWNSAMPLE_PREC8  1

#define ALIGN_TO(x, y) (((x) + ((y) - 1)) & ~((y) - 1))
#define ROUNDING_OFFSET 0.5f

/* Bits per word of the XOR scanline buffer; render_w must be a multiple of it. */
#define FONT_TTF_BLOCK_BIT 32

/* Scanline kernel: choose 1 for wide/dense outlines such as CJK, or 0 for
 * narrow/sparse outlines such as small ASCII. Benchmark the target workload. */
#define FONT_TTF_SCANLINE_PREFIX_XOR 1


/*============================================================================*
 *                            Variables
 *============================================================================*/

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

#if ENABLE_FONT_V3_TYPO
/**
 * @brief Populate glyph metrics using V3 or legacy rules.
 * @param chr Output character metrics.
 * @param glyphData Source glyph metrics in font units.
 * @param unicode Unicode code point.
 * @param font_height Requested font height in pixels.
 * @param scale Font-unit to pixel scale.
 * @param bold_weight Bold expansion in pixels.
 * @param typo_ctx Typography mode and metrics.
 * @param dot_addr Glyph data retained for rendering.
 */
static void ttf_populate_glyph_metrics(mem_char_t *chr, const FontGlyphData *glyphData,
                                       uint32_t unicode, uint16_t font_height,
                                       float scale, uint8_t bold_weight,
                                       const gui_font_typo_context_t *typo_ctx,
                                       uint8_t *dot_addr);
#endif

/**
 * @brief Classify the signs of two transform coefficients.
 * @param a First coefficient.
 * @param b Second coefficient.
 * @return Sign combination used to select bounding-box corners.
 */
static TransformCase determineTransformCase(float a, float b)
{
    if (a >= 0 && b >= 0)
    {
        return POS_POS;
    }
    else if (a >= 0 && b < 0)
    {
        return POS_NEG;
    }
    else if (a < 0 && b >= 0)
    {
        return NEG_POS;
    }
    else
    {
        return NEG_NEG;
    }
}

/**
 * @brief Count set bits independently in each byte of a word.
 * @param value Packed bytes to count.
 * @return Per-byte population counts in the corresponding byte lanes.
 */
static inline uint32_t font_ttf_popcount_bytes(uint32_t value)
{
    value = value - ((value >> 1) & 0x55555555u);
    value = (value & 0x33333333u) + ((value >> 2) & 0x33333333u);
    return (value + (value >> 4)) & 0x0F0F0F0Fu;
}

/* Expand a source nibble from MSB to LSB into four 0x00/0xff coverage bytes.
 * The constants account for the little-endian byte order of the uint32 store. */
static const uint32_t font_ttf_expand_4bit[16] =
{
    0x00000000u, 0xFF000000u, 0x00FF0000u, 0xFFFF0000u,
    0x0000FF00u, 0xFF00FF00u, 0x00FFFF00u, 0xFFFFFF00u,
    0x000000FFu, 0xFF0000FFu, 0x00FF00FFu, 0xFFFF00FFu,
    0x0000FFFFu, 0xFF00FFFFu, 0x00FFFFFFu, 0xFFFFFFFFu
};

/**
 * @brief Apply a perspective transform to one point.
 * @param x Input x coordinate.
 * @param y Input y coordinate.
 * @param m Transformation matrix.
 * @param x_out Output x coordinate.
 * @param y_out Output y coordinate.
 */
static void transformPointV2(float x, float y, float m[3][3], float *x_out, float *y_out)
{
    float w = m[2][0] * x + m[2][1] * y + m[2][2];
    *x_out = (m[0][0] * x + m[0][1] * y + m[0][2]) / w;
    *y_out = (m[1][0] * x + m[1][1] * y + m[1][2]) / w;
}

/**
 * @brief Apply an affine transform to one point.
 * @param x Input x coordinate.
 * @param y Input y coordinate.
 * @param m Transformation matrix.
 * @param x_out Output x coordinate.
 * @param y_out Output y coordinate.
 */
static void transformPoint(float x, float y, float m[3][3], float *x_out, float *y_out)
{
    *x_out = m[0][0] * x + m[0][1] * y + m[0][2];
    *y_out = m[1][0] * x + m[1][1] * y + m[1][2];
}

/**
 * @brief Compute an affine-transformed bounding box.
 * @param x0 Input minimum x coordinate.
 * @param y0 Input minimum y coordinate.
 * @param x1 Input maximum x coordinate.
 * @param y1 Input maximum y coordinate.
 * @param m Transformation matrix.
 * @param caseX Sign case for selecting x extrema.
 * @param caseY Sign case for selecting y extrema.
 * @param xmin Output minimum x coordinate.
 * @param xmax Output maximum x coordinate.
 * @param ymin Output minimum y coordinate.
 * @param ymax Output maximum y coordinate.
 */
static void computeBoundingBoxFloat(float x0, float y0, float x1, float y1, float m[3][3],
                                    TransformCase caseX, TransformCase caseY, float *xmin, float *xmax, float *ymin, float *ymax)
{
    float temp_x, temp_y;

    if (caseX == POS_POS || caseX == POS_NEG)
    {
        transformPoint(x0, (caseX == POS_POS ? y0 : y1), m, xmin, &temp_y);
        transformPoint(x1, (caseX == POS_POS ? y1 : y0), m, xmax, &temp_y);
    }
    else
    {
        transformPoint(x1, (caseX == NEG_POS ? y0 : y1), m, xmin, &temp_y);
        transformPoint(x0, (caseX == NEG_POS ? y1 : y0), m, xmax, &temp_y);
    }

    /* caseY encodes (sign m[1][0], sign m[1][1]), so the y extreme is selected by
     * m[1][1] and the x corner by m[1][0]. Grouping by m[1][1] used to be wrong
     * for POS_NEG/NEG_NEG (vertical mirror, 180 deg), which swapped ymin/ymax. */
    if (caseY == POS_POS || caseY == NEG_POS)
    {
        transformPoint((caseY == POS_POS ? x0 : x1), y0, m, &temp_x, ymin);
        transformPoint((caseY == POS_POS ? x1 : x0), y1, m, &temp_x, ymax);
    }
    else
    {
        transformPoint((caseY == POS_NEG ? x0 : x1), y1, m, &temp_x, ymin);
        transformPoint((caseY == POS_NEG ? x1 : x0), y0, m, &temp_x, ymax);
    }
}

/**
 * @brief Compute a perspective-transformed floating-point bounding box.
 * @param x0 Input minimum x coordinate.
 * @param y0 Input minimum y coordinate.
 * @param x1 Input maximum x coordinate.
 * @param y1 Input maximum y coordinate.
 * @param m Transformation matrix.
 * @param xmin Output minimum x coordinate.
 * @param xmax Output maximum x coordinate.
 * @param ymin Output minimum y coordinate.
 * @param ymax Output maximum y coordinate.
 */
static void computeBoundingBoxFloatV2(float x0, float y0, float x1, float y1, float m[3][3],
                                      float *xmin, float *xmax, float *ymin, float *ymax)
{
    float tx0, ty0, tx1, ty1, tx2, ty2, tx3, ty3;

    transformPointV2(x0, y0, m, &tx0, &ty0);
    transformPointV2(x1, y0, m, &tx1, &ty1);
    transformPointV2(x0, y1, m, &tx2, &ty2);
    transformPointV2(x1, y1, m, &tx3, &ty3);

    *xmin = fminf(fminf(tx0, tx1), fminf(tx2, tx3));
    *xmax = fmaxf(fmaxf(tx0, tx1), fmaxf(tx2, tx3));
    *ymin = fminf(fminf(ty0, ty1), fminf(ty2, ty3));
    *ymax = fmaxf(fmaxf(ty0, ty1), fmaxf(ty2, ty3));
}

/**
 * @brief Compute a perspective-transformed integer bounding box.
 * @param x0 Input minimum x coordinate.
 * @param y0 Input minimum y coordinate.
 * @param x1 Input maximum x coordinate.
 * @param y1 Input maximum y coordinate.
 * @param m Transformation matrix.
 * @param xmin Output minimum x coordinate.
 * @param xmax Output maximum x coordinate.
 * @param ymin Output minimum y coordinate.
 * @param ymax Output maximum y coordinate.
 */
static void computeBoundingBoxIntV2(int x0, int y0, int x1, int y1, float m[3][3], int *xmin,
                                    int *xmax, int *ymin, int *ymax)
{
    float fxmin, fxmax, fymin, fymax;

    computeBoundingBoxFloatV2((float)x0, (float)y0, (float)x1, (float)y1, m, &fxmin, &fxmax, &fymin,
                              &fymax);

    *xmin = (int)fxmin;
    *xmax = (int)fxmax;
    *ymin = (int)fymin;
    *ymax = (int)fymax;
}

void add_point_to_line(LINE_T *line, ttf_point p1, ttf_point p2)
{
    if (p1.y < p2.y)
    {
        line->y0 = (int)(p1.y + ROUNDING_OFFSET);
        line->y1 = (int)(p2.y + ROUNDING_OFFSET);
        line->dxy = (p2.x - p1.x) / (p2.y - p1.y);
        line->x0 = p1.x - line->dxy * line->y0;
    }
    else
    {
        line->y0 = (int)(p2.y + ROUNDING_OFFSET);
        line->y1 = (int)(p1.y + ROUNDING_OFFSET);
        line->dxy = (p2.x - p1.x) / (p2.y - p1.y);
        line->x0 = p2.x - line->dxy * line->y0;
    }
}

/**
 * @brief Undo the V3 bearing already included in chr.x/chr.y.
 * @param chr Character containing the rounded bearing.
 * @param is_v3 Whether V3 typography is active.
 * @param back_x Output horizontal correction in pixels.
 * @param back_y Output vertical correction in pixels.
 */
static void ttf_calc_bearing_backout(const mem_char_t *chr, bool is_v3,
                                     float *back_x, float *back_y)
{
#if ENABLE_FONT_V3_TYPO
    if (is_v3)
    {
        *back_x = -chr->bearing_x;
        *back_y = chr->bearing_y;
        return;
    }
#else
    (void)chr;
    (void)is_v3;
#endif
    *back_x = 0.0f;
    *back_y = 0.0f;
}

/**
 * @brief Return the fractional glyph origin relative to chr.x/chr.y.
 * @param gd Glyph bounds in font units.
 * @param chr Character placement and bearing.
 * @param scale Font-unit to pixel scale.
 * @param ascent Legacy ascent; zero for V3 typography.
 * @param is_v3 Whether V3 typography is active.
 * @param origin_x Output horizontal origin in pixels.
 * @param origin_y Output vertical origin in pixels.
 */
static void ttf_calc_glyph_origin(const FontGlyphData *gd, const mem_char_t *chr,
                                  float scale, short ascent, bool is_v3,
                                  float *origin_x, float *origin_y)
{
    float back_x, back_y;
    ttf_calc_bearing_backout(chr, is_v3, &back_x, &back_y);

    *origin_x = scale * gd->x0 + back_x;
    *origin_y = scale * (ascent + gd->y0) + back_y;
}

/**
 * @brief Split placement into a pixel origin and sub-pixel remainder.
 * @param origin_x Exact bitmap left position in pixels.
 * @param origin_y Exact bitmap top position in pixels.
 * @param span_x Bitmap width before alignment in pixels.
 * @param span_y Bitmap height before alignment in pixels.
 * @param raster_prec Sub-samples per pixel axis.
 * @param geo Output aligned raster geometry.
 */
static void ttf_split_placement(float origin_x, float origin_y, float span_x, float span_y,
                                uint8_t raster_prec, ttf_raster_geo_t *geo)
{
    /* A negative span would make render_w/render_h negative and turn the
     * render_size / out_size products into huge unsigned values downstream. */
    if (span_x < 0) { span_x = 0; }
    if (span_y < 0) { span_y = 0; }

    geo->out_x0 = (int)floorf(origin_x);
    geo->out_y0 = (int)floorf(origin_y);
    geo->sub_x = (origin_x - geo->out_x0) * raster_prec;
    geo->sub_y = (origin_y - geo->out_y0) * raster_prec;

    /* Include the sub-pixel shift and historical two-sample margin.
     * Align dimensions for downsampling and the scanline buffer. */
    geo->render_h = ALIGN_TO((int)(geo->sub_y + span_y * raster_prec + 2), raster_prec);
    geo->out_h = geo->render_h / raster_prec;
    geo->render_w = ALIGN_TO((int)(geo->sub_x + span_x * raster_prec + 2), raster_prec);
    geo->render_w = ALIGN_TO(geo->render_w, FONT_TTF_BLOCK_BIT);
    geo->out_w = geo->render_w / raster_prec;
}

/**
 * @brief Build raster geometry for identity or translation transforms.
 * @param gd Glyph bounds in font units.
 * @param scale Font-unit to pixel scale.
 * @param raster_prec Sub-samples per pixel axis.
 * @param bold_weight Bold expansion in pixels.
 * @param origin_x Exact horizontal origin in pixels.
 * @param origin_y Exact vertical origin in pixels.
 * @param geo Output aligned raster geometry.
 */
static void ttf_calc_raster_geometry(const FontGlyphData *gd, float scale, uint8_t raster_prec,
                                     uint8_t bold_weight, float origin_x, float origin_y,
                                     ttf_raster_geo_t *geo)
{
    ttf_split_placement(origin_x - bold_weight, origin_y - bold_weight,
                        scale * (gd->x1 - gd->x0) + bold_weight * 2,
                        scale * (gd->y1 - gd->y0) + bold_weight * 2,
                        raster_prec, geo);
}

/**
 * @brief Dilate the bitmap according to the selected bold mode.
 * @param bitmap Bitmap modified in place.
 * @param width Bitmap width in pixels.
 * @param height Bitmap height in pixels.
 * @param bold_weight Dilation radius in pixels.
 * @param bold_mode Horizontal-only or full dilation mode.
 */
static void font_ttf_bitmap_embolden(uint8_t *bitmap, int width, int height, uint8_t bold_weight,
                                     uint8_t bold_mode)
{
    if (bold_weight == 0 || bitmap == NULL || width <= 0 || height <= 0)
    {
        return;
    }

    uint32_t size = width * height;
    uint8_t *temp_buf = gui_malloc(size);
    if (temp_buf == NULL)
    {
        return;
    }

    /* First pass: horizontal dilation */
#if defined(FONT_TTF_USE_MVE) && FONT_TTF_MVE_EMBOLDEN
    int padded_width = width + bold_weight * 2;
    uint8_t *padded_buf = gui_malloc(padded_width);
    if (padded_buf == NULL)
    {
        gui_free(temp_buf);
        return;
    }

    memset(padded_buf, 0, bold_weight);
    memset(padded_buf + bold_weight + width, 0, bold_weight);
    uint8_t *row_buf = padded_buf + bold_weight;

    for (int y = 0; y < height; y++)
    {
        uint8_t *row = bitmap + y * width;
        memcpy(row_buf, row, width);

        int x = 0;
        for (; x + 16 <= width; x += 16)
        {
            uint8x16_t max_vec = vldrbq_u8(&row_buf[x]);
            for (int b = 1; b <= bold_weight; b++)
            {
                uint8x16_t left_vec = vldrbq_u8(&row_buf[x - b]);
                uint8x16_t right_vec = vldrbq_u8(&row_buf[x + b]);
                max_vec = vmaxq_u8(max_vec, left_vec);
                max_vec = vmaxq_u8(max_vec, right_vec);
            }
            vstrbq_u8(&row[x], max_vec);
        }

        for (; x < width; x++)
        {
            uint8_t max_val = row_buf[x];
            for (int b = 1; b <= bold_weight; b++)
            {
                if (row_buf[x - b] > max_val) { max_val = row_buf[x - b]; }
                if (row_buf[x + b] > max_val) { max_val = row_buf[x + b]; }
            }
            row[x] = max_val;
        }
    }
    gui_free(padded_buf);
#else
    int padded_width = width + bold_weight * 2;
    uint8_t *padded_buf = gui_malloc(padded_width);
    if (padded_buf == NULL)
    {
        gui_free(temp_buf);
        return;
    }

    memset(padded_buf, 0, bold_weight);
    memset(padded_buf + bold_weight + width, 0, bold_weight);
    uint8_t *row_buf = padded_buf + bold_weight;

    for (int y = 0; y < height; y++)
    {
        uint8_t *row = bitmap + y * width;
        memcpy(row_buf, row, width);

        for (int x = 0; x < width; x++)
        {
            uint8_t max_val = row_buf[x];
            for (int b = 1; b <= bold_weight; b++)
            {
                if (row_buf[x - b] > max_val) { max_val = row_buf[x - b]; }
                if (row_buf[x + b] > max_val) { max_val = row_buf[x + b]; }
            }
            row[x] = max_val;
        }
    }
    gui_free(padded_buf);
#endif

    /* Second pass: vertical dilation (only for BOLD_FULL) */
    if (bold_mode == BOLD_FULL)
    {
        memcpy(temp_buf, bitmap, size);

        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                uint8_t max_val = temp_buf[y * width + x];

                for (int b = 1; b <= bold_weight; b++)
                {
                    if (y - b >= 0)
                    {
                        uint8_t val = temp_buf[(y - b) * width + x];
                        if (val > max_val) { max_val = val; }
                    }
                    if (y + b < height)
                    {
                        uint8_t val = temp_buf[(y + b) * width + x];
                        if (val > max_val) { max_val = val; }
                    }
                }

                bitmap[y * width + x] = max_val;
            }
        }
    }

    gui_free(temp_buf);
}


void font_ttf_draw_bitmap_classic(gui_text_t *text, uint8_t *buf,
                                  gui_text_rect_t *rect,
                                  int x, int y,
                                  int w, int h)
{
    gui_dispdev_t *dc = gui_get_dc();

    int x_start = _UI_MAX3(x, rect->xboundleft, dc->section.x1);
    int x_end = _UI_MIN3(x + w - 1, rect->xboundright, dc->section.x2);

    int y_start = _UI_MAX3(dc->section.y1, y, rect->yboundtop);
    int y_end = _UI_MIN3(y + h - 1, rect->yboundbottom, dc->section.y2);
    if (x_start > x_end || y_start > y_end)
    {
        return;
    }

    gui_color_t outcolor = text->color;
    outcolor.color.rgba.a = _UI_UDIV255(text->color.color.rgba.a * text->base.opacity_value);

    draw_font_t render_font =
    {
        .target_buf = dc->frame_buf,
        .target_buf_stride = dc->fb_width * (dc->bit_depth >> 3),
        .color = outcolor,
        .blend_mode = text->font_blend_mode,
        .target_rect = {
            .x1 = dc->section.x1,
            .y1 = dc->section.y1,
            .x2 = dc->section.x2,
            .y2 = dc->section.y2,
        },
        .clip_rect = {
            .x1 = x_start,
            .y1 = y_start,
            .x2 = x_end,
            .y2 = y_end
        },
        .render_mode = 8,
    };

    switch (dc->bit_depth)
    {
    case 16:
        render_font.target_format = RGB565;
        break;
    case 24:
        render_font.target_format = RGB888;
        // render_font.target_format = ARGB8565;
        break;
    case 32:
        render_font.target_format = ARGB8888;
        break;
    case 8:
        render_font.target_format = GRAY8;
        break;
    default:
        break;
    }

    font_glyph_t glyph =
    {
        .data = buf,
        .pos_x = x,
        .pos_y = y,
        .width = w,
        .height = h,
        .stride = w
    };

    font_glyph_render(&render_font, &glyph);
}

uint32_t font_index_bsearch_ttf(uint8_t *index_table,
                                uint32_t index_area_size,
                                uint32_t unicode)
{
    if (index_area_size == 0 || index_table == NULL)
    {
        return 0;
    }

    uint32_t entry_size = sizeof(uint16_t) + sizeof(uint32_t);  /* 6 bytes per entry */
    uint32_t count = index_area_size / entry_size;

    if (count == 0)
    {
        return 0;
    }

    int32_t left = 0;
    int32_t right = (int32_t)count - 1;

    while (left <= right)
    {
        int32_t mid = left + (right - left) / 2;
        uint8_t *entry_ptr = index_table + mid * entry_size;
        uint16_t mid_unicode = *(uint16_t *)entry_ptr;

        if (mid_unicode == unicode)
        {
            return *(uint32_t *)(entry_ptr + sizeof(uint16_t));
        }
        else if (mid_unicode < unicode)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    return 0;  /* Not found */
}

/**
 * @brief Look up a glyph offset in a direct or searchable index.
 * @param unicode Unicode code point.
 * @param ttfbin TTF header describing the index.
 * @param table_ptr Direct index source or memory-resident table.
 * @param font_mode Font source mode.
 * @param preloaded_index_table Searchable index loaded into memory.
 * @return Glyph offset, or 0 when absent.
 */
static uint32_t getGlyphOffset(uint32_t unicode, GUI_FONT_HEAD_TTF *ttfbin, uint8_t *table_ptr,
                               FONT_SRC_MODE font_mode, uint8_t *preloaded_index_table)
{
    if (ttfbin->index_method == 0)
    {
        /* Direct offset mode - bounds check against index table size */
        if (unicode >= ttfbin->index_area_size / sizeof(uint32_t))
        {
            return 0;
        }

        if (font_mode == FONT_SRC_MEMADDR)
        {
            uint32_t glyphOffset = *((uint32_t *)table_ptr + unicode);
            if (glyphOffset != 0xffffffff)
            {
                return glyphOffset;
            }
        }
        else if (font_mode == FONT_SRC_FTL)
        {
            uint32_t glyphOffset = 0;
            gui_ftl_read((uintptr_t)table_ptr + unicode * sizeof(uint32_t),
                         (uint8_t *)&glyphOffset, sizeof(uint32_t));
            if (glyphOffset != 0xffffffff)
            {
                return glyphOffset;
            }
        }
    }
    else
    {
        /* Index search mode - use binary search */
        if (font_mode == FONT_SRC_MEMADDR)
        {
            /* Directly use memory index table for binary search */
            return font_index_bsearch_ttf(table_ptr, ttfbin->index_area_size, unicode);
        }
        else if (font_mode == FONT_SRC_FTL)
        {
            /* Use preloaded index table for binary search */
            if (preloaded_index_table != NULL)
            {
                return font_index_bsearch_ttf(preloaded_index_table, ttfbin->index_area_size, unicode);
            }
        }
    }
    return 0;
}

/**
 * @brief Map coverage values to 8-bit alpha.
 * @param img_out Coverage buffer modified in place.
 * @param out_size Buffer size in bytes.
 * @param raster_prec Sub-samples per pixel axis.
 */
static void adjustImageBufferPrecision(uint8_t *img_out, uint32_t out_size, uint8_t raster_prec)
{
    uint8_t shift_bit = 0;
    switch (raster_prec)
    {
    case 1:
        return;

    case 2:
        shift_bit = 6;
        break;

    case 4:
        shift_bit = 4;
        break;

    case 8:
        shift_bit = 2;
        break;

    default:
        break;
    }

    uint32_t i = 0;
#if defined(FONT_TTF_USE_MVE) && FONT_TTF_MVE_ADJUST_PRECISION
    /* Process 16 coverage bytes per iteration with saturating left shift.
     * For valid coverage [0, raster_prec^2], saturation maps full coverage to 255. */
    for (; i + 16 <= out_size; i += 16)
    {
        uint8x16_t input = vldrbq_u8(img_out + i);
        uint8x16_t output = vqrshlq_n_u8(input, shift_bit);
        vst1q(img_out + i, output);
    }
#endif
    for (; i < out_size; i++)
    {
        if (img_out[i] == raster_prec * raster_prec)
        {
            img_out[i] = 0xff;
        }
        else
        {
            img_out[i] = img_out[i] << shift_bit;
        }
    }
}

void makeImageBuffer(uint8_t *img_out, const uint32_t *img, uint8_t raster_prec, int out_w,
                     int out_h, int render_w, int render_h, uint32_t line_word, uint32_t block_bit)
{
    (void)render_w;
    (void)block_bit;
    memset(img_out, 0, out_w * out_h);
    if (raster_prec == 4)
    {
        const uint32_t *row1 = img;
        uint8_t *dst_row = img_out;

        for (int y = 0; y < render_h;
             y += 4, row1 += line_word * 4, dst_row += out_w)
        {
            const uint32_t *src1 = row1;
            const uint32_t *src2 = row1 + line_word;
            const uint32_t *src3 = row1 + line_word * 2;
            const uint32_t *src4 = row1 + line_word * 3;
            uint8_t *dst = dst_row;
            uint32_t ux = 0;

#if defined(FONT_TTF_USE_MVE) && FONT_TTF_MVE_DOWNSAMPLE_PREC4
            /* Count four words in parallel, split even/odd nibbles into bytes,
             * then interleave them in output order. */
            const uint32x4_t mask1 = vdupq_n_u32(0x55555555u);
            const uint32x4_t mask2 = vdupq_n_u32(0x33333333u);
            const uint32x4_t mask4 = vdupq_n_u32(0x0F0F0F0Fu);
            for (; ux + 4 <= line_word;
                 ux += 4, src1 += 4, src2 += 4, src3 += 4, src4 += 4, dst += 32)
            {
#define FONT_TTF_MVE_NIBBLE_POPCOUNT(src) \
    ({ \
        uint32x4_t v = vldrwq_u32(src); \
        v = vsubq_u32(v, vandq_u32(vshrq_n_u32(v, 1), mask1)); \
        vaddq_u32(vandq_u32(v, mask2), \
                  vandq_u32(vshrq_n_u32(v, 2), mask2)); \
    })
                uint32x4_t t = FONT_TTF_MVE_NIBBLE_POPCOUNT(src1);
                uint32x4_t even = vandq_u32(t, mask4);
                uint32x4_t odd = vandq_u32(vshrq_n_u32(t, 4), mask4);

                t = FONT_TTF_MVE_NIBBLE_POPCOUNT(src2);
                even = vaddq_u32(even, vandq_u32(t, mask4));
                odd = vaddq_u32(odd, vandq_u32(vshrq_n_u32(t, 4), mask4));
                t = FONT_TTF_MVE_NIBBLE_POPCOUNT(src3);
                even = vaddq_u32(even, vandq_u32(t, mask4));
                odd = vaddq_u32(odd, vandq_u32(vshrq_n_u32(t, 4), mask4));
                t = FONT_TTF_MVE_NIBBLE_POPCOUNT(src4);
                even = vaddq_u32(even, vandq_u32(t, mask4));
                odd = vaddq_u32(odd, vandq_u32(vshrq_n_u32(t, 4), mask4));
#undef FONT_TTF_MVE_NIBBLE_POPCOUNT

                uint8x16x2_t output;
                output.val[0] = vrev32q_u8(vreinterpretq_u8_u32(odd));
                output.val[1] = vrev32q_u8(vreinterpretq_u8_u32(even));
                vst2q_u8(dst, output);
            }
#endif

            /* Scalar tail also serves as the complete non-MVE implementation. */
            for (; ux < line_word;
                 ux++, src1++, src2++, src3++, src4++, dst += 8)
            {
                uint32_t pixel1 = *src1;
                uint32_t pixel2 = *src2;
                uint32_t pixel3 = *src3;
                uint32_t pixel4 = *src4;
                /* Use OR: addition can wrap to zero and skip a covered block. */
                if ((pixel1 | pixel2 | pixel3 | pixel4) == 0)
                {
                    continue;
                }
                /* Count all eight nibbles of each source word in parallel. */
                uint32_t t1 = pixel1 - ((pixel1 >> 1) & 0x55555555u);
                uint32_t t2 = pixel2 - ((pixel2 >> 1) & 0x55555555u);
                uint32_t t3 = pixel3 - ((pixel3 >> 1) & 0x55555555u);
                uint32_t t4 = pixel4 - ((pixel4 >> 1) & 0x55555555u);
                t1 = (t1 & 0x33333333u) + ((t1 >> 2) & 0x33333333u);
                t2 = (t2 & 0x33333333u) + ((t2 >> 2) & 0x33333333u);
                t3 = (t3 & 0x33333333u) + ((t3 >> 2) & 0x33333333u);
                t4 = (t4 & 0x33333333u) + ((t4 >> 2) & 0x33333333u);

                /* Accumulate even and odd nibbles in separate byte lanes to
                 * prevent carries between fields. */
                uint32_t even = (t1 & 0x0F0F0F0Fu) + (t2 & 0x0F0F0F0Fu) +
                                (t3 & 0x0F0F0F0Fu) + (t4 & 0x0F0F0F0Fu);
                uint32_t odd = ((t1 >> 4) & 0x0F0F0F0Fu) + ((t2 >> 4) & 0x0F0F0F0Fu) +
                               ((t3 >> 4) & 0x0F0F0F0Fu) + ((t4 >> 4) & 0x0F0F0F0Fu);

                dst[0] = (odd >> 24) & 0x1f;
                dst[1] = (even >> 24) & 0x1f;
                dst[2] = (odd >> 16) & 0x1f;
                dst[3] = (even >> 16) & 0x1f;
                dst[4] = (odd >> 8) & 0x1f;
                dst[5] = (even >> 8) & 0x1f;
                dst[6] = odd & 0x1f;
                dst[7] = even & 0x1f;
            }
        }
    }
    else if (raster_prec == 2)
    {
        const uint32_t *row1 = img;
        uint8_t *dst_row = img_out;

        for (int y = 0; y < render_h;
             y += 2, row1 += line_word * 2, dst_row += out_w)
        {
            const uint32_t *src1 = row1;
            const uint32_t *src2 = row1 + line_word;
            uint8_t *dst = dst_row;

            for (uint32_t ux = 0; ux < line_word;
                 ux++, src1++, src2++, dst += 16)
            {
                uint32_t pixel1 = *src1;
                uint32_t pixel2 = *src2;
                if ((pixel1 | pixel2) == 0)
                {
                    continue;
                }
                /* Count all sixteen 2-bit fields in parallel. */
                uint32_t t1 = pixel1 - ((pixel1 >> 1) & 0x55555555u);
                uint32_t t2 = pixel2 - ((pixel2 >> 1) & 0x55555555u);

                /* Split alternating 2-bit fields into 4-bit slots before adding
                 * the two rows, preventing carries between adjacent fields. */
                uint32_t lo = (t1 & 0x33333333u) + (t2 & 0x33333333u);
                uint32_t hi = ((t1 >> 2) & 0x33333333u) + ((t2 >> 2) & 0x33333333u);

                /* Emit fields from the most-significant nibble; hi contains even
                 * pixels and lo contains odd pixels. */
#if defined(FONT_TTF_USE_MVE) && FONT_TTF_MVE_DOWNSAMPLE_PREC2
                /* Gather and shift packed nibbles into 16 output bytes.
                 * arm_mve.h supports little-endian MVE only. */
                const uint32_t packed[2] = {hi, lo};
                const uint8x16_t offsets = {3, 7, 3, 7, 2, 6, 2, 6,
                                            1, 5, 1, 5, 0, 4, 0, 4
                                           };
                const int8x16_t shifts = {-4, -4, 0, 0, -4, -4, 0, 0,
                                          -4, -4, 0, 0, -4, -4, 0, 0
                                         };
                uint8x16_t output = vldrbq_gather_offset_u8((const uint8_t *)packed, offsets);
                output = vandq_u8(vshlq_u8(output, shifts), vdupq_n_u8(0x7));
                vstrbq_u8(dst, output);
#else
                dst[0] = (hi >> 28) & 0x7;
                dst[1] = (lo >> 28) & 0x7;
                dst[2] = (hi >> 24) & 0x7;
                dst[3] = (lo >> 24) & 0x7;
                dst[4] = (hi >> 20) & 0x7;
                dst[5] = (lo >> 20) & 0x7;
                dst[6] = (hi >> 16) & 0x7;
                dst[7] = (lo >> 16) & 0x7;
                dst[8] = (hi >> 12) & 0x7;
                dst[9] = (lo >> 12) & 0x7;
                dst[10] = (hi >> 8) & 0x7;
                dst[11] = (lo >> 8) & 0x7;
                dst[12] = (hi >> 4) & 0x7;
                dst[13] = (lo >> 4) & 0x7;
                dst[14] = hi & 0x7;
                dst[15] = lo & 0x7;
#endif
            }
        }
    }
    else if (raster_prec == 1)
    {
        const uint32_t *src = img;
        const uint32_t *end = img + (size_t)render_h * line_word;
        uint32_t *dst = (uint32_t *)img_out;

        /* Expand each word into 32 coverage bytes. MVE gathers bits by lane;
         * the scalar path expands eight nibbles through a lookup table. */
#if defined(FONT_TTF_USE_MVE) && FONT_TTF_MVE_DOWNSAMPLE_PREC1
        const uint8x16_t offsets_hi = {3, 3, 3, 3, 3, 3, 3, 3,
                                       2, 2, 2, 2, 2, 2, 2, 2
                                      };
        const uint8x16_t offsets_lo = {1, 1, 1, 1, 1, 1, 1, 1,
                                       0, 0, 0, 0, 0, 0, 0, 0
                                      };
        const int8x16_t shifts = {-7, -6, -5, -4, -3, -2, -1, 0,
                                  -7, -6, -5, -4, -3, -2, -1, 0
                                 };
        const uint8x16_t bit_mask = vdupq_n_u8(1);
        const uint8x16_t zero = vdupq_n_u8(0);
#endif
        for (; src < end; src++, dst += 8)
        {
            uint32_t pixel = *src;
            if (pixel == 0)
            {
                continue;
            }

#if defined(FONT_TTF_USE_MVE) && FONT_TTF_MVE_DOWNSAMPLE_PREC1
            uint8x16_t output = vldrbq_gather_offset_u8((const uint8_t *)&pixel, offsets_hi);
            output = vandq_u8(vshlq_u8(output, shifts), bit_mask);
            vstrbq_u8((uint8_t *)dst, vsubq_u8(zero, output));

            output = vldrbq_gather_offset_u8((const uint8_t *)&pixel, offsets_lo);
            output = vandq_u8(vshlq_u8(output, shifts), bit_mask);
            vstrbq_u8((uint8_t *)(dst + 4), vsubq_u8(zero, output));
#else
            dst[0] = font_ttf_expand_4bit[(pixel >> 28) & 0xf];
            dst[1] = font_ttf_expand_4bit[(pixel >> 24) & 0xf];
            dst[2] = font_ttf_expand_4bit[(pixel >> 20) & 0xf];
            dst[3] = font_ttf_expand_4bit[(pixel >> 16) & 0xf];
            dst[4] = font_ttf_expand_4bit[(pixel >> 12) & 0xf];
            dst[5] = font_ttf_expand_4bit[(pixel >> 8) & 0xf];
            dst[6] = font_ttf_expand_4bit[(pixel >> 4) & 0xf];
            dst[7] = font_ttf_expand_4bit[pixel & 0xf];
#endif
        }
    }
    else if (raster_prec == 8)
    {
        const uint32_t *row1 = img;
        uint8_t *dst_row = img_out;

        for (int y = 0; y < render_h;
             y += 8, row1 += line_word * 8, dst_row += out_w)
        {
            const uint32_t *src1 = row1;
            const uint32_t *src2 = row1 + line_word;
            const uint32_t *src3 = row1 + line_word * 2;
            const uint32_t *src4 = row1 + line_word * 3;
            const uint32_t *src5 = row1 + line_word * 4;
            const uint32_t *src6 = row1 + line_word * 5;
            const uint32_t *src7 = row1 + line_word * 6;
            const uint32_t *src8 = row1 + line_word * 7;
            uint8_t *dst = dst_row;
            uint32_t ux = 0;

#if defined(FONT_TTF_USE_MVE) && FONT_TTF_MVE_DOWNSAMPLE_PREC8
            /* Apply byte-wise SWAR popcount to four words at once, sum eight rows,
             * then reverse bytes to restore MSB-first pixel order. */
            const uint32x4_t mask1 = vdupq_n_u32(0x55555555u);
            const uint32x4_t mask2 = vdupq_n_u32(0x33333333u);
            const uint32x4_t mask4 = vdupq_n_u32(0x0F0F0F0Fu);
            for (; ux + 4 <= line_word;
                 ux += 4, src1 += 4, src2 += 4, src3 += 4, src4 += 4,
                 src5 += 4, src6 += 4, src7 += 4, src8 += 4, dst += 16)
            {
#define FONT_TTF_MVE_BYTE_POPCOUNT(src) \
    ({ \
        uint32x4_t v = vldrwq_u32(src); \
        v = vsubq_u32(v, vandq_u32(vshrq_n_u32(v, 1), mask1)); \
        v = vaddq_u32(vandq_u32(v, mask2), \
                      vandq_u32(vshrq_n_u32(v, 2), mask2)); \
        vandq_u32(vaddq_u32(v, vshrq_n_u32(v, 4)), mask4); \
    })
                uint8x16_t sum = vreinterpretq_u8_u32(FONT_TTF_MVE_BYTE_POPCOUNT(src1));
                sum = vaddq_u8(sum, vreinterpretq_u8_u32(FONT_TTF_MVE_BYTE_POPCOUNT(src2)));
                sum = vaddq_u8(sum, vreinterpretq_u8_u32(FONT_TTF_MVE_BYTE_POPCOUNT(src3)));
                sum = vaddq_u8(sum, vreinterpretq_u8_u32(FONT_TTF_MVE_BYTE_POPCOUNT(src4)));
                sum = vaddq_u8(sum, vreinterpretq_u8_u32(FONT_TTF_MVE_BYTE_POPCOUNT(src5)));
                sum = vaddq_u8(sum, vreinterpretq_u8_u32(FONT_TTF_MVE_BYTE_POPCOUNT(src6)));
                sum = vaddq_u8(sum, vreinterpretq_u8_u32(FONT_TTF_MVE_BYTE_POPCOUNT(src7)));
                sum = vaddq_u8(sum, vreinterpretq_u8_u32(FONT_TTF_MVE_BYTE_POPCOUNT(src8)));
#undef FONT_TTF_MVE_BYTE_POPCOUNT
                vstrbq_u8(dst, vrev32q_u8(sum));
            }
#endif

            /* Scalar tail is also the non-MVE fallback. Keep its cheap per-word
             * zero check; a vector-group check would add reduction overhead. */
            for (; ux < line_word;
                 ux++, src1++, src2++, src3++, src4++,
                 src5++, src6++, src7++, src8++, dst += 4)
            {
                uint32_t pixel1 = *src1;
                uint32_t pixel2 = *src2;
                uint32_t pixel3 = *src3;
                uint32_t pixel4 = *src4;
                uint32_t pixel5 = *src5;
                uint32_t pixel6 = *src6;
                uint32_t pixel7 = *src7;
                uint32_t pixel8 = *src8;

                if ((pixel1 | pixel2 | pixel3 | pixel4 |
                     pixel5 | pixel6 | pixel7 | pixel8) == 0)
                {
                    continue;
                }

                /* Count each word by byte; the eight-row sum fits in byte lanes. */
                uint32_t sum = font_ttf_popcount_bytes(pixel1) +
                               font_ttf_popcount_bytes(pixel2) +
                               font_ttf_popcount_bytes(pixel3) +
                               font_ttf_popcount_bytes(pixel4) +
                               font_ttf_popcount_bytes(pixel5) +
                               font_ttf_popcount_bytes(pixel6) +
                               font_ttf_popcount_bytes(pixel7) +
                               font_ttf_popcount_bytes(pixel8);

                dst[0] = (sum >> 24) & 0xff;
                dst[1] = (sum >> 16) & 0xff;
                dst[2] = (sum >> 8) & 0xff;
                dst[3] = sum & 0xff;
            }
        }
    }
}

/*============================================================================*
 *                      ttf font Library Functions
 *============================================================================*/

uint8_t gui_font_ttf_init_ftl(uint8_t *font_bin_addr)
{
    if (!font_bin_addr)
    {
        return UINT8_MAX;
    }

    /* Check if already loaded */
    FONT_LIB_NODE *node = gui_font_lib_find(font_bin_addr);
    if (node != NULL && node->font_type == GUI_FONT_SRC_TTF)
    {
        gui_font_lib_addref(node);
        return 0;
    }

    /* Read header */
    uint8_t *data = gui_malloc(sizeof(GUI_FONT_HEAD_TTF));
    if (data == NULL)
    {
        return UINT8_MAX;
    }
    gui_ftl_read((uintptr_t)font_bin_addr, data, sizeof(GUI_FONT_HEAD_TTF));

    GUI_FONT_HEAD_TTF *font = (GUI_FONT_HEAD_TTF *)data;
    if (font->file_type != FONT_FILE_TTF_FLAG)
    {
        gui_log("gui_font_ttf_init_ftl: invalid TTF file\n");
        gui_free(data);
        return UINT8_MAX;
    }

    /* Read header + index area */
    uint32_t head_index_len = font->head_length + font->index_area_size;
    data = gui_realloc(data, head_index_len);
    if (data == NULL)
    {
        return UINT8_MAX;
    }
    gui_ftl_read((uintptr_t)font_bin_addr, data, head_index_len);
    font = (GUI_FONT_HEAD_TTF *)data;

    /* Register to font_lib_manager */
    node = gui_font_lib_register(font_bin_addr, font->font_size, FONT_SRC_FTL,
                                 GUI_FONT_SRC_TTF, data, head_index_len);
    if (node == NULL)
    {
        gui_free(data);
        return UINT8_MAX;
    }

    gui_log("gui_font_ttf_init_ftl: loaded TTF, size=%d\n", font->font_size);
    return 0;
}

uint8_t gui_font_ttf_init_fs(uint8_t *font_path)
{
    if (!font_path)
    {
        return UINT8_MAX;
    }

    /* Check if already loaded */
    FONT_LIB_NODE *node = gui_font_lib_find(font_path);
    if (node != NULL && node->font_type == GUI_FONT_SRC_TTF)
    {
        gui_font_lib_addref(node);
        return 0;
    }

    /* Open font file */
    gui_vfs_file_t *file = gui_vfs_open((const char *)font_path, GUI_VFS_READ);
    if (file == NULL)
    {
        gui_log("gui_font_ttf_init_fs: failed to open %s\n", font_path);
        return UINT8_MAX;
    }

    /* Read header */
    uint8_t *data = gui_malloc(sizeof(GUI_FONT_HEAD_TTF));
    if (data == NULL)
    {
        gui_vfs_close(file);
        return UINT8_MAX;
    }

    int read_size = gui_vfs_read(file, data, sizeof(GUI_FONT_HEAD_TTF));
    if (read_size != sizeof(GUI_FONT_HEAD_TTF))
    {
        gui_log("gui_font_ttf_init_fs: failed to read header\n");
        gui_free(data);
        gui_vfs_close(file);
        return UINT8_MAX;
    }

    GUI_FONT_HEAD_TTF *font = (GUI_FONT_HEAD_TTF *)data;
    if (font->file_type != FONT_FILE_TTF_FLAG)
    {
        gui_log("gui_font_ttf_init_fs: invalid TTF file\n");
        gui_free(data);
        gui_vfs_close(file);
        return UINT8_MAX;
    }

    /* Read header + index area */
    uint32_t head_index_len = font->head_length + font->index_area_size;
    data = gui_realloc(data, head_index_len);
    if (data == NULL)
    {
        gui_vfs_close(file);
        return UINT8_MAX;
    }

    gui_vfs_seek(file, 0, GUI_VFS_SEEK_SET);
    read_size = gui_vfs_read(file, data, head_index_len);
    gui_vfs_close(file);

    if (read_size != (int)head_index_len)
    {
        gui_log("gui_font_ttf_init_fs: failed to read index area\n");
        gui_free(data);
        return UINT8_MAX;
    }

    font = (GUI_FONT_HEAD_TTF *)data;

    /* Register to font_lib_manager */
    node = gui_font_lib_register(font_path, font->font_size, FONT_SRC_FILESYS,
                                 GUI_FONT_SRC_TTF, data, head_index_len);
    if (node == NULL)
    {
        gui_free(data);
        return UINT8_MAX;
    }

    gui_log("gui_font_ttf_init_fs: loaded %s, size=%d\n", font_path, font->font_size);
    return 0;
}

uint8_t gui_font_ttf_init_mem(uint8_t *font_bin_addr)
{
    if (!font_bin_addr)
    {
        return UINT8_MAX;
    }

    /* Check if already loaded */
    FONT_LIB_NODE *node = gui_font_lib_find(font_bin_addr);
    if (node != NULL && node->font_type == GUI_FONT_SRC_TTF)
    {
        gui_font_lib_addref(node);
        return 0;
    }

    GUI_FONT_HEAD_TTF *font = (GUI_FONT_HEAD_TTF *)font_bin_addr;
    if (font->file_type != FONT_FILE_TTF_FLAG)
    {
        gui_log("gui_font_ttf_init_mem: invalid TTF file\n");
        return UINT8_MAX;
    }

    /* Register to font_lib_manager (no cached data for MEM mode) */
    node = gui_font_lib_register(font_bin_addr, font->font_size, FONT_SRC_MEMADDR,
                                 GUI_FONT_SRC_TTF, NULL, 0);
    if (node == NULL)
    {
        return UINT8_MAX;
    }

    return 0;
}

uint8_t gui_font_ttf_delete(uint8_t *font_bin_addr)
{
    if (!font_bin_addr)
    {
        return UINT8_MAX;
    }

    gui_font_lib_release(font_bin_addr);
    return 0;
}

/**
 * @brief Return the cached TTF header.
 * @param font_file Font resource identifier.
 * @return Cached header, or NULL if unavailable.
 */
static GUI_FONT_HEAD_TTF *get_cached_ttf_header(uint8_t *font_file)
{
    FONT_LIB_NODE *node = gui_font_lib_find(font_file);
    if (node != NULL && node->font_type == GUI_FONT_SRC_TTF && node->cached_data != NULL)
    {
        return (GUI_FONT_HEAD_TTF *)node->cached_data;
    }
    return NULL;
}

/**
 * @brief Return the cached TTF index table.
 * @param font_file Font resource identifier.
 * @return Cached index table, or NULL if unavailable.
 */
static uint8_t *get_cached_ttf_index_table(uint8_t *font_file)
{
    FONT_LIB_NODE *node = gui_font_lib_find(font_file);
    if (node != NULL && node->font_type == GUI_FONT_SRC_TTF && node->cached_data != NULL)
    {
        GUI_FONT_HEAD_TTF *header = (GUI_FONT_HEAD_TTF *)node->cached_data;
        return node->cached_data + header->head_length;
    }
    return NULL;
}

/* Search registered TTF fonts by priority, excluding the primary font.
 * Supports MEMADDR, FTL, and FILESYS sources. */
int gui_font_ttf_fallback_search(uint32_t unicode, uint16_t font_height,
                                 uint8_t bold_weight, uint8_t *skip_file,
                                 mem_char_t *out_chr)
{
    uint16_t min_prio = 0;
    while (min_prio <= 0xFF)
    {
        /* Find next candidate with lowest priority >= min_prio */
        FONT_LIB_NODE *best = NULL;
        uint8_t best_prio = 0xFF;
        FONT_LIB_NODE *node = gui_font_lib_get_head();
        while (node != NULL)
        {
            if (node->font_type == GUI_FONT_SRC_TTF &&
                node->font_file != skip_file &&
                node->priority >= min_prio &&
                (best == NULL || node->priority < best_prio))
            {
                best = node;
                best_prio = node->priority;
            }
            node = node->next;
        }

        if (best == NULL)
        {
            break;
        }

        /* Get TTF header */
        GUI_FONT_HEAD_TTF *ttfbin = get_cached_ttf_header(best->font_file);
        bool need_free = false;

        if (ttfbin == NULL)
        {
            if (best->src_mode == FONT_SRC_MEMADDR)
            {
                ttfbin = (GUI_FONT_HEAD_TTF *)best->font_file;
            }
            else if (best->src_mode == FONT_SRC_FTL)
            {
                ttfbin = gui_malloc(sizeof(GUI_FONT_HEAD_TTF));
                if (ttfbin == NULL) { min_prio = best_prio + 1; continue; }
                gui_ftl_read((uintptr_t)best->font_file, (uint8_t *)ttfbin, sizeof(GUI_FONT_HEAD_TTF));
                need_free = true;
            }
            else if (best->src_mode == FONT_SRC_FILESYS)
            {
                gui_vfs_file_t *file = gui_vfs_open((const char *)best->font_file, GUI_VFS_READ);
                if (file == NULL) { min_prio = best_prio + 1; continue; }
                ttfbin = gui_malloc(sizeof(GUI_FONT_HEAD_TTF));
                if (ttfbin == NULL) { gui_vfs_close(file); min_prio = best_prio + 1; continue; }
                gui_vfs_read(file, ttfbin, sizeof(GUI_FONT_HEAD_TTF));
                gui_vfs_close(file);
                need_free = true;
            }
        }

        if (ttfbin == NULL || ttfbin->file_type != FONT_FILE_TTF_FLAG)
        {
            if (need_free && ttfbin) { gui_free(ttfbin); }
            min_prio = (best_prio < 0xFF) ? best_prio + 1 : 0xFF + 1;
            continue;
        }

        /* Get index table */
        uint8_t *index_table_ptr = get_cached_ttf_index_table(best->font_file);
        uint8_t *alloc_index = NULL;

        if (index_table_ptr == NULL)
        {
            if (best->src_mode == FONT_SRC_MEMADDR)
            {
                index_table_ptr = best->font_file + ttfbin->head_length;
            }
            else if (best->src_mode == FONT_SRC_FTL && ttfbin->index_area_size > 0)
            {
                alloc_index = gui_malloc(ttfbin->index_area_size);
                if (alloc_index)
                {
                    gui_ftl_read((uintptr_t)best->font_file + ttfbin->head_length,
                                 alloc_index, ttfbin->index_area_size);
                    index_table_ptr = alloc_index;
                }
            }
            else if (best->src_mode == FONT_SRC_FILESYS && ttfbin->index_area_size > 0)
            {
                alloc_index = gui_malloc(ttfbin->index_area_size);
                if (alloc_index)
                {
                    gui_vfs_file_t *file = gui_vfs_open((const char *)best->font_file, GUI_VFS_READ);
                    if (file)
                    {
                        gui_vfs_seek(file, ttfbin->head_length, GUI_VFS_SEEK_SET);
                        gui_vfs_read(file, alloc_index, ttfbin->index_area_size);
                        gui_vfs_close(file);
                        index_table_ptr = alloc_index;
                    }
                    else
                    {
                        gui_free(alloc_index);
                        alloc_index = NULL;
                    }
                }
            }
        }

        GUI_ASSERT(index_table_ptr != NULL);
        /* Search for the glyph */
        uint32_t ttfoffset = getGlyphOffset(unicode, ttfbin, index_table_ptr,
                                            (best->src_mode == FONT_SRC_FILESYS) ?
                                            FONT_SRC_MEMADDR : best->src_mode,
                                            index_table_ptr);

        if (ttfoffset != 0)
        {
#if ENABLE_FONT_V3_TYPO
            gui_font_typo_context_t fb_typo_ctx = gui_font_ttf_get_typo_context(ttfbin, font_height);
            float scale;
            if (fb_typo_ctx.is_v3)
            {
                scale = (float)font_height / fb_typo_ctx.metrics.units_per_em;
            }
            else
            {
                scale = (float)font_height / (ttfbin->ascent - ttfbin->descent);
            }
#else
            float scale = (float)font_height / (ttfbin->ascent - ttfbin->descent);
#endif /* ENABLE_FONT_V3_TYPO */

            if (best->src_mode == FONT_SRC_MEMADDR)
            {
                FontGlyphData *glyphData = (FontGlyphData *)(best->font_file + ttfoffset);
#if ENABLE_FONT_V3_TYPO
                ttf_populate_glyph_metrics(out_chr, glyphData, unicode, font_height,
                                           scale, bold_weight, &fb_typo_ctx,
                                           best->font_file + ttfoffset);
#else
                out_chr->unicode = unicode;
                out_chr->h = font_height;
                out_chr->char_h = font_height;
                out_chr->dot_addr = best->font_file + ttfoffset;
                out_chr->char_w = glyphData->advance * scale + bold_weight * 2;
                if (out_chr->char_w == 0 && glyphData->x1 > glyphData->x0)
                {
                    int _vis = (int)roundf((glyphData->x1 - glyphData->x0) * scale) + bold_weight * 2;
                    out_chr->char_w = (uint16_t)(_vis > 0 ? _vis : 1);
                }
#endif

                if (alloc_index) { gui_free(alloc_index); }
                if (need_free) { gui_free(ttfbin); }
                // gui_log("[TTF fallback] U+%04X: primary=%p -> fallback=%p (prio=%d, MEMADDR)\n",
                //         unicode, skip_file, best->font_file, best->priority);
                return 0;
            }
            else if (best->src_mode == FONT_SRC_FTL)
            {
                uint8_t *font_ptr = best->font_file + ttfoffset;
                FontGlyphData *glyphData = gui_malloc(sizeof(FontGlyphData));
                if (glyphData)
                {
                    gui_ftl_read((uintptr_t)font_ptr, (uint8_t *)glyphData, sizeof(FontGlyphData));
                    int line_count = 0;
                    uint8_t winding_length = glyphData->winding_count;
                    uint8_t *winding_lengths = gui_malloc(winding_length);
                    if (winding_lengths)
                    {
                        gui_ftl_read((uintptr_t)(font_ptr + offsetof(FontGlyphData, winding_lengths)),
                                     winding_lengths, winding_length);
                        for (int i = 0; i < glyphData->winding_count; i++)
                        {
                            line_count += winding_lengths[i];
                        }
                        uint8_t *dot_addr = gui_malloc(sizeof(FontGlyphData) + winding_length +
                                                       line_count * sizeof(FontWindings));
                        if (dot_addr)
                        {
                            memcpy(dot_addr, glyphData, sizeof(FontGlyphData));
                            memcpy(dot_addr + offsetof(FontGlyphData, winding_lengths),
                                   winding_lengths, winding_length);
                            gui_ftl_read((uintptr_t)font_ptr + offsetof(FontGlyphData, winding_lengths) +
                                         winding_length,
                                         dot_addr + offsetof(FontGlyphData, winding_lengths) + winding_length,
                                         line_count * sizeof(FontWindings));

#if ENABLE_FONT_V3_TYPO
                            ttf_populate_glyph_metrics(out_chr, glyphData, unicode, font_height,
                                                       scale, bold_weight, &fb_typo_ctx, dot_addr);
#else
                            out_chr->unicode = unicode;
                            out_chr->h = font_height;
                            out_chr->char_h = font_height;
                            out_chr->dot_addr = dot_addr;
                            out_chr->char_w = glyphData->advance * scale + bold_weight * 2;
                            if (out_chr->char_w == 0 && glyphData->x1 > glyphData->x0)
                            {
                                int _vis = (int)roundf((glyphData->x1 - glyphData->x0) * scale) + bold_weight * 2;
                                out_chr->char_w = (uint16_t)(_vis > 0 ? _vis : 1);
                            }
#endif

                            gui_free(winding_lengths);
                            gui_free(glyphData);
                            if (alloc_index) { gui_free(alloc_index); }
                            if (need_free) { gui_free(ttfbin); }
                            // gui_log("[TTF fallback] U+%04X: primary=%p -> fallback=%p (prio=%d, FTL)\n",
                            //         unicode, skip_file, best->font_file, best->priority);
                            return 0;
                        }
                        gui_free(winding_lengths);
                    }
                    gui_free(glyphData);
                }
            }
            else if (best->src_mode == FONT_SRC_FILESYS)
            {
                gui_vfs_file_t *file = gui_vfs_open((const char *)best->font_file, GUI_VFS_READ);
                if (file)
                {
                    FontGlyphData *glyphData = gui_malloc(sizeof(FontGlyphData));
                    if (glyphData)
                    {
                        gui_vfs_seek(file, ttfoffset, GUI_VFS_SEEK_SET);
                        gui_vfs_read(file, glyphData, sizeof(FontGlyphData));
                        int line_count = 0;
                        uint8_t winding_length = glyphData->winding_count;
                        uint8_t *winding_lengths = gui_malloc(winding_length);
                        if (winding_lengths)
                        {
                            gui_vfs_seek(file, ttfoffset + offsetof(FontGlyphData, winding_lengths),
                                         GUI_VFS_SEEK_SET);
                            gui_vfs_read(file, winding_lengths, winding_length);
                            for (int i = 0; i < glyphData->winding_count; i++)
                            {
                                line_count += winding_lengths[i];
                            }
                            uint8_t *dot_addr = gui_malloc(sizeof(FontGlyphData) + winding_length +
                                                           line_count * sizeof(FontWindings));
                            if (dot_addr)
                            {
                                memcpy(dot_addr, glyphData, sizeof(FontGlyphData));
                                memcpy(dot_addr + offsetof(FontGlyphData, winding_lengths),
                                       winding_lengths, winding_length);
                                gui_vfs_seek(file,
                                             ttfoffset + offsetof(FontGlyphData, winding_lengths) + winding_length,
                                             GUI_VFS_SEEK_SET);
                                gui_vfs_read(file,
                                             dot_addr + offsetof(FontGlyphData, winding_lengths) + winding_length,
                                             line_count * sizeof(FontWindings));

#if ENABLE_FONT_V3_TYPO
                                ttf_populate_glyph_metrics(out_chr, glyphData, unicode, font_height,
                                                           scale, bold_weight, &fb_typo_ctx, dot_addr);
#else
                                out_chr->unicode = unicode;
                                out_chr->h = font_height;
                                out_chr->char_h = font_height;
                                out_chr->dot_addr = dot_addr;
                                out_chr->char_w = glyphData->advance * scale + bold_weight * 2;
                                if (out_chr->char_w == 0 && glyphData->x1 > glyphData->x0)
                                {
                                    int _vis = (int)roundf((glyphData->x1 - glyphData->x0) * scale) + bold_weight * 2;
                                    out_chr->char_w = (uint16_t)(_vis > 0 ? _vis : 1);
                                }
#endif

                                gui_free(winding_lengths);
                                gui_free(glyphData);
                                gui_vfs_close(file);
                                if (alloc_index) { gui_free(alloc_index); }
                                if (need_free) { gui_free(ttfbin); }
                                // gui_log("[TTF fallback] U+%04X: primary=%p -> fallback=%p (prio=%d, FILESYS)\n",
                                //         unicode, skip_file, best->font_file, best->priority);
                                return 0;
                            }
                            gui_free(winding_lengths);
                        }
                        gui_free(glyphData);
                    }
                    gui_vfs_close(file);
                }
            }
        }

        if (alloc_index) { gui_free(alloc_index); }
        if (need_free) { gui_free(ttfbin); }
        min_prio = (best_prio < 0xFF) ? best_prio + 1 : 0xFF + 1;
    }

    return -1;
}

#if ENABLE_FONT_V3_TYPO
gui_font_typo_context_t gui_font_ttf_get_typo_context(const GUI_FONT_HEAD_TTF *header,
                                                      uint16_t font_height)
{
    gui_font_typo_context_t ctx;
    memset(&ctx, 0, sizeof(ctx));

    if (header->version[0] >= 3)
    {
        /* V3: parse units_per_em from header extension (last 2 bytes of header) */
        uint16_t units_per_em = *(uint16_t *)((uint8_t *)header + header->head_length - 2);

        if (units_per_em == 0)
        {
            gui_log("Warning: units_per_em is 0, falling back to ascent - descent\n");
            units_per_em = header->ascent - header->descent;
        }

        ctx.metrics.ascender = header->ascent;
        ctx.metrics.descender = header->descent;
        ctx.metrics.line_gap = header->lineGap;
        ctx.metrics.units_per_em = units_per_em;

        /* Reuse existing calc function (but handle font_height > 255) */
        /* gui_font_typo_calc_layout() accepts uint8_t font_size */
        if (font_height <= 255)
        {
            gui_font_typo_layout_t layout = gui_font_typo_calc_layout(&ctx.metrics,
                                                                      (uint8_t)font_height);
            ctx.is_v3 = true;
            ctx.baseline_px = layout.baseline;
            ctx.default_line_height = layout.line_height;
        }
        else
        {
            /* Inline calculation for large font sizes (font_height > 255) */
            ctx.is_v3 = true;
            ctx.baseline_px = (int16_t)((int32_t)header->ascent * font_height / units_per_em);
            ctx.default_line_height = (int16_t)((int32_t)(header->ascent - header->descent +
                                                          header->lineGap) * font_height / units_per_em);
        }
    }
    else
    {
        /* Deprecated legacy layout for pre-v3 fonts.
         * Regenerate with Font_Tool v3+ before removing this path. */
        ctx.is_v3 = false;
        ctx.baseline_px = 0;
        ctx.default_line_height = (int16_t)font_height;
    }

    return ctx;
}

/**
 * @copydoc ttf_populate_glyph_metrics
 */
static void ttf_populate_glyph_metrics(mem_char_t *chr, const FontGlyphData *glyphData,
                                       uint32_t unicode, uint16_t font_height,
                                       float scale, uint8_t bold_weight,
                                       const gui_font_typo_context_t *typo_ctx,
                                       uint8_t *dot_addr)
{
    chr->unicode = unicode;
    chr->h = font_height;
    chr->char_h = font_height;
    chr->dot_addr = dot_addr;

    if (typo_ctx->is_v3)
    {
        /* V3 standard: compute bearing/advance from font units */

        /* bearing_x = round(sx0 * scale), clamp to int8_t */
        float raw_bx = glyphData->x0 * scale;
        int16_t bx = (int16_t)roundf(raw_bx);
        if (bx < -128 || bx > 127)
        {
            gui_log("Warning: bearing_x overflow for U+%04X: %d, clamping to int8_t\n",
                    unicode, bx);
            bx = (bx < -128) ? -128 : 127;
        }
        chr->bearing_x = (int8_t)bx;

        /* bearing_y = round(-sy0 * scale), clamp to int8_t */
        /* sy0 is negative for glyphs above baseline (Y-flipped coordinate),
         * so -sy0 gives the distance from baseline to glyph top in font units.
         * This matches bitmap V3 bearingY = round(bbox.y2 * scale). */
        float raw_by = -glyphData->y0 * scale;
        int16_t by = (int16_t)roundf(raw_by);
        if (by < -128 || by > 127)
        {
            gui_log("Warning: bearing_y overflow for U+%04X: %d, clamping to int8_t\n",
                    unicode, by);
            by = (by < -128) ? -128 : 127;
        }
        chr->bearing_y = (int8_t)by;

        /* advance = round(advance_fu * scale) + bold_weight * 2 */
        int16_t raw_advance = (int16_t)roundf(glyphData->advance * scale);
        uint16_t final_advance = (uint16_t)(raw_advance + bold_weight * 2);
        chr->advance = final_advance;
        chr->char_w = final_advance;  /* for compatibility with width sum */
        /* Combining marks (advance=0) have visual extent but zero advance.
         * Set char_w from bbox so the draw loop does not skip them. */
        if (final_advance == 0 && glyphData->x1 > glyphData->x0)
        {
            int vis_w = (int)roundf((glyphData->x1 - glyphData->x0) * scale) + bold_weight * 2;
            chr->char_w = (uint16_t)(vis_w > 0 ? vis_w : 1);
        }
    }
    else
    {
        /* Deprecated width calculation for pre-v3 fonts. */
        chr->char_w = glyphData->advance * scale + bold_weight * 2;
        /* Combining marks (advance=0): set char_w from bbox to avoid draw-loop skip. */
        if (chr->char_w == 0 && glyphData->x1 > glyphData->x0)
        {
            int vis_w = (int)roundf((glyphData->x1 - glyphData->x0) * scale) + bold_weight * 2;
            chr->char_w = (uint16_t)(vis_w > 0 ? vis_w : 1);
        }
    }
}
#endif /* ENABLE_FONT_V3_TYPO */

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Load glyph metadata for a text object.
 * @param text Text object to populate.
 */
static void font_ttf_get_info(gui_text_t *text)
{
    GUI_ASSERT(text->path != NULL);
    GUI_FONT_HEAD_TTF *ttfbin;
    bool need_free_ttfbin = false;

    /* Try to get cached header first */
    ttfbin = get_cached_ttf_header(text->path);

    if (ttfbin == NULL)
    {
        /* Not cached, load from source */
        if (text->font_mode == FONT_SRC_MEMADDR)
        {
            ttfbin = (GUI_FONT_HEAD_TTF *)text->path;
        }
        else if (text->font_mode == FONT_SRC_FTL)
        {
            ttfbin = gui_malloc(sizeof(GUI_FONT_HEAD_TTF));
            GUI_ASSERT(ttfbin != NULL);
            gui_ftl_read((uintptr_t)text->path, (uint8_t *)ttfbin, sizeof(GUI_FONT_HEAD_TTF));
            need_free_ttfbin = true;
        }
        else if (text->font_mode == FONT_SRC_FILESYS)
        {
            gui_vfs_file_t *file = gui_vfs_open((const char *)text->path, GUI_VFS_READ);
            if (file == NULL)
            {
                gui_log("TTF fileSYS: failed to open %s\n", (const char *)text->path);
                return;
            }
            ttfbin = gui_malloc(sizeof(GUI_FONT_HEAD_TTF));
            GUI_ASSERT(ttfbin != NULL);
            gui_vfs_read(file, ttfbin, sizeof(GUI_FONT_HEAD_TTF));
            gui_vfs_close(file);
            need_free_ttfbin = true;
        }
        else
        {
            GUI_ASSERT(NULL != NULL);
            return;
        }
    }

    if (ttfbin->file_type != FONT_FILE_TTF_FLAG)
    {
        gui_log("this ttf-bin font file is not valid \n");
        if (need_free_ttfbin)
        {
            gui_free(ttfbin);
        }
        return;
    }

    uint32_t *unicode_buf = NULL;
    uint16_t unicode_len = 0;
    unicode_len = process_content_by_charset(text->charset, text->content, text->len, &unicode_buf);
    if (unicode_len == 0)
    {
        gui_log("Warning! After process, unicode len of text: %s is 0!\n", text->base.name);
        text->font_len = 0;
        if (need_free_ttfbin)
        {
            gui_free(ttfbin);
        }
        return;
    }
    if (text->arabic)
    {
        unicode_len = process_ap_unicode(unicode_buf, unicode_len);
    }
    mem_char_t *chr = gui_malloc(sizeof(mem_char_t) * unicode_len);
    if (chr == NULL)
    {
        GUI_ASSERT(NULL != NULL);
        gui_free(unicode_buf);
        if (need_free_ttfbin)
        {
            gui_free(ttfbin);
        }
        return;
    }
    memset(chr, 0, sizeof(mem_char_t) * unicode_len);
    text->data = chr;
    text->content_refresh = false;

    int32_t all_char_w = 0;
    uint32_t line_flag = 0;
    uint32_t uni_i = 0;
    uint32_t chr_i = 0;

    /* Build typography context once per text widget (not per-glyph) */
#if ENABLE_FONT_V3_TYPO
    gui_font_typo_context_t typo_ctx = gui_font_ttf_get_typo_context(ttfbin, text->font_height);

    float scale;
    if (typo_ctx.is_v3)
    {
        /* V3 standard: scale = font_height / units_per_em */
        scale = (float)text->font_height / typo_ctx.metrics.units_per_em;
    }
    else
    {
        /* Deprecated scale calculation for pre-v3 fonts. */
        scale = (float)text->font_height / (ttfbin->ascent - ttfbin->descent);
    }
#else
    float scale = (float)text->font_height / (ttfbin->ascent - ttfbin->descent);
#endif /* ENABLE_FONT_V3_TYPO */

    int16_t emoji_baseline_px = 0;
#if ENABLE_FONT_V3_TYPO
    emoji_baseline_px = typo_ctx.baseline_px;
#endif

    uint8_t *preloaded_index_table = NULL;
    bool need_free_index_table = false;

    /* Try to use cached index table first */
    preloaded_index_table = get_cached_ttf_index_table(text->path);

    if (preloaded_index_table == NULL)
    {
        /* Not cached, load from source */
        if (text->font_mode == FONT_SRC_FTL && ttfbin->index_method == 1 && ttfbin->index_area_size > 0)
        {
            preloaded_index_table = gui_malloc(ttfbin->index_area_size);
            if (preloaded_index_table != NULL)
            {
                gui_ftl_read((uintptr_t)text->path + ttfbin->head_length,
                             preloaded_index_table, ttfbin->index_area_size);
                need_free_index_table = true;
            }
        }
        else if (text->font_mode == FONT_SRC_FILESYS && ttfbin->index_method == 1 &&
                 ttfbin->index_area_size > 0)
        {
            preloaded_index_table = gui_malloc(ttfbin->index_area_size);
            if (preloaded_index_table != NULL)
            {
                gui_vfs_file_t *file = gui_vfs_open((const char *)text->path, GUI_VFS_READ);
                if (file != NULL)
                {
                    gui_vfs_seek(file, ttfbin->head_length, GUI_VFS_SEEK_SET);
                    gui_vfs_read(file, preloaded_index_table, ttfbin->index_area_size);
                    gui_vfs_close(file);
                    need_free_index_table = true;
                }
                else
                {
                    gui_free(preloaded_index_table);
                    preloaded_index_table = NULL;
                }
            }
        }
    }

    for (uni_i = 0; uni_i < unicode_len; uni_i++)
    {
        chr[chr_i].unicode = unicode_buf[uni_i];
        if (unicode_buf[uni_i] == 0x0A)
        {
            line_flag ++;

            chr[chr_i].unicode = unicode_buf[uni_i];
            // chr[chr_i].x = 0;
            // chr[chr_i].y = 0;
            // chr[chr_i].w = 0;
            chr[chr_i].h = text->font_height;
            // chr[chr_i].char_y = 0;
            chr[chr_i].char_w = 0;
            chr[chr_i].char_h = 0;
            // chr[chr_i].dot_addr = 0;
        }
        else if (unicode_buf[uni_i] == 0x20)
        {
            chr[chr_i].unicode = unicode_buf[uni_i];
            // chr[chr_i].x = 0;
            // chr[chr_i].y = 0;
            // chr[chr_i].w = 0;
            chr[chr_i].h = text->font_height;
            // chr[chr_i].char_y = 0;
            uint16_t fallback_space_width = text->space_width > 0 ?
                                            text->space_width : (text->font_height + 3) / 4;

#if ENABLE_FONT_V3_TYPO
            if (typo_ctx.is_v3)
            {
                /* V3 standard: look up space glyph advance from font data */
                uint8_t *space_index_table_ptr = NULL;
                uint8_t *space_fs_index_table = NULL;

                if (text->font_mode == FONT_SRC_MEMADDR)
                {
                    space_index_table_ptr = (uint8_t *)text->path + ttfbin->head_length;
                }
                else if (text->font_mode == FONT_SRC_FILESYS && ttfbin->index_method == 0 &&
                         ttfbin->index_area_size > 0)
                {
                    /* FILESYS index_method=0: load index table for space lookup */
                    space_fs_index_table = gui_malloc(ttfbin->index_area_size);
                    if (space_fs_index_table != NULL)
                    {
                        gui_vfs_file_t *idx_file = gui_vfs_open((const char *)text->path, GUI_VFS_READ);
                        if (idx_file != NULL)
                        {
                            gui_vfs_seek(idx_file, ttfbin->head_length, GUI_VFS_SEEK_SET);
                            gui_vfs_read(idx_file, space_fs_index_table, ttfbin->index_area_size);
                            gui_vfs_close(idx_file);
                            space_index_table_ptr = space_fs_index_table;
                        }
                        else
                        {
                            gui_free(space_fs_index_table);
                            space_fs_index_table = NULL;
                        }
                    }
                }
                else
                {
                    space_index_table_ptr = preloaded_index_table;
                }

                uint32_t space_offset = 0;
                if (space_index_table_ptr != NULL)
                {
                    space_offset = getGlyphOffset(0x20, ttfbin, space_index_table_ptr,
                                                  text->font_mode == FONT_SRC_FILESYS ?
                                                  FONT_SRC_MEMADDR : text->font_mode,
                                                  preloaded_index_table);
                }

                if (space_offset != 0)
                {
                    if (text->font_mode == FONT_SRC_MEMADDR)
                    {
                        FontGlyphData *spaceGlyph = (FontGlyphData *)((uint8_t *)text->path + space_offset);
                        chr[chr_i].char_w = (uint8_t)roundf(spaceGlyph->advance * scale);
                        chr[chr_i].advance = chr[chr_i].char_w;
                    }
                    else if (text->font_mode == FONT_SRC_FTL)
                    {
                        FontGlyphData spaceGlyphData;
                        gui_ftl_read((uintptr_t)((uint8_t *)text->path + space_offset),
                                     (uint8_t *)&spaceGlyphData, sizeof(FontGlyphData));
                        chr[chr_i].char_w = (uint8_t)roundf(spaceGlyphData.advance * scale);
                        chr[chr_i].advance = chr[chr_i].char_w;
                    }
                    else if (text->font_mode == FONT_SRC_FILESYS)
                    {
                        gui_vfs_file_t *space_file = gui_vfs_open((const char *)text->path, GUI_VFS_READ);
                        if (space_file != NULL)
                        {
                            FontGlyphData spaceGlyphData;
                            gui_vfs_seek(space_file, space_offset, GUI_VFS_SEEK_SET);
                            gui_vfs_read(space_file, &spaceGlyphData, sizeof(FontGlyphData));
                            gui_vfs_close(space_file);
                            chr[chr_i].char_w = (uint8_t)roundf(spaceGlyphData.advance * scale);
                            chr[chr_i].advance = chr[chr_i].char_w;
                        }
                        else
                        {
                            /* Fallback if file open fails */
                            chr[chr_i].char_w = fallback_space_width;
                        }
                    }
                }
                else
                {
                    /* Space glyph not found: fallback to legacy approximation */
                    chr[chr_i].char_w = fallback_space_width;
                }

                if (space_fs_index_table != NULL)
                {
                    gui_free(space_fs_index_table);
                }
            }
            else
#endif /* ENABLE_FONT_V3_TYPO */
            {
                /* Deprecated space-width fallback for pre-v3 fonts. */
                chr[chr_i].char_w = fallback_space_width;
            }

            chr[chr_i].char_h = (text->font_height + 3) / 4;
            // chr[chr_i].dot_addr = 0;
        }
        else if (gui_font_try_load_emoji(&chr[chr_i], text, emoji_baseline_px,
                                         unicode_buf, &uni_i, unicode_len))
        {
            /* Emoji handled; uni_i may be advanced for a sequence. */
        }
        else
        {
            uint8_t *index_table_ptr = NULL;
            uint8_t *fs_index_table = NULL;  /* For FILESYS index_method=0 */

            if (text->font_mode == FONT_SRC_FILESYS)
            {
                /* For FILESYS, we need to load index table from file for index_method=0 */
                if (ttfbin->index_method == 0 && ttfbin->index_area_size > 0)
                {
                    fs_index_table = gui_malloc(ttfbin->index_area_size);
                    if (fs_index_table != NULL)
                    {
                        gui_vfs_file_t *file = gui_vfs_open((const char *)text->path, GUI_VFS_READ);
                        if (file != NULL)
                        {
                            gui_vfs_seek(file, ttfbin->head_length, GUI_VFS_SEEK_SET);
                            gui_vfs_read(file, fs_index_table, ttfbin->index_area_size);
                            gui_vfs_close(file);
                            index_table_ptr = fs_index_table;
                        }
                        else
                        {
                            gui_free(fs_index_table);
                            fs_index_table = NULL;
                        }
                    }
                }
                else
                {
                    /* index_method=1 uses preloaded_index_table */
                    index_table_ptr = preloaded_index_table;
                }
            }
            else
            {
                index_table_ptr = (uint8_t *)text->path + ttfbin->head_length;
            }

            /* Skip the glyph when no index table is available. */
            if (index_table_ptr == NULL)
            {
                if (fs_index_table != NULL)
                {
                    gui_free(fs_index_table);
                }
                continue;
            }

            uint32_t ttfoffset = getGlyphOffset(unicode_buf[uni_i], ttfbin,
                                                index_table_ptr,
                                                text->font_mode == FONT_SRC_FILESYS ? FONT_SRC_MEMADDR : text->font_mode,
                                                preloaded_index_table);

            if (fs_index_table != NULL)
            {
                gui_free(fs_index_table);
            }

            if (ttfoffset == 0)
            {
                /* Fallback: try other TTF fonts */
                if (gui_font_ttf_fallback_search(unicode_buf[uni_i], text->font_height,
                                                 text->bold_weight, (uint8_t *)text->path,
                                                 &chr[chr_i]) == 0)
                {
                    /* Found in TTF fallback */
                }
                else
                {
                    // gui_log("[TTF fallback] U+%04X: not found in primary=%p or any fallback\n",
                    //         unicode_buf[uni_i], text->path);
                    continue;
                }
            }
            else
            {

                uint8_t *font_ptr = (uint8_t *)text->path + ttfoffset;

                if (text->font_mode == FONT_SRC_MEMADDR)
                {
                    FontGlyphData *glyphData = (FontGlyphData *)font_ptr;

#if ENABLE_FONT_V3_TYPO
                    ttf_populate_glyph_metrics(&chr[chr_i], glyphData, unicode_buf[uni_i],
                                               text->font_height, scale, text->bold_weight,
                                               &typo_ctx, font_ptr);
#else
                    chr[chr_i].unicode = unicode_buf[uni_i];
                    chr[chr_i].h = text->font_height;
                    chr[chr_i].char_h = text->font_height;
                    chr[chr_i].dot_addr = font_ptr;
                    chr[chr_i].char_w = glyphData->advance * scale + text->bold_weight * 2;
                    if (chr[chr_i].char_w == 0 && glyphData->x1 > glyphData->x0)
                    {
                        int _vis = (int)roundf((glyphData->x1 - glyphData->x0) * scale)
                                   + text->bold_weight * 2;
                        chr[chr_i].char_w = (uint16_t)(_vis > 0 ? _vis : 1);
                    }
#endif
                }
                else if (text->font_mode == FONT_SRC_FTL)
                {
                    FontGlyphData *glyphData = gui_malloc(sizeof(FontGlyphData));
                    GUI_ASSERT(glyphData != NULL);
                    gui_ftl_read((uintptr_t)font_ptr, (uint8_t *)glyphData, sizeof(FontGlyphData));

                    int line_count = 0;
                    uint8_t winding_length = glyphData->winding_count;
                    uint8_t *winding_lengths = gui_malloc(winding_length);
                    GUI_ASSERT(winding_lengths != NULL);
                    gui_ftl_read((uintptr_t)(uint8_t *)(font_ptr + offsetof(FontGlyphData, winding_lengths)),
                                 winding_lengths, winding_length);
                    for (int i = 0; i < glyphData->winding_count; i++)
                    {
                        line_count += winding_lengths[i];
                    }

                    uint8_t *dot_addr = gui_malloc(sizeof(FontGlyphData) + winding_length + line_count * sizeof(
                                                       FontWindings));
                    GUI_ASSERT(dot_addr != NULL);
                    memcpy(dot_addr, glyphData, sizeof(FontGlyphData));
                    memcpy(dot_addr + offsetof(FontGlyphData, winding_lengths), winding_lengths, winding_length);
                    gui_ftl_read((uintptr_t)font_ptr + offsetof(FontGlyphData, winding_lengths) + winding_length,
                                 dot_addr + offsetof(FontGlyphData, winding_lengths) + winding_length,
                                 line_count * sizeof(FontWindings));


#if ENABLE_FONT_V3_TYPO
                    ttf_populate_glyph_metrics(&chr[chr_i], glyphData, unicode_buf[uni_i],
                                               text->font_height, scale, text->bold_weight,
                                               &typo_ctx, dot_addr);
#else
                    chr[chr_i].unicode = unicode_buf[uni_i];
                    chr[chr_i].h = text->font_height;
                    chr[chr_i].char_h = text->font_height;
                    chr[chr_i].dot_addr = dot_addr;
                    chr[chr_i].char_w = glyphData->advance * scale + text->bold_weight * 2;
                    if (chr[chr_i].char_w == 0 && glyphData->x1 > glyphData->x0)
                    {
                        int _vis = (int)roundf((glyphData->x1 - glyphData->x0) * scale)
                                   + text->bold_weight * 2;
                        chr[chr_i].char_w = (uint16_t)(_vis > 0 ? _vis : 1);
                    }
#endif

                    gui_free(glyphData);
                    gui_free(winding_lengths);
                }
                else if (text->font_mode == FONT_SRC_FILESYS)
                {
                    gui_vfs_file_t *file = gui_vfs_open((const char *)text->path, GUI_VFS_READ);
                    if (file == NULL)
                    {
                        continue;
                    }
                    FontGlyphData *glyphData = gui_malloc(sizeof(FontGlyphData));
                    GUI_ASSERT(glyphData != NULL);
                    gui_vfs_seek(file, ttfoffset, GUI_VFS_SEEK_SET);
                    gui_vfs_read(file, glyphData, sizeof(FontGlyphData));

                    int line_count = 0;
                    uint8_t winding_length = glyphData->winding_count;
                    uint8_t *winding_lengths = gui_malloc(winding_length);
                    GUI_ASSERT(winding_lengths != NULL);
                    gui_vfs_seek(file, ttfoffset + offsetof(FontGlyphData, winding_lengths), GUI_VFS_SEEK_SET);
                    gui_vfs_read(file, winding_lengths, winding_length);
                    for (int i = 0; i < glyphData->winding_count; i++)
                    {
                        line_count += winding_lengths[i];
                    }

                    uint8_t *dot_addr = gui_malloc(sizeof(FontGlyphData) + winding_length + line_count * sizeof(
                                                       FontWindings));
                    GUI_ASSERT(dot_addr != NULL);
                    memcpy(dot_addr, glyphData, sizeof(FontGlyphData));
                    memcpy(dot_addr + offsetof(FontGlyphData, winding_lengths), winding_lengths, winding_length);
                    gui_vfs_seek(file, ttfoffset + offsetof(FontGlyphData, winding_lengths) + winding_length,
                                 GUI_VFS_SEEK_SET);
                    gui_vfs_read(file, dot_addr + offsetof(FontGlyphData, winding_lengths) + winding_length,
                                 line_count * sizeof(FontWindings));
                    gui_vfs_close(file);

#if ENABLE_FONT_V3_TYPO
                    ttf_populate_glyph_metrics(&chr[chr_i], glyphData, unicode_buf[uni_i],
                                               text->font_height, scale, text->bold_weight,
                                               &typo_ctx, dot_addr);
#else
                    chr[chr_i].unicode = unicode_buf[uni_i];
                    chr[chr_i].h = text->font_height;
                    chr[chr_i].char_h = text->font_height;
                    chr[chr_i].dot_addr = dot_addr;
                    chr[chr_i].char_w = glyphData->advance * scale + text->bold_weight * 2;
                    if (chr[chr_i].char_w == 0 && glyphData->x1 > glyphData->x0)
                    {
                        int _vis = (int)roundf((glyphData->x1 - glyphData->x0) * scale)
                                   + text->bold_weight * 2;
                        chr[chr_i].char_w = (uint16_t)(_vis > 0 ? _vis : 1);
                    }
#endif

                    gui_free(glyphData);
                    gui_free(winding_lengths);
                }
            } /* end else (ttfoffset != 0) */
        }

        all_char_w += chr[chr_i].char_w;
        chr_i ++;
    }
    text->char_width_sum = all_char_w;
    text->char_line_sum = line_flag;
    text->font_len = unicode_len;
    text->active_font_len = chr_i;
    gui_free(unicode_buf);

    if (need_free_index_table && preloaded_index_table != NULL)
    {
        gui_free(preloaded_index_table);
    }

    if (need_free_ttfbin)
    {
        gui_free(ttfbin);
    }
}

void gui_font_get_ttf_info(gui_text_t *text)
{

    font_ttf_get_info(text);

}

void gui_font_ttf_adapt_rect(gui_text_t *text, gui_text_rect_t *rect)
{
    gui_matrix_t *tm = text->base.matrix;

    int mx1, my1, mx2, my2;
    computeBoundingBoxIntV2(0, 0, text->base.w - 1, text->base.h - 1,
                            tm->m, &mx1, &mx2, &my1, &my2);
    rect->x1 = mx1;
    rect->y1 = my1;
    rect->x2 = mx2;
    rect->y2 = my2;

    rect->xboundleft = mx1;
    rect->xboundright = mx2;
    rect->yboundtop = my1;
    rect->yboundbottom = my2;
}

void gui_font_ttf_load(gui_text_t *text, gui_text_rect_t *rect)
{
    if (text == NULL)
    {
        return;
    }
    if (text->data == NULL)
    {
        gui_font_get_ttf_info(text);
    }
    else
    {
        if (text->content_refresh)
        {
            gui_font_ttf_destroy(text);
            gui_font_get_ttf_info(text);
        }
    }
    // if (text->layout_refresh)
    {
        if (text != NULL)
        {
            gui_font_mem_layout(text, rect);
        }
    }
}

void gui_font_ttf_destroy(gui_text_t *text)
{
    if (text->data)
    {
        mem_char_t *chr = text->data;
        if (text->font_mode == FONT_SRC_FTL || text->font_mode == FONT_SRC_FILESYS)
        {
            for (int i = 0; i < text->font_len; i++)
            {
                if (!chr[i].is_emoji)
                {
                    gui_free(chr[i].dot_addr);
                }
            }
        }
        for (int i = 0; i < text->font_len; i++)
        {
            if (chr[i].buf != NULL)
            {
                gui_free(chr[i].buf);
            }
        }
        gui_free(text->data);
        text->data = NULL;
    }
}

void gui_font_ttf_unload(gui_text_t *text)
{
    /* Cross-frame static cache: keep alive until content change or destroy. */
    if (text->font_cache_static)
    {
        return;
    }
    /* Non-RAMLESS: object-level chr[] cache persists across frames. */
    gui_dispdev_t *dc = gui_get_dc();
    if (dc == NULL || dc->type != DC_RAMLESS)
    {
        return;
    }
    gui_font_ttf_destroy(text);
}

/**
 * @brief Rasterize and draw all glyphs in a text object.
 * @param text Text object containing glyphs and rendering settings.
 * @param rect Text bounds and clipping rectangle.
 */
static void font_ttf_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    GUI_ASSERT(text->path != NULL);
    GUI_FONT_HEAD_TTF *ttfbin;
    bool need_free_ttfbin = false;

    /* Try to get cached header first */
    ttfbin = get_cached_ttf_header(text->path);

    if (ttfbin == NULL)
    {
        /* Not cached, load from source */
        if (text->font_mode == FONT_SRC_MEMADDR)
        {
            ttfbin = (GUI_FONT_HEAD_TTF *)text->path;
        }
        else if (text->font_mode == FONT_SRC_FTL)
        {
            ttfbin = gui_malloc(sizeof(GUI_FONT_HEAD_TTF));
            GUI_ASSERT(ttfbin != NULL);
            gui_ftl_read((uintptr_t)text->path, (uint8_t *)ttfbin, sizeof(GUI_FONT_HEAD_TTF));
            need_free_ttfbin = true;
        }
        else if (text->font_mode == FONT_SRC_FILESYS)
        {
            gui_vfs_file_t *file = gui_vfs_open((const char *)text->path, GUI_VFS_READ);
            if (file == NULL)
            {
                return;
            }
            ttfbin = gui_malloc(sizeof(GUI_FONT_HEAD_TTF));
            GUI_ASSERT(ttfbin != NULL);
            gui_vfs_read(file, ttfbin, sizeof(GUI_FONT_HEAD_TTF));
            gui_vfs_close(file);
            need_free_ttfbin = true;
        }
        else
        {
            GUI_ASSERT(NULL != NULL);
            return;
        }
    }

    if (ttfbin->file_type != FONT_FILE_TTF_FLAG)
    {
        gui_log("this ttf-bin font file is not valid \n");
        if (need_free_ttfbin)
        {
            gui_free(ttfbin);
        }
        return;
    }
    mem_char_t *chr = text->data;

    /* Build typography context to detect V3 vs legacy */
#if ENABLE_FONT_V3_TYPO
    gui_font_typo_context_t typo_ctx = gui_font_ttf_get_typo_context(ttfbin, text->font_height);

    short ascent = 0;
    float scale = 0;

    if (typo_ctx.is_v3)
    {
        /* V3 standard: scale = font_height / units_per_em */
        /* ascent = 0 because bearing-based layout already handles baseline positioning */
        ascent = 0;
        scale = (float)text->font_height / typo_ctx.metrics.units_per_em;
    }
    else
    {
        /* Deprecated scale and ascent positioning for pre-v3 fonts. */
        ascent = ttfbin->ascent;
        scale = (float)text->font_height / (ttfbin->ascent - ttfbin->descent);
    }
#else
    short ascent = ttfbin->ascent;
    float scale = (float)text->font_height / (ttfbin->ascent - ttfbin->descent);
#endif /* ENABLE_FONT_V3_TYPO */

    if (scale <= 0)
    {
        if (need_free_ttfbin)
        {
            gui_free(ttfbin);
        }
        return;
    }

    uint8_t raster_prec = 1 << text->rendermode;

#if ENABLE_FONT_V3_TYPO
    const bool is_v3 = typo_ctx.is_v3;
#else
    const bool is_v3 = false;
#endif

    gui_dispdev_t *dc = gui_get_dc();

    gui_matrix_t *tm = text->base.matrix;
    TransformCase caseX = determineTransformCase(tm->m[0][0], tm->m[0][1]);
    TransformCase caseY = determineTransformCase(tm->m[1][0], tm->m[1][1]);

    FONT_MATRIX_TYPE tm_type;
    if (tm->m[2][0] != 0 || tm->m[2][1] != 0 || tm->m[2][2] != 1)
    {
        tm_type = FONT_HOMOGENEOUS;
    }
    else if (tm->m[0][0] != 1 || tm->m[0][1] != 0 || tm->m[1][0] != 0 || tm->m[1][1] != 1)
    {
        tm_type = FONT_SCALE;
    }
    else if (tm->m[0][2] != 0 || tm->m[1][2] != 0)
    {
        tm_type = FONT_TRANSFORM;
    }
    else
    {
        tm_type = FONT_IDENTITY;
    }

    /* Keep cache mode stable for this pass; glyph updates must not affect the
     * dispatch of later glyphs, especially across RAMLESS sections. */
    const bool prev_static = text->font_cache_static;

    for (uint16_t index = 0; index < text->active_font_len; index++)
    {
        if (chr[index].is_emoji)
        {
            gui_font_draw_emoji(text, chr + index, chr[index].dot_addr);
            continue;
        }
        if (chr[index].buf)
        {
            bool now_static = (tm_type == FONT_IDENTITY || tm_type == FONT_TRANSFORM);
            if (now_static && prev_static)
            {
                /* Recompute placement because chr[].x/y may change each frame;
                 * the cached bitmap itself is position-independent. */
                FontGlyphData *gd = (FontGlyphData *)chr[index].dot_addr;
                float origin_x, origin_y;
                ttf_raster_geo_t geo;
                ttf_calc_glyph_origin(gd, &chr[index], scale, ascent, is_v3, &origin_x, &origin_y);
                ttf_calc_raster_geometry(gd, scale, raster_prec, text->bold_weight,
                                         origin_x, origin_y, &geo);
                font_ttf_draw_bitmap_classic(text, chr[index].buf, rect,
                                             chr[index].x + geo.out_x0,
                                             chr[index].y + geo.out_y0,
                                             geo.out_w, geo.out_h);
                continue;
            }
            if (dc->type == DC_RAMLESS && !prev_static)
            {
                /* Reuse the bitmap across RAMLESS sections, then free it after
                 * the scan band passes the glyph. */
                font_ttf_draw_bitmap_classic(text, chr[index].buf, rect, chr[index].x, chr[index].y,
                                             chr[index].w, chr[index].h);
                if (dc->pfb_type == PFB_Y_DIRECTION)
                {
                    if (dc->section_count * dc->fb_height >= (unsigned long)(chr[index].y + chr[index].h))
                    {
                        gui_free(chr[index].buf);
                        chr[index].buf = NULL;
                        chr[index].char_w = 0;
                    }
                }
                else
                {
                    if (dc->section_count * dc->fb_width >= (unsigned long)(chr[index].x + chr[index].w))
                    {
                        gui_free(chr[index].buf);
                        chr[index].buf = NULL;
                        chr[index].char_w = 0;
                    }
                }
                continue;
            }
            /* Bitmap no longer valid for the current transform: an
             * identity/translate static cache while tm_type is now scale/rotate,
             * or a non-RAMLESS scale leftover. Free and re-rasterize. */
            gui_free(chr[index].buf);
            chr[index].buf = NULL;
            goto ttf_rasterize;
        }
ttf_rasterize:
        if (chr[index].dot_addr == 0 || chr[index].char_w == 0)
        {
            continue;
        }
        FontGlyphData *glyphData = (FontGlyphData *)chr[index].dot_addr;

        float render_scale = scale * raster_prec;

        float glyph_x0 = 0;
        float glyph_y0 = 0;
        float glyph_x1 = 0;
        float glyph_y1 = 0;
        float glyph_x0m = 0;
        float glyph_y0m = 0;
        float glyph_x1m = 0;
        float glyph_y1m = 0;

        int render_w = 0;
        int render_h = 0;

        int out_x0 = 0;
        int out_y0 = 0;
        int out_x1 = 0;
        int out_y1 = 0;
        int out_w = 0;
        int out_h = 0;

        uint32_t block_bit = FONT_TTF_BLOCK_BIT;
        uint32_t line_word = 0;

        int mx0 = 0;
        int my0 = 0;
        int mx1 = 0;
        int my1 = 0;

        int line_count = 0;
        ttf_point *windingsfm;
        uint8_t *winding_lengths = chr[index].dot_addr + offsetof(FontGlyphData, winding_lengths);
        FontWindings *windings = (FontWindings *)(winding_lengths + glyphData->winding_count);
        for (int i = 0; i < glyphData->winding_count; i++)
        {
            line_count += winding_lengths[i];
        }
        if (line_count == 0)
        {
            continue;  /* glyph has advance but no outline (e.g. non-breaking space) */
        }
        uint32_t windingsf_size = line_count * sizeof(ttf_point);
        ttf_point *windingsf = gui_malloc(windingsf_size);
        GUI_ASSERT(windingsf != NULL);

        /* Calculate extra space needed for bold effect */
        uint8_t bold_weight = text->bold_weight;
        int bold_extra = bold_weight * raster_prec;

        switch (tm_type)
        {
        /* Affine and perspective share placement; only bbox and point transforms differ.
         * Prior MVE variants were not faster than the scalar loops. */
        case FONT_HOMOGENEOUS:
        case FONT_SCALE:
            {
                const bool perspective = (tm_type == FONT_HOMOGENEOUS);

                /* Pen origin (pre-matrix, pixels); the outline below re-applies the
                 * glyph bearing through glyphData->x0/y0. */
                float back_x, back_y;
                ttf_calc_bearing_backout(&chr[index], is_v3, &back_x, &back_y);
                float pen_x = chr[index].x - text->offset_x + back_x;
                float pen_y = chr[index].y - text->offset_y + back_y;

                glyph_x0 = scale * glyphData->x0;
                glyph_y0 = scale * (ascent + glyphData->y0);
                glyph_x1 = scale * glyphData->x1;
                glyph_y1 = scale * (ascent + glyphData->y1);

                /* Derive placement from the exact device-space bbox to avoid
                 * amplifying pre-transform rounding errors. */
                if (perspective)
                {
                    computeBoundingBoxFloatV2(pen_x + glyph_x0, pen_y + glyph_y0,
                                              pen_x + glyph_x1, pen_y + glyph_y1, tm->m,
                                              &glyph_x0m, &glyph_x1m, &glyph_y0m, &glyph_y1m);
                }
                else
                {
                    computeBoundingBoxFloat(pen_x + glyph_x0, pen_y + glyph_y0,
                                            pen_x + glyph_x1, pen_y + glyph_y1, tm->m,
                                            caseX, caseY,
                                            &glyph_x0m, &glyph_x1m, &glyph_y0m, &glyph_y1m);
                }

                ttf_raster_geo_t geo;
                ttf_split_placement(glyph_x0m - bold_weight, glyph_y0m - bold_weight,
                                    glyph_x1m - glyph_x0m + bold_weight * 2,
                                    glyph_y1m - glyph_y0m + bold_weight * 2,
                                    raster_prec, &geo);

                render_w = geo.render_w;
                render_h = geo.render_h;
                line_word = render_w / block_bit;
                out_w = geo.out_w;
                out_h = geo.out_h;

                mx0 = geo.out_x0;
                my0 = geo.out_y0;
                mx1 = mx0 + out_w - 1;
                my1 = my0 + out_h - 1;

                uint32_t windingsfm_size = line_count * sizeof(ttf_point);
                windingsfm = gui_malloc(windingsfm_size);
                GUI_ASSERT(windingsfm != NULL);
#if FIX_AUTO_VECTORIZE
                /* Copy out of the packed/unaligned font data so the loops below can
                 * be auto-vectorized. */
                uint32_t windingsd_size = line_count * sizeof(FontWindings);
                FontWindings *windingsd = gui_malloc(windingsd_size);
                GUI_ASSERT(windingsd != NULL);
                memcpy(windingsd, windings, windingsd_size);
                const FontWindings *wsrc = windingsd;
#else
                const FontWindings *wsrc = windings;
#endif
                /* font units, pen-local -> pixels, widget space */
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = wsrc[i].x;
                    windingsf[i].x = windingsf[i].x * scale;
                    windingsf[i].x += pen_x;

                    windingsf[i].y = ascent - wsrc[i].y;
                    windingsf[i].y = windingsf[i].y * scale;
                    windingsf[i].y += pen_y;
                }
                if (perspective)
                {
                    for (int i = 0; i < line_count; i++)
                    {
                        transformPointV2(windingsf[i].x, windingsf[i].y, tm->m,
                                         &windingsfm[i].x, &windingsfm[i].y);
                    }
                }
                else
                {
                    for (int i = 0; i < line_count; i++)
                    {
                        transformPoint(windingsf[i].x, windingsf[i].y, tm->m,
                                       &windingsfm[i].x, &windingsfm[i].y);
                    }
                }
                /* pixels -> bitmap sub-samples. mx0/my0 is the floored exact bbox, so
                 * this carries the sub-pixel remainder and the bold margin for free. */
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = (windingsfm[i].x - mx0) * raster_prec;
                    windingsf[i].y = (windingsfm[i].y - my0) * raster_prec;
                }
                gui_free(windingsfm);
#if FIX_AUTO_VECTORIZE
                gui_free(windingsd);
#endif
            }
            break;
        case FONT_TRANSFORM:
        case FONT_IDENTITY:
            {
                /* Exact glyph origin, split into a whole-pixel bitmap position and
                 * a sub-pixel remainder that shifts the outline below, so every
                 * glyph is sampled on the same target pixel grid. */
                float origin_x, origin_y;
                ttf_raster_geo_t geo;
                ttf_calc_glyph_origin(glyphData, &chr[index], scale, ascent, is_v3,
                                      &origin_x, &origin_y);
                ttf_calc_raster_geometry(glyphData, scale, raster_prec, bold_weight,
                                         origin_x, origin_y, &geo);

                render_w = geo.render_w;
                render_h = geo.render_h;
                line_word = render_w / block_bit;
                out_w = geo.out_w;
                out_h = geo.out_h;

                out_x0 = geo.out_x0;
                out_y0 = geo.out_y0;
                out_x1 = out_x0 + out_w - 1;
                out_y1 = out_y0 + out_h - 1;

                mx0 = out_x0 + chr[index].x;
                mx1 = out_x1 + chr[index].x;
                my0 = out_y0 + chr[index].y;
                my1 = out_y1 + chr[index].y;

#if defined(FONT_TTF_USE_MVE_FP) && FONT_TTF_MVE_WINDING_TRANSFORM
                /* Stage packed flash data in RAM before explicit MVE loads. With
                 * FIX_AUTO_VECTORIZE enabled, the scalar path pays the same copy
                 * cost, which is why this implementation now has a net benefit. */
                {
                    uint32_t windingsd_size = line_count * sizeof(FontWindings);
                    FontWindings *windingsd = gui_malloc(windingsd_size);
                    GUI_ASSERT(windingsd != NULL);
                    memcpy(windingsd, windings, windingsd_size);

                    const float offset_x = -glyphData->x0 * render_scale + bold_extra + geo.sub_x;
                    const float offset_y = -glyphData->y0 * render_scale + bold_extra + geo.sub_y;
                    const float32x4_t v_mul = {render_scale, -render_scale, render_scale, -render_scale};
                    const float32x4_t v_add = {offset_x, offset_y, offset_x, offset_y};

                    int i = 0;
                    for (; i + 1 < line_count; i += 2)
                    {
                        int32x4_t vi = vldrhq_s32((int16_t *)&windingsd[i].x);
                        float32x4_t vf = vcvtq_f32_s32(vi);
                        vf = vfmaq_f32(v_add, vf, v_mul);
                        vstrwq_f32(&windingsf[i].x, vf);
                    }
                    for (; i < line_count; i++)
                    {
                        windingsf[i].x = (windingsd[i].x - glyphData->x0) * render_scale + bold_extra + geo.sub_x;
                        windingsf[i].y = (-glyphData->y0 - windingsd[i].y) * render_scale + bold_extra + geo.sub_y;
                    }
                    gui_free(windingsd);
                }
#else
                /* Hoist glyph bounds because -fno-strict-aliasing prevents the
                 * compiler from doing so. Keep expressions bit-identical. */
                const int glyph_x0 = glyphData->x0;
                const int glyph_y0 = glyphData->y0;
#if FIX_AUTO_VECTORIZE
                uint32_t windingsd_size = line_count * sizeof(FontWindings);
                FontWindings *windingsd = gui_malloc(windingsd_size);
                GUI_ASSERT(windingsd != NULL);
                memcpy(windingsd, windings, windingsd_size);
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = (windingsd[i].x - glyph_x0) * render_scale + bold_extra + geo.sub_x;
                    windingsf[i].y = (- glyph_y0 - windingsd[i].y) * render_scale + bold_extra + geo.sub_y;
                }
                gui_free(windingsd);
#else
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = (windings[i].x - glyph_x0) * render_scale + bold_extra + geo.sub_x;
                    windingsf[i].y = (- glyph_y0 - windings[i].y) * render_scale + bold_extra + geo.sub_y;
                }
#endif
#endif
            }
            break;
        default:
            break;
        }


        uint32_t line_list_size = line_count * sizeof(LINE_T);
        LINE_T *line_list = gui_malloc(line_list_size);
        GUI_ASSERT(line_list != NULL);

        int lint_count_actual = 0;
        int winding_offset = 0;
        for (int i = 0; i < glyphData->winding_count; i++)
        {
            ttf_point *winding = windingsf + winding_offset;
            /* Hoisted for the same aliasing reason as glyph_x0 above: the
             * line_list store may alias winding_lengths, so the length was
             * reloaded once per edge just to re-evaluate the loop bound. */
            const int winding_len = winding_lengths[i];
            winding_offset += winding_len;
            for (int j = 0; j < winding_len - 1; j++)
            {
                if (winding[j].y == winding[j + 1].y)
                {
                    continue;
                }
                add_point_to_line(&line_list[lint_count_actual++], winding[j], winding[j + 1]);
            }
        }

        // for (int i = 0; i < lint_count_actual; i++)
        // {
        //     gui_log("line %d : x0 %f , y0 %d , y1 %d , dxy %f\n",
        //             i,line_list[i].x0,line_list[i].y0,line_list[i].y1,line_list[i].dxy);
        // }


        uint32_t render_size = render_w * render_h / 8;
        uint32_t *img = gui_malloc(render_size);
        GUI_ASSERT(img != NULL);
        memset(img, 0, render_size);

        uint32_t out_size = out_w * out_h;
        uint8_t *img_out = gui_malloc(out_size);
        GUI_ASSERT(img_out != NULL);

        /* Even-odd fill: mark crossings, then propagate parity across each row.
         * Hoist edge values because -fno-strict-aliasing blocks compiler hoisting. */
#if FONT_TTF_SCANLINE_PREFIX_XOR
        for (int i = 0; i < lint_count_actual; i++)
        {
            const float x0 = line_list[i].x0;
            const float dxy = line_list[i].dxy;
            const int y_end = line_list[i].y1;
            uint32_t *row = img + (uint32_t)line_list[i].y0 * line_word;

            for (int y = line_list[i].y0; y < y_end; y++, row += line_word)
            {
                uint32_t xint = (uint32_t)(x0 + dxy * y);

                row[xint / FONT_TTF_BLOCK_BIT] ^=
                    0x80000000u >> (xint % FONT_TTF_BLOCK_BIT);
            }
        }

        uint32_t *prefix_row = img;

        for (int y = 0; y < render_h; y++, prefix_row += line_word)
        {
            uint32_t carry = 0;

            for (uint32_t li = 0; li < line_word; li++)
            {
                uint32_t w = prefix_row[li];

                w ^= w >> 1;
                w ^= w >> 2;
                w ^= w >> 4;
                w ^= w >> 8;
                w ^= w >> 16;
                w ^= carry;
                prefix_row[li] = w;
                carry = 0u - (w & 1u);
            }
        }
#else
        /* XOR each crossing to the row end. Choose this branch when sparse,
         * narrow outlines outperform the fixed per-row prefix pass. */
        for (int i = 0; i < lint_count_actual; i++)
        {
            const float x0 = line_list[i].x0;
            const float dxy = line_list[i].dxy;
            const int y_end = line_list[i].y1;
            uint32_t *row = img + (uint32_t)line_list[i].y0 * line_word;

            for (int y = line_list[i].y0; y < y_end; y++, row += line_word)
            {
                uint32_t xint = (uint32_t)(x0 + dxy * y);
                uint32_t word = xint / FONT_TTF_BLOCK_BIT;

                row[word] ^= 0xFFFFFFFFu >> (xint % FONT_TTF_BLOCK_BIT);
                for (uint32_t li = word + 1; li < line_word; li++)
                {
                    row[li] = ~row[li];
                }
            }
        }
#endif
        makeImageBuffer(img_out, img, raster_prec, out_w, out_h, render_w, render_h, line_word, block_bit);

        /* Apply bold effect before anti-aliasing adjustment. This is included in
         * downsample because the current benchmark uses bold_weight == 0. */
        if (bold_weight > 0)
        {
            font_ttf_bitmap_embolden(img_out, out_w, out_h, bold_weight, text->bold_mode);
        }

        adjustImageBufferPrecision(img_out, out_size, raster_prec);


        font_ttf_draw_bitmap_classic(text, img_out, rect, mx0, my0, out_w, out_h);

        gui_free(windingsf);
        gui_free(line_list);
        gui_free(img);

        if ((tm_type == FONT_IDENTITY || tm_type == FONT_TRANSFORM) && text->font_cache_enable)
        {
            /* Cross-frame cache: keep bitmap until content change / disable / destroy.
             * Do NOT overwrite chr[].x/y -- hit path needs the layout position. */
            if (chr[index].buf)
            {
                gui_free(chr[index].buf);
            }
            chr[index].buf = img_out;
            text->font_cache_static = 1;
        }
        else if (dc->type == DC_RAMLESS)
        {
            /* RAMLESS section-banding (scale/rotate, or cache disabled): store the
             * absolute bitmap position in chr[].x/y for the banding hit path;
             * freed per frame as the scan band passes the glyph. */
            chr[index].buf = img_out;
            chr[index].x = mx0;
            chr[index].y = my0;
            chr[index].w = out_w;
            chr[index].h = out_h;
            chr[index].char_w = mx1 - mx0 + 1;
            chr[index].char_h = my1 - my0 + 1;
            text->font_cache_static = 0;
        }
        else
        {
            /* Non-RAMLESS scale/rotate (or cache disabled): rasterize every frame. */
            gui_free(img_out);
            text->font_cache_static = 0;
        }
    }
    if (need_free_ttfbin)
    {
        gui_free(ttfbin);
    }
}

void gui_font_ttf_draw(gui_text_t *text, gui_text_rect_t *rect)
{

    font_ttf_draw(text, rect);

}

