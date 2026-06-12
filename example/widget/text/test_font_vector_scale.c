/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "test_font.h"
#include "gui_win.h"
#include "gui_matrix.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/
#define VEC_SCALE_MIN   0.3f
#define VEC_SCALE_MAX   3.0f
#define VEC_SCALE_STEP  0.05f

/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/**
 * @brief Oscillate a scale factor between VEC_SCALE_MIN and VEC_SCALE_MAX.
 *
 * Shared helper for both scaling test cases so that the window-issued
 * scale and the text self-scale sweep through the same range.
 *
 * @param scale Current scale value (in/out).
 * @param step  Current step direction/magnitude (in/out).
 */
static void vector_scale_step(float *scale, float *step)
{
    *scale += *step;
    if (*scale >= VEC_SCALE_MAX)
    {
        *scale = VEC_SCALE_MAX;
        *step = -VEC_SCALE_STEP;
    }
    else if (*scale <= VEC_SCALE_MIN)
    {
        *scale = VEC_SCALE_MIN;
        *step = VEC_SCALE_STEP;
    }
}

/**
 * @brief Window timer callback: scale issued by the parent window.
 *
 * Directly manipulates the window's matrix so that child text widgets
 * inherit the scale transform top-down through the widget tree.
 *
 * @param param Timer owner object, i.e. the window's base object.
 */
static void win_scale_timer_cb(void *param)
{
    gui_win_t *win = (gui_win_t *)param;
    static float scale = 1.0f;
    static float step = VEC_SCALE_STEP;

    vector_scale_step(&scale, &step);
    matrix_identity(win->base.matrix);
    matrix_scale(scale, scale, win->base.matrix);
    gui_log("[vec_scale] win scale = %.2f\n", (double)scale);
}

/**
 * @brief Text timer callback: text scaling itself (no window parent).
 *
 * The text widget applies a scale matrix to itself, exercising the
 * vector-font self-scale rendering path independent of any parent.
 *
 * @param param Timer owner object, i.e. the text's base object.
 */
static void text_scale_timer_cb(void *param)
{
    gui_text_t *text = (gui_text_t *)param;
    static float scale = 1.0f;
    static float step = VEC_SCALE_STEP;
    gui_matrix_t matrix;

    vector_scale_step(&scale, &step);
    matrix_identity(&matrix);
    matrix_scale(scale, scale, &matrix);
    gui_text_set_matrix(text, &matrix);
    gui_log("[vec_scale] text self scale = %.2f\n", (double)scale);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Create a vector-font scaling test (V3 vector font).
 *
 * Reproduces the vector-font scaling rendering anomaly and serves as a
 * persistent regression test. Two scaling paths run simultaneously:
 *
 *  - Top half : a window contains several vector-font texts; the window
 *    has a timer that loops gui_win_set_scale_rate(), so the texts are
 *    scaled top-down by the parent (win-issued scale).
 *  - Bottom half : standalone vector-font texts not nested in any window;
 *    each has its own timer that applies a scale matrix to itself
 *    (text self-scale).
 */
void text_vector_scale_test(void)
{
    struct gui_dispdev *dc = gui_get_dc();
    const uint32_t font_size = 32;
    const int16_t half_h = dc->screen_height / 2;

    /* ----- Part 1: scale issued by the parent window ----- */
    gui_win_t *win = gui_win_create(gui_obj_get_root(), "vec_scale_win",
                                    0, 0, dc->screen_width, half_h);

    char *win_str = "Win Scale";
    gui_text_t *wt = gui_text_create(win, "win_text", 0, font_size, dc->screen_width, font_size);
    gui_text_set(wt, win_str, GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen(win_str), font_size);
    gui_text_type_set(wt, font32vb4, FONT_SRC_MEMADDR);
    gui_text_rendermode_set(wt, 2);

    char *win_digit = "0123456789";
    gui_text_t *wd = gui_text_create(win, "win_digit", 0, font_size * 2, dc->screen_width, font_size);
    gui_text_set(wd, win_digit, GUI_FONT_SRC_TTF, APP_COLOR_GREEN, strlen(win_digit), font_size);
    gui_text_type_set(wd, font32vb4, FONT_SRC_MEMADDR);
    gui_text_rendermode_set(wd, 2);

    gui_obj_create_timer(&(win->base), 10, true, win_scale_timer_cb);
    gui_obj_start_timer(&(win->base));

    /* ----- Part 2: text scaling itself, no window parent ----- */
    char *self_str = "Self Scale";
    gui_text_t *st = gui_text_create(gui_obj_get_root(), "self_text",
                                     0, half_h + font_size, dc->screen_width, font_size);
    gui_text_set(st, self_str, GUI_FONT_SRC_TTF, APP_COLOR_RED, strlen(self_str), font_size);
    gui_text_type_set(st, font32vb4, FONT_SRC_MEMADDR);
    gui_text_rendermode_set(st, 2);

    gui_obj_create_timer(&(st->base), 10, true, text_scale_timer_cb);
    gui_obj_start_timer(&(st->base));
}
