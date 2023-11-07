#include "app_launcher.h"
#include <time.h>
#include "trace.h"
/*-----------------x
|
|
|
|
|
y*/
static gui_app_t app_dashboard =
{
    .screen =
    {
        .name = "app_dashboard",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_dashboard_ui_design,
    .active_ms = 60 * 1000 * 60,
};



uint8_t speed_mode = 0;
gui_img_t *sph0, *sph1, *speed;
gui_dynamic_img_t *sph0m, *sph1m, *speedm;
static void *sph_list[10] =
{
    SPE_0_BIN, SPE_1_BIN, SPE_2_BIN, SPE_3_BIN, SPE_4_BIN,
    SPE_5_BIN, SPE_6_BIN, SPE_7_BIN, SPE_8_BIN, SPE_9_BIN,
};
static void *dashboard_list[48] =
{
    DASHBOARD_0_BIN,  DASHBOARD_1_BIN,  DASHBOARD_2_BIN,  DASHBOARD_3_BIN,  DASHBOARD_4_BIN,
    DASHBOARD_5_BIN,  DASHBOARD_6_BIN,  DASHBOARD_7_BIN,  DASHBOARD_8_BIN,  DASHBOARD_9_BIN,
    DASHBOARD_10_BIN, DASHBOARD_11_BIN, DASHBOARD_12_BIN, DASHBOARD_13_BIN, DASHBOARD_14_BIN,
    DASHBOARD_15_BIN, DASHBOARD_16_BIN, DASHBOARD_17_BIN, DASHBOARD_18_BIN, DASHBOARD_19_BIN,
    DASHBOARD_20_BIN, DASHBOARD_21_BIN, DASHBOARD_22_BIN, DASHBOARD_23_BIN, DASHBOARD_24_BIN,
    DASHBOARD_25_BIN, DASHBOARD_26_BIN, DASHBOARD_27_BIN, DASHBOARD_28_BIN, DASHBOARD_29_BIN,
    DASHBOARD_30_BIN, DASHBOARD_31_BIN, DASHBOARD_32_BIN, DASHBOARD_33_BIN, DASHBOARD_34_BIN,
    DASHBOARD_35_BIN, DASHBOARD_36_BIN, DASHBOARD_37_BIN, DASHBOARD_38_BIN, DASHBOARD_39_BIN,
    DASHBOARD_40_BIN, DASHBOARD_41_BIN, DASHBOARD_42_BIN, DASHBOARD_43_BIN, DASHBOARD_44_BIN,
    DASHBOARD_45_BIN, DASHBOARD_46_BIN, DASHBOARD_47_BIN,
};
gui_app_t *get_app_dashboard(void)
{
    return &app_dashboard;
}

static void gui_font_init()
{
    gui_set_font_mem_resourse(32, GBK_32_32_DOT_BIN, GBK_UNICODE_TABLE_BIN);
}

static void change_speed_mode()
{
    if (++speed_mode > 47)
    {
        speed_mode = 0;
    }
    // gui_log("speed_mode : %d, sph0 : %d, sph1 : %d",speed_mode,speed_mode/10,speed_mode%10);
    gui_img_set_attribute(sph0, NULL, sph_list[speed_mode / 10], 206, 112);
    gui_img_set_attribute(sph1, NULL, sph_list[speed_mode % 10], 244, 112);
    gui_img_set_attribute(speed, NULL, dashboard_list[speed_mode], 138, 42);
    // gui_log("speed addr : %x ",sph_list[speed_mode]);
}
void task_entry(void *p)
{
    APP_PRINT_INFO0("dashboard task_entry");
    while (1)
    {
        //Application

        change_speed_mode();
        gui_thread_mdelay(40);
    }
}
static char test_img_red[60] =
{
    0x00, 0x00, 0x05, 0x00,
    0x05, 0x00, 0x00, 0x00,
    0xf8, 0x00, 0xf8, 0x00,
    0xf8, 0x00, 0xf8, 0x00,
    0xf8, 0x00, 0xf8, 0x00,
    0xf8, 0x00, 0xf8, 0x00,
    0xf8, 0x00, 0xf8, 0x00,
    0xf8, 0x00, 0xf8, 0x00,
    0xf8, 0x00, 0xf8, 0x00,
    0xf8, 0x00, 0xf8, 0x00,
    0xf8, 0x00, 0xf8, 0x00,
    0xf8, 0x00, 0xf8, 0x00,
    0xf8, 0x00, 0xf8, 0x00,
    0xf8, 0x00, 0xf8, 0x00,
    0xf8, 0x00, 0xf8, 0x00,
};
#define TIME_DY 100
void app_dashboard_ui_design(gui_app_t *app)
{
    APP_PRINT_INFO0("app_dashboard_ui_design");
    gui_obj_t *screen = &app->screen;
    gui_font_init();

    gui_img_t *bg = gui_img_create_from_mem(screen, "bg", BACKGROUND_BIN, 0, 0, 0, 0);
#if 0
    sph0 = gui_img_create_from_mem(screen, "sph0", sph_list[2], 206, 112, 0, 0);
    sph1 = gui_img_create_from_mem(screen, "sph1", sph_list[5], 244, 112, 0, 0);
    speed = gui_img_create_from_mem(screen, "speed", dashboard_list[24], 138, 42, 0, 0);
#else
    speedm = gui_dynamic_create_from_mem(screen, "speed", dashboard_list, 138, 42, 47, TIME_DY,
                                         TIME_DY * 100000);
    speedm->base.base.draw_img.blend_mode = IMG_FILTER_BLACK;
    sph0m = gui_dynamic_create_from_mem(screen, "sph0", sph_list, 206, 112, 5, TIME_DY * 10,
                                        TIME_DY * 1000000);
    sph0m->base.base.draw_img.blend_mode = IMG_FILTER_BLACK;
    sph1m = gui_dynamic_create_from_mem(screen, "sph1", sph_list, 244, 112, 10, TIME_DY,
                                        TIME_DY * 1000000);
    sph1m->base.base.draw_img.blend_mode = IMG_FILTER_BLACK;

#endif
    APP_PRINT_INFO0("app_dashboard_ui_design OK");
}
void gui_dashboard_init()
{
    APP_PRINT_INFO0("gui_dashboard_init");
    gui_app_install(get_app_dashboard(), get_app_dashboard()->ui_design, NULL);
    gui_app_startup(get_app_dashboard());
}
