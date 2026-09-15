/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "acc_sw_entry.h"
#include "acc_sw_compressed.h"
#include "acc_sw_uncompressed.h"
#include "gui_post_process.h"

static void sw_acc_prepare_cb(draw_img_t *image, gui_rect_t *rect)
{
    (void)image;
    (void)rect;
}

static void sw_acc_end_cb(draw_img_t *image)
{
    if (image->acc_user != NULL && blur_depose != NULL)
    {
        blur_depose(&image->acc_user);
    }
}

void sw_acc_blit(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect)
{
    if (image == NULL || image->data == NULL)
    {
        return;
    }

    gui_rgb_data_head_t *header = (gui_rgb_data_head_t *)image->data;

    if (header->compress)
    {
        sw_acc_blit_compressed(image, dc, rect);
    }
    else
    {
        sw_acc_blit_uncompressed(image, dc, rect);
    }
}

void sw_acc_init(void)
{
    draw_img_acc_prepare_cb = sw_acc_prepare_cb;
    draw_img_acc_end_cb = sw_acc_end_cb;
}
