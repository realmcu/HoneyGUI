/*
 * File      : gui_win.h
 */
#ifndef __GUI_MATRIX_H__
#define __GUI_MATRIX_H__

#include <guidef.h>

#ifdef __cplusplus
extern "C" {
#endif


void multiply(struct gui_matrix *matrix, struct gui_matrix *mult);
void matrix_identity(struct gui_matrix *matrix);
void matrix_translate(float x, float y, struct gui_matrix *matrix);
void matrix_rotate(float degrees, struct gui_matrix *matrix);
void matrix_inverse(struct gui_matrix *matrix);
void matrix_scale(float scale_x, float scale_y, struct gui_matrix *matrix);
void matrix_perspective(float px, float py, struct gui_matrix *matrix);
void pox_mul(struct gui_matrix *matrix, struct gui_pox *pox);


#ifdef __cplusplus
}
#endif

#endif

