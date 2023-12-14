#include <gui_tabview.h>
#include "gui_card.h"
#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_chargebox/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "app_chargebox.h"
#include "gui_win.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include <stdio.h>

static void app_chargebox_ui_design(gui_app_t *app);


gui_tabview_t *tv;

static gui_app_t app_chargebox =
{
    .screen =
    {
        .name = "app_chargebox",
        .x    = 0,
        .y    = 0,
        .parent = NULL,
    },
    .ui_design = app_chargebox_ui_design,
    .thread_entry = NULL,
    .active_ms = 1000000,
};

void *get_app_chargebox(void)
{
    return &app_chargebox;
}

static void kb_button_cb(void *obj, gui_event_t e)
{
    gui_log("line = %d \n", __LINE__);
}

static void app_chargebox_ui_design(gui_app_t *app)
{
    gui_log("app_chargebox_ui_design\n");

#ifndef _WIN32
#include "mem_config.h"
    static bool flag;
    if (!flag)
    {
        memcpy((void *)SPIC2_ADDR, (void *)0x04400000, 0x100000 * 12);
        flag = !flag;
    }

#endif
    tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_win_t *win = gui_win_create(&(app->screen), "window", 0, 0, 0, 0);
    gui_obj_add_event_cb(win, (gui_event_cb_t)kb_button_cb, GUI_EVENT_KB_UP_PRESSED, NULL);
    gui_tabview_set_style(tv, CLASSIC);

    gui_set_font_mem_resourse(32, GBK_32_32_DOT_BIN, GBK_UNICODE_TABLE_BIN);

    gui_tab_t *tb_conn = gui_tab_create(tv, "tb_conn",     0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_func = gui_tab_create(tv, "tb_func",     0, 0, 0, 0, 1, 0);

    page_tb_conn(tb_conn);
    page_tb_func(tb_func);
}


uint8_t resource_root[1024 * 1024 * 20];
static int app_init(void)
{
#if defined _WIN32
    int fd;
    fd = open("./gui_engine/example/screen_320_385/root_image_chargebox/root(0x4400000).bin", 0);
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
    rtgui_server_init();
    gui_app_startup(get_app_chargebox());
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);

