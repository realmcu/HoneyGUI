/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "test_font.h"
#include "gui_rect.h"

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
#define GRID_COLS           3
#define GRID_ROWS           3

/*============================================================================*
 *                            Macros
 *============================================================================*/

/*============================================================================*
 *                            Variables
 *============================================================================*/
static const gui_view_descriptor_t *left_view = NULL;
static const gui_view_descriptor_t *right_view = NULL;

static void ui_text_font(gui_view_t *view);
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, ui_text_font, NULL);

/* Row = vertical alignment (top / mid / bottom), column = horizontal. */
static const char *const text_string[GRID_ROWS][GRID_COLS] =
{
    {"MULTI_LEFT\nTop\nleft", "MULTI_CENTER\nTop\ncenter", "MULTI_RIGHT\nTop\nright"},
    {
        "MULTI_MID_LEFT\nMid\nleft", "MULTI_MID_CENTER\nMid\ncenter",
        "MULTI_MID_RIGHT\nMid\nright"
    },
    {
        "MULTI_BOT_LEFT\nBottom\nleft", "MULTI_BOT_CENTER\nBottom\ncenter",
        "MULTI_BOT_RIGHT\nBottom\nright"
    }
};

static const TEXT_MODE text_mode[GRID_ROWS][GRID_COLS] =
{
    {MULTI_LEFT, MULTI_CENTER, MULTI_RIGHT},
    {MULTI_MID_LEFT, MULTI_MID_CENTER, MULTI_MID_RIGHT},
    {MULTI_BOT_LEFT, MULTI_BOT_CENTER, MULTI_BOT_RIGHT}
};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/


static int gui_view_get_other_view_descriptor_init(void)
{
    left_view = gui_view_descriptor_get("test_font_layout_single");
    right_view = gui_view_descriptor_get("test_font_layout_scroll");
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

/**
 * @brief Create a 3x3 grid of multi-line layout modes
 * @param view Parent view widget
 */
static void ui_text_font(gui_view_t *view)
{
    gui_color_t row_color[GRID_ROWS] =
    {
        APP_COLOR_WHITE,
        APP_COLOR_YELLOW,
        APP_COLOR_CYAN
    };

    int32_t cell_w = gui_get_screen_width() / GRID_COLS;
    int32_t cell_h = gui_get_screen_height() / GRID_ROWS;

    for (int row = 0; row < GRID_ROWS; row++)
    {
        for (int col = 0; col < GRID_COLS; col++)
        {
            int16_t x = (int16_t)(col * cell_w);
            int16_t y = (int16_t)(row * cell_h);

            /* Border makes the vertical anchor visible: the last text line must
             * sit on the cell's top edge, centre line, or bottom edge. */
            gui_rounded_rect_t *frame = gui_rect_create(view, "cell", x, y,
                                                        (int)cell_w - 1, (int)cell_h - 1, 0,
                                                        gui_rgba(0, 0, 0, 0));
            gui_rect_set_stroke(frame, 1.0f, gui_rgba(80, 80, 80, UINT8_MAX));

            const char *str = text_string[row][col];
            gui_text_t *text = gui_text_create(view, "text", x, y,
                                               (int)cell_w - 1, (int)cell_h - 1);
            gui_text_set(text, (void *)str, TEST_FONT_TYPE, row_color[row],
                         (uint16_t)strlen(str), FONT_SIZE);
            gui_text_type_set(text, TEST_FONT, FONT_SRC_MEMADDR);
            gui_text_mode_set(text, text_mode[row][col]);
        }
    }

    gui_view_switch_on_event(view, left_view->name, SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION, GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, right_view->name, SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION, GUI_EVENT_TOUCH_MOVE_LEFT);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
