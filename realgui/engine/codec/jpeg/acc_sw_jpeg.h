/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __ACC_SW_JPEG_H__
#define __ACC_SW_JPEG_H__

#include "guidef.h"

#ifdef __cplusplus
extern "C" {
#endif

void *gui_acc_jpeg_load(void *input, int len, int *w, int *h, int *channel);
void gui_acc_jpeg_free(void *decode_image);

#ifdef __cplusplus
}
#endif

#endif
