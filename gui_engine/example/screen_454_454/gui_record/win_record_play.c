#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_text.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_win.h"
#include "gui_tabview.h"
#include "gui_switch.h"
#include "gui_scroll_text.h"

gui_win_t *win_record_play = NULL;
gui_switch_t *switch_record_play_back = NULL;
gui_scroll_text_t *scroll_text_record_play_title = NULL;
gui_text_t *text_record_play_time = NULL;
gui_switch_t *switch_record_play_pause = NULL;

static void record_play_back_touch_cb(void *obj, gui_event_t event)
{
    gui_log("record_play_back_touch_cb\n");
    gui_obj_t *object = (gui_obj_t *)obj;

    gui_obj_t *object_return = pop_current_widget();
    gui_obj_show(object_return, true);
    //object_return->not_show = false;

    gui_log("record_play_back_touch_cb object->parent->name = %s\n", object->parent->name);
    gui_tree_free(win_record_play);
    win_record_play = NULL;
}

static void switch_record_play_pause_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_record_play_pause_touch_cb, event = %d\n", event);

    switch (event)
    {
    // touch to play
    case GUI_EVENT_1:
        break;

    //touch to pause
    case GUI_EVENT_2:
        break;

    default:
        break;
    }

}

static void scroll_text_record_play_title_animate_cb(void *obj)
{
    gui_log("scroll_text_record_play_title_animate_cb\n");

    gui_switch_t *file = (gui_switch_t *)obj;
    gui_scroll_text_t *file_name = NULL;

    gui_list_t *node = NULL;
    gui_list_for_each(node, &file->base.child_list)
    {
        gui_obj_t *find_obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (find_obj->type == SCROLLTEXTBOX)
        {
            file_name = (gui_scroll_text_t *)find_obj;
            break;
        }
    }

    gui_log("scroll_text_record_play_title_animate_cb obj = 0x%x\n", (uint32_t *)obj);
    gui_log("scroll_text_record_play_title_animate_cb title = 0x%x\n", (uint32_t *)file_name);

    char *string_title = (char *)file_name->base.content;
    gui_scroll_text_set(scroll_text_record_play_title, string_title, GUI_FONT_SOURCE_BMP,
                        gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX), strlen(string_title), FONT_H_32);
}

static void text_record_play_time_animate_cb(void *obj)
{
    gui_log("text_record_play_time_animate_cb\n");
}

void design_win_record_play(gui_win_t *parent, gui_switch_t *selected_record_file)
{
    gui_log("design_win_record_play win_record_play = 0x%x\n", (uint32_t *)win_record_play);
    switch_record_play_back = gui_switch_create(parent, 129, 24, 48, 48, ICON_BACK_BIN,
                                                ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_record_play_back, (gui_event_cb_t)record_play_back_touch_cb,
                         GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_record_play_back, (gui_event_cb_t)record_play_back_touch_cb,
                         GUI_EVENT_2,
                         NULL);

    switch_record_play_pause = gui_switch_create(parent, 134, 71, 192, 192, ICON_RECORD_PLAY_BIN,
                                                 ICON_RECORD_PAUSE_BIN);
    switch_record_play_pause->off_hl_pic_addr = ICON_RECORD_PLAY_TOUCH_BIN;
    switch_record_play_pause->on_hl_pic_addr = ICON_RECORD_PAUSE_TOUCH_BIN;
    gui_obj_add_event_cb(switch_record_play_pause, (gui_event_cb_t)switch_record_play_pause_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_record_play_pause, (gui_event_cb_t)switch_record_play_pause_touch_cb,
                         GUI_EVENT_2, NULL);

    scroll_text_record_play_title = gui_scroll_text_create(parent, "scroll_text_record_play_title", 170,
                                                           24, 128, FONT_H_32);
    gui_scroll_text_scroll_set(scroll_text_record_play_title, SCROLL_X, 0, 0, 5000, 0);
    gui_scroll_text_set(scroll_text_record_play_title, "录音文件123456", GUI_FONT_SOURCE_BMP,
                        gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX), strlen("录音文件123456"), FONT_H_32);
    gui_text_set_animate(scroll_text_record_play_title, 1000, 2,
                         scroll_text_record_play_title_animate_cb, selected_record_file);

    char *string_record_play_time = "00:00:00/00:00:00";
    text_record_play_time = gui_text_create(parent, "text_record_play_time", 108, 281, 248, FONT_H_32);
    gui_text_set(text_record_play_time, string_record_play_time, GUI_FONT_SOURCE_BMP, APP_COLOR_WHITE,
                 strlen(string_record_play_time), FONT_H_32);
    gui_text_set_animate(text_record_play_time, 1000, 0, text_record_play_time_animate_cb,
                         text_record_play_time);
}


