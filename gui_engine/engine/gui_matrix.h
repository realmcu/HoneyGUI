/*
 * File      : gui_win.h
 */
#ifndef __GUI_MATRIX_H__
#define __GUI_MATRIX_H__

#include <guidef.h>
#include "def_matrix.h"

#ifdef __cplusplus
extern "C" {
#endif



void matrix_transfrom_blit(float w, float h, gui_vertex_t *p, gui_vertex_t *v0, gui_vertex_t *v1,
                           gui_vertex_t *v2,
                           gui_vertex_t *v3, struct gui_matrix *matrix);
void matrix_compute_rotate(float rx, float ry, float rz, struct gui_matrix *rotate);
void matrix_transfrom_rotate(struct gui_matrix *rotate, gui_vertex_t *vertex, gui_vertex_t *rc,
                             float tx,
                             float ty, float tz);

void matrix_multiply(struct gui_matrix *matrix, struct gui_matrix *mult);
void matrix_identity(struct gui_matrix *matrix);
bool matrix_is_identity(struct gui_matrix *matrix);
void matrix_translate(float x, float y, struct gui_matrix *matrix);
void matrix_rotate(float degrees, struct gui_matrix *matrix);
void matrix_inverse(struct gui_matrix *matrix);
void matrix_scale(float scale_x, float scale_y, struct gui_matrix *matrix);
void matrix_perspective(float px, float py, struct gui_matrix *matrix);
void matrix_multiply_point(struct gui_matrix *matrix, struct gui_point3f *pox);
void matrix_multiply_normal(struct gui_matrix *matrix, gui_vertex_t *normal);

float fix_cos(int angle);
float fix_sin(int angle);


#ifdef __cplusplus
}
#endif

#endif

