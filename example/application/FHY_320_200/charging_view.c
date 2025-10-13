/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_text.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "charging_view"

#define ICON_CHARGING_X             138
#define ICON_CHARGING_START_Y       78
#define ICON_CHARGING_STOP_Y        62
#define ICON_CHARGING_TARGET_SCALE  0.8f

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void charging_view_design(gui_view_t *view);

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
    .on_switch_in = charging_view_design,
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
    lock_view = gui_view_descriptor_get("lock_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void entrance_animation(void *p)
{
    GUI_UNUSED(p);
    static uint8_t cnt = 0;
    cnt++;
    uint8_t cnt_max_1 = 25;
    uint8_t cnt_max_2 = 50;
    uint8_t cnt_max_3 = 75;
    uint8_t cnt_max_4 = 100;
    gui_img_t *bg = (gui_img_t *)p;
    gui_img_t *icon = (gui_img_t *)gui_list_entry(GUI_BASE(p)->brother_list.next, gui_obj_t,
                                                  brother_list);

    if (cnt <= cnt_max_1) //fade in
    {
        gui_img_set_opacity(bg, UINT8_MAX * cnt / cnt_max_1);
        gui_img_set_opacity(icon, UINT8_MAX * cnt / cnt_max_1);
    }
    else if (cnt <= cnt_max_2) //wait
    {
        return;
    }
    else if (cnt <= cnt_max_3) //scale
    {
        float scale = 1.0f + (ICON_CHARGING_TARGET_SCALE - 1.0f) * (cnt - cnt_max_2) /
                      (cnt_max_3 - cnt_max_2);
        gui_img_scale(icon, scale, scale);
    }
    else if (cnt <= cnt_max_4) //move
    {
        gui_img_set_opacity(bg, UINT8_MAX * (cnt_max_4 - cnt) / (cnt_max_4 - cnt_max_3));
        gui_img_translate(icon, 0, (ICON_CHARGING_STOP_Y - ICON_CHARGING_START_Y) * (cnt - cnt_max_3) /
                          (cnt_max_4 - cnt_max_3));
    }
    if (cnt == cnt_max_4)
    {
        gui_obj_hidden((void *)bg, true);
        gui_obj_stop_timer(GUI_BASE(p));
        sprintf(battery_barn_str, "%d%%", battery_barn_val);
        gui_text_t *text = gui_text_create(GUI_BASE(p)->parent, 0, 0, 100, 320, 32);
        gui_text_set(text, battery_barn_str, GUI_FONT_SRC_BMP, FG_WHITE, strlen(battery_barn_str), 32);
        gui_text_type_set(text, CAPTION_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);
        cnt = 0;
    }
}

static void charging_view_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, lock_view, SWITCH_OUT_NONE_ANIMATION,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_TOUCH_CLICKED);

    gui_obj_t *parent = GUI_BASE(view);
    gui_img_t *bg = gui_img_create_from_mem(parent, "bg", BATTERY_CHARGING_BG_BIN, 0, 0, 0, 0);
    gui_img_set_opacity(bg, 0);

    int16_t focus = 22;
    gui_img_t *charging_icon = gui_img_create_from_mem(parent, "icon", ICON_CHARGING_BIN,
                                                       ICON_CHARGING_X + focus, ICON_CHARGING_START_Y + focus, 0, 0);
    gui_img_set_focus(charging_icon, focus, focus);

    gui_obj_create_timer(GUI_BASE(bg), 10, true, entrance_animation);
}