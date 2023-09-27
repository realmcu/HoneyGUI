#include <gui_tabview.h>
#include <gui_obj.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "draw_font.h"
#include <gui_magic_img.h>
#include <gui_win.h>

#define APP_OFFSET_X 9
#define APP_OFFSET_Y 4
#define RECT_OFFSET_X 5
#define RECT_OFFSET_Y 0
extern void exit_tablist(void *obj, gui_event_t e);
extern uint8_t APP_AMOUNT;
void design_tablist_sport(void *parent)
{
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img", APP_SPORT_BIN,
                                                                 APP_OFFSET_X, APP_OFFSET_Y, 0,
                                                                 0);
    gui_img_scale(tablist_img, 0.6, 0.6);
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", TABLIST_RECT_BIN,
                                                          RECT_OFFSET_X, RECT_OFFSET_Y, 0, 0);
    gui_win_t *win = gui_win_create(parent, "win", ((gui_tab_t *)parent)->base.x,
                                    ((gui_tab_t *)parent)->base.y, ((gui_tab_t *)parent)->base.w * APP_AMOUNT,
                                    ((gui_tab_t *)parent)->base.h);
    gui_obj_add_event_cb(win, (gui_event_cb_t)exit_tablist, GUI_EVENT_TOUCH_CLICKED, NULL);
}
void design_tablist_heart(void *parent)
{
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", TABLIST_RECT_BIN,
                                                          RECT_OFFSET_X, RECT_OFFSET_Y, 0, 0);
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img",
                                                                 APP_HEART_RATE_BIN, APP_OFFSET_X,
                                                                 APP_OFFSET_Y, 0, 0);
    gui_img_scale(tablist_img, 0.6, 0.6);
}
void design_tablist_blood(void *parent)
{
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", TABLIST_RECT_BIN,
                                                          RECT_OFFSET_X, RECT_OFFSET_Y, 0, 0);
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img",
                                                                 APP_BLOOD_OXYGEN_BIN, APP_OFFSET_X,
                                                                 APP_OFFSET_Y, 0, 0);
    gui_img_scale(tablist_img, 0.6, 0.6);
}
void design_tablist_call(void *parent)
{
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", TABLIST_RECT_BIN,
                                                          RECT_OFFSET_X, RECT_OFFSET_Y, 0, 0);
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img", APP_CALL_BIN,
                                                                 APP_OFFSET_X, APP_OFFSET_Y, 0,
                                                                 0);
    gui_img_scale(tablist_img, 0.6, 0.6);
}
void design_tablist_music(void *parent)
{
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", TABLIST_RECT_BIN,
                                                          RECT_OFFSET_X, RECT_OFFSET_Y, 0, 0);
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img", APP_MUSIC_BIN,
                                                                 APP_OFFSET_X, APP_OFFSET_Y, 0,
                                                                 0);
    gui_img_scale(tablist_img, 0.6, 0.6);
}
void design_tablist_note(void *parent)
{
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", TABLIST_RECT_BIN,
                                                          RECT_OFFSET_X, RECT_OFFSET_Y, 0, 0);
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img",
                                                                 APP_VOICE_NOTE_BIN, APP_OFFSET_X,
                                                                 APP_OFFSET_Y, 0, 0);
    gui_img_scale(tablist_img, 0.6, 0.6);
}
void design_tablist_weather(void *parent)
{
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", TABLIST_RECT_BIN,
                                                          RECT_OFFSET_X, RECT_OFFSET_Y, 0, 0);
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img", APP_WEATHER_BIN,
                                                                 APP_OFFSET_X, APP_OFFSET_Y,
                                                                 0, 0);
    gui_img_scale(tablist_img, 0.6, 0.6);
}
void design_tablist_breath(void *parent)
{
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", TABLIST_RECT_BIN,
                                                          RECT_OFFSET_X, RECT_OFFSET_Y, 0, 0);
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img", APP_BREATH_BIN,
                                                                 APP_OFFSET_X, APP_OFFSET_Y, 0,
                                                                 0);
    gui_img_scale(tablist_img, 0.6, 0.6);
}
void design_tablist_sleep(void *parent)
{
    gui_magic_img_t *rect = gui_magic_img_create_from_mem(parent, "rect", TABLIST_RECT_BIN,
                                                          RECT_OFFSET_X, RECT_OFFSET_Y, 0, 0);
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img", APP_SLEEP_BIN,
                                                                 APP_OFFSET_X, APP_OFFSET_Y, 0,
                                                                 0);
    gui_img_scale(tablist_img, 0.6, 0.6);
}