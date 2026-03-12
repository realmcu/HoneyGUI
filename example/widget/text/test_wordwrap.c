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
 * @brief Test word-wrap and word-break protection
 *
 * Test cases:
 * 1. Normal text with spaces - standard word-wrap
 * 2. Single oversized word (no spaces) - forced char-level break
 * 3. Mixed: normal words + one oversized word
 * 4. Oversized word at the beginning then normal text
 * 5. Small words, multiple spaces, newlines, right-aligned
 *
 * Easter egg - where do these words come from?
 *   Case 1: Article 18, Universal Declaration of Human Rights (1948)
 *   Case 2: "bababadalgharaghtakamminarronnkonnbronn..." - the thunderword
 *           from James Joyce's "Finnegans Wake" (1939), 100 letters
 *           representing the sound of thunder
 *   Case 3: "Pneumonoultramicroscopicsilicovolcanoconiosis" - 45 letters,
 *           the longest word in major English dictionaries, a lung disease
 *           caused by inhaling fine silica dust
 *   Case 4: "Floccinaucinihilipilification" - 29 letters, from Latin
 *           flocci + nauci + nihili + pili + -fication, meaning
 *           "the act of estimating something as worthless"
 *   Case 5: William Shakespeare, "Hamlet" Act 3 Scene 1 (c. 1600)
 */
void text_wordwrap_test(void)
{
    int16_t y = 0;
    int16_t font_height = 32;
    int16_t gap = font_height;

    /* Case 1: Normal word-wrap with spaces */
    char *text_normal =
        "Everyone has the right to freedom of thought, conscience and religion.";
    gui_text_t *t1 = gui_text_create(gui_obj_get_root(), "wrap_normal", 0, y, 0, 0);
    gui_text_set(t1, text_normal, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(text_normal), font_height);
    gui_text_mode_set(t1, MULTI_LEFT);
    gui_text_wordwrap_set(t1, true);
    y += font_height * 2 + gap;

    /* Case 2: Single oversized word - no spaces at all */
    char *text_long =
        "bababadalgharaghtakamminarronnkonnbronntonnerronntuonnthunntrovarrhounawnskawntoohoohoordenenthurnuk";
    gui_text_t *t2 = gui_text_create(gui_obj_get_root(), "wrap_longword", 0, y, 0, 0);
    gui_text_set(t2, text_long, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(text_long), font_height);
    gui_text_mode_set(t2, MULTI_LEFT);
    gui_text_wordwrap_set(t2, true);
    y += font_height * 3 + gap;

    /* Case 3: Mixed - normal words then one oversized word */
    char *text_mixed =
        "Diagnosis: Pneumonoultramicroscopicsilicovolcanoconiosis requires immediate treatment.";
    gui_text_t *t3 = gui_text_create(gui_obj_get_root(), "wrap_mixed", 0, y, 0, 0);
    gui_text_set(t3, text_mixed, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(text_mixed), font_height);
    gui_text_mode_set(t3, MULTI_LEFT);
    gui_text_wordwrap_set(t3, true);
    y += font_height * 3 + gap;

    /* Case 4: Scroll Y with oversized word, half screen width */
    char *text_begin =
        "Floccinaucinihilipilification is the act of deeming something worthless.";
    gui_scroll_text_t *t4 = gui_scroll_text_create(gui_obj_get_root(), "wrap_scroll", 0, y,
                                                   gui_get_screen_width() / 2, 100);
    gui_scroll_text_set(t4, text_begin, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                        strlen(text_begin), font_height);
    gui_scroll_text_scroll_set(t4, SCROLL_Y, 100, 100, 10000, 0);
    gui_text_wordwrap_set(&t4->base, true);

    /* Case 5: Small words, multiple spaces, newlines, MULTI_RIGHT, translucent */
    char *text_edge =
        "To be, or not to be,\nthat is the question:\n"
        "Whether 'tis nobler in\nthe mind to suffer.";
    gui_text_t *t5 = gui_text_create(gui_obj_get_root(), "wrap_edge", gui_get_screen_width() / 2, y,
                                     gui_get_screen_width() / 2, 0);
    gui_text_set(t5, text_edge, GUI_FONT_SRC_BMP, APP_COLOR_RED, strlen(text_edge), font_height);
    gui_text_mode_set(t5, MULTI_RIGHT);
    gui_text_wordwrap_set(t5, true);
}
