/* view yellow start*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>
#include "tiger_blue.txt"
#include "tiger_grey.txt"
#include "tiger_laven.txt"
#include "tiger_lime.txt"
#include "tiger_turk.txt"
#include "tiger_white.txt"
#include "tiger_yellow.txt"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_view.h"

#define CURRENT_VIEW_NAME "yellow_view"


static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *blue_view = NULL;
const static gui_view_descriptor_t *white_view = NULL;
static void app_ui_view_yellow_design(gui_view_t *view);

static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .design_cb = app_ui_view_yellow_design,
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
    blue_view = gui_view_descriptor_get("blue_view");
    white_view = gui_view_descriptor_get("white_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void app_ui_view_yellow_design(gui_view_t *view)
{
    gui_img_t *img = gui_img_create_from_mem(view, "img", (void *)_actiger_yellow, 0, 0, 0,
                                             0);
    gui_img_scale(img, 1.875f, 2.034f);
    gui_img_set_mode(img, IMG_BYPASS_MODE);

    gui_view_switch_on_event(view, blue_view, VIEW_REDUCTION, VIEW_REDUCTION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);

    gui_view_switch_on_event(view, white_view, VIEW_ROTATE, VIEW_ROTATE, GUI_EVENT_TOUCH_MOVE_LEFT);

    gui_view_switch_on_event(view, white_view, VIEW_STILL, VIEW_TRANSPLATION,
                             GUI_EVENT_TOUCH_MOVE_UP);

    gui_view_switch_on_event(view, blue_view, VIEW_ROTATE, VIEW_ROTATE,
                             GUI_EVENT_TOUCH_MOVE_DOWN);

    gui_view_switch_on_event(view, blue_view, VIEW_ANIMATION_8, VIEW_ANIMATION_1,
                             GUI_EVENT_TOUCH_CLICKED);
    gui_view_switch_on_event(view, white_view, VIEW_ANIMATION_8, VIEW_ANIMATION_1,
                             GUI_EVENT_KB_SHORT_CLICKED);
}
/* view yellow end*/






