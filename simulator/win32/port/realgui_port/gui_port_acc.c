/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <draw_img.h>
#include "gui_api.h"


extern void sw_acc_init(void);
extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);
extern void sw_acc_deinit(void);

extern void *gui_acc_decode(void *in);

static gui_blit_ops_t sw_acc_blit_ops =
{
    .init = sw_acc_init,
    .process = sw_acc_blit,
    .deinit = sw_acc_deinit,
};

static const gui_idu_ops_t idu_ops =
{
    .load = gui_acc_decode,
    .release = gui_free,
};

static acc_engine_t acc =
{
    .blit = &sw_acc_blit_ops,
    .idu = &idu_ops,
};

void gui_port_acc_init(void)
{
    gui_acc_info_register(&acc);
}

void gui_port_acc_deinit(void)
{
    gui_acc_info_register(NULL);
}
