/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __ACC_SW_ENTRY_H__
#define __ACC_SW_ENTRY_H__

#include "draw_img.h"
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void sw_acc_blit(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);
void sw_acc_init(void);

#ifdef __cplusplus
}
#endif

#endif
