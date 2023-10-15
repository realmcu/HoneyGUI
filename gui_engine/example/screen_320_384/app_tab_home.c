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
gui_win_t *dynamic_island;
gui_win_t *dynamic_set_start;
float degree = 0;
uint32_t time_count = 0;
uint8_t dynamic_island_count = 0;
extern void enter_app_set_start(void *obj, gui_event_t e);
extern void enter_homelist(void *obj, gui_event_t e);

void switch_dynamic_island(void *obj, gui_event_t e);
#ifdef RTL8762G
void *home[2] =
{
    ZIP_HOME_BACKGROUND_BIN,
    ZIP_HOME_WATCH1_BIN,
};
#else
void *home[2] =
{
    HOME_BACKGROUND_BIN,
    HOME_WATCH1_BIN,
};
#endif // RTL8762G

void turn(void)
{
    degree += 0.01;
    gui_img_rotation(hour, degree, 8, 90);
    gui_img_rotation(minute, degree * 5, 8, 140);
    gui_img_rotation(second, degree * 5 * 5, 7, 143);
    if (++time_count == 100)
    {
        time_count = 0;
        switch_dynamic_island(NULL, 0);
    }

}
void show_clock(void *obj, gui_event_t e)
{
    if (hour->base.base.not_show == false)
    {
        gui_obj_show(hour, false);
        gui_obj_show(minute, false);
        gui_obj_show(second, false);
        gui_img_set_attribute((gui_img_t *)home_bg, "home_bg", home[1], 0, 0);
    }
    else
    {
        gui_obj_show(hour, true);
        gui_obj_show(minute, true);
        gui_obj_show(second, true);
        gui_img_set_attribute((gui_img_t *)home_bg, "home_bg", home[0], 0, 0);
    }
}
void switch_dynamic_island(void *obj, gui_event_t e)
{
    dynamic_island_count ++;
    if (dynamic_island_count == 1)
    {
        dynamic_island_rect = gui_magic_img_create_from_mem(dynamic_island, "dynamic_island_rect1",
                                                            DYNAMIC_ISLAND_POINT_BIN, 107, 35, 0, 0);
    }
    else if (dynamic_island_count == 2)
    {
        gui_tree_free((gui_obj_t *)dynamic_island_rect);
        dynamic_island_rect = gui_magic_img_create_from_mem(dynamic_island, "dynamic_island_rect2",
                                                            DYNAMIC_ISLAND_PIC1_BIN, -10, 25, 0, 0);
        // dynamic_island_rect = gui_magic_img_create_from_mem(dynamic_island, "dynamic_islcand_pic0",
        //                                                     DYNAMIC_ISLAND_260_66_BIN, 30, 25, 0, 0);
        // gui_magic_img_t *dynamic_islcand_pic1 = gui_magic_img_create_from_mem(dynamic_island_rect, "dynamic_islcand_pic1",
        //                                                     DYNAMIC_ISLAND_MUSIC_BIN, 10, 10, 0, 0);
        // gui_magic_img_t *dynamic_islcand_pic2 = gui_magic_img_create_from_mem(dynamic_island_rect, "dynamic_islcand_pic2",
        //                                                     DYNAMIC_ISLAND_RECORDING_START_BIN, 204, 10, 0, 0);
    }
    else if (dynamic_island_count == 3)
    {
        gui_tree_free((gui_obj_t *)dynamic_island_rect);
        dynamic_island_rect = gui_magic_img_create_from_mem(dynamic_island, "dynamic_island_rect3",
                                                            DYNAMIC_ISLAND_PIC2_BIN, -10, 25, 0, 0);
        // dynamic_island_rect = gui_magic_img_create_from_mem(dynamic_island, "dynamic_islcand_pic0",
        //                                                     DYNAMIC_ISLAND_260_66_BIN, 30, 25, 0, 0);
        // gui_magic_img_t *dynamic_islcand_pic1 = gui_magic_img_create_from_mem(dynamic_island_rect, "dynamic_islcand_pic1",
        //                                                     DYNAMIC_ISLAND_RECORDING_BIN, 10, 10, 0, 0);
        // gui_magic_img_t *dynamic_islcand_pic2 = gui_magic_img_create_from_mem(dynamic_island_rect, "dynamic_islcand_pic2",
        //                                                     DYNAMIC_ISLAND_RECORDING_START_BIN, 204, 10, 0, 0);
    }
    else if (dynamic_island_count == 4)
    {
        gui_tree_free((gui_obj_t *)dynamic_island_rect);
        dynamic_island_rect = gui_magic_img_create_from_mem(dynamic_island, "dynamic_island_rect40",
                                                            DYNAMIC_ISLAND_PIC3_BIN, -10, 25, 0, 0);
        // dynamic_island_rect = gui_magic_img_create_from_mem(dynamic_island, "dynamic_islcand_pic0",
        //                                                     DYNAMIC_ISLAND_260_66_BIN, 30, 25, 0, 0);
        gui_magic_img_t *dynamic_islcand_pic1 = gui_magic_img_create_from_mem(dynamic_island_rect,
                                                                              "dynamic_island_rect41",
                                                                              DYNAMIC_ISLAND_WECHAT_BIN, 10, 10, 0, 0);
    }
    else if (dynamic_island_count == 5)
    {
        gui_tree_free((gui_obj_t *)dynamic_island_rect);
        dynamic_island_rect = gui_magic_img_create_from_mem(dynamic_island, "dynamic_island_rect5",
                                                            DYNAMIC_ISLAND_PIC4_BIN, -10, 25, 0, 0);
        // dynamic_island_rect = gui_magic_img_create_from_mem(dynamic_island, "dynamic_islcand_pic0",
        //                                                     DYNAMIC_ISLAND_260_66_BIN, 30, 25, 0, 0);
        // gui_magic_img_t *dynamic_islcand_pic1 = gui_magic_img_create_from_mem(dynamic_island_rect, "dynamic_islcand_pic1",
        //                                                     DYNAMIC_ISLAND_EARPHONE_BIN, 15, 15, 0, 0);
    }
    else if (dynamic_island_count == 6)
    {
        gui_tree_free((gui_obj_t *)dynamic_island_rect);
        dynamic_island_rect = gui_magic_img_create_from_mem(dynamic_island, "dynamic_island_rect6",
                                                            DYNAMIC_ISLAND_260_66_BIN, -10, 25, 0, 0);
        gui_magic_img_t *dynamic_islcand_pic1 = gui_magic_img_create_from_mem(dynamic_island_rect,
                                                                              "dynamic_island_rect_pic1",
                                                                              DYNAMIC_ISLAND_SPORT_BIN, 10, 10, 0, 0);
        gui_magic_img_t *dynamic_islcand_pic2 = gui_magic_img_create_from_mem(dynamic_island_rect,
                                                                              "dynamic_island_rect_pic2",
                                                                              DYNAMIC_ISLAND_BPM_ICON_BIN, 82, 12, 0, 0);
        gui_magic_img_t *dynamic_islcand_pic3 = gui_magic_img_create_from_mem(dynamic_island_rect,
                                                                              "dynamic_island_rect_pic3",
                                                                              DYNAMIC_ISLAND_BPM_NUM_BIN, 112, 14, 0, 0);
        gui_magic_img_t *dynamic_islcand_pic4 = gui_magic_img_create_from_mem(dynamic_island_rect,
                                                                              "dynamic_island_rect_pic4",
                                                                              DYNAMIC_ISLAND_BPM_BIN, 146, 14, 0, 0);
        gui_magic_img_t *dynamic_islcand_pic5 = gui_magic_img_create_from_mem(dynamic_island_rect,
                                                                              "dynamic_island_rect_pic5",
                                                                              DYNAMIC_ISLAND_STEPS_ICON_BIN, 82, 40, 0, 0);
        gui_magic_img_t *dynamic_islcand_pic6 = gui_magic_img_create_from_mem(dynamic_island_rect,
                                                                              "dynamic_island_rect_pic6",
                                                                              DYNAMIC_ISLAND_STEPS_NUM_BIN, 112, 40, 0, 0);
        gui_magic_img_t *dynamic_islcand_pic7 = gui_magic_img_create_from_mem(dynamic_island_rect,
                                                                              "dynamic_island_rect_pic7",
                                                                              DYNAMIC_ISLAND_STEPS_BIN, 166, 40, 0, 0);
    }
    else
    {
        gui_tree_free((gui_obj_t *)dynamic_island_rect);
        dynamic_island_count = 0;
    }
}

void design_tab_home(void *parent)
{
    home_bg = gui_magic_img_create_from_mem(parent, "home_bg", home[0], 0,
                                            0, 0, 0);
    hour = gui_magic_img_create_from_mem(parent, "hour", TIME_HOUR_BIN, 160, 192, 0, 0);
    minute = gui_magic_img_create_from_mem(parent, "minute", TIME_MUNITE_BIN, 160, 192, 0, 0);
    second = gui_magic_img_create_from_mem(parent, "second", TIME_SECOND_BIN, 160, 192, 0, 0);

    gui_win_t *clock = gui_win_create(parent, "clock", 0, 84, 320, 300);
    gui_obj_add_event_cb(clock, (gui_event_cb_t)show_clock, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(clock, (gui_event_cb_t)enter_homelist, GUI_EVENT_TOUCH_LONG, NULL);

    dynamic_island = gui_win_create(parent, "dynamic_island", 40, 0, 280, 84);
    gui_obj_add_event_cb(dynamic_island, (gui_event_cb_t)switch_dynamic_island, GUI_EVENT_TOUCH_CLICKED,
                         NULL);
    dynamic_set_start = gui_win_create(parent, "dynamic_island", 0, 0, 40, 40);
    gui_obj_add_event_cb(dynamic_island, (gui_event_cb_t)enter_app_set_start, GUI_EVENT_TOUCH_CLICKED,
                         NULL);
    gui_magicimage_set_animate(second, 100000, 0xffffffff, turn, NULL);

}
