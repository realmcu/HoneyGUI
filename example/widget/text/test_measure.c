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

    /* [3] BMP horizontal scroll */
    gui_scroll_text_t *scroll_x = gui_scroll_text_create(root, "scroll_x", 320, 0, 160, 50);
    char *str3 = "HoneyGUI Horizontal Scroll Text Layout Measure";
    gui_scroll_text_set(scroll_x, str3, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(str3), 32);
    gui_scroll_text_type_set(scroll_x, font32b4, FONT_SRC_MEMADDR);
    gui_scroll_text_scroll_set(scroll_x, SCROLL_X, 0, 0, 8000, 0);

    gui_text_layout_measure(&scroll_x->base);
    gui_log("[BMP scroll X] w=%d h=%d lines=%d visible=%d/%d\n",
            scroll_x->base.char_width_sum, scroll_x->base.char_height_sum,
            scroll_x->base.char_line_sum, scroll_x->base.active_font_len,
            scroll_x->base.font_len);
}
/* gui text measure example end */
