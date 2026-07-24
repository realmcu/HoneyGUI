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

/*============================================================================*
 *                            Variables
 *============================================================================*/
static char scale_small[] = "20 " EMOJI_JOY;
static char scale_normal[] = "32 " EMOJI_HEART_EYES;
static char scale_large[] = "48 " EMOJI_ROFL;
static char align_left[] = "LEFT " EMOJI_JOY;
static char align_center[] = "CENTER " EMOJI_HEART_EYES;
static char align_right[] = "RIGHT " EMOJI_ROFL;
static char wrap_text[] = "Wrap " EMOJI_JOY " inline emoji with words " EMOJI_HEART_EYES
                          " and a ZWJ sequence " EMOJI_RAINBOW_FLAG;
static char scroll_x_text[] = "MARQUEE " EMOJI_JOY EMOJI_HEART_EYES EMOJI_ROFL
                              " inline color emoji " EMOJI_RAINBOW_FLAG;
static char scroll_y_text[] = "Vertical " EMOJI_JOY " scroll\nMixed text " EMOJI_HEART_EYES
                              "\nZWJ flag " EMOJI_RAINBOW_FLAG;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static gui_text_t *emoji_text_create(const char *name, char *content,
                                     int16_t x, int16_t y, int16_t w, int16_t h,
                                     uint16_t font_size, TEXT_MODE mode, gui_color_t color)
{
    gui_text_t *text = gui_text_create(gui_obj_get_root(), name, x, y, w, h);
    gui_text_set(text, content, GUI_FONT_SRC_BMP, color, strlen(content), font_size);
    gui_text_type_set(text, font32b2, FONT_SRC_MEMADDR);
    gui_text_emoji_set(text, EMOJI_PATH, EMOJI_SOURCE_SIZE);
    gui_text_mode_set(text, mode);
    return text;
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
    emoji_text_create("emoji_20", scale_small, 20, 4, 120, 52,
                      20, MID_LEFT, APP_COLOR_WHITE);
    emoji_text_create("emoji_32", scale_normal, 150, 4, 130, 52,
                      32, MID_LEFT, APP_COLOR_WHITE);
    emoji_text_create("emoji_48", scale_large, 290, 4, 150, 52,
                      48, MID_LEFT, APP_COLOR_WHITE);

    emoji_text_create("emoji_left", align_left, 20, 62, DEMO_WIDTH, 34,
                      32, MID_LEFT, APP_COLOR_WHITE);
    emoji_text_create("emoji_center", align_center, 20, 98, DEMO_WIDTH, 34,
                      32, MID_CENTER, APP_COLOR_GREEN);
    emoji_text_create("emoji_right", align_right, 20, 134, DEMO_WIDTH, 34,
                      32, MID_RIGHT, APP_COLOR_YELLOW);

    gui_text_t *wrapped = emoji_text_create("emoji_wrap", wrap_text, 20, 174,
                                            DEMO_WIDTH, 112, 32, MULTI_LEFT,
                                            APP_COLOR_WHITE);
    gui_text_wordwrap_set(wrapped, true);
    gui_text_extra_line_spacing_set(wrapped, 4);

    gui_scroll_text_t *scroll_x = emoji_scroll_text_create("emoji_scroll_x", scroll_x_text,
                                                           294, 38, SCROLL_X);
    gui_scroll_text_scroll_pause_set(scroll_x, 1000);

    gui_scroll_text_t *scroll_y = emoji_scroll_text_create("emoji_scroll_y", scroll_y_text,
                                                           342, 64, SCROLL_Y);
    gui_text_wordwrap_set(&scroll_y->base, true);
    gui_scroll_text_loop_set(scroll_y, true, 0);
}
/* gui text emoji example end */
