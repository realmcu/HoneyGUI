/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fb_sdl.h"
#include "gui_h264bsd.h"

#include "earth_240x240_header.txt"


__attribute__((constructor(1001))) static int h264bsd_example(void)
{
    printf("h264bsd example test code:\n");

    uint8_t *pixel = malloc(DRV_LCD_WIDTH * DRV_LCD_HEIGHT * sizeof(uint16_t));
    memset(pixel, 0x0, DRV_LCD_WIDTH * DRV_LCD_HEIGHT * sizeof(uint16_t));

    void *handle = gui_h264bsd_create_decoder((uint8_t *)_acearth_240x240_header + 24,
                                              sizeof(_acearth_240x240_header));

    gui_h264bsd_get_frame(handle, pixel, DRV_LCD_WIDTH * DRV_LCD_HEIGHT * sizeof(uint16_t));

    port_direct_draw_bitmap_to_lcd(0, 0, DRV_LCD_WIDTH, DRV_LCD_HEIGHT, (uint8_t *)pixel);




    return 0;
}