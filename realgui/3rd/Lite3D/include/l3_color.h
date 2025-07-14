#ifndef __L3_COLOR_H__
#define __L3_COLOR_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

/** @brief
  * @{
  * The color used in the GUI:
  *
  *         bit        bit
  * L3_RGB565  15 R,G,B   0
  * BGR565  15 B,G,R   0
  * L3_RGB888  23 R,G,B   0
  * ARGB888 31 A,R,G,B 0
  * RGBA888 31 R,G,B,A 0
  * ABGR888 31 A,B,G,R 0
  *
  * The gui_color is defined as ARGB888.
  *        bit31 A,R,G,B bit0
  */

#pragma pack(1)
typedef struct _l3_color_rgb888
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
} l3_color_rgb888_t;

typedef struct _l3_color_rgb565
{
    uint16_t b : 5;
    uint16_t g : 6;
    uint16_t r : 5;
} l3_color_rgb565_t;

typedef struct _l3_color_argb8565
{
    uint16_t b : 5;
    uint16_t g : 6;
    uint16_t r : 5;
    uint8_t a;
} l3_color_argb8565_t;

typedef struct _l3_color_argb8888
{
    uint8_t b; ///< Blue color component, at the lowest address
    uint8_t g; ///< Green color component
    uint8_t r; ///< Red color component
    uint8_t a; ///< Alpha channel for transparency, at the highest address
} l3_color_argb8888_t;
#pragma pack()

#ifdef __cplusplus
}
#endif

#endif