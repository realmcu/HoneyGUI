/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include "tp_algo.h"
#include "app_main_dashboard.h"
#include "gui_view.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "start_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void start_engine_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *dashboard_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = start_engine_design,
};

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
    dashboard_view = gui_view_descriptor_get("dashboard_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static void press_start(void *obj, gui_event_t e, void *param)
{
    gui_img_t *img = (gui_img_t *)obj;
    gui_img_set_image_data(img, START_ENGINE_PRESS_BIN);
}

static void release_start(void *obj, gui_event_t e, void *param)
{
    gui_img_t *img = (gui_img_t *)obj;
    gui_img_set_image_data(img, START_ENGINE_RELEASE_BIN);

    touch_info_t *tp = tp_get_info();
    gui_obj_t *parent = GUI_BASE(GUI_BASE(obj)->parent);
    // If the touch point is still within the parent object, show the "gloom" image and start its timer
    if ((gui_obj_point_in_obj_rect(parent, tp->x + tp->deltaX - parent->x,
                                   tp->y + tp->deltaY - parent->y) == true))
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(o, "gloom", current_view);
        gui_obj_start_timer(GUI_BASE(o));
        gui_obj_hidden(o, false);
    }
}

static void exit_animation(void *p)
{
    static uint16_t cnt = 0;
    uint16_t cnt_max = 30;
    gui_obj_t *obj = GUI_BASE(p);
    cnt++;
    gui_img_t *img = (gui_img_t *)p;
    if (cnt <= cnt_max)
    {
        gui_img_set_opacity(img, cnt * 255 / cnt_max);
    }
    else if (cnt >= cnt_max + 5)
    {
        cnt = 0;
        gui_obj_stop_timer(obj);
        gui_view_set_animate_step(current_view, 800);
        gui_view_switch_direct(current_view, dashboard_view, SWITCH_OUT_ANIMATION_FADE,
                               SWITCH_IN_ANIMATION_FADE);
    }
}

static void start_engine_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);
    gui_img_t *gloom = gui_img_create_from_mem(parent, "gloom", STARTENGINEGLOOM_BIN, 80, 58, 0, 0);
    gui_obj_hidden(GUI_BASE(gloom), true);
    gui_img_t *engine = gui_img_create_from_mem(parent, 0, STARTENGINE_BIN, 305, 145, 0, 0);
    gui_img_set_mode(engine, IMG_SRC_OVER_MODE);
    gui_img_t *shadow = gui_img_create_from_mem(engine, 0, START_ENGINE_RELEASE_BIN, 42, 42, 0, 0);
    gui_obj_add_event_cb(shadow, press_start, GUI_EVENT_TOUCH_PRESSED, NULL);
    gui_obj_add_event_cb(shadow, release_start, GUI_EVENT_TOUCH_RELEASED, NULL);

    gui_obj_create_timer(GUI_BASE(gloom), 10, true, exit_animation);
    gui_obj_stop_timer(GUI_BASE(gloom));
}