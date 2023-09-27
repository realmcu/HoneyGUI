#include <gui_tabview.h>
#include <gui_obj.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "draw_font.h"
#include <gui_magic_img.h>
#include "gui_win.h"

gui_magic_img_t *hour;
gui_magic_img_t *minute;
gui_magic_img_t *second;
gui_magic_img_t *home_bg;
gui_magic_img_t *dynamic_island_rect;

float degree = 0;

void turn(void)
{
    degree += 0.01;
    gui_img_rotation(hour, degree, 8, 90);
    gui_img_rotation(minute, degree * 5, 8, 140);
    gui_img_rotation(second, degree * 5 * 5, 7, 143);
}
void show_clock(void *obj, gui_event_t e)
{
    if (hour->base.base.not_show == false)
    {
        gui_obj_show(hour, false);
        gui_obj_show(minute, false);
        gui_obj_show(second, false);
        gui_img_set_attribute(home_bg, "home_bg", HOME_WATCH1_BIN, 0, 0);
    }
    else
    {
        gui_obj_show(hour, true);
        gui_obj_show(minute, true);
        gui_obj_show(second, true);
        gui_img_set_attribute(home_bg, "home_bg", HOME_BACKGROUND_BIN, 0, 0);
    }
}
void switch_dynamic_island(void *obj, gui_event_t e)
{
    if (dynamic_island_rect->base.base.not_show == false)
    {
        gui_obj_show(dynamic_island_rect, false);
    }
    else
    {
        gui_obj_show(dynamic_island_rect, true);
    }
}
void design_tab_home(void *parent)
{
    home_bg = gui_magic_img_create_from_mem(parent, "home_bg", HOME_BACKGROUND_BIN, 0,
                                            0, 0, 0);
    hour = gui_magic_img_create_from_mem(parent, "hour", TIME_HOUR_BIN, 160, 192, 0, 0);
    minute = gui_magic_img_create_from_mem(parent, "minute", TIME_MUNITE_BIN, 160, 192, 0, 0);
    second = gui_magic_img_create_from_mem(parent, "second", TIME_SECOND_BIN, 160, 192, 0, 0);

    dynamic_island_rect = gui_magic_img_create_from_mem(parent, "dynamic_island_rect",
                                                        DYNAMIC_ISLAND_POINT_BIN, 147, 35, 0, 0);
    gui_obj_show(dynamic_island_rect, false);

    gui_win_t *clock = gui_win_create(parent, "clock", 0, 84, 320, 300);
    gui_obj_add_event_cb(clock, (gui_event_cb_t)show_clock, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_win_t *dynamic_island = gui_win_create(parent, "clock", 0, 0, 320, 84);
    gui_obj_add_event_cb(dynamic_island, (gui_event_cb_t)switch_dynamic_island, GUI_EVENT_TOUCH_CLICKED,
                         NULL);

    gui_magicimage_set_animate(hour, 100000, 0xffffffff, turn, NULL);
}
