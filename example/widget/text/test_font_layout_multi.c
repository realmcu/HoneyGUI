/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file test_font_layout_multi.c
  * @brief Multi-line text layout mode test
  * @details Test application for multi-line TEXT_MODE layout modes
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
#define CURRENT_VIEW_NAME   "test_font_layout_multi"
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
    "MULTI_LEFT\nMultiple lines\nLeft aligned",
    "MULTI_CENTER\nMultiple lines\nCenter aligned",
    "MULTI_RIGHT\nMultiple lines\nRight aligned",
    "MULTI_MID_LEFT\nMultiple lines\nMid left aligned",
    "MULTI_MID_CENTER\nMultiple lines\nMid center aligned",
    "MULTI_MID_RIGHT\nMultiple lines\nMid right aligned"
};

static TEXT_MODE text_mode[FONT_NUM] =
{
    MULTI_LEFT,
    MULTI_CENTER,
    MULTI_RIGHT,
    MULTI_MID_LEFT,
    MULTI_MID_CENTER,
    MULTI_MID_RIGHT
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
    left_view = gui_view_descriptor_get("test_font_layout_single");
    right_view = gui_view_descriptor_get("test_font_layout_scroll");
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

/**
 * @brief Create text widgets with different multi-line layout modes
 * @param view Parent view widget
 */
static void ui_text_font(gui_view_t *view)
{
    gui_text_t *text[FONT_NUM];
    gui_color_t text_color[FONT_NUM] =
    {
        APP_COLOR_RED,
        APP_COLOR_GREEN,
        APP_COLOR_BLUE,
        APP_COLOR_YELLOW,
        APP_COLOR_CYAN,
        APP_COLOR_WHITE
    };

    for (int i = 0; i < FONT_NUM; i++)
    {
        text[i] = gui_text_create(view, "text", 0, 0, 0, 0);
        gui_text_set(text[i], text_string[i], TEST_FONT_TYPE, text_color[i], strlen(text_string[i]),
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
