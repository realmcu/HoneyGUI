/*
 * File      : gui_win.h
 */
#ifndef __GUI_MATRIX_H__
#define __GUI_MATRIX_H__

#include <guidef.h>

#ifdef __cplusplus
extern "C" {
#endif


void multiply(struct rtgui_matrix *matrix, struct rtgui_matrix *mult);
void matrix_identity(struct rtgui_matrix *matrix);
void matrix_translate(float x, float y, struct rtgui_matrix *matrix);
void matrix_rotate(float degrees, struct rtgui_matrix *matrix);
void matrix_inverse(struct rtgui_matrix *matrix);
void matrix_scale(float scale_x, float scale_y, struct rtgui_matrix *matrix);
void matrix_perspective(float px, float py, struct rtgui_matrix *matrix);
void pox_mul(struct rtgui_matrix *matrix, struct rtgui_pox *pox);


#ifdef __cplusplus
}
#endif

#endif

