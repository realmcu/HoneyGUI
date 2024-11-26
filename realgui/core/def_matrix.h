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
#ifndef __DEF_MATRIX_H__
#define __DEF_MATRIX_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

#ifdef __cplusplus
}
#endif

#endif

