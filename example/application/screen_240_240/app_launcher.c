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
static void *addr_list[] =
{
    FIRE_00000_BIN, //nozip
    FIRE_00001_BIN, //zip...
    FIRE_00002_BIN,
    FIRE_00003_BIN,
    FIRE_00004_BIN,
    FIRE_00005_BIN,
    FIRE_00006_BIN,
    FIRE_00007_BIN,
    FIRE_00008_BIN,
    FIRE_00009_BIN,
    FIRE_00010_BIN,
    FIRE_00011_BIN,
    FIRE_00012_BIN,
    FIRE_00013_BIN,
    FIRE_00014_BIN,
    FIRE_00015_BIN,
    FIRE_00016_BIN,
    FIRE_00017_BIN,
    FIRE_00018_BIN,
    FIRE_00019_BIN,
};
uint8_t fire_index;
#define FIRE_S 0
#define FIRE_E 19
void fire_live(void *p, void *obj)
{
    if (fire_index < FIRE_E)
    {
        fire_index ++;
        gui_img_set_attribute((gui_img_t *)obj, "fire", addr_list[fire_index], 0, 0);
    }
    else
    {
        fire_index = FIRE_S;
        gui_img_set_attribute((gui_img_t *)obj, "fire", addr_list[fire_index], 0, 0);
    }
}

static void app_launcher_ui_design(gui_app_t *app)
{
    gui_log("app_launcher_ui_design\n");

    fire_index = FIRE_S;
    gui_img_t *fire = gui_img_create_from_mem(&(app->screen), "lake", addr_list[fire_index], 0, 0, 0,
                                              0);
    gui_img_set_animate(fire, 50, INT32_MAX, fire_live, fire);
    gui_img_set_mode(fire, IMG_BYPASS_MODE);
    return;
}

#if defined _WIN32
uint8_t resource_root[1024 * 1024 * 20];
#endif
static int app_init(void)
{
#if defined _WIN32
    int fd;
    fd = open("./realgui/example/screen_240_240/root_image/roots(0x859000).bin", 0);
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
    gui_app_startup(get_launcher_app());
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
