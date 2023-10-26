#include <gui_tabview.h>
#include <gui_obj.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "draw_font.h"
#include <gui_magic_img.h>
#include <gui_win.h>

extern void exit_homelist(void *obj, gui_event_t e);
extern gui_tabview_t *homelist_tab;

uint8_t HOME_ACOUNT = 8;

#define HOMELIST_X 0
bool move_flag = true;
void move_homelist(void)
{
#ifndef RTL87x2G
    gui_thread_mdelay(1);
    gui_log("homelist_tab->base.x is %d \n", homelist_tab->base.x);
#endif // !RTL87x2G
    if (move_flag)
    {
        homelist_tab->base.x --;
        if (homelist_tab->base.x <= - 210 * 7 + 55)
        {
            move_flag = false;
        }
    }
    else
    {
        homelist_tab->base.x ++;
        if (homelist_tab->base.x >= 210 - 55)
        {
            move_flag = true;
        }
    }
}
void design_homelist_0(void *parent)
{
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img",
                                                                 HOMELIST_COLORFUL_BIN,
                                                                 HOMELIST_X, 0, 0, 0);
    gui_magicimage_set_animate(tablist_img, 100000, 0xffffffff, move_homelist, NULL);
    gui_win_t *win = gui_win_create(parent, "win", ((gui_tab_t *)parent)->base.x,
                                    ((gui_tab_t *)parent)->base.y, ((gui_tab_t *)parent)->base.w * HOME_ACOUNT,
                                    ((gui_tab_t *)parent)->base.h);
    gui_obj_add_event_cb(win, (gui_event_cb_t)exit_homelist, GUI_EVENT_TOUCH_CLICKED, NULL);
}
void design_homelist_1(void *parent)
{
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img",
                                                                 HOMELIST_DOG_BIN,
                                                                 HOMELIST_X, 0, 0, 0);
}
void design_homelist_2(void *parent)
{
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img",
                                                                 HOMELIST_LINE_BLACK_BIN,
                                                                 HOMELIST_X, 0, 0, 0);
}
void design_homelist_3(void *parent)
{
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img",
                                                                 HOMELIST_LINE_ORANGE_BIN,
                                                                 HOMELIST_X, 0, 0, 0);
}
void design_homelist_4(void *parent)
{
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img",
                                                                 HOMELIST_NUMBER_BIN,
                                                                 HOMELIST_X, 0, 0, 0);
}
void design_homelist_5(void *parent)
{
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img",
                                                                 HOMELIST_WATCH_BLACK_BIN,
                                                                 HOMELIST_X, 0, 0, 0);
}
void design_homelist_6(void *parent)
{
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img",
                                                                 HOMELIST_WATCH_WHITE_BIN,
                                                                 HOMELIST_X, 0, 0, 0);
}
void design_homelist_7(void *parent)
{
    gui_magic_img_t *tablist_img = gui_magic_img_create_from_mem(parent, "tablist_img",
                                                                 HOMELIST_ADD_BIN,
                                                                 HOMELIST_X, 0, 0, 0);
}
