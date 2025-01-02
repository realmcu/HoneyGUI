#include "guidef.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>
#include "lvgl.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include <sys/types.h>
#include <pthread.h>
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_port_fs.h"

#include <stdio.h>
#include "app_main.h"

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

lv_obj_t *scr_watchface;
lv_obj_t *scr_up_curtain;
lv_obj_t *scr_down_curtain;
lv_obj_t *scr_left_curtain;
lv_obj_t *scr_right_curtain;

uint8_t resource_root[1024 * 1024 * 20];
static void app_dialing_ui_design(gui_app_t *app)
{
#if defined _WIN32
    extern int open(const char *file, int flags, ...);
    extern int read(int fd, void *buf, size_t len);
    extern int close(int fd);
    defaultPath = "realgui\\example\\screen_410_502_lvgl\\root_image_lvgl\\root\\";
    int fd;
    fd = open("./realgui/example/screen_410_502_lvgl/root_image_lvgl/root(0x4400000).bin", 0);
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

    LV_LOG("LVGL start \n");

    //initialize curtains
    lv_watchface_init();
    lv_up_curtain_init();
    lv_down_curtain_init();
    lv_left_curtain_init();
    lv_right_curtain_init();

    lv_disp_load_scr(scr_watchface);
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
    .ui_design = app_dialing_ui_design,
    .thread_entry = lvgl_loop,
    .active_ms = 1000000,
    .lvgl = true,
};

static int app_init(void)
{
    gui_server_init();
    gui_app_startup(&app_lvgl);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);

