#include <gui_tabview.h>
#include <gui_obj.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "draw_font.h"
#include <gui_win.h>
#include "gui_img.h"
#include "gui_obj.h"
extern void enter_tablist(void *obj, gui_event_t e);
uint8_t APP_AMOUNT = 9;


#ifdef RTL87x2G
void *app_image_list[9] =
{
    ZIP_APP_SPORT_BIN,
    ZIP_APP_HEART_RATE_BIN,
    ZIP_APP_BLOOD_OXYGEN_BIN,
    ZIP_APP_CALL_BIN,
    ZIP_APP_MUSIC_BIN,
    ZIP_APP_VOICE_NOTE_BIN,
    ZIP_APP_WEATHER_BIN,
    ZIP_APP_BREATH_BIN,
    ZIP_APP_SLEEP_BIN,
};
#else
void *app_image_list[9] =
{
    APP_SPORT_BIN,
    APP_HEART_RATE_BIN,
    APP_BLOOD_OXYGEN_BIN,
    APP_CALL_BIN,
    APP_MUSIC_BIN,
    APP_VOICE_NOTE_BIN,
    APP_WEATHER_BIN,
    APP_BREATH_BIN,
    APP_SLEEP_BIN,
};
#endif // RTL87x2G
void design_tab_sport(void *parent)
{
    gui_img_t *home_bg = gui_img_create_from_mem(parent, "design_tab_sport",
                                                 app_image_list[0], 0, 0, 0,
                                                 0);
    gui_win_t *win = gui_win_create(parent, "win", 0, 0, ((gui_tab_t *)parent)->base.w * APP_AMOUNT,
                                    384);
    gui_obj_add_event_cb(win, (gui_event_cb_t)enter_tablist, GUI_EVENT_TOUCH_LONG, NULL);
}
void design_tab_heart(void *parent)
{
    gui_img_t *home_bg = gui_img_create_from_mem(parent, "design_tab_heart",
                                                 app_image_list[1], 0,
                                                 0, 0, 0);
}
void design_tab_blood(void *parent)
{
    gui_img_t *home_bg = gui_img_create_from_mem(parent, "design_tab_blood",
                                                 app_image_list[2], 0,
                                                 0, 0, 0);
}
void design_tab_call(void *parent)
{
    gui_img_t *home_bg = gui_img_create_from_mem(parent, "design_tab_call",
                                                 app_image_list[3], 0, 0, 0,
                                                 0);
}
void design_tab_music(void *parent)
{
    gui_img_t *home_bg = gui_img_create_from_mem(parent, "design_tab_music",
                                                 app_image_list[4], 0, 0, 0,
                                                 0);
}
void design_tab_note(void *parent)
{
    gui_img_t *home_bg = gui_img_create_from_mem(parent, "design_tab_note",
                                                 app_image_list[5], 0,
                                                 0, 0, 0);
}
void design_tab_weather(void *parent)
{
    gui_img_t *home_bg = gui_img_create_from_mem(parent, "design_tab_weather",
                                                 app_image_list[6], 0, 0,
                                                 0, 0);
}
void design_tab_breath(void *parent)
{
    gui_img_t *home_bg = gui_img_create_from_mem(parent, "design_tab_breath",
                                                 app_image_list[7], 0, 0, 0,
                                                 0);
}
void design_tab_sleep(void *parent)
{
    gui_img_t *home_bg = gui_img_create_from_mem(parent, "design_tab_sleep",
                                                 app_image_list[8], 0, 0, 0,
                                                 0);
}