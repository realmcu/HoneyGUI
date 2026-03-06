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
    gui_text_t *text = gui_text_create(gui_obj_get_root(), "text", 0, 0, 0, 0);
    gui_text_set(text, "HoneyGUI", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 8, 32);
    gui_text_type_set(text, font32b1, FONT_SRC_MEMADDR);

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
 * | 3 | custom_font_rendering_demo()     | Custom font engine with user-defined load/draw/unload/destroy            |
 * | 4 | text_font_rendering_test()       | Swipeable views: bitmap(1/2/4/8-bit), vector, bold, matrix, img, bench   |
 * | 5 | text_font_layout_test()          | Swipeable views: single-line, multi-line, scroll, vertical, RTL          |
 * | 6 | text_multi_language_test()       | Language list with per-language rendering (AR, EN, ZH, TH, HE)           |
 * | 7 | text_font_scroll_function_test() | Scroll X/Y/reverse with stop/reset/pause/resume and alignment modes      |
 * | 8 | text_font_source_mode_test()     | 3x4 grid: source(MEMADDR/FTL/FILESYS) x type(BMP/IMG/MAT/TTF)            |
 *
 * @return 0 on success
 */
static int app_init(void)
{
    font_file_init();

    text_widget_example();              /* [1] Basic text widget       */
    // scroll_text_widget_example();    /* [2] Scroll text             */
    // custom_font_rendering_demo();    /* [3] Custom font rendering   */
    // text_font_rendering_test();      /* [4] Font rendering styles   */
    // text_font_layout_test();         /* [5] Text layout modes       */
    // text_multi_language_test();      /* [6] Multi-language support  */
    // text_font_scroll_function_test();/* [7] Scroll function control */
    // text_font_source_mode_test();    /* [8] Font source modes       */

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
