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

#define CURRENT_VIEW_NAME "notification_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
void notification_app(gui_view_t *view);

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = notification_app,
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

static float shift_y = 0.0f;
static float face2_rot_x = 0.0f;

static void update_notification_animation()
{
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        shift_y += tp->deltaY / 100.0f;
        shift_y = fmaxf(-10, fminf(2, shift_y)); // Limit shift_y to [-8, 2]

        face2_rot_x += tp->deltaY * 3.0f;

        if (face2_rot_x < -60.0f)
        {
            face2_rot_x = -60.0f;
        }
        else if (face2_rot_x > 0.0f)
        {
            face2_rot_x = 0.0f;
        }

    }

}

static void notification_global_cb(gui_3d_t *this)
{
    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 2), gui_point_4d(0, 0, 0), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, 0, 10, 25, 0, 0, 0, 5);

}
static gui_3d_matrix_t notification_face_cb(gui_3d_t *this, size_t face_index)
{
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t transform_matrix;


    gui_3d_calculator_matrix(&face_matrix, 0, shift_y, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0, 1),
                             0, 1);


    transform_matrix = gui_3d_matrix_multiply(face_matrix, this->world);

    return transform_matrix;

}
void notification_app(gui_view_t *view)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *obj = GUI_BASE(view);
    // gui_obj_create_timer(obj, 10, true, return_timer_cb);

    gui_img_t *FlapOpen = gui_img_create_from_mem(obj, "FlapOpen", FLAPOPEN_BIN, 37, 345, 0, 0);

    gui_img_t *EnvelopeBg = gui_img_create_from_mem(obj, "EnvelopeBg", ENVELOPEBG_BIN, 37, 410, 0, 0);

    gui_img_t *Envelope = gui_img_create_from_mem(obj, "Envelope", ENVELOPE_BIN, 37, 410, 0, 0);


    gui_3d_t *notification_3d = gui_3d_create(obj, "3d-widget", DESC_NOTIFICATION_BIN,
                                              GUI_3D_DRAW_FRONT_AND_SORT, 5, 0,
                                              400, 400);

    gui_3d_set_global_transform_cb(notification_3d, (gui_3d_global_transform_cb)notification_global_cb);
    gui_3d_set_face_transform_cb(notification_3d, (gui_3d_face_transform_cb)notification_face_cb);


    gui_obj_create_timer(&(notification_3d->base), 10, true, update_notification_animation);
    gui_obj_start_timer(&(notification_3d->base));

}
#endif
