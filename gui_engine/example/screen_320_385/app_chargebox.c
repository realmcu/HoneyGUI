#include <gui_tabview.h>
#include "gui_card.h"
#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include <gui_app.h>
#include "gui_tab.h"
#include "app_chargebox.h"
#include "gui_win.h"
#include "gui_server.h"
#include "draw_font.h"
#include "gui_components_init.h"
#include <stdio.h>
#include "font_mem.h"
#include "gui_button.h"

#define GIF_LOAD_NAME "arrow_load"

gui_tabview_t *tabview_main;
gui_tabview_t *tabview_info;
gui_win_t *win;
gui_img_t *screen_wallpaper;
gui_img_t *bat_left;
gui_img_t *bat_right;
gui_img_t *bat_case;
gui_img_t *bluetooth;
gui_text_t *left_battery_text;
gui_text_t *right_battery_text;
gui_text_t *case_battery_text;

static uint8_t arrow_x = 50;
uint8_t wallpaper_idx = 0;
uint32_t *wallpaper_list[WALLPAPER_NUM] = {WALLPAPER1_BIN, WALLPAPER2_BIN, WALLPAPER3_BIN};

static gui_app_t app_chargebox =
{
    .screen =
    {
        .name = "app_chargebox",
        .x    = 0,
        .y    = 0,
        .parent = NULL,
    },
    .ui_design = app_chargebox_ui_design,
    .thread_entry = NULL,
    .active_ms = 1000000,
};

void *get_app_chargebox(void)
{
    return &app_chargebox;
}

void app_gui_ble_action_cb(void *obj, gui_event_t e, void *param)
{
#ifndef _WIN32
#include "app_task.h"
    T_IO_MSG io_gui_msg_send;
    io_gui_msg_send.type = IO_MSG_TYPE_GUI;
    T_APP_GUI_SUBEVENT_TYPE subtype = (T_APP_GUI_SUBEVENT_TYPE)param;
    gui_log("subtype: %d", subtype);
    io_gui_msg_send.subtype = subtype;
    app_send_msg_to_apptask(&io_gui_msg_send);
#endif
}

static void screen_unlock_cb(void *obj, gui_event_t e)
{
    GET_BASE(win)->not_show = true;
    GET_BASE(tabview_info)->not_show = false;
    GET_BASE(tabview_main)->not_show = false;
}

static void img_loading_animate_callback(gui_win_t *win)
{
    gui_img_t *img;
    gui_obj_tree_get_widget_by_name((void *)win, GIF_LOAD_NAME, (void *)&img);
    if (win->animate->Beginning_frame)
    {
        arrow_x = arrow_x + 20;
        gui_img_set_attribute(img, GIF_LOAD_NAME, ARROW_BIN, arrow_x, GET_BASE(img)->y);
        if (arrow_x == 330)
        {
            arrow_x = 0;
        }
    }
}

static void app_chargebox_ui_design(gui_app_t *app)
{
    gui_log("app_chargebox_ui_design\n");
    gui_font_mem_init(HARMONYOS_REGULAR_SIZE24_BITS1_FONT_BIN);
    gui_font_mem_init(HARMONYOS_REGULAR_SIZE28_BITS1_FONT_BIN);

    tabview_info = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    GET_BASE(tabview_info)->not_show = true;
    tabview_main = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);
    GET_BASE(tabview_main)->not_show = true;

    win = gui_win_create(&(app->screen), "window", 0, 0, 0, 0);
    screen_wallpaper = gui_img_create_from_mem(win, "screen lock", wallpaper_list[wallpaper_idx], 0, 0,
                                               0, 0);
    gui_img_set_mode(screen_wallpaper, IMG_BYPASS_MODE);
    gui_img_create_from_mem(win, GIF_LOAD_NAME, ARROW_BIN, 50, 120, 0, 0);
    gui_win_set_animate(win, 4, -1, img_loading_animate_callback, win);
    gui_obj_add_event_cb(win, (gui_event_cb_t)screen_unlock_cb, GUI_EVENT_2, NULL);

    bat_left = gui_img_create_from_mem(tabview_info, "BAT_L", BAT_L_BIN, 10, 10, 0, 0);
    gui_img_set_mode(bat_left, IMG_BYPASS_MODE);
    bat_right = gui_img_create_from_mem(tabview_info, "BAT_R", BAT_R_BIN, 110, 10, 0, 0);
    gui_img_set_mode(bat_right, IMG_BYPASS_MODE);
    bat_case = gui_img_create_from_mem(tabview_info, "BAT_CASE", BAT_CASE_BIN, 210, 10, 0, 0);
    gui_img_set_mode(bat_case, IMG_BYPASS_MODE);
    bluetooth = gui_img_create_from_mem(tabview_info, "BLUETOOTH_OFF", BLUETOOTH_OFF_BIN, 340, 10, 0,
                                        0);
    gui_img_set_mode(bluetooth, IMG_BYPASS_MODE);

    char *left_battery = "100%";
    left_battery_text = gui_text_create(tabview_info, "battery level txt", 50, 15, 100, 24);
    gui_text_set(left_battery_text, left_battery, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                            UINT8_MAX), strlen(left_battery), 24);

    char *right_battery = "100%";
    right_battery_text = gui_text_create(tabview_info, "battery level txt", 150, 15, 100, 24);
    gui_text_set(right_battery_text, right_battery, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                              UINT8_MAX), strlen(right_battery), 24);

    char *case_battery = "100%";
    case_battery_text = gui_text_create(tabview_info, "battery level txt", 250, 15, 100, 24);
    gui_text_set(case_battery_text, case_battery, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                            UINT8_MAX), strlen(case_battery), 24);

    gui_tab_t *tb_music = gui_tab_create(tabview_main, "tb_music",           0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_volume = gui_tab_create(tabview_main, "tb_volume",         0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_call = gui_tab_create(tabview_main, "tb_call",             0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_anc = gui_tab_create(tabview_main, "tb_anc",               0, 0, 0, 0, 3, 0);
    gui_tab_t *tb_equalizer = gui_tab_create(tabview_main, "tb_equalizer",   0, 0, 0, 0, 4, 0);
    gui_tab_t *tb_clock = gui_tab_create(tabview_main, "tb_clock",           0, 0, 0, 0, 5, 0);
    gui_tab_t *tb_brightness = gui_tab_create(tabview_main, "tb_brightness", 0, 0, 0, 0, 6, 0);
    gui_tab_t *tb_language = gui_tab_create(tabview_main, "tb_language",     0, 0, 0, 0, 7, 0);
    gui_tab_t *tb_wallpaper = gui_tab_create(tabview_main, "tb_wallpaper",   0, 0, 0, 0, 8, 0);
    gui_tab_t *tb_findtws = gui_tab_create(tabview_main, "tb_find",          0, 0, 0, 0, 9, 0);
    gui_tab_t *tb_flashlight = gui_tab_create(tabview_main, "tb_flashlight", 0, 0, 0, 0, 10, 0);

    page_tb_music(tb_music);
    page_tb_volume(tb_volume);
    page_tb_call(tb_call);
    page_tb_anc(tb_anc);
    page_tb_equalizer(tb_equalizer);
    page_tb_clock(tb_clock);
    page_tb_brightness(tb_brightness);
    page_tb_language(tb_language);
    page_tb_wallpaper(tb_wallpaper);
    page_tb_findtws(tb_findtws);
    page_tb_flashlight(tb_flashlight);
}


uint8_t resource_root[1024 * 1024 * 20];
static int app_init(void)
{
#if defined _WIN32
    int fd;
    fd = open("./gui_engine/example/screen_320_385/root_image_chargebox/root(0xa00000).bin", 0);
    if (fd > 0)
    {
        printf("open root(0xa00000).bin Successful!\n");
        read(fd, resource_root, 1024 * 1024 * 20);
    }
    else
    {
        printf("open root(0xa00000).bin Fail!\n");
        printf("open root(0xa00000).bin Fail!\n");
        printf("open root(0xa00000).bin Fail!\n");
        return 0;
    }
#endif
    gui_server_init();
    gui_app_startup(get_app_chargebox());
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);

