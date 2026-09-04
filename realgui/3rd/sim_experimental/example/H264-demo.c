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

#define H264_FRAME_WIDTH  240
#define H264_FRAME_HEIGHT 240
#define H264_STREAM_OFFSET 24U

__attribute__((constructor(1001))) static int h264bsd_example(void)
{
    printf("h264bsd example test code:\n");

    uint8_t *pixel = calloc(H264_FRAME_WIDTH * H264_FRAME_HEIGHT, sizeof(uint16_t));
    if (pixel == NULL)
    {
        return -1;
    }

    void *handle = gui_h264bsd_create_decoder((uint8_t *)_acearth_240x240_header + H264_STREAM_OFFSET,
                                              sizeof(_acearth_240x240_header) - H264_STREAM_OFFSET);
    if (handle == NULL)
    {
        free(pixel);
        return -1;
    }

    if (gui_h264bsd_get_frame(handle, pixel,
                              H264_FRAME_WIDTH * H264_FRAME_HEIGHT * sizeof(uint16_t)) ==
        H264BSD_SUCCESS)
    {
        port_direct_draw_bitmap_to_lcd(0, 0, H264_FRAME_WIDTH, H264_FRAME_HEIGHT, pixel);
    }

    gui_h264bsd_destroy_decoder(handle);
    free(pixel);

    return 0;
}
