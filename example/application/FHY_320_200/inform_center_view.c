/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_list.h"

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef enum
{
    ANC = 0,
    AA,
    TT
} AMBIENT_SOUND_STATUS;

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "inform_center_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void inform_center_view_design(gui_view_t *view);
static void update_page_indicator(void);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *quick_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = inform_center_view_design,
};

static char time_str[20] = {0};
char battery_earphone_l_str[4] = {0};
char battery_earphone_r_str[4] = {0};
char battery_barn_str[4] = {0};
static char message_num_str[4] = {0};

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
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void time_update_cb(void *obj)
{
    if (!timeinfo)
    {
        return;
    }
    sprintf(time_str, "%s %s %d %02d:%02d", day[timeinfo->tm_wday], month[timeinfo->tm_mon],
            timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);
    gui_text_content_set((gui_text_t *)obj, time_str, strlen(time_str));
}

static void click_button_message(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    gui_log("click message button\n");
}

static void click_button_left(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    status_asc = !status_asc;
    gui_img_t *bg = (gui_img_t *)obj;
    gui_img_t *icon = (gui_img_t *)gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
                                                  brother_list);

    if (theme_bg_white)
    {
        if (status_asc)
        {
            gui_img_set_a8_fg_color(bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
            gui_img_set_a8_fg_color(icon, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_set_a8_fg_color(bg, BG_2_LIGHT.color.argb_full);
            gui_img_set_a8_fg_color(icon, FG_3_LIGHT.color.argb_full);
        }
    }
    else
    {
        if (status_asc)
        {
            gui_img_set_a8_fg_color(bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
            gui_img_set_a8_fg_color(icon, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_set_a8_fg_color(bg, BG_2_DARK.color.argb_full);
            gui_img_set_a8_fg_color(icon, FG_3_DARK.color.argb_full);
        }
    }
}

static void click_button_center(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    status_spatial_sound = !status_spatial_sound;
    gui_img_t *bg = (gui_img_t *)obj;
    gui_img_t *icon = (gui_img_t *)gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
                                                  brother_list);

    if (theme_bg_white)
    {
        if (status_spatial_sound)
        {
            gui_img_set_a8_fg_color(bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
            gui_img_set_a8_fg_color(icon, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_set_a8_fg_color(bg, BG_2_LIGHT.color.argb_full);
            gui_img_set_a8_fg_color(icon, FG_3_LIGHT.color.argb_full);
        }
    }
    else
    {
        if (status_spatial_sound)
        {
            gui_img_set_a8_fg_color(bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
            gui_img_set_a8_fg_color(icon, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_set_a8_fg_color(bg, BG_2_DARK.color.argb_full);
            gui_img_set_a8_fg_color(icon, FG_3_DARK.color.argb_full);
        }
    }
}

static void click_button_right(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    status_flashlight = !status_flashlight;
    gui_img_t *bg = (gui_img_t *)obj;
    gui_img_t *icon = (gui_img_t *)gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
                                                  brother_list);

    if (theme_bg_white)
    {
        if (status_flashlight)
        {
            gui_img_set_a8_fg_color(bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
            gui_img_set_a8_fg_color(icon, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_set_a8_fg_color(bg, BG_2_LIGHT.color.argb_full);
            gui_img_set_a8_fg_color(icon, FG_3_LIGHT.color.argb_full);
        }
    }
    else
    {
        if (status_flashlight)
        {
            gui_img_set_a8_fg_color(bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
            gui_img_set_a8_fg_color(icon, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_set_a8_fg_color(bg, BG_2_DARK.color.argb_full);
            gui_img_set_a8_fg_color(icon, FG_3_DARK.color.argb_full);
        }
    }
}

static void show_bg(void *obj)
{
    static bool flag = false;
    if (gui_view_get_next())
    {
        gui_obj_hidden((void *)obj, false);
        flag = false;
    }
    else
    {
        if (flag)
        {
            flag = false;
            gui_obj_hidden((void *)obj, true);
        }
        else
        {
            flag = true;
        }
    }
}

static void inform_center_view_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, quick_view, SWITCH_OUT_TO_TOP_USE_TRANSLATION,
                             SWITCH_INIT_STATE,
                             GUI_EVENT_TOUCH_MOVE_UP);

    gui_obj_t *parent = GUI_BASE(view);
    gui_color_t font_color;
    gui_img_t *bg = gui_img_create_from_mem(parent, 0, SCREEN_BG_BIN, 0, 0, 0, 0);
    gui_obj_create_timer((void *)bg, 10, true, show_bg);
    if (theme_bg_white)
    {
        gui_set_bg_color(SCREEN_BG_LIGHT);
        gui_img_set_a8_fg_color(bg, SCREEN_BG_LIGHT.color.argb_full);
        font_color = FG_1_LIGHT;
    }
    else
    {
        gui_set_bg_color(BG_1_LIGHT);
        gui_img_set_a8_fg_color(bg, GUI_COLOR_ARGB8888(255, 0, 0, 0));
        font_color = FG_1_DARK;
    }

    gui_text_t *text = gui_text_create(parent, 0, 0, 0, gui_get_screen_width(), 40);
    gui_text_set(text, time_str, GUI_FONT_SRC_BMP, font_color, strlen(time_str), 28);
    gui_text_type_set(text, CAPTION_2_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);
    gui_obj_create_timer(GUI_BASE(text), 3000, true, time_update_cb);


    gui_img_t *battery_bg = gui_img_create_from_mem(parent, "battery_bg", BATTERY_STATUS_BG_BIN, 12, 40,
                                                    0, 0);
    gui_img_t *barn_inner = gui_img_create_from_mem(battery_bg, 0, BARN_INNER_BIG_BIN, 88, 15, 0, 0);
    gui_img_t *barn_outer = gui_img_create_from_mem(battery_bg, 0, BARN_OUTER_BIG_BIN, 85, 11, 0, 0);
    gui_img_t *earphone_l = gui_img_create_from_mem(battery_bg, 0, EARPLUG_BIG_L_BIN, 31, 11, 0, 0);
    gui_img_t *earphone_r = gui_img_create_from_mem(battery_bg, 0, EARPLUG_BIG_R_BIN, 150, 11, 0, 0);

    sprintf(battery_earphone_l_str, "%d%%", battery_earphone_l_val);
    sprintf(battery_earphone_r_str, "%d%%", battery_earphone_r_val);
    sprintf(battery_barn_str, "%d%%", battery_barn_val);
    text = gui_text_create(battery_bg, 0, 23, 37, 0, 0);
    gui_text_set(text, battery_earphone_l_str, GUI_FONT_SRC_BMP, font_color,
                 strlen(battery_earphone_l_str), 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);
    text = gui_text_create(battery_bg, 0, 82, 37, 0, 0);
    gui_text_set(text, battery_earphone_l_str, GUI_FONT_SRC_BMP, font_color,
                 strlen(battery_earphone_l_str), 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);
    text = gui_text_create(battery_bg, 0, 142, 37, 0, 0);
    gui_text_set(text, battery_earphone_l_str, GUI_FONT_SRC_BMP, font_color,
                 strlen(battery_earphone_l_str), 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);

    gui_img_t *message_bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, 217, 40, 0,
                                                    0);
    gui_obj_add_event_cb(message_bg, click_button_message, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_t *message = gui_img_create_from_mem(message_bg, 0, ICON_MESSAGE_SMALL_BIN, 36, 13, 0, 0);
    gui_img_t *arrow = gui_img_create_from_mem(message_bg, 0, ICON_ARRAW_R_BIN, 72, 26, 0, 0);
    sprintf(message_num_str, "%d", message_num_val);
    text = gui_text_create(message_bg, 0, 0, 37, 90, 64);
    gui_text_set(text, message_num_str, GUI_FONT_SRC_BMP, font_color, strlen(message_num_str), 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);

    gui_img_t *icon_bg_left = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, 12, 118,
                                                      0, 0);
    gui_img_t *icon_bg_center = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, 115,
                                                        118, 0, 0);
    gui_img_t *icon_bg_right = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, 218, 118,
                                                       0, 0);
    gui_img_t *icon_left = gui_img_create_from_mem(icon_bg_left, 0, ICON_ASC_BIN, 25, 14, 0, 0);
    gui_img_t *icon_center = gui_img_create_from_mem(icon_bg_center, 0, ICON_SPATIAL_SOUND_BIN, 25, 14,
                                                     0, 0);
    gui_img_t *icon_right = gui_img_create_from_mem(icon_bg_right, 0, ICON_FLASHLIGHT_BIN, 25, 14, 0,
                                                    0);
    gui_obj_add_event_cb(icon_bg_left, click_button_left, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(icon_bg_center, click_button_center, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(icon_bg_right, click_button_right, GUI_EVENT_TOUCH_CLICKED, NULL);

    if (theme_bg_white)
    {
        gui_img_set_a8_fg_color(battery_bg, BG_1_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(barn_inner, FG_1_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(barn_outer, FG_1_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(earphone_l, FG_1_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(earphone_r, FG_1_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(message_bg, BG_1_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(message, FG_1_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(arrow, FG_2_LIGHT.color.argb_full);
        if (status_asc)
        {
            gui_img_set_a8_fg_color(icon_bg_left, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
            gui_img_set_a8_fg_color(icon_left, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_set_a8_fg_color(icon_bg_left, BG_2_LIGHT.color.argb_full);
            gui_img_set_a8_fg_color(icon_left, FG_3_LIGHT.color.argb_full);
        }
        if (status_asc)
        {
            gui_img_set_a8_fg_color(icon_bg_center, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
            gui_img_set_a8_fg_color(icon_center, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_set_a8_fg_color(icon_bg_center, BG_2_LIGHT.color.argb_full);
            gui_img_set_a8_fg_color(icon_center, FG_3_LIGHT.color.argb_full);
        }
        if (status_asc)
        {
            gui_img_set_a8_fg_color(icon_bg_right, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
            gui_img_set_a8_fg_color(icon_right, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_set_a8_fg_color(icon_bg_right, BG_2_LIGHT.color.argb_full);
            gui_img_set_a8_fg_color(icon_right, FG_3_LIGHT.color.argb_full);
        }
    }
    else
    {
        gui_img_set_a8_fg_color(battery_bg, BG_1_DARK.color.argb_full);
        gui_img_set_a8_fg_color(barn_inner, FG_1_DARK.color.argb_full);
        gui_img_set_a8_fg_color(barn_outer, FG_1_DARK.color.argb_full);
        gui_img_set_a8_fg_color(earphone_l, FG_1_DARK.color.argb_full);
        gui_img_set_a8_fg_color(earphone_r, FG_1_DARK.color.argb_full);
        gui_img_set_a8_fg_color(message_bg, BG_1_DARK.color.argb_full);
        gui_img_set_a8_fg_color(message, FG_1_DARK.color.argb_full);
        gui_img_set_a8_fg_color(arrow, FG_2_DARK.color.argb_full);
        if (status_asc)
        {
            gui_img_set_a8_fg_color(icon_bg_left, BG_THEME1_BRIGHT_DARK.color.argb_full);
            gui_img_set_a8_fg_color(icon_left, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_set_a8_fg_color(icon_bg_left, BG_2_DARK.color.argb_full);
            gui_img_set_a8_fg_color(icon_left, FG_3_DARK.color.argb_full);
        }
        if (status_asc)
        {
            gui_img_set_a8_fg_color(icon_bg_center, BG_THEME1_BRIGHT_DARK.color.argb_full);
            gui_img_set_a8_fg_color(icon_center, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_set_a8_fg_color(icon_bg_center, BG_2_DARK.color.argb_full);
            gui_img_set_a8_fg_color(icon_center, FG_3_DARK.color.argb_full);
        }
        if (status_asc)
        {
            gui_img_set_a8_fg_color(icon_bg_right, BG_THEME1_BRIGHT_DARK.color.argb_full);
            gui_img_set_a8_fg_color(icon_right, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_set_a8_fg_color(icon_bg_right, BG_2_DARK.color.argb_full);
            gui_img_set_a8_fg_color(icon_right, FG_3_DARK.color.argb_full);
        }
    }
}