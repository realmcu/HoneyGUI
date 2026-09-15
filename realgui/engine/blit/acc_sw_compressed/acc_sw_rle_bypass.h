/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __ACC_SW_RLE_BYPASS_H__
#define __ACC_SW_RLE_BYPASS_H__

#include "guidef.h"
#include "draw_img.h"


#ifdef __cplusplus
extern "C" {
#endif

void rle_bypass_blit_2_rgb565(draw_img_t *image, gui_dispdev_t *dc,
                              struct gui_rect *rect);

#ifdef __cplusplus
}
#endif

#endif
