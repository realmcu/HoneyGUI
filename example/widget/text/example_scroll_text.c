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
 * @brief Create and configure a horizontal and a vertical scroll text widget example
 */
/* gui scroll text widget example start*/
void scroll_text_widget_example(void)
{
    char *test_text =
        "Everyone has the right to freedom of thought, conscience and religion; this right includes freedom to change his religion or belief, and freedom, either alone or in community with others and in public or private, to manifest his religion or belief in teaching, practice, worship and observance. ";

    gui_scroll_text_t *scroll_text_x = gui_scroll_text_create(gui_obj_get_root(), "scroll_text", 0, 200,
                                                              300, 200);
    gui_scroll_text_set(scroll_text_x, test_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(test_text),
                        32);
    gui_scroll_text_scroll_set(scroll_text_x, SCROLL_X, 300, 100, 40000, 0);
    gui_scroll_text_type_set(scroll_text_x, font32b2, FONT_SRC_MEMADDR);

    gui_scroll_text_t *scroll_text_y = gui_scroll_text_create(gui_obj_get_root(), "scroll_text", 50,
                                                              250, 300, 100);
    gui_scroll_text_set(scroll_text_y, test_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(test_text),
                        32);
    gui_scroll_text_scroll_set(scroll_text_y, SCROLL_Y, 100, 100, 10000, 0);
    gui_scroll_text_type_set(scroll_text_y, font32b2, FONT_SRC_MEMADDR);
    gui_text_wordwrap_set(&scroll_text_y->base, true);
}
/* gui scroll text widget example end*/
