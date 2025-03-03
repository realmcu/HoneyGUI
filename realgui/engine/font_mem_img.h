/*
 * File      : font_mem.h
 */
#ifndef __FONT_MEM_IMG_H__
#define __FONT_MEM_IMG_H__

#include "guidef.h"
#include "gui_api.h"
#include "draw_font.h"
#include "gui_text.h"
#include "font_mem.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Post-processing work for drawing bitmap fonts using internal engines
 *
 * @param text Widget pointer
 */
void gui_font_scale_destroy(gui_text_t *text);

/**
 * @brief transform bmp text to img
 *
 * @param text text pointer
 * @param font_img_type img type
 * @param img_x img x
 * @param img_y img y
 * @return void* text img buffer
 */
void *gui_text_bmp2img(gui_text_t *text, GUI_FormatType font_img_type, int16_t *img_x,
                       int16_t *img_y);


#ifdef __cplusplus
}
#endif

#endif

