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
#include "gui_img_scope.h"
#include "gui_scroll_text.h"
#include "record_mgr.h"
#ifndef _WIN32
#include "app_task.h"
#include "app_mmi.h"
#endif

static void record_files_back_touch_cb(void *obj, uint16_t event)
{
    gui_log("record_files_back_touch_cb\n");
    app_record_switch_tabs(RECORD_MAIN);
}

static void switch_record_files_touch_cb(void *obj, uint16_t event)
{
    gui_log("switch_record_files_touch_cb, obj = 0x%x\n", obj);
    switch (event)
    {
    case GUI_EVENT_1:
    case GUI_EVENT_2:
        {
            app_record_switch_tabs(RECORD_PLAY);
        }
        break;

    default:
        break;

    }
}

void design_tab_record_files(void *parent)
{

    gui_switch_t *switch_record_files_back = gui_switch_create(parent, 129, 24, 48, 48, ICON_BACK_BIN,
                                                               ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_record_files_back, (gui_event_cb_t)record_files_back_touch_cb,
                         GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_record_files_back, (gui_event_cb_t)record_files_back_touch_cb,
                         GUI_EVENT_2,
                         NULL);

    char *string_title_record_files = "录音文件";
    gui_text_t *text_title_record_files = gui_text_create(parent, "text_title_record_files", 177, 24,
                                                          strlen(string_title_record_files) / FONT_CHINESE_BYTE * FONT_CHINESE_W, FONT_H_32);
    gui_text_set(text_title_record_files, string_title_record_files, GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE,
                 strlen(string_title_record_files), FONT_H_32);

    gui_switch_t *switch_record_file = gui_switch_create(parent, 83, 111, 288, 64,
                                                         ICON_TEXT_BASE_DARK_BIN,
                                                         ICON_TEXT_BASE_DARK_BIN);
    switch_record_file->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_record_file->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    gui_obj_add_event_cb(switch_record_file, (gui_event_cb_t)switch_record_files_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_record_file, (gui_event_cb_t)switch_record_files_touch_cb,
                         GUI_EVENT_2, NULL);

    char *string_record_file = "record.dat";
    gui_text_t *text_record_file = gui_text_create(switch_record_file, "text_title_record_files", 10,
                                                   12, 160, FONT_H_32);
    gui_text_set(text_record_file, string_record_file, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(string_record_file), FONT_H_32);

}

