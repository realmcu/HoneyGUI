#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_img.h"
#include "gui_page.h"
#include "gui_grid.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_win.h"
#include "gui_tabview.h"
#include "gui_scroll_text.h"

#define MAX_FILES_NUM        20

gui_curtain_t *curtain_down_record = NULL;
gui_img_t *img_base_record_files = NULL;
gui_text_t *text_title_record_files = NULL;
gui_switch_t *switch_record_files_delete = NULL;
gui_page_t *page_record_files = NULL;
gui_grid_t *grid_record_files = NULL;
gui_switch_t *switch_record_files[MAX_FILES_NUM] = {NULL};
gui_scroll_text_t *scroll_text_record_files[MAX_FILES_NUM] = {NULL};

//example for add record files
gui_win_t *win_add_record_files = NULL;
char *record_files_name[MAX_FILES_NUM] = {"录音文件2023/11/16", "录音文件2023/11/17", "录音文件2023/11/18", "录音文件2023/11/19", "录音文件2023/11/20",
                                          "录音文件2023/11/21", "录音文件2023/11/22", "录音文件2023/11/23", "录音文件2023/11/24", "录音文件2023/11/25",
                                          "录音文件2023/11/26", "录音文件2023/11/27", "录音文件2023/11/28", "录音文件2023/11/29", "录音文件2023/11/30",
                                          "录音文件2023/11/31", "录音文件2023/12/1", "录音文件2023/12/2", "录音文件2023/12/3", "录音文件2023/12/4"
                                         };

static uint8_t record_file_num = 0;
char *txet_record_delete_file = "确认删除录音？";

//if current view has a "delete icon", need to set delete info before create a win_delete
static void switch_record_file_delete_yes_action(void *obj)
{
    gui_log("switch_record_file_delete_yes_action, obj = 0x%x\n", (uint32_t *)obj);
    //delete selected record file
}
static void switch_record_file_delete_no_action(void *obj)
{
    gui_log("switch_record_file_delete_no_action, obj = 0x%x\n", (uint32_t *)obj);
    //do nothing
}
//static void switch_record_file_delete_text(void *obj)
//{
//    gui_log("switch_record_file_delete_text, obj = 0x%x\n", obj);
//    // set delete text
//}

static void switch_record_files_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_record_files_touch_cb, obj = 0x%x\n", (uint32_t *)obj);

    gui_switch_t *this = (gui_switch_t *)obj;
    //for delete and record play windows creation
    gui_app_t *app = get_app_watch_ui();
    extern gui_win_t *win_record_play;
    extern gui_win_t *win_confirm;
    extern gui_text_t *text_confirm;

    gui_log("switch_record_files_touch_cb win_record_play = 0x%x\n", (uint32_t *)win_record_play);
    switch (event)
    {

    case GUI_EVENT_1:
    case GUI_EVENT_2:
        // jump to win_record_play
        //create record play window
        gui_log("GUI_EVENT_1 GUI_EVENT_2 obj = 0x%x\n", (uint32_t *)obj);
        if (win_record_play != NULL)
        {
            gui_tree_free(win_record_play);
            win_record_play = NULL;
        }
        win_record_play = gui_win_create(&(app->screen), "win_record_play", 0, 0, LCD_W, LCD_H);
        extern void design_win_record_play(gui_win_t *parent, gui_switch_t *selected_record_file);
        design_win_record_play(win_record_play, this);

        push_current_widget(tabview_main);
        gui_obj_show(tabview_main, false);
        //tabview_main->base.not_show = true;

        break;

    case GUI_EVENT_TOUCH_LONG:
        // delete
        //change color
        gui_log("GUI_EVENT_TOUCH_LONG obj = 0x%x\n", (uint32_t *)obj);
        switch_record_files_delete->base.not_show = !switch_record_files_delete->base.not_show;
        for (int j = 0; j < record_file_num; j ++)
        {
            if (this == switch_record_files[j])
            {
                continue;
            }
            if (switch_record_files_delete->base.not_show)
            {
                switch_record_files[j]->base.not_show = false;
            }
            else
            {
                switch_record_files[j]->base.not_show = true;
            }
        }

        //create delete window, set delete action
        if (!switch_record_files_delete->base.not_show)
        {
            if (win_confirm != NULL)
            {
                gui_tree_free(win_confirm);
                win_confirm = NULL;
                set_confirm_yes(NULL, NULL);
                set_confirm_no(NULL, NULL);
            }
            win_confirm = gui_win_create(&(app->screen), "win_confirm", 0, 0, LCD_W, LCD_H);
            set_confirm_yes(switch_record_file_delete_yes_action, this);
            set_confirm_no(switch_record_file_delete_no_action, this);
            set_confirm_text(txet_record_delete_file, 123, 131, 7);

            void design_win_confirm(void *parent);
            design_win_confirm(win_confirm);
            gui_obj_show(win_confirm, false);
        }

        break;

    default:
        break;

    }
}

static void switch_record_files_delete_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_record_files_delete_touch_cb\n");

    extern gui_win_t *win_confirm;

    switch (event)
    {
    case GUI_EVENT_1:
    case GUI_EVENT_2:
        // jump to win_confirm
        win_confirm->base.not_show = false;
        push_current_widget(tabview_main);
        tabview_main->base.not_show = true;
        break;

    default:
        break;
    }
}

//example for add record files
static void win_add_record_files_touch_cb(void *obj, gui_event_t event)
{
    gui_log("win_add_record_files_touch_cb\n");
    if (record_file_num < MAX_FILES_NUM)
    {
        switch_record_files[record_file_num] = gui_switch_create(grid_record_files, 0, 0, 288, 64,
                                                                 ICON_FILES_BASE_DARK_BIN, ICON_FILES_BASE_DARK_BIN);
        switch_record_files[record_file_num]->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        switch_record_files[record_file_num]->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        switch_record_files[record_file_num]->long_touch_state_pic_addr = ICON_TEXT_BASE_RED_BIN;
        switch_record_files[record_file_num]->long_touch_state_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        switch_record_files[record_file_num]->long_touch_enable = true;
        gui_log("win_add_record_files_touch_cb switch_record_files = 0x%x, num = %d\n",
                (uint32_t *)switch_record_files[record_file_num], record_file_num);
        gui_obj_add_event_cb(switch_record_files[record_file_num],
                             (gui_event_cb_t)switch_record_files_touch_cb,
                             GUI_EVENT_1, NULL);
        gui_obj_add_event_cb(switch_record_files[record_file_num],
                             (gui_event_cb_t)switch_record_files_touch_cb,
                             GUI_EVENT_2, NULL);
        gui_obj_add_event_cb(switch_record_files[record_file_num],
                             (gui_event_cb_t)switch_record_files_touch_cb,
                             GUI_EVENT_TOUCH_LONG, NULL);

        char *string_record_files = record_files_name[record_file_num];
        scroll_text_record_files[record_file_num] = gui_scroll_text_create(
                                                        switch_record_files[record_file_num], "scroll_text_record_files", 39, 11, 192, FONT_H_32);
        gui_scroll_text_set(scroll_text_record_files[record_file_num], string_record_files,
                            "rtk_font_mem", gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX), strlen(string_record_files), FONT_H_32);
        gui_scroll_text_scroll_set(scroll_text_record_files[record_file_num], SCROLL_X, 0, 0, 5000, 0);
        gui_log("win_add_record_files_touch_cb scroll_text_record_files = 0x%x, num = %d\n",
                (uint32_t *)scroll_text_record_files[record_file_num], record_file_num);

        record_file_num++;

        //update page and grid attribute
        page_record_files->get_yend = 0;
        grid_record_files->row = record_file_num + 1;
    }

}
//example for add record files
static void example_add_record_files(void)
{
    win_add_record_files = gui_win_create(curtain_down_record, "win_add_record_files", 177, 24, 128,
                                          32);
    gui_obj_add_event_cb(win_add_record_files, (gui_event_cb_t)win_add_record_files_touch_cb,
                         GUI_EVENT_TOUCH_RELEASED, NULL);
}

void design_curtain_record_files(void *parent)
{
    //gui_curtainview_t *curtainview_record = (gui_curtainview_t *)parent;
    curtain_down_record = gui_curtain_create(parent, "curtain_down_record", 0, 0, LCD_W, LCD_H,
                                             CURTAIN_DOWN, 1.0f);
    img_base_record_files = gui_img_create_from_mem(curtain_down_record, "img_base_record_files",
                                                    WATCH_BASE_BLACK_BIN, 0, 0, LCD_W, LCD_H);
    // blend_mode default value is IMG_FILTER_BLACK. pic with rgb 00 00 00(or maybe 01 01 01?) seems like not be painted
    // to solve:
    // 1. set blend_mode as IMG_BYPASS_MODE
    // 2. load pic WATCH_BASE_BLACK_BIN with a little bit higher rgb (05 05 05 for example)
    img_base_record_files->draw_img.blend_mode = IMG_BYPASS_MODE;

    switch_record_files_delete = gui_switch_create(img_base_record_files, 88, 373, 277, 81,
                                                   ICON_RECORD_DELETE_BIN, ICON_RECORD_DELETE_BIN);
    switch_record_files_delete->on_hl_pic_addr = ICON_RECORD_DELETE_TOUCH_BIN;
    switch_record_files_delete->off_hl_pic_addr = ICON_RECORD_DELETE_TOUCH_BIN;
    gui_obj_add_event_cb(switch_record_files_delete,
                         (gui_event_cb_t)switch_record_files_delete_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_record_files_delete,
                         (gui_event_cb_t)switch_record_files_delete_touch_cb,
                         GUI_EVENT_2, NULL);
    switch_record_files_delete->base.not_show = true;

    page_record_files = gui_page_create(img_base_record_files, "page_record_files", 0, 0, LCD_W, LCD_H);

    char *string_title_record_files = "录音文件";
    text_title_record_files = gui_text_create(page_record_files, "text_title_record_files", 177, 24,
                                              strlen(string_title_record_files) / FONT_CHINESE_BYTE * FONT_CHINESE_W, FONT_H_32);
    gui_text_set(text_title_record_files, string_title_record_files, "rtk_font_mem", APP_COLOR_WHITE,
                 strlen(string_title_record_files), FONT_H_32);

    grid_record_files = gui_grid_create(page_record_files, 83, 111, 1, 1, 50, 64 + 18);
    //touch title to add a record file
    example_add_record_files();

}