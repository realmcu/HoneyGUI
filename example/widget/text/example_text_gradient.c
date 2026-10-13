/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                           Header Files
 *============================================================================*/
#include "test_font.h"
#include "gui_rect.h"

/*============================================================================*
 *                           Constants
 *============================================================================*/
#define ROW_X            10
#define ROW_Y            8
#define ROW_STEP         56
#define ROW_W            230
#define ROW_H            52
#define LABEL_X          256
#define LABEL_W          220
#define TEXT_SIZE        36
#define LABEL_SIZE       20
#define SCROLL_Y         404
#define SCROLL_STEP      38
#define SCROLL_W         460
#define SCROLL_H         32
#define SCROLL_SIZE      28

/*============================================================================*
 *                            Variables
 *============================================================================*/
static const gui_text_gradient_stop_t stops[] =
{
    {0.0f, {.color.argb_full = 0xFFFF2020}},
    {1.0f, {.color.argb_full = 0xFF4060FF}},
};

/* Gradient boxes read back from the 480x480 simulator (x0, y0, x1, y1, inclusive).
 * Hardcoded: re-measure if the font, text size or row layout changes. */
static const int16_t real_box[][4] =
{
    {13,  19,  175, 54},    /* INK, "HoneyGUI" */
    {13,  75,  175, 110},
    {13,  131, 175, 166},
    {58,  187, 189, 222},   /* INK, "Agy ace" */
    {58,  237, 190, 278},   /* LAYOUT */
    {10,  288, 239, 339},   /* WIDGET */
    {10,  349, 177, 390},   /* LAYOUT, "HoneyGUI" */
};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/* The gray rect is the widget box; the red frame is the box the gradient uses. */
static gui_text_t *gradient_row_create(int row, char *content, TEXT_MODE mode,
                                       gui_text_gradient_dir_t dir,
                                       gui_text_gradient_box_t box, char *label)
{
    int y = ROW_Y + row * ROW_STEP;
    const int16_t *b = real_box[row];

    gui_rect_create(gui_obj_get_root(), "gradient_bg", ROW_X, y, ROW_W, ROW_H, 0,
                    gui_rgb(48, 48, 48));

    gui_text_t *text = gui_text_create(gui_obj_get_root(), "gradient_text", ROW_X, y,
                                       ROW_W, ROW_H);
    gui_text_set(text, content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen(content), TEXT_SIZE);
    gui_text_type_set(text, font32vb4, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, mode);
    gui_text_set_gradient(text, dir, stops, 2);
    gui_text_set_gradient_box(text, box);

    gui_rounded_rect_t *frame = gui_rect_create(gui_obj_get_root(), "gradient_box", b[0], b[1],
                                                b[2] - b[0] + 1, b[3] - b[1] + 1, 0,
                                                gui_rgba(0, 0, 0, 0));
    gui_rect_set_stroke(frame, 1.0f, APP_COLOR_RED);

    gui_text_t *info = gui_text_create(gui_obj_get_root(), "gradient_label", LABEL_X, y,
                                       LABEL_W, ROW_H);
    gui_text_set(info, label, GUI_FONT_SRC_TTF, APP_COLOR_SILVER, strlen(label), LABEL_SIZE);
    gui_text_type_set(info, font32vb4, FONT_SRC_MEMADDR);
    gui_text_mode_set(info, MULTI_LEFT);

    return text;
}

static void gradient_scroll_create(int index, char *content, gui_text_gradient_box_t box)
{
    gui_scroll_text_t *scroll = gui_scroll_text_create(gui_obj_get_root(), "gradient_scroll",
                                                       ROW_X, SCROLL_Y + index * SCROLL_STEP,
                                                       SCROLL_W, SCROLL_H);
    gui_scroll_text_set(scroll, content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                        strlen(content), SCROLL_SIZE);
    gui_scroll_text_type_set(scroll, font32vb4, FONT_SRC_MEMADDR);
    gui_scroll_text_scroll_set(scroll, SCROLL_X, 0, 0, 8000, 0);
    gui_scroll_text_loop_set(scroll, true, 32);
    gui_text_set_gradient(&scroll->base, GUI_TEXT_GRADIENT_HORIZONTAL, stops, 2);
    gui_text_set_gradient_box(&scroll->base, box);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Vector text gradient: three directions, three box modes and custom handles
 *
 * Rows 1-3 show each direction with the default INK box. Rows 4-6 use the same
 * centered diagonal gradient on "Agy ace" so the INK, LAYOUT and WIDGET boxes can
 * be compared; WIDGET spans the whole rect, so only its middle colors appear.
 * Row 7 moves the handles inside a LAYOUT box. The red frame marks the box
 * each row's gradient is normalized against. The two bottom rows are looping
 * scroll texts: INK carries the gradient with the text, WIDGET keeps it fixed.
 */
/* gui text gradient example start */
void text_gradient_example(void)
{
    char *name = "HoneyGUI";
    char *glyphs = "Agy ace";

    gradient_row_create(0, name, MID_LEFT, GUI_TEXT_GRADIENT_HORIZONTAL,
                        GUI_TEXT_GRADIENT_BOX_INK,
                        "dir: HORIZONTAL\nbox: INK (default)");
    gradient_row_create(1, name, MID_LEFT, GUI_TEXT_GRADIENT_VERTICAL,
                        GUI_TEXT_GRADIENT_BOX_INK,
                        "dir: VERTICAL\nbox: INK (default)");
    gradient_row_create(2, name, MID_LEFT, GUI_TEXT_GRADIENT_DIAGONAL_TL_BR,
                        GUI_TEXT_GRADIENT_BOX_INK,
                        "dir: DIAGONAL_TL_BR\nbox: INK (default)");

    gradient_row_create(3, glyphs, MID_CENTER, GUI_TEXT_GRADIENT_DIAGONAL_TL_BR,
                        GUI_TEXT_GRADIENT_BOX_INK,
                        "box: INK\nglyph outline union");
    gradient_row_create(4, glyphs, MID_CENTER, GUI_TEXT_GRADIENT_DIAGONAL_TL_BR,
                        GUI_TEXT_GRADIENT_BOX_LAYOUT,
                        "box: LAYOUT\nadvance x line height");
    gradient_row_create(5, glyphs, MID_CENTER, GUI_TEXT_GRADIENT_DIAGONAL_TL_BR,
                        GUI_TEXT_GRADIENT_BOX_WIDGET,
                        "box: WIDGET\nwhole gray rect");

    gui_text_t *handles = gradient_row_create(6, name, MID_LEFT,
                                              GUI_TEXT_GRADIENT_HORIZONTAL,
                                              GUI_TEXT_GRADIENT_BOX_LAYOUT,
                                              "box: LAYOUT\nhandles (.2,.3)-(.8,.7)");
    gui_text_set_gradient_handles(handles, 0.2f, 0.3f, 0.8f, 0.7f);

    gradient_scroll_create(0, "INK: gradient moves with the text, the start is always red. ",
                           GUI_TEXT_GRADIENT_BOX_INK);
    gradient_scroll_create(1, "WIDGET: gradient stays put, the text changes color as it scrolls. ",
                           GUI_TEXT_GRADIENT_BOX_WIDGET);
}
/* gui text gradient example end */
