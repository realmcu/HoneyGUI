/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __A8_2_RGB565_H__
#define __A8_2_RGB565_H__

#include "draw_img.h"
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void a8_2_rgb565(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);

#ifdef __cplusplus
}
#endif

#endif
