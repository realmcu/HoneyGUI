#include <gui_tabview.h>
#include "gui_card.h"
#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "gui_perspective.h"
#include "draw_font.h"
#include <gui_img.h>
#include "gui_canvas.h"
#include "app_gui_main.h"
#include "gui_perspective.h"
#include "gui_cube.h"
#include "gui_win.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include <stdio.h>
#ifndef _WIN32
#include "wdg.h"
#endif

gui_tabview_t *tabview_main = NULL;
gui_tab_t *tab_watchface = NULL;
gui_tab_t *tab_player = NULL;
gui_tab_t *tab_call = NULL;
gui_tab_t *tab_contacts = NULL;
gui_tab_t *tab_record = NULL;
gui_tab_t *tab_menu = NULL;
static void design_app_watch_ui(gui_app_t *app);
extern void gui_task_ext_execution_sethook(void (*hook)(void));

static void gui_task_kick_wdg(void)
{
#ifndef _WIN32
    WDG_Kick();
#endif
}

static gui_app_t app_watch_ui =
{
    .screen =
    {
        .name = "app_watch",
        .x    = 0,
        .y    = 0,
        .parent = NULL,
        .matrix = NULL,
    },
    .ui_design = design_app_watch_ui,
    .thread_entry = NULL,
    .active_ms = 60000,
};

void *get_app_watch_ui(void)
{
    return &app_watch_ui;
}

static void design_app_watch_ui(gui_app_t *app)
{
    gui_log("design_app_watch_ui\n");

    gui_task_ext_execution_sethook(gui_task_kick_wdg);

    gui_font_mem_init(FONT_BIN);

#ifndef _WIN32
#include "mem_config.h"
    //memcpy((void *)SPIC2_ADDR, (void *)0x04400000, 0x100000 * 12);
#endif
    tabview_main = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_tabview_set_style(tabview_main, REDUCTION);

    tab_watchface = gui_tab_create(tabview_main, "tb_watchface",  0, 0, 0, 0, 0, 0);
    tab_player = gui_tab_create(tabview_main, "tab_player",      0, 0, 0, 0, 1, 0);
    tab_call = gui_tab_create(tabview_main, "tab_call",           0, 0, 0, 0, 2, 0);
    tab_contacts = gui_tab_create(tabview_main, "tab_contacts",        0, 0, 0, 0, 3, 0);
    tab_record = gui_tab_create(tabview_main, "tab_record",       0, 0, 0, 0, 4, 0);
    tab_menu = gui_tab_create(tabview_main, "tab_menu",       0, 0, 0, 0, -1, 0);
    design_tab_watchface(tab_watchface);
    design_tab_player(tab_player);
    design_tab_call(tab_call);
    design_tab_contacts(tab_contacts);
    design_tab_record(tab_record);
    design_tab_menu(tab_menu);
}

#if defined _WIN32
uint8_t resource_root[1024 * 1024 * 20];
#endif
static int watch_app_init(void)
{
#if defined _WIN32
    int fd;
    fd = open("./gui_engine/example/screen_454_454/root_image/root(0x4400000).bin", 0);
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
    extern int gui_server_init(void);
    gui_server_init();
    gui_app_startup(get_app_watch_ui());
    return 0;
}

GUI_INIT_APP_EXPORT(watch_app_init);

