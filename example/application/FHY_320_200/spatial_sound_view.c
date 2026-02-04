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

/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME SPATIAL_SOUND_VIEW

#define BUTTON_X_LEFT 0
#define BUTTON_X_MID  99
#define BUTTON_X_RIGHT 198

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void spatial_sound_view_design(gui_view_t *view);

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
    .on_switch_in = spatial_sound_view_design,
};

static const char *descriptor_rec = NULL;
static uint8_t button_bg_target_x = 0;

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

static void click_button_back(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    f_status.password = !f_status.password; // do not restart spatial_sound
    if (!descriptor_rec) { return; }
    gui_view_switch_direct(current_view, descriptor_rec, SWITCH_OUT_ANIMATION_MOVE_TO_RIGHT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_LEFT);
}

static void button_move(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    int step = 20;

    if (obj->x == button_bg_target_x)
    {
        update_icon_color(obj->parent, spatial_sound_type);
        gui_obj_delete_timer(obj);
    }
    else if (obj->x < button_bg_target_x)
    {
        obj->x += step;
        if (obj->x > button_bg_target_x)
        {
            obj->x = button_bg_target_x;
        }
    }
    else
    {
        obj->x -= step;
        if (obj->x < button_bg_target_x)
        {
            obj->x = button_bg_target_x;
        }
    }
}

static void click_button_controlboard(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);

    gui_obj_t *o = GUI_BASE(obj);
    if (strcmp(o->name, "l") == 0)
    {
        spatial_sound_type = SPATIAL_SOUND_MOVIE;
        button_bg_target_x = BUTTON_X_LEFT;
    }
    else if (strcmp(o->name, "m") == 0)
    {
        spatial_sound_type = SPATIAL_SOUND_MUSIC;
        button_bg_target_x = BUTTON_X_MID;
    }
    else
    {
        spatial_sound_type = SPATIAL_SOUND_GAME;
        button_bg_target_x = BUTTON_X_RIGHT;
    }
    gui_obj_create_timer(gui_list_entry(o->parent->child_list.next, gui_obj_t, brother_list), 10, true,
                         button_move);
}

static void spatial_sound_view_design(gui_view_t *view)
{
    if (gui_view_get_current())
    {
        descriptor_rec = gui_view_get_current()->descriptor->name;
    }

    gui_view_set_animate_step(view, 10);
    gui_obj_t *parent = GUI_BASE(view);

    gui_color_t font_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
    }

    gui_text_t *t = gui_text_create(parent, 0, 0, 0, 320, 60);
    gui_text_set(t, (void *)page_name_array[language_index][SPATIAL_SOUND], GUI_FONT_SRC_BMP,
                 font_color,
                 strlen((void *)page_name_array[language_index][SPATIAL_SOUND]), 30);
    gui_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t, MID_CENTER);
    gui_win_t *win_icon_back = (gui_win_t *)gui_win_create(parent, 0, 0, 0, 60, 60);
    gui_img_t *icon_back = gui_img_create_from_mem(win_icon_back, 0, ICON_BACK_BIN, 0, 8, 0, 0);
    gui_obj_add_event_cb(win_icon_back, click_button_back, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_a8_recolor(icon_back, font_color.color.argb_full);

    gui_img_t *control_bg = gui_img_create_from_mem(parent, 0, CONTROL_BG_BIN, 12, 92, 0, 0);
    gui_img_t *button_bg = gui_img_create_from_mem(control_bg,
                                                   (void *)page_name_array[language_index][SPATIAL_SOUND],
                                                   BUTTON_BG_ELLIPSE_BIN,
                                                   spatial_sound_type * 99, 0, 0, 0);
    gui_img_t *movie = gui_img_create_from_mem(control_bg, "l", SPATIAL_SOUND_MOVIE_BIN, 25, 21, 0,
                                               0);
    gui_img_t *music = gui_img_create_from_mem(control_bg, "m", SPATIAL_SOUND_MUSIC_BIN, 124, 21, 0,
                                               0);
    gui_img_t *game = gui_img_create_from_mem(control_bg, "r", SPATIAL_SOUND_GAME_BIN, 223, 21, 0,
                                              0);
    update_icon_color((void *)control_bg, spatial_sound_type);
    gui_img_a8_recolor(movie, FG_WHITE.color.argb_full);
    gui_img_a8_recolor(music, FG_WHITE.color.argb_full);
    gui_img_a8_recolor(game, FG_WHITE.color.argb_full);
    gui_obj_add_event_cb(movie, click_button_controlboard, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(music, click_button_controlboard, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(game, click_button_controlboard, GUI_EVENT_TOUCH_CLICKED, NULL);

    if (theme_bg_white)
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_LIGHT.color.argb_full);
        gui_img_a8_recolor(button_bg, BG_THEME1_BRIGHT_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(control_bg, BG_THEME1_DARK_DARK.color.argb_full);
        gui_img_a8_recolor(button_bg, BG_THEME1_BRIGHT_DARK.color.argb_full);
    }
}
