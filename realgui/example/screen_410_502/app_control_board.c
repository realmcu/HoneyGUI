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
#include "app_hongkong.h"

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502

#define CURRENT_VIEW_NAME "app_control_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
static void page_tb_control(gui_view_t *view);

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .design_cb = page_tb_control,
};

static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    watchface_view = gui_view_descriptor_get("watchface_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

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
    sw_state.sw_1 = sw_lte->ifon;
    sw_state.sw_2 = sw_wifi->ifon;
    sw_state.sw_3 = sw_phone->ifon;
    sw_state.sw_4 = sw_mute->ifon;
    sw_state.sw_5 = sw_nobother->ifon;
}

static void page_tb_control(gui_view_t *view)
{
    clear_mem();

    gui_view_switch_on_event(view, watchface_view, VIEW_TRANSPLATION, VIEW_STILL,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
    win_control = gui_win_create(view, NULL, 0, 0, 0, 0);
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
