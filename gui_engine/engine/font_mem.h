/*
 * File      : font_mem.h
 */
#ifndef __FONT_MEM_H__
#define __FONT_MEM_H__

#include "guidef.h"
#include "gui_api.h"
#include "draw_font.h"
#include "gui_text.h"
#ifdef __cplusplus
extern "C" {
#endif

#define FONT_MALLOC_PSRAM(x)  gui_malloc(x)
#define FONT_FREE_PSRAM(x)  gui_free(x)

#define FONT_FILE_FLAG 0x01

typedef struct
{
    uint8_t char_y;
    uint8_t baseline;
    uint8_t char_w;
    uint8_t char_h;
} GUI_CHAR_HEAD;

typedef struct
{
    uint32_t unicode;
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    uint8_t char_y;
    uint8_t char_w;
    uint8_t char_h;
    uint8_t *dot_addr;
    gui_img_t *emoji_img;
} mem_char_t;

typedef struct
{
    uint8_t *font_file;
    uint8_t font_size;
    FONT_SRC_MODE type;
    uint8_t *data;
} MEM_FONT_LIB;

#pragma pack(1)
typedef struct
{
    uint8_t head_length;               /*font file head length*/
    uint8_t file_type;                 /*0x1-FONT_FILE_FLAG is font file*/
    uint8_t version[4];                /*version*/
    uint8_t font_size;                 /*font size*/
    uint8_t rendor_mode;               /*support 1/2/4/8*/
    union
    {
        uint8_t value;
        struct
        {
            uint8_t bold : 1;          /*bold*/
            uint8_t italic : 1;        /*italic*/
            uint8_t scan_mode : 1;     /*scan_mode*/
            uint8_t index_method : 1;  /*0 offset ; 1 address*/
            uint8_t crop : 1;          /*enable crop can crop font file*/
            uint8_t rsvd : 3;          /*rsvd*/
        } detail;
    } font_mode_detail;                /*detail information*/
    uint32_t index_area_size;          /*length of index area*/
    uint8_t font_name_length;          /*length of font name*/
    // uint8_t* font_name;
} GUI_FONT_HEAD;
#pragma pack()

/**
 * @brief Initialize the character binary file and store the font and
 * corresponding information in the font list
 *
 * @param font_bin_addr the binary file address of this font type
 */
uint8_t gui_font_mem_init(uint8_t *font_bin_addr);

/**
 * @brief Initialize the character binary file and store the font and
 * corresponding information in the font list
 *
 * @param font_bin_addr font file address
 * @return uint8_t
 */
uint8_t gui_font_mem_init_ftl(uint8_t *font_bin_addr);

/**
 * @brief Initialize the character binary file and store the font and
 * corresponding information in the font list
 *
 * @param font_bin_addr font file address
 * @return uint8_t
 */
uint8_t gui_font_mem_init_fs(uint8_t *font_bin_addr);

/**
 * @brief Initialize the character binary file and store the font and
 * corresponding information in the font list
 *
 * @param font_bin_addr font file address
 * @return uint8_t
 */
uint8_t gui_font_mem_init_mem(uint8_t *font_bin_addr);

/**
 * @brief Destroy this flot type in font list
 *
 * @param font_bin_addr font file address
 * @return uint8_t
 */
uint8_t gui_font_mem_destroy(uint8_t *font_bin_addr);

/**
 * @brief Preprocessing of bitmap fonts using internal engines
 *
 * @param text Widget pointer
 * @param rect Widget boundary
*/
void gui_font_mem_load(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Drawing of bitmap fonts using internal engine
 *
 * @param text Widget pointer
 * @param rect Widget boundary
 */
void gui_font_mem_draw(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief Post-processing work for drawing bitmap fonts using internal engines
 *
 * @param text Widget pointer
 */
void gui_font_mem_unload(gui_text_t *text);

/**
 * @brief GUI_FONT_SRC_BMP text widget destroy function.
 *
 * @param text Widget pointer
 */
void gui_font_mem_obj_destroy(gui_text_t *text);

/**
 * @brief Get the pixel width of the text in the current font file.
 *
 * @param content text pointer
 * @param font_bin_addr font file address
 * @param charset text encoding format
 * @return uint32_t
 */
uint32_t gui_get_mem_char_width(void *content, void *font_bin_addr, TEXT_CHARSET charset);

/**
 * @brief Get the pixel width of the utf-8 text in the current font file.
 *
 * @param content text pointer
 * @param font_bin_addr font file address
 * @return uint32_t
 */
uint32_t gui_get_mem_utf8_char_width(void *content, void *font_bin_addr);

/**
 * @brief Get the fontlib name object
 *
 * @param font_size font size
 * @return uint8_t font lib index
 */
uint8_t get_fontlib_by_size(uint8_t font_size);

/**
 * @brief Get the fontlib name object
 *
 * @param font_file font file
 * @return uint8_t font lib index
 */
uint8_t get_fontlib_by_name(uint8_t *font_file);

/**
 * @brief text layout by mode
 *
 * @param text Widget pointer
 * @param rect Widget boundary
 */
void gui_font_mem_layout(gui_text_t *text, gui_text_rect_t *rect);

/**
 * @brief get dot info by utf-8 or utf-16
 *
 * @param text Widget pointer
 */
void gui_font_get_dot_info(gui_text_t *text);

#ifdef __cplusplus
}
#endif

#endif

