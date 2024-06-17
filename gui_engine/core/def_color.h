/**
 * @file def_color.h
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-05-31
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __DEF_COLOR_H__
#define __DEF_COLOR_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#pragma pack(1)
typedef struct _color_rgb888
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
} color_rgb888_t;

typedef struct _color_rgb565
{
    uint16_t b : 5;
    uint16_t g : 6;
    uint16_t r : 5;
} color_rgb565_t;

typedef struct _color_argb8565
{
    uint16_t b : 5;
    uint16_t g : 6;
    uint16_t r : 5;
    uint8_t a;
} color_argb8565_t;

typedef struct _color_argb88888
{
    uint8_t b; ///< Blue color component, at the lowest address
    uint8_t g; ///< Green color component
    uint8_t r; ///< Red color component
    uint8_t a; ///< Alpha channel for transparency, at the highest address
} color_argb8888_t;
#pragma pack()

typedef struct _gui_color
{
    union
    {
        struct
        {
            uint8_t b; ///< Blue color component, at the lowest address
            uint8_t g; ///< Green color component
            uint8_t r; ///< Red color component
            uint8_t a; ///< Alpha channel for transparency, at the highest address
        } rgba;
        uint32_t argb_full;
    } color;
} gui_color_t;

#ifdef __cplusplus
}
#endif

#endif

