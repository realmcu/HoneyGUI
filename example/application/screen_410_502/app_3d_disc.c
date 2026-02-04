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
#define CURRENT_VIEW_NAME "disc_view"
#define CUBE_COUNT 22
#define MAX_ROT_X_ANGLE 65.0f
#define MIN_ROT_X_ANGLE 0.0f
#define STEP_HEIGHT 2.0f

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void disc_app(gui_view_t *view);

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
    .on_switch_in = disc_app,
};

/* Animation Variables */
static float rot_x_angle = 0.0f;
static float rot_z_angle = 0.0f;

static float shift_z[CUBE_COUNT] = {0}; // cube shift
static int step_direction[CUBE_COUNT] = {1};  // cube move direction, 1 rise, -1 fall
static int active_cube = 0;

/* Music */
static bool is_playing = false;

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


static void update_disc_animation(void *param)
{
    (void)param;

    gui_lite3d_t *lite3d_disc = (gui_lite3d_t *)param;
    touch_info_t *tp = tp_get_info();

    if ((tp->pressed || tp->pressing) && tp->y < (lite3d_disc->base.y + lite3d_disc->base.h))
    {
        rot_z_angle -= tp->deltaX / 5.0f;
        rot_x_angle += tp->deltaY / 5.0f;
        rot_x_angle = fmaxf(MIN_ROT_X_ANGLE, fminf(rot_x_angle, MAX_ROT_X_ANGLE));
    }
    else if (is_playing)
    {
        rot_z_angle --;

        // update all cube states
        for (int i = 0; i < CUBE_COUNT; i++)
        {
            if (step_direction[i] == 1 && shift_z[i] >= STEP_HEIGHT)
            {
                // change direction to fall
                step_direction[i] = -1;
                if (i == active_cube)
                {
                    // activate the next cube to rise
                    active_cube = (active_cube + 1) % CUBE_COUNT;
                    step_direction[active_cube] = 1;
                }
            }

            // Update the current cube height
            shift_z[i] += step_direction[i] * 0.4f;
            if (shift_z[i] < 0.0f)
            {
                shift_z[i] = 0.0f;
            }
        }
    }
}

static void disc_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 50), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, 0, -5, 50, -65 + rot_x_angle, 0, 0, 5);

}

static l3_4x4_matrix_t disc_face_cb(l3_model_base_t *this, size_t face_index)
{
    (void)face_index;
    l3_4x4_matrix_t face_matrix;
    l3_4x4_matrix_t transform_matrix;

    l3_calculator_4x4_matrix(&face_matrix, 0, 0, 0, l3_4d_point(0, 0, 0), l3_4d_vector(0, 0, 1),
                             rot_z_angle, 1);

    l3_4x4_matrix_mul(&this->world, &face_matrix, &transform_matrix);

    return transform_matrix;

}

static l3_4x4_matrix_t disc_cube_face_cb(l3_model_base_t *this, size_t face_index)
{
    l3_4x4_matrix_t face_matrix;
    l3_4x4_matrix_t transform_matrix;

    l3_calculator_4x4_matrix(&face_matrix, 0, 0, 0, l3_4d_point(0, 0, 0), l3_4d_vector(0, 0, 1),
                             rot_z_angle, 1);

    uint8_t cube_index = face_index / 6;
    if (cube_index < CUBE_COUNT)
    {
        l3_calculator_4x4_matrix(&face_matrix, 0, 0, -shift_z[cube_index], l3_4d_point(0, 0, 0),
                                 l3_4d_vector(0, 0, 1),
                                 rot_z_angle, 1);
    }

    l3_4x4_matrix_mul(&this->world, &face_matrix, &transform_matrix);

    return transform_matrix;

}

static void switch_to_origin_img(void *param)
{
    (void)param;

    gui_img_t *img = (gui_img_t *)param;
    if (strcmp(img->base.name, "music_backward") == 0)
    {
        gui_img_set_src(img, MUSIC_BACKWARD_BIN, IMG_SRC_MEMADDR);
    }
    else
    {
        gui_img_set_src(img, MUSIC_FORWARD_BIN, IMG_SRC_MEMADDR);
    }
}

static void switch_to_highlight_img(void *obj, gui_event_t *e)
{
    (void)e;

    gui_img_t *img = (gui_img_t *)obj;
    if (strcmp(img->base.name, "music_backward") == 0)
    {
        gui_img_set_src(img, MUSIC_BACKWARD_HIGHLIGHT_BIN, IMG_SRC_MEMADDR);
    }
    else
    {
        gui_img_set_src(img, MUSIC_FORWARD_HIGHLIGHT_BIN, IMG_SRC_MEMADDR);
    }

    gui_obj_create_timer(GUI_BASE(img), 200, false, switch_to_origin_img);
    gui_obj_start_timer(GUI_BASE(img));
}

static void switch_to_play_pause_img(void *obj, gui_event_t *e)
{
    (void)e;

    gui_img_t *img = (gui_img_t *)obj;

    is_playing = !is_playing;
    if (is_playing == false)
    {
        gui_img_set_src(img, MUSIC_PAUSE_BIN, IMG_SRC_MEMADDR);
    }
    else
    {
        gui_img_set_src(img, MUSIC_PLAY_BIN, IMG_SRC_MEMADDR);
    }
}

static void disc_app(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    gui_view_switch_on_event(view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_PRESSED);

    is_playing = false;
    active_cube = 0;
    for (int i = 0; i < CUBE_COUNT; i++)
    {
        shift_z[i] = 0.0f;
        step_direction[i] = 0;
    }
    step_direction[0] = 1;

    // Disc
    l3_model_base_t *disc_3d = l3_create_model(DESC_DISC_BIN, L3_DRAW_FRONT_ONLY, 15, 0, 380, 380);
    l3_set_global_transform(disc_3d, (l3_global_transform_cb)disc_global_cb);
    l3_set_face_transform(disc_3d, (l3_face_transform_cb)disc_face_cb);
    gui_lite3d_t *lite3d_disc = gui_lite3d_create(obj, "lite3d_disc", disc_3d, 15, 0, 380, 380);

    // Cube
    l3_model_base_t *disc_cube = l3_create_model(DESC_DISC_CUBE_BIN, L3_DRAW_FRONT_AND_SORT, 15, 0, 380,
                                                 380);
    l3_set_global_transform(disc_cube, (l3_global_transform_cb)disc_global_cb);
    l3_set_face_transform(disc_cube, (l3_face_transform_cb)disc_cube_face_cb);
    gui_lite3d_create(obj, "lite3d_disc_cube", disc_cube, 0, 0, 0, 0);

    gui_obj_create_timer(GUI_BASE(lite3d_disc), 10, true, update_disc_animation);

    // Music control
    gui_win_t *win_music = gui_win_create(obj, "win_music", 0, 380, 0, 0);

    gui_img_t *backward_img = gui_img_create_from_mem(win_music, "music_backward", MUSIC_BACKWARD_BIN,
                                                      40, 0, 0, 0);
    gui_obj_add_event_cb(backward_img, (gui_event_cb_t)switch_to_highlight_img, GUI_EVENT_TOUCH_CLICKED,
                         NULL);

    gui_img_t *play_pause_img = gui_img_create_from_mem(win_music, "music_play_pause", MUSIC_PAUSE_BIN,
                                                        165, 0, 0, 0);
    gui_obj_add_event_cb(play_pause_img, (gui_event_cb_t)switch_to_play_pause_img,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *forward_img = gui_img_create_from_mem(win_music, "music_forward", MUSIC_FORWARD_BIN, 290,
                                                     0, 0, 0);
    gui_obj_add_event_cb(forward_img, (gui_event_cb_t)switch_to_highlight_img, GUI_EVENT_TOUCH_CLICKED,
                         NULL);

}