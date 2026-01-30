/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include "tp_algo.h"
#include <math.h>
#include "app_main_watch.h"
#include "gui_view.h"
#include "gui_lite3d.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "digital_clock_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void digital_clock_app(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = digital_clock_app,
};

/* 3D Digital Clock */
static uint8_t *clock_fig_bin[10] = {0};
// {
//     FIG0_BIN, FIG1_BIN, FIG2_BIN, FIG3_BIN, FIG4_BIN,
//     FIG5_BIN, FIG6_BIN, FIG7_BIN, FIG8_BIN, FIG9_BIN
// };
static uint8_t current_hour0 = 0;    // Face 0
static uint8_t current_hour1 = 1;    // Face 1
static uint8_t current_minute0 = 0;  // Face 2
static uint8_t current_minute1 = 0;  // Face 3
static uint8_t current_second0 = 0;  // Face 4
static uint8_t current_second1 = 4;  // Face 5

/* Animation Variables */
static float rot_x = 0.0f;
static float rot_y = 0.0f;
static float angle = 0.0f;
static float last_time = 0.0f;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    menu_view = gui_view_descriptor_get("menu_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static void update_digital_clock_animation(void *param)
{
    // touch_info_t *tp = tp_get_info();
    gui_lite3d_t *this = (gui_lite3d_t *)param;

    angle += 0.1f;
    rot_x = 5.0f * cosf(0.5f * angle);
    rot_y = 5.0f * sinf(0.5f * angle);

    if (gui_ms_get() - last_time > 1000)
    {
        last_time = gui_ms_get();

        // Update seconds
        current_second1++;
        if (current_second1 > 9)
        {
            current_second0++;
            current_second1 = 0;
        }
        if (current_second0 > 5)
        {
            current_second0 = 0;
            current_minute1++;
        }

        // Update minutes
        if (current_minute1 > 9)
        {
            current_minute0++;
            current_minute1 = 0;
        }
        if (current_minute0 > 5)
        {
            current_minute0 = 0;
            current_hour1++;
        }

        // Update hours
        if (current_hour1 > 9)
        {
            current_hour0++;
            current_hour1 = 0;
        }
        if (current_hour0 > 2 && current_hour1 > 3)
        {
            // Reset hours if it goes beyond 23
            current_hour0 = 0;
            current_hour1 = 0;
        }

        l3_set_face_image(this->model, 4, clock_fig_bin[current_second0]);
        l3_set_face_image(this->model, 5, clock_fig_bin[current_second1]);

        l3_set_face_image(this->model, 2, clock_fig_bin[current_minute0]);
        l3_set_face_image(this->model, 3, clock_fig_bin[current_minute1]);

        l3_set_face_image(this->model, 0, clock_fig_bin[current_hour0]);
        l3_set_face_image(this->model, 1, clock_fig_bin[current_hour1]);
    }
}

static void digital_clock_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 10), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, 2, 2.5, 27, 20.0f + rot_x, rot_y, 0, 5);

}


static void digital_clock_app(gui_view_t *view)
{
    if (clock_fig_bin[0] == NULL)
    {
        uint8_t *clock_fig[10] =
        {
            FIG0_BIN, FIG1_BIN, FIG2_BIN, FIG3_BIN, FIG4_BIN,
            FIG5_BIN, FIG6_BIN, FIG7_BIN, FIG8_BIN, FIG9_BIN
        };
        memcpy(clock_fig_bin, clock_fig, sizeof(clock_fig));
    }

    gui_obj_t *obj = GUI_BASE(view);
    gui_view_switch_on_event(view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_PRESSED);

    l3_model_base_t *digital_clock_3d = l3_create_model(DESC_DIGITAL_CLOCK_BIN, L3_DRAW_FRONT_AND_BACK,
                                                        0, 0,
                                                        410, 502);
    l3_set_global_transform(digital_clock_3d, (l3_global_transform_cb)digital_clock_global_cb);
    gui_lite3d_t *lite3d_clock = gui_lite3d_create(obj, "lite3d_clock", digital_clock_3d, 0, 0, 410,
                                                   502);

    gui_obj_create_timer(&(lite3d_clock->base), 10, true, update_digital_clock_animation);

}