#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_tabview.h"
#include "gui_cardview.h"
#include "gui_card.h"
#include "gui_tab.h"
#include "gui_canvas.h"

static void cardlist_clock(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page0", CARD_HEALTH_BIN, 14, 0, 0, 0);
}
static void cardlist_activity(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page1", CARD_MORE_BIN, 14, 0, 0, 0);
}
static void cardlist_heart(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page2", CARD_MUSIC_BIN, 14, 0, 0, 0);
}
static void cardlist_blood(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page3", CARD_MUSIC1_BIN, 14, 0, 0, 0);
}
static void cardlist_weather(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page4", CARD_WEATHER1_BIN, 14, 0, 0, 0);
}
static void cardlist_music(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page5", CARD_MORE_BIN, 14, 0, 0, 0);
}


static void canvas_cb_black(gui_canvas_t *canvas)
{
    nvgRect(canvas->vg, 0, 0, 368, 448);
    nvgFillColor(canvas->vg, nvgRGBA(0, 0, 128, 150));
    nvgFill(canvas->vg);
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

void curtain_down_design(void *parent_widget)
{
    gui_curtain_t *ct_card = parent_widget;
    gui_curtainview_t *curtainview = (gui_curtainview_t *)ct_card->base.parent;
    gui_curtainview_set_done_cb(curtainview, done_cb);

    gui_canvas_t *canvas = gui_canvas_create(parent_widget, "canvas", 0, 0, 0, 368, 448);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_black);

    cv = gui_cardview_create(parent_widget, "cardview", 0, 0, 0, 124);

    gui_cardview_status_cb(cv, cv_status_cb);



    gui_card_t *tb_music = gui_card_create(cv, "tb_music",         0, 0, 0, 124, 0, 5);
    gui_card_t *tb_weather = gui_card_create(cv, "tb_weather",     0, 0, 0, 124, 0, 4);
    gui_card_t *tb_blood = gui_card_create(cv, "tb_tb_bloodcube",  0, 0, 0, 124, 0, 3);
    gui_card_t *tb_heart = gui_card_create(cv, "tb_heart",         0, 0, 0, 124, 0, 2);
    gui_card_t *tb_activity = gui_card_create(cv, "tb_activity",   0, 0, 0, 124, 0, 1);
    gui_card_t *tb_clock = gui_card_create(cv, "tb_clock",         0, 0, 0, 124, 0, 0);

    cardlist_clock(tb_clock);
    cardlist_activity(tb_activity);
    cardlist_heart(tb_heart);
    cardlist_blood(tb_blood);
    cardlist_weather(tb_weather);
    cardlist_music(tb_music);

}

void test_cv(void *p)
{
    cv = gui_cardview_create(p, "cardview", 0, 0, 0, 124);
    cv->mute = false;



    gui_card_t *tb_music = gui_card_create(cv, "tb_music",         0, 0, 0, 124, 0, 5);
    gui_card_t *tb_weather = gui_card_create(cv, "tb_weather",     0, 0, 0, 124, 0, 4);
    gui_card_t *tb_blood = gui_card_create(cv, "tb_tb_bloodcube",  0, 0, 0, 124, 0, 3);
    gui_card_t *tb_heart = gui_card_create(cv, "tb_heart",         0, 0, 0, 124, 0, 2);
    gui_card_t *tb_activity = gui_card_create(cv, "tb_activity",   0, 0, 0, 124, 0, 1);
    gui_card_t *tb_clock = gui_card_create(cv, "tb_clock",         0, 0, 0, 124, 0, 0);

    cardlist_clock(tb_clock);
    cardlist_activity(tb_activity);
    cardlist_heart(tb_heart);
    cardlist_blood(tb_blood);
    cardlist_weather(tb_weather);
    cardlist_music(tb_music);
}
