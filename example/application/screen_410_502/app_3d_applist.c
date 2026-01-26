/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/* 3d applist demo start*/
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
#define CURRENT_VIEW_NAME "applist_view"
#define APP_NUM 6

/*============================================================================*
*                             Types
*============================================================================*/
typedef struct
{
    int16_t pos_x;
    int16_t pos_y;
} Position;

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void applist_app(gui_view_t *view);

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
    .on_switch_in = applist_app,
};

/* 3D Applications List */
static gui_lite3d_t *app_3d_list[APP_NUM];  // Static array
static const Position app_positions[APP_NUM] =
{
    {0, 50},
    {100, 50},
    {200, 50},
    {0, 200},
    {100, 200},
    {200, 200}
};

/* Animation Variables */
static float rot_y_angle = 0.0f;   // Current angle
static float velocity = 0.0f;       // Angular velocity
static bool isAnimating = false;    // Whether animation is ongoing
static const float amplitude = 30.0f;     // Initial amplitude
static const float damping =
    0.95f;        // Damping coefficient (between 0.9~0.99, higher values result in longer oscillation)
static const float frequency =
    0.2f;      // Oscillation frequency (higher values result in faster oscillation)

static bool click_amplify = false;
static float click_on_rot_y = 0.0f;
static float click_on_shift_z = 0.0f;

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


static void update_applist_animation(void *param)
{
    touch_info_t *tp = tp_get_info();
    (void)param;

    if (tp->pressed)
    {
        isAnimating = true;        // Start animation
        rot_y_angle = 0.0f;        // Reset angle
        velocity = amplitude * frequency; // Initial velocity
    }

    if (isAnimating)
    {
        // Harmonic motion + damping decay
        float acceleration = -frequency * frequency *
                             rot_y_angle; // Acceleration (towards equilibrium point)
        velocity += acceleration;  // Update velocity
        velocity *= damping;      // Apply damping decay
        rot_y_angle += velocity;   // Update angle

        // Stop animation when oscillation is small enough
        if (fabsf(rot_y_angle) < 0.1f && fabsf(velocity) < 0.1f)
        {
            rot_y_angle = 0.0f;    // Ensure complete stop
            velocity = 0.0f;
            isAnimating = false;
        }
    }
}

static void applist_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 15), 1,
                             32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, 0, 0, 15, 0, 0, 0, 5);
}

static l3_4x4_matrix_t applist_face_cb(l3_model_base_t *this, size_t face_index)
{
    (void)face_index;
    l3_4x4_matrix_t face_matrix;
    l3_4x4_matrix_t transform_matrix;

    l3_calculator_4x4_matrix(&face_matrix, 0, 0, 0, l3_4d_point(0, 0, 0), l3_4d_vector(0, 1, 0),
                             rot_y_angle, 1);

    l3_4x4_matrix_mul(&this->world, &face_matrix, &transform_matrix);

    return transform_matrix;
}

static void app_click_on_cb(l3_model_base_t *this)
{
    if (click_on_rot_y < 720.f)
    {
        click_on_rot_y += 10.0f;
    }
    if (click_on_shift_z < 5.0f)
    {
        click_on_shift_z += 0.2f;
    }
    this->x = 0;
    this->y = 46;
    this->viewPortWidth = 410;
    this->viewPortHeight = 410;
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 15), 1,
                             32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, 0, 0, 15.0f - click_on_shift_z, 0, click_on_rot_y, 0, 5);
}

static int get_app_index(gui_lite3d_t *target)
{
    for (int i = 0; i < APP_NUM; i++)
    {
        if (app_3d_list[i] == target)
        {
            return i;
        }
    }
    return -1;
}

static void gui_app_switch(gui_lite3d_t *this)
{
    gui_log("clicked app: %s\n", this->base.name);

    if (!click_amplify)
    {
        click_amplify = true;
        for (int i = 0; i < APP_NUM; i++)
        {
            gui_lite3d_t *app = app_3d_list[i];

            if (app != this)
            {
                app->base.not_show = true;
            }
        }

        click_on_rot_y = 0.0f;
        click_on_shift_z = 0.0f;
        l3_set_global_transform(this->model, (l3_global_transform_cb)app_click_on_cb);
    }
    else
    {
        click_amplify = false;

        int clicked_index = get_app_index(this);
        if (clicked_index == -1)
        {
            gui_log("Error: Target app not found!");
            return;
        }
        this->model->x = app_positions[clicked_index].pos_x;
        this->model->y = app_positions[clicked_index].pos_y;
        this->model->viewPortWidth = 200;
        this->model->viewPortHeight = 200;
        l3_set_global_transform(this->model, (l3_global_transform_cb)applist_global_cb);

        for (int i = 0; i < APP_NUM; i++)
        {
            app_3d_list[i]->base.not_show = false;
        }
    }

}

static void applist_app(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    gui_view_switch_on_event(view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    click_amplify = false;

    // App0
    l3_model_base_t *app0 = l3_create_model(DESC_APP_BIN, L3_DRAW_FRONT_ONLY,
                                            app_positions[0].pos_x, app_positions[0].pos_y, 200, 200);
    app_3d_list[0] = gui_lite3d_create(obj, "app0", app0, 0, 0, 0, 0);

    // App1
    l3_model_base_t *app1 = l3_create_model(DESC_APP_BIN, L3_DRAW_FRONT_ONLY,
                                            app_positions[1].pos_x, app_positions[1].pos_y, 200, 200);
    l3_set_face_image(app1, 5, FILM_BIN);
    app_3d_list[1] = gui_lite3d_create(obj, "app1", app1, 0, 0, 0, 0);

    // App2
    l3_model_base_t *app2 = l3_create_model(DESC_APP_BIN, L3_DRAW_FRONT_ONLY,
                                            app_positions[2].pos_x, app_positions[2].pos_y, 200, 200);
    l3_set_face_image(app2, 5, MUSIC_BIN);
    app_3d_list[2] = gui_lite3d_create(obj, "app2", app2, 0, 0, 0, 0);

    // App3
    l3_model_base_t *app3 = l3_create_model(DESC_APP_BIN, L3_DRAW_FRONT_ONLY,
                                            app_positions[3].pos_x, app_positions[3].pos_y, 200, 200);
    l3_set_face_image(app3, 5, MAIL_BIN);
    app_3d_list[3] = gui_lite3d_create(obj, "app3", app3, 0, 0, 0, 0);

    // App4
    l3_model_base_t *app4 = l3_create_model(DESC_APP_BIN, L3_DRAW_FRONT_ONLY,
                                            app_positions[4].pos_x, app_positions[4].pos_y, 200, 200);
    l3_set_face_image(app4, 5, VOICE_BIN);
    app_3d_list[4] = gui_lite3d_create(obj, "app4", app4, 0, 0, 0, 0);

    // App5
    l3_model_base_t *app5 = l3_create_model(DESC_APP_BIN, L3_DRAW_FRONT_ONLY,
                                            app_positions[5].pos_x, app_positions[5].pos_y, 200, 200);
    l3_set_face_image(app5, 5, WECHAT_BIN);
    app_3d_list[5] = gui_lite3d_create(obj, "app5", app5, 0, 0, 0, 0);

    for (int i = 0; i < APP_NUM; i++)
    {
        l3_set_global_transform(app_3d_list[i]->model, (l3_global_transform_cb)applist_global_cb);
        l3_set_face_transform(app_3d_list[i]->model, (l3_face_transform_cb)applist_face_cb);

        gui_lite3d_on_click(app_3d_list[i], (gui_event_cb_t)gui_app_switch, app_3d_list[i]);
    }


    gui_obj_create_timer(GUI_BASE(app_3d_list[0]), 10, true, update_applist_animation);

}
/* 3d applist demo end*/
