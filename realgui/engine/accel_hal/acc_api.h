/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __ACC_API_H__
#define __ACC_API_H__

#include "guidef.h"
#include "draw_img.h"



#ifdef __cplusplus
extern "C" {
#endif

void gui_acc_blit_to_dc(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);


#ifdef __cplusplus
}
#endif

#endif
