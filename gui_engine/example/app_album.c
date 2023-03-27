#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_curtain.h>
#include <gui_iconlist.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include "rtk_gui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
static void app_album_ui_design(gui_app_t *app);
static gui_app_t app_album =
{
    .screen =
    {
        .name = "app_album",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_album_ui_design,
};

void *get_app_album(void)
{
    return &app_album;
}


static void win_exit_cb(void *obj, gui_event_t e)
{
    extern void *get_launcher_app(void);
    gui_switch_app(get_app_album(), get_launcher_app());
}

static void alien_center(gui_img_t *image)
{
    GET_BASE(image)->x = (gui_get_screen_width() - gui_img_get_width(image)) / 2;
    GET_BASE(image)->y = (gui_get_screen_height() - gui_img_get_height(image)) / 2;
}
static void app_album_ui_design(gui_app_t *app)
{

    gui_win_t *win = gui_win_create(&(app->screen), "win", 0, 0, 320, 320);
    gui_obj_add_event_cb(win, win_exit_cb, GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE, NULL);
    gui_tabview_t *tv = gui_tabview_create(win, "tabview", 0, 0, 0, 0);
    gui_tab_t *tb0 = gui_tab_create(tv, "tb0", 0, 0, 0, 0, 0, 0);
    gui_tab_t *tb1 = gui_tab_create(tv, "tb1", 0, 0, 0, 0, 1, 0);
    gui_tab_t *tb2 = gui_tab_create(tv, "tb1", 0, 0, 0, 0, 2, 0);
    gui_img_t *image_0;
    gui_img_t *image_1;
    image_0 = gui_img_create_from_mem(tb0, "sun", TIGER_BIN, 0, 0, 0, 0);
    alien_center(image_0);
    image_0 = gui_img_create_from_mem(tb2, "sun", TIGER_BIN, 0, 0, 0, 0);
    alien_center(image_0);
    image_1 = gui_img_create_from_mem(tb1, "sun", SUN_BIN, 0, 0, 0, 0);
    alien_center(image_1);

}
