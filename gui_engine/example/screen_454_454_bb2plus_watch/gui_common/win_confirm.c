#include "root_image/ui_resource.h"
#include "gui_common.h"
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_win.h"
#include "app_gui_main.h"
#include "gui_tabview.h"
#include "gui_obj.h"

gui_obj_t *obj_stack[GUI_OBJ_STACK_SIZE_MAX] = {NULL};
int8_t obj_stack_index = 0;

gui_win_t *win_confirm = NULL;
gui_text_t *text_confirm = NULL;
gui_switch_t *switch_confirm_yes = NULL;
gui_switch_t *switch_confirm_no = NULL;

void (*confirm_yes_action)(void *obj) = NULL;
void (*confirm_no_action)(void *obj) = NULL;
void (*confirm_text_display)(void *obj) = NULL;

typedef struct _win_confirm_action_t
{
    void (*confirm_yes_action)(void *obj);
    void (*confirm_no_action)(void *obj);
    void (*confirm_text_display)(void *obj);
    void *yes_action_obj;
    void *no_action_obj;
    void *text_display_obj;
} win_confirm_action_t;

win_confirm_action_t win_confirm_action;

void set_confirm_yes(void *yes_action, void *obj)
{
    win_confirm_action.confirm_yes_action = yes_action;
    win_confirm_action.yes_action_obj = obj;
}

void set_confirm_no(void *no_action, void *obj)
{
    win_confirm_action.confirm_no_action = no_action;
    win_confirm_action.no_action_obj = obj;
}

void set_confirm_text(void *text_display, void *obj)
{
    win_confirm_action.confirm_text_display = text_display;
    win_confirm_action.text_display_obj = obj;
}

void push_current_widget(void *obj)
{
    if (obj_stack_index < GUI_OBJ_STACK_SIZE_MAX)
    {
        obj_stack[obj_stack_index] = (gui_obj_t *)obj;
    }
    else
    {
        GUI_ASSERT(obj_stack_index >= GUI_OBJ_STACK_SIZE_MAX);
    }
    obj_stack_index++;
    gui_log("push_current_widget: obj_stack = 0x%x obj_stack_index = %d\n",
            obj_stack[obj_stack_index - 1], obj_stack_index);
}

gui_obj_t *pop_current_widget(void)
{
    obj_stack_index--;
    if (obj_stack_index < GUI_OBJ_STACK_SIZE_MAX && obj_stack_index >= 0)
    {
        gui_log("pop_current_widget obj_stack = %x, obj_stack_index = %d\n", obj_stack[obj_stack_index],
                obj_stack_index);
        return obj_stack[obj_stack_index];
    }
    else
    {
        GUI_ASSERT(obj_stack_index < 0);
    }
}

static void switch_confirm_yes_touch_cb(void *obj, gui_event_cb_t event)
{
    gui_log("switch_confirm_yes_touch_cb\n");
    win_confirm_action.confirm_yes_action(win_confirm_action.yes_action_obj);

    gui_obj_t *object = (gui_obj_t *)obj;
    gui_log("obj->parent->name = %s\n", object->parent->name);
    gui_tree_free(win_confirm);
    win_confirm = NULL;
    set_confirm_yes(NULL, NULL);
    set_confirm_no(NULL, NULL);
    set_confirm_text(NULL, NULL);

    gui_obj_t *object_return = pop_current_widget();
    gui_obj_show(object_return, true);
    //object_return->not_show = false;
}

static void switch_confirm_no_touch_cb(void *obj, gui_event_cb_t event)
{
    gui_log("switch_confirm_no_touch_cb\n");
    win_confirm_action.confirm_no_action(win_confirm_action.no_action_obj);

    // gui_obj_t *object = (gui_obj_t *)obj;
    // gui_log("obj->parent->name = %s\n", object->parent->name);
    // gui_tree_free(win_confirm);
    // win_confirm = NULL;
    // set_confirm_yes(NULL, NULL);
    // set_confirm_no(NULL, NULL);
    // set_confirm_text(NULL, NULL);

    // gui_obj_t *object_return = pop_current_widget();
    // object_return->not_show = false;
}

static void text_confirm_update_cb(void *obj)
{
    gui_log("text_confirm_update_cb\n");
    win_confirm_action.confirm_text_display(win_confirm_action.text_display_obj);
}


void design_win_confirm(void *parent)
{
    text_confirm = gui_text_create(parent, "text_confirm", 123, 131, 224, FONT_H_32);
    gui_text_set(text_confirm, "确认删除文件？", "rtk_font_mem", 0xffffffff, 19, FONT_H_32);
    //win_confirm_action.confirm_text_display(win_confirm_action.text_display_obj);
    //gui_obj_add_update_cb(text_confirm, text_confirm_update_cb, 0xffffffff, false, NULL);

    switch_confirm_yes = gui_switch_create(parent, 75, 249, 96, 96, ICON_CONFIRM_BIN, ICON_CONFIRM_BIN);
    switch_confirm_yes->off_hl_pic_addr = ICON_CONFIRM_TOUCH_BIN;
    switch_confirm_yes->on_hl_pic_addr = ICON_CONFIRM_TOUCH_BIN;
    gui_obj_add_event_cb(switch_confirm_yes, (gui_event_cb_t)switch_confirm_yes_touch_cb, GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_confirm_yes, (gui_event_cb_t)switch_confirm_yes_touch_cb, GUI_EVENT_2,
                         NULL);

    switch_confirm_no = gui_switch_create(parent, 283, 249, 96, 96, ICON_CANCEL_BIN, ICON_CANCEL_BIN);
    switch_confirm_no->off_hl_pic_addr = ICON_CANCEL_TOUCH_BIN;
    switch_confirm_no->on_hl_pic_addr = ICON_CANCEL_TOUCH_BIN;
    gui_obj_add_event_cb(switch_confirm_no, (gui_event_cb_t)switch_confirm_no_touch_cb, GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_confirm_no, (gui_event_cb_t)switch_confirm_no_touch_cb, GUI_EVENT_2,
                         NULL);
}


