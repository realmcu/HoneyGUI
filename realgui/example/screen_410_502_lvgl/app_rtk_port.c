#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "os_task.h"
#include "os_timer.h"
#include "trace.h"

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_port_fs.h"

#include "app_main.h"

#define APP_TASK_PRIORITY               1   /* Task priorities. */
#define APP_TASK_STACK_SIZE             (512 * 16)

static void port_log(lv_log_level_t level, const char *buf)
{
    if (level >= LV_LOG_LEVEL)
    {
        DBG_DIRECT("%s", buf);
    }
}

void *lvgl_task_handle;

static void *gui_timer0 = NULL;
static void lv_tick(void *pxTimer)
{
    lv_tick_inc(10);
}

static void lvgl_demo_run(void *p)
{
    os_timer_create(&gui_timer0, "lvgl tick", 1, 10, true, lv_tick);
    os_timer_start(&gui_timer0);
    lv_init();
    lv_log_register_print_cb((lv_log_print_g_cb_t)port_log);
    lv_port_disp_init();
    lv_port_indev_init();
    // lv_port_fs_init();

    DBG_DIRECT("LVGL start \n");

    watch_demo_init();
    while (1)
    {
        lv_task_handler();
    }
}

void rt_lvgl_demo_init(void)
{
    /* littleGL demo gui thread */
    os_task_create(&lvgl_task_handle, "lvgl", lvgl_demo_run, 0, APP_TASK_STACK_SIZE,
                   APP_TASK_PRIORITY);
}
