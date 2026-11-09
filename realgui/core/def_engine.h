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



/* Everything the blit stage needs to place one image on screen. The blit code
 * lives in engine/ and must not see widget headers, so this is the boundary
 * type: widgets fill it, engine reads it. */
typedef struct draw_img
{
    void *data;
    uint16_t img_w;
    uint16_t img_h;
    /* Target position relative to the top-left of the section being drawn:
     * source pixel (sx, sy) lands at (sx + rel_x, sy + rel_y) on screen. */
    int16_t rel_x;
    int16_t rel_y;
    uint32_t fg_color_set;  //A8 image set color
    uint32_t bg_color_fix;  //bg color fix for A8 image
    uint8_t opacity_value;
    uint8_t blend_mode;     // BLEND_MODE_TYPE
} draw_img_t;


typedef struct gui_blit_ops
{
    void (*init)(void);
    void (*process)(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);
    void (*deinit)(void);
} gui_blit_ops_t;

/* acc_engine_t struct define start */
typedef struct acc_engine
{
    const gui_blit_ops_t *blit;
    void (*fb_clear)(uint8_t *addr, gui_color_t color, uint32_t len);
    bool enable_thread_sync;
    void *cache_mem;
    uint32_t cache_size;

} acc_engine_t;
/* acc_engine_t struct define end */



#ifdef __cplusplus
}
#endif

#endif

