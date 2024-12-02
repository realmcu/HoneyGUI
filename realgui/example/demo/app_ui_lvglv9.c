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
#include "lv_demos.h"
#include "lv_examples.h"

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

static void on_benchmark_finished(void)
{
    disp_enable_update();
}

static void app_dialing_ui_design(gui_app_t *app)
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

    LV_LOG("LVGL start \n");

    /*Get Started Example*/
    /*https://lvgl.100ask.net/9.1/examples.html#examples*/
    // lv_example_get_started_1();
    // lv_example_get_started_2();
    // lv_example_get_started_3();
    // lv_example_get_started_4();

    /*https://lvgl.io/demos*/
    /*Widgets Demo*/
    lv_demo_widgets();
    /*Music Player App*/
    // lv_demo_music();

    /*Benchmark*/
    // lv_demo_benchmark();
    /*Stress*/
    // lv_demo_stress();
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

