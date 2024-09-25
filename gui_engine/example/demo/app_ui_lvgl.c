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
#include "lv_demo_widgets.h"



static void lvgl_loop(void *tihs)
{
    while ((1))
    {
        pthread_testcancel();
        lv_task_handler();                 //! run lv task at the max speed
    }
}

static void lv_tick(void)
{
    lv_tick_inc(10);
}

static void on_benchmark_finished(void)
{
    disp_enable_update();
}

static void event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        LV_LOG_USER("Clicked");
        //gui_switch_app(gui_current_app(), get_app_hongkong());
    }
    else if (code == LV_EVENT_VALUE_CHANGED)
    {
        LV_LOG_USER("Toggled");
    }
}

static void load_font_bin_file(void)
{
    lv_font_t *my_font;
    my_font = lv_font_load("F:/lv_font_harmony_32.bin");
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(label, my_font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(label, "Hello LVGL! By bin file.");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 60, 15);
}
static void load_img_bin_file(void)
{
    lv_obj_t *icon = lv_img_create(lv_scr_act());
    lv_img_set_src(icon, "F:/lvgl_logo.bin");
    lv_obj_set_pos(icon, 10, 10);
}

#include "../screen_lvgl/root/lv_font_harmony_32.c"
static void load_font_c_file(void)
{
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(label, &lv_font_harmony_32, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(label, "Hello LVGL! By C file.");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 60, 105);
}
#include "../screen_lvgl/root/lvgl_logo.c"
static void load_img_c_file(void)
{
    lv_obj_t *icon = lv_img_create(lv_scr_act());
    lv_img_set_src(icon, &lvgl_logo);
    lv_obj_set_pos(icon, 10, 100);
}
static void lv_tool_file_test(void)
{
//C file
    load_img_c_file();
    load_font_c_file();

//bin file
    load_img_bin_file();
    load_font_bin_file();
}

static void app_dialing_ui_design(gui_app_t *app)
{
    if (lv_is_initialized() == true)
    {
        return;
    }
    gui_set_tick_hook(lv_tick);
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    lv_port_fs_init();
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
    // lv_obj_t *btn2 = lv_btn_create(lv_scr_act());
    // lv_obj_add_event_cb(btn2, event_handler, LV_EVENT_ALL, NULL);
    // lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 40);
    // lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    // lv_obj_set_height(btn2, LV_SIZE_CONTENT);

    // lv_obj_t *label = lv_label_create(btn2);
    // lv_label_set_text(label, "Toggle");
    // lv_obj_center(label);
#endif
    // lv_example_arc_1();
    // lv_example_btn_1();
    // lv_tool_file_test();
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







