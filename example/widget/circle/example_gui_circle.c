/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/* gui circle example start */
#include "gui_components_init.h"
#include "gui_circle.h"
static void gui_circle_cb(gui_obj_t *obj)
{
    gui_log("gui_circle_cb: %s\n", obj->name);
}

// Gradient circle examples
void example_circle_gradient(void *parent)
{
    // Example 1: Radial gradient (center to edge, white to blue)
    gui_circle_t *circle1 = gui_circle_create(parent, "circle_radial",
                                              120, 120, 80,
                                              gui_rgba(255, 255, 255, 255));
    gui_circle_set_radial_gradient(circle1);
    gui_circle_add_gradient_stop(circle1, 0.0f, gui_rgba(255, 255, 255, 255)); // White center
    gui_circle_add_gradient_stop(circle1, 1.0f, gui_rgba(0, 0, 255, 255));     // Blue edge

    // Example 2: Radial gradient with multiple colors (sun effect)
    gui_circle_t *circle2 = gui_circle_create(parent, "circle_sun",
                                              320, 120, 80,
                                              gui_rgba(255, 255, 0, 255));
    gui_circle_set_radial_gradient(circle2);
    gui_circle_add_gradient_stop(circle2, 0.0f, gui_rgba(255, 255, 255, 255)); // White center
    gui_circle_add_gradient_stop(circle2, 0.3f, gui_rgba(255, 255, 0, 255));   // Yellow
    gui_circle_add_gradient_stop(circle2, 0.7f, gui_rgba(255, 128, 0, 255));   // Orange
    gui_circle_add_gradient_stop(circle2, 1.0f, gui_rgba(255, 0, 0, 255));     // Red edge

    // Example 3: Angular gradient (rainbow around circle)
    gui_circle_t *circle3 = gui_circle_create(parent, "circle_angular",
                                              120, 320, 80,
                                              gui_rgba(255, 0, 0, 255));
    gui_circle_set_angular_gradient(circle3, 0, 360);
    gui_circle_add_gradient_stop(circle3, 0.00f, gui_rgba(255, 0, 0, 255));    // Red
    gui_circle_add_gradient_stop(circle3, 0.17f, gui_rgba(255, 127, 0, 255));  // Orange
    gui_circle_add_gradient_stop(circle3, 0.33f, gui_rgba(255, 255, 0, 255));  // Yellow
    gui_circle_add_gradient_stop(circle3, 0.50f, gui_rgba(0, 255, 0, 255));    // Green
    gui_circle_add_gradient_stop(circle3, 0.67f, gui_rgba(0, 0, 255, 255));    // Blue
    gui_circle_add_gradient_stop(circle3, 0.83f, gui_rgba(75, 0, 130, 255));   // Indigo
    gui_circle_add_gradient_stop(circle3, 1.00f, gui_rgba(255, 0, 0, 255));    // Red (seamless)

    // Example 4: Radial gradient with transparency (glow effect)
    gui_circle_t *circle4 = gui_circle_create(parent, "circle_glow",
                                              320, 320, 80,
                                              gui_rgba(0, 255, 0, 255));
    gui_circle_set_radial_gradient(circle4);
    gui_circle_add_gradient_stop(circle4, 0.0f, gui_rgba(0, 255, 0, 255));     // Opaque green
    gui_circle_add_gradient_stop(circle4, 0.5f, gui_rgba(0, 255, 0, 180));     // Semi-transparent
    gui_circle_add_gradient_stop(circle4, 1.0f, gui_rgba(0, 255, 0, 50));      // Nearly transparent
}

static int geometry_circle_demo_init(void)
{
    const uint32_t W = 480, H = 480;
    const uint32_t big_d   = 368;
    const uint32_t small_d = 66;
    const uint32_t margin  = 36;

    gui_circle_t *big_circle = gui_circle_create(gui_obj_get_root(), "big_circle", W / 2,
                                                 H / 2, big_d / 2, gui_rgba(0, 255,
                                                                            0, 255));

    gui_circle_create(gui_obj_get_root(), "top_left", margin + small_d / 2, margin + small_d / 2,
                      small_d / 2,
                      gui_rgba(255, 0, 0, 255));


    gui_circle_create(gui_obj_get_root(), "top_right", W - margin - small_d / 2,
                      margin + small_d / 2, small_d / 2,
                      gui_rgba(0, 255, 0, 255));


    gui_circle_create(gui_obj_get_root(), "bottom_left", margin + small_d / 2,
                      H - margin - small_d / 2, small_d / 2,
                      gui_rgba(0, 0, 255, 255));


    gui_circle_create(gui_obj_get_root(), "bottom_right", W - margin - small_d / 2,
                      H - margin - small_d / 2, small_d / 2,
                      gui_rgba(0, 0, 255, 150));
    gui_obj_add_event_cb(big_circle, (gui_event_cb_t)gui_circle_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    // Uncomment to show gradient examples
    // example_circle_gradient(gui_obj_get_root());
    return 0;
}

GUI_INIT_APP_EXPORT(geometry_circle_demo_init);
/* gui circle example end */
