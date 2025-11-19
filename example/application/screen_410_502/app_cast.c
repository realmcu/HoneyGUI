/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <time.h>
#include "gui_win.h"
#include "gui_api.h"
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_video.h"
#include "gui_text.h"
#include "app_main_watch.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH  (int16_t)gui_get_width_height()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "cast_view"
#define X_TARGET  37
#define Y_TARGET  42
#define X_ORIGIN -300
#define Y_ORIGIN 50
#define SCALE_ORINGIN 2.f
#define COUNT_MAX 30

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void create_cast(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = create_cast,
    .keep = false,
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
    menu_view = gui_view_descriptor_get("menu_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static void create_cast(gui_view_t *view)
{
    gui_obj_hidden(&(gui_view_get_current()->base), true);
    gui_video_t *video = gui_video_create_from_mem(view, 0, (void *)CAST_DEMO_400_496_HEADER_H264, 5, 3,
                                                   400,
                                                   496);
    gui_video_set_state(video, GUI_VIDEO_STATE_PLAYING);
    gui_video_set_repeat_count(video, GUI_VIDEO_REPEAT_INFINITE);


    gui_view_set_animate_step(view, 1000);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_NONE_ANIMATION,
                             SWITCH_IN_NONE_ANIMATION,
                             GUI_EVENT_KB_SHORT_CLICKED);
}