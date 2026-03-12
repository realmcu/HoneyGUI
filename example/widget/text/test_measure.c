/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                           Header Files
 *============================================================================*/
#include "test_font.h"

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/* gui text measure example start */
void text_measure_test(void)
{
    gui_obj_t *root = gui_obj_get_root();

    /* [1] BMP single-line */
    gui_text_t *bmp_single = gui_text_create(root, "bmp_single", 0, 0, 200, 50);
    char *str1 = "HoneyGUI";
    gui_text_set(bmp_single, str1, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(str1), 32);
    gui_text_type_set(bmp_single, font32b4, FONT_SRC_MEMADDR);
    gui_text_mode_set(bmp_single, LEFT);

    gui_text_layout_measure(bmp_single);
    gui_log("[BMP single] w=%d h=%d lines=%d visible=%d/%d\n",
            bmp_single->char_width_sum, bmp_single->char_height_sum,
            bmp_single->char_line_sum, bmp_single->active_font_len, bmp_single->font_len);

    /* [2] BMP multi-line with wordwrap */
    gui_text_t *bmp_multi = gui_text_create(root, "bmp_multi", 0, 50, 100, 200);
    char *str2 = "Hello HoneyGUI Text Layout Measure";
    gui_text_set(bmp_multi, str2, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(str2), 32);
    gui_text_type_set(bmp_multi, font32b4, FONT_SRC_MEMADDR);
    gui_text_mode_set(bmp_multi, MULTI_LEFT);
    gui_text_wordwrap_set(bmp_multi, true);

    gui_text_layout_measure(bmp_multi);
    gui_log("[BMP multi]  w=%d h=%d lines=%d visible=%d/%d\n",
            bmp_multi->char_width_sum, bmp_multi->char_height_sum,
            bmp_multi->char_line_sum, bmp_multi->active_font_len, bmp_multi->font_len);

    /* [3] TTF single-line */
    gui_text_t *ttf_single = gui_text_create(root, "ttf_single", 0, 260, 300, 50);
    char *str3 = "TTF Single";
    gui_text_set(ttf_single, str3, GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen(str3), 32);
    gui_text_type_set(ttf_single, font32vb4, FONT_SRC_MEMADDR);
    gui_text_mode_set(ttf_single, LEFT);

    gui_text_layout_measure(ttf_single);
    gui_log("[TTF single] w=%d h=%d lines=%d visible=%d/%d\n",
            ttf_single->char_width_sum, ttf_single->char_height_sum,
            ttf_single->char_line_sum, ttf_single->active_font_len, ttf_single->font_len);

    /* [4] TTF multi-line with wordwrap */
    gui_text_t *ttf_multi = gui_text_create(root, "ttf_multi", 0, 320, 120, 200);
    char *str4 = "TTF Multi Line Word Wrap Test Example";
    gui_text_set(ttf_multi, str4, GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen(str4), 32);
    gui_text_type_set(ttf_multi, font32vb4, FONT_SRC_MEMADDR);
    gui_text_mode_set(ttf_multi, MULTI_LEFT);
    gui_text_wordwrap_set(ttf_multi, true);

    gui_text_layout_measure(ttf_multi);
    gui_log("[TTF multi]  w=%d h=%d lines=%d visible=%d/%d\n",
            ttf_multi->char_width_sum, ttf_multi->char_height_sum,
            ttf_multi->char_line_sum, ttf_multi->active_font_len, ttf_multi->font_len);
}
/* gui text measure example end */
