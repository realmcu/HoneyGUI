/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "wallpaper_view"


/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void wallpaper_view_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *quick_view = NULL;
static const gui_view_descriptor_t *charging_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = wallpaper_view_design,
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
    quick_view = gui_view_descriptor_get("quick_view");
    charging_view = gui_view_descriptor_get("charging_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void timer_cb(void *p)
{
    GUI_UNUSED(p);
}

static void wallpaper_view_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, quick_view, SWITCH_OUT_TO_LEFT_USE_TRANSLATION, SWITCH_INIT_STATE,
                             GUI_EVENT_TOUCH_MOVE_LEFT);

    gui_view_switch_on_event(view, charging_view, SWITCH_OUT_NONE_ANIMATION,
                             SWITCH_IN_NONE_ANIMATION, GUI_EVENT_TOUCH_CLICKED);

    gui_obj_t *parent = GUI_BASE(view);
    gui_img_t *img = gui_img_create_from_mem(parent, 0, WALLPAPER_BIN, 0, 0, 0, 0);
    gui_img_set_mode(img, IMG_BYPASS_MODE);
}