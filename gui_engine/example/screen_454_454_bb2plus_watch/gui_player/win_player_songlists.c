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

gui_text_t *text_song_list_title = NULL;
gui_text_t *text_song[3] = {NULL};
gui_win_t *win_song_lists = NULL;
gui_win_t *win_song_lists_back = NULL;
gui_switch_t *switch_song_files[3] = {NULL};
gui_switch_t *switch_song_delete = NULL;
gui_img_t *img_song_list_back = NULL;
extern gui_win_t *win_confirm;

char *txet_delete_songs = "确认删除歌曲？";
char *song_files_name[3] = {"song name 1", "song name 2", "song name 3"};

//if current view has a "delete icon", need to set delete info before create a win_delete
static void switch_song_delete_yes_action(void *obj)
{
    gui_log("switch_song_delete_yes_action, obj = 0x%x\n", obj);
    //delete selected record file
}
static void switch_song_delete_no_action(void *obj)
{
    gui_log("switch_song_delete_no_action, obj = 0x%x\n", obj);
    win_song_lists->base.not_show = false;

    gui_tree_free(win_confirm);
    win_confirm = NULL;
    //do nothing
}
//static void switch_song_delete_text(void *obj)
//{
//    gui_log("switch_song_delete_text, obj = 0x%x\n", obj);
//    // set delete text
//}

static void switch_song_back_cb(void *obj, gui_event_t event)
{
    gui_log("switch_song_back_cb event = %d\n", event);
    switch (event)
    {
    case GUI_EVENT_TOUCH_RELEASED:
        //if (win_song_lists != NULL)
        {
            gui_tree_free(win_song_lists);
            win_song_lists = NULL;
        }
        gui_obj_show(tabview_main, true);
        break;

    default:
        break;
    }
}

static void switch_song_cb(void *obj, gui_event_t event)
{
    gui_log("switch_song_1_cb event = %d\n", event);
    //gui_switch_t *this = (gui_switch_t *)obj;
    gui_app_t *app = get_app_watch_ui();
    extern gui_win_t *win_confirm;
    gui_obj_t *object_return = NULL;

    switch (event)
    {
    case GUI_EVENT_2: // switch is on(touch to do some turn-off action)
    case GUI_EVENT_1: // switch is off(touch to do some turn-on action)
        gui_tree_free(win_song_lists);
        win_song_lists = NULL;

        object_return = pop_current_widget();
        gui_obj_show(object_return, true);
        break;
    case GUI_EVENT_TOUCH_LONG:
        gui_log("GUI_EVENT_TOUCH_LONG obj = 0x%x\n", obj);
        switch_song_delete->base.not_show = true;
        for (int j = 0; j < 3; j ++)
        {
            gui_log("GUI_EVENT_TOUCH_LONG obj = 0x%x, j = %d, long touch = %d\n", switch_song_files[j], j,
                    switch_song_files[j]->long_touch_state);
            if (switch_song_files[j]->long_touch_state)
            {
                switch_song_delete->base.not_show = false;
            }
        }
        break;

    default:
        break;
    }
}

static void switch_song_delete_cb(void *obj, gui_event_t event)
{
    gui_log("switch_song_3_cb event = %d\n", event);
    extern gui_win_t *win_confirm;
    gui_app_t *app = get_app_watch_ui();

    switch (event)
    {
    case GUI_EVENT_2: // switch is on(touch to do some turn-off action)
    case GUI_EVENT_1: // switch is off(touch to do some turn-on action)
        //create delete window, set delete action
        if (!switch_song_delete->base.not_show)
        {
            if (win_confirm != NULL)
            {
                gui_tree_free(win_confirm);
                win_confirm = NULL;
                set_confirm_yes(NULL, NULL);
                set_confirm_no(NULL, NULL);
                set_confirm_text(NULL, 0, 0, 0);
            }
            win_confirm = gui_win_create(&(app->screen), "win_confirm", 0, 0, LCD_W, LCD_H);

            set_confirm_yes(switch_song_delete_yes_action, NULL);
            set_confirm_no(switch_song_delete_no_action, NULL);
            set_confirm_text(txet_delete_songs, 123, 131, 7);

            void design_win_confirm(void *parent);
            design_win_confirm(win_confirm);
            gui_obj_show(win_confirm, true);
        }
        push_current_widget(win_song_lists);
        win_song_lists->base.not_show = true;
        break;

    default:
        break;
    }
}


void design_win_song_lists(gui_win_t *parent)
{
    char *string_song_list_title = "播放列表";
    //char *string_song_1 = "Feels";
    //char *string_song_2 = "Atentions";
    //char *string_song_3 = "Hello";
    int font_size = 32;


    win_song_lists_back = gui_win_create(parent, "win_song_lists_back", 131, 24, 176, 48);
    gui_obj_add_event_cb(win_song_lists_back, (gui_event_cb_t)switch_song_back_cb,
                         GUI_EVENT_TOUCH_RELEASED, NULL);

    img_song_list_back = gui_img_create_from_mem(parent, "img_back", ICON_BACK_BIN, 131, 24,
                                                 48, 48);
    text_song_list_title = gui_text_create(parent, "text_song_list_title", 177, 30,
                                           strlen(string_song_list_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, font_size);
    gui_text_set(text_song_list_title, string_song_list_title, "rtk_font_mem", 0xffffffff,
                 strlen(string_song_list_title), font_size);

    for (uint8_t i = 0; i < 3; i++)
    {
        switch_song_files[i] = gui_switch_create(parent, 83, 111 + i * 82, 288, 64, ICON_TEXT_BASE_DARK_BIN,
                                                 ICON_TEXT_BASE_DARK_BIN);
        switch_song_files[i]->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        switch_song_files[i]->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        switch_song_files[i]->long_touch_enable = true;
        switch_song_files[i]->long_touch_state_pic_addr = ICON_TEXT_BASE_RED_BIN;
        switch_song_files[i]->long_touch_state_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
        //add touch event callback
        //GUI_EVENT_1 for switch is off(turn-on action)
        //GUI_EVENT_2 for switch is on(turn-off action)
        gui_obj_add_event_cb(switch_song_files[i], (gui_event_cb_t)switch_song_cb, GUI_EVENT_1, NULL);
        gui_obj_add_event_cb(switch_song_files[i], (gui_event_cb_t)switch_song_cb, GUI_EVENT_2, NULL);
        gui_obj_add_event_cb(switch_song_files[i], (gui_event_cb_t)switch_song_cb, GUI_EVENT_TOUCH_LONG,
                             NULL);

        text_song[i] = gui_text_create(parent, "text_songs", 100, 123 + i * 82,
                                       strlen(song_files_name[i]) * FONT_NUM_ALPHA_W, font_size);
        gui_text_set(text_song[i], song_files_name[i], "rtk_font_mem", 0xffffffff,
                     strlen(song_files_name[i]), font_size);
    }

    switch_song_delete = gui_switch_create(parent, 88, 373, 277, 81,
                                           ICON_RECORD_DELETE_BIN, ICON_RECORD_DELETE_BIN);
    switch_song_delete->on_hl_pic_addr = ICON_RECORD_DELETE_TOUCH_BIN;
    switch_song_delete->off_hl_pic_addr = ICON_RECORD_DELETE_TOUCH_BIN;
    gui_obj_add_event_cb(switch_song_delete, (gui_event_cb_t)switch_song_delete_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_song_delete, (gui_event_cb_t)switch_song_delete_cb, GUI_EVENT_2, NULL);
    switch_song_delete->base.not_show = true;
}


