/*============================================================================*
 *                        Header Files
 *============================================================================*/
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

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "face_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void face_app(gui_view_t *view);

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
    .on_switch_in = face_app,
};

/* Animation Variables */
static float rot_angle = 0.0f;

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


static void update_face_animation()
{
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        if (tp->x > 20 && tp->x < 390)
        {
            rot_angle += tp->deltaX / 5.0f;
        }

    }
}

static void face_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 50), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, 0, 22, 50, 0, rot_angle, 0, 5);
}

static void face_app(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    gui_3d_t *face_3d = gui_3d_create(obj, "3d-widget", DESC_FACE_BIN,
                                      GUI_3D_DRAW_FRONT_AND_SORT, 15, 60,
                                      380, 380);
    gui_3d_set_global_transform_cb(face_3d, (gui_3d_global_transform_cb)face_global_cb);

    gui_obj_create_timer(&(face_3d->base), 10, true, update_face_animation);

}