/**
 * @file auto_test_app.c
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-06-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_img.h"

static void auto_test_app_entry(gui_app_t *app);
static gui_app_t app_auto_test =
{
    .screen =
    {
        .name = "auto_test",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = auto_test_app_entry,
    .active_ms = 1000 * 5,
};

gui_app_t *get_launcher_app(void)
{
    return &app_auto_test;
}

static void auto_test_app_entry(gui_app_t *app)
{
    gui_log("auto_test_app start \n");
}

#if defined _WIN32
uint8_t resource_root[1024 * 1024 * 20];
#endif
static int auto_test_app_init(void)
{
    extern int gui_server_init(void);
    gui_server_init();
    gui_app_startup(get_launcher_app());
    return 0;
}
GUI_INIT_APP_EXPORT(auto_test_app_init);




