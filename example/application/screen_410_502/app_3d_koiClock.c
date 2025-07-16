/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "guidef.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_gray.h"
#include "gui_obj.h"
#include "string.h"
#include "gui_server.h"
#include "gui_view.h"
#include "gui_3d.h"
#include "tp_algo.h"
#include "gui_components_init.h"
#include "root_image/ui_resource.h"
#include "app_main_watch.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "koiclock_view"
#define CLOCK_RADIUS_X 60
#define CLOCK_RADIUS_Y 55
#define CLOCK_CENTER_X 0
#define CLOCK_CENTER_Y 17

/*============================================================================*
*                             Types
*============================================================================*/


/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_ui_koiclock_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = app_ui_koiclock_design,
    .on_switch_out = NULL,

    .keep = false,
};

/* Fish Movement */
//x:80 ~ -80,y:110 ~ -80
static float fish_angle = 0.0f;

static float fish0_x = 0.0f;
static float fish0_y = 0.0f;

static float fish1_x = 0;
static float fish1_y = 0;

static float fish2_x = 0;
static float fish2_y = 0;

static float fish0_rz = 0.0f;
static float fish1_rz = 0.0f;
static float fish2_rz = 0.0f;

static float fish_time = 0.0f;
const static float rotation_factor = 60.0f;
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

// static void return_to_menu()
// {
//     gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
//                            SWITCH_IN_ANIMATION_FADE);
// }

// static void return_timer_cb()
// {
//     touch_info_t *tp = tp_get_info();
//     GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, return_to_menu)
// }

static void update_koi0()
{
    static float theta0 = 0.0f; // starting angle for fish0
    float angular_velocity = 0.03f; // Adjust as needed for speed

    // Updating position for each fish using the circular path calculation
    fish0_x = CLOCK_CENTER_X + CLOCK_RADIUS_X * cosf(theta0);
    fish0_y = CLOCK_CENTER_Y + CLOCK_RADIUS_Y * sinf(theta0);
    // gui_log("fish0_x: %f, fish0_y: %f\n", fish0_x, fish0_y);

    // d +=3;
    fish0_rz = theta0 * rotation_factor + sinf(theta0 * 5) * 0.8f;;
    // gui_log("fish0_rz %f\n",fish0_rz);

    theta0 += angular_velocity;

    if (theta0 >= 2 * M_PI_F) { theta0 -= 2 * M_PI_F; }

    fish_time += 1.2f;
    fish_angle = 3 * sinf(fish_time);
}

static void update_koi1()
{
    static float theta1 = M_PI_F / 4; // starting angle for fish1
    float angular_velocity = 0.02f; // Adjust as needed for speed

    fish1_x = CLOCK_CENTER_X + CLOCK_RADIUS_X * cosf(theta1);
    fish1_y = CLOCK_CENTER_Y + CLOCK_RADIUS_Y * sinf(theta1);
    fish1_rz = theta1 * rotation_factor + sinf(theta1 * 5) * 0.8f;

    theta1 += angular_velocity;

    if (theta1 >= 2 * M_PI_F) { theta1 -= 2 * M_PI_F; }
}

static void update_koi2()
{
    static float theta2 = 2 * M_PI_F / 3; // starting angle for fish2
    float angular_velocity = 0.02f; // Adjust as needed for speed

    fish2_x = CLOCK_CENTER_X + CLOCK_RADIUS_X * cosf(theta2);
    fish2_y = CLOCK_CENTER_Y + CLOCK_RADIUS_Y * sinf(theta2);
    fish2_rz = theta2 * rotation_factor + sinf(theta2 * 5) * 0.8f;

    theta2 += angular_velocity;

    if (theta2 >= 2 * M_PI_F) { theta2 -= 2 * M_PI_F; }
}

static void fish0_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, -fish0_x, -fish0_y, 80, 0, 0, fish0_rz,
                             4);

}
static void fish0_shadow_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, 4.0f - fish0_x, 5.0f - fish0_y, 80, 0, 0, fish0_rz,
                             4);

}
static void fish1_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, -fish1_x, -fish1_y, 80, 0, 0, fish1_rz,
                             4);

}
static void fish1_shadow_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, 4.0f - fish1_x, 5.0f - fish1_y, 80, 0, 0, fish1_rz,
                             4);

}
static void fish2_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, -fish2_x, -fish2_y, 80, 0, 0, fish2_rz,
                             4);

}
static void fish2_shadow_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, 4.0f - fish2_x, 5.0f - fish2_y, 80, 0, 0, fish2_rz,
                             4);

}
static gui_3d_matrix_t fish_face_cb(gui_3d_t *this, size_t face_index/*face offset*/)
{
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t transform_matrix;

    if (face_index == 0)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0, 1),
                                 0, 1);
    }
    if (face_index == 1)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0, 1),
                                 fish_angle, 1);
    }
    if (face_index == 2)//head
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0, 1),
                                 0, 1);
    }
    if (face_index == 3)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, -5, 0), gui_3d_vector(0, 0, 1),
                                 fish_angle, 1);
    }
    if (face_index == 4)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, -8, 0), gui_3d_vector(0, 0, 1),
                                 fish_angle, 1);
    }

    transform_matrix = gui_3d_matrix_multiply(face_matrix, this->world);

    return transform_matrix;
}

static void app_ui_koiclock_design(gui_view_t *view)
{
    srand((uint32_t)gui_ms_get());
    gui_obj_t *obj = GUI_BASE(view);
    // gui_obj_create_timer(obj, 10, true, return_timer_cb);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    gui_img_create_from_mem(view, "koiclock_bg", LOTUS_BG_BIN, 0, 0, gui_get_screen_width(),
                            gui_get_screen_height());

    gui_win_t *fish0_window = gui_win_create(obj, "fish0_window", 0, 46, 48, 100);
    gui_win_t *fish1_window = gui_win_create(obj, "fish1_window", 0, 46, 48, 100);
    gui_win_t *fish2_window = gui_win_create(obj, "fish2_window", 0, 46, 48, 100);

    gui_3d_t *fish0_shadow = gui_3d_create(fish0_window, "3d-fish0", DESC_KOI_SHADOW_BIN,
                                           GUI_3D_DRAW_FRONT_ONLY,
                                           0, 0,
                                           410, 502);
    gui_3d_t *fish1_shadow = gui_3d_create(fish1_window, "3d-fish1", DESC_KOI_SHADOW_BIN,
                                           GUI_3D_DRAW_FRONT_ONLY,
                                           0, 0,
                                           410, 502);
    gui_3d_t *fish2_shadow = gui_3d_create(fish2_window, "3d-fish2", DESC_KOI_SHADOW_BIN,
                                           GUI_3D_DRAW_FRONT_ONLY,
                                           0, 0,
                                           410, 502);

    gui_3d_t *fish0 = gui_3d_create(fish0_window, "3d-fish0", DESC_KOI0_BIN, GUI_3D_DRAW_FRONT_ONLY, 0,
                                    0,
                                    0, 0);
    gui_3d_t *fish1 = gui_3d_create(fish1_window, "3d-fish1", DESC_KOI1_BIN, GUI_3D_DRAW_FRONT_ONLY, 0,
                                    0,
                                    0, 0);
    gui_3d_t *fish2 = gui_3d_create(fish2_window, "3d-fish2", DESC_KOI2_BIN, GUI_3D_DRAW_FRONT_ONLY, 0,
                                    0,
                                    0, 0);

    gui_3d_set_global_transform_cb(fish0, (gui_3d_global_transform_cb)fish0_global_cb);
    gui_3d_set_face_transform_cb(fish0, (gui_3d_face_transform_cb)fish_face_cb);
    gui_3d_set_global_transform_cb(fish0_shadow, (gui_3d_global_transform_cb)fish0_shadow_global_cb);
    gui_3d_set_face_transform_cb(fish0_shadow, (gui_3d_face_transform_cb)fish_face_cb);

    gui_3d_set_global_transform_cb(fish1, (gui_3d_global_transform_cb)fish1_global_cb);
    gui_3d_set_face_transform_cb(fish1, (gui_3d_face_transform_cb)fish_face_cb);
    gui_3d_set_global_transform_cb(fish1_shadow, (gui_3d_global_transform_cb)fish1_shadow_global_cb);
    gui_3d_set_face_transform_cb(fish1_shadow, (gui_3d_face_transform_cb)fish_face_cb);

    gui_3d_set_global_transform_cb(fish2, (gui_3d_global_transform_cb)fish2_global_cb);
    gui_3d_set_face_transform_cb(fish2, (gui_3d_face_transform_cb)fish_face_cb);
    gui_3d_set_global_transform_cb(fish2_shadow, (gui_3d_global_transform_cb)fish2_shadow_global_cb);
    gui_3d_set_face_transform_cb(fish2_shadow, (gui_3d_face_transform_cb)fish_face_cb);


    gui_img_t *clock_img = gui_img_create_from_mem(fish0_window, "img_clock", CLOCK_BIN, 26, 56, 0, 0);
    gui_img_set_mode(clock_img, IMG_SRC_OVER_MODE);

    gui_obj_create_timer(&(fish0_window->base), 17, true, update_koi0);
    gui_obj_start_timer(&(fish0_window->base));

    gui_obj_create_timer(&(fish1_window->base), 17, true, update_koi1);
    gui_obj_start_timer(&(fish1_window->base));

    gui_obj_create_timer(&(fish2_window->base), 17, true, update_koi2);
    gui_obj_start_timer(&(fish2_window->base));
}
