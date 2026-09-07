/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fb_sdl.h"
#include "l3.h"
#include "desc_earth_tria.txt"

#define MODEL_CANVAS_WIDTH 480
#define MODEL_CANVAS_HEIGHT 480

static float rot_x_angle = 0.0f;
static float rot_y_angle = 0.0f;

static void earth_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 10), 1, 32767,
                             90,  this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, 0, 0, 10, rot_x_angle, rot_y_angle, 0, 5);
}

static int lite3D_example(void)
{
    printf("Lite3D example test code!\n");

    uint8_t *pixel = malloc(DRV_LCD_WIDTH * DRV_LCD_HEIGHT * sizeof(uint16_t));
    memset(pixel, 0x0, DRV_LCD_WIDTH * DRV_LCD_HEIGHT * sizeof(uint16_t));

    l3_model_base_t *earth_3d = l3_create_model((void *)_acdesc_earth_tria,
                                                L3_DRAW_FRONT_AND_SORT, 0, 0,
                                                MODEL_CANVAS_WIDTH, MODEL_CANVAS_HEIGHT);
    if (earth_3d == NULL)
    {
        fprintf(stderr, "Failed to create the Earth model\n");
        free(pixel);
        return -1;
    }

    l3_set_global_transform(earth_3d, (l3_global_transform_cb)earth_global_cb);

    l3_set_target_canvas(earth_3d, 0, 0, MODEL_CANVAS_WIDTH, MODEL_CANVAS_HEIGHT,
                         16/*LITE_RGB565*/,
                         pixel);

    l3_push(earth_3d, NULL);
    l3_draw(earth_3d);

    port_direct_draw_bitmap_to_lcd(0, 0, DRV_LCD_WIDTH, DRV_LCD_HEIGHT, (uint8_t *)pixel);

    l3_free_model(earth_3d);
    free(pixel);

    return 0;
}
__attribute__((constructor(1001))) static int lite3d_demo(void)
{
    return lite3D_example();
}
