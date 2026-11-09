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
 *                           Private Functions
 *============================================================================*/

/**
 * @brief Test event callback
 * @param obj Event source object
 * @param e Event type
 */
static void test_event_cb(void *obj, gui_event_t *e)
{
    gui_obj_t *this = (gui_obj_t *)obj;

    gui_log("Event test obj name = %s, e = 0x%x !\n", this->name, e->code);
}

/**
 * @brief Timer callback
 * @param param Timer parameters
 */
static void test_timer_cb(void *param)
{
    (void)param;

    gui_log("timer cb test!\n");
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Create and configure a text widget example
 */
/* gui text widget example start*/
void text_widget_example(void)
{
    char *text_string = "HoneyGUI";
    gui_text_t *text = gui_text_create(gui_obj_get_root(), "text", 0, 0, 0, 0);
    gui_text_set(text, text_string, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(text_string), 32);
    gui_text_type_set(text, font32b2, FONT_SRC_MEMADDR);

    gui_obj_add_event_cb(text, (gui_event_cb_t)test_event_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_obj_create_timer(&(text->base), 1000, true, test_timer_cb);
    gui_obj_start_timer(&(text->base));
}
/* gui text widget example end*/

/**
 * @brief Text widget demo application entry
 *
 * Uncomment the desired test case(s) below to run.
 * Only one test case should be active at a time to
 * avoid UI conflicts.
 *
 * @note Available test cases:
 *
 * | # | Function                         | Description                                                              |
 * |---|----------------------------------|--------------------------------------------------------------------------|
 * | 1 | text_widget_example()            | Basic text widget with click event and periodic timer callback           |
 * | 2 | scroll_text_widget_example()     | Horizontal (SCROLL_X) and vertical (SCROLL_Y) auto-scrolling text        |
 * | 3 | text_font_rendering_test()       | Swipeable views: bitmap 1/2/4/8-bit, typography, benchmark               |
 * | 4 | text_font_layout_test()          | Swipeable views: single-line, multi-line, scroll, vertical, RTL          |
 * | 5 | text_multi_language_test()       | Language list with per-language rendering (needs gui_list)               |
 * | 6 | text_font_scroll_function_test() | Scroll X/Y/reverse with stop/reset/pause/resume and alignment modes      |
 * | 7 | text_font_scroll_loop_test()     | Scroll loop (marquee) for X/Y forward and reverse directions             |
 * | 8 | text_wordwrap_test()             | Word-wrap with oversized word break protection                           |
 * | 9 | text_measure_test()              | Layout measure: single-line and multi-line metrics                       |
 * |10 | text_clip_test()                 | Partial text display using win clip (needs gui_rect)                    |
 * |11 | text_font_typo_rendering_test()  | Typography rendering: bitmap baseline and line height                    |
 * |12 | text_font_fallback_test()        | Mixed-language fallback: CJK+EN in one string with priority chain        |
 * |13 | text_missing_glyph_test()        | Missing glyph: skip vs substitute chain                                  |
 *
 * @return 0 on success
 */
static int app_init(void)
{
    font_file_init();

    text_widget_example();              /* [1] Basic text widget       */
    // scroll_text_widget_example();    /* [2] Scroll text             */
    // text_font_rendering_test();      /* [3] Font rendering styles   */
    // text_font_layout_test();         /* [4] Text layout modes       */
    // text_multi_language_test();      /* [5] Needs gui_list          */
    // text_font_scroll_function_test();/* [6] Scroll function control */
    // text_font_scroll_loop_test();    /* [7] Scroll loop (marquee)   */
    // text_wordwrap_test();            /* [8] Word-wrap & break test  */
    // text_measure_test();             /* [9] Layout measure test     */
    // text_clip_test();                /* [10] Needs gui_rect         */
    // text_font_typo_rendering_test(); /* [11] Typography bitmap      */
    // text_font_fallback_test();       /* [12] Mixed-lang fallback    */
    // text_missing_glyph_test();       /* [13] Missing glyph modes    */

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
