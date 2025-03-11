/**
 * @file def_matrix.h
 * @author wenjing_jiang(wenjing_jiang@realsil.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-06-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __DEF_TYPE_H__
#define __DEF_TYPE_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Swap the bytes of a 16-bit value
 */
#define GUI_SWAP16(x) ((uint16_t)(                              \
                                                                (((uint16_t)(x) & (uint16_t)0x00ff) << 8) |               \
                                                                (((uint16_t)(x) & (uint16_t)0xff00) >> 8)                 \
                                 ))

/**
 * @brief Swap the bytes of a 32-bit value
 */
#define GUI_SWAP32(x) ((uint32_t)(                              \
                                                                (((uint32_t)(x) & (uint32_t)0x000000ff) << 24) |          \
                                                                (((uint32_t)(x) & (uint32_t)0x0000ff00) <<  8) |          \
                                                                (((uint32_t)(x) & (uint32_t)0x00ff0000) >>  8) |          \
                                                                (((uint32_t)(x) & (uint32_t)0xff000000) >> 24)            \
                                 ))

#ifndef M_PI
#define M_PI    ((float)3.14159265358979323846)
#endif

#define _UI_MIN(x, y)           (((x)<(y))?(x):(y))
#define _UI_MAX(x, y)           (((x)>(y))?(x):(y))
#define _UI_BITBYTES(bits)      ((bits + 7)/8)
#define _UI_ABS(x)              ((x)>=0? (x):-(x))

typedef struct gui_vertex
{
    float x;                    //!< x axis
    float y;                    //!< y axis
    float z;                    //!< z axis
} gui_vertex_t;

typedef struct gui_point2f
{
    float x;
    float y;
} gui_point2f_t;
typedef struct gui_matrix
{
    float m[3][3];
} gui_matrix_t;

typedef struct gui_point3f
{
    float p[3];
} gui_point3f_t;

typedef struct gui_rect
{
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
} gui_rect_t;

#ifdef __cplusplus
}
#endif

#endif

