#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_text.h"
#include "gui_scroll_text.h"
#include "gui_switch.h"
#include "gui_curtainview.h"
#include "gui_app.h"
#include "gui_win.h"

#define GUI_DIAL_NUM_MAX 24
#define BUFFER_DIAL_NUM_MAX 64

gui_win_t *win_dial = NULL;
gui_text_t *text_dial_title = NULL;
gui_text_t *text_dial_num = NULL;
gui_switch_t *switch_dial_back = NULL;
gui_switch_t *switch_dial_num[10] = {NULL};
gui_switch_t *switch_dial_delete = NULL;
gui_switch_t *switch_dial_outgoing_call = NULL;
void *icon_dial_num_bin[10] = {ICON_DIAL_0_BIN, ICON_DIAL_1_BIN, ICON_DIAL_2_BIN, ICON_DIAL_3_BIN, ICON_DIAL_4_BIN,
                               ICON_DIAL_5_BIN, ICON_DIAL_6_BIN, ICON_DIAL_7_BIN, ICON_DIAL_8_BIN, ICON_DIAL_9_BIN
                              };
void *icon_dial_num_touch_bin[10] = {ICON_DIAL_TOUCH_0_BIN, ICON_DIAL_TOUCH_1_BIN, ICON_DIAL_TOUCH_2_BIN, ICON_DIAL_TOUCH_3_BIN, ICON_DIAL_TOUCH_4_BIN,
                                     ICON_DIAL_TOUCH_5_BIN, ICON_DIAL_TOUCH_6_BIN, ICON_DIAL_TOUCH_7_BIN, ICON_DIAL_TOUCH_8_BIN, ICON_DIAL_TOUCH_9_BIN
                                    };

char buffer_dial_num[BUFFER_DIAL_NUM_MAX];
char *p_display_dial_num = buffer_dial_num;
int8_t dial_num_index = 0;

static void reset_dial_num(void)
{
    dial_num_index = 0;
    memset(buffer_dial_num, 0x00, BUFFER_DIAL_NUM_MAX);
    p_display_dial_num = buffer_dial_num;
}

static void switch_dial_back_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_dial_back_touch_cb\n");
    gui_obj_t *object = (gui_obj_t *)obj;
    //return to previous view
    gui_obj_t *object_return = pop_current_widget();
    gui_obj_show(object_return, true);

    gui_log("switch_dial_back_touch_cb object->parent->name = %s\n", object->parent->name);
    gui_tree_free(win_dial);
    win_dial = NULL;
    //dial num reset
    reset_dial_num();
}
static void switch_dial_num_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_dial_num_touch_cb\n");

    gui_switch_t *this = (gui_switch_t *)obj;
    char touch_dial_num = 0;
    uint32_t offset_num = 0;
    switch (event)
    {
    case GUI_EVENT_1:
    case GUI_EVENT_2:
        //free后再次进入win dial页面，create的widget地址和上次会不同?
        //offset_num = ((uint32_t)this - (uint32_t)switch_dial_num[0]) / 0x370; // sizeof gui_switch_t and gui_img_t
        for (uint8_t i = 0; i < 10; i++)
        {
            if (this->on_pic_addr == icon_dial_num_bin[i])
            {
                touch_dial_num = i + 0x30;
            }
        }
        //touch_dial_num =  (char)offset_num + 0x30;
        // gui_log("switch_dial_num_touch_cb this = 0x%x\n", this);
        // gui_log("switch_dial_num_touch_cb switch_dial_num[0] = 0x%x\n", switch_dial_num[0]);
        // gui_log("switch_dial_num_touch_cb offset_num = 0x%x\n", offset_num);
        break;

    case GUI_EVENT_TOUCH_LONG:
        if (this->on_pic_addr == icon_dial_num_bin[7])
        {
            touch_dial_num = '*';
        }
        else if (this->on_pic_addr == icon_dial_num_bin[9])
        {
            touch_dial_num = '#';
        }
        else if (this->on_pic_addr == icon_dial_num_bin[0])
        {
            touch_dial_num = '+';
        }
        else
        {
            for (uint8_t i = 0; i < 10; i++)
            {
                if (this->on_pic_addr == icon_dial_num_bin[i])
                {
                    touch_dial_num = i + 0x30;
                }
            }
        }
        this->long_touch_state = false;
        break;
    }

    if (dial_num_index < BUFFER_DIAL_NUM_MAX - 1)
    {
        buffer_dial_num[dial_num_index] = touch_dial_num;
        buffer_dial_num[dial_num_index + 1] = '\0';
        dial_num_index ++;
    }
    else
    {
        dial_num_index = BUFFER_DIAL_NUM_MAX - 1;
    }
    if (dial_num_index > GUI_DIAL_NUM_MAX && dial_num_index < BUFFER_DIAL_NUM_MAX - 1)
    {
        p_display_dial_num++;
    }
    gui_text_set(text_dial_num, (const char *)p_display_dial_num, "rtk_font_mem", 0xffffffff,
                 strlen(p_display_dial_num), FONT_H_32);
}

static void switch_dial_delete_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_dial_delete_touch_cb\n");
    if (dial_num_index > GUI_DIAL_NUM_MAX)
    {
        p_display_dial_num--;
    }
    dial_num_index--;
    if (dial_num_index < 0)
    {
        dial_num_index = 0;
    }
    buffer_dial_num[dial_num_index] = '\0';

    gui_text_set(text_dial_num, (const char *)p_display_dial_num, "rtk_font_mem", 0xffffffff,
                 strlen(p_display_dial_num), FONT_H_32);
}

static void switch_dial_outgoing_call_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_dial_outgoing_call_touch_cb\n");
    //create window calling
    extern gui_win_t *win_calling;
    gui_app_t *app = get_app_watch_ui();

    win_calling = gui_win_create(&(app->screen), "win_calling", 0, 0, LCD_H, LCD_W);

    extern void design_win_calling(void *parent, char *dial_num);
    design_win_calling(win_calling, buffer_dial_num);

    push_current_widget(win_dial);
    gui_obj_show(win_dial, false);
}

void design_win_dial(void *parent)
{
    char *string_dial_title = "电话";
    text_dial_title = gui_text_create(parent, "text_dial_title", 204, 24,
                                      strlen(string_dial_title) / FONT_CHINESE_BYTE * FONT_CHINESE_W, FONT_H_32);
    gui_text_set(text_dial_title, string_dial_title, "rtk_font_mem", 0xffffffff,
                 strlen(string_dial_title), FONT_H_32);

    switch_dial_back = gui_switch_create(parent, 129, 24, 48, 48, ICON_BACK_BIN, ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_dial_back, (gui_event_cb_t)switch_dial_back_touch_cb, GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_dial_back, (gui_event_cb_t)switch_dial_back_touch_cb, GUI_EVENT_2,
                         NULL);

    text_dial_num = gui_text_create(parent, "text_dial_num", 0, 79, LCD_W, FONT_H_32);
    gui_text_set(text_dial_num, (const char *)p_display_dial_num, "rtk_font_mem", 0xffffffff,
                 strlen(p_display_dial_num), FONT_H_32);
    gui_text_mode_set(text_dial_num, CENTER);

    for (uint8_t i = 0; i < 10; i++)
    {
        if (i == 0)
        {
            switch_dial_num[i] = gui_switch_create(parent, 118, 374, 118, 80, icon_dial_num_bin[i],
                                                   icon_dial_num_bin[i]);
            switch_dial_num[i]->on_hl_pic_addr = icon_dial_num_touch_bin[i];
            switch_dial_num[i]->off_hl_pic_addr = icon_dial_num_touch_bin[i];
            switch_dial_num[i]->long_touch_enable = true;
            gui_obj_add_event_cb(switch_dial_num[i], (gui_event_cb_t)switch_dial_num_touch_cb, GUI_EVENT_1,
                                 NULL);
            gui_obj_add_event_cb(switch_dial_num[i], (gui_event_cb_t)switch_dial_num_touch_cb, GUI_EVENT_2,
                                 NULL);
            switch_dial_num[i]->long_touch_enable = true;
            gui_obj_add_event_cb(switch_dial_num[i], (gui_event_cb_t)switch_dial_num_touch_cb,
                                 GUI_EVENT_TOUCH_LONG, NULL);
        }
        else if (i > 0 && i < 10 && i != 7)
        {
            switch_dial_num[i] = gui_switch_create(parent, (i - 1) % 3 * 118, 134 + (i - 1) / 3 * 80, 118, 80,
                                                   icon_dial_num_bin[i], icon_dial_num_bin[i]);
            switch_dial_num[i]->on_hl_pic_addr = icon_dial_num_touch_bin[i];
            switch_dial_num[i]->off_hl_pic_addr = icon_dial_num_touch_bin[i];
            gui_obj_add_event_cb(switch_dial_num[i], (gui_event_cb_t)switch_dial_num_touch_cb, GUI_EVENT_1,
                                 NULL);
            gui_obj_add_event_cb(switch_dial_num[i], (gui_event_cb_t)switch_dial_num_touch_cb, GUI_EVENT_2,
                                 NULL);
            switch_dial_num[i]->long_touch_enable = true;
            gui_obj_add_event_cb(switch_dial_num[i], (gui_event_cb_t)switch_dial_num_touch_cb,
                                 GUI_EVENT_TOUCH_LONG, NULL);
        }
        else if (i == 7)
        {
            switch_dial_num[i] = gui_switch_create(parent, 10, 294, 108, 80, icon_dial_num_bin[i],
                                                   icon_dial_num_bin[i]);
            switch_dial_num[i]->on_hl_pic_addr = icon_dial_num_touch_bin[i];
            switch_dial_num[i]->off_hl_pic_addr = icon_dial_num_touch_bin[i];
            gui_obj_add_event_cb(switch_dial_num[i], (gui_event_cb_t)switch_dial_num_touch_cb, GUI_EVENT_1,
                                 NULL);
            gui_obj_add_event_cb(switch_dial_num[i], (gui_event_cb_t)switch_dial_num_touch_cb, GUI_EVENT_2,
                                 NULL);
            switch_dial_num[i]->long_touch_enable = true;
            gui_obj_add_event_cb(switch_dial_num[i], (gui_event_cb_t)switch_dial_num_touch_cb,
                                 GUI_EVENT_TOUCH_LONG, NULL);
        }
    }
    switch_dial_delete = gui_switch_create(parent, 368, 142, 64, 64, ICON_BACKSPACE_BIN,
                                           ICON_BACKSPACE_BIN);
    switch_dial_delete->on_hl_pic_addr = ICON_TOUCH_BACKSPACE_BIN;
    switch_dial_delete->off_hl_pic_addr = ICON_TOUCH_BACKSPACE_BIN;
    gui_obj_add_event_cb(switch_dial_delete, (gui_event_cb_t)switch_dial_delete_touch_cb, GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_dial_delete, (gui_event_cb_t)switch_dial_delete_touch_cb, GUI_EVENT_2,
                         NULL);

    switch_dial_outgoing_call = gui_switch_create(parent, 363, 214, 64, 64, ICON_ANSWER_BIN,
                                                  ICON_ANSWER_BIN);
    switch_dial_outgoing_call->on_hl_pic_addr = ICON_ANSWER_TOUCH_BIN;
    switch_dial_outgoing_call->off_hl_pic_addr = ICON_ANSWER_TOUCH_BIN;
    gui_obj_add_event_cb(switch_dial_outgoing_call, (gui_event_cb_t)switch_dial_outgoing_call_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_dial_outgoing_call, (gui_event_cb_t)switch_dial_outgoing_call_touch_cb,
                         GUI_EVENT_2, NULL);

    reset_dial_num();
    //gui_tree_print((gui_obj_t *)win_dial);
}