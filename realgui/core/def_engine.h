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

/* acc_engine_t struct define start */
typedef struct acc_engine
{
    void (*blit)(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);
    void (*fb_clear)(uint8_t *addr, gui_color_t color, uint32_t len);
    void (*blur)(gui_dispdev_t *dc, gui_rect_t *rect, uint8_t blur_degree, void *cache_mem);

    void *(* jpeg_load)(void *input, int len, int *w, int *h, int *channel);
    void (* jpeg_free)(void *);
    void *(* idu_load)(void *input);
    void (* idu_free)(void *);
    bool enable_async;
    bool enable_thread_sync;
    void *hw_acc_cache_mem; //for hardware acceleration cache memory
    uint32_t hw_acc_cache_size; //for hardware acceleration cache memory size

} acc_engine_t;
/* acc_engine_t struct define end */



#ifdef __cplusplus
}
#endif

#endif
