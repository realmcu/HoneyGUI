/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_scroll_text.h"

/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME INFORM_CENTER_VIEW

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
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = inform_center_view_design,
};

static char date_str[20] = {0};
char battery_headband_str[8] = {0};
char battery_tx_str[8] = {0};
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

static void time_update_cb(void *obj)
{
    if (!timeinfo)
    {
        return;
    }
    if (time_format_24)
    {
        sprintf(date_str, "%s %s %d %02d:%02d", day[language_index][timeinfo->tm_wday],
                month[language_index][timeinfo->tm_mon],
                timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);
    }
    else
    {
        int tm_hour = (timeinfo->tm_hour % 12 == 0 ? 12 : timeinfo->tm_hour % 12);
        sprintf(date_str, "%s %s %d %d:%02d %s", day[language_index][timeinfo->tm_wday],
                month[language_index][timeinfo->tm_mon],
                timeinfo->tm_mday, tm_hour, timeinfo->tm_min, (timeinfo->tm_hour >= 12 ? "PM" : "AM"));
    }

    gui_text_content_set((gui_text_t *)obj, date_str, strlen(date_str));
}

static void click_button_message(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);


    gui_view_switch_direct(current_view, NOTIFICATION_VIEW, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
}

static void click_button(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);


    bool *status = e->user_data;
    *status = !(*status);
    gui_img_t *bg = (gui_img_t *)obj;
    gui_img_t *icon = (gui_img_t *)gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
                                                  brother_list);

    if (status == &f_status.auto_play_pause)
    {
        if (*status)
        {
            gui_img_set_image_data(icon, PAUSE_SMALL_BIN);
        }
        else
        {
            gui_img_set_image_data(icon, PLAY_SMALL_BIN);
        }
        gui_img_refresh_size(icon);
        return;
    }

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

static void click_button_flashlight(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    gui_view_set_animate_step(gui_view_get_current(), 400);
    gui_view_switch_direct(gui_view_get_current(), FLASHLIGHT_VIEW, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
}

static void click_button_dark_light_mode(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    bool *status = e->user_data;
    *status = !(*status);
    msg_2_regenerate_current_view();
}

static void click_button_timer(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);


    gui_view_switch_direct(current_view, TIMER_VIEW, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
}

static void click_button_call(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    gui_view_switch_direct(current_view, CALL_VIEW, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
}

static void click_button_2_info_center(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);


    detail_page_design_func = page_information_center_customize_design;
    gui_view_switch_direct(current_view, DETAIL_VIEW, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
}

static void function_icon_design(gui_obj_t *parent)
{
    gui_color_t screen_bg_color;
    gui_color_t font_color;
    gui_color_t bg_color_off;
    gui_color_t bg_color_off_invalid;
    gui_color_t icon_infor_color;
    gui_color_t icon_color_off;
    gui_color_t icon_color_invalid;
    gui_color_t bg_theme1_color;
    gui_color_t bg_theme1_color_invalid;
    gui_color_t bg_theme2_color;
    gui_color_t bg_theme3_color;
    if (theme_bg_white)
    {
        screen_bg_color = SCREEN_BG_LIGHT;
        font_color = FG_1_LIGHT;
        bg_color_off = BG_2_LIGHT;
        bg_color_off_invalid = BG_1_LIGHT;
        icon_infor_color = FG_THEME3_LIGHT;
        icon_color_off = FG_3_LIGHT;
        icon_color_invalid = FG_2_LIGHT;
        bg_theme1_color = BG_THEME1_BRIGHT_LIGHT;
        bg_theme2_color = BG_THEME2_BRIGHT_LIGHT;
        bg_theme3_color = BG_THEME3_BRIGHT_LIGHT;
        bg_theme1_color_invalid = BG_THEME1_DARK_LIGHT;
    }
    else
    {
        screen_bg_color = SCREEN_BG_DARK;
        font_color = FG_1_DARK;
        bg_color_off = BG_2_DARK;
        bg_color_off_invalid = BG_1_DARK;
        icon_infor_color = FG_THEME3_DARK;
        icon_color_off = FG_3_DARK;
        icon_color_invalid = FG_2_DARK;
        bg_theme1_color = BG_THEME1_BRIGHT_DARK;
        bg_theme2_color = BG_THEME2_BRIGHT_DARK;
        bg_theme3_color = BG_THEME3_BRIGHT_DARK;
        bg_theme1_color_invalid = BG_THEME1_DARK_DARK;
    }
    int x = 12;
    if (info_center_func_cnt == 0)
    {
        gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_296_68_BIN, 12, 118, 0, 0);
        gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_CUSTOMIZE_INFOCENTER_SMALL_BIN, 18, 24, 0, 0);
        gui_img_t *arrow = gui_img_create_from_mem(bg, 0, ICON_ARROW_R_BIN, 277, 28, 0, 0);
        gui_text_t *text = gui_text_create(bg, 0, 52, 0, 296, 68);
        gui_text_set(text, (void *)page_name_array[language_index][INFORMATION_CENTER_CUSTOMIZE],
                     GUI_FONT_SRC_BMP,
                     font_color,
                     strlen((void *)page_name_array[language_index][INFORMATION_CENTER_CUSTOMIZE]), 20);
        gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, MID_LEFT);
        gui_obj_add_event_cb(bg, click_button_2_info_center, GUI_EVENT_TOUCH_CLICKED,
                             &f_status.auto_play_pause);
        gui_img_a8_recolor(bg, bg_color_off_invalid.color.argb_full);
        gui_img_a8_recolor(icon, icon_infor_color.color.argb_full);
        gui_img_a8_recolor(arrow, icon_color_invalid.color.argb_full);
        gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
        gui_img_a8_mix_alpha(arrow, arrow->fg_color_set >> 24);
    }
    else
    {
        if (f_status.infor_center_func_0)
        {
            gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, x, 118, 0, 0);
            gui_img_t *icon = gui_img_create_from_mem(bg, 0, PAUSE_SMALL_BIN, 36, 22, 0, 0);
            if (f_status.headband_connect)
            {
                gui_img_a8_recolor(bg, bg_theme1_color.color.argb_full);
                gui_img_a8_recolor(icon, FG_WHITE.color.argb_full);
                gui_obj_add_event_cb(bg, click_button, GUI_EVENT_TOUCH_CLICKED, &f_status.auto_play_pause);
            }
            else
            {
                gui_img_a8_recolor(bg, bg_theme1_color_invalid.color.argb_full);
                gui_img_a8_recolor(icon, icon_color_invalid.color.argb_full);
                gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
            }
            if (!f_status.auto_play_pause)
            {
                gui_img_set_image_data(icon, PLAY_SMALL_BIN);
                gui_img_refresh_size(icon);
            }
            x += 105;
        }
        if (f_status.infor_center_func_1)
        {
            gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, x, 118, 0, 0);
            gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_ASC_BIN, 25, 14, 0, 0);
            if (ambient_sound_type == ANC)
            {
                gui_img_set_image_data(icon, ICON_ASC_BIN);
            }
            else if (ambient_sound_type == TT)
            {
                gui_img_set_image_data(icon, ICON_TT_BIN);
            }
            if (f_status.headband_connect)
            {
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
                gui_obj_add_event_cb(bg, click_button, GUI_EVENT_TOUCH_CLICKED, &f_status.ambient_sound);
            }
            else
            {
                if (f_status.ambient_sound)
                {
                    gui_img_a8_recolor(bg, bg_theme1_color_invalid.color.argb_full);
                    gui_img_a8_recolor(icon, FG_WHITE.color.argb_full);
                }
                else
                {
                    gui_img_a8_recolor(bg, bg_color_off_invalid.color.argb_full);
                    gui_img_a8_recolor(icon, icon_color_invalid.color.argb_full);
                }
            }
            gui_img_a8_fix_bg(bg, screen_bg_color.color.argb_full);
            gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
            gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
            x += 105;
        }
        if (f_status.infor_center_func_2)
        {
            gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, x, 118, 0, 0);
            gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_EQ_BIN, 25, 14, 0, 0);
            if (f_status.headband_connect)
            {
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
                gui_obj_add_event_cb(bg, click_button, GUI_EVENT_TOUCH_CLICKED, &f_status.equalizer);
            }
            else
            {
                if (f_status.equalizer)
                {
                    gui_img_a8_recolor(bg, bg_theme1_color_invalid.color.argb_full);
                    gui_img_a8_recolor(icon, FG_WHITE.color.argb_full);
                }
                else
                {
                    gui_img_a8_recolor(bg, bg_color_off_invalid.color.argb_full);
                    gui_img_a8_recolor(icon, icon_color_invalid.color.argb_full);
                }
            }
            gui_img_a8_fix_bg(bg, screen_bg_color.color.argb_full);
            gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
            gui_img_a8_mix_alpha(icon, icon->fg_color_set >> 24);
            x += 105;
        }
        if (f_status.infor_center_func_3)
        {
            gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, x, 118, 0, 0);
            gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_SPATIAL_SOUND_BIN, 25, 14, 0, 0);
            if (f_status.headband_connect)
            {
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
                gui_obj_add_event_cb(bg, click_button, GUI_EVENT_TOUCH_CLICKED, &f_status.spatial_sound);
            }
            else
            {
                if (f_status.spatial_sound)
                {
                    gui_img_a8_recolor(bg, bg_theme1_color_invalid.color.argb_full);
                    gui_img_a8_recolor(icon, FG_WHITE.color.argb_full);
                }
                else
                {
                    gui_img_a8_recolor(bg, bg_color_off_invalid.color.argb_full);
                    gui_img_a8_recolor(icon, icon_color_invalid.color.argb_full);
                }
            }
            gui_img_a8_fix_bg(bg, screen_bg_color.color.argb_full);
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
            gui_img_a8_fix_bg(bg, screen_bg_color.color.argb_full);
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
            gui_img_a8_fix_bg(bg, screen_bg_color.color.argb_full);
            gui_img_a8_recolor(icon, FG_WHITE.color.argb_full);
            x += 105;
        }
    }

    if (info_center_func_cnt == 1)
    {
        gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_194_68_BIN, x, 118, 0, 0);
        gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_CUSTOMIZE_INFOCENTER_SMALL_BIN, 18, 24, 0, 0);
        gui_img_t *arrow = gui_img_create_from_mem(bg, 0, ICON_ARROW_R_BIN, 175, 28, 0, 0);
        gui_obj_add_event_cb(bg, click_button_2_info_center, GUI_EVENT_TOUCH_CLICKED,
                             &f_status.auto_play_pause);
        gui_scroll_text_t *text = gui_scroll_text_create(bg, 0, 44, 25, 118, 68);
        gui_scroll_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_scroll_text_set(text, (void *)page_name_array[language_index][INFORMATION_CENTER_CUSTOMIZE],
                            GUI_FONT_SRC_BMP,
                            font_color,
                            strlen(page_name_array[language_index][INFORMATION_CENTER_CUSTOMIZE]), 20);
        gui_scroll_text_scroll_set(text, SCROLL_X, 118, 118, 3000, 0);
        gui_img_a8_recolor(bg, bg_color_off_invalid.color.argb_full);
        gui_img_a8_recolor(icon, icon_infor_color.color.argb_full);
        gui_img_a8_recolor(arrow, icon_color_invalid.color.argb_full);
        gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
        gui_img_a8_mix_alpha(arrow, arrow->fg_color_set >> 24);
    }
    else if (info_center_func_cnt == 2)
    {
        gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_90_68_BIN, x, 118, 0, 0);
        gui_img_t *icon = gui_img_create_from_mem(bg, 0, ICON_CUSTOMIZE_INFOCENTER_SMALL_BIN, 34, 24, 0, 0);
        gui_img_t *arrow = gui_img_create_from_mem(bg, 0, ICON_ARROW_R_BIN, 73, 28, 0, 0);
        gui_obj_add_event_cb(bg, click_button_2_info_center, GUI_EVENT_TOUCH_CLICKED,
                             &f_status.auto_play_pause);
        gui_img_a8_recolor(bg, bg_color_off_invalid.color.argb_full);
        gui_img_a8_recolor(icon, icon_infor_color.color.argb_full);
        gui_img_a8_recolor(arrow, icon_color_invalid.color.argb_full);
        gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
        gui_img_a8_mix_alpha(arrow, arrow->fg_color_set >> 24);
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

static void call_timer_cb(void *p)
{
    GUI_UNUSED(p);
    gui_text_content_set((void *)p, calltime_str, strlen(calltime_str));
    if (f_status.call != CALL_ING)
    {
        msg_2_regenerate_current_view();
    }
}

static void timer_design(gui_obj_t *parent)
{
    gui_img_t *bg;
    gui_img_t *icon;
    gui_img_t *arrow;
    gui_text_t *text;
    if (f_status.call == CALL_ING)
    {
        bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_142_64_BIN, 166, 116, 0, 0);
        icon = gui_img_create_from_mem(bg, 0, ICON_TIMER_MEDIUM_BIN, 20, 21, 0, 0);
        arrow = gui_img_create_from_mem(bg, 0, ICON_ARROW_R_BIN, 123, 28, 0, 0);
        text = gui_text_create(bg, 0, 71, 0, 50, 68);
    }
    else
    {
        bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_296_68_BIN, 12, 116, 0, 0);
        icon = gui_img_create_from_mem(bg, 0, ICON_TIMER_MEDIUM_BIN, 20, 21, 0, 0);
        arrow = gui_img_create_from_mem(bg, 0, ICON_ARROW_R_BIN, 277, 28, 0, 0);
        gui_text_t *t = gui_text_create(bg, 0, 52, 0, 100, 68);
        gui_text_set(t, "Timer", GUI_FONT_SRC_BMP, theme_bg_white ? FG_1_LIGHT : FG_1_DARK, 5, 20);
        gui_text_type_set(t, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_LEFT);
        text = gui_text_create(bg, 0, 226, 0, 50, 68);
    }

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

static void call_design(gui_obj_t *parent)
{
    gui_img_t *bg;
    gui_img_t *icon;
    gui_img_t *arrow;
    gui_text_t *text;
    if (f_status.timer)
    {
        bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_142_64_BIN, 12, 116, 0, 0);
        icon = gui_img_create_from_mem(bg, 0, ICON_CALL_MEDIUM_BIN, 16, 20, 0, 0);
        arrow = gui_img_create_from_mem(bg, 0, ICON_ARROW_R_BIN, 123, 28, 0, 0);
        text = gui_text_create(bg, 0, 71, 0, 50, 68);

        timer_design(parent);
    }
    else
    {
        bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_296_68_BIN, 12, 116, 0, 0);
        icon = gui_img_create_from_mem(bg, 0, ICON_CALL_MEDIUM_BIN, 16, 20, 0, 0);
        arrow = gui_img_create_from_mem(bg, 0, ICON_ARROW_R_BIN, 277, 28, 0, 0);
        text = gui_text_create(bg, 0, 226, 0, 50, 68);

        gui_text_t *text_number = gui_text_create(bg, 0, 52, 0, 140, 68);
        gui_text_set(text_number, call_source_str, GUI_FONT_SRC_BMP, FG_NORMAL, strlen(call_source_str),
                     20);
        gui_text_type_set(text_number, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text_number, MID_LEFT);
    }

    sprintf(calltime_str, "%02d:%02d", calltime_val / 60, calltime_val % 60);
    gui_text_set(text, calltime_str, GUI_FONT_SRC_BMP, FG_NORMAL, strlen(calltime_str), 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_LEFT);
    gui_obj_add_event_cb(bg, click_button_call, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_create_timer(GUI_BASE(text), 1000, true, call_timer_cb);
    gui_obj_start_timer(GUI_BASE(text));
    gui_img_a8_recolor(icon, FG_NORMAL.color.argb_full);
    if (theme_bg_white)
    {
        gui_img_a8_recolor(bg, BG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(arrow, FG_2_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(bg, BG_1_DARK.color.argb_full);
        gui_img_a8_recolor(arrow, FG_2_DARK.color.argb_full);
    }
    gui_img_a8_mix_alpha(arrow, arrow->fg_color_set >> 24);
    gui_img_a8_mix_alpha(bg, bg->fg_color_set >> 24);
}

static void inform_center_view_design(gui_view_t *view)
{
    gui_view_set_animate_step(current_view, 10);
    gui_view_switch_on_event(view, QUICK_VIEW, SWITCH_OUT_TO_TOP_USE_TRANSLATION,
                             SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_UP);

    gui_obj_t *parent = GUI_BASE(view);
    gui_color_t font_color_valid;
    gui_color_t font_color_invalid;
    gui_color_t bg_color;
    gui_color_t arrow_color;
    gui_color_t charging_color;

    if (theme_bg_white)
    {
        gui_set_bg_color(SCREEN_BG_LIGHT);
        font_color_valid = FG_1_LIGHT;
        font_color_invalid.color.argb_full = GUI_COLOR_ARGB8888(0xFF, 0x93, 0x93, 0x93);
        bg_color = BG_1_LIGHT;
        arrow_color = FG_2_LIGHT;
        charging_color = FG_THEME1_LIGHT;
    }
    else
    {
        gui_set_bg_color(SCREEN_BG_DARK);
        font_color_valid = FG_1_DARK;
        font_color_invalid.color.argb_full = GUI_COLOR_ARGB8888(0xFF, 122, 122, 122);
        bg_color = BG_1_DARK;
        arrow_color = FG_2_DARK;
        charging_color = FG_THEME1_DARK;
    }

    gui_text_t *text = gui_text_create(parent, 0, 0, 0, gui_get_screen_width(), 40);
    gui_text_set(text, date_str, GUI_FONT_SRC_BMP, font_color_valid, strlen(date_str), 20);
    gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);
    gui_obj_create_timer(GUI_BASE(text), 30000, true, time_update_cb);

    gui_img_t *battery_bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_142_64_BIN, 12, 40,
                                                    0, 0);
    gui_img_t *tx_inner = gui_img_create_from_mem(battery_bg, 0, ICON_TX_INNER_BIN, 97, 16, 0, 0);
    gui_img_t *tx_outer = gui_img_create_from_mem(battery_bg, 0, ICON_TX_OUTER_BIN, 94, 13, 0, 0);
    gui_img_t *headband = gui_img_create_from_mem(battery_bg, 0, ICON_HEADBAND_BIN, 25, 9, 0, 0);
    sprintf(battery_tx_str, "%d%%", battery_tx_val);
    gui_text_t *text_tx = gui_text_create(battery_bg, 0, 91, 36, 30, 0);
    gui_text_set(text_tx, battery_tx_str, GUI_FONT_SRC_BMP, font_color_valid, strlen(battery_tx_str),
                 20);
    gui_text_type_set(text_tx, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_tx, CENTER);

    gui_color_t font_color;
    if (f_status.headband_connect)
    {
        font_color = font_color_valid;
        sprintf(battery_headband_str, "%d%%", battery_headband_val);
    }
    else
    {
        font_color = font_color_invalid;
        sprintf(battery_headband_str, "--");
    }

    gui_text_t *text_headband = gui_text_create(battery_bg, 0, 22, 36, 30, 0);
    gui_text_set(text_headband, battery_headband_str, GUI_FONT_SRC_BMP, font_color_valid,
                 strlen(battery_headband_str), 20);
    gui_text_type_set(text_headband, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_headband, CENTER);

    gui_img_t *message_bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_142_64_BIN, 166, 40, 0,
                                                    0);
    gui_obj_add_event_cb(message_bg, click_button_message, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_t *message = gui_img_create_from_mem(message_bg, 0, ICON_MESSAGE_SMALL_BIN, 61, 13, 0, 0);
    gui_img_t *arrow = gui_img_create_from_mem(message_bg, 0, ICON_ARROW_R_BIN, 115, 26, 0, 0);
    if (message_num_val > 0)
    {
        font_color = font_color_valid;
        sprintf(message_num_str, "%d", message_num_val);
    }
    else
    {
        font_color = font_color_invalid;
        sprintf(message_num_str, "--");
    }
    gui_text_t *text_message = gui_text_create(message_bg, 0, 0, 36, 142, 64);
    gui_text_set(text_message, message_num_str, GUI_FONT_SRC_BMP, font_color, strlen(message_num_str),
                 20);
    gui_text_type_set(text_message, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_message, CENTER);

    if (f_status.call == CALL_ING)
    {
        call_design(parent);
    }
    else if (f_status.timer)
    {
        timer_design(parent);
    }
    else
    {
        function_icon_design(parent);
    }

    {
        gui_img_a8_recolor(battery_bg, bg_color.color.argb_full);
        gui_img_a8_recolor(message_bg, bg_color.color.argb_full);
        gui_img_a8_recolor(message, font_color_valid.color.argb_full);
        gui_img_a8_recolor(arrow, arrow_color.color.argb_full);
        {
            uint32_t color;
            if (f_status.tx_charging)
            {
                color = charging_color.color.argb_full;
                text_tx->base.x += 5;
                gui_img_t *charging = gui_img_create_from_mem(battery_bg, 0, ICON_CHARGING_SMALL_BIN, 18, 41, 0, 0);
                gui_img_a8_recolor(charging, font_color.color.argb_full);
            }
            else if (battery_tx_val > 10)
            {
                color = font_color.color.argb_full;
            }
            else
            {
                color = FG_WARNING.color.argb_full;
            }
            gui_img_a8_recolor(tx_inner, color);
            gui_img_a8_recolor(tx_outer, color);
        }

        if (f_status.headband_connect)
        {
            uint32_t color;
            if (f_status.headband_charging)
            {
                color = charging_color.color.argb_full;
                text_headband->base.x += 5;
                gui_img_t *charging = gui_img_create_from_mem(battery_bg, 0, ICON_CHARGING_SMALL_BIN, 86, 41, 0, 0);
                gui_img_a8_recolor(charging, font_color_valid.color.argb_full);
            }
            else if (battery_headband_val > 10)
            {
                color = FG_1_LIGHT.color.argb_full;
            }
            else
            {
                color = FG_WARNING.color.argb_full;
            }
            gui_img_a8_recolor(headband, color);
        }
        else
        {
            gui_img_a8_recolor(headband, FG_2_LIGHT.color.argb_full);
        }
    }
    gui_img_a8_mix_alpha(battery_bg, battery_bg->fg_color_set >> 24);
    gui_img_a8_mix_alpha(message_bg, message_bg->fg_color_set >> 24);
    gui_img_a8_mix_alpha(arrow, arrow->fg_color_set >> 24);
    gui_img_a8_mix_alpha(headband, headband->fg_color_set >> 24);
}