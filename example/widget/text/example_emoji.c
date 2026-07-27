/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "test_font.h"

/*============================================================================*
 *                           Constants
 *============================================================================*/
#define EMOJI_PATH             ((uint8_t *)"/emoji/emoji_u")
#define EMOJI_SOURCE_SIZE      32
#define DEMO_WIDTH             440

#define EMOJI_JOY              "\xF0\x9F\x98\x82"
#define EMOJI_HEART_EYES       "\xF0\x9F\x98\x8D"
#define EMOJI_ROFL             "\xF0\x9F\xA4\xA3"
#define EMOJI_RAINBOW_FLAG     "\xF0\x9F\x8F\xB3\xE2\x80\x8D\xF0\x9F\x8C\x88"
/* BMP presentation cases from Unicode emoji-data.txt. */
#define SYMBOL_HEART_TEXT      "\xE2\x9D\xA4"              /* U+2764 */
#define SYMBOL_HEART_EMOJI     "\xE2\x9D\xA4\xEF\xB8\x8F" /* U+2764 U+FE0F */
#define EMOJI_STAR             "\xE2\xAD\x90"              /* U+2B50, emoji default */
#define EMOJI_SPARKLES         "\xE2\x9C\xA8"              /* U+2728, emoji default */
#define SYMBOL_SPARKLES_TEXT   "\xE2\x9C\xA8\xEF\xB8\x8E" /* U+2728 U+FE0E */

/*============================================================================*
 *                            Variables
 *============================================================================*/
static char scale_small[] = "20 " EMOJI_JOY SYMBOL_HEART_EMOJI;
static char scale_normal[] = "32 " EMOJI_HEART_EYES EMOJI_STAR;
static char scale_large[] = "48 " EMOJI_ROFL EMOJI_SPARKLES;
static char presentation_text[] = "Text " SYMBOL_HEART_TEXT " " SYMBOL_SPARKLES_TEXT;
static char presentation_emoji[] = "Emoji " SYMBOL_HEART_EMOJI " " EMOJI_SPARKLES;
static char align_left[] = "LEFT " EMOJI_JOY;
static char align_center[] = "CENTER " EMOJI_HEART_EYES;
static char align_right[] = "RIGHT " EMOJI_ROFL;
static char wrap_text[] = "Wrap " EMOJI_JOY " inline emoji with words " EMOJI_HEART_EYES
                          " and a ZWJ sequence " EMOJI_RAINBOW_FLAG;
static char scroll_x_text[] = "MARQUEE " EMOJI_JOY EMOJI_HEART_EYES EMOJI_ROFL
                              " inline color emoji " EMOJI_RAINBOW_FLAG;
static char scroll_y_text[] = "Vertical " EMOJI_JOY " scroll\nMixed text " EMOJI_HEART_EYES
                              "\nZWJ flag " EMOJI_RAINBOW_FLAG;

static char ttf_mixed_text[] = "TTF A" EMOJI_JOY "B " EMOJI_HEART_EYES " vector";
static char ttf_presentation_color[] = "Color " SYMBOL_HEART_EMOJI " " EMOJI_SPARKLES;
static char ttf_zwj_text[] = "ZWJ " EMOJI_RAINBOW_FLAG " inline";
static char ttf_wrap_text[] = "Vector text " EMOJI_JOY " wraps with color emoji " EMOJI_ROFL
                              " and keeps the baseline aligned.";

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static gui_text_t *emoji_text_create(const char *name, char *content,
                                     int16_t x, int16_t y, int16_t w, int16_t h,
                                     uint16_t font_size, TEXT_MODE mode, gui_color_t color,
                                     FONT_SRC_TYPE font_type, void *font)
{
    gui_text_t *text = gui_text_create(gui_obj_get_root(), name, x, y, w, h);
    gui_text_set(text, content, font_type, color, strlen(content), font_size);
    gui_text_type_set(text, font, FONT_SRC_MEMADDR);
    gui_text_emoji_set(text, EMOJI_PATH, EMOJI_SOURCE_SIZE);
    gui_text_mode_set(text, mode);
    return text;
}

static gui_text_t *bmp_emoji_text_create(const char *name, char *content,
                                         int16_t x, int16_t y, int16_t w, int16_t h,
                                         uint16_t font_size, TEXT_MODE mode, gui_color_t color)
{
    return emoji_text_create(name, content, x, y, w, h, font_size, mode, color,
                             GUI_FONT_SRC_BMP, font32b2);
}

static gui_text_t *ttf_emoji_text_create(const char *name, char *content,
                                         int16_t y, int16_t h, uint16_t font_size,
                                         TEXT_MODE mode, gui_color_t color)
{
    return emoji_text_create(name, content, 20, y, DEMO_WIDTH, h, font_size, mode, color,
                             GUI_FONT_SRC_TTF, font32vb4);
}

static gui_scroll_text_t *emoji_scroll_text_create(const char *name, char *content,
                                                   int16_t y, int16_t h, TEXT_MODE mode)
{
    gui_scroll_text_t *text = gui_scroll_text_create(gui_obj_get_root(), name, 20, y,
                                                     DEMO_WIDTH, h);
    gui_scroll_text_set(text, content, GUI_FONT_SRC_BMP, APP_COLOR_CYAN,
                        strlen(content), 32);
    gui_scroll_text_type_set(text, font32b2, FONT_SRC_MEMADDR);
    gui_text_emoji_set(&text->base, EMOJI_PATH, EMOJI_SOURCE_SIZE);
    gui_scroll_text_scroll_set(text, mode, 0, 0, 8000, 0);
    gui_scroll_text_loop_set(text, true, 32);
    return text;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Render inline color emoji with scaling, layout, wrapping and scrolling
 *
 * Emoji bitmaps are loaded from the ROMFS pack mounted under "/emoji". The
 * cases cover three output sizes, horizontal alignment, word wrapping, a ZWJ
 * sequence, and horizontal and vertical scrolling.
 */
/* gui text emoji example start */
void text_emoji_example(void)
{
    bmp_emoji_text_create("emoji_20", scale_small, 20, 4, 120, 52,
                          20, MID_LEFT, APP_COLOR_WHITE);
    bmp_emoji_text_create("emoji_32", scale_normal, 150, 4, 130, 52,
                          32, MID_LEFT, APP_COLOR_WHITE);
    bmp_emoji_text_create("emoji_48", scale_large, 290, 4, 150, 52,
                          48, MID_LEFT, APP_COLOR_WHITE);

    bmp_emoji_text_create("emoji_left", align_left, 20, 62, DEMO_WIDTH, 34,
                          32, MID_LEFT, APP_COLOR_WHITE);
    bmp_emoji_text_create("emoji_center", align_center, 20, 98, DEMO_WIDTH, 34,
                          32, MID_CENTER, APP_COLOR_GREEN);
    bmp_emoji_text_create("emoji_right", align_right, 20, 134, DEMO_WIDTH, 34,
                          32, MID_RIGHT, APP_COLOR_YELLOW);

    bmp_emoji_text_create("emoji_presentation_text", presentation_text, 20, 170,
                          DEMO_WIDTH, 34, 32, MID_LEFT, APP_COLOR_WHITE);
    bmp_emoji_text_create("emoji_presentation_color", presentation_emoji, 20, 206,
                          DEMO_WIDTH, 34, 32, MID_LEFT, APP_COLOR_WHITE);

    gui_text_t *wrapped = bmp_emoji_text_create("emoji_wrap", wrap_text, 20, 246,
                                                DEMO_WIDTH, 76, 32, MULTI_LEFT,
                                                APP_COLOR_WHITE);
    gui_text_wordwrap_set(wrapped, true);
    gui_text_extra_line_spacing_set(wrapped, 4);

    gui_scroll_text_t *scroll_x = emoji_scroll_text_create("emoji_scroll_x", scroll_x_text,
                                                           326, 38, SCROLL_X);
    gui_scroll_text_scroll_pause_set(scroll_x, 1000);

    gui_scroll_text_t *scroll_y = emoji_scroll_text_create("emoji_scroll_y", scroll_y_text,
                                                           368, 38, SCROLL_Y);
    gui_text_wordwrap_set(&scroll_y->base, true);
    gui_scroll_text_loop_set(scroll_y, true, 0);
}
/* gui text emoji example end */

/**
 * @brief Render color emoji inline with vector font glyphs
 */
void text_ttf_emoji_example(void)
{
    ttf_emoji_text_create("ttf_emoji_mixed", ttf_mixed_text, 20, 60, 40,
                          MID_LEFT, APP_COLOR_WHITE);
    ttf_emoji_text_create("ttf_emoji_text", presentation_text, 84, 50, 32,
                          MID_LEFT, APP_COLOR_CYAN);
    ttf_emoji_text_create("ttf_emoji_color", ttf_presentation_color, 138, 50, 32,
                          MID_LEFT, APP_COLOR_GREEN);
    ttf_emoji_text_create("ttf_emoji_zwj", ttf_zwj_text, 192, 54, 36,
                          MID_LEFT, APP_COLOR_YELLOW);

    gui_text_t *wrapped = ttf_emoji_text_create("ttf_emoji_wrap", ttf_wrap_text,
                                                252, 150, 32, MULTI_LEFT,
                                                APP_COLOR_WHITE);
    gui_text_wordwrap_set(wrapped, true);
    gui_text_extra_line_spacing_set(wrapped, 6);
}
