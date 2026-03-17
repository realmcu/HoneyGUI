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
 *                           Private Functions
 *============================================================================*/

/**
 * @brief Helper: draw a semi-transparent rect as visual indicator
 */
static void draw_indicator(void *parent, const char *name, int16_t x, int16_t y, int16_t w,
                           int16_t h, gui_color_t color, uint8_t opacity)
{
    gui_rounded_rect_t *r = gui_rect_create(parent, name, x, y, w, h, 0, color);
    gui_rect_set_opacity(r, opacity);
}

static int16_t move_dir = 2;
static void clip_move_timer_cb(void *param)
{
    gui_text_t *text = (gui_text_t *)param;
    int16_t sw = gui_get_screen_width();
    int16_t x = text->base.x + move_dir;
    if (x > sw / 2 || x < -sw / 2)
    {
        move_dir = -move_dir;
        x = text->base.x + move_dir;
    }
    gui_text_move(text, x, text->base.y);
}

static int16_t move_dir7 = 2;
static void clip_move_timer_cb7(void *param)
{
    gui_win_t *win = (gui_win_t *)param;
    int16_t sw = gui_get_screen_width();
    int16_t x = win->base.x + move_dir7;
    if (x > sw / 2 || x < -sw / 2)
    {
        move_dir7 = -move_dir7;
        x = win->base.x + move_dir7;
    }
    gui_win_move(win, x, win->base.y);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Test partial/clipped text display using gui_text_set_scope
 *
 * Test cases:
 * 1. Full text (no clipping) - reference
 * 2. Show bottom half only
 * 3. Show top half only
 * 4. Show right half only
 * 5. Show center region
 *
 * Dark rect = full text area, bright rect = scope (visible) area
 */
void text_clip_test(void)
{
    int16_t fh = 32;  /* font height */
    int16_t sw = gui_get_screen_width();
    int16_t y = 0;
    int16_t gap = 16;
    char *str = "HoneyGUI Clip Test: Show partial text region";
    uint16_t len = strlen(str);
    int16_t th = fh * 2; /* total text height: 2 lines */
    uint8_t bg_opa = 40; /* dark background opacity */
    uint8_t sc_opa = 80; /* scope highlight opacity */

    /* Case 1: Full text, no clipping (reference) */
    draw_indicator(gui_obj_get_root(), "bg1", 0, y, sw, th, APP_COLOR_CYAN, bg_opa);
    gui_text_t *t1 = gui_text_create(gui_obj_get_root(), "clip_full", 0, y, sw, 0);
    gui_text_set(t1, str, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, len, fh);
    gui_text_type_set(t1, font32b4, FONT_SRC_MEMADDR);
    gui_text_wordwrap_set(t1, true);
    gui_text_mode_set(t1, MULTI_LEFT);
    y += th + gap;

    /* Case 2: Show bottom half only */
    draw_indicator(gui_obj_get_root(), "bg2", 0, y, sw, th, APP_COLOR_GREEN, bg_opa);
    draw_indicator(gui_obj_get_root(), "sc2", 0, y + fh, sw, fh, APP_COLOR_GREEN, sc_opa);
    gui_text_t *t2 = gui_text_create(gui_obj_get_root(), "clip_bottom", 0, y, sw, th);
    gui_text_set(t2, str, GUI_FONT_SRC_BMP, APP_COLOR_GREEN, len, fh);
    gui_text_type_set(t2, font32b4, FONT_SRC_MEMADDR);
    gui_text_mode_set(t2, MULTI_LEFT);
    gui_text_wordwrap_set(t2, true);
    gui_text_set_scope(t2, 0, fh, sw, fh);
    y += th + gap;

    /* Case 3: Show top half only */
    draw_indicator(gui_obj_get_root(), "bg3", 0, y, sw, th, APP_COLOR_RED, bg_opa);
    draw_indicator(gui_obj_get_root(), "sc3", 0, y, sw, fh, APP_COLOR_RED, sc_opa);
    gui_text_t *t3 = gui_text_create(gui_obj_get_root(), "clip_top", 0, y, sw, th);
    gui_text_set(t3, str, GUI_FONT_SRC_BMP, APP_COLOR_RED, len, fh);
    gui_text_type_set(t3, font32b4, FONT_SRC_MEMADDR);
    gui_text_mode_set(t3, MULTI_LEFT);
    gui_text_wordwrap_set(t3, true);
    gui_text_set_scope(t3, 0, 0, sw, fh);
    y += th + gap;

    /* Case 4: Show right half only */
    draw_indicator(gui_obj_get_root(), "bg4", 0, y, sw, th, APP_COLOR_BLUE, bg_opa);
    draw_indicator(gui_obj_get_root(), "sc4", sw / 2, y, sw / 2, th, APP_COLOR_BLUE, sc_opa);
    gui_text_t *t4 = gui_text_create(gui_obj_get_root(), "clip_right", 0, y, sw, th);
    gui_text_set(t4, str, GUI_FONT_SRC_BMP, APP_COLOR_BLUE, len, fh);
    gui_text_type_set(t4, font32b4, FONT_SRC_MEMADDR);
    gui_text_mode_set(t4, MULTI_LEFT);
    gui_text_wordwrap_set(t4, true);
    gui_text_set_scope(t4, sw / 2, 0, sw / 2, th);
    y += th + gap;

    /* Case 5: Show center region */
    int16_t crop = 40;
    draw_indicator(gui_obj_get_root(), "bg5", 0, y, sw, th, APP_COLOR_YELLOW, bg_opa);
    draw_indicator(gui_obj_get_root(), "sc5", crop, y + fh / 2, sw - crop * 2, fh, APP_COLOR_YELLOW,
                   sc_opa);
    gui_text_t *t5 = gui_text_create(gui_obj_get_root(), "clip_center", 0, y, sw, th);
    gui_text_set(t5, str, GUI_FONT_SRC_TTF, APP_COLOR_YELLOW, len, fh);
    gui_text_type_set(t5, font32vb4, FONT_SRC_MEMADDR);
    gui_text_mode_set(t5, MULTI_LEFT);
    gui_text_wordwrap_set(t5, true);
    gui_text_set_scope(t5, crop, fh / 2, sw - crop * 2, fh);
    y += th + gap;

    /* Case 6: Moving text with absolute scope (fixed window) */
    int16_t scope_w = sw / 2;
    int16_t scope_x = (sw - scope_w) / 2;
    draw_indicator(gui_obj_get_root(), "bg6", 0, y, sw, fh, APP_COLOR_WHITE, bg_opa);
    draw_indicator(gui_obj_get_root(), "sc6", scope_x, y, scope_w, fh, APP_COLOR_WHITE, sc_opa);
    gui_text_t *t6 = gui_text_create(gui_obj_get_root(), "clip_abs", 0, y, sw, fh);
    gui_text_set(t6, str, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, len, fh);
    gui_text_type_set(t6, font32b4, FONT_SRC_MEMADDR);
    gui_text_mode_set(t6, LEFT);
    gui_text_set_scope_absolute(t6, scope_x, y, scope_w, fh);
    gui_obj_create_timer(&t6->base, 30, true, clip_move_timer_cb);
    gui_obj_start_timer(&t6->base);
    y += fh + gap;

    /* Case 7: Moving text with relative scope (scope rect follows text) */
    gui_win_t *win7 = gui_win_create(gui_obj_get_root(), "win7", 0, y, sw, fh);
    draw_indicator(win7, "bg7", 0, 0, sw, fh, APP_COLOR_CYAN, bg_opa);
    draw_indicator(win7, "sc7", sw / 4, 0, sw / 2, fh, APP_COLOR_CYAN, sc_opa);
    gui_text_t *t7 = gui_text_create(win7, "clip_rel", 0, 0, sw, fh);
    gui_text_set(t7, str, GUI_FONT_SRC_BMP, APP_COLOR_CYAN, len, fh);
    gui_text_type_set(t7, font32b4, FONT_SRC_MEMADDR);
    gui_text_mode_set(t7, LEFT);
    gui_text_set_scope(t7, sw / 4, 0, sw / 2, fh);
    gui_obj_create_timer(&win7->base, 30, true, clip_move_timer_cb7);
    gui_obj_start_timer(&win7->base);
}
