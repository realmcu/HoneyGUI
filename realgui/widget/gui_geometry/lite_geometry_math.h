/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LITE_GEOMETRY_MATH_H
#define LITE_GEOMETRY_MATH_H
#include "lite_geometry.h"

#define LG_TRIGO_SHIFT 15 /**<  >> LG_TRIGO_SHIFT to normalize*/
#define LG_RADIUS_CIRCLE 0x7FFF


typedef struct
{
    uint16_t i;
    uint16_t f;
} lg_sqrt_res_t;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static inline float clamp(float x, float min_val, float max_val)
{
    if (x < min_val) { return min_val; }
    if (x > max_val) { return max_val; }
    return x;
}
static inline float smoothstep(float edge0, float edge1, float x)
{
    if (edge0 > edge1)
    {
        float temp = edge0;
        edge0 = edge1;
        edge1 = temp;
        x = -x;
    }

    x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}
// Sine function using a lookup table
int32_t lg_trigo_sin(int16_t angle);
int32_t lg_trigo_cos(int16_t angle);
uint16_t lg_atan2(int x, int y);
void lg_sqrt(uint32_t x, lg_sqrt_res_t *q, uint32_t mask);
int32_t lg_sqrt32(uint32_t x);
int64_t lg_pow(int64_t base, int8_t exp);

#endif  // LITE_GEOMETRY_MATH_H