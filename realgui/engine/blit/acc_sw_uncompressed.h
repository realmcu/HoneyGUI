/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __ACC_SW_UNCOMPRESSED_H__
#define __ACC_SW_UNCOMPRESSED_H__

#include "draw_img.h"

#ifdef __cplusplus
extern "C" {
#endif

void sw_acc_blit_uncompressed(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);

#ifdef __cplusplus
}
#endif

#endif
