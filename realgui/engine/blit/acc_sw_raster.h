/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __ACC_SW_RASTER_H__
#define __ACC_SW_RASTER_H__

#include "guidef.h"
#include "draw_img.h"


#ifdef __cplusplus
extern "C" {
#endif

void do_raster(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);

#ifdef __cplusplus
}
#endif

#endif
