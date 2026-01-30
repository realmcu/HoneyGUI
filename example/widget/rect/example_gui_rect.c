/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/* gui rect example start */
#include "gui_components_init.h"
#include "gui_rect.h"
#include "gui_view.h"
#include "gui_view_instance.h"

#define RECT_VIEW_NAME "rect_round_view"
gui_rounded_rect_t *round_rect = NULL;
void round_rect_timer_cb(void *arg)
{
    gui_rect_set_position(arg, 100, 100);
    gui_fb_change();
}
void rect_click_demo(void)
{
    round_rect = gui_rect_create(gui_obj_get_root(), "round_rect", 0, 0, 200, 200, 10,
                                 gui_rgba(255, 0, 0, 255));
    gui_rect_set_style(round_rect, 200, 200, 200, 200, 10, gui_rgba(255, 0, 0, 255));
    gui_rect_on_click(round_rect, round_rect_timer_cb, NULL);
}

// Gradient rectangle examples
void example_rect_gradient(void *parent)
{
    // Example 1: Horizontal gradient (red to blue)
    gui_rounded_rect_t *rect1 = gui_rect_create(parent, "rect_h_gradient",
                                                20, 20, 200, 80, 10,
                                                gui_rgba(255, 0, 0, 255));
    gui_rect_set_linear_gradient(rect1, RECT_GRADIENT_HORIZONTAL);
    gui_rect_add_gradient_stop(rect1, 0.0f, gui_rgba(255, 0, 0, 255));     // Red
    gui_rect_add_gradient_stop(rect1, 1.0f, gui_rgba(0, 0, 255, 255));     // Blue

    // Example 2: Vertical gradient (green to yellow)
    gui_rounded_rect_t *rect2 = gui_rect_create(parent, "rect_v_gradient",
                                                240, 20, 200, 80, 10,
                                                gui_rgba(0, 255, 0, 255));
    gui_rect_set_linear_gradient(rect2, RECT_GRADIENT_VERTICAL);
    gui_rect_add_gradient_stop(rect2, 0.0f, gui_rgba(0, 255, 0, 255));     // Green
    gui_rect_add_gradient_stop(rect2, 1.0f, gui_rgba(255, 255, 0, 255));   // Yellow

    // Example 3: Diagonal gradient (top-left to bottom-right)
    gui_rounded_rect_t *rect3 = gui_rect_create(parent, "rect_d1_gradient",
                                                20, 120, 200, 120, 15,
                                                gui_rgba(255, 0, 255, 255));
    gui_rect_set_linear_gradient(rect3, RECT_GRADIENT_DIAGONAL_TL_BR);
    gui_rect_add_gradient_stop(rect3, 0.0f, gui_rgba(255, 0, 255, 255));   // Magenta
    gui_rect_add_gradient_stop(rect3, 0.5f, gui_rgba(255, 255, 255, 255)); // White
    gui_rect_add_gradient_stop(rect3, 1.0f, gui_rgba(0, 255, 255, 255));   // Cyan

    // Example 4: Diagonal gradient (top-right to bottom-left)
    gui_rounded_rect_t *rect4 = gui_rect_create(parent, "rect_d2_gradient",
                                                240, 120, 200, 120, 15,
                                                gui_rgba(255, 128, 0, 255));
    gui_rect_set_linear_gradient(rect4, RECT_GRADIENT_DIAGONAL_TR_BL);
    gui_rect_add_gradient_stop(rect4, 0.0f, gui_rgba(255, 128, 0, 255));   // Orange
    gui_rect_add_gradient_stop(rect4, 1.0f, gui_rgba(128, 0, 255, 255));   // Purple

    // Example 5: Multi-color rainbow horizontal gradient
    gui_rounded_rect_t *rect5 = gui_rect_create(parent, "rect_rainbow",
                                                20, 260, 420, 60, 20,
                                                gui_rgba(255, 0, 0, 255));
    gui_rect_set_linear_gradient(rect5, RECT_GRADIENT_HORIZONTAL);
    gui_rect_add_gradient_stop(rect5, 0.00f, gui_rgba(255, 0, 0, 255));    // Red
    gui_rect_add_gradient_stop(rect5, 0.17f, gui_rgba(255, 127, 0, 255));  // Orange
    gui_rect_add_gradient_stop(rect5, 0.33f, gui_rgba(255, 255, 0, 255));  // Yellow
    gui_rect_add_gradient_stop(rect5, 0.50f, gui_rgba(0, 255, 0, 255));    // Green
    gui_rect_add_gradient_stop(rect5, 0.67f, gui_rgba(0, 0, 255, 255));    // Blue
    gui_rect_add_gradient_stop(rect5, 0.83f, gui_rgba(75, 0, 130, 255));   // Indigo
    gui_rect_add_gradient_stop(rect5, 1.00f, gui_rgba(148, 0, 211, 255));  // Violet

    // Example 6: Gradient with transparency
    gui_rounded_rect_t *rect6 = gui_rect_create(parent, "rect_alpha_gradient",
                                                20, 340, 200, 80, 10,
                                                gui_rgba(0, 128, 255, 255));
    gui_rect_set_linear_gradient(rect6, RECT_GRADIENT_HORIZONTAL);
    gui_rect_add_gradient_stop(rect6, 0.0f, gui_rgba(0, 128, 255, 255));   // Opaque blue
    gui_rect_add_gradient_stop(rect6, 1.0f, gui_rgba(0, 128, 255, 50));    // Transparent blue
}

static void app_rect_round_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, "rect_view", SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_PRESSED);
    gui_view_set_animate_step(view, 500);
    uint8_t alpha = 200;
    gui_rect_create(gui_obj_get_root(), "bg", 0, 0, 480, 480, 0, gui_rgba(255, 200, 200,
                                                                          255));

    gui_rect_create(gui_obj_get_root(), "geometry1", 20, 20, 200, 200, 20, gui_rgba(255, 0,
                                                                                    0, alpha));
    gui_rect_create(gui_obj_get_root(), "geometry2", 50, 50, 200, 200, 20, gui_rgba(0, 255,
                                                                                    0, alpha));
    gui_rect_create(gui_obj_get_root(), "geometry3", 80, 80, 200, 200, 20, gui_rgba(0, 0,
                                                                                    255, alpha));
    gui_rect_create(gui_obj_get_root(), "geometry1", 150, 150, 200, 200, 20, gui_rgba(255, 0,
                    0, alpha));
    gui_rect_create(gui_obj_get_root(), "geometry2", 210, 210, 200, 200, 20, gui_rgba(0, 255,
                    0, alpha));

    // rect_click_demo();
    // example_rect_gradient(gui_obj_get_root());
}
static int geometry_rect_demo_init(void)
{
    gui_view_create(gui_obj_get_root(), RECT_VIEW_NAME, 0, 0, 0, 0);
    // Uncomment to show gradient examples

    return 0;
}
GUI_VIEW_INSTANCE(RECT_VIEW_NAME, false, app_rect_round_design, 0);
GUI_INIT_APP_EXPORT(geometry_rect_demo_init);
/* gui rect example end */
