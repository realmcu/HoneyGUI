#include <gui_tabview.h>
#include "gui_cardview.h"
#include "gui_card.h"
#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_hongkong/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "gui_perspective.h"
#include "draw_font.h"
#include <gui_magic_img.h>
#include "gui_switch.h"
#include "gui_canvas.h"

static void app_hongkong_ui_design(gui_app_t *app);
static gui_tabview_t *tv;
static gui_tabview_t *tv_up;
static gui_win_t *win;
gui_cardview_t *cv;
extern void page_tb_clock(void *parent);
extern void page_tb_activity(void *parent);
extern void page_tb_heart(void *parent);
extern void page_tb_blood(void *parent);
extern void page_tb_stress(void *parent);
extern void page_tb_sleep(void *parent);
extern void page_tb_weather(void *parent);
extern void page_tb_music(void *parent);

static void callback_tab();
static gui_app_t app_hongkong =
{
    .screen =
    {
        .name = "app_hongkong",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_hongkong_ui_design,
    .thread_entry = NULL,
    .active_ms = 1000000,
};

void *get_app_hongkong(void)
{
    return &app_hongkong;
}
static void canvas_cb(gui_canvas_t *canvas)
{
    nvgRect(canvas->vg, 0, 0, 368, 448);
    nvgFillColor(canvas->vg, nvgRGBA(255, 255, 255, 30));
    nvgFill(canvas->vg);
}
static void canvas_cb_balck(gui_canvas_t *canvas)
{
    nvgRect(canvas->vg, 0, 0, 368, 448);
    nvgFillColor(canvas->vg, nvgRGBA(0, 0, 0, 255));
    nvgFill(canvas->vg);
}
static void callback(void *obj, gui_event_t e)
{
    gui_log("win widget long touch enter cb\n");

    gui_tree_free(win);
    win = gui_win_create(get_app_hongkong(), "win", 0, 0, 320, 320);
    gui_obj_add_event_cb(win, (gui_event_cb_t)callback_tab, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_canvas_t *canvas = gui_canvas_create(win, "canvas", 0, 0, 0, 368, 448);
    gui_canvas_set_canvas_cb(canvas, canvas_cb);
    cv = gui_cardview_create(win, "cardview", 0, 0, 0, 0);
    gui_cardview_t *tv = cv;
    gui_cardview_set_style(tv, STACKING);
    gui_card_t *tb_clock = gui_card_create(tv, "tb_clock",         0, 0, 0, 0, 0, 0);
    gui_card_t *tb_activity = gui_card_create(tv, "tb_activity",   0, 0, 0, 0, 0, 1);
    gui_card_t *tb_heart = gui_card_create(tv, "tb_heart",         0, 0, 0, 0, 0, 2);
    gui_card_t *tb_blood = gui_card_create(tv, "tb_tb_bloodcube",  0, 0, 0, 0, 0, 3);
    gui_card_t *tb_stress = gui_card_create(tv, "tb_stress",       0, 0, 0, 0, 0, 4);
    gui_card_t *tb_sleep = gui_card_create(tv, "tb_sleep",         0, 0, 0, 0, 0, 5);
    gui_card_t *tb_weather = gui_card_create(tv, "tb_weather",     0, 0, 0, 0, 0, 6);
    gui_card_t *tb_music = gui_card_create(tv, "tb_music",         0, 0, 0, 0, 0, 7);

    gui_cardview_set_style(tv, STACKING);

    page_tb_clock(tb_clock);
    page_tb_activity(tb_activity);
    page_tb_heart(tb_heart);
    page_tb_blood(tb_blood);
    page_tb_stress(tb_stress);
    page_tb_sleep(tb_sleep);
    page_tb_weather(tb_weather);
    page_tb_music(tb_music);
}

static void app_hongkong_ui_design(gui_app_t *app)
{
    gui_log("app_hongkong_ui_design\n");

    win = gui_win_create(&(app->screen), "win", 0, 0, 320, 320);

    // extern void page_array_flash(void *parent);
    // page_array_flash(win);
    // return;


    tv = gui_tabview_create(win, "tabview", 0, 0, 0, 0);
    gui_tabview_set_style(tv, REDUCTION);
    gui_tab_t *tb_clock = gui_tab_create(tv, "tb_clock",           0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_activity = gui_tab_create(tv, "tb_activity",     0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_heart = gui_tab_create(tv, "tb_heart",           0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_blood = gui_tab_create(tv, "tb_blood",           0, 0, 0, 0, 3, 0);
    gui_tab_t *tb_stress = gui_tab_create(tv, "tb_stress",         0, 0, 0, 0, 4, 0);
    gui_tab_t *tb_sleep = gui_tab_create(tv, "tb_sleep",           0, 0, 0, 0, 5, 0);
    gui_tab_t *tb_weather = gui_tab_create(tv, "tb_weather",       0, 0, 0, 0, 6, 0);
    gui_tab_t *tb_music = gui_tab_create(tv, "tb_music",           0, 0, 0, 0, 7, 0);
    // gui_tab_t *tb_3d = gui_tab_create(tv, "tb_3d",              0, 0, 0, 0, -2, 0);
    // uint8_t *array_flash[] = {ACTIVITY_BIN, BLOODOXYGEN_BIN, CLOCKN_BIN, STRESS_BIN, MUSIC_BIN, QUICKCARD_BIN};
    // gui_perspective_t *img_test = gui_perspective_create(tb_3d, "test", array_flash, 0, 0, 454, 454);
    // page_tb_clock(tb_clock);
    page_tb_activity(tb_activity);
    page_tb_heart(tb_heart);
    page_tb_blood(tb_blood);
    page_tb_stress(tb_stress);
    page_tb_sleep(tb_sleep);
    page_tb_weather(tb_weather);
    page_tb_music(tb_music);

    gui_obj_add_event_cb(win, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_LONG, NULL);

    gui_curtainview_t *ct = gui_curtainview_create(tb_clock, "ct", 0, 0, 368, 448);
    GET_BASE(ct)->cover = true;
    gui_curtain_t *ct_clock = gui_curtain_create(ct, "1", 0, 0, 368, 448, CURTAIN_MIDDLE, 1);
    gui_curtain_t *ct_control0 = gui_curtain_create(ct, "2", 0, 0, 368, 448, CURTAIN_UP, 1);
    gui_curtain_t *ct_message = gui_curtain_create(ct, "3", 0, 0, 368, 448, CURTAIN_DOWN, 1);
    gui_curtain_t *ct_left = gui_curtain_create(ct, "3", 0, 0, 368, 448, CURTAIN_LEFT, 0.6f);
    extern void page_ct_clock(void *parent);
    extern void page_ct_message(void *parent);
    extern void page_ct_sidebar(void *parent);
    page_ct_clock(ct_clock);
    page_ct_message(ct_message);
    page_ct_sidebar(ct_left);
    gui_canvas_t *canvas = gui_canvas_create(ct_control0, "canvas", 0, 0, 0, 368, 448);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_balck);
    tv_up = gui_tabview_create(ct_control0, "tabview_up", 0, 0, 0, 0);
    gui_tabview_set_style(tv_up, CLASSIC);
    gui_tab_t *tb_control0 = gui_tab_create(tv_up, "tb_control0",    0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_control1 = gui_tab_create(tv_up, "tb_control1",    0, 0, 0, 0, 1, 0);
    extern void page_tb_control0(void *parent);
    extern void page_tb_control1(void *parent);
    page_tb_control0(tb_control0);
    page_tb_control1(tb_control1);
}
static void callback_tab()
{
    int idx = cv->cur_id.y;
    gui_tree_free(win);
    app_hongkong_ui_design(get_app_hongkong());

    gui_tabview_jump_tab(tv, idx, 0);

}
