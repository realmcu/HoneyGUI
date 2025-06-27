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
#define CURRENT_VIEW_NAME "prism_thick_view"

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef struct
{
    float pos_x;
    float pos_y;
    float pos_z;
} Position;

typedef struct
{
    float rot_x;
    float rot_y;
    float rot_z;
} Rotation;

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void prism_thick_app(gui_view_t *view);

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
    .on_switch_in = prism_thick_app,
};

/* Animation Variables */
static Position world_pos_raw = {0.0f, 7.0f, 30.0f};
static Position world_pos_target = {0.0f, 0.0f, 17.5f};

static Position world_pos_temp = {0.0f, 0.0f, 0.0f};
static Rotation world_rot_temp = {0.0f, 0.0f, 0.0f};

static float corrected_angle;
static float progress_percent = 0.0f;
static int enter_face_index = 0;

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


static void update_prism_thick_angle()
{
    touch_info_t *tp = tp_get_info();

    if (tp->pressed || tp->pressing)
    {
        world_rot_temp.rot_y += tp->deltaX / 5.0f;

        corrected_angle = roundf(world_rot_temp.rot_y / 60.0f) * 60.0f;
    }
    else if (world_rot_temp.rot_y != corrected_angle)
    {
        float t = 0.15f;
        float delta = corrected_angle - world_rot_temp.rot_y;
        world_rot_temp.rot_y +=  delta * t * (2.0f - t);

        if (fabsf(world_rot_temp.rot_y - corrected_angle) < 0.1f)
        {
            world_rot_temp.rot_y = corrected_angle;
        }
    }

    while (world_rot_temp.rot_y < -360) { world_rot_temp.rot_y += 360; }
    while (world_rot_temp.rot_y > 360) { world_rot_temp.rot_y -= 360; }
}


static void prism_thick_global_cb(gui_3d_t *this)
{
    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0),
                                 gui_point_4d(world_pos_temp.pos_x, world_pos_temp.pos_y, world_pos_temp.pos_z),
                                 1, 32767, 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, world_pos_temp.pos_x, world_pos_temp.pos_y,
                             world_pos_temp.pos_z,
                             0, world_rot_temp.rot_y, 0, 5);

}

static void gui_prism_thick_swap_states()
{
    Position temp = world_pos_raw;
    world_pos_raw = world_pos_target;
    world_pos_target = temp;

    progress_percent = 0.0f;
}

static void prism_thick_render_animate_cb(void *param)
{
    gui_3d_t *prism_thick_3d = (gui_3d_t *)param;

    if (progress_percent < 1.0f)
    {
        progress_percent += 0.05f;
    }
    else
    {
        progress_percent = 1.0f;
    }
    world_pos_temp.pos_x = world_pos_raw.pos_x +
                           (world_pos_target.pos_x - world_pos_raw.pos_x) * progress_percent;
    world_pos_temp.pos_y = world_pos_raw.pos_y +
                           (world_pos_target.pos_y - world_pos_raw.pos_y) * progress_percent;
    world_pos_temp.pos_z = world_pos_raw.pos_z +
                           (world_pos_target.pos_z - world_pos_raw.pos_z) * progress_percent;


    if (progress_percent == 1)
    {
        gui_prism_thick_swap_states();

        enter_face_index = (int)(world_rot_temp.rot_y / 60.0f) % 6;
        if (enter_face_index < 0)
        {
            enter_face_index += 6;
        }

        gui_obj_delete_timer(&(prism_thick_3d->base));
        gui_obj_create_timer(&(prism_thick_3d->base), 10, true, update_prism_thick_angle);
        gui_obj_start_timer(&(prism_thick_3d->base));
    }
}

static void gui_prism_thick_enter_animate(gui_3d_t *prism_thick_3d)
{
    gui_prism_thick_swap_states();

    gui_obj_create_timer(&(prism_thick_3d->base), 10, true, prism_thick_render_animate_cb);
    gui_obj_start_timer(&(prism_thick_3d->base));

}

static void prism_thick_on_face_click_cb(void *obj, gui_event_t e, void *param)
{
    GUI_ASSERT(obj != NULL);
    gui_3d_t *prism_thick = (gui_3d_t *)obj;

    gui_obj_create_timer(&(prism_thick->base), 10, true, prism_thick_render_animate_cb);
    gui_obj_start_timer(&(prism_thick->base));

}

static void prism_thick_position_init()
{
    world_pos_raw.pos_x = 0.0f;
    world_pos_raw.pos_y = 7.0f;
    world_pos_raw.pos_z = 30.0f;

    world_pos_target.pos_x = 0.0f;
    world_pos_target.pos_y = 0.0f;
    world_pos_target.pos_z = 17.5f;
}

static void prism_thick_app(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);
    gui_view_set_animate_step(view, 1000);

    gui_3d_t *prism_thick_3d = gui_3d_create(obj, "3d-widget", DESC_PRISM_THICK_BIN,
                                             GUI_3D_DRAW_FRONT_AND_SORT, 0, 0,
                                             410, 502);

    gui_3d_set_global_transform_cb(prism_thick_3d, (gui_3d_global_transform_cb)prism_thick_global_cb);
    prism_thick_position_init();

    gui_prism_thick_enter_animate(prism_thick_3d);
    gui_3d_on_click(prism_thick_3d, prism_thick_on_face_click_cb, NULL);

}