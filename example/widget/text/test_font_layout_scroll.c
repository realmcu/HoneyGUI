/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file test_font_layout_scroll.c
  * @brief Scroll text layout mode test
  * @details Test application for scroll TEXT_MODE layout modes
  * @author luke_sun@realsil.com.cn
  * @date 2025/12/09
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "test_font.h"

/*============================================================================*
 *                           Configuration
 *============================================================================*/

#if LAYOUT_USE_VECTOR_FONT
#define TEST_FONT           font32vb4           // Vector font
#define TEST_FONT_TYPE      GUI_FONT_SRC_TTF    // Vector font uses TTF source
#else
#define TEST_FONT           font32b8            // Bitmap font
#define TEST_FONT_TYPE      GUI_FONT_SRC_BMP    // Bitmap font uses BMP source
#endif

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/
#define CURRENT_VIEW_NAME   "test_font_layout_scroll"
#define FONT_SIZE           32
#define FONT_NUM            6
#define SCROLL_INTERVAL_MS  10000
#define SCROLL_DURATION_MS  0

/*============================================================================*
 *                            Macros
 *============================================================================*/

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *left_view = NULL;
static const gui_view_descriptor_t *right_view = NULL;

static void ui_text_font(gui_view_t *view);
static const gui_view_descriptor_t descriptor =
{
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = ui_text_font,
    .on_switch_out = NULL,
};



/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    left_view = gui_view_descriptor_get("test_font_layout_multi");
    right_view = gui_view_descriptor_get("test_font_layout_vertical");
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

/**
 * @brief Create scroll text widgets with different scroll modes
 * @param view Parent view widget
 */
static void ui_text_font(gui_view_t *view)
{
    // Get screen dimensions from DC
    uint32_t screen_width = gui_get_screen_width();
    uint32_t screen_height = gui_get_screen_height();
    uint32_t half_height = screen_height / 2;

    // Horizontal scroll modes (upper half)
    gui_scroll_text_t *text_h[4];
    char *text_string_h[4] =
    {
        "SCROLL_X - Horizontal scrolling text example. This text will scroll from right to left continuously.",
        "SCROLL_X_REVERSE - Reverse horizontal scrolling text example. This text will scroll from left to right continuously.",
        "SCROLL_X_MID - Horizontal mid scrolling text example. This text will scroll from right to left with vertical centering.",
        "SCROLL_X_MID_REVERSE - Reverse horizontal mid scrolling text example. This text will scroll from left to right with vertical centering."
    };
    TEXT_MODE text_mode_h[4] = {SCROLL_X, SCROLL_X_REVERSE, SCROLL_X_MID, SCROLL_X_MID_REVERSE};
    gui_color_t text_color_h[4] = {APP_COLOR_RED, APP_COLOR_BLUE, APP_COLOR_CYAN, APP_COLOR_WHITE};

    for (int i = 0; i < 4; i++)
    {
        text_h[i] = gui_scroll_text_create(view, "scroll_text_h", 0, 0, screen_width, half_height);
        gui_scroll_text_set(text_h[i], text_string_h[i], TEST_FONT_TYPE, text_color_h[i],
                            strlen(text_string_h[i]), FONT_SIZE);
        gui_scroll_text_type_set(text_h[i], TEST_FONT, FONT_SRC_MEMADDR);
        gui_scroll_text_scroll_set(text_h[i], text_mode_h[i], 0, 0,
                                   SCROLL_INTERVAL_MS, SCROLL_DURATION_MS);
    }

    // Vertical scroll modes (lower half)
    gui_scroll_text_t *text_v[2];
    char *text_string_v[2] =
    {
        "SCROLL_Y - Vertical scrolling text example. This text will scroll from bottom to top continuously. Everyone has the right to freedom of thought, conscience and religion.",
        "SCROLL_Y_REVERSE - Reverse vertical scrolling text example. This text will scroll from top to bottom continuously. Everyone has the right to freedom of thought, conscience and religion."
    };
    TEXT_MODE text_mode_v[2] = {SCROLL_Y, SCROLL_Y_REVERSE};
    gui_color_t text_color_v[2] = {APP_COLOR_GREEN, APP_COLOR_YELLOW};

    for (int i = 0; i < 2; i++)
    {
        text_v[i] = gui_scroll_text_create(view, "scroll_text_v", 0, half_height, screen_width,
                                           FONT_SIZE * 2);
        gui_scroll_text_set(text_v[i], text_string_v[i], TEST_FONT_TYPE, text_color_v[i],
                            strlen(text_string_v[i]), FONT_SIZE);
        gui_scroll_text_type_set(text_v[i], TEST_FONT, FONT_SRC_MEMADDR);
        gui_scroll_text_scroll_set(text_v[i], text_mode_v[i], 0, 0,  SCROLL_INTERVAL_MS,
                                   SCROLL_DURATION_MS);
        // Enable word wrap for vertical scrolling
        gui_text_wordwrap_set(&text_v[i]->base, true);
    }

    gui_view_switch_on_event(view, left_view->name, SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION, GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, right_view->name, SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION, GUI_EVENT_TOUCH_MOVE_LEFT);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

