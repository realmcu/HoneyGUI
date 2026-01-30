/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "app_main_watch.h"
#include "gui_view_instance.h"
#include "gui_list.h"
#include "gui_text.h"
#include "gui_arc.h"
#include "gui_rect.h"
/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "compass_view"
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void compass_design(gui_view_t *view);
static void compass_clean(gui_view_t *view);

extern int16_t g_selected_workout_index;

/*============================================================================*
 *                           GUI_VIEW_INSTANCE
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, compass_design, NULL);

/*============================================================================*
 *                            Variables
 *============================================================================*/
// static gui_list_t *list = NULL;
static gui_view_t *current_view = NULL;
static gui_win_t *compass_win = NULL;
static gui_img_t *compass_img = NULL;
static gui_text_t *compass_text = NULL;
int16_t current_angle = 315;  // Initial angle: NW 315° (exported for compass_info_view)
static const int16_t ICON_DEFAULT_ANGLE = 315;  // Icon's default direction is NW 315°
const char *direction_names[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
                                 "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"
                                };
char compass_display_str[32];

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
// Clean up compass resources
static void compass_clean(gui_view_t *view)
{
    GUI_UNUSED(view);
    current_angle = 0;
}

// Get direction name based on angle (0-360)
const char *get_direction_name(int16_t angle)
{
    // Normalize angle to 0-360
    angle = angle % 360;
    if (angle < 0) { angle += 360; }

    // Each direction covers 22.5 degrees (360/16)
    int16_t index = (angle + 11) / 23;  // +11 for rounding
    if (index >= 16) { index = 0; }

    return direction_names[index];
}

// Update compass display with new angle
static void update_compass_display(int16_t angle)
{
    current_angle = angle;

    // Update compass image rotation
    // Icon's default direction is 315°, rotate counterclockwise to reach target angle
    // Rotation = -(315° - target_angle) = target_angle - 315°
    // But if gui_img_rotation is clockwise, we need: 315° - target_angle
    if (compass_img)
    {
        int16_t rotation_angle = ICON_DEFAULT_ANGLE - angle;
        gui_img_rotation(compass_img, rotation_angle);
    }

    // Update combined text (e.g., "NW~315°")
    if (compass_text)
    {
        const char *dir = get_direction_name(angle);
        sprintf(compass_display_str, "%s~%d°", dir, angle);
        gui_text_content_set(compass_text, compass_display_str, strlen(compass_display_str));
    }
}

// Timer callback for simulating compass data changes
static void compass_update_cb(void *p)
{
    GUI_UNUSED(p);

    // Simulate compass rotation: increment angle by 5 degrees each update
    int16_t new_angle = (current_angle + 5) % 360;
    update_compass_display(new_angle);
}

static void time_update_cb(void *p)
{
    GUI_UNUSED(p);
    if (!timeinfo)
    {
        return;
    }
    else
    {
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(t_time, "time_b", current_view);
            gui_text_content_set((gui_text_t *)t_time, time_str, strlen(time_str));
        }

    }
}

static void info_img_press_cb(void *p)
{
    GUI_UNUSED(p);
    gui_view_switch_direct(gui_view_get_current(), "compass_info_view", SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
static void compass_design(gui_view_t *view)
{
    current_view = view;
    const char *obj_name = gui_view_get_current()->descriptor->name;
    VIEW_SWITCH_STYLE swtich_in = SWITCH_IN_ANIMATION_FADE;
    VIEW_SWITCH_STYLE swtich_out = SWITCH_OUT_ANIMATION_FADE;
    if (strcmp(obj_name, "bottom_view") == 0)
    {
        gui_view_set_animate_step(current_view, 60);
        swtich_in = SWITCH_IN_FROM_TOP_USE_TRANSLATION;
        swtich_out = SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION;
        gui_view_switch_on_event(current_view, "bottom_view",
                                 swtich_out,
                                 swtich_in,
                                 GUI_EVENT_KB_SHORT_PRESSED);
    }
    else if (strcmp(obj_name, "menu_view") == 0)
    {
        gui_view_switch_on_event(current_view, "menu_view",
                                 swtich_out,
                                 swtich_in,
                                 GUI_EVENT_KB_SHORT_PRESSED);
    }
    else
    {
        gui_view_switch_on_event(current_view, "watchface_view",
                                 swtich_out,
                                 swtich_in,
                                 GUI_EVENT_KB_SHORT_PRESSED);
    }
    gui_obj_t *parent = GUI_BASE(view);

    uint16_t img_width = 0;
    uint16_t img_height = 0;
    gui_rect_create(parent, "compass_bg", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, gui_rgb(0xf0, 0xf0,
                    0xf0));
    gui_img_t *info_img_bg = gui_img_create_from_mem(parent, "info_img_bg", F_72_72_CIRCLE_A8_BIN, 30,
                                                     23, 0, 0);
    img_width = gui_img_get_width(info_img_bg);
    img_height = gui_img_get_height(info_img_bg);
    gui_img_a8_recolor(info_img_bg, GUI_COLOR_ARGB8888(255, 200, 200, 200));
    gui_obj_add_event_cb(GUI_BASE(info_img_bg), (gui_event_cb_t)info_img_press_cb,
                         GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_t *delete_img = gui_img_create_from_mem(parent, "delete_img", DELETE_ICON_BIN, 30, 23, 0,
                                                    0);
    gui_obj_hidden(GUI_BASE(delete_img), true);

    gui_text_t *infor_text = gui_text_create(info_img_bg, "compass_text", 0, 0, img_width, img_height);
    gui_text_set(infor_text, "i", GUI_FONT_SRC_TTF, gui_rgb(10, 10, 10),
                 strlen("i"), 48);
    gui_text_type_set(infor_text, INTER_24PT_MEDIUM_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(infor_text, MID_CENTER);
    gui_text_rendermode_set(infor_text, 2);

    compass_win = gui_win_create(parent, "compass_win", 0, 100, SCREEN_WIDTH, SCREEN_HEIGHT - 100);
    compass_img = gui_img_create_from_mem(compass_win, "compass_img", COMPASS_ICON_BIN, 205, 150, 0, 0);

    // Set rotation center to image center
    img_width = gui_img_get_width(compass_img);
    img_height = gui_img_get_height(compass_img);
    gui_img_set_focus(compass_img, img_width / 2.0f, img_height / 2.0f);
    // Icon's default direction is 315°, no rotation needed initially
    gui_img_rotation(compass_img, 0);

    // Combined direction and angle text (e.g., "NW~315°")
    compass_text = gui_text_create(compass_win, "compass_text", 0, 300, SCREEN_WIDTH, 80);
    sprintf(compass_display_str, "%s~%d°", get_direction_name(current_angle), current_angle);
    gui_text_set(compass_text, compass_display_str, GUI_FONT_SRC_TTF, gui_rgb(10, 10, 10),
                 strlen(compass_display_str), 60);
    gui_text_type_set(compass_text, INTER_24PT_MEDIUM_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(compass_text, MID_CENTER);
    gui_text_rendermode_set(compass_text, 2);


    // ----------time-----------------//
    gui_text_t *t_time = gui_text_create(view, "time_b", 0, 0, 380, 80);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(10, 10, 10),
                 strlen(time_str), 34);
    gui_text_type_set(t_time, INTER_24PT_MEDIUM_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, MID_RIGHT);
    gui_text_rendermode_set(t_time, 2);

    gui_obj_create_timer(GUI_BASE(view), 30000, true, time_update_cb);
    gui_obj_create_timer(GUI_BASE(compass_win), 2000, true,
                         compass_update_cb);  // Update compass every 2 seconds


}
