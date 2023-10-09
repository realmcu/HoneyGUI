#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>
#include "gui_win.h"
#include "gui_watch_gradient_spot.h"
#include "gui_text.h"
#include "gui_watchface_gradient.h"
#include "gui_cardview.h"
#include "gui_card.h"

gui_win_t *win_watch;
gui_magic_img_t *img;
gui_watch_gradient_spot_t *watch;
gui_text_t *rate;
static gui_watchface_gradient_t *canvas;

// static void canvas_cb(gui_canvas_t *canvas)
// {
//     nvgRect(canvas->vg, 0, 0, 368, 448);
//     nvgFillColor(canvas->vg, nvgRGBA(30, 30, 30, 255));
//     nvgFill(canvas->vg);
// }

// static void callback_tab()
// {
//     int idx = cv->cur_id.y;
//     gui_tree_free(win);
//     app_hongkong_ui_design(get_app_hongkong());

//     gui_tabview_jump_tab(tv, idx, 0);

// }

static void callback_touch_long(void *obj, gui_event_t e)
{
    gui_log("win widget long touch enter cb\n");

    // gui_tree_free(win);
    // win = gui_win_create(get_app_hongkong(), "win", 0, 0, 320, 320);
    // gui_obj_add_event_cb(win, (gui_event_cb_t)callback_tab, GUI_EVENT_TOUCH_CLICKED, NULL);

    // gui_canvas_t *canvas = gui_canvas_create(win, "canvas", 0, 0, 0, 368, 448);
    // gui_canvas_set_canvas_cb(canvas, canvas_cb);
    // cv = gui_cardview_create(win, "cardview", 0, 0, 0, 0);
    // gui_cardview_t *tv = cv;
    // gui_cardview_set_style(tv, STACKING);
    // gui_card_t *tb_clock = gui_card_create(tv, "tb_clock",         0, 0, 0, 0, 0, 0);
    // gui_card_t *tb_activity = gui_card_create(tv, "tb_activity",   0, 0, 0, 0, 0, 1);
    // gui_card_t *tb_heart = gui_card_create(tv, "tb_heart",         0, 0, 0, 0, 0, 2);
    // gui_card_t *tb_blood = gui_card_create(tv, "tb_tb_bloodcube",  0, 0, 0, 0, 0, 3);
    // gui_card_t *tb_weather = gui_card_create(tv, "tb_weather",     0, 0, 0, 0, 0, 4);
    // gui_card_t *tb_music = gui_card_create(tv, "tb_music",         0, 0, 0, 0, 0, 5);

    // gui_cardview_set_style(tv, STACKING);

    // tablist_clock(tb_clock);
    // tablist_activity(tb_activity);
    // tablist_heart(tb_heart);
    // tablist_blood(tb_blood);
    // tablist_weather(tb_weather);
    // tablist_music(tb_music);

}

// static void callback_watchface()
// {
//     static char clock;
//     if (clock == 1)
//     {

//         GET_BASE(img)->not_show = false;
//         GET_BASE(watch)->not_show = true;
//         GET_BASE(canvas)->not_show = true;
//         clock = 2;
//     }
//     else if (clock == 2)
//     {
//         GET_BASE(img)->not_show = true;
//         GET_BASE(watch)->not_show = false;
//         GET_BASE(canvas)->not_show = true;
//         clock = 0;
//     }
//     else if (clock == 0)
//     {
//         GET_BASE(img)->not_show = true;
//         GET_BASE(watch)->not_show = true;
//         GET_BASE(canvas)->not_show = false;
//         clock = 1;
//     }

// }



void page_ct_clock(void *parent)
{
    win_watch = gui_win_create(parent, "win", 0, 0, 368, 448);

    gui_obj_add_event_cb(win_watch, (gui_event_cb_t)callback_touch_long, GUI_EVENT_TOUCH_LONG, NULL);

    img = gui_magic_img_create_from_mem(parent, "page0", BACKGROUND_BIN, 0, 0, 0, 0);
    watch = gui_watch_gradient_spot_create(win_watch, "watchface", 0, 0, 0, 0);
    gui_watch_gradient_spot_set_center(watch, 368 / 2, 448 / 2);
    canvas = gui_watchface_gradient_create(parent, "watchface_gradient", (368 - 368) / 2,
                                           (448 - 448) / 2, 368, 448);
    GET_BASE(watch)->not_show = true;
    GET_BASE(canvas)->not_show = true;

}

