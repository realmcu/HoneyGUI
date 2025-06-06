/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file font_ttf.c
  * @brief realui ttf engine
  * @details srealui ttf engine
  * @author luke_sun@realsil.com.cn
  * @date 2024/12/09
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "draw_font.h"
#include "font_ttf.h"
#include "font_mem.h"
#include <stddef.h>
#include <math.h>
#include "font_rendering_utils.h"


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


/*============================================================================*
 *                           Constants
 *============================================================================*/
static const uint32_t masks[32] =
{
    0xFFFFFFFF, 0x7FFFFFFF, 0x3FFFFFFF, 0x1FFFFFFF,
    0x0FFFFFFF, 0x07FFFFFF, 0x03FFFFFF, 0x01FFFFFF,
    0x00FFFFFF, 0x007FFFFF, 0x003FFFFF, 0x001FFFFF,
    0x000FFFFF, 0x0007FFFF, 0x0003FFFF, 0x0001FFFF,
    0x0000FFFF, 0x00007FFF, 0x00003FFF, 0x00001FFF,
    0x00000FFF, 0x000007FF, 0x000003FF, 0x000001FF,
    0x000000FF, 0x0000007F, 0x0000003F, 0x0000001F,
    0x0000000F, 0x00000007, 0x00000003, 0x00000001
};

static const uint8_t lookup_table_8b[256] =
{
#   define B2(n) n,     n+1,     n+1,     n+2
#   define B4(n) B2(n), B2(n+1), B2(n+1), B2(n+2)
#   define B6(n) B4(n), B4(n+1), B4(n+1), B4(n+2)
    B6(0), B6(1), B6(1), B6(2)
};

static const uint8_t lookup_table_2b[4] =
{
    0, 1, 1, 2
};
static const uint8_t lookup_table_4b[16] =
{
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
};


/*============================================================================*
 *                            Macros
 *============================================================================*/

/*MVE*/
#define GUI_ENABLE_MVE      1
#if  __ARM_FEATURE_MVE & GUI_ENABLE_MVE
#define FONT_TTF_USE_MVE
#include "arm_mve.h"
#endif

/*
    FIX_AUTO_VECTORIZE is used to adapt to auto-vectorization,
    and it only needs to be enabled when the compiler's auto-vectorization feature is turned on.
    It will reduce rendering speed and increase memory overhead.
*/
#define FIX_AUTO_VECTORIZE   1

#define ALIGN_TO(x, y) (((x) + ((y) - 1)) & ~((y) - 1))
#define ROUNDING_OFFSET 0.5f

/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

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

static void transformPointV2(float x, float y, float m[3][3], float *x_out, float *y_out)
{
    float w = m[2][0] * x + m[2][1] * y + m[2][2];
    *x_out = (m[0][0] * x + m[0][1] * y + m[0][2]) / w;
    *y_out = (m[1][0] * x + m[1][1] * y + m[1][2]) / w;
}
static void transformPoint(float x, float y, float m[3][3], float *x_out, float *y_out)
{
    *x_out = m[0][0] * x + m[0][1] * y + m[0][2];
    *y_out = m[1][0] * x + m[1][1] * y + m[1][2];
}

static void computeBoundingBoxInt(int x0, int y0, int x1, int y1, float m[3][3],
                                  TransformCase caseX, TransformCase caseY, int *xmin, int *xmax, int *ymin, int *ymax)
{
    float x_min, x_max, y_min, y_max;
    float temp_x, temp_y;

    if (caseX == POS_POS || caseX == POS_NEG)
    {
        transformPoint(x0, (caseX == POS_POS ? y0 : y1), m, &x_min, &temp_y);
        transformPoint(x1, (caseX == POS_POS ? y1 : y0), m, &x_max, &temp_y);
    }
    else
    {
        transformPoint(x1, (caseX == NEG_POS ? y0 : y1), m, &x_min, &temp_y);
        transformPoint(x0, (caseX == NEG_POS ? y1 : y0), m, &x_max, &temp_y);
    }

    if (caseY == POS_POS || caseY == POS_NEG)
    {
        transformPoint((caseY == POS_POS ? x0 : x1), y0, m, &temp_x, &y_min);
        transformPoint((caseY == POS_POS ? x1 : x0), y1, m, &temp_x, &y_max);
    }
    else
    {
        transformPoint((caseY == NEG_POS ? x1 : x0), y0, m, &temp_x, &y_min);
        transformPoint((caseY == NEG_POS ? x0 : x1), y1, m, &temp_x, &y_max);
    }

    *xmin = (int)x_min;
    *xmax = (int)x_max;
    *ymin = (int)y_min;
    *ymax = (int)y_max;
}

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

    if (caseY == POS_POS || caseY == POS_NEG)
    {
        transformPoint((caseY == POS_POS ? x0 : x1), y0, m, &temp_x, ymin);
        transformPoint((caseY == POS_POS ? x1 : x0), y1, m, &temp_x, ymax);
    }
    else
    {
        transformPoint((caseY == NEG_POS ? x1 : x0), y0, m, &temp_x, ymin);
        transformPoint((caseY == NEG_POS ? x0 : x1), y1, m, &temp_x, ymax);
    }
}

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


void font_ttf_draw_bitmap_classic(gui_text_t *text, uint8_t *buf,
                                  gui_text_rect_t *rect,
                                  int x, int y,
                                  int w, int h)
{
    gui_dispdev_t *dc = gui_get_dc();

    int x_start = _UI_MAX3(x, rect->xboundleft, dc->section.x1);
    int x_end = rect->xboundright ? _UI_MIN(x + w - 1, rect->xboundright)
                : _UI_MIN(x + w - 1, dc->section.x2);

    int y_start = _UI_MAX3(dc->section.y1, y, rect->yboundtop);
    int y_end = rect->yboundbottom ? _UI_MIN(dc->section.y2, y + h - 1)
                : _UI_MIN(dc->section.y2, y + h - 1);
    if (x_start >= x_end || y_start >= y_end) { return; }

    draw_font_t render_font =
    {
        .target_buf = dc->frame_buf,
        .target_buf_stride = dc->fb_width * (dc->bit_depth >> 3),
        .color = text->color,
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
        .target_format = (dc->bit_depth == 32) ? ARGB8888 :
        (dc->bit_depth == 24) ? RGB888 :
        (dc->bit_depth == 16) ? RGB565 : RGB565
    };

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

uint32_t getGlyphOffsetFromMemory(uint16_t unicode, GUI_FONT_HEAD_TTF *ttfbin)
{
    if (ttfbin->index_method == 0)
    {
        uint8_t *ptr = (uint8_t *)ttfbin + offsetof(GUI_FONT_HEAD_TTF,
                                                    font_name) + ttfbin->font_name_length;
        uint32_t glyphOffset = *((uint32_t *)ptr + unicode);
        if (glyphOffset != 0xffffffff)
        {
            return glyphOffset;
        }
    }
    else
    {
        uint8_t *ptr = (uint8_t *)ttfbin + offsetof(GUI_FONT_HEAD_TTF,
                                                    font_name) + ttfbin->font_name_length;
        uint32_t indexEntries = ttfbin->index_area_size / (sizeof(uint16_t) + sizeof(uint32_t));
        for (uint32_t i = 0; i < indexEntries; ++i)
        {
            uint16_t currentUnicode = *(uint16_t *)ptr;
            ptr += sizeof(uint16_t);

            uint32_t glyphOffset = *(uint32_t *)ptr;
            ptr += sizeof(uint32_t);

            if (currentUnicode == unicode)
            {
                return glyphOffset;
            }
        }
    }
    return 0;
}

void adjustImageBufferPrecision(uint8_t *img_out, uint32_t out_size, uint8_t raster_prec)
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
#if defined FONT_TTF_USE_MVE && 1
    //MVE CODE, quicker about 2ms
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
    memset(img_out, 0, out_w * out_h);
    if (raster_prec == 4)
    {
        for (uint32_t y = 0; y < render_h; y += raster_prec)
        {
            for (uint32_t ux = 0; ux < line_word; ux++)
            {
                uint32_t pixel1 = img[y * line_word + ux];
                uint32_t pixel2 = img[(y + 1) * line_word + ux];
                uint32_t pixel3 = img[(y + 2) * line_word + ux];
                uint32_t pixel4 = img[(y + 3) * line_word + ux];
                if ((pixel1 + pixel2 + pixel3 + pixel4) == 0)
                {
                    continue;
                }
                uint32_t offset = y / raster_prec * out_w + ux * block_bit / raster_prec;
                img_out[offset + 0] = lookup_table_4b[(pixel1 >> 28) & 0xf] + lookup_table_4b[(pixel2 >> 28) & 0xf]
                                      +
                                      lookup_table_4b[(pixel3 >> 28) & 0xf] + lookup_table_4b[(pixel4 >> 28) & 0xf];
                img_out[offset + 1] = lookup_table_4b[(pixel1 >> 24) & 0xf] + lookup_table_4b[(pixel2 >> 24) & 0xf]
                                      +
                                      lookup_table_4b[(pixel3 >> 24) & 0xf] + lookup_table_4b[(pixel4 >> 24) & 0xf];
                img_out[offset + 2] = lookup_table_4b[(pixel1 >> 20) & 0xf] + lookup_table_4b[(pixel2 >> 20) & 0xf]
                                      +
                                      lookup_table_4b[(pixel3 >> 20) & 0xf] + lookup_table_4b[(pixel4 >> 20) & 0xf];
                img_out[offset + 3] = lookup_table_4b[(pixel1 >> 16) & 0xf] + lookup_table_4b[(pixel2 >> 16) & 0xf]
                                      +
                                      lookup_table_4b[(pixel3 >> 16) & 0xf] + lookup_table_4b[(pixel4 >> 16) & 0xf];
                img_out[offset + 4] = lookup_table_4b[(pixel1 >> 12) & 0xf] + lookup_table_4b[(pixel2 >> 12) & 0xf]
                                      +
                                      lookup_table_4b[(pixel3 >> 12) & 0xf] + lookup_table_4b[(pixel4 >> 12) & 0xf];
                img_out[offset + 5] = lookup_table_4b[(pixel1 >> 8) & 0xf] + lookup_table_4b[(pixel2 >> 8) & 0xf] +
                                      lookup_table_4b[(pixel3 >> 8) & 0xf] + lookup_table_4b[(pixel4 >> 8) & 0xf];
                img_out[offset + 6] = lookup_table_4b[(pixel1 >> 4) & 0xf] + lookup_table_4b[(pixel2 >> 4) & 0xf] +
                                      lookup_table_4b[(pixel3 >> 4) & 0xf] + lookup_table_4b[(pixel4 >> 4) & 0xf];
                img_out[offset + 7] = lookup_table_4b[(pixel1 >> 0) & 0xf] + lookup_table_4b[(pixel2 >> 0) & 0xf] +
                                      lookup_table_4b[(pixel3 >> 0) & 0xf] + lookup_table_4b[(pixel4 >> 0) & 0xf];
            }
        }
    }
    else if (raster_prec == 2)
    {
        for (uint32_t y = 0; y < render_h; y += raster_prec)
        {
            for (uint32_t ux = 0; ux < line_word; ux++)
            {
                uint32_t pixel1 = img[y * line_word + ux];
                uint32_t pixel2 = img[y * line_word + ux + line_word];
                if ((pixel1 + pixel2) == 0)
                {
                    continue;
                }
                uint32_t offset = y / raster_prec * out_w + ux * block_bit / raster_prec;
                img_out[offset + 0] =  lookup_table_2b[(pixel1 >> 30) & 0x3] + lookup_table_2b[(pixel2 >> 30) &
                                                                                               0x3];
                img_out[offset + 1] =  lookup_table_2b[(pixel1 >> 28) & 0x3] + lookup_table_2b[(pixel2 >> 28) &
                                                                                               0x3];
                img_out[offset + 2] =  lookup_table_2b[(pixel1 >> 26) & 0x3] + lookup_table_2b[(pixel2 >> 26) &
                                                                                               0x3];
                img_out[offset + 3] =  lookup_table_2b[(pixel1 >> 24) & 0x3] + lookup_table_2b[(pixel2 >> 24) &
                                                                                               0x3];
                img_out[offset + 4] =  lookup_table_2b[(pixel1 >> 22) & 0x3] + lookup_table_2b[(pixel2 >> 22) &
                                                                                               0x3];
                img_out[offset + 5] =  lookup_table_2b[(pixel1 >> 20) & 0x3] + lookup_table_2b[(pixel2 >> 20) &
                                                                                               0x3];
                img_out[offset + 6] =  lookup_table_2b[(pixel1 >> 18) & 0x3] + lookup_table_2b[(pixel2 >> 18) &
                                                                                               0x3];
                img_out[offset + 7] =  lookup_table_2b[(pixel1 >> 16) & 0x3] + lookup_table_2b[(pixel2 >> 16) &
                                                                                               0x3];
                img_out[offset + 8] =  lookup_table_2b[(pixel1 >> 14) & 0x3] + lookup_table_2b[(pixel2 >> 14) &
                                                                                               0x3];
                img_out[offset + 9] =  lookup_table_2b[(pixel1 >> 12) & 0x3] + lookup_table_2b[(pixel2 >> 12) &
                                                                                               0x3];
                img_out[offset + 10] = lookup_table_2b[(pixel1 >> 10) & 0x3] + lookup_table_2b[(pixel2 >> 10) &
                                                                                               0x3];
                img_out[offset + 11] = lookup_table_2b[(pixel1 >> 8) & 0x3] + lookup_table_2b[(pixel2 >> 8) & 0x3];
                img_out[offset + 12] = lookup_table_2b[(pixel1 >> 6) & 0x3] + lookup_table_2b[(pixel2 >> 6) & 0x3];
                img_out[offset + 13] = lookup_table_2b[(pixel1 >> 4) & 0x3] + lookup_table_2b[(pixel2 >> 4) & 0x3];
                img_out[offset + 14] = lookup_table_2b[(pixel1 >> 2) & 0x3] + lookup_table_2b[(pixel2 >> 2) & 0x3];
                img_out[offset + 15] = lookup_table_2b[(pixel1 >> 0) & 0x3] + lookup_table_2b[(pixel2 >> 0) & 0x3];
            }
        }
    }
    else if (raster_prec == 1)
    {
        for (uint32_t y = 0; y < render_h; y += raster_prec)
        {
            for (uint32_t ux = 0; ux < line_word; ux++)
            {
                uint32_t pixel1 = img[y * line_word + ux];
                if (!pixel1)
                {
                    continue;
                }
                img_out[y * out_w + ux * block_bit + 0] = (pixel1 >> 31) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 1] = (pixel1 >> 30) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 2] = (pixel1 >> 29) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 3] = (pixel1 >> 28) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 4] = (pixel1 >> 27) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 5] = (pixel1 >> 26) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 6] = (pixel1 >> 25) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 7] = (pixel1 >> 24) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 8] = (pixel1 >> 23) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 9] = (pixel1 >> 22) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 10] = (pixel1 >> 21) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 11] = (pixel1 >> 20) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 12] = (pixel1 >> 19) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 13] = (pixel1 >> 18) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 14] = (pixel1 >> 17) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 15] = (pixel1 >> 16) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 16] = (pixel1 >> 15) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 17] = (pixel1 >> 14) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 18] = (pixel1 >> 13) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 19] = (pixel1 >> 12) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 20] = (pixel1 >> 11) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 21] = (pixel1 >> 10) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 22] = (pixel1 >> 9) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 23] = (pixel1 >> 8) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 24] = (pixel1 >> 7) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 25] = (pixel1 >> 6) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 26] = (pixel1 >> 5) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 27] = (pixel1 >> 4) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 28] = (pixel1 >> 3) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 29] = (pixel1 >> 2) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 30] = (pixel1 >> 1) & 1 ? 0xff : 0;
                img_out[y * out_w + ux * block_bit + 31] = (pixel1 >> 0) & 1 ? 0xff : 0;
            }
        }
    }
    else if (raster_prec == 8)
    {
        for (uint32_t y = 0; y < render_h; y += raster_prec)
        {
            for (uint32_t ux = 0; ux < line_word; ux++)
            {
                uint32_t pixel1 = img[y * line_word + ux];
                uint32_t pixel2 = img[(y + 1) * line_word + ux];
                uint32_t pixel3 = img[(y + 2) * line_word + ux];
                uint32_t pixel4 = img[(y + 3) * line_word + ux];
                uint32_t pixel5 = img[(y + 4) * line_word + ux];
                uint32_t pixel6 = img[(y + 5) * line_word + ux];
                uint32_t pixel7 = img[(y + 6) * line_word + ux];
                uint32_t pixel8 = img[(y + 7) * line_word + ux];

                uint32_t offset = y / raster_prec * out_w + ux * block_bit / raster_prec;
                img_out[offset + 0] = lookup_table_8b[(pixel1 >> 24) & 0xff] + lookup_table_8b[(pixel2 >> 24) &
                                                                                               0xff] +
                                      lookup_table_8b[(pixel3 >> 24) & 0xff] + lookup_table_8b[(pixel4 >> 24) & 0xff] +
                                      lookup_table_8b[(pixel5 >> 24) & 0xff] + lookup_table_8b[(pixel6 >> 24) & 0xff] +
                                      lookup_table_8b[(pixel7 >> 24) & 0xff] + lookup_table_8b[(pixel8 >> 24) & 0xff];
                img_out[offset + 1] = lookup_table_8b[(pixel1 >> 16) & 0xff] + lookup_table_8b[(pixel2 >> 16) &
                                                                                               0xff] +
                                      lookup_table_8b[(pixel3 >> 16) & 0xff] + lookup_table_8b[(pixel4 >> 16) & 0xff] +
                                      lookup_table_8b[(pixel5 >> 16) & 0xff] + lookup_table_8b[(pixel6 >> 16) & 0xff] +
                                      lookup_table_8b[(pixel7 >> 16) & 0xff] + lookup_table_8b[(pixel8 >> 16) & 0xff];
                img_out[offset + 2] = lookup_table_8b[(pixel1 >> 8) & 0xff] + lookup_table_8b[(pixel2 >> 8) & 0xff]
                                      +
                                      lookup_table_8b[(pixel3 >> 8) & 0xff] + lookup_table_8b[(pixel4 >> 8) & 0xff] +
                                      lookup_table_8b[(pixel5 >> 8) & 0xff] + lookup_table_8b[(pixel6 >> 8) & 0xff] +
                                      lookup_table_8b[(pixel7 >> 8) & 0xff] + lookup_table_8b[(pixel8 >> 8) & 0xff];
                img_out[offset + 3] = lookup_table_8b[pixel1 & 0xff] + lookup_table_8b[pixel2 & 0xff] +
                                      lookup_table_8b[pixel3 & 0xff] + lookup_table_8b[pixel4 & 0xff] +
                                      lookup_table_8b[pixel5 & 0xff] + lookup_table_8b[pixel6 & 0xff] +
                                      lookup_table_8b[pixel7 & 0xff] + lookup_table_8b[pixel8 & 0xff];

            }
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_font_get_ttf_info(gui_text_t *text)
{
    GUI_FONT_HEAD_TTF *ttfbin = (GUI_FONT_HEAD_TTF *)text->path;
    GUI_ASSERT(ttfbin != NULL);
    if (ttfbin->file_type != FONT_FILE_TTF_FLAG)
    {
        gui_log("this ttf-bin font file is not valid \n");
        return;
    }

    uint32_t *unicode_buf = NULL;
    uint16_t unicode_len = 0;
    unicode_len = process_content_by_charset(text->charset, text->content, text->len, &unicode_buf);
    if (unicode_len == 0)
    {
        gui_log("Warning! After process, unicode len of text: %s is 0!\n", text->base.name);
        text->font_len = 0;
        return;
    }
    mem_char_t *chr = gui_malloc(sizeof(mem_char_t) * unicode_len);
    if (chr == NULL)
    {
        GUI_ASSERT(NULL != NULL);
        return;
    }
    memset(chr, 0, sizeof(mem_char_t) * unicode_len);
    text->data = chr;
    text->content_refresh = false;

    int32_t all_char_w = 0;
    uint32_t line_flag = 0;
    uint32_t uni_i = 0;
    uint32_t chr_i = 0;

    float scale = (float)text->font_height / (ttfbin->ascent - ttfbin->descent);
//    short advance = 0;

    for (uni_i = 0; uni_i < unicode_len; uni_i++)
    {
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
            chr[chr_i].char_w = (text->font_height + 1) / 2;
            chr[chr_i].char_h = (text->font_height + 1) / 2;
            // chr[chr_i].dot_addr = 0;
        }
        else
        {
            uint32_t ttfoffset = getGlyphOffsetFromMemory(unicode_buf[uni_i], ttfbin);
            if (ttfoffset == 0)
            {
                gui_log("Character %x not found in TTF-BIN file \n", unicode_buf[uni_i]);
                continue;
            }
            uint8_t *font_ptr = (uint8_t *)text->path + ttfoffset;
            FontGlyphData *glyphData = (FontGlyphData *)font_ptr;

            chr[chr_i].unicode = unicode_buf[uni_i];
            // chr[chr_i].x = 0;
            // chr[chr_i].y = 0;
            // chr[chr_i].w = 0;
            chr[chr_i].h = text->font_height;
            // chr[chr_i].char_y = 0;
            chr[chr_i].char_w = glyphData->advance * scale;
            chr[chr_i].char_h = text->font_height;
            chr[chr_i].dot_addr = font_ptr;
        }

        all_char_w += chr[chr_i].char_w;
        chr_i ++;
    }
    text->char_width_sum = all_char_w;
    text->char_line_sum = line_flag;
    text->font_len = unicode_len;
    text->active_font_len = chr_i;
    gui_free(unicode_buf);
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
            gui_font_ttf_unload(text);
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

void gui_font_ttf_unload(gui_text_t *text)
{
    if (text->data)
    {
        mem_char_t *chr = text->data;
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
    return;
}

void gui_font_ttf_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    GUI_FONT_HEAD_TTF *ttfbin = (GUI_FONT_HEAD_TTF *)text->path;
    if (ttfbin->file_type != FONT_FILE_TTF_FLAG)
    {
        gui_log("this ttf-bin font file is not valid \n");
        return;
    }
    mem_char_t *chr = text->data;

    short ascent = 0;
    float scale = 0;

    ascent = ttfbin->ascent;
    scale = (float)text->font_height / (ttfbin->ascent - ttfbin->descent);

    if (scale <= 0)
    {
        return;
    }

    uint8_t raster_prec = 1 << text->rendermode;

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

    for (uint16_t index = 0; index < text->active_font_len; index++)
    {
        if (chr[index].buf)
        {
            font_ttf_draw_bitmap_classic(text, chr[index].buf, rect, chr[index].x, chr[index].y, chr[index].w,
                                         chr[index].h);
            if (dc->section_count * dc->fb_height >= chr[index].y + chr[index].h)
            {
                gui_free(chr[index].buf);
                chr[index].buf = NULL;
                chr[index].char_w = 0;
            }
            continue;
        }
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
        float glyph_w = 0;
        float glyph_h = 0;

        float glyph_x0m = 0;
        float glyph_y0m = 0;
        float glyph_x1m = 0;
        float glyph_y1m = 0;

        int mx_offset = 0;
        int my_offset = 0;

        int render_w = 0;
        int render_h = 0;

        int out_x0 = 0;
        int out_y0 = 0;
        int out_x1 = 0;
        int out_y1 = 0;
        int out_w = 0;
        int out_h = 0;

        uint32_t block_bit = 32;
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
        GUI_ASSERT(line_count != 0);
        ttf_point *windingsf = gui_malloc(line_count * sizeof(ttf_point));

        switch (tm_type)
        {
        case FONT_HOMOGENEOUS:
            {
                glyph_x0 = scale * glyphData->x0;
                glyph_y0 = scale * (ascent + glyphData->y0);
                glyph_x1 = scale * (glyphData->x1);
                glyph_y1 = scale * (ascent + glyphData->y1);

                mx_offset = chr[index].x - text->offset_x;
                my_offset = chr[index].y - text->offset_y;

                computeBoundingBoxFloatV2(mx_offset + glyph_x0, my_offset + glyph_y0,
                                          mx_offset + glyph_x1, my_offset + glyph_y1,
                                          tm->m, &glyph_x0m, &glyph_x1m, &glyph_y0m, &glyph_y1m);

                glyph_x0m *= raster_prec;
                glyph_y0m *= raster_prec;
                glyph_x1m *= raster_prec;
                glyph_y1m *= raster_prec;

                glyph_w = glyph_x1m - glyph_x0m + 1;
                glyph_h = glyph_y1m - glyph_y0m + 1;

                // gui_log("text out glyph_w %d  my0 %d out_w %d out_h %d\n",glyph_w,glyph_h);

                render_w = ALIGN_TO((int)(glyph_w + 1), raster_prec);
                render_h = ALIGN_TO((int)(glyph_h + 1), raster_prec);

                render_w = ALIGN_TO(render_w, block_bit);
                line_word = render_w / block_bit;

                out_w = render_w / raster_prec;
                out_h = render_h / raster_prec;

                out_x0 = glyph_x0 + ROUNDING_OFFSET;
                out_y0 = glyph_y0 + ROUNDING_OFFSET;
                out_x1 = out_x0 + out_w - 1;
                out_y1 = out_y0 + out_h - 1;

                computeBoundingBoxIntV2(mx_offset + out_x0, my_offset + out_y0,
                                        mx_offset + out_x1, my_offset + out_y1,
                                        tm->m, &mx0, &mx1, &my0, &my1);

#if defined FONT_TTF_USE_MVE && 0
                /*MVE code time is equal to normal code(auto vectorize).If add matrix, should try again.*/
                /*todo by luke*/
                windingsfm = gui_malloc(line_count * sizeof(ttf_point));
                FontWindings *windingsd = gui_malloc(line_count * sizeof(FontWindings));
                memcpy(windingsd, windings, line_count * sizeof(FontWindings));
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = windingsd[i].x;
                    windingsf[i].x = windingsf[i].x * scale;
                    windingsf[i].x += mx_offset;

                    windingsf[i].y = ascent - windingsd[i].y;
                    windingsf[i].y = windingsf[i].y * scale;
                    windingsf[i].y += my_offset;
                }
                for (int i = 0; i < line_count; i++)
                {
                    transformPointV2(windingsf[i].x, windingsf[i].y, tm->m, &windingsfm[i].x, &windingsfm[i].y);
                }
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = windingsfm[i].x * raster_prec - glyph_x0m;
                    windingsf[i].y = windingsfm[i].y * raster_prec - glyph_y0m;
                }
                gui_free(windingsd);
#else
#if FIX_AUTO_VECTORIZE
                windingsfm = gui_malloc(line_count * sizeof(ttf_point));
                FontWindings *windingsd = gui_malloc(line_count * sizeof(FontWindings));
                memcpy(windingsd, windings, line_count * sizeof(FontWindings));
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = windingsd[i].x;
                    windingsf[i].x = windingsf[i].x * scale;
                    windingsf[i].x += mx_offset;

                    windingsf[i].y = ascent - windingsd[i].y;
                    windingsf[i].y = windingsf[i].y * scale;
                    windingsf[i].y += my_offset;
                }
                for (int i = 0; i < line_count; i++)
                {
                    transformPointV2(windingsf[i].x, windingsf[i].y, tm->m, &windingsfm[i].x, &windingsfm[i].y);
                }
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = windingsfm[i].x * raster_prec - glyph_x0m;
                    windingsf[i].y = windingsfm[i].y * raster_prec - glyph_y0m;
                }
                gui_free(windingsfm);
                gui_free(windingsd);
#else
                windingsfm = gui_malloc(line_count * sizeof(ttf_point));
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = windings[i].x;
                    windingsf[i].x = windingsf[i].x * scale;
                    windingsf[i].x += mx_offset;

                    windingsf[i].y = ascent - windings[i].y;
                    windingsf[i].y = windingsf[i].y * scale;
                    windingsf[i].y += my_offset;
                }
                for (int i = 0; i < line_count; i++)
                {
                    transformPointV2(windingsf[i].x, windingsf[i].y, tm->m, &windingsfm[i].x, &windingsfm[i].y);
                }
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = windingsfm[i].x * raster_prec - glyph_x0m;
                    windingsf[i].y = windingsfm[i].y * raster_prec - glyph_y0m;
                }
                gui_free(windingsfm);
#endif
#endif
            }
            break;
        case FONT_SCALE:
            {
                glyph_x0 = render_scale * glyphData->x0;
                glyph_y0 = render_scale * (ascent + glyphData->y0);
                glyph_x1 = render_scale * glyphData->x1;
                glyph_y1 = render_scale * (ascent + glyphData->y1);

                computeBoundingBoxFloat(glyph_x0, glyph_y0,
                                        glyph_x1, glyph_y1,
                                        tm->m, caseX, caseY,
                                        &glyph_x0m, &glyph_x1m, &glyph_y0m, &glyph_y1m);

                glyph_w = glyph_x1m - glyph_x0m + 1;
                glyph_h = glyph_y1m - glyph_y0m + 1;

                render_w = ALIGN_TO((int)(glyph_w + 1), raster_prec);
                render_h = ALIGN_TO((int)(glyph_h + 1), raster_prec);

                render_w = ALIGN_TO(render_w, block_bit);
                line_word = render_w / block_bit;

                out_w = render_w / raster_prec;
                out_h = render_h / raster_prec;

                out_x0 = glyph_x0 / raster_prec + ROUNDING_OFFSET;
                out_y0 = glyph_y0 / raster_prec + ROUNDING_OFFSET;
                out_x1 = out_x0 + out_w - 1;
                out_y1 = out_y0 + out_h - 1;

                mx_offset = chr[index].x - text->offset_x;
                my_offset = chr[index].y - text->offset_y;

                computeBoundingBoxInt(mx_offset + out_x0, my_offset + out_y0,
                                      mx_offset + out_x1, my_offset + out_y1,
                                      tm->m, caseX, caseY,
                                      &mx0, &mx1, &my0, &my1);

#if defined FONT_TTF_USE_MVE && 0
                //MVE code time is equal to normal code(auto vectorize).If add matrix, should try again.
                /*todo by luke*/
                windingsfm = gui_malloc(line_count * sizeof(ttf_point));
                FontWindings *windingsd = gui_malloc(line_count * sizeof(FontWindings));
                memcpy(windingsd, windings, line_count * sizeof(FontWindings));
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = windingsd[i].x * render_scale;
                    windingsf[i].y = (ascent - windingsd[i].y) * render_scale;
                }
                for (int i = 0; i < line_count; i++)
                {
                    transformPoint(windingsf[i].x, windingsf[i].y, tm->m, &windingsfm[i].x, &windingsfm[i].y);
                }
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = windingsfm[i].x - glyph_x0m;
                    windingsf[i].y = windingsfm[i].y - glyph_y0m;
                }
                gui_free(windingsfm);
                gui_free(windingsd);
#else
#if FIX_AUTO_VECTORIZE
                windingsfm = gui_malloc(line_count * sizeof(ttf_point));
                FontWindings *windingsd = gui_malloc(line_count * sizeof(FontWindings));
                memcpy(windingsd, windings, line_count * sizeof(FontWindings));
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = windingsd[i].x * render_scale;
                    windingsf[i].y = (ascent - windingsd[i].y) * render_scale;
                }
                for (int i = 0; i < line_count; i++)
                {
                    transformPoint(windingsf[i].x, windingsf[i].y, tm->m, &windingsfm[i].x, &windingsfm[i].y);
                }
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = windingsfm[i].x - glyph_x0m;
                    windingsf[i].y = windingsfm[i].y - glyph_y0m;
                }
                gui_free(windingsfm);
                gui_free(windingsd);
#else
                windingsfm = gui_malloc(line_count * sizeof(ttf_point));
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = windings[i].x * render_scale;
                    windingsf[i].y = (ascent - windings[i].y) * render_scale;
                }
                for (int i = 0; i < line_count; i++)
                {
                    transformPoint(windingsf[i].x, windingsf[i].y, tm->m, &windingsfm[i].x, &windingsfm[i].y);
                }
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = windingsfm[i].x - glyph_x0m;
                    windingsf[i].y = windingsfm[i].y - glyph_y0m;
                }
                gui_free(windingsfm);
#endif
#endif
            }
            break;
        case FONT_TRANSFORM:
        case FONT_IDENTITY:
            {
                glyph_x0 = render_scale * glyphData->x0;
                glyph_y0 = render_scale * glyphData->y0;
                glyph_x1 = render_scale * glyphData->x1;
                glyph_y1 = render_scale * glyphData->y1;

                glyph_w = glyph_x1 - glyph_x0 + 1;
                glyph_h = glyph_y1 - glyph_y0 + 1;

                render_w = ALIGN_TO((int)(glyph_w + 1), raster_prec);
                render_h = ALIGN_TO((int)(glyph_h + 1), raster_prec);

                render_w = ALIGN_TO(render_w, block_bit);
                line_word = render_w / block_bit;

                out_w = render_w / raster_prec;
                out_h = render_h / raster_prec;

                out_x0 = glyph_x0 / raster_prec + ROUNDING_OFFSET;
                out_y0 = glyph_y0 / raster_prec + ascent * scale + ROUNDING_OFFSET;
                out_x1 = out_x0 + out_w - 1;
                out_y1 = out_y0 + out_h - 1;

                mx0 = out_x0 + chr[index].x;
                mx1 = out_x1 + chr[index].x;
                my0 = out_y0 + chr[index].y;
                my1 = out_y1 + chr[index].y;

#if defined FONT_TTF_USE_MVE && 0
                //MVE code time >= normal code.
                FontWindings *windingsd = gui_malloc(line_count * sizeof(FontWindings));
                memcpy(windingsd, windings, line_count * sizeof(FontWindings));

                uint32_t li = 0;

                /*MVE Code Start*/
                const float32x4_t addv = {-glyph_x0, -glyph_y0, -glyph_x0, -glyph_y0};
                const float32x4_t mulv = {render_scale, -render_scale, render_scale, -render_scale};

                for (; li + 1 < line_count; li += 2)
                {
                    int32x4_t resulti = vldrhq_s32(&windingsd[li].x);
                    float32x4_t result = vcvtq_f32_s32(resulti);

                    result = vmulq_f32(result, mulv);
                    result = vaddq_f32(result, addv);
                    vstrwq_f32(&windingsf[li].x, result);
                }
                /*MVE Code End*/
                for (; li < line_count; li ++)
                {
                    windingsf[li].x = (windingsd[li].x - glyphData->x0) * render_scale;
                    windingsf[li].y = (- glyphData->y0 - windingsd[li].y) * render_scale;
                }
                gui_free(windingsd);
#else
#if FIX_AUTO_VECTORIZE
                FontWindings *windingsd = gui_malloc(line_count * sizeof(FontWindings));
                memcpy(windingsd, windings, line_count * sizeof(FontWindings));
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = (windingsd[i].x - glyphData->x0) * render_scale;
                    windingsf[i].y = (- glyphData->y0 - windingsd[i].y) * render_scale;
                }
                gui_free(windingsd);
#else
                for (int i = 0; i < line_count; i++)
                {
                    windingsf[i].x = (windings[i].x - glyphData->x0) * render_scale;
                    windingsf[i].y = (- glyphData->y0 - windings[i].y) * render_scale;
                }
#endif
#endif
            }
            break;
        default:
            break;
        }

        LINE_T *line_list = gui_malloc(line_count * sizeof(LINE_T));

        int lint_count_actual = 0;
        int winding_offset = 0;
        for (int i = 0; i < glyphData->winding_count; i++)
        {
            ttf_point *winding = windingsf + winding_offset;
            winding_offset += winding_lengths[i];
            for (int j = 0; j < winding_lengths[i] - 1; j++)
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
        memset(img, 0, render_size);

        uint32_t out_size = out_w * out_h;
        uint8_t *img_out = gui_malloc(out_size);

        for (int i = 0; i < lint_count_actual; i++)
        {
            for (int y = line_list[i].y0; y < line_list[i].y1; y++)
            {
                float xi = line_list[i].x0 + line_list[i].dxy * y;
                // GUI_ASSERT(xi >= 0)
                // GUI_ASSERT(xi <= render_w)
                // GUI_ASSERT(y >= 0)
                // GUI_ASSERT(y <= render_h)
                uint32_t xint = (uint32_t)(xi);
                img[y * line_word + xint / block_bit] ^= masks[xint % block_bit];
                for (uint32_t li = xint / block_bit + 1; li < line_word; li++)
                {
                    img[y * line_word + li] ^= masks[0];
                }
            }
        }
        makeImageBuffer(img_out, img, raster_prec, out_w, out_h, render_w, render_h, line_word, block_bit);

        adjustImageBufferPrecision(img_out, out_size, raster_prec);
        font_ttf_draw_bitmap_classic(text, img_out, rect, mx0, my0, out_w, out_h);

        gui_free(windingsf);
        gui_free(line_list);
        gui_free(img);

        if (dc->type == DC_RAMLESS)
        {
            chr[index].buf = img_out;
            chr[index].x = mx0;
            chr[index].y = my0;
            chr[index].w = out_w;
            chr[index].h = out_h;
            chr[index].char_w = mx1 - mx0 + 1;
            chr[index].char_h = my1 - my0 + 1;
        }
        else
        {
            gui_free(img_out);
        }
    }
}
