/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __ACC_SW_GENERIC_H__
#define __ACC_SW_GENERIC_H__

#include "guidef.h"
#include "draw_img.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Blit an image with the generic pixel-by-pixel software path.
 *
 * This common software fallback supports both uncompressed images and
 * RLE-compressed images.
 *
 * @param image Image source and rendering parameters.
 * @param dc Display device containing the destination frame buffer.
 * @param rect Optional clipping rectangle, or NULL to use the image bounds.
 */
void sw_acc_blit_generic(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);

#ifdef __cplusplus
}
#endif

#endif
