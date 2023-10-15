#include <gui_tabview.h>
#include "gui_cardview.h"
#include "gui_card.h"
#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "gui_perspective.h"
#include "draw_font.h"
#include <gui_magic_img.h>
#include "gui_switch.h"
#include "gui_canvas.h"
#include "gui_img.h"

static void app_tablist_ui_design(gui_app_t *app);
gui_tabview_t *tablist_tab;


extern void design_tablist_sport(void *parent);
extern void design_tablist_heart(void *parent);
extern void design_tablist_blood(void *parent);
extern void design_tablist_call(void *parent);
extern void design_tablist_music(void *parent);
extern void design_tablist_note(void *parent);
extern void design_tablist_weather(void *parent);
extern void design_tablist_breath(void *parent);
extern void design_tablist_sleep(void *parent);

static void callback_tab();
static gui_app_t app_tablist =
{
    .screen =
    {
        .name = "app_tablist",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_tablist_ui_design,
    .thread_entry = NULL,
    .active_ms = 1000000,
};

void *get_app_tablist(void)
{
    return &app_tablist;
}

static void app_tablist_ui_design(gui_app_t *app)
{
    gui_log("app_hk_bee4_ui_design\n");

    tablist_tab = gui_tabview_create(&(app->screen), "tabview", 55, 49, 210, 240);
    gui_tabview_set_style(tablist_tab, CLASSIC);

    gui_img_t *tablist_add = gui_img_create_from_mem(&(app->screen), "add", TABLIST_ADD_BIN, 44, 324, 0,
                                                     0);
    gui_img_t *tablist_delete = gui_img_create_from_mem(&(app->screen), "add", TABLIST_DELETE_BIN, 194,
                                                        324, 0, 0);
    gui_tab_t *tab_sport = gui_tab_create(tablist_tab, "tab_sport",          0, 0, 210, 0, 0, 0);
    gui_tab_t *tab_heart = gui_tab_create(tablist_tab, "tb_heart",           0, 0, 210, 0, 1, 0);
    gui_tab_t *tab_blood = gui_tab_create(tablist_tab, "tb_blood",           0, 0, 210, 0, 2, 0);
    gui_tab_t *tab_call = gui_tab_create(tablist_tab, "tb_call",             0, 0, 210, 0, 3, 0);
    gui_tab_t *tab_music = gui_tab_create(tablist_tab, "tb_music",           0, 0, 210, 0, 4, 0);
    gui_tab_t *tab_weather = gui_tab_create(tablist_tab, "tb_weather",       0, 0, 210, 0, 5, 0);
    gui_tab_t *tab_note = gui_tab_create(tablist_tab, "tb_note",             0, 0, 210, 0, 6, 0);
    gui_tab_t *tab_breath = gui_tab_create(tablist_tab, "tb_breath",         0, 0, 210, 0, 7, 0);
    gui_tab_t *tab_sleep = gui_tab_create(tablist_tab, "tb_note",            0, 0, 210, 0, 8, 0);


    design_tablist_sport(tab_sport);
    design_tablist_heart(tab_heart);
    design_tablist_blood(tab_blood);
    design_tablist_call(tab_call);
    design_tablist_music(tab_music);
    design_tablist_weather(tab_weather);
    design_tablist_note(tab_note);
    design_tablist_breath(tab_breath);
    design_tablist_sleep(tab_sleep);
}
