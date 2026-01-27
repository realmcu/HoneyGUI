/**
 * @file l3_sw_raster.h
 * @author wanghao@realsil.com.cn
 * @brief
 * @version 0.1
 * @date 2023-12-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __L3_SW_RASTER_H__
#define __L3_SW_RASTER_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "l3_common.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    L3_RGB565 = 0, //bit[4:0] for Blue, bit[10:5] for Green, bit[15:11] for Red, pls refs def_def.h
    L3_ARGB8565 = 1, //bit[4:0] for Blue, bit[10:5] for Green, bit[15:11] for Red, bit[23:16] for Alpha
    L3_RGB888 = 3, //bit[7:0] for Blue, bit[15:8] for Green, bit[23:16] for Red
    L3_ARGB8888 = 4, //bit[7:0] for Blue, bit[15:8] for Green, bit[23:16] for Red, bit[24:31] for Alpha
    L3_XRGB8888 = 5, //bit[7:0] for Blue, bit[15:8] for Green, bit[23:16] for Red, bit[24:31] for (255 - Alpha)
} L3_FormatType;

/* BLEND_MODE_TYPE structure start*/
typedef enum
{
    L3_IMG_BYPASS_MODE = 0,
    L3_IMG_FILTER_BLACK,
    L3_IMG_SRC_OVER_MODE, //S * Sa + (1 - Sa) * D
    L3_IMG_COVER_MODE,
    L3_IMG_RECT,
} L3_BLEND_MODE_TYPE;
/* BLEND_MODE_TYPE structure end*/

typedef struct l3_imdc_file_header
{
    struct
    {
        uint8_t algorithm: 2;
        uint8_t feature_1: 2;
        uint8_t feature_2: 2;
        uint8_t pixel_bytes: 2;
    } algorithm_type;
    uint8_t reserved[3];
    uint32_t raw_pic_width;
    uint32_t raw_pic_height;
} l3_imdc_file_header_t;

typedef struct l3_imdc_file
{
    l3_imdc_file_header_t header;
    uint32_t compressed_addr[1024];

} l3_imdc_file_t;

typedef struct l3_img_file
{
    l3_img_head_t img_header;
    union
    {
        l3_imdc_file_t imdc_file;
        uint32_t unzip_data[1024];
    } data;

} l3_img_file_t;

#pragma pack(1)

typedef struct l3_imdc_rgb565_node
{
    uint8_t len;
    uint16_t pixel16;  //rgb565
} l3_imdc_rgb565_node_t;
typedef struct l3_imdc_argb8565_node
{
    uint8_t len;
    uint16_t pixel;  //argb8565
    uint8_t alpha;
} l3_imdc_argb8565_node_t;
typedef struct l3_imdc_rgb888_node
{
    uint8_t len;
    uint8_t pixel_b;  //rgb888
    uint8_t pixel_g;
    uint8_t pixel_r;
} l3_imdc_rgb888_node_t;
typedef struct l3_imdc_argb8888_node
{
    uint8_t len;
    uint32_t pixel32;    //argb8888
} l3_imdc_argb8888_node_t;
typedef struct l3_imdc_u8_node
{
    uint8_t len;
    uint8_t value;  // I8 palette index or A8 alpha value
} l3_imdc_u8_node_t;
#pragma pack()

typedef struct
{
    uint8_t *writebuf;
    int write_off;
    uintptr_t image_base;
    uint32_t image_off;
    char input_type;
    uint8_t dc_bytes_per_pixel;
    uint8_t opacity_value;
    uint32_t blend_mode;
    uint8_t *palette_data;  // Pointer to CLUT data for I8 format
} gui_raster_params_t;


void do_raster_pixel(const gui_raster_params_t *params);
void gui_get_rle_pixel(l3_draw_rect_img_t *image, int x, int y, uint8_t *pixel);
bool l3_draw_img_target_area(l3_draw_rect_img_t *image, l3_canvas_t *dc, l3_rect_t *rect,
                             int32_t *x_start, int32_t *x_end, int32_t *y_start, int32_t *y_end);
void gui_get_source_color(uint8_t *source_red, uint8_t *source_green, uint8_t *source_blue,
                          uint8_t *source_alpha,
                          uintptr_t image_base, uint32_t image_off, char input_type,
                          uint8_t *palette_data);

#ifdef __cplusplus
}
#endif

#endif
