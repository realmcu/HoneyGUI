#include "lvgl.h"

#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "app_msg.h"
#include "app_task.h"
#include "board.h"
#include "trace.h"
#include "os_sync.h"
#include "rtl_gpio.h"

#include "lv_port_disp.h"
#include "lv_demo_benchmark.h"
#include "lv_demo_widgets.h"
#include "os_timer.h"
#include "trace.h"

#define APP_TASK_PRIORITY               1   /* Task priorities. */
#define APP_TASK_STACK_SIZE             (512 * 16)

static void port_log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char buf[256];
    vsnprintf(buf, sizeof(buf), format, args);

    DBG_DIRECT("[GUI MODULE]%s", buf);

    va_end(args);
}

void *lvgl_task_handle;

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

    lv_example_arc_1();

    while (1)
    {
        lv_timer_handler();                 //! run lv task at the max speed
        platform_delay_ms(10);

    }
}

void rt_lvgl_demo_init(void)
{
    /* littleGL demo gui thread */
    os_task_create(lvgl_task_handle, "lvgl", lvgl_demo_run, 0, APP_TASK_STACK_SIZE,
                   APP_TASK_PRIORITY);

}
//INIT_APP_EXPORT(rt_lvgl_demo_init);
