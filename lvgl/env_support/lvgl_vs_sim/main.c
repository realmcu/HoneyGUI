/*
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "unistd.h"
#include "menu_config.h"
#include "signal.h"
#include "time.h"
#include "sdl_init.h"
#include "lvgl.h"
#include "lv_example_widgets.h"
#include <pthread.h>




static void *lv_tick_task(void *arg)
{
    while (1)
    {
        usleep(1000);
        lv_tick_inc(1);

    }
}
extern void disp_enable_update(void);
extern void disp_disable_update(void);

static void on_benchmark_finished(void)
{
    disp_enable_update();
}


int main(int argc, char **argv)
{
    pthread_t thread;
    pthread_create(&thread, NULL, lv_tick_task, NULL);
    sdl_dc_init();

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
    // lv_demo_widgets();
#endif
    //lv_example_arc_1();
    //lv_example_btn_1();
    lv_example_btn_1();

    while (1)
    {
        lv_task_handler();                 //! run lv task at the max speed
    }

    return 0;
}

