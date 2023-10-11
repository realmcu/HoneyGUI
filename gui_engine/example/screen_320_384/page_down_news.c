
#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_magic_img.h>
#include <gui_curtain.h>
#include "gui_cardview.h"
#include "gui_card.h"
#include "gui_tab.h"
#include "gui_canvas.h"

static void canvas_cb(gui_canvas_t *canvas)
{
    nvgRect(canvas->vg, 0, 0, 320, 384);
    nvgFillColor(canvas->vg, nvgRGBA(30, 30, 30, 255));
    nvgFill(canvas->vg);
}
void page_down_message_health(void *parent)
{

    gui_magic_img_t *health_message =  gui_magic_img_create_from_mem(parent, "health_message",
                                                                     CARD_HEALTH_BIN,
                                                                     10, 0, 0, 0);
    gui_magic_img_t *health_watch =  gui_magic_img_create_from_mem(parent, "card_time",
                                                                   CARD_TIME_BIN,
                                                                   170, -138, 0, 0);
    gui_img_set_opacity(health_watch, 50);
    gui_magic_img_t *week =  gui_magic_img_create_from_mem(parent, "time", SUN_BIN, 25, -90, 0, 0);
    gui_magic_img_t *data =  gui_magic_img_create_from_mem(parent, "data", DATA_BIN, 25, -37, 0, 0);
}
void page_down_message_music(void *parent)
{
    gui_magic_img_t *muisc_message =  gui_magic_img_create_from_mem(parent, "muisc_message",
                                                                    CARD_MUSIC1_BIN,
                                                                    10, 0, 0, 0);

}
void page_down_message_weather1(void *parent)
{
    gui_magic_img_t *weather_message =  gui_magic_img_create_from_mem(parent, "weather_message1",
                                                                      CARD_WEATHER1_BIN,
                                                                      10, 0, 0, 0);
}
void page_down_message_weather2(void *parent)
{
    gui_magic_img_t *weather_message =  gui_magic_img_create_from_mem(parent, "weather_message2",
                                                                      CARD_WEATHER1_BIN,
                                                                      10, 0, 0, 0);
}
void page_down_message_mult(void *parent)
{
    gui_magic_img_t *muit_message =  gui_magic_img_create_from_mem(parent, "muit_message",
                                                                   CARD_MORE_BIN,
                                                                   10, 0, 0, 0);
    gui_magic_img_t *all_app =  gui_magic_img_create_from_mem(parent, "close",
                                                              DYNAMIC_ISLAND_260_66_BIN,
                                                              30, 156, 0, 0);
    gui_magic_img_t *menu = gui_magic_img_create_from_mem(parent, "menu", CARD_LIST_BIN, 140, 170, 0,
                                                          0);
}
//void page_down_all(void *parent)
//{
//    gui_magic_img_t *all_app =  gui_magic_img_create_from_mem(parent, "close",
//                                                              DYNAMIC_ISLAND_260_66_BIN,
//                                                              30, 306, 0, 0);
//    gui_magic_img_t *menu = gui_magic_img_create_from_mem(parent, "menu", CARD_LIST_BIN, 140, 320, 0,
//                                                          0);
//    //gui_img_set_opacity(close, 50);
//}
void  page_down_message_design(void *parent)
{
    gui_canvas_t *canvas = gui_canvas_create(parent, "canvas", 0, 0, 0, 320, 384);
    gui_canvas_set_canvas_cb(canvas, canvas_cb);
    gui_cardview_t *cd =  gui_cardview_create(parent, "cardview",    0, 0, 0, 0);
    gui_cardview_set_style(cd, STACKING);

    gui_card_t *tb_health   =    gui_card_create(cd, "tb_health",  0, 0, 0, 0, 0, 0);
    gui_card_t *tb_weather1 = gui_card_create(cd, "tb_weather1",   0, 0, 0, 0, 0, 1);
    gui_card_t *tb_music    =  gui_card_create(cd, "tb_music",     0, 0, 0, 0, 0, 2);
    gui_card_t *tb_weather2 =  gui_card_create(cd, "tb_weather2",  0, 0, 0, 0, 0, 3);
    gui_card_t *tb_more     =      gui_card_create(cd, "tb_more",  0, 0, 0, 0, 0, 4);
    gui_card_t *tb_non      =       gui_card_create(cd, "tb_non",  0, 0, 0, 0, 0, 5);
    //gui_cardview_set_style(cd, STACKING);

    page_down_message_health(tb_health);
    page_down_message_weather1(tb_weather1);
    page_down_message_music(tb_music);
    page_down_message_weather2(tb_weather2);
    page_down_message_mult(tb_more);

}