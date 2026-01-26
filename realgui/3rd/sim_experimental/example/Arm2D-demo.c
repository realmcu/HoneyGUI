/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fb_sdl.h"
#include "nanovg.h"
#include "nanovg_agge.h"
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include "arm_2d.h"



static int ARM2D_example(void)
{
    printf("ARM2D example test code:\n");

    uint8_t *pixel = malloc(DRV_LCD_WIDTH * DRV_LCD_HEIGHT * DRV_PIXEL_BITS / 8);

    memset(pixel, 0x0, DRV_LCD_WIDTH * DRV_LCD_HEIGHT * DRV_PIXEL_BITS / 8);

    const arm_2d_tile_t rootTile =
    {
        .tRegion = {
            .tSize = {
                .iWidth = DRV_LCD_WIDTH,
                .iHeight = DRV_LCD_HEIGHT / 2,
            },
        },
        .tInfo = {
            .bIsRoot = true,
            .bHasEnforcedColour = true,
            .tColourInfo = {
                .chScheme = ARM_2D_COLOUR_CCCA8888,
            },
        },
        .pchBuffer = (uint8_t *)pixel,
    };

    const arm_2d_tile_t childTile =
    {
        .tRegion = {
            .tLocation = {
                .iX = 0,
                .iY = -DRV_LCD_HEIGHT / 2,
            },
            .tSize = {
                .iWidth = DRV_LCD_WIDTH,
                .iHeight = DRV_LCD_HEIGHT,
            },
        },
        .tInfo = {
            .bIsRoot = false,
            .bHasEnforcedColour = true,
            .bVirtualScreen = true,
            .tColourInfo = {
                .chScheme = ARM_2D_COLOUR_CCCA8888,
            },
        },
        .ptParent = &rootTile,
    };

    //arm_2d_rgb32_fill_colour(&ptTile, NULL, 0xFF0000FF);
    extern const arm_2d_tile_t c_tileALPHAMask;


    arm_2d_point_float_t tCentre =
    {
        .fX = 0,
        .fY = 0,
    };
    arm_2d_point_float_t tPivot =
    {
        .fX = 0,
        .fY = 0,
    };
    bool bIsNewFrame = true;
    arm_2dp_cccn888_fill_colour_with_mask_opacity_and_transform_xy(
        NULL,
        &c_tileALPHAMask,
        &childTile,
        NULL,
        tCentre,
        0.0f,
        1.0f,
        1.0f,
        0xFFFF00FF,
        255,
        &tPivot);


    port_direct_draw_bitmap_to_lcd(0, DRV_LCD_HEIGHT / 2, DRV_LCD_WIDTH, DRV_LCD_HEIGHT / 2,
                                   (uint8_t *)pixel);

    return 0;
}
__attribute__((constructor(1001))) static int arm2d_demo(void)
{
    return ARM2D_example();
}