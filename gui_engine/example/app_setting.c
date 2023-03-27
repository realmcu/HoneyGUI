/*************************app data start**********************/
/*1. gui head file*/
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_iconlist.h>
#include "gui_server.h"
#include <gui_app.h>
#include <string.h>

static gui_win_t *win;
static gui_img_t *image_list;
static gui_img_t *image_chess;
static gui_img_t *image_roulette;
static gui_img_t *image_roulette2;
static gui_img_t *image_style4;
static gui_img_t *image_style5;
static gui_text_t *text_list;
static gui_text_t *text_chess;
static gui_text_t *text_roulette;
static gui_text_t *text_roulette2;
static gui_text_t *text_style4;
static gui_text_t *text_style5;
static gui_text_t *text_theme;
static gui_text_t *text_info;
static gui_text_t *text_bt;
static gui_text_t *text_lcd;
static gui_text_t *text_app;
static gui_text_t *text_update;
static gui_text_t *text_info_version;
static gui_text_t *text_info_os;
static gui_text_t *text_info_rom;
static gui_text_t *text_info_ram;
static gui_text_t *text_info_lcd;
static gui_iconlist_t *bt_menu;
static gui_icon_t *bt_icon[6];
static gui_text_t *bt_addr[6];
static gui_text_t *bt_name[6];
void *bt_scan_timer;
size_t bt_scan_timer_period;
void *bt_scan_end_timer;
size_t bt_scan_end_timer_period;

static void app_setting_ui_design(gui_app_t *app);
//    flybird
static gui_app_t app_setting =
{
    .screen =
    {
        .name = "app_setting",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_setting_ui_design,
};

void *get_app_setting(void)
{
    return &app_setting;
}

/*5. trigger callback function*/
static void theme_cb(void *obj, gui_event_t e);
static void info_cb(void *obj, gui_event_t e);
static void bt_cb(void *obj, gui_event_t e);
static void lcd_cb(void *obj, gui_event_t e);
static void apps_cb(void *obj, gui_event_t e);
static void update_cb(void *obj, gui_event_t e);

static void switch_to_verticallist_cb(void *obj, gui_event_t e);
static void switch_to_chessboard_cb(void *obj, gui_event_t e);
static void switch_to_roulette_cb(void *obj, gui_event_t e);
static void switch_to_style4_cb(void *obj, gui_event_t e);
static void switch_to_style5_cb(void *obj, gui_event_t e);
static void switch_to_style6_cb(void *obj, gui_event_t e);
static void win_base_page_cb(void *obj, gui_event_t e);
static void return_to_base_page_cb(void *obj, gui_event_t e);
static void bt_return_to_base_page_cb(void *obj, gui_event_t e);
/*************************app data end************************/
/*************************user data start**********************/
#include <string.h>
#include "rtk_gui_resource.h"
#define THEME0_NAME             "VList"
#define THEME1_NAME             "Chessboard"
#define THEME2_NAME             "Wheel"
#define THEME3_NAME             "Style 4"
#define THEME4_NAME             "Style 5"
#define THEME5_NAME             "Style 6"
#define SELECTED_IMAGE          SELECT_ICON_GREEN_BIN
#define UNSELECTED_IMAGE        SELECT_ICON_WHITE_BIN
#define SELECTED_FONT_COLOR     0x0400
#define UNSELECTED_FONT_COLOR   0xffff
#define FONT_NAME               "rtk_font_mem32"
#define FONT_SIZE               32
#define SETTING_ICON_WIDTH              320
#define SETTING_ICON_HIGHT              65
#define ICONLIST_PARENT_NAME    "iconlist_window1"
/*************************user data end************************/
static void create_base_page(void)
{
    gui_app_t *app = get_app_setting();
    win = gui_win_create(&(app->screen), "win_set", 0, 0, 320, 320);
    gui_obj_add_event_cb(win, win_base_page_cb, GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE,  NULL);

    /*create iconlist*/
    gui_iconlist_t *base_menu = gui_iconlist_create(win, "base_menu", 0, 0, 0, 0, NULL, NULL);
    base_menu->set_background(base_menu, ICONBACK_BIN, ICONBACKHIGHLIGHT_BIN);
    gui_icon_t *theme = gui_icon_create(base_menu, "theme", 0, 0, SETTING_ICON_WIDTH,
                                        SETTING_ICON_HIGHT, 0);
    gui_icon_t *info = gui_icon_create(base_menu, "info", 0, 0, SETTING_ICON_WIDTH,
                                       SETTING_ICON_HIGHT, 1);
    gui_icon_t *bt = gui_icon_create(base_menu, "bt", 0, 0, SETTING_ICON_WIDTH,
                                     SETTING_ICON_HIGHT, 2);
    gui_icon_t *lcd = gui_icon_create(base_menu, "icon_roulette2", 0, 0, SETTING_ICON_WIDTH,
                                      SETTING_ICON_HIGHT, 3);
    gui_icon_t *apps = gui_icon_create(base_menu, "apps", 0, 0, SETTING_ICON_WIDTH,
                                       SETTING_ICON_HIGHT, 4);
    gui_icon_t *update = gui_icon_create(base_menu, "update", 0, 0, SETTING_ICON_WIDTH,
                                         SETTING_ICON_HIGHT, 5);
    gui_obj_add_event_cb(theme, theme_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    gui_obj_add_event_cb(info, info_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    gui_obj_add_event_cb(bt, bt_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    gui_obj_add_event_cb(lcd, lcd_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    gui_obj_add_event_cb(apps, apps_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    gui_obj_add_event_cb(update, update_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);

    text_theme = gui_text_create(theme, "text_theme", 10, 20, 320, 40);
    text_info = gui_text_create(info, "info_theme", 10, 20, 320, 40);
    text_bt = gui_text_create(bt, "bt_theme", 10, 20, 320, 40);
    text_lcd = gui_text_create(lcd, "lcd_theme", 10, 20, 320, 40);
    text_app = gui_text_create(apps, "apps_theme", 10, 20, 320, 40);
    text_update = gui_text_create(update, "update_theme", 10, 20, 320, 40);
    gui_text_set(text_theme, "theme", FONT_NAME, UNSELECTED_FONT_COLOR, strlen("theme"), 32);
    gui_text_set(text_info, "info", FONT_NAME, UNSELECTED_FONT_COLOR, strlen("info"), 32);
    gui_text_set(text_bt, "bt", FONT_NAME, UNSELECTED_FONT_COLOR, strlen("bt"), 32);
    gui_text_set(text_lcd, "lcd", FONT_NAME, UNSELECTED_FONT_COLOR, strlen("lcd"), 32);
    gui_text_set(text_app, "app", FONT_NAME, UNSELECTED_FONT_COLOR, strlen("app"), 32);
    gui_text_set(text_update, "update", FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen("update"), 32);
    //gui_exec(&(app->screen));
}
static void theme_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_setting();
    gui_tree_free(win);
    /*create theme page*/
    win = gui_win_create(&(app->screen), "win_set", 0, 0, 320, 320);
    gui_obj_add_event_cb(win, return_to_base_page_cb, GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE,  NULL);
    gui_iconlist_t *iconlist_0 = gui_iconlist_create(win, "iconlist_0", 0, 0, 0, 0, NULL, NULL);
    ICON_LIST_THEME_T iconlist_window1_theme = gui_iocnlist_get_theme(ICONLIST_PARENT_NAME);
    gui_icon_t *icon_list = gui_icon_create(iconlist_0, "icon_roulette", 0, 0, SETTING_ICON_WIDTH,
                                            SETTING_ICON_HIGHT, 0);
    gui_icon_t *icon_chess = gui_icon_create(iconlist_0, "icon_chess", 0, 0, SETTING_ICON_WIDTH,
                                             SETTING_ICON_HIGHT, 1);
    gui_icon_t *icon_roulette = gui_icon_create(iconlist_0, "icon_roulette", 0, 0, SETTING_ICON_WIDTH,
                                                SETTING_ICON_HIGHT, 2);
    gui_icon_t *icon_roulette2 = gui_icon_create(iconlist_0, "icon_roulette2", 0, 0, SETTING_ICON_WIDTH,
                                                 SETTING_ICON_HIGHT, 3);
    gui_icon_t *icon_styl4 = gui_icon_create(iconlist_0, "icon_styl4", 0, 0, SETTING_ICON_WIDTH,
                                             SETTING_ICON_HIGHT, 4);
    gui_icon_t *icon_style5 = gui_icon_create(iconlist_0, "icon_style5", 0, 0, SETTING_ICON_WIDTH,
                                              SETTING_ICON_HIGHT, 5);
    gui_obj_add_event_cb(icon_list, switch_to_verticallist_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    gui_obj_add_event_cb(icon_chess, switch_to_chessboard_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    gui_obj_add_event_cb(icon_roulette, switch_to_roulette_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    gui_obj_add_event_cb(icon_roulette2, switch_to_style4_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    gui_obj_add_event_cb(icon_styl4, switch_to_style5_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    gui_obj_add_event_cb(icon_style5, switch_to_style6_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    /*icon 1*/
    text_list = gui_text_create(icon_list, "text_roulette", 10, 20, 320, 40);
    if (iconlist_window1_theme == ICON_LIST_THEME_0)
    {
        image_list = gui_img_create_from_mem(icon_list, "setting", SELECTED_IMAGE, 200, 30, 0, 0);
        gui_text_set(text_list, THEME0_NAME, FONT_NAME, SELECTED_FONT_COLOR,
                     strlen(THEME0_NAME), 32);
    }
    else
    {
        image_list = gui_img_create_from_mem(icon_list, "icon", UNSELECTED_IMAGE, 200, 30, 0, 0);
        gui_text_set(text_list, THEME0_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                     strlen(THEME0_NAME), 32);
    }
    /*icon 2*/
    text_chess = gui_text_create(icon_chess, "text_chess", 10, 20, 320, 40);
    if (iconlist_window1_theme == ICON_LIST_THEME_1)
    {
        image_chess = gui_img_create_from_mem(icon_chess, "icon", SELECTED_IMAGE, 200, 30, 0, 0);
        gui_text_set(text_chess, THEME1_NAME, FONT_NAME, SELECTED_FONT_COLOR,
                     strlen(THEME1_NAME), 32);
    }
    else
    {
        image_chess = gui_img_create_from_mem(icon_chess, "icon", UNSELECTED_IMAGE, 200, 30, 0, 0);
        gui_text_set(text_chess, THEME1_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                     strlen(THEME1_NAME), 32);
    }
    /*icon 3*/
    text_roulette = gui_text_create(icon_roulette, "text_roulette", 10, 20, 320, 40);
    if (iconlist_window1_theme == ICON_LIST_THEME_2)
    {
        image_roulette = gui_img_create_from_mem(icon_roulette, "icon", SELECTED_IMAGE, 200, 30, 0, 0);
        gui_text_set(text_roulette, THEME2_NAME, FONT_NAME, SELECTED_FONT_COLOR,
                     strlen(THEME2_NAME), 32);
    }
    else
    {
        image_roulette = gui_img_create_from_mem(icon_roulette, "icon", UNSELECTED_IMAGE, 200, 30, 0,
                                                 0);
        gui_text_set(text_roulette, THEME2_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                     strlen(THEME2_NAME), 32);
    }
    /*icon 4*/
    text_roulette2 = gui_text_create(icon_roulette2, "text_roulette2", 10, 20, 320, 40);
    if (iconlist_window1_theme == ICON_LIST_THEME_3)
    {
        image_roulette2 = gui_img_create_from_mem(icon_roulette2, "icon", SELECTED_IMAGE, 200, 30, 0,
                                                  0);
        gui_text_set(text_roulette2, THEME3_NAME, FONT_NAME, SELECTED_FONT_COLOR,
                     strlen(THEME3_NAME), 32);
    }
    else
    {
        image_roulette2 = gui_img_create_from_mem(icon_roulette2, "icon", UNSELECTED_IMAGE, 200, 30, 0,
                                                  0);
        gui_text_set(text_roulette2, THEME3_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                     strlen(THEME3_NAME), 32);
    }
    /*icon 5*/
    text_style4 = gui_text_create(icon_styl4, "text_style4", 10, 20, 320, 40);
    if (iconlist_window1_theme == ICON_LIST_THEME_4)
    {
        image_style4 = gui_img_create_from_mem(icon_styl4, "icon", SELECTED_IMAGE, 200, 30, 0, 0);
        gui_text_set(text_style4, THEME4_NAME, FONT_NAME, SELECTED_FONT_COLOR,
                     strlen(THEME4_NAME), 32);
    }
    else
    {
        image_style4 = gui_img_create_from_mem(icon_styl4, "icon", UNSELECTED_IMAGE, 200, 30, 0, 0);
        gui_text_set(text_style4, THEME4_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                     strlen(THEME4_NAME), 32);
    }
    /*icon 6*/
    text_style5 = gui_text_create(icon_style5, "text_style5", 10, 20, 320, 40);
    if (iconlist_window1_theme == ICON_LIST_THEME_5)
    {
        image_style5 = gui_img_create_from_mem(icon_style5, "icon", SELECTED_IMAGE, 200, 30, 0, 0);
        gui_text_set(text_style5, THEME5_NAME, FONT_NAME, SELECTED_FONT_COLOR,
                     strlen(THEME5_NAME), 32);
    }
    else
    {
        image_style5 = gui_img_create_from_mem(icon_style5, "icon", UNSELECTED_IMAGE, 200, 30, 0, 0);
        gui_text_set(text_style5, THEME5_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                     strlen(THEME5_NAME), 32);
    }
    //gui_exec(&(app->screen));
}
static void info_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_setting();
    gui_tree_free(win);
    win = gui_win_create(&(app->screen), "win_set", 0, 0, 320, 320);
    gui_obj_add_event_cb(win, return_to_base_page_cb, GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE,  NULL);
    gui_iconlist_t *info_menu = gui_iconlist_create(win, "info_menu", 0, 0, 0, 0, NULL, NULL);
    gui_icon_t *version = gui_icon_create(info_menu, "version", 0, 0, SETTING_ICON_WIDTH,
                                          SETTING_ICON_HIGHT, 0);
    gui_icon_t *os = gui_icon_create(info_menu, "os", 0, 0, SETTING_ICON_WIDTH,
                                     SETTING_ICON_HIGHT, 1);
    gui_icon_t *lcd = gui_icon_create(info_menu, "lcd", 0, 0, SETTING_ICON_WIDTH,
                                      SETTING_ICON_HIGHT, 2);
    gui_icon_t *rom = gui_icon_create(info_menu, "rom", 0, 0, SETTING_ICON_WIDTH,
                                      SETTING_ICON_HIGHT, 3);
    gui_icon_t *ram = gui_icon_create(info_menu, "ram", 0, 0, SETTING_ICON_WIDTH,
                                      SETTING_ICON_HIGHT, 4);
    text_info_version = gui_text_create(version, "text_info_version", 10, 20, 320, 40);
    text_info_os = gui_text_create(os, "text_info_os", 10, 20, 320, 40);
    text_info_lcd = gui_text_create(lcd, "text_info_lcd", 10, 20, 320, 40);
    text_info_rom = gui_text_create(rom, "text_info_rom", 10, 20, 320, 40);
    text_info_ram = gui_text_create(ram, "text_info_ram", 10, 20, 320, 40);
    gui_text_set(text_info_version, "RTL8762D 90MHz",
                 FONT_NAME, UNSELECTED_FONT_COLOR, strlen("RTL8762DW 4.19"), 32);
    gui_text_set(text_info_os, "RT-THREAD RTK-GUI2.0",
                 FONT_NAME, UNSELECTED_FONT_COLOR, strlen("RT-THREAD RTK-GUI2.0"), 32);
    gui_text_set(text_info_lcd, "320*320 RGB565 8080",
                 FONT_NAME, UNSELECTED_FONT_COLOR, strlen("320*320 RGB565 8080"), 32);
    gui_text_set(text_info_rom, "256+512KB+32MB",
                 FONT_NAME, UNSELECTED_FONT_COLOR, strlen("RTL8762DW 4.19"), 32);
    gui_text_set(text_info_ram, "192KB",
                 FONT_NAME, UNSELECTED_FONT_COLOR, strlen("192KB"), 32);
    //gui_exec(&(app->screen));
}
typedef struct
{
    uint8_t      bd_addr[6];  /**< remote BD */
    uint8_t      bd_type;              /**< remote BD type*/
    const char   *name;
} T_DEV_INFO;
void bt_scan_timer_cb()
{
    if (gui_current_app() != get_app_setting())
    {
        return;
    }
    gui_app_t *app = get_app_setting();
    //extern void cmd_scan(void);
    //cmd_scan();
    //gui_timer_start(bt_scan_end_timer);
    GUI_ASSERT(NULL != NULL);
}

char text_box_string[6][30];
void bt_scan_end_timer_cb()
{

    if (gui_current_app() != get_app_setting())
    {
        return;
    }
    //extern size_t get_dev_list_count(void);
    //extern T_DEV_INFO *get_dev_list(void);
    //extern void cmd_stopscan(void);
    gui_app_t *app = get_app_setting();
    // cmd_stopscan();
    //size_t dev_list_count = get_dev_list_count();
    /*if (dev_list_count)
    {
        for (size_t i = 0; i < 6; i++)
        {
            bt_addr[i]->set(bt_addr[i], "scanning", FONT_NAME, UNSELECTED_FONT_COLOR,
                                 strlen("scanning"));
            bt_name[i]->set(bt_name[i], " ", FONT_NAME, UNSELECTED_FONT_COLOR, strlen(" "));
        }

        T_DEV_INFO *dev_list =  get_dev_list();
        for (size_t i = 0; i < dev_list_count; i++)
        {
            sprintf(text_box_string[i], "%02x:%02x:%02x:%02x:%02x:%02x",
                    dev_list[i].bd_addr[5], dev_list[i].bd_addr[4],
                    dev_list[i].bd_addr[3], dev_list[i].bd_addr[2],
                    dev_list[i].bd_addr[1], dev_list[i].bd_addr[0]
                   );
            bt_addr[i]->set(bt_addr[i], text_box_string[i], FONT_NAME, UNSELECTED_FONT_COLOR,
                                 strlen(text_box_string[i]));
            bt_name[i]->set(bt_name[i], dev_list[i].name, FONT_NAME, UNSELECTED_FONT_COLOR,
                                 strlen(dev_list[i].name));
        }
    }
    //gui_exec(&(app->screen));*/
}
static void bt_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_setting();
    gui_tree_free(win);
    win = gui_win_create(&(app->screen), "win_set", 0, 0, 320, 320);
    gui_obj_add_event_cb(win, bt_return_to_base_page_cb, GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE,  NULL);
    bt_menu = gui_iconlist_create(win, "bt_menu", 0, 0, 0, 0, NULL, NULL);
    for (size_t i = 0; i < 6; i++)
    {
        bt_icon[i] = gui_icon_create(bt_menu, "text", 0, 0, 0, 0, i);
        bt_addr[i] = gui_text_create(bt_icon[i], "text", 0, 0, 320, 40);
        gui_text_set(bt_addr[i], "scanning", FONT_NAME, UNSELECTED_FONT_COLOR,
                     strlen("scanning"), 32);
        bt_name[i] = gui_text_create(bt_icon[i], "text", 0, 37, 320, 40);
        gui_text_set(bt_name[i], " ", FONT_NAME, UNSELECTED_FONT_COLOR, strlen(" "), 32);
    }


    //gui_timer_start(bt_scan_timer);
    GUI_ASSERT(NULL != NULL);
    //gui_exec(&(app->screen));
}
static void lcd_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_setting();
    //gui_exec(&(app->screen));
}
static void apps_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_setting();
    //gui_exec(&(app->screen));
}
static void update_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_setting();
    //gui_exec(&(app->screen));
}
static void switch_to_verticallist_cb(void *obj, gui_event_t e)
{
    gui_log("switch_to_verticallist_cb!\n");
    gui_app_t *app = get_app_setting();
    gui_iocnlist_set_theme(ICONLIST_PARENT_NAME, ICON_LIST_THEME_0);
    gui_img_set_attribute(image_list, NULL, SELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_chess, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_roulette, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_roulette2, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_style4, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_style5, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_text_set(text_list, THEME0_NAME, FONT_NAME, SELECTED_FONT_COLOR,
                 strlen(THEME0_NAME), 32);
    gui_text_set(text_chess, THEME1_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME1_NAME), 32);
    gui_text_set(text_roulette, THEME2_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME2_NAME), 32);
    gui_text_set(text_roulette2, THEME3_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME3_NAME), 32);
    gui_text_set(text_style4, THEME4_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME4_NAME), 32);
    gui_text_set(text_style5, THEME5_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME5_NAME), 32);
    //gui_exec(&(app->screen));
}
static void switch_to_chessboard_cb(void *obj, gui_event_t e)
{
    gui_log("switch_to_chessboard_cb!\n");
    gui_app_t *app = get_app_setting();
    gui_iocnlist_set_theme(ICONLIST_PARENT_NAME, ICON_LIST_THEME_1);
    gui_img_set_attribute(image_list, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_chess, NULL, SELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_roulette, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_roulette2, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_style4, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_style5, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_text_set(text_list, THEME0_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME0_NAME), 32);
    gui_text_set(text_chess, THEME1_NAME, FONT_NAME, SELECTED_FONT_COLOR,
                 strlen(THEME1_NAME), 32);
    gui_text_set(text_roulette, THEME2_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME2_NAME), 32);
    gui_text_set(text_roulette2, THEME3_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME3_NAME), 32);
    gui_text_set(text_style4, THEME4_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME4_NAME), 32);
    gui_text_set(text_style5, THEME5_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME5_NAME), 32);
    //gui_exec(&(app->screen));
}
static void switch_to_roulette_cb(void *obj, gui_event_t e)
{
    gui_log("switch_to_roulette_cb!\n");
    gui_app_t *app = get_app_setting();
    gui_iocnlist_set_theme(ICONLIST_PARENT_NAME, ICON_LIST_THEME_2);
    gui_img_set_attribute(image_list, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_chess, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_roulette, NULL, SELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_roulette2, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_style4, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_style5, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_text_set(text_list, THEME0_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME0_NAME), 32);
    gui_text_set(text_chess, THEME1_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME1_NAME), 32);
    gui_text_set(text_roulette, THEME2_NAME, FONT_NAME, SELECTED_FONT_COLOR,
                 strlen(THEME2_NAME), 32);
    gui_text_set(text_roulette2, THEME3_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME3_NAME), 32);
    gui_text_set(text_style4, THEME4_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME4_NAME), 32);
    gui_text_set(text_style5, THEME5_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME5_NAME), 32);
    //gui_exec(&(app->screen));
}
static void switch_to_style4_cb(void *obj, gui_event_t e)
{
    gui_log("switch_to_roulette2_cb!\n");
    gui_app_t *app = get_app_setting();
    gui_iocnlist_set_theme(ICONLIST_PARENT_NAME, ICON_LIST_THEME_3);
    gui_img_set_attribute(image_list, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_chess, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_roulette, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_roulette2, NULL, SELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_style4, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_img_set_attribute(image_style5, NULL, UNSELECTED_IMAGE, 200, 30);
    gui_text_set(text_list, THEME0_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME0_NAME), 32);
    gui_text_set(text_chess, THEME1_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME1_NAME), 32);
    gui_text_set(text_roulette, THEME2_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME2_NAME), 32);
    gui_text_set(text_roulette2, THEME3_NAME, FONT_NAME, SELECTED_FONT_COLOR,
                 strlen(THEME3_NAME), 32);
    gui_text_set(text_style4, THEME4_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME4_NAME), 32);
    gui_text_set(text_style5, THEME5_NAME, FONT_NAME, UNSELECTED_FONT_COLOR,
                 strlen(THEME5_NAME), 32);
    //gui_exec(&(app->screen));
}
static void switch_to_style5_cb(void *obj, gui_event_t e)
{
    return;
}
static void switch_to_style6_cb(void *obj, gui_event_t e)
{
    return;
}
static void win_base_page_cb(void *obj, gui_event_t e)
{
    extern void *get_launcher_app(void);
    gui_app_t *app = get_app_setting();
    gui_switch_app(app, get_launcher_app());
}
static void return_to_base_page_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_setting();
    gui_tree_free(win);
    create_base_page();
}
static void bt_return_to_base_page_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_setting();
    //gui_timer_stop(bt_scan_timer);
    GUI_ASSERT(NULL != NULL);
    //gui_timer_stop(bt_scan_end_timer);
    GUI_ASSERT(NULL != NULL);
    gui_tree_free(win);
    create_base_page();
}
static void app_setting_ui_design(gui_app_t *app)
{
    create_base_page();
}

