/*
 * File      : draw_img.h
 */
#ifndef __DRAW_IMAGE_H__
#define __DRAW_IMAGE_H__

#include "guidef.h"
#include "gui_api.h"
#include "gui_matrix.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    BYTE_PIXEL_RGB565 = 2,
    BYTE_PIXEL_RGB888 = 3,
    BYTE_PIXEL_RGBA8888 = 4,
} GUI_BYTE_PIXEL_ENUM;

/**
 * GUI_FormatType
 * Pixel and image format identifiers.
 * Note: Bit indices refer to positions within the pixel word where bit0 is LSB.
 */
typedef enum
{
    /* Packed RGB formats, pls refs def_color.h */
    RGB565       = 0,  /* 16-bit: bit[4:0] for Blue, bit[10:5] for Green, bit[15:11] for Red */
    ARGB8565     = 1,  /* 24-bit: bit[4:0] for Blue, bit[10:5] for Green, bit[15:11] for Red, bit[23:16] for Alpha, packed*/
    RGB888       = 3,  /* 24-bit: bit[7:0] for Blue, bit[15:8] for Green, bit[23:16] for Red */
    ARGB8888     = 4,  /* 32-bit: bit[7:0] for Blue, bit[15:8] for Green, bit[23:16] for Red, bit[31:24] for Alpha */
    XRGB8888     = 5,  /* 32-bit: bit[7:0] for Blue, bit[15:8] for Green, bit[23:16] for Red, bit[31:24] for X (unused or fixed 0xFF) */

    /* Luma / mask / indexed formats */
    BINARY       = 6,  /* 1-bit monochrome (black/white) */
    GRAY         = 7,  /* Grayscale, 8 bits per pixel */
    ALPHAMASK    = 9,  /* Alpha-only mask (default 8 bits) */
    PALETTE      = 10, /* Indexed color using a palette (CLUT) */

    /* Encoded image container formats */
    BMP          = 11, /* BMP image file format */
    JPEG         = 12, /* JPEG image file format */
    PNG          = 13, /* PNG image file format */
    GIF          = 14, /* GIF image file format */

    RTKARGB8565  = 15, /* Planar: [A 8bpp plane][RGB565 plane], not interleaved */

    /* Grayscale subtypes (luminance only) */
    GRAY1        = BINARY, /* 1 bpp grayscale (2 levels) */
    GRAY2        = 0x21,   /* 2 bpp grayscale (4 levels) */
    GRAY4        = 0x22,   /* 4 bpp grayscale (16 levels) */
    GRAY8        = GRAY,   /* 8 bpp grayscale (256 levels) */
    A8           = 0x30,   /* 8 bpp alpha (256 levels) */
    A4           = 0x31,   /* 4 bpp alpha (16 levels) */
    A2           = 0x32,   /* 2 bpp alpha (4 levels) */
    A1           = 0x33,   /* 1 bpp alpha (2 levels) */
    X8           = 0x34,   /* 8 bpp X (256 levels) */
    X4           = 0x35,   /* 4 bpp X (16 levels) */
    X2           = 0x36,   /* 2 bpp X (4 levels) */
    X1           = 0x37,   /* 1 bpp X (2 levels) */
    I8           = 0x38,   /* 8 bpp indexed (256 levels) */
    I4           = 0x39,   /* 4 bpp indexed (16 levels) */
    I2           = 0x3a,   /* 2 bpp indexed (4 levels) */
    I1           = 0x3b,   /* 1 bpp indexed (2 levels) */
} GUI_FormatType;

/* BLEND_MODE_TYPE structure start*/
typedef enum
{
    IMG_BYPASS_MODE = 0,
    IMG_FILTER_BLACK,
    IMG_SRC_OVER_MODE, //S * Sa + (1 - Sa) * D
    IMG_COVER_MODE,
    IMG_RECT,
    IMG_2D_SW_RGB565_ONLY,
    IMG_2D_SW_SRC_OVER_MODE, //S * Sa + (1 - Sa) * D
    IMG_2D_SW_FIX_A8_FG, //fixed fg
    IMG_2D_SW_FIX_A8_BGFG, //fixed fg and bg
} BLEND_MODE_TYPE;
/* BLEND_MODE_TYPE structure end*/

/* IMG_SOURCE_MODE_TYPE enum start*/
typedef enum
{
    IMG_SRC_MEMADDR = 0,
    IMG_SRC_FILESYS,
    IMG_SRC_FTL,
} IMG_SOURCE_MODE_TYPE;
/* IMG_SOURCE_MODE_TYPE enum end*/


void draw_img_load_scale(draw_img_t *img, IMG_SOURCE_MODE_TYPE src_mode);
bool draw_img_new_area(draw_img_t *img, gui_rect_t *rect);
gui_rgb_data_head_t draw_img_get_header(draw_img_t *img, IMG_SOURCE_MODE_TYPE src_mode);
uint32_t draw_img_get_pixel_byte(draw_img_t *img, IMG_SOURCE_MODE_TYPE src_mode);
void draw_img_cache(draw_img_t *img, IMG_SOURCE_MODE_TYPE src_mode);
void draw_img_free(draw_img_t *img, IMG_SOURCE_MODE_TYPE src_mode);
bool draw_img_target_area(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect,
                          int32_t *x_start, int32_t *x_end, int32_t *y_start, int32_t *y_end);
extern void (* draw_img_acc_prepare_cb)(struct draw_img *image, gui_rect_t *rect);
extern void (* draw_img_acc_end_cb)(struct draw_img *image);

#ifdef __cplusplus
}
#endif

#endif

