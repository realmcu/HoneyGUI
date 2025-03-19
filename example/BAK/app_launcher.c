#include <gui_tabview.h>
#include "gui_cardview.h"
#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "gui_card.h"
#include "gui_perspective.h"
#include "draw_font.h"
#include "gui_components_init.h"

static void app_launcher_ui_design(gui_app_t *app);
static gui_tabview_t *tv_main;
static gui_win_t *win_main;


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

static void callback(void *obj, gui_event_t e)
{
    gui_log("win widget long touch enter cb\n");

    gui_obj_tree_free(tv_main);


    gui_cardview_t *tv = gui_cardview_create(win_main, "cardview", 0, 0, 0, 0);
    gui_cardview_set_style(tv, STACKING);
    gui_card_t *tb_watch = gui_card_create(tv, "tb_watch",             0, 0, 0, 0, 0, 0);
    gui_card_t *tb_sport = gui_card_create(tv, "tb_sport",             0, 0, 0, 0, 0, 1);
    gui_card_t *tb_colorwheel = gui_card_create(tv, "tb_colorwheel",   0, 0, 0, 0, 0, 2);
    gui_card_t *tb_cube = gui_card_create(tv, "tb_cube",               0, 0, 0, 0, 0, 3);
    gui_card_t *tb_svg = gui_card_create(tv, "tb_svg",                 0, 0, 0, 0, 0, 4);
    gui_card_t *tb_wave = gui_card_create(tv, "tb_wave",               0, 0, 0, 0, 0, 5);

    extern void page_tb_watch(void *parent);
    extern void page_tb_sport(void *parent);
    extern void page_tb_colorwheel(void *parent);
    extern void page_tb_cube(void *parent);
    extern void page_tb_svg(void *parent);
    extern void page_tb_wave(void *parent);
    page_tb_watch(tb_watch);
    page_tb_sport(tb_sport);
    page_tb_colorwheel(tb_colorwheel);
    page_tb_cube(tb_cube);
    page_tb_svg(tb_svg);
    page_tb_wave(tb_wave);

}

static void app_launcher_ui_design(gui_app_t *app)
{
    gui_log("app_launcher_ui_design\n");

    win_main = gui_win_create(&(app->screen), "win", 0, 0, 320, 320);
    gui_obj_add_event_cb(win_main, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_LONG, NULL);


    tv_main = gui_tabview_create(win_main, "tabview", 0, 0, 0, 0);
    gui_tabview_set_style(tv_main, REDUCTION);
    gui_tab_t *tb_watch = gui_tab_create(tv_main, "tb_watch",             0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_sport = gui_tab_create(tv_main, "tb_sport",             0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_colorwheel = gui_tab_create(tv_main, "tb_colorwheel",   0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_cube = gui_tab_create(tv_main, "tb_cube",               0, 0, 0, 0, 3, 0);
    gui_tab_t *tb_svg = gui_tab_create(tv_main, "tb_svg",                 0, 0, 0, 0, 4, 0);
    gui_tab_t *tb_wave = gui_tab_create(tv_main, "tb_wave",               0, 0, 0, 0, 5, 0);
    gui_tab_t *tb_3d = gui_tab_create(tv_main, "tb_3d",                   0, 0, 0, 0, 6, 0);


    extern void page_tb_watch(void *parent);
    extern void page_tb_sport(void *parent);
    extern void page_tb_colorwheel(void *parent);
    extern void page_tb_cube(void *parent);
    extern void page_tb_svg(void *parent);
    extern void page_tb_wave(void *parent);
    page_tb_watch(tb_watch);
    page_tb_sport(tb_sport);
    page_tb_colorwheel(tb_colorwheel);
    page_tb_cube(tb_cube);
    page_tb_svg(tb_svg);
    page_tb_wave(tb_wave);
}

uint8_t resource_root[1024 * 1024 * 20];
static int app_init(void)
{
#if defined _WIN32
    int fd;
    fd = open("./realgui/example/screen_454_454/root_image/root(0x4400000).bin", 0);
    if (fd > 0)
    {
        printf("open root(0x4400000).bin Successful!\n");
        read(fd, resource_root, 1024 * 1024 * 20);
    }
    else
    {
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        return 0;
    }
#endif
    gui_server_init();
    gui_app_startup(get_app_launcher());
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
