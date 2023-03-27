#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_curtain.h>
#include <gui_iconlist.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include "rtk_gui_resource.h"
#include <gui_app.h>
#include "gui_button.h"
gui_win_t *win;
gui_img_t *u_1;
gui_img_t *u_2;
gui_img_t *u_3;
gui_img_t *u_4;
gui_img_t *u_5;
gui_img_t *d_6;
gui_img_t *d_7;
gui_img_t *d_8;
gui_img_t *d_9;
gui_img_t *d_10;
gui_img_t *m_1;
gui_img_t *u_6;
gui_img_t *d_11;
bool end_flag;
static void app_flybird_ui_design(gui_app_t *app_game);
//    flybird
static gui_app_t app_flybird =
{
    .screen =
    {
        .name = "app_flybird",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_flybird_ui_design,
};

void *get_app_flybird(void)
{
    return &app_flybird;
}

static bool judge_collision(int bird, int stone);
static void false_icon_callback(void *obj, gui_event_t e);
#define IMAGE1               (void *)SHORT_STONE_BIN
#define IMAGE2               (void *)LONG_STONE_BIN
#define IMAGE0               (void *)BIRD_BIN
#define IMAGE_END            (void *)END_STONE_BIN
#define STONE_SHORT_Y_OFFSET 80
#define STONE_LONG_Y_OFFSET  50
#define STONE_X_OFFSET       80
#define STONE_LENGHT         145
#define STONE_SHORT_UP_Y     (-STONE_SHORT_Y_OFFSET)
#define STONE_LONG_UP_Y      (-STONE_LONG_Y_OFFSET)
#define STONE_SHORT_DOWN_Y   (320 - STONE_LENGHT + STONE_SHORT_Y_OFFSET)
#define STONE_LONG_DOWN_Y    (320 - STONE_LENGHT + STONE_LONG_Y_OFFSET)
static uint32_t left = 0;
static int32_t bird_y = 0;
static bool judge_collision(int bird, int stone)
{
    if (bird < 0 || bird > 319)
    {
        return true;
    }
    else if ((stone >= 0 && stone <= STONE_X_OFFSET / 2 - 1) || (stone >= STONE_X_OFFSET &&
                                                                 stone <= STONE_X_OFFSET * 3 / 2 - 1))
    {
        return false;
    }
    else if (stone >= STONE_X_OFFSET / 2 && stone <= STONE_X_OFFSET - 1)
    {
        if (bird > STONE_LENGHT - STONE_LONG_Y_OFFSET && bird < STONE_SHORT_DOWN_Y)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else if (stone >= STONE_X_OFFSET * 3 / 2 && stone <= STONE_X_OFFSET * 2)
    {
        if (bird > STONE_LENGHT - STONE_SHORT_Y_OFFSET && bird < STONE_LONG_DOWN_Y)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    return false;
}
static void false_icon_callback(void *obj, gui_event_t e)
{
    gui_log("false_icon_callback");
    extern void *get_launcher_app(void);
    gui_switch_app(get_app_flybird(), get_launcher_app());
}
static bool tp;
// static void touch_button_cb(void *obj, gui_event_t e)
// {
//     tp = true;
// }
// static void touch_button_release_cb(void *obj, gui_event_t e)
// {
//     tp = false;
// }
void game_animate_cb(gui_win_t *win)
{
    left = (win)->animate->progress_percent * 160;

    if (tp)
    {
        bird_y -= 10;
    }
    else
    {
        bird_y += 10;
    }
    bool rst = judge_collision(103 + bird_y, left);
    if (rst && !end_flag)
    {
        end_flag = true;
        gui_icon_t *icon = gui_icon_create(win, "icon", 50, 50, 228, 228, 0);
        gui_log("before gui_img_create_from_mem(icon, IMAGE_END, 0,0,0,0)\n");
        gui_img_create_from_mem(icon, "end", IMAGE_END, 0, 0, 0, 0);
        gui_log("after gui_img_create_from_mem(icon, IMAGE_END, 0,0,0,0)\n");
        gui_text_t *text = gui_text_create(icon, "text1", 80, 30, 240, 32);
        gui_text_set(text, "FALSE!", "rtk_font_mem32", 0x00f8, strlen("FALSE!"), 32);
        gui_obj_add_event_cb(icon, false_icon_callback, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
    gui_img_set_attribute(u_1, NULL, IMAGE2, STONE_X_OFFSET * 0 - left,
                          STONE_SHORT_UP_Y);
    gui_img_set_attribute(u_2, NULL, IMAGE2, STONE_X_OFFSET * 1 - left,
                          STONE_LONG_UP_Y);
    gui_img_set_attribute(u_3, NULL, IMAGE2, STONE_X_OFFSET * 2 - left,
                          STONE_SHORT_UP_Y);
    gui_img_set_attribute(u_4, NULL, IMAGE2, STONE_X_OFFSET * 3 - left,
                          STONE_LONG_UP_Y);
    gui_img_set_attribute(u_5, NULL, IMAGE2, STONE_X_OFFSET * 4 - left,
                          STONE_SHORT_UP_Y);
    gui_img_set_attribute(u_6, NULL, IMAGE2, STONE_X_OFFSET * 5 - left,
                          STONE_LONG_UP_Y);
    gui_img_set_attribute(d_6, NULL, IMAGE2, STONE_X_OFFSET * 0 - left,
                          STONE_LONG_DOWN_Y);
    gui_img_set_attribute(d_7, NULL, IMAGE2, STONE_X_OFFSET * 1 - left,
                          STONE_SHORT_DOWN_Y);
    gui_img_set_attribute(d_8, NULL, IMAGE2, STONE_X_OFFSET * 2 - left,
                          STONE_LONG_DOWN_Y);
    gui_img_set_attribute(d_9, NULL, IMAGE2, STONE_X_OFFSET * 3 - left,
                          STONE_SHORT_DOWN_Y);
    gui_img_set_attribute(d_10, NULL, IMAGE2, STONE_X_OFFSET * 4 - left,
                          STONE_LONG_DOWN_Y);
    gui_img_set_attribute(d_11, NULL, IMAGE2, STONE_X_OFFSET * 5 - left,
                          STONE_SHORT_DOWN_Y);
    gui_img_set_attribute(m_1, NULL, IMAGE0, 160, 103 + bird_y);
}

static void app_flybird_ui_design(gui_app_t *app_game)
{

    win = gui_win_create(&(app_game->screen), "win", 0, 0, 320, 320);
    (win)->set_animate(win, 1000, -1, game_animate_cb, win);
    u_1 = gui_img_create_from_mem(win, "stone", IMAGE2, STONE_X_OFFSET * 0,
                                  STONE_SHORT_UP_Y, 0, 0);
    u_2 = gui_img_create_from_mem(win, "stone", IMAGE2, STONE_X_OFFSET * 1,
                                  STONE_LONG_UP_Y, 0, 0);
    u_3 = gui_img_create_from_mem(win, "stone", IMAGE2, STONE_X_OFFSET * 2,
                                  STONE_SHORT_UP_Y, 0, 0);
    u_4 = gui_img_create_from_mem(win, "stone", IMAGE2, STONE_X_OFFSET * 3,
                                  STONE_LONG_UP_Y, 0, 0);
    u_5 = gui_img_create_from_mem(win, "stone", IMAGE2, STONE_X_OFFSET * 4,
                                  STONE_SHORT_UP_Y, 0, 0);
    u_6 = gui_img_create_from_mem(win, "stone", IMAGE2, STONE_X_OFFSET * 5,
                                  STONE_LONG_UP_Y, 0, 0);
    d_6 = gui_img_create_from_mem(win, "stone", IMAGE2, STONE_X_OFFSET * 0,
                                  STONE_LONG_DOWN_Y, 0, 0);
    d_7 = gui_img_create_from_mem(win, "stone", IMAGE2, STONE_X_OFFSET * 1,
                                  STONE_SHORT_DOWN_Y, 0, 0);
    d_8 = gui_img_create_from_mem(win, "stone", IMAGE2, STONE_X_OFFSET * 2,
                                  STONE_LONG_DOWN_Y, 0, 0);
    d_9 = gui_img_create_from_mem(win, "stone", IMAGE2, STONE_X_OFFSET * 3,
                                  STONE_SHORT_DOWN_Y, 0, 0);
    d_10 = gui_img_create_from_mem(win, "stone", IMAGE2, STONE_X_OFFSET * 4,
                                   STONE_LONG_DOWN_Y, 0, 0);
    d_11 = gui_img_create_from_mem(win, "stone", IMAGE2, STONE_X_OFFSET * 5,
                                   STONE_SHORT_DOWN_Y, 0, 0);
    m_1 = gui_img_create_from_mem(win, "m_1", IMAGE0, 160, 103, 0, 0);
    end_flag = false;
    left = 0;
    bird_y = 0;
    // gui_button_t *touch_button = gui_button_create(win, "touch_button", 0, 0, gui_get_screen_width(),
    //                                                gui_get_screen_height());
    // touch_button->onPress(touch_button, touch_button_cb, NULL);
    // touch_button->onRelease(touch_button, touch_button_release_cb, NULL);
}



