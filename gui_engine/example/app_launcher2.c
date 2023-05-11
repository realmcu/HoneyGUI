#include <gui_tabview.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "rtk_gui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "draw_font.h"


static void app_launcher2_ui_design(gui_app_t *app);
static gui_tabview_t *tv;


static gui_app_t app_launcher2 =
{
    .screen =
    {
        .name = "app_launcher2",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_launcher2_ui_design,
};

void *get_app_launcher2(void)
{
    return &app_launcher2;
}


static void app_launcher2_ui_design(gui_app_t *app)
{
    gui_log("app_launcher2_ui_design\n");
    gui_font_stb_init(TANGYUANTI_TTF);

    gui_win_t *win = gui_win_create(&(app->screen), "win", 0, 0, 320, 320);
    tv = gui_tabview_create(win, "tabview", 0, 0, 0, 0);
    gui_tabview_set_style(tv, SLIDE_SCALE_FADE);

    gui_tab_t *tb0 = gui_tab_create(tv, "tb0", 0, 0, 0, 0, 0, 0);
    gui_tab_t *tb1 = gui_tab_create(tv, "tb1", 0, 0, 0, 0, 1, 0);
    gui_tab_t *tb2 = gui_tab_create(tv, "tb1", 0, 0, 0, 0, 0, 1);
    gui_tab_t *tb3 = gui_tab_create(tv, "tb0", 0, 0, 0, 0, 2, 0);
    gui_tab_t *tb4 = gui_tab_create(tv, "tb1", 0, 0, 0, 0, 3, 0);
    gui_tab_t *tb5 = gui_tab_create(tv, "tb1", 0, 0, 0, 0, 4, 0);
    gui_tab_t *tb6 = gui_tab_create(tv, "tb1", 0, 0, 0, 0, 5, 0);
    gui_tab_t *tb7 = gui_tab_create(tv, "tb1", 0, 0, 0, 0, 6, 0);

    gui_curtainview_t *curtainview = gui_curtainview_create(tb0, "curtainview", 0, 0, 0, 0);
    gui_curtain_t *c1 = gui_curtain_create(curtainview, "c1", 0, 0, 0, 0, CURTAIN_MIDDLE, 1);
    gui_curtain_t *c2 = gui_curtain_create(curtainview, "c2", 0, 0, 0, 0, CURTAIN_UP, 1);
    gui_curtain_t *c3 = gui_curtain_create(curtainview, "c3", 0, 0, 0, 0, CURTAIN_LEFT, 0.5f);

    extern void watchface(void *parent);
    watchface(c1);

    extern void curtain_up(void *curtian);
    curtain_up(c2);

    extern void curtain_left(void *curtian);
    curtain_left(c3);

    extern void sport_chart(gui_obj_t *parent);
    sport_chart((void *)tb1);

    extern void grid_icon(void *parent);
    grid_icon(tb2);

    extern void palette_wheel(gui_obj_t *screen);
    palette_wheel((void *)tb3);

    extern void cube_page(gui_obj_t *parent);
    cube_page((void *)tb4);

    extern void music_play(gui_obj_t *screen);
    music_play((void *)tb5);

    extern void create_selector(gui_obj_t *screen);
    create_selector((void *)tb6);

    extern void wave_page(void *parent);
    wave_page(tb7);

}

