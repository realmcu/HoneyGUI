/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "oobe_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void oobe_view_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = oobe_view_design,
};

static const gui_view_descriptor_t *support_view = NULL;

char device_name[16] = "[JBL Buds]";
static char welcome_str[36] = "Welcome to your ";
static char warning_str[44] = "Get to know your headphones in a\nfew steps.";

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
    support_view = gui_view_descriptor_get("support_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void click_button(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_view_set_animate_step(current_view, 10);
    gui_view_switch_direct(current_view, support_view->name, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
}

static void wait_cb(void *obj)
{
    gui_set_bg_color(SCREEN_BG_DARK);
    gui_color_t font_color = FG_1_DARK;
    gui_obj_t *parent = GUI_BASE(obj);
    gui_obj_child_free(parent);

    gui_win_t *win = gui_win_create(parent, 0, 0, 0, 0, 0);
    sprintf(welcome_str + 16, "%s", device_name);
    gui_text_t *t = gui_text_create(win, 0, 16, 28, 288, 60);
    gui_text_set(t, (void *)welcome_str, GUI_FONT_SRC_BMP, font_color, strlen(welcome_str), 30);
    gui_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t, MULTI_LEFT);
    t = gui_text_create(win, 0, 16, 90, 288, 60);
    gui_text_set(t, (void *)warning_str, GUI_FONT_SRC_BMP, font_color, strlen(warning_str), 20);
    gui_text_type_set(t, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t, MULTI_LEFT);

    gui_img_t *img = gui_img_create_from_mem(win, 0, BUTTON_GET_START_BIN, 87, 139, 0, 0);
    gui_img_set_mode(img, IMG_BYPASS_MODE);
    gui_obj_add_event_cb(img, click_button, GUI_EVENT_TOUCH_CLICKED, NULL);
}

static void oobe_view_design(gui_view_t *view)
{
    gui_set_bg_color(gui_rgb(0x0A, 0x10, 0x1A));

    gui_obj_t *parent = GUI_BASE(view);
    gui_img_t *img = gui_img_create_from_mem(parent, 0, JBL_BIN, 84, 60, 0, 0);
    gui_img_set_mode(img, IMG_BYPASS_MODE);

    gui_obj_create_timer(parent, 2000, false, wait_cb);
    gui_obj_start_timer(parent);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void set_device_name(const char *name)
{
    sprintf(device_name, "[%s]", name);
}