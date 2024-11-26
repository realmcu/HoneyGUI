#include "root_image/ui_resource.h"
#include "gui_common.h"
#include "gui_curtainview.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_win.h"
#include "app_gui_main.h"
#include "gui_tabview.h"
#include "gui_obj.h"


static gui_text_t *text_confirm = NULL;
#if 0
gui_img_t *img_base_menu_confirming = NULL;
gui_img_t *img_loading = NULL;
gui_img_t *img_confirm_yes = NULL;
gui_img_t *img_confirm_no = NULL;
gui_img_t *img_success = NULL;
gui_img_t *img_fail = NULL;
uint8_t degree = 0;
#endif
typedef struct _text_display_t
{
    char *text;
    int16_t x;
    int16_t y;
    uint16_t length;
} text_display_t;

static text_display_t text_display;
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

static win_confirm_action_t win_confirm_action;

void set_confirm_yes(void yes_action(void *), void *obj)
{
    win_confirm_action.confirm_yes_action = yes_action;
    win_confirm_action.yes_action_obj = obj;
}

void set_confirm_no(void no_action(void *), void *obj)
{
    win_confirm_action.confirm_no_action = no_action;
    win_confirm_action.no_action_obj = obj;
}

void set_confirm_text(char *confirm_text_display, int16_t x, int16_t y, uint16_t length)
{
    text_display.text = confirm_text_display;
    text_display.x = x;
    text_display.y = y;
    text_display.length = length;
}
#if 0
static void img_success_touch_cb(void *obj, uint16_t event)
{
    gui_log("img_success_touch_cb");
    if (event == GUI_EVENT_TOUCH_CLICKED)
    {
        gui_obj_t *object_return = pop_current_widget();
        gui_obj_show(object_return, true);
        if (win_confirm != NULL)
        {
            gui_obj_tree_free(win_confirm);
            win_confirm = NULL;
        }
    }
}

static void img_fail_touch_cb(void *obj, uint16_t event)
{
    gui_log("img_fail_touch_cb");
    if (event == GUI_EVENT_TOUCH_CLICKED)
    {
        gui_obj_t *object_return = pop_current_widget();
        gui_obj_show(object_return, true);
        if (win_confirm != NULL)
        {
            gui_obj_tree_free(win_confirm);
            win_confirm = NULL;
        }
    }
}

static void img_loading_animate_callback(gui_img_t *img)
{
    gui_log("img_loading_animate_callback");
    gui_img_translate(img, img->base.w / 2, img->base.h / 2);
    gui_img_rotation(img, 360 * img->animate->progress_percent, img->base.w / 2, img->base.h / 2);
    if (img->animate->progress_percent == 1.0f)
    {
        bool test_flag = 1;//todo
        img->animate->dur = 0;
        if (test_flag)
        {
            img_success = gui_img_create_from_mem(win_confirm, "img_success", ICON_SUCCESS_BIN, 163, 163,
                                                  128, 128);
            gui_obj_add_event_cb(img_success, (gui_event_cb_t)img_success_touch_cb, GUI_EVENT_TOUCH_CLICKED,
                                 NULL);
        }
        else
        {
            img_fail = gui_img_create_from_mem(win_confirm, "img_fail", ICON_FAIL_BIN, 163, 163,
                                               128, 128);
            gui_obj_add_event_cb(img_success, (gui_event_cb_t)img_fail_touch_cb, GUI_EVENT_TOUCH_CLICKED,
                                 NULL);
        }
    }
}
#endif
static void switch_confirm_yes_touch_cb(void *obj, gui_event_cb_t event)
{
    gui_log("switch_confirm_yes_touch_cb\n");

#if 0
    img_base_menu_confirming = gui_img_create_from_mem(win_confirm, "img_base_menu_confirming",
                                                       WATCH_BASE_GREY_70_ALPHA_BIN, 0, 0,
                                                       454, 454);
    img_loading = gui_img_create_from_mem(win_confirm, "img_loading", ICON_LOADING_BIN, 179, 179,
                                          96, 96);
    gui_img_set_animate(img_loading, 1000, 0, img_loading_animate_callback, img_loading);

    img_confirm_yes = gui_img_create_from_mem(win_confirm, "img_confirm_yes", ICON_CONFIRM_BIN, 75, 249,
                                              96, 96);
    img_confirm_yes->draw_img.opacity_value = 128;
    img_confirm_no = gui_img_create_from_mem(win_confirm, "img_confirm_no", ICON_CANCEL_BIN, 283, 249,
                                             96, 96);
    img_confirm_no->draw_img.opacity_value = 128;
    text_confirm = gui_text_create(win_confirm, "text_confirm", text_display.x, text_display.y,
                                   text_display.length * 32, FONT_H_32);
    gui_text_set(text_confirm, text_display.text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 (text_display.length * 3),
                 FONT_H_32);


    gui_obj_show(switch_confirm_yes, false);
    gui_obj_show(switch_confirm_no, false);
#endif

    win_confirm_action.confirm_yes_action(win_confirm_action.yes_action_obj);

    set_confirm_yes(NULL, NULL);
    set_confirm_no(NULL, NULL);
    set_confirm_text(NULL, 0, 0, 0);
}

static void switch_confirm_no_touch_cb(void *obj, gui_event_cb_t event)
{
    gui_log("switch_confirm_no_touch_cb\n");
    win_confirm_action.confirm_no_action(win_confirm_action.no_action_obj);

    set_confirm_yes(NULL, NULL);
    set_confirm_no(NULL, NULL);
    set_confirm_text(NULL, 0, 0, 0);
}

void design_tab_confirm(void *parent)
{
    text_confirm = gui_text_create(parent, "text_confirm", text_display.x, text_display.y,
                                   text_display.length * 32, FONT_H_32);
    gui_text_set(text_confirm, text_display.text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 (text_display.length * 3),
                 FONT_H_32);

    gui_switch_t *switch_confirm_yes = gui_switch_create(parent, 75, 249, 96, 96, ICON_CONFIRM_BIN,
                                                         ICON_CONFIRM_BIN);
    switch_confirm_yes->off_hl_pic_addr = ICON_CONFIRM_TOUCH_BIN;
    switch_confirm_yes->on_hl_pic_addr = ICON_CONFIRM_TOUCH_BIN;
    gui_obj_add_event_cb(switch_confirm_yes, (gui_event_cb_t)switch_confirm_yes_touch_cb, GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_confirm_yes, (gui_event_cb_t)switch_confirm_yes_touch_cb, GUI_EVENT_2,
                         NULL);

    gui_switch_t *switch_confirm_no = gui_switch_create(parent, 283, 249, 96, 96, ICON_CANCEL_BIN,
                                                        ICON_CANCEL_BIN);
    switch_confirm_no->off_hl_pic_addr = ICON_CANCEL_TOUCH_BIN;
    switch_confirm_no->on_hl_pic_addr = ICON_CANCEL_TOUCH_BIN;
    gui_obj_add_event_cb(switch_confirm_no, (gui_event_cb_t)switch_confirm_no_touch_cb, GUI_EVENT_1,
                         NULL);
    gui_obj_add_event_cb(switch_confirm_no, (gui_event_cb_t)switch_confirm_no_touch_cb, GUI_EVENT_2,
                         NULL);
}

