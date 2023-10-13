#include <gui_tabview.h>
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
#include "gui_img_with_animate.h"
#include "app_hongkong.h"
#include "gui_perspective.h"
static void app_hongkong_ui_design(gui_app_t *app);


gui_tabview_t *tv;

static gui_app_t app_hongkong =
{
    .screen =
    {
        .name = "app_hongkong",
        .x    = 0,
        .y    = 0,
        .parent = NULL,
    },
    .ui_design = app_hongkong_ui_design,
    .thread_entry = NULL,
    .active_ms = 1000000,
};

void *get_app_hongkong(void)
{
    return &app_hongkong;
}

static void app_hongkong_ui_design(gui_app_t *app)
{
    gui_log("app_hongkong_ui_design\n");

    // extern void test_cv(void *p);
    // test_cv(&(app->screen));
    // return;

    tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_tabview_set_style(tv, REDUCTION);

    gui_tab_t *tb_clock = gui_tab_create(tv, "tb_clock",           0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_activity = gui_tab_create(tv, "tb_activity",     0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_heart = gui_tab_create(tv, "tb_heart",           0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_blood = gui_tab_create(tv, "tb_blood",           0, 0, 0, 0, 3, 0);
    gui_tab_t *tb_weather = gui_tab_create(tv, "tb_weather",       0, 0, 0, 0, 4, 0);
    gui_tab_t *tb_music = gui_tab_create(tv, "tb_music",           0, 0, 0, 0, 5, 0);

    page_tb_clock(tb_clock);
    page_tb_activity(tb_activity);
    page_tb_heart(tb_heart);
    page_tb_blood(tb_blood);
    page_tb_weather(tb_weather);
    page_tb_music(tb_music);
    extern void always_on_ui_design(gui_obj_t *parent);
    always_on_ui_design(&(app->screen));
}

