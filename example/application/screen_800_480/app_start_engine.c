/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include "tp_algo.h"
#include "app_main_dashboard.h"
#include "gui_view.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "start_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void start_engine_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, start_engine_design, NULL);

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void press_start(void *obj, gui_event_t *e)
{
    (void)e;

    gui_img_t *img = (gui_img_t *)obj;
    gui_img_set_src(img, START_ENGINE_PRESS_BIN, IMG_SRC_MEMADDR);
}

static void release_start(void *obj, gui_event_t *e)
{
    (void)e;

    gui_img_t *img = (gui_img_t *)obj;
    gui_img_set_src(img, START_ENGINE_RELEASE_BIN, IMG_SRC_MEMADDR);

    touch_info_t *tp = tp_get_info();
    gui_obj_t *parent = GUI_BASE(GUI_BASE(obj)->parent);
    // If the touch point is still within the parent object, show the "gloom" image and start its timer
    if ((gui_obj_point_in_obj_rect(parent, tp->x + tp->deltaX - parent->x,
                                   tp->y + tp->deltaY - parent->y) == true))
    {
        gui_obj_t *o = gui_obj_get_handle((void *)current_view_line_33, "gloom");
        gui_obj_start_timer(GUI_BASE(o));
        gui_obj_hidden(o, false);
    }
}

static void exit_animation(void *p)
{
    static uint16_t cnt = 0;
    uint16_t cnt_max = 30;
    gui_obj_t *obj = GUI_BASE(p);
    cnt++;
    gui_img_t *img = (gui_img_t *)p;
    if (cnt <= cnt_max)
    {
        gui_img_set_opacity(img, cnt * 255 / cnt_max);
    }
    else if (cnt >= cnt_max + 5)
    {
        cnt = 0;
        gui_obj_stop_timer(obj);
        gui_view_set_animate_step(current_view_line_33, 800);
        gui_view_switch_direct(current_view_line_33, "dashboard_view", SWITCH_OUT_ANIMATION_FADE,
                               SWITCH_IN_ANIMATION_FADE);
    }
}

static void start_engine_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);
    gui_img_t *gloom = gui_img_create_from_mem(parent, "gloom", STARTENGINEGLOOM_BIN, 80, 58, 0, 0);
    gui_obj_hidden(GUI_BASE(gloom), true);
    gui_img_t *engine = gui_img_create_from_mem(parent, 0, STARTENGINE_BIN, 305, 145, 0, 0);
    gui_img_set_mode(engine, IMG_SRC_OVER_MODE);
    gui_img_t *shadow = gui_img_create_from_mem(engine, 0, START_ENGINE_RELEASE_BIN, 42, 42, 0, 0);
    gui_obj_add_event_cb(shadow, press_start, GUI_EVENT_TOUCH_PRESSED, NULL);
    gui_obj_add_event_cb(shadow, release_start, GUI_EVENT_TOUCH_RELEASED, NULL);

    gui_obj_create_timer(GUI_BASE(gloom), 10, true, exit_animation);
    gui_obj_stop_timer(GUI_BASE(gloom));
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void app_dashboard_start_view_to_main(void)
{
    if (current_view_line_33 == NULL)
    {
        gui_log("Error: start_view not initialized\n");
        return;
    }

    // Show gloom animation and switch to dashboard view
    gui_obj_t *o = gui_obj_get_handle((void *)current_view_line_33, "gloom");
    if (o != NULL)
    {
        gui_obj_start_timer(GUI_BASE(o));
        gui_obj_hidden(o, false);
    }
    gui_log("Switching from start_view to dashboard_view\n");
}