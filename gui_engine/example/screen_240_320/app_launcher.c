#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include <gui_app.h>
#include <math.h>
#include "font_mem.h"
#include "font_stb.h"
#include "gui_components_init.h"
#include "app_launcher.h"
#include "gui_tab.h"
#include "gui_turn_table.h"
#include "gui_cube.h"
#ifndef _WIN32
#include "os_mem.h"
#endif // !_WIN32

static void app_launcher_ui_design(gui_app_t *app);
static gui_app_t app_launcher =
{
    .screen =
    {
        .name = "app_launcher",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_launcher_ui_design,
    .active_ms = 1000 * 60 * 60,
};

gui_app_t *get_launcher_app(void)
{
    return &app_launcher;
}

static void app_launcher_ui_design(gui_app_t *app)
{
    gui_log("app_launcher_ui_design\n");
    gui_img_t *flash_img = gui_img_create_from_mem(&(app->screen), "lake", LAKE_BIN, 0, 0, 0, 0);
    gui_img_set_mode(flash_img, IMG_BYPASS_MODE);
    return;
}

#if defined _WIN32
uint8_t resource_root[1024 * 1024 * 20];
#endif
static int app_init(void)
{
#if defined _WIN32
    int fd;
    fd = open("./gui_engine/example/screen_240_320/root_image/roots(0x858000).bin", 0);
    if (fd > 0)
    {
        printf("open root(0x858000).bin Successful!\n");
        read(fd, resource_root, 1024 * 1024 * 20);
    }
    else
    {
        printf("open root(0x858000).bin Fail!\n");
        printf("open root(0x858000).bin Fail!\n");
        printf("open root(0x858000).bin Fail!\n");
        return 0;
    }
#endif
    gui_server_init();
    gui_app_startup(get_launcher_app());
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
