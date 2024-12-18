#include <gui_tabview.h>
#include "gui_card.h"
#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_hongkong/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "app_hongkong.h"
#include "gui_win.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include <stdio.h>
#include "guidef.h"
#include "wheel_algo.h"
#include "kb_algo.h"
#include <time.h>

static void app_hongkong_ui_design(gui_app_t *app);

static gui_tabview_t *tv;
static gui_win_t *win_hk; //, *win_control;
bool control_flag = 0;
static bool enter_menu_flag = 0;

static gui_app_t app_hongkong =
{
    .screen =
    {
        .name = "app_hongkong",
        .x    = 0,
        .y    = 0,
        .parent = NULL,
    },
    .ui_design = app_hongkong_ui_design,
    .active_ms = 1000000,
    .shutdown_animation_flag = GUI_APP_ANIMATION_10,
    .startup_animation_flag = GUI_APP_ANIMATION_9,
};

void *get_app_hongkong(void)
{
    return &app_hongkong;
}

static void kb_button_cb()
{
    extern gui_kb_port_data_t *kb_get_data(void);
    gui_kb_port_data_t *kb = kb_get_data();
    static uint32_t time_press = 0;
    static uint8_t hold = 0;
    if (hold)
    {
        if (kb->event == GUI_KB_EVENT_UP)
        {
            hold = 0;
            uint32_t time = kb->timestamp_ms_release - time_press;
            if (time <= 500)
            {
                gui_log("pressing time = %d\n", time);
                gui_obj_event_set(GUI_BASE(win_hk), GUI_EVENT_8);
            }
        }
    }
    if (kb->event == GUI_KB_EVENT_DOWN && !hold)
    {
        time_press = kb->timestamp_ms_press;
        hold = 1;
    }
}

static void switch_app_menu()
{
    if (!enter_menu_flag)
    {
        enter_menu_flag = 1;
        gui_log("enter menu\n");
        extern void close_box2d_ring(void);
        close_box2d_ring();

        extern void *get_app_menu();
        gui_switch_app(gui_current_app(), get_app_menu());
    }
}

static void app_hongkong_ui_design(gui_app_t *app)
{
    gui_log("app_hongkong_ui_design\n");
    // extern gui_app_t *_get_app_APP_MUSIC_handle(void);
    // gui_switch_app(gui_current_app(), _get_app_APP_MUSIC_handle());
    // extern gui_app_t *get_app_menu();
    // gui_switch_app(gui_current_app(), get_app_menu());
    // extern gui_app_t *_get_app_APP_BOX2D_RING_handle();
    // gui_switch_app(gui_current_app(), _get_app_APP_BOX2D_RING_handle());
    // extern gui_app_t *_get_app_APP_FRUIT_NINJA_handle();
    // gui_switch_app(gui_current_app(), _get_app_APP_FRUIT_NINJA_handle());
    // return;

    tv = gui_tabview_create(app->window, "hongkong_tabview", 0, 0, 0, 0);
    win_hk = gui_win_create(app->window, "window_hongkong", 0, 0, 0, 0);
    gui_win_set_animate(win_hk, 1000, -1, kb_button_cb, NULL);
    gui_obj_add_event_cb(win_hk, (gui_event_cb_t)switch_app_menu, GUI_EVENT_8,
                         app->window); //GUI_EVENT_8
    enter_menu_flag = 0;
    gui_tabview_set_style(tv, TAB_CUBE);
    gui_tabview_enable_pre_load(tv, true);
    extern void page_tb_control_enter(void *parent);
    page_tb_control_enter(app->window);

    gui_tab_t *tb_clock = gui_tab_create(tv, "tb_clock",           0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_activity = gui_tab_create(tv, "tb_activity",     0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_heart = gui_tab_create(tv, "tb_heart",           0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_cube = gui_tab_create(tv, "tb_cube",           0, 0, 0, 0, 3, 0);
    gui_tab_t *tb_weather = gui_tab_create(tv, "tb_weather",       0, 0, 0, 0, 5, 0);
    gui_tab_t *tb_music = gui_tab_create(tv, "tb_music",           0, 0, 0, 0, 4, 0);
    gui_tab_t *tb_ani = gui_tab_create(tv, "tb_ani",          0, 0, 0, 0, 6, 0);
    // page_tb_clock(win);
    page_tb_clock(gui_tab_get_rte_obj(tb_clock));
    // page_tb_control(gui_tab_get_rte_obj(tb_activity));
    page_tb_activity(gui_tab_get_rte_obj(tb_activity));
    page_tb_heart(gui_tab_get_rte_obj(tb_heart));
    page_tb_cube(gui_tab_get_rte_obj(tb_cube));
    page_tb_weather(gui_tab_get_rte_obj(tb_weather));
    page_tb_music(gui_tab_get_rte_obj(tb_music));
}

// static void data_generate_task_entry()
// {
//     while(true)
//     {
//         extern void json_refreash();
//         json_refreash();
//         gui_thread_mdelay(2000);
//     }
// }

typedef enum
{
    MESSAGE = 0,
    OS,
} app_name;

typedef struct information
{
    const char *informer;
    const char *content;
    const char *time;
    app_name app
} information_t;

static void inform_generate_task_entry()
{
    while (true)
    {
        char *content = "Watch will attempt to install this update later tonight.";
        extern struct tm *timeinfo;
        char time[10];
        sprintf(time, "%d:%d", timeinfo->tm_hour, timeinfo->tm_min);

        information_t payload =
        {
            "watchOS 10.3.1",
            content,
            time,
            OS
        };
        extern void pagelist_create(information_t *payload);
        gui_msg_t msg =
        {
            .event = GUI_EVENT_USER_DEFINE,
            .payload = &payload,
            .cb = (gui_msg_cb)pagelist_create,
        };

        gui_send_msg_to_server(&msg);

        gui_thread_mdelay(2000);
    }
}

uint8_t resource_root[1024 * 1024 * 20];
static int app_init(void)
{
#if defined _WIN32
    extern int open(const char *file, int flags, ...);
    extern int read(int fd, void *buf, size_t len);
    extern int close(int fd);
    defaultPath = "realgui\\example\\screen_410_502\\root_image_hongkong\\root\\";
    int fd;
    fd = open("./realgui/example/screen_410_502/root_image_hongkong/root(0x4400000).bin", 0);
    if (fd > 0)
    {
        printf("open root(0x4400000).bin Successful!\n");
        read(fd, resource_root, 1024 * 1024 * 20);
    }
    else
    {
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        return 0;
    }
#endif
    gui_server_init();
    gui_app_startup(get_app_hongkong());
    // gui_thread_create("data_generate_task", data_generate_task_entry, 0, 1024 * 2, 2);
    gui_thread_create("inform_generate_task_entry", inform_generate_task_entry, 0, 1024 * 2, 2);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);

