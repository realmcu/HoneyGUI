/*
 * File      : font.h
 * This file is part of RT-Thread GUI Engine
 * COPYRIGHT (C) 2006 - 2017, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-10-16     Bernard      first version
 */
#ifndef __FONT_H__
#define __FONT_H__

#include <guidef.h>
#include <gui_api.h>
#include <gui_text.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rtgui_font_engine rtgui_font_engine_t;

struct rtgui_font_engine
{
    const char *name;
    gui_list_t list;
    void (*font_load)(gui_text_t *text);
    void (*font_unload)(gui_text_t *text);
    void (*font_draw)(gui_text_t *text, struct rtgui_rect *rect);
};

struct font_lib
{
    uint8_t *font_file;
    uint8_t *dot_name;
    uint8_t *table_name;
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
};
typedef struct font_stb_screen
{
    int width;
    int height;
    int offset;
    uint8_t *buf;
} FONT_STB_SCREEN;

/* init rtgui text system */
void rtgui_system_font_init(void);
void rtgui_font_register_engine(struct rtgui_font_engine *engine);
static struct rtgui_font_engine *rtgui_font_get_engine_by_type(char *string);

void rtgui_text_create(gui_text_t *text);
void rtgui_text_destroy(gui_text_t *text);
/* draw a text */
void rtgui_font_draw(gui_text_t *text, struct rtgui_rect *rect);

uint16_t utf8_to_unicode(uint8_t *utf8, uint16_t len, uint16_t *unicode_array,
                         uint16_t unicode_buf_len);
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
#if RTK_GUI_FONT_STB
/**
 * @brief Initialize a standard ttf file and send the font and
 * corresponding information to the parser
 *
 * @param font_ttf_addr the address of ttf file
 */
void gui_font_stb_init(void *font_ttf_addr);
#endif
#ifdef __cplusplus
}
#endif

#endif

