/* 3d face demo start*/
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
static float rot_angle = 5.0f;

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


static void update_face_animation(void *param)
{
    touch_info_t *tp = tp_get_info();
    (void)param;

    if (tp->pressed || tp->pressing)
    {
        if (tp->x > 20 && tp->x < 390)
        {
            rot_angle += tp->deltaX / 5.0f;
        }

    }
}

static void face_global_cb(l3_model_base_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 65), 1,
                             32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, 0, 22, 65, 0, rot_angle, 0, 5);

}

static void face_app(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    l3_model_base_t *face_3d = l3_create_model(DESC_FACE_BIN, L3_DRAW_FRONT_AND_SORT, 0, 0, 410, 502);
    l3_set_global_transform(face_3d, (l3_global_transform_cb)face_global_cb);

    gui_lite3d_t *lite3d_face = gui_lite3d_create(obj, "lite3d-widget", face_3d,
                                                  0, 0, 410, 502);

    gui_obj_create_timer(GUI_BASE(lite3d_face), 10, true, update_face_animation);


}
/* 3d face demo end*/
