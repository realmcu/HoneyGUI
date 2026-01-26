/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "app_main_watch.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "tp_algo.h"
#include "guidef.h"
#include "gui_list.h"
#include "app_main_watch.h"
#include "gui_view_instance.h"
#include "gui_rect.h"
#include "gui_arc.h"
/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "battery_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void battery_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
extern uint8_t battery_level;
extern char battery_content[4];
static gui_text_t *battery_remain_text = NULL;
static gui_view_t *current_view = NULL;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, battery_design, NULL);

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

static void update_battery_display(void *obj)
{
    GUI_UNUSED(obj);

    // Update battery arc
    GUI_WIDGET_POINTER_BY_NAME_ROOT(arc_battery, "arc_battery",
                                    current_view);
    if (arc_battery)
    {
        float progress = (float)battery_level / 100.0f;
        if (progress > 1.0f) { progress = 1.0f; }
        float battery_angle = -90.0f + 360.0f * progress;
        gui_arc_set_end_angle((gui_arc_t *)arc_battery, battery_angle);

        // Change color based on battery level
        gui_color_t color;
        if (battery_level < 20)
        {
            color = gui_rgba(255, 59, 48, 255); // Red for low battery
        }
        else
        {
            color = gui_rgba(52, 199, 89, 255); // Green for normal
        }
        gui_arc_set_color((gui_arc_t *)arc_battery, color);
    }

    // Update battery percentage text
    if (battery_remain_text)
    {
        sprintf(battery_content, "%d", battery_level);
        gui_text_content_set(battery_remain_text, battery_content, strlen(battery_content));
    }

    // Update battery title color
    GUI_WIDGET_POINTER_BY_NAME_ROOT(sport_text, "ac_text1", current_view);
    if (sport_text)
    {
        gui_color_t title_color = battery_level < 20 ? gui_rgb(200, 0, 0) : gui_rgb(0, 200, 0);
        gui_text_color_set((gui_text_t *)sport_text, title_color);
    }
}
// static void battery_press_cb(void *p)
// {
//     GUI_UNUSED(p);
//     gui_view_switch_direct(current_view, "_view",
//                            SWITCH_IN_ANIMATION_FADE, SWITCH_OUT_ANIMATION_FADE);
// }
static void battery_return_press_cb(void *p)
{
    GUI_UNUSED(p);
    gui_view_switch_direct(current_view, "bottom_view",
                           SWITCH_IN_ANIMATION_FADE, SWITCH_OUT_ANIMATION_FADE);
}
static void battery_design(gui_view_t *view)
{
    current_view = view;
    const char *obj_name = gui_view_get_current()->descriptor->name;
    if (strcmp(obj_name, "bottom_view") == 0)
    {
        gui_view_set_animate_step(current_view, 60);
        gui_view_switch_on_event(current_view, "bottom_view",
                                 SWITCH_OUT_ANIMATION_FADE,
                                 SWITCH_IN_ANIMATION_FADE,
                                 GUI_EVENT_KB_SHORT_CLICKED);
    }
    gui_obj_t *parent = GUI_BASE(view);

    // ----------time-----------------//
    gui_text_t *t_time = gui_text_create(parent, "time_b", -40, 20, 0, 0);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                 strlen(time_str), 42);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, RIGHT);
    gui_text_rendermode_set(t_time, 2);
    gui_obj_create_timer(GUI_BASE(view), 30000, true, time_update_cb);

    gui_color_t title_color = battery_level < 20 ? gui_rgb(200, 0, 0) : gui_rgb(0, 200, 0);
    gui_text_t *sport_text = gui_text_create(parent, "ac_text1",  -40, 60, 0, 0);
    gui_text_set(sport_text, "Battery", GUI_FONT_SRC_TTF, title_color,
                 strlen("Battery"), 42);
    gui_text_type_set(sport_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(sport_text, RIGHT);

    // Battery arc display using gui_lite_arc
    {
        int center_x = SCREEN_WIDTH / 2;
        int center_y = 220;
        float radius = 100.0f;
        float line_width = 24.0f;

        // Background arc (dark gray) - full circle
        gui_arc_create(parent, "arc_battery_bg",
                       center_x, center_y, (int)radius, 0, 360, line_width, gui_rgba(50, 50, 50, 255));

        // Foreground arc (green for normal battery, red for low)
        gui_color_t arc_color = battery_level < 20 ? gui_rgba(255, 59, 48, 255) : gui_rgba(52, 199, 89,
                                255);
        float progress = (float)battery_level / 100.0f;
        if (progress > 1.0f) { progress = 1.0f; }
        float battery_angle = -90.0f + 360.0f * progress;

        gui_arc_create(parent, "arc_battery",
                       center_x, center_y, (int)radius, -90, battery_angle, line_width, arc_color);

        // Battery percentage text (centered in the circle)
        // Use the global battery_content buffer from bottom_view
        sprintf(battery_content, "%d", battery_level);
        battery_remain_text = gui_text_create(parent, "battery_remain_text", center_x - 60, center_y - 40,
                                              0, 0);
        gui_text_set(battery_remain_text, battery_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                     strlen(battery_content), 85);
        gui_text_type_set(battery_remain_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(battery_remain_text, LEFT);

        // Add "%" unit text next to the number
        gui_text_t *battery_unit_text = gui_text_create(parent, "battery_unit_text", center_x + 25,
                                                        center_y - 40, 0, 0);
        gui_text_set(battery_unit_text, "%", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen("%"), 85);
        gui_text_type_set(battery_unit_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(battery_unit_text, LEFT);

        // Create timer to update battery display
        gui_obj_create_timer(parent, 1000, true, update_battery_display);
    }
    {
        gui_rounded_rect_t *battery_rect = gui_rect_create(parent, "battery_rect", 35, 400,
                                                           340, 80, 40, gui_rgba(200, 200, 200, 255 * 0.7));
        gui_text_t *battery_text = gui_text_create(battery_rect, "battery_text", 35, 20, 0, 0);
        gui_text_set(battery_text, "Low Power Mode", GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                     strlen("Low Power Mode"), 36);
        gui_text_type_set(battery_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(battery_text, LEFT);
    }

    gui_img_t *battery_return = gui_img_create_from_mem(parent, "battery_return", DELETE_ICON_BIN,
                                                        30, 30, 0, 0);
    gui_obj_add_event_cb(GUI_BASE(battery_return), (gui_event_cb_t)battery_return_press_cb,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

}


