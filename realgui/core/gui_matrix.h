/*
 * File      : gui_matrix.h
 */
#ifndef __GUI_MATRIX_H__
#define __GUI_MATRIX_H__

#include "guidef.h"
#include "def_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Utility macros */
#define MATRIX_MAX(a, b) ((a) > (b) ? (a) : (b))
#define MATRIX_MIN(a, b) ((a) < (b) ? (a) : (b))

/* Matrix basic operations */
void matrix_identity(struct gui_matrix *matrix);
void matrix_multiply(struct gui_matrix *matrix, struct gui_matrix *mult);
void matrix_inverse(struct gui_matrix *matrix);

/* Matrix state check */
bool matrix_is_identity(struct gui_matrix *matrix);
bool matrix_only_translate(struct gui_matrix *matrix);
bool matrix_is_full_rank(struct gui_matrix *m);

/* Matrix transformations */
void matrix_translate(float x, float y, struct gui_matrix *matrix);
void matrix_rotate(float degrees, struct gui_matrix *matrix);
void matrix_scale(float scale_x, float scale_y, struct gui_matrix *matrix);

/* Matrix point/vector operations */
void matrix_multiply_point(struct gui_matrix *matrix, struct gui_point3f *pox);
void matrix_multiply_normal(struct gui_matrix *matrix, gui_vertex_t *normal);

/* 3D rotation operations */
void matrix_compute_rotate(float rx, float ry, float rz, gui_matrix_t *rotate);
void matrix_transfrom_rotate(gui_matrix_t *rotate, gui_vertex_t *vertex, gui_vertex_t *rc,
                             float tx, float ty, float tz);

/* Perspective transform */
void matrix_transfrom_blit(float w, float h, gui_vertex_t *p,
                           gui_vertex_t *v0, gui_vertex_t *v1,
                           gui_vertex_t *v2, gui_vertex_t *v3,
                           gui_matrix_t *matrix);

/* Area operations */
bool matrix_get_transform_area(gui_matrix_t *matrix, gui_rect_t *input_area,
                               gui_rect_t *result_area);

/* Rectangle operations */
bool rect_intersect(gui_rect_t *result_rect, gui_rect_t *rect1, gui_rect_t *rect2);
void rect_move(gui_rect_t *out, gui_rect_t *in, int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif

