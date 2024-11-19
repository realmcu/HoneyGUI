/*
 * File      : font_freetype.h
 */
#ifndef __FONT_FREETYPE_H__
#define __FONT_FREETYPE_H__

#include <guidef.h>
#include <gui_api.h>
#include <draw_font.h>
#include "gui_text.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t *font_ttf_file;
    uint32_t font_file_size;
} FT_FONT_LIB;


/**
 * @brief Preprocessing of ttf font drawing with ft
 *
 * @param text Widget pointer
 */
void gui_font_ft_load(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Drawing of ttf fonts using ft
 *
 * @param text Widget pointer
 * @param rect Widget boundary
 */
void gui_font_ft_draw(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Post-processing work for drawing ttf fonts using ft
 *
 * @param text Widget pointer
 */
void gui_font_ft_unload(gui_text_t *text);

/**
 * @brief Add a TTF file to the FreeType font list
 *
 * @param font_ttf_addr file address pointer
 * @param font_file_size font file size
 */
void gui_font_ft_mem_init(void *font_ttf_addr, uint32_t font_file_size);


#ifdef __cplusplus
}
#endif

#endif

