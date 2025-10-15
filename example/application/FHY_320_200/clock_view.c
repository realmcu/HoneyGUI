/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "clock_view"

#define TAB_START_POS_X 17
#define TAB_START_POS_Y 60
#define TAB_STOP_POS_X  224
#define TAB_SIZE        80

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void clock_view_design(gui_view_t *view);
static void clock3_design(gui_view_t *view);
static void clock4_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *lock_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = clock_view_design,
};

gui_img_t *vec = NULL;
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
    lock_view = gui_view_descriptor_get("lock_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void time_update(void *obj)
{
    GUI_UNUSED(obj);
    static uint8_t cnt = 0;
    cnt++;
    switch (cnt % 4)
    {
    case 0:
        gui_img_a8_fix_bg(vec, GUI_COLOR_ARGB8888(255, 0xFF, 0x59, 0x01));
        break;
    case 1:
        gui_img_a8_fix_bg(vec, GUI_COLOR_ARGB8888(255, 0x8F, 0xBF, 0x80));
        break;
    case 2:
        gui_img_a8_fix_bg(vec, GUI_COLOR_ARGB8888(255, 0x7C, 0x9E, 0xFF));
        break;
    case 3:
        gui_img_a8_fix_bg(vec, GUI_COLOR_ARGB8888(255, 0xB4, 0x90, 0xFF));
        break;

    default:
        break;
    }

    if (!timeinfo)
    {
        return;
    }
    switch (clock_style)
    {
    case 1:
        break;
    case 2:
        break;
    case 3:
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(hour, "hour", current_view);
            gui_img_rotation((gui_img_t *)hour, (timeinfo->tm_hour % 12) * 30.0f + timeinfo->tm_min / 2.f);
            GUI_WIDGET_POINTER_BY_NAME_ROOT(min, "min", current_view);
            gui_img_rotation((gui_img_t *)min, timeinfo->tm_min * 6.0f);
        }
        break;
    case 4:
        break;
    case 5:
        break;
    default:
        break;
    }

}

static void clock_view_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, lock_view, SWITCH_OUT_NONE_ANIMATION,
                             SWITCH_IN_NONE_ANIMATION,
                             GUI_EVENT_TOUCH_CLICKED);

    gui_set_bg_color(BG_1_LIGHT);
    switch (clock_style)
    {
    case 1:
        break;
    case 2:
        break;
    case 3:
        clock3_design(view);
        break;
    case 4:
        clock4_design(view);
        break;
    case 5:
        break;
    default:
        clock3_design(view);
        break;
    }

    gui_obj_create_timer(GUI_BASE(view), 1000, true, time_update);
}

static void clock3_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);

    // Background
    vec = gui_img_create_from_mem(parent, 0, CLOCK3_VECTOR_BIN, 0, 0, 0, 0);
    gui_img_a8_recolor(vec, FG_1_DARK.color.argb_full);
    gui_img_a8_fix_bg(vec, theme_color.color.argb_full);
    gui_img_set_mode(vec, IMG_2D_SW_FIX_A8_BGFG);

    // time hands
    gui_img_t *img = gui_img_create_from_mem(parent, "min", CLOCK3_MIN_BIN, 160, 100, 0, 0);
    gui_img_set_mode(img, IMG_2D_SW_SRC_OVER_MODE);
    // gui_img_a8_recolor(img, BG_1_LIGHT.color.argb_full);
    gui_img_set_focus(img, 160 - 151, 100 - 15); // img target is (151, 15), focus is (160, 100)
    img = gui_img_create_from_mem(parent, "hour", CLOCK3_HOUR_BIN, 160, 100, 0, 0);
    gui_img_set_mode(img, IMG_2D_SW_SRC_OVER_MODE);
    // gui_img_a8_recolor(img, BG_1_LIGHT.color.argb_full);
    gui_img_set_focus(img, 160 - 148, 100 - 47); // img target is (148, 47), focus is (160, 100)
    img = gui_img_create_from_mem(parent, 0, CLOCK3_CENTER_BIN, 146, 86, 0, 0);
    gui_img_a8_recolor(img, FG_WHITE.color.argb_full);
}

static void clock4_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);

    vec = gui_img_create_from_mem(parent, "bg", CLOCK4_VECTOR_BIN, 55, 0, 0, 0);
    gui_img_a8_recolor(vec, FG_1_DARK.color.argb_full);
    gui_img_a8_fix_bg(vec, theme_color.color.argb_full);
    gui_img_set_mode(vec, IMG_2D_SW_FIX_A8_BGFG);


    // gui_obj_create_timer(GUI_BASE(tab), 10, true, time_update);
}