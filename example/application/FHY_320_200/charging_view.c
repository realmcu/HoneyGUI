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
static const gui_view_descriptor_t *wallpaper_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = charging_view_design,
};

static void *icon_array[8] = {0};

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
    wallpaper_view = gui_view_descriptor_get("wallpaper_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void battery_update_cb(void *p)
{
    gui_text_t *text = (gui_text_t *)p;
    battery_barn_val++;
    if (battery_barn_val >= 100)
    {
        battery_barn_val = 0;
    }
    sprintf(battery_barn_str, "%d%%", battery_barn_val);
    gui_text_content_set(text, battery_barn_str, strlen(battery_barn_str));
}

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
    gui_img_t *icon_bg = (gui_img_t *)gui_list_entry(GUI_BASE(current_view)->child_list.prev, gui_obj_t,
                                                     brother_list);

    if (cnt <= cnt_max_1) //fade in
    {
        gui_img_set_opacity(bg, UINT8_MAX * cnt / cnt_max_1);
        gui_img_set_opacity(icon_bg, UINT8_MAX * cnt / cnt_max_1);
    }
    else if (cnt <= cnt_max_2) //wait
    {
        return;
    }
    else if (cnt <= cnt_max_3) //scale
    {
        uint8_t index = 7 * (cnt - cnt_max_2) / (cnt_max_3 - cnt_max_2);
        gui_img_t *icon = (gui_img_t *)gui_list_entry(GUI_BASE(icon_bg)->child_list.next, gui_obj_t,
                                                      brother_list);
        gui_img_set_image_data(icon, icon_array[index]);
    }
    else if (cnt <= cnt_max_4) //move
    {
        gui_img_set_opacity(bg, UINT8_MAX * (cnt_max_4 - cnt) / (cnt_max_4 - cnt_max_3));
        gui_img_translate(icon_bg, 0, (ICON_CHARGING_STOP_Y - ICON_CHARGING_START_Y) * (cnt - cnt_max_3) /
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
        gui_obj_create_timer(GUI_BASE(text), 30, true, battery_update_cb);
        cnt = 0;
    }
}

static void charging_view_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, wallpaper_view, SWITCH_OUT_NONE_ANIMATION,
                             SWITCH_IN_NONE_ANIMATION,
                             GUI_EVENT_TOUCH_CLICKED);

    gui_set_bg_color(BG_1_LIGHT);
    if (!icon_array[0])
    {
        icon_array[0] = ICON_CHARGING_0_BIN;
        icon_array[1] = ICON_CHARGING_1_BIN;
        icon_array[2] = ICON_CHARGING_2_BIN;
        icon_array[3] = ICON_CHARGING_3_BIN;
        icon_array[4] = ICON_CHARGING_4_BIN;
        icon_array[5] = ICON_CHARGING_5_BIN;
        icon_array[6] = ICON_CHARGING_6_BIN;
        icon_array[7] = ICON_CHARGING_7_BIN;
    }


    gui_obj_t *parent = GUI_BASE(view);
    gui_img_t *bg = gui_img_create_from_mem(parent, 0, BATTERY_CHARGING_BG_BIN, 0, 0, 0, 0);
    // gui_img_set_mode(bg, IMG_BYPASS_MODE);
    // gui_img_t *bg = gui_img_create_from_mem(parent, 0, BATTERY_CHARGING_BG_NEW_BIN, 0, 0, 0, 0);
    // gui_img_set_a8_fg_color(bg, GUI_COLOR_ARGB8888(255, 0x0, 0x51, 0xFF));
    // gui_img_t *bg_mid = gui_img_create_from_mem(bg, 0, BATTERY_CHARGING_MID_BG_BIN, 35, 0, 0, 0);
    // gui_img_set_a8_fg_color(bg_mid, GUI_COLOR_ARGB8888(255, 0x0, 0xD9, 0xFF));
    // gui_img_t *bg_top = gui_img_create_from_mem(bg, 0, BATTERY_CHARGING_TOP_BG_BIN, 0, 0, 0, 0);
    // gui_img_set_a8_fg_color(bg_top, GUI_COLOR_ARGB8888(255, 0x0, 0xFF, 0xF6));

    gui_img_t *charging_icon_bg = gui_img_create_from_mem(parent, "icon", ICON_CHARGING_BG_BIN,
                                                          ICON_CHARGING_X, ICON_CHARGING_START_Y, 0, 0);
    gui_img_set_a8_fg_color(charging_icon_bg, GUI_COLOR_ARGB8888(255, 0, 0xFF, 0xA6));
    gui_img_t *charging_icon = gui_img_create_from_mem(charging_icon_bg, 0, icon_array[0], 10, 4, 0, 0);
    gui_img_set_a8_fg_color(charging_icon, GUI_COLOR_ARGB8888(255, 0, 0xFF, 0xF6));

    gui_obj_create_timer(GUI_BASE(bg), 10, true, entrance_animation);
}