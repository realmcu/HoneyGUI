/*
 * File      : font_mem.h
 */
#ifndef __FONT_MEM_MATRIX_H__
#define __FONT_MEM_MATRIX_H__

#include "guidef.h"
#include "gui_api.h"
#include "draw_font.h"
#include "gui_text.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Preprocessing of bitmap fonts using internal engines
 *
 * @param text Widget pointer
 * @param rect Widget boundary
 */
void gui_font_mat_load(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Drawing of bitmap fonts with matric using internal engine
 *
 * @param text Widget pointer
 * @param rect Widget boundary
 */
void gui_font_mat_draw(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @briefs Post-processing work for drawing bitmap fonts using internal engines
 *
 * @param text Widget pointer
 */
void gui_font_mat_unload(gui_text_t *text);

/**
 * @brief Destroy font matrix resources
 *
 * @param text Text widget pointer
 */
void gui_font_mat_destroy(gui_text_t *text);

#ifdef __cplusplus
}
#endif

#endif

