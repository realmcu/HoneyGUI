/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __DEF_ENGINE_H__
#define __DEF_ENGINE_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "def_type.h"
#include "def_color.h"
#include "def_fb.h"



typedef struct draw_img
{
    uint16_t img_w;
    uint16_t img_h;
    int16_t img_target_x; //display start X
    int16_t img_target_y;  //display start Y
    uint16_t img_target_w;
    uint16_t img_target_h;
    void *data;
    gui_matrix_t matrix; //seems can remove by howie
    gui_matrix_t inverse;
    uint8_t opacity_value;
    uint32_t checksum : 8;
    uint32_t blend_mode : 5;
    uint32_t high_quality : 1;
    uint32_t fg_color_set;  //A8 image set color
    uint32_t bg_color_fix;  //bg color fix for A8 image
    uint8_t alpha_mix;      //alpha mix for A8 image
    void *acc_user;
    float raster_prog;
} draw_img_t;


typedef struct gui_blit_ops
{
    void (*init)(void);
    void (*process)(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);
    void (*deinit)(void);
} gui_blit_ops_t;

typedef struct gui_blur_ops
{
    void (*init)(void);
    void (*prepare)(void **cache_mem);
    void (*process)(uint8_t *buffer, uint16_t buffer_stride, uint16_t bit_depth,
                    const gui_rect_t *buffer_rect, const gui_rect_t *valid_rect,
                    const gui_rect_t *target_rect, uint8_t blur_degree, void *cache_mem);
    void (*release)(void **cache_mem);
    bool (*scale)(uint8_t *target_buffer, const uint8_t *source_buffer,
                  uint16_t source_width, uint16_t source_height,
                  uint16_t source_stride, uint16_t target_stride,
                  uint8_t factor, uint16_t target_width, uint16_t target_height,
                  uint8_t bytes_per_pixel, bool scale_up);
    void (*deinit)(void);
} gui_blur_ops_t;

typedef struct gui_jpeg_ops
{
    void (*init)(void);
    void *(*load)(void *input, int len, int *w, int *h, int *channel);
    void (*release)(void *data);
    void (*deinit)(void);
} gui_jpeg_ops_t;

typedef struct gui_idu_ops
{
    void (*init)(void);
    void *(*load)(void *input);
    void (*release)(void *data);
    void (*deinit)(void);
} gui_idu_ops_t;


/* acc_engine_t struct define start */
typedef struct acc_engine
{
    const gui_blit_ops_t *blit;
    void (*fb_clear)(uint8_t *addr, gui_color_t color, uint32_t len);
    gui_blur_ops_t *blur;
    const gui_jpeg_ops_t *jpeg;
    const gui_idu_ops_t *idu;
    bool enable_thread_sync;
    void *cache_mem;
    uint32_t cache_size;

} acc_engine_t;
/* acc_engine_t struct define end */



#ifdef __cplusplus
}
#endif

#endif

