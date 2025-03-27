/**
* @file lv_custom_matrix.h
*
*/

#ifndef LV_CUSTOM_MATRIX_H
#define LV_CUSTOM_MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

#if LV_USE_MATRIX
#include "lv_matrix.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct lv_vertex
{
    float x;                    //!< x axis
    float y;                    //!< y axis
    float z;                    //!< z axis
} lv_vertex_t;

typedef struct lv_point2f
{
    float x;
    float y;
} lv_point2f_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Compute rotation matrix from Euler angles
 * @param degree_x Rotation angle around X-axis (degrees)
 * @param degree_y Rotation angle around Y-axis (degrees)
 * @param degree_z Rotation angle around Z-axis (degrees)
 * @param rotate    Pointer to store resultant rotation matrix
 */
void lv_matrix_compute_rotate(float degree_x, float degree_y, float degree_z, lv_matrix_t *rotate);

/**
 * Transform vertex coordinates using rotation matrix and translation
 * @param rotate    Precomputed rotation matrix
 * @param vertex    Input vertex coordinates
 * @param rc        Output transformed coordinates
 * @param tx        X-axis translation offset
 * @param ty        Y-axis translation offset
 * @param tz        Z-axis translation offset
 */
void lv_matrix_transfrom_rotate(lv_matrix_t *rotate, lv_vertex_t *vertex, lv_vertex_t *rc, float tx,
                                float ty, float tz);

/**
 * Calculate perspective transform matrix for quadrilateral mapping
 * @param w     Source rectangle width
 * @param h     Source rectangle height
 * @param p     Perspective reference point
 * @param v0    Target quadrilateral vertex 0
 * @param v1    Target quadrilateral vertex 1
 * @param v2    Target quadrilateral vertex 2
 * @param v3    Target quadrilateral vertex 3
 * @param matrix Output 3x3 transformation matrix
 */
void lv_matrix_transfrom_blit(float w, float h, lv_vertex_t *p,
                              lv_vertex_t *v0, lv_vertex_t *v1,
                              lv_vertex_t *v2, lv_vertex_t *v3,
                              lv_matrix_t *matrix);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_MATRIX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CUSTOM_MATRIX_H*/
