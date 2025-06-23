#if 0
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

#define CURRENT_VIEW_NAME "energybox_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
void energybox_app(gui_view_t *view);

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = energybox_app,
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

// static void return_timer_cb()
// {
//     touch_info_t *tp = tp_get_info();
//     GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, return_to_menu)
// }

static float overall_rot_y = 0.0f;
static float face_shift = 0.0f;
static float shift_angle = 0.0f;

static void update_energybox_animation()
{
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        overall_rot_y += tp->deltaX / 5.0f;
    }
    else
    {
        overall_rot_y++;

        shift_angle += 0.05f;
        face_shift = 0.25f * sinf(shift_angle) + 0.25f;  // 0 ~ 0.5f

    }
}

static void energybox_global_cb(gui_3d_t *this)
{
    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 5, 20), gui_point_4d(0, 0, 0), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, 0, 12, 40, 0, 45 + overall_rot_y, 0, 5);

}
static gui_3d_matrix_t energybox_face_cb(gui_3d_t *this, size_t face_index)
{
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t transform_matrix;

    if (face_index == 0)
    {
        gui_3d_calculator_matrix(&face_matrix, -face_shift, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0,
                                 1), 0, 1);
    }
    else if (face_index == 1)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, face_shift, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0,
                                 1), 0, 1);
    }
    else if (face_index == 2)
    {
        gui_3d_calculator_matrix(&face_matrix, face_shift, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0,
                                 1), 0, 1);
    }
    else if (face_index == 3)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, -face_shift, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0,
                                 1), 0, 1);
    }
    else if (face_index == 4)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, -face_shift, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0,
                                 1), 0, 1);
    }
    else if (face_index == 5)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, face_shift, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0,
                                 1), 0, 1);
    }

    transform_matrix = gui_3d_matrix_multiply(face_matrix, this->world);

    return transform_matrix;

}

void energybox_app(gui_view_t *view)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *obj = GUI_BASE(view);
    // gui_obj_create_timer(obj, 10, true, return_timer_cb);

    gui_3d_t *energybox_3d = gui_3d_create(obj, "3d-widget", DESC_ENERGYBOX_BIN,
                                           GUI_3D_DRAW_FRONT_AND_BACK, 5, 0,
                                           400, 400);

    gui_3d_set_global_transform_cb(energybox_3d, (gui_3d_global_transform_cb)energybox_global_cb);
    gui_3d_set_face_transform_cb(energybox_3d, (gui_3d_face_transform_cb)energybox_face_cb);


    gui_obj_create_timer(&(energybox_3d->base), 10, true, update_energybox_animation);
    gui_obj_start_timer(&(energybox_3d->base));

}
#endif
