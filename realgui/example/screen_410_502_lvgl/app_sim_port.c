#include "guidef.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "lvgl.h"
#include <sys/types.h>
#include <pthread.h>
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_port_fs.h"

#include <stdio.h>
#include <time.h>
#include "app_main.h"
#include "lv_custom_tile_slide.h"
#include "lv_custom_tile_snapshot.h"

#include "kb_algo.h"
#ifdef __WIN32
#include <gui_app.h>
#include "gui_server.h"
#include "gui_components_init.h"
#endif

static void lvgl_loop(void *tihs)
{
    while ((1))
    {
        pthread_testcancel();
        lv_timer_handler();                 //! run lv task at the max speed
    }
}

static void port_log(lv_log_level_t level, const char *buf)
{
    gui_log("[LVGL]%s", buf);
}

static void lv_tick(void)
{
    lv_tick_inc(10);
}

#ifdef __WIN32
uint8_t resource_root[1024 * 1024 * 20];
#endif
static void load_ui_source(void)
{
#if defined _WIN32
    extern int open(const char *file, int flags, ...);
    extern int read(int fd, void *buf, size_t len);
    extern int close(int fd);
    defaultPath = "realgui\\example\\screen_410_502_lvgl\\root_image_lvgl\\root\\";
    int fd;
    fd = open("./realgui/example/screen_410_502_lvgl/root_image_lvgl/root(0x253E400).bin", 0);
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
    }
#endif
}

static void lvgl_demo_run(gui_app_t *app)
{
    if (lv_is_initialized() == true)
    {
        return;
    }
    gui_set_tick_hook(lv_tick);
    lv_init();
    lv_log_register_print_cb((lv_log_print_g_cb_t)port_log);
    lv_port_disp_init();
    lv_port_indev_init();
    // lv_port_fs_init();

    watch_demo_init();
    while (1)
    {
        lv_task_handler();
    }
}

static gui_app_t app_lvgl =
{
    .screen =
    {
        .name = "app_ui_lvgl",
        .x    = 0,
        .y    = 0,
        .parent = NULL,
    },
    .ui_design = lvgl_demo_run,
    .thread_entry = lvgl_loop,
    .active_ms = 1000000,
    .lvgl = true,
};

static int app_init(void)
{
    gui_server_init();
    load_ui_source();
    gui_app_startup(&app_lvgl);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);

