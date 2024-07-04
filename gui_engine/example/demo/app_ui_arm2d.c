#include "guidef.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>
#include "arm_2d.h"
#include "arm_2d_disp_adapters.h"
#include "arm_2d_helper.h"
#include "gui_server.h"
#include <pthread.h>
#include "gui_components_init.h"
#include "arm_2d_scenes.h"
#ifdef RTE_Acceleration_Arm_2D_Extra_Benchmark
#   include "arm_2d_benchmark.h"
#endif


ARM_NOINIT
extern
arm_2d_scene_player_t DISP0_ADAPTER;

static void arm2d_loop(void *tihs)
{
    while ((1))
    {
        pthread_testcancel();
        while (arm_fsm_rt_cpl != disp_adapter0_task());
    }
}

static void app_arm2d_ui_design(gui_app_t *app)
{
    arm_irq_safe
    {
        arm_2d_init();
    }

    disp_adapter0_init();

#ifdef RTE_Acceleration_Arm_2D_Extra_Benchmark
    arm_2d_run_benchmark();
#endif
}


static gui_app_t rtk_gui_demo =
{
    .screen =
    {
        .name = "app_ui_arm2d",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_arm2d_ui_design,
    .thread_entry = arm2d_loop,
    .active_ms = 1000 * 60 * 60,
    .arm2d = true,
};


static int app_init(void)
{
    gui_server_init();
    gui_app_startup(&rtk_gui_demo);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);

