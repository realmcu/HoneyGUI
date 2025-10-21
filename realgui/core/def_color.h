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


/** @brief
  * @{
  * The color used in the GUI:
  *
  *         bit        bit
  * RGB565  15 R,G,B   0
  * BGR565  15 B,G,R   0
  * RGB888  23 R,G,B   0
  * ARGB888 31 A,R,G,B 0
  * RGBA888 31 R,G,B,A 0
  * ABGR888 31 A,B,G,R 0
  * ALPHAMASK 7 A       0
  *
  * The gui_color is defined as ARGB888.
  *        bit31 A,R,G,B bit0
  */

/**
 * @brief Combines R, G, B, and A components into a 32-bit color value in ABGR8888 format.
 *
 * The resulting integer has the following byte layout:
 * - Bits 31-24: Alpha (A)
 * - Bits 23-16: Blue (B)
 * - Bits 15-8:  Green (G)
 * - Bits 7-0:   Red (R)
 *
 * @param r Red component (0-255).
 * @param g Green component (0-255).
 * @param b Blue component (0-255).
 * @param a Alpha component (0-255).
 * @return A 32-bit unsigned integer representing the color in ABGR8888 format (e.g., 0xAABBGGRR).
 */
#define GUI_COLOR_ARGB8888(a, r, g, b) \
    ( ((uint32_t)(a) << 24) | \
      ((uint32_t)(r) << 16) | \
      ((uint32_t)(g) << 8)  | \
      ((uint32_t)(b)) )


#pragma pack(1)
typedef struct _color_rgb888
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
} color_rgb888_t;

typedef struct _color_rgb565
{
    union
    {
        struct
        {
            uint16_t b : 5;
            uint16_t g : 6;
            uint16_t r : 5;
        } rgb_channel;
        uint16_t rgb565;
    } color;
} color_rgb565_t;

typedef struct _color_argb8565
{
    union
    {
        struct
        {
            uint16_t b : 5;
            uint16_t g : 6;
            uint16_t r : 5;
        } rgb_channel;
        uint16_t rgb565;
    } color;
    uint8_t a;
} color_argb8565_t;


typedef struct _color_argb88888
{
    union
    {
        struct
        {
            uint8_t b; ///< Blue color component, at the lowest address
            uint8_t g; ///< Green color component
            uint8_t r; ///< Red color component
            uint8_t a; ///< Alpha channel for transparency, at the highest address
        } argb_channel;
        uint32_t argb8888;
    } color;
} color_argb8888_t;

typedef struct _color_a8
{
    uint8_t a; ///< Alpha channel for transparency, at the highest address
} color_a8_t;
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

typedef struct _gui_surface
{
    gui_color_t *surfaceBuffer;
    int width;
    int height;

    int limit_left;
    int limit_top;
    int limit_right;
    int limit_bottom;
} gui_surface_t;

#ifdef __cplusplus
}
#endif

#endif

