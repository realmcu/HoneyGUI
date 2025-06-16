#include "root_image_hongkong/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include "tp_algo.h"
#include <math.h>
#include "cJSON.h"
#include "app_hongkong.h"
#include "gui_view.h"
#include "gui_list.h"
#include "gui_3d.h"

#define CURRENT_VIEW_NAME "disc_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
void disc_app(gui_view_t *view);

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = disc_app,
};

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


static void return_to_menu()
{
    gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void return_timer_cb()
{
    touch_info_t *tp = tp_get_info();
    GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, return_to_menu)
}


#define CUBE_COUNT 22
#define MAX_ROT_X_ANGLE 65.0f
#define MIN_ROT_X_ANGLE 0.0f
#define STEP_HEIGHT 2.0f

static float rot_x_angle = 0.0f;
static float rot_z_angle = 0.0f;

float shift_z[CUBE_COUNT] = {0}; // cube shift
int step_direction[CUBE_COUNT] = {1};  // cube move direction, 1 rise, -1 fall
int active_cube = 0;

static void update_disc_animation()
{
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        rot_z_angle += tp->deltaX / 5.0f;
        rot_x_angle += tp->deltaY / 5.0f;
        rot_x_angle = fmax(MIN_ROT_X_ANGLE, fmin(rot_x_angle, MAX_ROT_X_ANGLE));
    }
    else
    {
        rot_z_angle ++;
    }

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

static void disc_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 40), gui_point_4d(0, 0, 0), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, 0, -5, 90, -65 + rot_x_angle, 0, 0, 5);

}

static gui_3d_matrix_t disc_face_cb(gui_3d_t *this, size_t face_index)
{
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t transform_matrix;

    gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0, 1),
                             rot_z_angle, 1);

    transform_matrix = gui_3d_matrix_multiply(face_matrix, this->world);

    return transform_matrix;

}

static gui_3d_matrix_t disc_cube_face_cb(gui_3d_t *this, size_t face_index)
{
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t transform_matrix;

    gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0, 1),
                             rot_z_angle, 1);

    uint8_t cube_index = face_index / 6;
    if (cube_index < CUBE_COUNT)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, -shift_z[cube_index], gui_3d_point(0, 0, 0),
                                 gui_3d_vector(0, 0, 1),
                                 rot_z_angle, 1);
    }

    transform_matrix = gui_3d_matrix_multiply(face_matrix, this->world);

    return transform_matrix;

}

void disc_app(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    gui_obj_create_timer(obj, 10, true, return_timer_cb);

    gui_3d_t *disc_3d = gui_3d_create(obj, "3d-widget", DESC_DISC_BIN,
                                      GUI_3D_DRAW_FRONT_ONLY, 15, 60, 380,
                                      380);
    gui_3d_set_global_transform_cb(disc_3d, (gui_3d_global_transform_cb)disc_global_cb);
    gui_3d_set_face_transform_cb(disc_3d, (gui_3d_face_transform_cb)disc_face_cb);

    gui_3d_t *disc_cube = gui_3d_create(obj, "3d-widget", DESC_DISC_CUBE_BIN,
                                        GUI_3D_DRAW_FRONT_AND_SORT, 15, 60, 380,
                                        380);

    gui_3d_set_global_transform_cb(disc_cube, (gui_3d_global_transform_cb)disc_global_cb);
    gui_3d_set_face_transform_cb(disc_cube, (gui_3d_face_transform_cb)disc_cube_face_cb);

    gui_obj_create_timer(&(disc_3d->base), 1, true, update_disc_animation);
    gui_obj_start_timer(&(disc_3d->base));

}