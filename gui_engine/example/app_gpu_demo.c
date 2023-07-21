/****************Fixed content start***********************/
#ifdef RTK_GUI_GPU_VG_LITE
#if 0
#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_curtain.h>
#include <gui_curtain.h>
#include <gui_chessboard.h>
#include <gui_roulette.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include "gui_demo.h"
#include "app_dialing.h"
#include "app_demopath.h"
gui_app_t *gpu_app;
/*************************app1: app_gpu**********************/
//static struct rt_mutex app_gpu_screen_mutex;
static gui_obj_t app_gpu_screen =
{
    .name = "app_gpu",
    .x    = 0,
    .y    = 0,
//    .mutex = &app_gpu_screen_mutex,
};
/* thread declation*/
static gui_thread_t gui_gpu_tid;
static gui_img_t *image_HourPointer[6];
static gui_img_t *image_MinutePointer[6];
static gui_img_t *image_SecondPointer[6];
static gui_img_t *image_HourPointer_fine;
static gui_img_t *image_MinutePointer_fine;
static gui_img_t *image_SecondPointer_fine;
static gui_timer_t clock_timer_handle = NULL;
static char *string;
static gui_curtain_t *curtain_left;
static gui_tabview_t *tabview_0;
static gui_tab_t *tab0;
static gui_tab_t *tab4;
static gui_curtain_t *curtain_center ;
static gui_img_t *image_center_icon_0;
static float i = 0.0f;
/*call back handler declaration*/
static void clock_rotation_handler(void *p_xtimer);
static void setting_icon_cb(void);
static void heartrate_icon_cb(void);
static void iconlist_win_cb(void);
static void flappybird_iocn_cb(void);
static void btcall_icon_cb(void);
static void iconbox_icon_cb(void);
static void home_icon_cb(void);
static void album_icon_cb(void);
static void create_icon_list_theme0(void *parent);
static void create_icon_list_theme1(void *parent);
static void create_icon_list_theme2(void *parent);
#ifdef CLOCK_ARR_FINE
#define GPU_HOUR_WIDTH                24
#define GPU_HOUR_CENTER_HEIGHT        105
#define GPU_MINUTE_WIDTH              30
#define GPU_MINUTE_CENTER_HEIGHT      132
#define GPU_SECOND_WIDTH              4
#define GPU_SECOND_CENTER_HEIGHT      141
#else
#define GPU_HOUR_WIDTH                12
#define GPU_HOUR_CENTER_HEIGHT        160
#define GPU_MINUTE_WIDTH              8
#define GPU_MINUTE_CENTER_HEIGHT      160
#define GPU_SECOND_WIDTH              3
#define GPU_SECOND_CENTER_HEIGHT      160
#endif
/*
#define CLOCK_ARR_FINE
#ifdef CLOCK_ARR_FINE
#define GPU_HOUR_WIDTH                28
#define GPU_HOUR_CENTER_HEIGHT        92
#define GPU_MINUTE_WIDTH              30
#define GPU_MINUTE_CENTER_HEIGHT      132
#define GPU_SECOND_WIDTH              4
#define GPU_SECOND_CENTER_HEIGHT      141
#else
#define GPU_HOUR_WIDTH                54
#define GPU_HOUR_CENTER_HEIGHT        105
#define GPU_MINUTE_WIDTH              54
#define GPU_MINUTE_CENTER_HEIGHT      144
#define GPU_SECOND_WIDTH              18
#define GPU_SECOND_CENTER_HEIGHT      145
#endif
*/
#define GPU_ICONLIST_PARENT_NAME      "iconlist_window1"
#define GPU_ICON_WIDTH            65
#define GPU_ICON_HIGHT            65
static void task_entry(void *p)
{
    gui_list_init(&(gpu_app->screen->child_list));
    gui_list_init(&(gpu_app->screen->brother_list));
    gui_log("enter gpu app thread\n");
    /****************Fixed content end***********************/
    /****************Generated content start*********************************************************************************
    ************************************************************************************************************************/
    /***create tabview*/
    tabview_0 = gui_tabview_create(gpu_app->screen, "tabview", 0, 0, 0, 0);
    /****create middle tab  idx=0,idy=0*/
    {
        tab0 = gui_tab_create(tabview_0, "tab0", 0, 0, 0, 0, 0, 0);
        {
            /*create curtainview*/
            gui_curtainview_t *curtainview = gui_curtainview_create(tab0, "curtainview", 0, 0, 0, 0, 0.5);
            /***********create curtain MIDDLE*/
            {
                curtain_center = gui_curtain_create(curtainview, "curtain_center", 0, 0, 0, 0, MIDDLE);
                gui_graphic_t *cg = gui_graphic_create(curtain_center, "cg", 0, 0, 0, 0);
                cg->draw_rect(cg, 0, 0, 200, 200, 0xffffff00);
            }
        }
    }
    gpu_app->inited = true;
    /****************Generated content end*********************************************************************************
    ***********************************************************************************************************************/
    /****************Fixed content start***********************/
    gui_exec(gpu_app->screen);
    while (1)
    {
        gui_thread_mdelay(10000000);
    }
}
int app_gpu_install(void)
{
    gpu_app = gui_install_app(&app_gpu_screen, gui_gpu_tid, NULL,
                              "app_gpu",
                              task_entry, NULL, 100);
    return 0;
}
int app_gpu_startup(void)
{
    gui_startup_app(gpu_app);
    return 0;
}
INIT_APP_EXPORT(app_gpu_install);
//INIT_APP_EXPORT(app_gpu_startup);
MSH_CMD_EXPORT(app_gpu_startup, lcd display test);
#endif
#endif /* RTK_GUI_GPU_VG_LITE */
/****************Fixed content end***********************/
