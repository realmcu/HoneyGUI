#include "lvgl.h"

#include <rtthread.h>
#include <rtdevice.h>
#include "lv_port_disp.h"
#include "lv_demo_benchmark.h"
#include "lv_demo_widgets.h"
#include "os_timer.h"

extern void disp_enable_update(void);
extern void disp_disable_update(void);

static void on_benchmark_finished(void)
{
    disp_enable_update();
}

void lv_example_arc_1(void)
{
    lv_obj_t *label = lv_label_create(lv_scr_act());
    /*Create an Arc*/
    lv_obj_t *arc = lv_arc_create(lv_scr_act());
    lv_obj_set_size(arc, 150, 150);
    lv_arc_set_rotation(arc, 135);
    lv_arc_set_bg_angles(arc, 0, 270);
    lv_arc_set_value(arc, 10);
    lv_obj_center(arc);
    //lv_obj_add_event_cb(arc, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, label);
    /*Manually update the label for the first time*/
    lv_event_send(arc, LV_EVENT_VALUE_CHANGED, NULL);
}


static void *test_timer1 = NULL;
static void test_timer_callback(void *pxTimer)
{
    lv_tick_inc(10);
}
static void lvgl_demo_run(void *p)
{
    os_timer_create(&test_timer1, "lvgl tick", 1, 10, true,
                    test_timer_callback);
    os_timer_start(&test_timer1);
    lv_log_register_print_cb((lv_log_print_g_cb_t)rt_kprintf);
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    LV_LOG("Running LVGL Benchmark... \n");
    LV_LOG("Please stand by... \n");
    LV_LOG("NOTE: You will NOT see anything until the end. \n");

    lv_demo_benchmark_set_finished_cb(&on_benchmark_finished);
    lv_demo_benchmark_set_max_speed(true);
    lv_demo_benchmark();


    lv_example_arc_1();

    while (1)
    {
        lv_timer_handler();                 //! run lv task at the max speed
        rt_thread_delay(10);

    }
}

int rt_lvgl_demo_init(void)
{
    rt_err_t ret = RT_EOK;
    rt_thread_t thread = RT_NULL;

    if (ret != RT_EOK)
    {
        return ret;
    }

    /* littleGL demo gui thread */
    thread = rt_thread_create("lv_demo", lvgl_demo_run, RT_NULL, 8 * 1024, 5, 10);
    if (thread == RT_NULL)
    {
        return RT_ERROR;
    }
    rt_thread_startup(thread);

    return RT_EOK;
}
//INIT_APP_EXPORT(rt_lvgl_demo_init);
MSH_CMD_EXPORT(rt_lvgl_demo_init, lcd display test);
