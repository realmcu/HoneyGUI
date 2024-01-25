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



struct gui_rgb_data_head
{
    unsigned char scan : 1;
    unsigned char rsvd : 4;
    unsigned char compress : 3;
    char type;
    short w;
    short h;
    char version;
    char rsvd2;
};

typedef enum
{
    RGB565 = 0,
    ARGB8565 = 1,
    RGB888 = 3,
    RGBA8888 = 4,
    BINARY = 5,
    ALPHAMASK = 9,
    IMDC_COMPRESS = 10,
    BMP = 11,
    JPEG = 12,
    PNG = 13,
    GIF = 14,
} GUI_FormatType;


typedef enum
{
    IMG_BYPASS_MODE = 0,
    IMG_FILTER_BLACK,
    IMG_SRC_OVER_MODE, //S * Sa + (1 - Sa) * D
} BLEND_MODE_TYPE;

typedef enum
{
    IMG_SRC_MEMADDR = 0,
    IMG_SRC_FILESYS,
} IMG_SOURCE_MODE_TYPE;

typedef struct draw_img
{
    int16_t img_x;
    int16_t img_y;
    uint16_t img_w;
    uint16_t img_h;
    uint16_t target_w;
    uint16_t target_h;
    void *data;
    struct gui_matrix *matrix;
    struct gui_matrix *inverse;
    unsigned char opacity_value;
    uint32_t blend_mode : 3;
    uint32_t checksum : 8;
    uint32_t src_mode : 3;
    uint32_t high_quality : 1;
} draw_img_t;

void gui_image_load_scale(draw_img_t *img);
bool gui_image_new_area(draw_img_t *img);
bool gui_image_target_area(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect,
                           int16_t *x_start, int16_t *x_end, int16_t *y_start, int16_t *y_end);
struct gui_rgb_data_head rtgui_image_get_header(draw_img_t *img);

#ifdef __cplusplus
}
#endif

#endif

