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

extern void enter_tablist(void *obj, gui_event_t e);
uint8_t APP_AMOUNT = 9;

void design_tab_sport(void *parent)
{
    gui_magic_img_t *home_bg = gui_magic_img_create_from_mem(parent, "home_bg", APP_SPORT_BIN, 0, 0, 0,
                                                             0);
    gui_win_t *win = gui_win_create(parent, "win", 0, 0, ((gui_tab_t *)parent)->base.w * APP_AMOUNT,
                                    384);
    gui_obj_add_event_cb(win, (gui_event_cb_t)enter_tablist, GUI_EVENT_TOUCH_LONG, NULL);
}
void design_tab_heart(void *parent)
{
    gui_magic_img_t *home_bg = gui_magic_img_create_from_mem(parent, "home_bg", APP_HEART_RATE_BIN, 0,
                                                             0, 0, 0);
}
void design_tab_blood(void *parent)
{
    gui_magic_img_t *home_bg = gui_magic_img_create_from_mem(parent, "home_bg", APP_BLOOD_OXYGEN_BIN, 0,
                                                             0, 0, 0);
}
void design_tab_call(void *parent)
{
    gui_magic_img_t *home_bg = gui_magic_img_create_from_mem(parent, "home_bg", APP_CALL_BIN, 0, 0, 0,
                                                             0);
}
void design_tab_music(void *parent)
{
    gui_magic_img_t *home_bg = gui_magic_img_create_from_mem(parent, "home_bg", APP_MUSIC_BIN, 0, 0, 0,
                                                             0);
}
void design_tab_note(void *parent)
{
    gui_magic_img_t *home_bg = gui_magic_img_create_from_mem(parent, "home_bg", APP_VOICE_NOTE_BIN, 0,
                                                             0, 0, 0);
}
void design_tab_weather(void *parent)
{
    gui_magic_img_t *home_bg = gui_magic_img_create_from_mem(parent, "home_bg", APP_WEATHER_BIN, 0, 0,
                                                             0, 0);
}
void design_tab_breath(void *parent)
{
    gui_magic_img_t *home_bg = gui_magic_img_create_from_mem(parent, "home_bg", APP_BREATH_BIN, 0, 0, 0,
                                                             0);
}
void design_tab_sleep(void *parent)
{
    gui_magic_img_t *home_bg = gui_magic_img_create_from_mem(parent, "home_bg", APP_SLEEP_BIN, 0, 0, 0,
                                                             0);
}