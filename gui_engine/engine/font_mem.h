/*
 * File      : font_mem.h
 */
#ifndef __FONT_MEM_H__
#define __FONT_MEM_H__

#include "guidef.h"
#include "gui_api.h"
#include "draw_font.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint16_t unicode;
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    int16_t char_w;
    uint8_t *dot_addr;
} mem_char_t;

typedef struct
{
    uint8_t *font_lib_name;
    uint8_t *font_lib_tab_name;
    uint8_t rendor_mode : 4;
    uint8_t index_method : 1;
} fontlib_name_t;

typedef struct
{
    uint8_t *font_file;
    uint8_t *bmp_addr;
    uint8_t *table_addr;
    uint8_t font_size;
    uint8_t rendor_mode;  // support 1 4 8
    union
    {
        uint8_t value;
        struct
        {
            uint8_t bold : 1;
            uint8_t italic : 1;
            uint8_t scanMode : 1;
            uint8_t index_method : 1;   // 0 offset ; 1 address
            uint8_t reserve : 4;
        } detail;
    } font_mode_detail;
} MEM_FONT_LIB;

/**
 * @brief Initialize the character binary file and store the font and
 * corresponding information in the font list
 *
 * @param font_size the size of this font file
 * @param font_bitmap_addr the bitmap file address of this font type
 * @param font_table_addr the table file address of this font type
 */
void gui_set_font_mem_resourse(unsigned char font_size, void *font_bitmap_addr,
                               void *font_table_addr);
/**
 * @brief Initialize the character binary file and store the font and
 * corresponding information in the font list
 *
 * @param font_bin_addr the binary file address of this font type
 */
void gui_font_mem_init(uint8_t *font_bin_addr);

/**
 * @brief Preprocessing of bitmap fonts using internal engines
 *
 * @param text Widget pointer
 */
void gui_font_mem_load(gui_text_t *text);

/**
 * @brief Drawing of bitmap fonts using internal engine
 *
 * @param text Widget pointer
 * @param rect Widget boundary
 */
void gui_font_mem_draw(gui_text_t *text, gui_rect_t *rect);

/**
 * @brief Post-processing work for drawing bitmap fonts using internal engines
 *
 * @param text Widget pointer
 */
void gui_font_mem_unload(gui_text_t *text);

#ifdef __cplusplus
}
#endif

#endif

