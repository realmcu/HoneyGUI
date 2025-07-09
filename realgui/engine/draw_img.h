/*
 * File      : draw_img.h
 */
#ifndef __DRAW_IMAGE_H__
#define __DRAW_IMAGE_H__

#include <guidef.h>
#include <gui_api.h>
#include <gui_matrix.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    BYTE_PIXEL_RGB565 = 2,
    BYTE_PIXEL_RGB888 = 3,
    BYTE_PIXEL_RGBA8888 = 4,
} GUI_BYTE_PIXEL_ENUM;



typedef enum
{
    RGB565 = 0, //bit[4:0] for Blue, bit[10:5] for Green, bit[15:11] for Red, pls refs def_def.h
    ARGB8565 = 1, //bit[4:0] for Blue, bit[10:5] for Green, bit[15:11] for Red, bit[23:16] for Alpha
    RGB888 = 3, //bit[7:0] for Blue, bit[15:8] for Green, bit[23:16] for Red
    ARGB8888 = 4, //bit[7:0] for Blue, bit[15:8] for Green, bit[23:16] for Red, bit[24:31] for Alpha
    XRGB8888 = 5, //bit[7:0] for Blue, bit[15:8] for Green, bit[23:16] for Red, bit[24:31] for (255 - Alpha)
    BINARY = 6,
    ALPHAMASK = 9,
    PALETTE = 10,
    BMP = 11,
    JPEG = 12,
    PNG = 13,
    GIF = 14,
    RTKARGB8565 = 15,

    ALPHA1BIT = 0X20, /*Gray 2*/
    ALPHA2BIT = 0X21, /*Gray 4*/
    ALPHA4BIT = 0X22, /*Gray 16*/
    ALPHA8BIT = 0X23, /*Gray 256*/
} GUI_FormatType;

/* BLEND_MODE_TYPE structure start*/
typedef enum
{
    IMG_BYPASS_MODE = 0,
    IMG_FILTER_BLACK,
    IMG_SRC_OVER_MODE, //S * Sa + (1 - Sa) * D
    IMG_COVER_MODE,
    IMG_RECT,
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

