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

static void draw_indicator(void *parent, const char *name,
                           int16_t x, int16_t y, int16_t w, int16_t h,
                           gui_color_t color, uint8_t opacity)
{
    gui_rounded_rect_t *r = gui_rect_create(parent, name, x, y, w, h, 0, color);
    gui_rect_set_opacity(r, opacity);
}

/* Case 5: absolute scope, text moves left-right */
static int16_t move_dir5 = 2;
static void clip_abs_move_cb(void *param)
{
    gui_text_t *t = (gui_text_t *)param;
    int16_t sw = gui_get_screen_width();
    int16_t x = t->base.x + move_dir5;
    if (x > sw / 2 || x < -sw / 2)
    {
        move_dir5 = -move_dir5;
        x = t->base.x + move_dir5;
    }
    gui_text_move(t, x, t->base.y);
}

/* Case 6: relative scope, win moves left-right */
static int16_t move_dir6 = 2;
static void clip_rel_move_cb(void *param)
{
    gui_win_t *w = (gui_win_t *)param;
    int16_t sw = gui_get_screen_width();
    int16_t x = w->base.x + move_dir6;
    if (x > sw / 2 || x < -sw / 2)
    {
        move_dir6 = -move_dir6;
        x = w->base.x + move_dir6;
    }
    gui_win_move(w, x, w->base.y);
}

/* Case 7: scroll_text moves up-down, absolute scope fixed */
static int16_t scroll_move_dir = 1;
static int16_t scroll_base_y = 0;
static void clip_scroll_move_cb(void *param)
{
    gui_scroll_text_t *st = (gui_scroll_text_t *)param;
    int16_t y = st->base.base.y + scroll_move_dir;
    if (y > scroll_base_y + 30 || y < scroll_base_y - 30)
    {
        scroll_move_dir = -scroll_move_dir;
        y = st->base.base.y + scroll_move_dir;
    }
    st->base.base.y = y;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Test text scope/clip display
 *
 * Cases:
 * 1. Full text (reference, no clip)
 * 2. Bottom half only (relative scope)
 * 3. Top half only (relative scope)
 * 4. Center region, TTF (relative scope)
 * 5. Moving text, absolute scope (fixed window)
 * 6. Moving text, relative scope (follows text)
 * 7. Scroll text SCROLL_X, absolute scope, text moves up-down
 */
void text_clip_test(void)
{
    int16_t fh = 32;
    int16_t sw = gui_get_screen_width();
    int16_t y = 0;
    int16_t gap = 12;
    char *str = "HoneyGUI Clip Test: Show partial text region";
    uint16_t len = strlen(str);
    int16_t th = fh * 2;
    uint8_t bg_opa = 40;
    uint8_t sc_opa = 80;

    /* Case 1: Full text, no clipping */
    draw_indicator(gui_obj_get_root(), "bg1", 0, y, sw, th, APP_COLOR_CYAN, bg_opa);
    gui_text_t *t1 = gui_text_create(gui_obj_get_root(), "clip_full", 0, y, sw, 0);
    gui_text_set(t1, str, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, len, fh);
    gui_text_type_set(t1, font32b4, FONT_SRC_MEMADDR);
    gui_text_mode_set(t1, MULTI_LEFT);
    y += th + gap;

    /* Case 2: Bottom half only */
    draw_indicator(gui_obj_get_root(), "bg2", 0, y, sw, th, APP_COLOR_GREEN, bg_opa);
    draw_indicator(gui_obj_get_root(), "sc2", 0, y + fh, sw, fh, APP_COLOR_GREEN, sc_opa);
    gui_text_t *t2 = gui_text_create(gui_obj_get_root(), "clip_bot", 0, y, sw, th);
    gui_text_set(t2, str, GUI_FONT_SRC_BMP, APP_COLOR_GREEN, len, fh);
    gui_text_type_set(t2, font32b4, FONT_SRC_MEMADDR);
    gui_text_mode_set(t2, MULTI_LEFT);
    gui_text_set_scope(t2, 0, fh, sw, fh);
    y += th + gap;

    /* Case 3: Top half only */
    draw_indicator(gui_obj_get_root(), "bg3", 0, y, sw, th, APP_COLOR_RED, bg_opa);
    draw_indicator(gui_obj_get_root(), "sc3", 0, y, sw, fh, APP_COLOR_RED, sc_opa);
    gui_text_t *t3 = gui_text_create(gui_obj_get_root(), "clip_top", 0, y, sw, th);
    gui_text_set(t3, str, GUI_FONT_SRC_BMP, APP_COLOR_RED, len, fh);
    gui_text_type_set(t3, font32b4, FONT_SRC_MEMADDR);
    gui_text_mode_set(t3, MULTI_LEFT);
    gui_text_set_scope(t3, 0, 0, sw, fh);
    y += th + gap;

    /* Case 4: Center region, TTF */
    int16_t crop = 40;
    draw_indicator(gui_obj_get_root(), "bg4", 0, y, sw, th, APP_COLOR_YELLOW, bg_opa);
    draw_indicator(gui_obj_get_root(), "sc4", crop, y + fh / 2, sw - crop * 2, fh, APP_COLOR_YELLOW,
                   sc_opa);
    gui_text_t *t4 = gui_text_create(gui_obj_get_root(), "clip_ctr", 0, y, sw, th);
    gui_text_set(t4, str, GUI_FONT_SRC_TTF, APP_COLOR_YELLOW, len, fh);
    gui_text_type_set(t4, font32vb4, FONT_SRC_MEMADDR);
    gui_text_mode_set(t4, MULTI_LEFT);
    gui_text_set_scope(t4, crop, fh / 2, sw - crop * 2, fh);
    y += th + gap;

    /* Case 5: Moving text, absolute scope fixed in center */
    int16_t sc_w = sw / 2;
    int16_t sc_x = (sw - sc_w) / 2;
    draw_indicator(gui_obj_get_root(), "bg5", 0, y, sw, fh, APP_COLOR_WHITE, bg_opa);
    draw_indicator(gui_obj_get_root(), "sc5", sc_x, y, sc_w, fh, APP_COLOR_WHITE, sc_opa);
    gui_text_t *t5 = gui_text_create(gui_obj_get_root(), "clip_abs", 0, y, sw, fh);
    gui_text_set(t5, str, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, len, fh);
    gui_text_type_set(t5, font32b4, FONT_SRC_MEMADDR);
    gui_text_mode_set(t5, LEFT);
    gui_text_set_scope_absolute(t5, sc_x, y, sc_w, fh);
    gui_obj_create_timer(&t5->base, 30, true, clip_abs_move_cb);
    gui_obj_start_timer(&t5->base);
    y += fh + gap;

    /* Case 6: Moving text, relative scope follows */
    gui_win_t *win6 = gui_win_create(gui_obj_get_root(), "win6", 0, y, sw, fh);
    draw_indicator(win6, "bg6", 0, 0, sw, fh, APP_COLOR_CYAN, bg_opa);
    draw_indicator(win6, "sc6", sw / 4, 0, sw / 2, fh, APP_COLOR_CYAN, sc_opa);
    gui_text_t *t6 = gui_text_create(win6, "clip_rel", 0, 0, sw, fh);
    gui_text_set(t6, str, GUI_FONT_SRC_BMP, APP_COLOR_CYAN, len, fh);
    gui_text_type_set(t6, font32b4, FONT_SRC_MEMADDR);
    gui_text_mode_set(t6, LEFT);
    gui_text_set_scope(t6, sw / 4, 0, sw / 2, fh);
    gui_obj_create_timer(&win6->base, 30, true, clip_rel_move_cb);
    gui_obj_start_timer(&win6->base);
    y += fh + gap;

    /* Case 7: Scroll text SCROLL_X + absolute scope + up-down movement */
    char *scroll_str =
        "Scrolling text with absolute scope clip, moving up and down to demonstrate the effect.";
    uint16_t scroll_len = strlen(scroll_str);
    int16_t scope_h = fh + 10;
    scroll_base_y = y;
    draw_indicator(gui_obj_get_root(), "bg7", 0, y - 5, sw, scope_h + 60, APP_COLOR_BLUE, bg_opa);
    draw_indicator(gui_obj_get_root(), "sc7", 0, y, sw, scope_h, APP_COLOR_BLUE, sc_opa);
    gui_scroll_text_t *st7 = gui_scroll_text_create(gui_obj_get_root(), "clip_scroll", 0, y, sw, fh);
    gui_scroll_text_set(st7, scroll_str, GUI_FONT_SRC_BMP, APP_COLOR_BLUE, scroll_len, fh);
    gui_scroll_text_scroll_set(st7, SCROLL_X, 0, 0, 8000, 0);
    gui_scroll_text_type_set(st7, font32b4, FONT_SRC_MEMADDR);
    gui_text_set_scope_absolute(&st7->base, 0, y, sw, scope_h);
    gui_obj_create_timer(&st7->base.base, 50, true, clip_scroll_move_cb);
    gui_obj_start_timer(&st7->base.base);
}
