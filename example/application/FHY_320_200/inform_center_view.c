/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_list.h"
#include "gui_message.h"

/*============================================================================*
 *                           Types
 *============================================================================*/

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
static const gui_view_descriptor_t *timer_view = NULL;
static const gui_view_descriptor_t *flashlight_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = inform_center_view_design,
};

static char time_str[20] = {0};
char battery_earbuds_connect_l_str[4] = {0};
char battery_earbuds_connect_r_str[4] = {0};
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
    timer_view = gui_view_descriptor_get("timer_view");
    flashlight_view = gui_view_descriptor_get("flashlight_view");
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
    if (time_format_24)
    {
        sprintf(time_str, "%s %s %d %02d:%02d", day[timeinfo->tm_wday], month[timeinfo->tm_mon],
                timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);
    }
    else
    {
        int tm_hour = (timeinfo->tm_hour % 12 == 0 ? 12 : timeinfo->tm_hour % 12);
        sprintf(time_str, "%s %s %d %d:%02d %s", day[timeinfo->tm_wday], month[timeinfo->tm_mon],
                timeinfo->tm_mday, tm_hour, timeinfo->tm_min, (timeinfo->tm_hour >= 12 ? "PM" : "AM"));
    }

    gui_text_content_set((gui_text_t *)obj, time_str, strlen(time_str));
}

static void click_button_message(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    gui_log("click message button\n");
}

static void click_button(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    bool *status = param;
    *status = !(*status);
    gui_img_t *bg = (gui_img_t *)obj;
    gui_img_t *icon = (gui_img_t *)gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
                                                  brother_list);

    if (theme_bg_white)
    {
        if (*status)
        {
            gui_img_a8_recolor(bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
            gui_img_a8_recolor(icon, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(bg, BG_2_LIGHT.color.argb_full);
            gui_img_a8_recolor(icon, FG_3_LIGHT.color.argb_full);
        }
    }
    else
    {
        if (*status)
        {
            gui_img_a8_recolor(bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
            gui_img_a8_recolor(icon, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(bg, BG_2_DARK.color.argb_full);
            gui_img_a8_recolor(icon, FG_3_DARK.color.argb_full);
        }
    }
    gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
    gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
}

static void click_button_flashlight(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_view_set_animate_step(gui_view_get_current(), 400);
    gui_view_switch_direct(gui_view_get_current(), flashlight_view, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
}

static void click_button_dark_light_mode(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    bool *status = param;
    *status = !(*status);
    gui_msg_t msg =
    {
        .event = GUI_EVENT_USER_DEFINE,
        .cb = regenerate_current_view,
    };
    gui_send_msg_to_server(&msg);
}

static void click_button_timer(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    gui_view_set_animate_step(current_view, 10);
    gui_view_switch_direct(current_view, timer_view, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
}

static void function_icon_design(gui_obj_t *parent)
{
    gui_color_t bg_color;
    gui_color_t bg_color_off;
    gui_color_t icon_color_off;
    gui_color_t bg_theme1_color;
    gui_color_t bg_theme2_color;
    gui_color_t bg_theme3_color;
    if (theme_bg_white)
    {
        bg_color = SCREEN_BG_LIGHT;
        bg_color_off = BG_2_LIGHT;
        icon_color_off = FG_3_LIGHT;
        bg_theme1_color = BG_THEME1_BRIGHT_LIGHT;
        bg_theme2_color = BG_THEME2_BRIGHT_LIGHT;
        bg_theme3_color = BG_THEME3_BRIGHT_LIGHT;
    }
    else
    {
        bg_color = SCREEN_BG_DARK;
        bg_color_off = BG_2_DARK;
        icon_color_off = FG_3_DARK;
        bg_theme1_color = BG_THEME1_BRIGHT_DARK;
        bg_theme2_color = BG_THEME2_BRIGHT_DARK;
        bg_theme3_color = BG_THEME3_BRIGHT_DARK;
    }

    int x = 12;
    if (f_status.infor_center_func_0)
    {
        gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, x, 118, 0, 0);
        gui_img_t *icon = gui_img_create_from_mem(bg, 0, PAUSE_SMALL_BIN, 36, 22, 0, 0);
        gui_obj_add_event_cb(bg, click_button, GUI_EVENT_TOUCH_CLICKED, &f_status.auto_play_pause);
        if (f_status.auto_play_pause)
        {
            gui_img_a8_recolor(bg, bg_theme1_color.color.argb_full);
            gui_img_a8_recolor(icon, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(bg, bg_color_off.color.argb_full);
            gui_img_a8_recolor(icon, icon_color_off.color.argb_full);
        }
        gui_img_a8_fix_bg(bg, bg_color.color.argb_full);
        gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
        gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
        x += 105;
    }
    if (f_status.infor_center_func_1)
    {
        gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, x, 118, 0, 0);
        gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_ASC_BIN, 25, 14, 0, 0);
        gui_obj_add_event_cb(bg, click_button, GUI_EVENT_TOUCH_CLICKED, &f_status.ambient_sound);
        if (f_status.ambient_sound)
        {
            gui_img_a8_recolor(bg, bg_theme1_color.color.argb_full);
            gui_img_a8_recolor(icon, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(bg, bg_color_off.color.argb_full);
            gui_img_a8_recolor(icon, icon_color_off.color.argb_full);
        }
        gui_img_a8_fix_bg(bg, bg_color.color.argb_full);
        gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
        gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
        x += 105;
    }
    if (f_status.infor_center_func_2)
    {
        gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, x, 118, 0, 0);
        gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_EQ_BIN, 25, 14, 0, 0);
        gui_obj_add_event_cb(bg, click_button, GUI_EVENT_TOUCH_CLICKED, &f_status.equalizer);
        if (f_status.equalizer)
        {
            gui_img_a8_recolor(bg, bg_theme1_color.color.argb_full);
            gui_img_a8_recolor(icon, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(bg, bg_color_off.color.argb_full);
            gui_img_a8_recolor(icon, icon_color_off.color.argb_full);
        }
        gui_img_a8_fix_bg(bg, bg_color.color.argb_full);
        gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
        gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
        x += 105;
    }
    if (f_status.infor_center_func_3)
    {
        gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, x, 118, 0, 0);
        gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_SPATIAL_SOUND_BIN, 25, 14, 0, 0);
        gui_obj_add_event_cb(bg, click_button, GUI_EVENT_TOUCH_CLICKED, &f_status.spatial_sound);
        if (f_status.spatial_sound)
        {
            gui_img_a8_recolor(bg, bg_theme1_color.color.argb_full);
            gui_img_a8_recolor(icon, FG_WHITE.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(bg, bg_color_off.color.argb_full);
            gui_img_a8_recolor(icon, icon_color_off.color.argb_full);
        }
        gui_img_a8_fix_bg(bg, bg_color.color.argb_full);
        gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
        gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
        x += 105;
    }
    if (f_status.infor_center_func_4)
    {
        gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, x, 118, 0, 0);
        gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_FLASHLIGHT_BIN, 25, 14, 0, 0);
        gui_obj_add_event_cb(bg, click_button_flashlight, GUI_EVENT_TOUCH_CLICKED, NULL);
        gui_img_a8_recolor(bg, bg_color_off.color.argb_full);
        gui_img_a8_recolor(icon, icon_color_off.color.argb_full);
        gui_img_a8_fix_bg(bg, bg_color.color.argb_full);
        gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
        gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
        x += 105;
    }
    if (f_status.infor_center_func_5)
    {
        gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, x, 118, 0, 0);
        gui_img_t *icon = gui_img_create_from_mem(bg, 0, theme_bg_white ? ICON_LIGHT_BIN : ICON_DARK_BIN,
                                                  25, 14, 0, 0);
        gui_obj_add_event_cb(bg, click_button_dark_light_mode, GUI_EVENT_TOUCH_CLICKED, &theme_bg_white);
        gui_img_a8_recolor(bg, bg_theme3_color.color.argb_full);
        gui_img_a8_fix_bg(bg, bg_color.color.argb_full);
        gui_img_a8_recolor(icon, FG_WHITE.color.argb_full);
        x += 105;
    }
}

static void timer_cb(void *p)
{
    GUI_UNUSED(p);
    gui_text_content_set((void *)p, timer_str, strlen(timer_str));
    if (timer_val == 0)
    {
        gui_obj_stop_timer((void *)p);
    }
}

static void timer_design(gui_obj_t *parent)
{
    gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_296_68_BIN, 12, 116, 0, 0);
    gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_TIMER_MEDIUM_BIN, 20, 21, 0, 0);
    gui_img_t *arrow = gui_img_create_from_mem(bg, 0, ICON_ARROW_R_BIN, 277, 28, 0, 0);
    gui_text_t *text = gui_text_create(bg, 0, 52, 0, 100, 68);
    gui_text_set(text, "Timer", GUI_FONT_SRC_BMP, theme_bg_white ? FG_1_LIGHT : FG_1_DARK, 5, 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_LEFT);
    text = gui_text_create(bg, 0, 226, 0, 50, 68);
    gui_text_set(text, timer_str, GUI_FONT_SRC_BMP, theme_bg_white ? FG_THEME2_LIGHT : FG_THEME2_DARK,
                 strlen(timer_str), 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_LEFT);
    gui_obj_add_event_cb(bg, click_button_timer, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_create_timer(GUI_BASE(text), 1000, true, timer_cb);
    gui_obj_start_timer(GUI_BASE(text));

    if (theme_bg_white)
    {
        gui_img_a8_recolor(bg, BG_THEME2_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(icon, FG_THEME2_LIGHT.color.argb_full);
        gui_img_a8_recolor(arrow, FG_2_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(bg, BG_THEME2_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(icon, FG_THEME2_DARK.color.argb_full);
        gui_img_a8_recolor(arrow, FG_2_DARK.color.argb_full);
    }
    gui_img_a8_mix_alpha(arrow, arrow->fg_color_set >> 24);
}

static void inform_center_view_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, quick_view, SWITCH_OUT_TO_TOP_USE_TRANSLATION,
                             SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_UP);

    gui_obj_t *parent = GUI_BASE(view);
    gui_color_t font_color;
    if (theme_bg_white)
    {
        gui_set_bg_color(SCREEN_BG_LIGHT);
        font_color = FG_1_LIGHT;
    }
    else
    {
        gui_set_bg_color(SCREEN_BG_DARK);
        font_color = FG_1_DARK;
    }

    gui_text_t *text = gui_text_create(parent, 0, 0, 0, gui_get_screen_width(), 40);
    gui_text_set(text, time_str, GUI_FONT_SRC_BMP, font_color, strlen(time_str), 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);
    gui_obj_create_timer(GUI_BASE(text), 30000, true, time_update_cb);

    gui_img_t *battery_bg = gui_img_create_from_mem(parent, "battery_bg", BATTERY_STATUS_BG_BIN, 12, 40,
                                                    0, 0);
    gui_img_t *barn_inner = gui_img_create_from_mem(battery_bg, 0, BARN_INNER_BIG_BIN, 88, 15, 0, 0);
    gui_img_t *barn_outer = gui_img_create_from_mem(battery_bg, 0, BARN_OUTER_BIG_BIN, 85, 11, 0, 0);
    gui_img_t *earbuds_connect_l = gui_img_create_from_mem(battery_bg, 0, EARPLUG_BIG_L_BIN, 31, 11, 0,
                                                           0);
    gui_img_t *earbuds_connect_r = gui_img_create_from_mem(battery_bg, 0, EARPLUG_BIG_R_BIN, 150, 11, 0,
                                                           0);

    sprintf(battery_earbuds_connect_l_str, "%d%%", battery_earbuds_connect_l_val);
    sprintf(battery_earbuds_connect_r_str, "%d%%", battery_earbuds_connect_r_val);
    sprintf(battery_barn_str, "%d%%", battery_barn_val);
    text = gui_text_create(battery_bg, 0, 23, 37, 30, 0);
    gui_text_set(text, battery_earbuds_connect_l_str, GUI_FONT_SRC_BMP, font_color,
                 strlen(battery_earbuds_connect_l_str), 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    text = gui_text_create(battery_bg, 0, 82, 37, 30, 0);
    gui_text_set(text, battery_barn_str, GUI_FONT_SRC_BMP, font_color,
                 strlen(battery_barn_str), 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    text = gui_text_create(battery_bg, 0, 142, 37, 30, 0);
    gui_text_set(text, battery_earbuds_connect_r_str, GUI_FONT_SRC_BMP, font_color,
                 strlen(battery_earbuds_connect_r_str), 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);

    gui_img_t *message_bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, 217, 40, 0,
                                                    0);
    gui_obj_add_event_cb(message_bg, click_button_message, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_t *message = gui_img_create_from_mem(message_bg, 0, ICON_MESSAGE_SMALL_BIN, 36, 13, 0, 0);
    gui_img_t *arrow = gui_img_create_from_mem(message_bg, 0, ICON_ARROW_R_BIN, 72, 26, 0, 0);
    sprintf(message_num_str, "%d", message_num_val);
    text = gui_text_create(message_bg, 0, 0, 37, 90, 64);
    gui_text_set(text, message_num_str, GUI_FONT_SRC_BMP, font_color, strlen(message_num_str), 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);

    if (f_status.timer)
    {
        timer_design(parent);
    }
    else
    {
        if (info_center_func_cnt)
        {
            function_icon_design(parent);
        }
    }

    if (theme_bg_white)
    {
        gui_img_a8_recolor(battery_bg, BG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(message_bg, BG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(message, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(arrow, FG_2_LIGHT.color.argb_full);
        {
            uint32_t color;
            if (battery_barn_val > 50)
            {
                color = FG_1_LIGHT.color.argb_full;
            }
            else if (battery_barn_val > 10)
            {
                color = FG_THEME1_LIGHT.color.argb_full;
            }
            else
            {
                color = FG_WARNING.color.argb_full;
            }
            gui_img_a8_recolor(barn_inner, color);
            gui_img_a8_recolor(barn_outer, color);
        }

        if (f_status.earbuds_connect_l)
        {
            uint32_t color;
            if (battery_earbuds_connect_l_val > 50)
            {
                color = FG_1_LIGHT.color.argb_full;
            }
            else if (battery_earbuds_connect_l_val > 10)
            {
                color = FG_THEME1_LIGHT.color.argb_full;
            }
            else
            {
                color = FG_WARNING.color.argb_full;
            }
            gui_img_a8_recolor(earbuds_connect_l, color);
        }
        else
        {
            gui_img_a8_recolor(earbuds_connect_l, FG_2_LIGHT.color.argb_full);
        }
        if (f_status.earbuds_connect_r)
        {
            uint32_t color;
            if (battery_earbuds_connect_r_val > 50)
            {
                color = FG_1_LIGHT.color.argb_full;
            }
            else if (battery_earbuds_connect_r_val > 10)
            {
                color = FG_THEME1_LIGHT.color.argb_full;
            }
            else
            {
                color = FG_WARNING.color.argb_full;
            }
            gui_img_a8_recolor(earbuds_connect_r, color);
        }
        else
        {
            gui_img_a8_recolor(earbuds_connect_r, FG_2_LIGHT.color.argb_full);
        }
    }
    else
    {
        gui_img_a8_recolor(battery_bg, BG_1_DARK.color.argb_full);
        {
            uint32_t color;
            if (battery_barn_val > 50)
            {
                color = FG_1_DARK.color.argb_full;
            }
            else if (battery_barn_val > 10)
            {
                color = FG_THEME1_DARK.color.argb_full;
            }
            else
            {
                color = FG_WARNING.color.argb_full;
            }
            gui_img_a8_recolor(barn_inner, color);
            gui_img_a8_recolor(barn_outer, color);
        }
        if (f_status.earbuds_connect_l)
        {
            uint32_t color;
            if (battery_earbuds_connect_l_val > 50)
            {
                color = FG_1_DARK.color.argb_full;
            }
            else if (battery_earbuds_connect_l_val > 10)
            {
                color = FG_THEME1_DARK.color.argb_full;
            }
            else
            {
                color = FG_WARNING.color.argb_full;
            }
            gui_img_a8_recolor(earbuds_connect_l, color);
        }
        else
        {
            gui_img_a8_recolor(earbuds_connect_l, FG_2_DARK.color.argb_full);
        }
        if (f_status.earbuds_connect_r)
        {
            uint32_t color;
            if (battery_earbuds_connect_r_val > 50)
            {
                color = FG_1_DARK.color.argb_full;
            }
            else if (battery_earbuds_connect_r_val > 10)
            {
                color = FG_THEME1_DARK.color.argb_full;
            }
            else
            {
                color = FG_WARNING.color.argb_full;
            }
            gui_img_a8_recolor(earbuds_connect_r, color);
        }
        else
        {
            gui_img_a8_recolor(earbuds_connect_r, FG_2_DARK.color.argb_full);
        }
        gui_img_a8_recolor(message_bg, BG_1_DARK.color.argb_full);
        gui_img_a8_recolor(message, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(arrow, FG_2_DARK.color.argb_full);
    }
    gui_img_a8_mix_alpha(battery_bg, battery_bg->fg_color_set >> 24);
    gui_img_a8_mix_alpha(message_bg, message_bg->fg_color_set >> 24);
    gui_img_a8_mix_alpha(arrow, arrow->fg_color_set >> 24);
    gui_img_a8_mix_alpha(earbuds_connect_l, earbuds_connect_l->fg_color_set >> 24);
    gui_img_a8_mix_alpha(earbuds_connect_r, earbuds_connect_r->fg_color_set >> 24);
}