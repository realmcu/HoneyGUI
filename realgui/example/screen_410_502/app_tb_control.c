#include "root_image_hongkong/ui_resource.h"
#include "gui_switch.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_canvas_round_rect.h"
#include "gui_canvas_rect.h"
#include "guidef.h"
#include "tp_algo.h"
#include "kb_algo.h"
#include <gui_app.h>
#include "kb_algo.h"

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502

typedef struct switch_state
{
    uint8_t sw_1 : 1;
    uint8_t sw_2 : 1;
    uint8_t sw_3 : 1;
    uint8_t sw_4 : 1;
    uint8_t sw_5 : 1;
} switch_state_t;

static switch_state_t sw_state = {0};
static gui_img_t *img_phone, *img_nobother, *img_mute;
static gui_win_t *win_control_enter, *win_control;
static bool enter_flag = 0;
static gui_switch_t *sw_lte, *sw_wifi, *sw_phone, *sw_mute, *sw_nobother;


static void callback_phone_on(void *obj, gui_event_t e, void *param)
{
    img_phone->data = PHONE_ON_ICON_BIN;
}
static void callback_phone_off(void *obj, gui_event_t e, void *param)
{
    img_phone->data = PHONE_OFF_ICON_BIN;
}
static void callback_nobother_on(void *obj, gui_event_t e, void *param)
{
    img_nobother->data = NOBOTHER_ON_ICON_BIN;
}
static void callback_nobother_off(void *obj, gui_event_t e, void *param)
{
    img_nobother->data = NOBOTHER_OFF_ICON_BIN;
}
static void callback_mute_on(void *obj, gui_event_t e, void *param)
{
    img_mute->data = MUTE_ON_ICON_BIN;
}
static void callback_mute_off(void *obj, gui_event_t e, void *param)
{
    img_mute->data = MUTE_OFF_ICON_BIN;
}

static void win_cb()
{
    float progress = win_control->animate->progress_percent;
    gui_obj_t *obj = GUI_BASE(win_control);
    // enter animation
    if (!enter_flag)
    {
        if (obj->x > 100)
        {
            obj->x = (1 - progress) * SCREEN_WIDTH;
        }
        else
        {
            obj->x = 0;
            enter_flag = 1;
        }
    }
    sw_state.sw_1 = sw_lte->ifon;
    sw_state.sw_2 = sw_wifi->ifon;
    sw_state.sw_3 = sw_phone->ifon;
    sw_state.sw_4 = sw_mute->ifon;
    sw_state.sw_5 = sw_nobother->ifon;
}

static void page_tb_control(gui_obj_t *parent)
{
    win_control = gui_win_create(parent, NULL, SCREEN_WIDTH, 0, 0, 0);
    gui_win_set_animate(win_control, 500, -1, (gui_animate_callback_t)win_cb, NULL);

    // draw background
    gui_canvas_rect_t *canvas_rect = gui_canvas_rect_create(GUI_BASE(win_control), NULL, 0, 0,
                                                            SCREEN_WIDTH, SCREEN_HEIGHT, gui_rgb(0, 0, 0));

    //switch
    sw_lte = gui_switch_create(win_control, 20, 100, 182, 121, CONTROL_LTE_OFF_BIN,
                               CONTROL_LTE_ON_BIN);
    sw_wifi = gui_switch_create(win_control, 207, 100, 182, 121, CONTROL_WIFI_OFF_BIN,
                                CONTROL_WIFI_ON_BIN);
    sw_phone = gui_switch_create(win_control, 20, 100 + 125 * 1, 182, 121,
                                 CONTROL_PHONE_OFF_BIN,
                                 CONTROL_PHONE_ON_BIN);
    sw_mute = gui_switch_create(win_control, 20, 100 + 125 * 2, 182, 121,
                                CONTROL_MUTE_OFF_BIN,
                                CONTROL_MUTE_ON_BIN);
    sw_nobother = gui_switch_create(win_control, 207, 100 + 125 * 2, 182, 121,
                                    CONTROL_NOBOTHER_OFF_BIN,
                                    CONTROL_NOBOTHER_ON_BIN);
    gui_switch_change_state(sw_lte, sw_state.sw_1);
    gui_switch_change_state(sw_wifi, sw_state.sw_2);
    gui_switch_change_state(sw_phone, sw_state.sw_3);
    gui_switch_change_state(sw_mute, sw_state.sw_4);
    gui_switch_change_state(sw_nobother, sw_state.sw_5);

    // capsule
    gui_canvas_round_rect_t *capsule = gui_canvas_round_rect_create(GUI_BASE(win_control), NULL,
                                                                    136, 20, 138, 47, 20, gui_rgb(196, 196, 196));
    img_phone = gui_img_create_from_mem(capsule, "capsule_phone", PHONE_OFF_ICON_BIN, 15, 9, 0, 0);
    gui_img_set_mode(img_phone, IMG_SRC_OVER_MODE);
    img_nobother = gui_img_create_from_mem(capsule, "capsule_nobother", NOBOTHER_OFF_ICON_BIN, 56, 9, 0,
                                           0);
    gui_img_set_mode(img_nobother, IMG_SRC_OVER_MODE);
    img_mute = gui_img_create_from_mem(capsule, "capsule_mute", MUTE_OFF_ICON_BIN, 98, 9, 0, 0);
    gui_img_set_mode(img_mute, IMG_SRC_OVER_MODE);

    if (sw_state.sw_3)
    {
        img_phone->data = PHONE_ON_ICON_BIN;
    }
    if (sw_state.sw_4)
    {
        img_mute->data = MUTE_ON_ICON_BIN;
    }
    if (sw_state.sw_5)
    {
        img_nobother->data = NOBOTHER_ON_ICON_BIN;
    }

    //add switch callback func
    gui_obj_add_event_cb(sw_phone, (gui_event_cb_t)callback_phone_on, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(sw_phone, (gui_event_cb_t)callback_phone_off, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(sw_nobother, (gui_event_cb_t)callback_nobother_on, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(sw_nobother, (gui_event_cb_t)callback_nobother_off, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(sw_mute, (gui_event_cb_t)callback_mute_on, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(sw_mute, (gui_event_cb_t)callback_mute_off, GUI_EVENT_2, NULL);

    gui_img_t *img_charge = gui_img_create_from_mem(win_control, NULL, CONTROL_PAD_BIN, 207,
                                                    100 + 125 * 1, 182, 121);
    gui_img_t *img_num = gui_img_create_from_mem(img_charge, NULL, UI_TEXT_9_BIN, 35, 40, 0, 0);
    gui_img_scale(img_num, 0.8, 0.8);
    gui_img_set_mode(img_num, IMG_SRC_OVER_MODE);
    img_num = gui_img_create_from_mem(img_charge, NULL, UI_TEXT_6_BIN, 65, 40, 0, 0);
    gui_img_scale(img_num, 0.8, 0.8);
    gui_img_set_mode(img_num, IMG_SRC_OVER_MODE);
    img_num = gui_img_create_from_mem(img_charge, NULL, UI_TEXT_PERCENT_BIN, 95, 40, 0, 0);
    gui_img_scale(img_num, 0.8, 0.8);
    gui_img_set_mode(img_num, IMG_SRC_OVER_MODE);
}

void win_control_enter_cb()
{
    extern gui_kb_port_data_t *port_kb_get_data(void);
    gui_kb_port_data_t *kb = port_kb_get_data();
    static bool kb_hold = 0;
    static bool win_control_release = 0;
    static uint32_t time_press = 0;
    if (enter_flag)
    {
        {
            gui_list_t *node = NULL;
            gui_list_t *tmp = NULL;
            gui_obj_t *obj = GUI_BASE(win_control_enter)->parent;
            gui_list_for_each_safe(node, tmp, &obj->child_list)
            {
                gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
                if (obj->magic != GUI_MAGIC_NUMBER)
                {
                    return;
                }
                obj->gesture = 1;
                obj->active = 0;
                obj->not_show = 1;
                // gui_log(" %s-->%s \n", obj->name, obj->parent->name);
            }
            GUI_BASE(win_control_enter)->gesture = 0;
            GUI_BASE(win_control_enter)->active = 1;
            GUI_BASE(win_control_enter)->not_show = 0;
        }
        gui_obj_t *obj = GUI_BASE(win_control);
        touch_info_t *tp = tp_get_info();
        static int16_t deltaX = 0;
        if (tp->deltaX == deltaX)
        {
            obj->x = 0;
            return;
        }
        if (tp->deltaX < 205 && tp->deltaX >= 0)
        {
            obj->x = tp->deltaX * 2;
            deltaX = tp->deltaX;
        }
        else if (tp->deltaX > 205)
        {
            // gui_log("tp->deltaX = %d\n", tp->deltaX);
            obj->x = SCREEN_WIDTH;
            enter_flag = 0;
            deltaX = 0;
            gui_obj_tree_free(win_control);
            win_control_release = 1;
        }
    }
    else
    {
        if (win_control_release)
        {
            win_control_release = 0;
            gui_list_t *node = NULL;
            gui_list_t *tmp = NULL;
            gui_obj_t *obj = GUI_BASE(win_control_enter)->parent;
            gui_list_for_each_safe(node, tmp, &obj->child_list)
            {
                gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
                if (obj->magic != GUI_MAGIC_NUMBER)
                {
                    return;
                }
                obj->gesture = 0;
                obj->active = 1;
                obj->not_show = 0;
                // gui_log(" %s-->%s \n", obj->name, obj->parent->name);
            }
        }
        if (kb_hold)
        {
            if (kb->event == GUI_KB_EVENT_UP)
            {
                kb_hold = 0;
                uint32_t time = kb->timestamp_ms_release - time_press;
                if (time > 1000)
                {
                    gui_log("pressing time = %d\n", time);
                    gui_obj_event_set(GUI_BASE(win_control_enter), GUI_EVENT_8);
                }
            }
        }
        if (kb->event == GUI_KB_EVENT_DOWN && !kb_hold)
        {
            time_press = kb->timestamp_ms_press;
            kb_hold = 1;
        }
    }
}

static void switch_control_board(void *obj, gui_event_t e, void *param)
{
    gui_log("switch_control_board\n");
    page_tb_control((gui_obj_t *)param);
}

void page_tb_control_enter(void *parent)
{
    win_control_enter = gui_win_create(parent, NULL, 0, 0, 0, 0);
    gui_obj_add_event_cb(win_control_enter, (gui_event_cb_t)switch_control_board, GUI_EVENT_8,
                         win_control_enter);
    gui_win_set_animate(win_control_enter, 1000, -1, win_control_enter_cb, NULL);
}
