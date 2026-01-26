/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_svg.h"
#include "gui_win.h"
#include "gui_obj.h"
#include "gui_components_init.h"

/* Simple SVG example data */
static const char svg_circle[] =
    "<svg width=\"100\" height=\"100\" xmlns=\"http://www.w3.org/2000/svg\">"
    "  <circle cx=\"50\" cy=\"50\" r=\"40\" fill=\"#FF5901\" />"
    "</svg>";

static const char svg_rect[] =
    "<svg width=\"100\" height=\"100\" xmlns=\"http://www.w3.org/2000/svg\">"
    "  <rect x=\"10\" y=\"10\" width=\"80\" height=\"80\" fill=\"#00A8E1\" rx=\"10\" />"
    "</svg>";

static const char svg_star[] =
    "<svg width=\"100\" height=\"100\" xmlns=\"http://www.w3.org/2000/svg\">"
    "  <polygon points=\"50,10 61,40 95,40 68,60 79,90 50,70 21,90 32,60 5,40 39,40\" fill=\"#FFD700\" />"
    "</svg>";

/* gui svg widget example start */
static int app_init(void)
{
    gui_svg_t *svg1 = gui_svg_create_from_mem(gui_obj_get_root(), "svg_circle",
                                              (void *)svg_circle, sizeof(svg_circle) - 1,
                                              10, 10);
    gui_svg_set_scale(svg1, 1.5f);
    gui_svg_set_opacity(svg1, 255);

    gui_svg_t *svg2 = gui_svg_create_from_mem(gui_obj_get_root(), "svg_rect",
                                              (void *)svg_rect, sizeof(svg_rect) - 1,
                                              100, 0);
    gui_svg_set_scale(svg2, 2.0f);

    gui_svg_t *svg3 = gui_svg_create_from_mem(gui_obj_get_root(), "svg_star",
                                              (void *)svg_star, sizeof(svg_star) - 1,
                                              10, 200);
    gui_svg_set_opacity(svg3, 200);

    gui_svg_create_from_file(gui_obj_get_root(), "svg_file",
                             "/pc/example/widget/svg/example.svg",
                             150, 150);

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
/* gui svg widget example end */
