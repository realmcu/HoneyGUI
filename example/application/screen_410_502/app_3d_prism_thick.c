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

#define CURRENT_VIEW_NAME "prism_thick_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
void prism_thick_app(gui_view_t *view);

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = prism_thick_app,
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

static float rot_y_angle = 0.0f;
static float target_angle;

static void update_prism_thick_animation()
{
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        rot_y_angle += tp->deltaX / 5.0f;

        target_angle = roundf(rot_y_angle / 60.0f) * 60.0f;
    }
    else if (rot_y_angle != target_angle)
    {
        rot_y_angle = rot_y_angle + (target_angle - rot_y_angle) * 0.1f;

        if (fabsf(rot_y_angle - target_angle) < 0.1f)
        {
            rot_y_angle = target_angle;
        }

    }

}


static void prism_thick_global_cb(gui_3d_t *this)
{
    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 2, 15), gui_point_4d(0, 0, 0), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, 0, 10, 45, 0, rot_y_angle, 0, 5);

}

static void gui_app_switch(gui_3d_t *this)
{

}

void prism_thick_app(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    gui_obj_create_timer(obj, 10, true, return_timer_cb);

    gui_3d_t *prism_thick_3d = gui_3d_create(obj, "3d-widget", DESC_PRISM_THICK_BIN,
                                             GUI_3D_DRAW_FRONT_AND_SORT, 5, 0,
                                             400, 400);

    gui_3d_set_global_transform_cb(prism_thick_3d, (gui_3d_global_transform_cb)prism_thick_global_cb);

    gui_3d_on_click(prism_thick_3d, (gui_event_cb_t)gui_app_switch, NULL);

    gui_obj_create_timer(&(prism_thick_3d->base), 10, true, update_prism_thick_animation);
    gui_obj_start_timer(&(prism_thick_3d->base));

}