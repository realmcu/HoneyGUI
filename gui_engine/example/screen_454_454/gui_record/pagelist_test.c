#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_img.h"
#include "gui_page.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_win.h"
#include "gui_tabview.h"
#include "gui_scroll_text.h"
#include "gui_pagelistview.h"
#include "gui_pagelist.h"

#define MAX_FILE_LIST_NUM 40
#define MAX_SHOW_FILE_LIST_NUM 4
gui_pagelistview_t *pagelistview_test = NULL;
gui_pagelist_t *pagelist_test = NULL;
gui_switch_t *switch_pagelist_test[MAX_SHOW_FILE_LIST_NUM] = {NULL};
gui_scroll_text_t *scroll_text_pagelist_test[MAX_SHOW_FILE_LIST_NUM] = {NULL};
char *files_name_test[MAX_FILE_LIST_NUM] = {"录音文件1", "录音文件2", "录音文件3", "录音文件4", "录音文件5",
                                            "录音文件6", "录音文件7", "录音文件8", "录音文件9", "录音文件10",
                                            "录音文件11", "录音文件12", "录音文件13", "录音文件14", "录音文件15",
                                            "录音文件16", "录音文件17", "录音文件18", "录音文件19", "录音文件20",
                                            "录音文件21", "录音文件22", "录音文件23", "录音文件24", "录音文件25",
                                            "录音文件26", "录音文件27", "录音文件28", "录音文件29", "录音文件30",
                                            "录音文件31", "录音文件32", "录音文件33", "录音文件34", "录音文件35",
                                            "录音文件36", "录音文件37", "录音文件38", "录音文件39", "录音文件40"
                                           };
int8_t show_num = 4;
int16_t real_num = 20;
int16_t list_gap_y = 20;
int16_t index_first = 0;
int16_t index_last = 0;

static void pagelist_test_update_list_first_cb(gui_pagelist_t *this, gui_switch_t *list_first)
{
    gui_log("pagelist_test_update_list_first_cb, list_first = 0x%x\n", (uint32_t *)list_first);
    gui_scroll_text_t *list_scroll_text = NULL;

    gui_list_t *node = NULL;
    gui_list_for_each(node, &list_first->base.child_list)
    {
        gui_obj_t *find_obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (find_obj->type == SCROLLTEXTBOX)
        {
            list_scroll_text = (gui_scroll_text_t *)find_obj;
            break;
        }
    }
    index_last ++;
    if (index_last > MAX_FILE_LIST_NUM - 1)
    {
        index_last = MAX_FILE_LIST_NUM - 1;
    }
    index_first ++;
    if (index_first > MAX_FILE_LIST_NUM  - MAX_SHOW_FILE_LIST_NUM)
    {
        index_first = MAX_FILE_LIST_NUM - MAX_SHOW_FILE_LIST_NUM ;
    }
    gui_scroll_text_set(list_scroll_text, files_name_test[index_last],
                        "rtk_font_mem", gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX), strlen(files_name_test[index_last]),
                        FONT_H_32);
}

static void pagelist_test_update_list_last_cb(gui_pagelist_t *obj, gui_switch_t *list_last)
{
    gui_log("pagelist_test_update_list_last_cb, list_first = 0x%x\n", (uint32_t *)list_last);
    gui_scroll_text_t *list_scroll_text = NULL;

    gui_list_t *node = NULL;
    gui_list_for_each(node, &list_last->base.child_list)
    {
        gui_obj_t *find_obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (find_obj->type == SCROLLTEXTBOX)
        {
            list_scroll_text = (gui_scroll_text_t *)find_obj;
            break;
        }
    }
    index_first --;
    if (index_first < 0)
    {
        index_first = 0;
    }
    index_last --;
    if (index_last < MAX_SHOW_FILE_LIST_NUM - 1)
    {
        index_last = MAX_SHOW_FILE_LIST_NUM - 1;
    }
    gui_scroll_text_set(list_scroll_text, files_name_test[index_first],
                        "rtk_font_mem", gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX), strlen(files_name_test[index_first]),
                        FONT_H_32);
}

static void win_increase_touch_cb(void *obj, gui_event_t e)
{
    gui_log("win_increase_touch_cb\n");
    gui_pagelist_increase_real_list_max(pagelist_test, 1);
}

static void win_decrease_touch_cb(void *obj, gui_event_t e)
{
    gui_log("win_decrease_touch_cb\n");
    gui_pagelist_decrease_real_list_max(pagelist_test, 1);
}

static void swtich_pagelist_touch_cb(void *obj, gui_event_t e)
{
    gui_switch_t *this = (gui_switch_t *)obj;
    gui_log("swtich_pagelist_touch_cb, y = %d, h = %d\n", this->base.y, this->base.h);
    if (this->base.y + this->base.h > pagelist_test->show_border_top &&
        this->base.y < pagelist_test->show_border_bottom)
    {
        gui_log("swtich_pagelist_touch_cb valid\n");
    }

}
void design_pagelist_test(void *parent)
{
    gui_win_t *win = gui_win_create(parent, "win_pagelist_test", 0, 0, LCD_W, LCD_H);


    //mask should be the brother of pagelistview
    //mask image rgb should be larger than 00 00 00
    pagelistview_test = gui_pagelistview_create(win, "pagelistview", 0, 0, LCD_W, LCD_H);
    gui_img_t *img_top_mask = gui_img_create_from_mem(win, "top_mask", WATCH_BASE_MASK_BLACK_BIN, 0, 0,
                                                      LCD_W, 100);
    gui_img_t *img_bottom_mask = gui_img_create_from_mem(win, "bottom_mask", WATCH_BASE_MASK_BLACK_BIN,
                                                         0, 354, LCD_W, 100);

    //test for increse\decrese file lists
    gui_win_t *win_increase = gui_win_create(img_top_mask, "win_increase", 0, 0, 32, 32);
    gui_win_t *win_decrease = gui_win_create(img_top_mask, "win_decrease", 400, 0, 32, 32);
    gui_text_t *text_increase = gui_text_create(win_increase, "text_increase", 0, 0, 32, 32);
    gui_text_set(text_increase, "加", "rtk_font_mem", gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX), 32,
                 32);
    gui_text_t *text_decrease = gui_text_create(win_decrease, "text_decrease", 0, 0, 32, 32);
    gui_text_set(text_decrease, "减", "rtk_font_mem", gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX), 32,
                 32);
    gui_obj_add_event_cb(win_increase, (gui_event_cb_t)win_increase_touch_cb, GUI_EVENT_TOUCH_RELEASED,
                         NULL);
    gui_obj_add_event_cb(win_decrease, (gui_event_cb_t)win_decrease_touch_cb, GUI_EVENT_TOUCH_RELEASED,
                         NULL);

    gui_pagelistview_add_top_mask(pagelistview_test, img_top_mask);
    gui_pagelistview_add_bottom_mask(pagelistview_test, img_bottom_mask);

    pagelist_test = gui_pagelist_create(pagelistview_test, "pagelist", 0, 0, LCD_W, LCD_H);
    for (int8_t i = 0; i < show_num; i++)
    {
        switch_pagelist_test[i] = gui_switch_create(pagelist_test, 83, 111 + i * (list_gap_y + 64), 288, 64,
                                                    ICON_TEXT_BASE_DARK_BIN, ICON_TEXT_BASE_DARK_BIN);
        switch_pagelist_test[i]->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        switch_pagelist_test[i]->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        scroll_text_pagelist_test[i] = gui_scroll_text_create(switch_pagelist_test[i],
                                                              "scroll_text_record_files", 0, 0, 128, FONT_H_32);
        gui_scroll_text_set(scroll_text_pagelist_test[i], files_name_test[i],
                            "rtk_font_mem", gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX), strlen(files_name_test[i]), FONT_H_32);
        gui_scroll_text_scroll_set(scroll_text_pagelist_test[i], SCROLL_X, 0, 0, 5000, 0);
        gui_obj_add_event_cb(switch_pagelist_test[i], (gui_event_cb_t)swtich_pagelist_touch_cb, GUI_EVENT_1,
                             NULL);
        gui_obj_add_event_cb(switch_pagelist_test[i], (gui_event_cb_t)swtich_pagelist_touch_cb, GUI_EVENT_2,
                             NULL);
    }
    index_first = 0;
    index_last = MAX_SHOW_FILE_LIST_NUM - 1;
    gui_pagelist_set_att(pagelist_test, real_num, show_num, list_gap_y, switch_pagelist_test[0],
                         switch_pagelist_test[MAX_SHOW_FILE_LIST_NUM - 1]);
    //pagelist_test->show_border_bottom = 354;
    //pagelist_test->show_border_top = 100;
    gui_pagelist_add_list_update_cb(pagelist_test,
                                    (gui_pagelist_update_cb_t)pagelist_test_update_list_first_cb,
                                    (gui_pagelist_update_cb_t)pagelist_test_update_list_last_cb);
}

gui_switch_t *switch_test[4];
gui_win_t *win_test[4];
gui_img_t *img_test[4];
gui_img_t *img_testtest;
void design_list_test(void *parent)
{
    gui_win_t *win1 = gui_win_create(parent, "win1", 0, 0, LCD_W, LCD_H);
    gui_win_t *win2 = gui_win_create(parent, "win1", 0, 0, LCD_W, LCD_H);
    gui_win_t *win3 = gui_win_create(parent, "win1", 0, 0, LCD_W, LCD_H);
    gui_win_t *win4 = gui_win_create(parent, "win1", 0, 0, LCD_W, LCD_H);
    for (uint8_t i = 0; i < 4; i ++)
    {
        switch_test[i] = gui_switch_create(win1, 0, 0, LCD_W, LCD_H, NULL, NULL);
        win_test[i] = gui_win_create(win2, "win_test", 0, 0, LCD_W, LCD_H);
        img_test[i] = gui_img_create_from_mem(win3, "img_test", NULL, 0, 0, LCD_W, LCD_H);
    }
    img_testtest = gui_img_create_from_mem(win4, "img_test", NULL, 0, 0, LCD_W, LCD_H);
    gui_log("done\n");
}