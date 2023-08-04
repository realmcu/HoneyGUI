#include <gui_tabview.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "rtk_gui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "draw_font.h"
#include <gui_magic_img.h>


static void app_launcher_ui_design(gui_app_t *app);
static gui_tabview_t *tv;


static gui_app_t app_launcher =
{
    .screen =
    {
        .name = "app_launcher",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_launcher_ui_design,
    .thread_entry = NULL,
    .active_ms = 1000000,
};

void *get_app_launcher(void)
{
    return &app_launcher;
}


static void app_launcher_ui_design(gui_app_t *app)
{
    gui_log("app_launcher_ui_design\n");
    gui_font_stb_init(TANGYUANTI_TTF);

    gui_win_t *win = gui_win_create(&(app->screen), "win", 0, 0, 320, 320);

    // gui_magic_img_t *img = gui_magic_img_create_from_mem(win, "test", C1_BIN, 0, 0, 454, 454);
    // gui_img_translate(img, (454 - 256) / 2, (454 - 256) / 2);
    // gui_img_rotation(img, 45, 128 + (454 - 256) / 2, 128 + (454 - 256) / 2);


    tv = gui_tabview_create(win, "tabview", 0, 0, 0, 0);
    gui_tabview_set_style(tv, SLIDE_SCALE_FADE);

    gui_tab_t *tb_watch = gui_tab_create(tv, "tb_watch",             0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_sport = gui_tab_create(tv, "tb_sport",             0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_colorwheel = gui_tab_create(tv, "tb_colorwheel",   0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_cube = gui_tab_create(tv, "tb_cube",               0, 0, 0, 0, 3, 0);
    gui_tab_t *tb_svg = gui_tab_create(tv, "tb_svg",                 0, 0, 0, 0, 4, 0);
    gui_tab_t *tb_wave = gui_tab_create(tv, "tb_wave",               0, 0, 0, 0, 5, 0);
    gui_tab_t *tb_60 = gui_tab_create(tv, "tb_",                     0, 0, 0, 0, 6, 0);
    gui_tab_t *tb_70 = gui_tab_create(tv, "tb_",                     0, 0, 0, 0, 7, 0);

    extern void page_tb_watch(void *parent);
    extern void page_tb_sport(void *parent);
    extern void page_tb_colorwheel(void *parent);
    extern void page_tb_cube(void *parent);
    extern void page_tb_svg(void *parent);
    extern void page_tb_wave(void *parent);
    page_tb_watch(tb_watch);
    page_tb_sport(tb_sport);
    page_tb_cube(tb_cube);
    page_tb_colorwheel(tb_colorwheel);
    page_tb_svg(tb_svg);
    page_tb_wave(tb_wave);

}

