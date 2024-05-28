
#ifndef __DEF_VG_H__
#define __DEF_VG_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


typedef enum CAP
{
    STROKE_CAP_BUTT,
    STROKE_CAP_ROUND,
    STROKE_CAP_SQUARE,
} STROKE_LINECAP;

typedef enum JOIN
{
    STROKE_JOIN_MITER,
    STROKE_JOIN_ROUND,
    STROKE_JOIN_BEVEL,
} STROKE_LINEJOIN;

typedef struct vg_fill
{
    union
    {
        uint32_t rgba;
        // canvas_linear_gradient_t linear_gradient;
        // canvas_radial_gradient_t radial_gradient;
    } color_data;
    // FILL_TYPE type;
} vg_fill_t;

typedef struct vg_stroke
{
    STROKE_LINECAP stroke_linecap;
    STROKE_LINEJOIN stroke_linejoin;
    // float *dash;
    // uint32_t dash_count;
    // float dash_phase;
    float stroke_width;
    // float miter_limit;
    vg_fill_t fill;
} vg_stroke_t;

typedef struct vg_arc
{
    float cx;
    float cy;
    float r;
    float a0;
    float a1;
    int dir;
    vg_fill_t fill;
    vg_stroke_t stroke;
} vg_arc_t;






#ifdef __cplusplus
}
#endif

#endif

