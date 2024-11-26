#include <gui_tabview.h>
#include "gui_card.h"
#include <gui_obj.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "font_mem.h"
#include <gui_img.h>
#include "app_gui_main.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_common.h"
#include "gui_fps.h"
#include <stdio.h>
#ifdef _ENABLE_RTK_SOC_WATCH_
#include "wdg.h"
#endif

gui_tabview_t *tabview_main = NULL;
gui_tab_t *tab_watchface = NULL;
gui_tab_t *tab_player = NULL;
gui_tab_t *tab_call = NULL;
gui_tab_t *tab_contacts = NULL;
gui_tab_t *tab_record = NULL;
gui_tab_t *tab_benchmark = NULL;
gui_tab_t *tab_menu = NULL;
gui_tab_t *tab_gfx_demo = NULL;
static void design_app_watch_ui(gui_app_t *app);
extern void gui_task_ext_execution_sethook(void (*hook)(void));

static void gui_task_kick_wdg(void)
{
#ifdef _ENABLE_RTK_SOC_WATCH_
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
        .type = SCREEN,
    },
    .ui_design = design_app_watch_ui,
    .thread_entry = NULL,
    .active_ms = 10000,
};

gui_app_t *get_app_watch_ui(void)
{
    return &app_watch_ui;
}
static void dtor()
{
    app_gui_main_free();
}
static void design_app_watch_ui(gui_app_t *app)
{
    gui_log("design_app_watch_ui\n");
    app->dtor = dtor;
    gui_task_ext_execution_sethook(gui_task_kick_wdg);

    gui_font_mem_init(FONT_BIN);


#ifdef _ENABLE_RTK_SOC_WATCH_
#include "mem_config.h"
    //memcpy((void *)SPIC2_ADDR, (void *)0x04400000, 0x100000 * 12);
#endif
    tabview_main = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    gui_tabview_set_style(tabview_main, TAB_CUBE);
    gui_tabview_enable_pre_load(tabview_main, true);

    gui_tab_t *tab_menu_loop = gui_tab_create(tabview_main, "tab_menu", 0, 0, 0, 0, -2, 0);
    tab_menu = gui_tab_create(tabview_main, "tab_menu",       0, 0, 0, 0, -1, 0);
    tab_watchface = gui_tab_create(tabview_main, "tb_watchface",  0, 0, 0, 0, 0, 0);
    tab_player = gui_tab_create(tabview_main, "tab_player",      0, 0, 0, 0, 1, 0);
    tab_call = gui_tab_create(tabview_main, "tab_call",           0, 0, 0, 0, 2, 0);
    tab_contacts = gui_tab_create(tabview_main, "tab_contacts",        0, 0, 0, 0, 3, 0);
    tab_record = gui_tab_create(tabview_main, "tab_record",       0, 0, 0, 0, 4, 0);
    tab_benchmark = gui_tab_create(tabview_main, "tab_benchmark",       0, 0, 0, 0, 5, 0);

    design_menu_loop_ui(GUI_BASE(tab_menu_loop));
    design_tab_menu(gui_tab_get_rte_obj(tab_menu));
    design_watchface_mgr(gui_tab_get_rte_obj(tab_watchface));
    design_player_mgr(gui_tab_get_rte_obj(tab_player));
    design_call_mgr(gui_tab_get_rte_obj(tab_call));
    design_contacts_mgr(gui_tab_get_rte_obj(tab_contacts));
    design_record_mgr(gui_tab_get_rte_obj(tab_record));
    design_benchmark_mgr(gui_tab_get_rte_obj(tab_benchmark));
#ifdef ENABLE_LITE_GFX
    extern void design_tab_watchface_butterfly(void *parent);
    tab_gfx_demo = gui_tab_create(tabview_main, "tb_watchface",  0, 0, 0, 0, 6, 0);
    // design_tab_watchface_butterfly(gui_tab_get_rte_obj(tab_gfx_demo));
    design_tab_watchface_butterfly(gui_tab_get_rte_obj(tab_gfx_demo));
    // design_tab_watchface_flowerfall(gui_tab_get_rte_obj(tab_gfx_demo));
#else
    gui_tab_t *tab_alipay = gui_tab_create(tabview_main, "alipay",       0, 0, 0, 0, 6, 0);
    extern void alipay_menu_ui(gui_obj_t *parent);
    alipay_menu_ui(GUI_BASE(tab_alipay));
#endif
    gui_fps_create(GUI_APP_ROOT_SCREEN);
    extern void apps_append_name();
    gui_app_append(app);
    apps_append_name();
}

#if defined _WIN32
uint8_t resource_root[1024 * 1024 * 20];
#endif
static int watch_app_init(void)
{
#if defined _WIN32
#ifndef ENABLE_MONKEY_TEST
    extern int read(int fd, void *buf, size_t len);
#endif
    int fd;
    extern int open(const char *file, int flags, ...);
    extern int close(int fd);
    defaultPath = "realgui\\example\\screen_454_454\\root_image\\root\\";
    fd = open("./realgui/example/screen_454_454/root_image/root(0x4400000).bin", 0);
    if (fd > 0)
    {
        if (read(fd, resource_root, 1024 * 1024 * 20) == -1)
        {
            printf("open root(0x4400000).bin read fail!");
            close(fd);
            return 0;
        }
        printf("open root(0x4400000).bin Successful!\n");
    }
    else
    {
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        return 0;
    }
    close(fd);
#endif
    extern int gui_server_init(void);
    gui_server_init();
    gui_app_startup(get_app_watch_ui());
    return 0;
}

GUI_INIT_APP_EXPORT(watch_app_init);

#if defined _WIN32
#ifdef ENABLE_MONKEY_TEST
#include "test_cmd.h"
void return_home(void *msg)
{
    gui_app_t *app = gui_current_app();
    if (app == get_app_watch_ui())
    {
        tabview_main->cur_id.x = 0;
        tabview_main->cur_id.y = 0;
    }
    else
    {
        tabview_main->cur_id.x = 0;
        tabview_main->cur_id.y = 0;
        gui_switch_app(app, get_app_watch_ui());
    }
    gui_log("return_home done \n");
}
void cmd_return_home(void)
{
    gui_server_exec_cb(return_home);
}
GUI_EXPORT_CMD(return_home, cmd_return_home);
#endif
#endif

