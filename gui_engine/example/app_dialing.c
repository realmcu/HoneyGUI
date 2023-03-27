#include "app_dialing.h"
#include "guidef.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_tabview.h"
#include "gui_iconlist.h"
#include "gui_curtain.h"
#include "gui_seekbar.h"
#include "gui_text.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "rtk_gui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"

bool touch_flag;
gui_tabview_t *app_dialing_window;
char *string_num;
uint16_t string_lenght;
gui_text_t *text_num_1;
gui_seekbar_t *seekbar;
void *app_dialing_tid;
gui_tab_t *tab0;
int32_t num_length;
int32_t num_disp_start_position = 310;
int lastN;

static void app_dialing_ui_design(gui_app_t *app);


static gui_app_t app_dialing =
{
    .screen =
    {
        .name = "app_launcher",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_dialing_ui_design,
};

gui_app_t *get_app_dialing(void)
{
    return &app_dialing;
}

char string_source[30];
char string_dest[2];
char string_display[18];



void strcpylastN(char *dest, char *src, int n)
{
    if (n > strlen(src))
    {
        strcpy(dest, src);
        return;
    }
    for (int i = 0; i < n; i++)
    {
        dest[i] = src[i + (strlen(src) - n)];
    }
    dest[n] = '\0';
}
void number_display_handle(void)
{
    gui_app_t *app = get_app_dialing();
    strcat(string_source, string_dest);
    strcpylastN(string_display, string_source, lastN);
    gui_log("%s  string_source = %s, string_dest = %s, string_display = %s\n", string_dest,
            string_source, string_dest, string_display);
    gui_log("icon%s_link_cb: strlen(string_source) = %d, strlen(string_dest) = %d, strlen(string_display) = %d\n",
            string_dest, strlen(string_source), strlen(string_dest), strlen(string_display));
    gui_tree_free(text_num_1);
    if ((num_disp_start_position - num_length) > 38)
    {
        text_num_1 = gui_text_create(tab0, "image_num_1",
                                     num_disp_start_position - num_length, 8,
                                     num_length, 32);
    }
    else
    {
        text_num_1 = gui_text_create(tab0, "image_num_1", 38, 8, 272, 32);
    }
    gui_text_set(text_num_1, (char *)string_display, "rtk_font_fs32", 0xffff,
                 strlen(string_display), 32);
    gui_app_exec(app);
}
static void icon1_link_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_dialing();
    strcpy(string_dest, "1");
    num_length += 16;
    gui_log("1  num_length = %d\n", num_length);
    gui_log("1  string_source = %s, string_dest = %s\n", string_source, string_dest);
    number_display_handle();
}
static void icon2_link_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_dialing();
    strcpy(string_dest, "2");
    num_length += 16;
    gui_log("2  num_length = %d\n", num_length);
    gui_log("2  string_dest = %s, string_source = %s \n", string_dest, string_source);
    number_display_handle();
}
static void icon3_link_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_dialing();
    strcpy(string_dest, "3");
    num_length += 16;
    gui_log("3  num_length = %d\n", num_length);
    number_display_handle();
}
static void icon4_link_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_dialing();
    strcpy(string_dest, "4");
    num_length += 16;
    gui_log("4  num_length = %d\n", num_length);
    number_display_handle();
}
static void icon5_link_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_dialing();
    strcpy(string_dest, "5");
    num_length += 16;
    gui_log("5  num_length = %d\n", num_length);
    number_display_handle();
}
static void icon6_link_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_dialing();
    strcpy(string_dest, "6");
    num_length += 16;
    gui_log("6  num_length = %d\n", num_length);
    number_display_handle();
}
static void icon7_link_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_dialing();
    strcpy(string_dest, "7");
    num_length += 16;
    gui_log("7  num_length = %d\n", num_length);
    number_display_handle();
}
static void icon8_link_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_dialing();
    strcpy(string_dest, "8");
    num_length += 16;
    gui_log("8  num_length = %d\n", num_length);
    number_display_handle();
}
static void icon9_link_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_dialing();
    strcpy(string_dest, "9");
    num_length += 16;
    gui_log("9  num_length = %d\n", num_length);
    number_display_handle();
}
static void icon0_link_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_dialing();
    strcpy(string_dest, "0");
    num_length += 16;
    gui_log("0  num_length = %d\n", num_length);
    number_display_handle();
}
static void icon_star_link_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_dialing();
    strcpy(string_dest, "*");
    num_length += 16;
    gui_log("*  num_length = %d\n", num_length);
    number_display_handle();
}
static void icon_sharp_link_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_dialing();
    strcpy(string_dest, "#");
    num_length += 16;
    gui_log("#  num_length = %d\n", num_length);
    number_display_handle();
}
static void icon_delete_link_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_dialing();
    string_source[strlen(string_source) - 1] = 0;  // delete the last number of string
    num_length -= 16;
    gui_log("D  num_length = %d\n", num_length);
    if (num_length <= 0)
    {
        num_length = 0;
    }
    gui_tree_free(text_num_1);
    strcpylastN(string_display, string_source, lastN);
    if ((num_disp_start_position - num_length) > 38)
    {
        text_num_1 = gui_text_create(tab0, "image_num_1",
                                     num_disp_start_position - num_length, 8,
                                     num_length, 32);
    }
    else
    {
        text_num_1 = gui_text_create(tab0, "image_num_1", 38, 8, 272, 32);
    }
    gui_text_set(text_num_1, (char *)string_display, "rtk_font_fs32", 0xffff,
                 strlen(string_display), 32);
    gui_app_exec(app);
}
static void icon_hang_up_link_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_dialing();
    memset(string_source, 0, strlen(string_source));
    memset(string_dest, 0, strlen(string_dest));
    memset(string_display, 0, strlen(string_display));
    num_length = 0;
    extern void *get_launcher_app(void);
    gui_switch_app(app, get_launcher_app());
}
void slider_cb(void *obj, gui_event_t e)
{
    static char string[10];
    sprintf(string, "%d", seekbar->base.get_progress(&(seekbar->base)));
    gui_text_set(text_num_1, (char *)string, "rtk_font_fs32", 0xffff,
                 strlen(string), 32);
}
static void icon_dialing_link_cb(void *obj, gui_event_t e)
{
    gui_log("Open call_interface\n");
    gui_app_t *app = get_app_dialing();
    gui_tree_free(tab0);
    gui_tab_t *tab1 = gui_tab_create(app_dialing_window, "tab1", 0, 0, 0, 0, 0, 0);
    gui_icon_t *icon_call_interface = gui_icon_create(tab1, "call_interface", 120, 180, 100, 50, 0);
    gui_img_t *image_call_interface = gui_img_create_from_mem(icon_call_interface, "call",
                                                              CALL_INTERFACE_BIN, -120, -180, 0, 0);
    text_num_1 = gui_text_create(tab1, "image_num_1",
                                 ((int)gui_get_screen_width() - 16 * strlen(string_display)) / 2, 80,
                                 300, 32);
    gui_text_set(text_num_1, (char *)string_display, "rtk_font_fs32", 0xffff,
                 strlen(string_display), 32);
    gui_obj_add_event_cb(icon_call_interface, icon_hang_up_link_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    seekbar = gui_seekbar_create(tab1, "v", 20, 160, 200, 20);
    seekbar->slider_cb = slider_cb;

    gui_app_exec(app);
}

static void app_dialing_ui_design(gui_app_t *app)
{
    gui_obj_t *screen = &app->screen;
    //mem_alloca();
    /*  Dialing keyboard page  */
    app_dialing_window = gui_tabview_create(screen, "dialing_win", 0, 0, 0, 0);
    tab0 = gui_tab_create(app_dialing_window, "tab0", 0, 0, 0, 0, 0, 0);
    gui_img_t *image_screen_dispaly = gui_img_create_from_mem(tab0, "screen", SCREEN_DISPALY_BIN,
                                                              0,
                                                              0, 0,
                                                              0);
    gui_img_t *image_horizontal_bar_0 = gui_img_create_from_mem(tab0, "horizontal",
                                                                HORIZONTAL_BAR_BIN, 0, 50,
                                                                0, 0);
    gui_img_t *image_vertical_bar_0 = gui_img_create_from_mem(tab0, "vertical0", VERTICAL_BAR_BIN,
                                                              100, 50, 0, 0);
    gui_img_t *image_vertical_bar_1 = gui_img_create_from_mem(tab0, "vertical1", VERTICAL_BAR_BIN,
                                                              210, 50, 0, 0);
    gui_icon_t *icon1 = gui_icon_create(tab0, "icon1", 0, 54, 100, 50, 0);
    gui_img_t *image_num_1 = gui_img_create_from_mem(icon1, "image_num_1", NUMBER_1_BIN, 0, 0, 0, 0);
    text_num_1 = gui_text_create(tab0, "image_num_1", num_disp_start_position, 8, 16,
                                 32);
    strcpy(string_dest, "");
    gui_text_set(text_num_1, string_dest, "rtk_font_fs32", 0xffff, strlen(string_dest), 32);
    gui_obj_add_event_cb(icon1, icon1_link_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    touch_flag = true;
    gui_icon_t *icon2 = gui_icon_create(tab0, "icon2", 110, 54, 100, 50, 0);
    gui_img_t *image_num_2 = gui_img_create_from_mem(icon2, "image_num_2", NUMBER_2_BIN, 0, 0, 0, 0);
    gui_obj_add_event_cb(icon2, icon2_link_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_icon_t *icon3 = gui_icon_create(tab0, "icon3", 220, 54, 100, 50, 0);
    gui_img_t *image_num_3 = gui_img_create_from_mem(icon3, "image_num_3", NUMBER_3_BIN, 0, 0, 0, 0);
    gui_obj_add_event_cb(icon3, icon3_link_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_t *image_horizontal_bar_1 = gui_img_create_from_mem(tab0, "h_bar_1",
                                                                HORIZONTAL_BAR_BIN,
                                                                0, 104, 0,
                                                                0);
    gui_icon_t *icon4 = gui_icon_create(tab0, "icon4", 0, 108, 100, 50, 0);
    gui_img_t *image_num_4 = gui_img_create_from_mem(icon4, "image_num_4", NUMBER_4_BIN, 0, 0, 0, 0);
    gui_obj_add_event_cb(icon4, icon4_link_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_icon_t *icon5 = gui_icon_create(tab0, "icon5", 110, 108, 100, 50, 0);
    gui_img_t *image_num_5 = gui_img_create_from_mem(icon5, "image_num_5", NUMBER_5_BIN, 0, 0, 0, 0);
    gui_obj_add_event_cb(icon5, icon5_link_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_icon_t *icon6 = gui_icon_create(tab0, "icon6", 220, 108, 100, 50, 0);
    gui_img_t *image_num_6 = gui_img_create_from_mem(icon6, "image_num_6", NUMBER_6_BIN, 0, 0, 0, 0);
    gui_obj_add_event_cb(icon6, icon6_link_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_t *image_horizontal_bar_2 = gui_img_create_from_mem(tab0, "h_bar_2",
                                                                HORIZONTAL_BAR_BIN,
                                                                0, 158, 0,
                                                                0);
    gui_icon_t *icon7 = gui_icon_create(tab0, "icon7", 0, 162, 100, 50, 0);
    gui_img_t *image_num_7 = gui_img_create_from_mem(icon7, "image_num_7", NUMBER_7_BIN, 0, 0, 0, 0);
    gui_obj_add_event_cb(icon7, icon7_link_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_icon_t *icon8 = gui_icon_create(tab0, "icon8", 110, 162, 100, 50, 0);
    gui_img_t *image_num_8 = gui_img_create_from_mem(icon8, "image_num_8", NUMBER_8_BIN, 0, 0, 0, 0);
    gui_obj_add_event_cb(icon8, icon8_link_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_icon_t *icon9 = gui_icon_create(tab0, "icon9", 220, 162, 100, 50, 0);
    gui_img_t *image_num_9 = gui_img_create_from_mem(icon9, "image_num_9", NUMBER_9_BIN, 0, 0, 0, 0);
    gui_obj_add_event_cb(icon9, icon9_link_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_t *image_horizontal_bar_3 = gui_img_create_from_mem(tab0, "h_bar_3",
                                                                HORIZONTAL_BAR_BIN,
                                                                0, 212, 0,
                                                                0);
    gui_icon_t *icon_star = gui_icon_create(tab0, "icon_star", 0, 216, 100, 50, 0);
    gui_img_t *image_num_star = gui_img_create_from_mem(icon_star, "star", NUMBER_STAR_BIN, 0, 0, 0,
                                                        0);
    gui_obj_add_event_cb(icon_star, icon_star_link_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_icon_t *icon0 = gui_icon_create(tab0, "icon0", 110, 216, 100, 50, 0);
    gui_img_t *image_num_0 = gui_img_create_from_mem(icon0, "0", NUMBER_0_BIN, 0, 0, 0, 0);
    gui_obj_add_event_cb(icon0, icon0_link_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_icon_t *icon_sharp = gui_icon_create(tab0, "icon_sharp", 220, 216, 100, 50, 0);
    gui_img_t *image_num_sharp = gui_img_create_from_mem(icon_sharp, "sharp", NUMBER_POUND_BIN, 0, 0,
                                                         0, 0);
    gui_obj_add_event_cb(icon_sharp, icon_sharp_link_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_t *image_horizontal_bar_4 = gui_img_create_from_mem(tab0, "h_bar_4",
                                                                HORIZONTAL_BAR_BIN,
                                                                0, 266, 0,
                                                                0);
    gui_icon_t *icon_dialing = gui_icon_create(tab0, "icon_dialing", 0, 270, 100, 50, 0);
    gui_img_t *image_dialing = gui_img_create_from_mem(icon_dialing, "dialing", DIALING_BIN, 0, 0, 0,
                                                       0);
    gui_obj_add_event_cb(icon_dialing, icon_dialing_link_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_t *image_contacts = gui_img_create_from_mem(tab0, "contacts", CONTACTS_BIN, 110, 270,
                                                        0,
                                                        0);
    gui_icon_t *icon_delete = gui_icon_create(tab0, "icon_delete", 220, 270, 100, 50, 0);
    gui_img_t *image_delete = gui_img_create_from_mem(icon_delete, "delete", DELETE_BIN, 0, 0, 0, 0);
    gui_obj_add_event_cb(icon_delete, icon_delete_link_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
}

