#include <gui_app.h>
#include "lvgl.h"

static void app_lvgl_ui_design(gui_app_t *app);


static void lvgl_loop(void *tihs)
{
    while ((1))
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

void *get_app_lvgl(void)
{
    return &app_lvgl;
}

static void lv_tick(void)
{
    lv_tick_inc(10);
}

static void app_lvgl_ui_design(gui_app_t *app)
{
    gui_set_tick_hook(lv_tick);
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
#if LV_USE_DEMO_BENCHMARK
    // LV_LOG("Running LVGL Benchmark... \n");
    // LV_LOG("Please stand by... \n");
    // LV_LOG("NOTE: You will NOT see anything until the end. \n");

    // lv_demo_benchmark_set_finished_cb(&on_benchmark_finished);
    // lv_demo_benchmark_set_max_speed(true);
    // lv_demo_benchmark();
#endif
#if LV_USE_DEMO_WIDGETS
    lv_demo_widgets();
#endif
    //lv_example_arc_1();
    //lv_example_btn_1();
    //lv_example_btn_1();
}

