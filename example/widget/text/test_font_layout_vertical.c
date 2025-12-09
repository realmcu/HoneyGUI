/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file test_font_layout_vertical.c
  * @brief Vertical text layout mode test
  * @details Test application for vertical TEXT_MODE layout modes
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
#define CURRENT_VIEW_NAME   "test_font_layout_vertical"
#define FONT_SIZE           32
#define FONT_NUM            6

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

static char *text_string[FONT_NUM] =
{
    "VERTICAL_LEFT_TOP",
    "VERTICAL_LEFT_MID",
    "VERTICAL_LEFT_BOT",
    "VERTICAL_RIGHT_TOP",
    "VERTICAL_RIGHT_MID",
    "VERTICAL_RIGHT_BOT"
};

static TEXT_MODE text_mode[FONT_NUM] =
{
    VERTICAL_LEFT_TOP,
    VERTICAL_LEFT_MID,
    VERTICAL_LEFT_BOT,
    VERTICAL_RIGHT_TOP,
    VERTICAL_RIGHT_MID,
    VERTICAL_RIGHT_BOT
};

// Different colors for each mode (initialized in function)

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
    right_view = gui_view_descriptor_get("test_font_layout_rtl");
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

/**
 * @brief Create text widgets with vertical layout modes
 * @param view Parent view widget
 */
static void ui_text_font(gui_view_t *view)
{
    // Get screen width dynamically from DC
    uint32_t screen_width = gui_get_screen_width();
    int16_t column_width = screen_width / 6;  // Divide screen into 6 columns

    // Calculate X coordinates for each column dynamically
    // Left side: 3 columns (0, 1/6, 2/6 of screen width)
    // Right side: 3 columns (3/6, 4/6, 5/6 of screen width)
    int16_t vertical_x[FONT_NUM] =
    {
        0,                      // VERTICAL_LEFT_TOP
        column_width,           // VERTICAL_LEFT_MID
        column_width * 2,       // VERTICAL_LEFT_BOT
        column_width * 3,       // VERTICAL_RIGHT_TOP
        column_width * 4,       // VERTICAL_RIGHT_MID
        column_width * 5        // VERTICAL_RIGHT_BOT
    };

    // Different colors for each mode
    gui_color_t text_colors[FONT_NUM] =
    {
        APP_COLOR_RED,          // VERTICAL_LEFT_TOP
        APP_COLOR_GREEN,        // VERTICAL_LEFT_MID
        APP_COLOR_BLUE,         // VERTICAL_LEFT_BOT
        APP_COLOR_YELLOW,       // VERTICAL_RIGHT_TOP
        APP_COLOR_CYAN,         // VERTICAL_RIGHT_MID
        APP_COLOR_PURPLE        // VERTICAL_RIGHT_BOT
    };

    gui_text_t *text[FONT_NUM];
    for (int i = 0; i < FONT_NUM; i++)
    {
        // Create text widget with dynamically calculated X coordinate and column width
        text[i] = gui_text_create(view, "text", vertical_x[i], 0, column_width, 0);
        gui_text_set(text[i], text_string[i], TEST_FONT_TYPE, text_colors[i], strlen(text_string[i]),
                     FONT_SIZE);
        gui_text_type_set(text[i], TEST_FONT, FONT_SRC_MEMADDR);
        gui_text_mode_set(text[i], text_mode[i]);
    }

    gui_view_switch_on_event(view, left_view->name, SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION, GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, right_view->name, SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION, GUI_EVENT_TOUCH_MOVE_LEFT);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
