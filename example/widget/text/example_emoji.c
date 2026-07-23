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
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Render text mixed with color emoji
 *
 * Uses ASCII UTF-8 escapes for three emoji and one ZWJ sequence. Emoji
 * bitmaps are loaded from the ROMFS pack mounted under "/emoji".
 */
/* gui text emoji example start */
void text_emoji_example(void)
{
    char *text_string = "Hi \xF0\x9F\x98\x82\xF0\x9F\x98\x8D\xF0\x9F\xA4\xA3"
                        "\xF0\x9F\x8F\xB3\xE2\x80\x8D\xF0\x9F\x8C\x88";
    gui_text_t *text = gui_text_create(gui_obj_get_root(), "text_emoji", 20, 200, 440, 80);
    gui_text_set(text, text_string, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(text_string), 32);
    gui_text_type_set(text, font32b2, FONT_SRC_MEMADDR);
    gui_text_emoji_set(text, (uint8_t *)"/emoji/emoji_u", 32);
}
/* gui text emoji example end */
