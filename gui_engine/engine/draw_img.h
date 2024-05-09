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
    RGB565 = 0, //bit[4:0] for Blue, bit[10:5] for Green, bit[15:11] for Red
    ARGB8565 = 1, //bit[4:0] for Blue, bit[10:5] for Green, bit[15:11] for Red, bit[23:16] for Alpha
    RGB888 = 3, //bit[7:0] for Blue, bit[15:8] for Green, bit[23:16] for Red
    RGBA8888 = 4, //bit[7:0] for Blue, bit[15:8] for Green, bit[23:16] for Red, bit[21:24] for Alpha
    BINARY = 5,
    ALPHAMASK = 9,
    BMP = 11,
    JPEG = 12,
    PNG = 13,
    GIF = 14,
    RTKARGB8565 = 15,
} GUI_FormatType;


typedef enum
{
    IMG_BYPASS_MODE = 0,
    IMG_FILTER_BLACK,
    IMG_SRC_OVER_MODE, //S * Sa + (1 - Sa) * D
    IMG_COVER_MODE,
    IMG_RECT,
} BLEND_MODE_TYPE;

typedef enum
{
    IMG_SRC_MEMADDR = 0,
    IMG_SRC_FILESYS,
} IMG_SOURCE_MODE_TYPE;

typedef struct draw_img
{
    uint16_t img_w;
    uint16_t img_h;
    int16_t img_target_x; //display start X
    int16_t img_target_y;  //display start Y
    uint16_t img_target_w;
    uint16_t img_target_h;
    void *data;
    gui_matrix_t matrix; //seems can remve by howie
    gui_matrix_t inverse;
    uint8_t opacity_value;
    uint32_t blend_mode : 3;
    uint32_t checksum : 8;
    uint32_t src_mode : 3;
    uint32_t high_quality : 1;
    //uint32_t color_mix; //todo for QuDai
    void *acc_user;
} draw_img_t;

void gui_image_load_scale(draw_img_t *img);
bool gui_image_new_area(draw_img_t *img, gui_rect_t *rect);
bool gui_image_target_area(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect,
                           int16_t *x_start, int16_t *x_end, int16_t *y_start, int16_t *y_end);
gui_rgb_data_head_t gui_image_get_header(draw_img_t *img);
uint32_t gui_image_get_pixel(draw_img_t *img);

extern void (* gui_image_acc_prepare_cb)(struct draw_img *image, gui_rect_t *rect);
extern void (* gui_image_acc_end_cb)(struct draw_img *image);

#ifdef __cplusplus
}
#endif

#endif

