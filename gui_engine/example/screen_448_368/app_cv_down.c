#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_tabview.h"
#include "gui_cardview.h"
#include "gui_card.h"
#include "gui_canvas.h"

static void cardlist_clock(void *parent)
{
    gui_img_create_from_mem(parent, "page0", CARD_HEALTH_BIN, 14, 0, 0, 0);
}
static void cardlist_activity(void *parent)
{
    gui_img_create_from_mem(parent, "page1", CARD_MORE_BIN, 14, 0, 0, 0);
}
static void cardlist_heart(void *parent)
{
    gui_img_create_from_mem(parent, "page2", CARD_MUSIC_BIN, 14, 0, 0, 0);
}
static void cardlist_blood(void *parent)
{
    gui_img_create_from_mem(parent, "page3", CARD_MUSIC1_BIN, 14, 0, 0, 0);
}
static void cardlist_weather(void *parent)
{
    gui_img_create_from_mem(parent, "page4", CARD_WEATHER1_BIN, 14, 0, 0, 0);
}
static void cardlist_music(void *parent)
{
    gui_img_create_from_mem(parent, "page5", CARD_MORE_BIN, 14, 0, 0, 0);
}


static void canvas_cb_black(gui_canvas_t *canvas)
{
    nvgRect(canvas->vg, 0, 0, 368, 224);
    nvgFillColor(canvas->vg, nvgRGBA(0, 0, 128, 255));
    nvgFill(canvas->vg);
}

static gui_cardview_t *cv;

static void cv_status_cb(gui_cardview_t *this)
{
    //gui_log("cv release value = %d \n", this->release_y);
}

void curtain_down_design(void *parent_widget)
{
    gui_curtain_t *ct_card = parent_widget;
    gui_curtainview_t *curtainview = (gui_curtainview_t *)ct_card->base.parent;

    gui_canvas_t *canvas = gui_canvas_create(parent_widget, "canvas", 0, 0, 0, 368, 448);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_black);

    cv = gui_cardview_create(parent_widget, "cardview", 0, 0, 0, 0);

    gui_cardview_status_cb(cv, cv_status_cb);



    gui_card_t *tb_music = gui_card_create(cv, "tb_music",         0, 0, 0, 124);
    gui_card_t *tb_weather = gui_card_create(cv, "tb_weather",     0, 0, 0, 124);
    gui_card_t *tb_blood = gui_card_create(cv, "tb_tb_bloodcube",  0, 0, 0, 124);
    gui_card_t *tb_heart = gui_card_create(cv, "tb_heart",         0, 0, 0, 124);
    gui_card_t *tb_activity = gui_card_create(cv, "tb_activity",   0, 0, 0, 124);
    gui_card_t *tb_clock = gui_card_create(cv, "tb_clock",         0, 0, 0, 124);

    cardlist_clock(tb_clock);
    cardlist_activity(tb_activity);
    cardlist_heart(tb_heart);
    cardlist_blood(tb_blood);
    cardlist_weather(tb_weather);
    cardlist_music(tb_music);

    gui_card_t *tb_music_test = gui_card_create(cv, "tb_music",         0, 0, 0, 124);
    gui_card_t *tb_weather_test = gui_card_create(cv, "tb_weather",     0, 0, 0, 124);
    gui_card_t *tb_blood_test = gui_card_create(cv, "tb_tb_bloodcube",  0, 0, 0, 124);
    gui_card_t *tb_heart_test = gui_card_create(cv, "tb_heart",         0, 0, 0, 124);
    gui_card_t *tb_activity_test = gui_card_create(cv, "tb_activity",   0, 0, 0, 124);
    gui_card_t *tb_clock_test = gui_card_create(cv, "tb_clock",         0, 0, 0, 124);

    cardlist_clock(tb_clock_test);
    cardlist_activity(tb_activity_test);
    cardlist_heart(tb_heart_test);
    cardlist_blood(tb_blood_test);
    cardlist_weather(tb_weather_test);
    cardlist_music(tb_music_test);
}
