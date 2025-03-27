#include <gui_tabview.h>
#include "gui_card.h"
#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_hongkong/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "app_hongkong.h"
#include "gui_win.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include <stdio.h>

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

static void kb_button_cb(void *obj, gui_event_t e)
{
    gui_log("line = %d \n", __LINE__);
}

static void app_hongkong_ui_design(gui_app_t *app)
{
    gui_log("app_hongkong_ui_design\n");
    // gui_img_t *time =  gui_img_create_from_mem(&(app->screen), "time", TIME_BIN, 142, 20, 0, 0);
    // return;

    tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_win_t *win = gui_win_create(&(app->screen), "window", 0, 0, 0, 0);
    gui_obj_add_event_cb(win, (gui_event_cb_t)kb_button_cb, GUI_EVENT_KB_UP_PRESSED, NULL);
    gui_tabview_set_style(tv, TAB_CUBE);
    gui_tabview_enable_pre_load(tv, true);

    gui_tab_t *tb_clock = gui_tab_create(tv, "tb_clock",           0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_activity = gui_tab_create(tv, "tb_activity",     0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_heart = gui_tab_create(tv, "tb_heart",           0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_cube = gui_tab_create(tv, "tb_cube",           0, 0, 0, 0, 3, 0);
    gui_tab_t *tb_weather = gui_tab_create(tv, "tb_weather",       0, 0, 0, 0, 5, 0);
    gui_tab_t *tb_music = gui_tab_create(tv, "tb_music",           0, 0, 0, 0, 4, 0);
    gui_tab_t *tb_ani = gui_tab_create(tv, "tb_ani",          0, 0, 0, 0, 6, 0);
    page_tb_clock(gui_tab_get_rte_obj(tb_clock));
    page_tb_activity(gui_tab_get_rte_obj(tb_activity));
    page_tb_heart(gui_tab_get_rte_obj(tb_heart));
    page_tb_cube(gui_tab_get_rte_obj(tb_cube));
    page_tb_weather(gui_tab_get_rte_obj(tb_weather));
    page_tb_music(gui_tab_get_rte_obj(tb_music));
}


uint8_t resource_root[1024 * 1024 * 20];
static int app_init(void)
{
#if defined _WIN32
    int fd;
    fd = open("./realgui/example/screen_448_368/root_image_hongkong/root(0x4400000).bin", 0);
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
    gui_app_startup(get_app_hongkong());
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);

