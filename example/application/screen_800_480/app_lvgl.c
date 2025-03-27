#include <gui_app.h>
#include "lvgl.h"
#include "gui_components_init.h"
#include <string.h>
#include "app_msg.h"
#include "board.h"
#include "trace.h"
#include "os_sync.h"
#include "os_task.h"
#include "rtl_gpio.h"
#include "os_sched.h"
#include "lv_port_disp.h"
#include "lv_demo_benchmark.h"
#include "lv_demo_widgets.h"
#include "os_timer.h"
#include "utils.h"
#include "trace.h"
#include "gui_win.h"
#include "app_dashboard_data.h"
#include "gui_app.h"
#include "rtl876x.h"
#include "app_launcher.h"
#include "lv_obj.h"

static void app_lvgl_ui_design(gui_app_t *app);
#define APP_TASK_PRIORITY               1   /* Task priorities. */
#define APP_TASK_STACK_SIZE             (512 * 16)
void *lvgl_task_handle;
static void lvgl_loop(void *tihs)
{
    while (1)
    {
        lv_task_handler();                 //! run lv task at the max speed
    }
}

static gui_app_t app_lvgl =
{
    .screen =
    {
        .name = "app_lvgl",
        .x    = 0,
        .y    = 0,
        .parent = NULL,
    },
    .ui_design = app_lvgl_ui_design,
    .thread_entry = lvgl_loop,
    .active_ms = 1000000,
    .lvgl = true,
};

gui_app_t *get_app_lvgl(void)
{
    return &app_lvgl;
}

static void lv_tick(void)
{
    lv_tick_inc(10);
}

static void on_benchmark_finished(void)
{
    disp_enable_update();
}


#if 0
static int app_init(void)
{
    gui_server_init();
    gui_app_startup(get_app_lvgl());
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
#endif
static void port_log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char buf[512];
    vsnprintf(buf, sizeof(buf), format, args);

    DBG_DIRECT("[GUI MODULE]%s", buf);

    va_end(args);
}
static void *test_timer1 = NULL;
static void test_timer_callback(void *pxTimer)
{
    lv_tick_inc(10);
}
static void lvgl_demo_run(void *p)
{
    os_timer_create(&test_timer1, "lvgl tick", 0, 10, true,
                    test_timer_callback);
    os_timer_start(&test_timer1);
    lv_log_register_print_cb((lv_log_print_g_cb_t)port_log);
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
#if LV_USE_DEMO_BENCHMARK
    LV_LOG("Running LVGL Benchmark... \n");
    LV_LOG("Please stand by... \n");
    LV_LOG("NOTE: You will NOT see anything until the end. \n");

    lv_demo_benchmark_set_finished_cb(&on_benchmark_finished);
    lv_demo_benchmark_set_max_speed(true);
    lv_demo_benchmark();
#endif

//    lv_example_arc_1();

    while (1)
    {
        lv_timer_handler();                 //! run lv task at the max speed
    }
}


static void app_lvgl_ui_design(gui_app_t *app)
{
//    gui_set_tick_hook(lv_tick);
//    lv_init();
//    lv_port_disp_init();
//    lv_port_indev_init();
//#if LV_USE_DEMO_BENCHMARK
//    LV_LOG("Running LVGL Benchmark... \n");
//    LV_LOG("Please stand by... \n");
//    LV_LOG("NOTE: You will NOT see anything until the end. \n");

//    lv_demo_benchmark_set_finished_cb(&on_benchmark_finished);
//    lv_demo_benchmark_set_max_speed(true);
//    lv_demo_benchmark();
//#endif
//#if LV_USE_DEMO_WIDGETS
//    lv_demo_widgets();
//#endif
//    //lv_example_arc_1();
//    //lv_example_btn_1();
//    //lv_example_btn_1();

    os_task_create(&lvgl_task_handle, "lvgl", lvgl_demo_run, 0, APP_TASK_STACK_SIZE,
                   APP_TASK_PRIORITY);
}