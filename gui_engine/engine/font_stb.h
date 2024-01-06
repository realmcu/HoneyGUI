/*
 * File      : font_stb.h
 */
#ifndef __FONT_STB_H__
#define __FONT_STB_H__

#include <guidef.h>
#include <gui_api.h>
#include <draw_font.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct font_stb_screen
{
    int width;
    int height;
    int offset;
    uint8_t *buf;
} FONT_STB_SCREEN;

/**
 * @brief Initialize a standard ttf file and send the font and
 *        corresponding information to the parser
 *
 * @param font_ttf_addr the address of ttf file
 */
void gui_font_stb_init(void *font_ttf_addr);

/**
 * @brief Preprocessing of ttf font drawing with stb
 *
 * @param text Widget pointer
 */
void gui_font_stb_load(gui_text_t *text);

/**
 * @brief Drawing of ttf fonts using stb
 *
 * @param text Widget pointer
 * @param rect Widget boundary
 */
void gui_font_stb_draw(gui_text_t *text, gui_rect_t *rect);

/**
 * @brief Post-processing work for drawing ttf fonts using stb
 *
 * @param text Widget pointer
 */
void gui_font_stb_unload(gui_text_t *text);


#ifdef __cplusplus
}
#endif

#endif

