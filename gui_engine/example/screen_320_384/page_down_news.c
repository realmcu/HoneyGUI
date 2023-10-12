
#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_magic_img.h>
#include <gui_curtain.h>
#include "gui_cardview.h"
#include "gui_card.h"
#include "gui_tab.h"
#include "gui_canvas.h"

void page_down_message_health(void *parent)
{

    gui_magic_img_t *health_message =  gui_magic_img_create_from_mem(parent, "health_message",
                                                                     CARD_HEALTH_BIN,
                                                                     10, 0, 0, 0);
    gui_magic_img_t *health_watch =  gui_magic_img_create_from_mem(parent, "card_time",
                                                                   CARD_TIME_BIN,
                                                                   170, -148, 0, 0);
    gui_img_set_opacity(health_watch, 50);
    gui_magic_img_t *week =  gui_magic_img_create_from_mem(parent, "time", SUN_BIN, 15, -100, 0, 0);
    gui_magic_img_t *data =  gui_magic_img_create_from_mem(parent, "data", DATA_BIN, 25, -47, 0, 0);
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
                                                              30, 256, 0, 0);
    gui_magic_img_t *menu = gui_magic_img_create_from_mem(parent, "menu", CARD_LIST_BIN, 140, 270, 0,
                                                          0);
}
static gui_cardview_t *cv;
static void done_cb(gui_curtainview_t *this)
{
    if (this->cur_curtain == CURTAIN_DOWN)
    {
        gui_log("CURTAIN_DOWN done!!! \n");
        cv->mute = false;
    }
    if (this->cur_curtain == CURTAIN_MIDDLE)
    {
        gui_log("CURTAIN_DOWN Reset!!! \n");
        cv->mute = true;
        cv->release_y = 0;
        cv->remain_y = 0;
    }

}
static void cv_status_cb(gui_cardview_t *this)
{
    //gui_log("cv release value = %d \n", this->release_y);

    gui_curtainview_t *curtainview = (gui_curtainview_t *)this->base.parent->parent;

    if (this->release_y != 0)
    {
        curtainview->mute = true;
    }
    else
    {
        curtainview->mute = false;
    }
}
void  page_down_message_design(void *parent)
{
    gui_magic_img_t *bg = gui_magic_img_create_from_mem(parent, "bg_up", RECT_320_384_BIN, 0, 0, 0, 0);
    gui_img_set_opacity(bg, 128);

    gui_curtain_t *ct_card = parent;
    gui_curtainview_t *curtainview = (gui_curtainview_t *)ct_card->base.parent;
    gui_curtainview_set_done_cb(curtainview, done_cb);

    cv =  gui_cardview_create(parent, "cardview",    0, 0, 0, 0);
    //gui_cardview_set_style(cd, STACKING);
    gui_cardview_status_cb(cv, cv_status_cb);

    gui_card_t *tb_more     =      gui_card_create(cv, "tb_more",  0, 50, 0, 0, 0, 4);
    gui_card_t *tb_weather2 =  gui_card_create(cv, "tb_weather2",  0, 50, 0, 0, 0, 3);
    gui_card_t *tb_music    =  gui_card_create(cv, "tb_music",     0, 50, 0, 0, 0, 2);
    gui_card_t *tb_weather1 = gui_card_create(cv, "tb_weather1",   0, 50, 0, 0, 0, 1);
    gui_card_t *tb_health   =  gui_card_create(cv, "tb_health",  0, 210, 0, 0, 0, 0);

    page_down_message_health(tb_health);
    page_down_message_weather1(tb_weather1);
    page_down_message_music(tb_music);
    page_down_message_weather2(tb_weather2);
    page_down_message_mult(tb_more);

}