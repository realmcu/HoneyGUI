#include "root_image/ui_resource.h"
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_switch.h"
#include "gui_win.h"
#include "app_gui_main.h"
#include "gui_tabview.h"
#include "gui_obj.h"
#include "gui_common.h"

gui_switch_t *switch_back_device_buds = NULL;
gui_switch_t *switch_text_base_bonding_buds_1 = NULL;
gui_switch_t *switch_text_base_bonding_buds_2 = NULL;
gui_switch_t *switch_text_base_bonding_buds_3 = NULL;
gui_switch_t *switch_delete_bonding_buds = NULL;
gui_text_t *text_device_buds_title = NULL;
gui_text_t *text_bonding_buds_1 = NULL;
gui_text_t *text_bonding_buds_2 = NULL;
gui_text_t *text_bonding_buds_3 = NULL;
gui_text_t *text_delete_bonding_buds = NULL;
char *txet_delete_buds = "确认删除绑定？";
extern gui_win_t *win_buds_device;
extern gui_win_t *win_confirm;
extern gui_win_t *win_menu_buds;

static void switch_back_device_buds_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_back_device_buds_touch_cb, event = %d\n", event);
    win_menu_buds->base.not_show = false;
    win_buds_device->base.not_show = true;

    gui_tree_free(win_buds_device);
    win_buds_device = NULL;
}

static void img_success_animate_callback(gui_img_t *img)
{
    gui_log("img_success_animate_callback");
    if (img->animate->progress_percent == 1.0f)
    {
        img->animate->dur = 0;
        win_buds_device->base.not_show = true;
        gui_obj_t *object_return = pop_current_widget();
        gui_obj_show(object_return, true);
        // todo free win_confirm
    }
}

static void img_loading_animate_callback(gui_img_t *img)
{
    gui_log("img_loading_animate_callback");
    gui_img_translate(img, img->base.w / 2, img->base.h / 2);
    gui_img_rotation(img, 360 * img->animate->progress_percent, img->base.w / 2, img->base.h / 2);
    if (img->animate->progress_percent == 1.0f)
    {
        bool test_flag = 1;
        img->animate->dur = 0;
        if (test_flag)
        {
            gui_img_t *img_success = gui_img_create_from_mem(win_buds_device, "img_success", ICON_SUCCESS_BIN,
                                                             163, 163,
                                                             128, 128);
            gui_img_set_animate(img_success, 500, 0, img_success_animate_callback, img_success);
        }
        else
        {
            gui_img_t *img_fail = gui_img_create_from_mem(win_buds_device, "img_fail", ICON_FAIL_BIN, 163, 163,
                                                          128, 128);
        }
    }
}

static void searching_buds_general_touch_cb(void)
{
    gui_img_t *img_base_menu_background = gui_img_create_from_mem(win_buds_device,
                                                                  "img_base_menu_background",
                                                                  WATCH_BASE_GREY_70_ALPHA_BIN, 0, 0,
                                                                  454, 454);
    gui_img_t *img_back_search_buds = gui_img_create_from_mem(win_buds_device, "img_back_search_buds",
                                                              ICON_BACK_BIN, 131, 24, 48, 48);
    gui_img_t *img_bond_buds_1 = gui_img_create_from_mem(win_buds_device, "img_bond_buds_1",
                                                         ICON_TEXT_BASE_DARK_BIN, 83, 111, 288, 64);
    img_bond_buds_1->draw_img.opacity_value = 128;
    gui_img_t *img_bond_buds_2 = gui_img_create_from_mem(win_buds_device, "img_bond_buds_2",
                                                         ICON_TEXT_BASE_DARK_BIN, 83, 195, 288, 64);
    img_bond_buds_2->draw_img.opacity_value = 128;
    gui_img_t *img_bond_buds_3 = gui_img_create_from_mem(win_buds_device, "img_bond_buds_3",
                                                         ICON_TEXT_BASE_DARK_BIN, 83, 275, 288, 64);
    img_bond_buds_3->draw_img.opacity_value = 128;

    gui_obj_show(switch_back_device_buds, false);
    gui_obj_show(switch_text_base_bonding_buds_1, false);
    gui_obj_show(switch_text_base_bonding_buds_2, false);
    gui_obj_show(switch_text_base_bonding_buds_3, false);
// todo
    int16_t font_size = 32;
    text_device_buds_title = gui_text_create(win_buds_device, "text_device_buds_title", 179, 24, 128,
                                             font_size);
    gui_text_set(text_device_buds_title, "绑定信息", "rtk_font_mem", APP_COLOR_WHITE, 12,
                 font_size);
    gui_text_type_set(text_device_buds_title, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_bonding_buds_1 = gui_text_create(win_buds_device, "text_bonding_buds_1", 154, 122, 160,
                                          font_size);
    gui_text_set(text_bonding_buds_1, "绑定耳机1", "rtk_font_mem", APP_COLOR_WHITE, 13, font_size);
    gui_text_type_set(text_bonding_buds_1, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_bonding_buds_2 = gui_text_create(win_buds_device, "text_bonding_buds_2", 154, 204, 160,
                                          font_size);
    gui_text_set(text_bonding_buds_2, "绑定耳机2", "rtk_font_mem", APP_COLOR_WHITE, 13, font_size);
    gui_text_type_set(text_bonding_buds_2, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_bonding_buds_3 = gui_text_create(win_buds_device, "text_bonding_buds_3", 154, 284, 160,
                                          font_size);
    gui_text_set(text_bonding_buds_3, "绑定耳机3", "rtk_font_mem", APP_COLOR_WHITE, 13, font_size);
    gui_text_type_set(text_bonding_buds_3, SIMKAI_SIZE32_BITS1_FONT_BIN);

    gui_img_t *img_loading = gui_img_create_from_mem(win_buds_device, "img_loading", ICON_LOADING_BIN,
                                                     179, 179, 96, 96);
    gui_img_set_animate(img_loading, 1000, 0, img_loading_animate_callback, img_loading);

}

static void switch_text_base_bonding_buds_1_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_text_base_bonding_buds_1_touch_cb, event = %d\n", event);
    switch (event)
    {
    case GUI_EVENT_1: //widget switch is on
    case GUI_EVENT_2: //widget switch is off
        {
            searching_buds_general_touch_cb();
            break;
        }
    case GUI_EVENT_TOUCH_LONG:
        {
            switch_delete_bonding_buds->base.not_show = !switch_delete_bonding_buds->base.not_show;
            text_delete_bonding_buds->base.not_show = !text_delete_bonding_buds->base.not_show;
            if (switch_delete_bonding_buds->base.not_show || text_delete_bonding_buds->base.not_show)
            {
                switch_text_base_bonding_buds_1->on_pic_addr = ICON_TEXT_BASE_DARK_BIN;
                switch_text_base_bonding_buds_1->off_pic_addr = ICON_TEXT_BASE_DARK_BIN;
                switch_text_base_bonding_buds_2->on_pic_addr = ICON_TEXT_BASE_DARK_BIN;
                switch_text_base_bonding_buds_2->off_pic_addr = ICON_TEXT_BASE_DARK_BIN;
                switch_text_base_bonding_buds_3->on_pic_addr = ICON_TEXT_BASE_DARK_BIN;
                switch_text_base_bonding_buds_3->off_pic_addr = ICON_TEXT_BASE_DARK_BIN;
            }
            else
            {
                switch_text_base_bonding_buds_1->on_pic_addr = ICON_TEXT_BASE_RED_BIN;
                switch_text_base_bonding_buds_1->off_pic_addr = ICON_TEXT_BASE_RED_BIN;
            }
            break;
        }
    default:
        break;
    }
}

static void switch_text_base_bonding_buds_2_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_text_base_bonding_buds_2_touch_cb, event = %d\n", event);
    switch (event)
    {
    case GUI_EVENT_1: //widget switch is on
    case GUI_EVENT_2: //widget switch is off
        {
            searching_buds_general_touch_cb();
            break;
        }
    case GUI_EVENT_TOUCH_LONG:
        {
            switch_delete_bonding_buds->base.not_show = !switch_delete_bonding_buds->base.not_show;
            text_delete_bonding_buds->base.not_show = !text_delete_bonding_buds->base.not_show;
            if (switch_delete_bonding_buds->base.not_show || text_delete_bonding_buds->base.not_show)
            {
                switch_text_base_bonding_buds_1->on_pic_addr = ICON_TEXT_BASE_DARK_BIN;
                switch_text_base_bonding_buds_1->off_pic_addr = ICON_TEXT_BASE_DARK_BIN;
                switch_text_base_bonding_buds_2->on_pic_addr = ICON_TEXT_BASE_DARK_BIN;
                switch_text_base_bonding_buds_2->off_pic_addr = ICON_TEXT_BASE_DARK_BIN;
                switch_text_base_bonding_buds_3->on_pic_addr = ICON_TEXT_BASE_DARK_BIN;
                switch_text_base_bonding_buds_3->off_pic_addr = ICON_TEXT_BASE_DARK_BIN;
            }
            else
            {
                switch_text_base_bonding_buds_2->on_pic_addr = ICON_TEXT_BASE_RED_BIN;
                switch_text_base_bonding_buds_2->off_pic_addr = ICON_TEXT_BASE_RED_BIN;
            }
            break;
        }
    default:
        break;
    }
}

static void switch_text_base_bonding_buds_3_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_text_base_bonding_buds_3_touch_cb, event = %d\n", event);
    switch (event)
    {
    case GUI_EVENT_1: //widget switch is on
    case GUI_EVENT_2: //widget switch is off
        {
            searching_buds_general_touch_cb();
            break;
        }
    case GUI_EVENT_TOUCH_LONG:
        {
            switch_delete_bonding_buds->base.not_show = !switch_delete_bonding_buds->base.not_show;
            text_delete_bonding_buds->base.not_show = !text_delete_bonding_buds->base.not_show;
            if (switch_delete_bonding_buds->base.not_show || text_delete_bonding_buds->base.not_show)
            {
                switch_text_base_bonding_buds_1->on_pic_addr = ICON_TEXT_BASE_DARK_BIN;
                switch_text_base_bonding_buds_1->off_pic_addr = ICON_TEXT_BASE_DARK_BIN;
                switch_text_base_bonding_buds_2->on_pic_addr = ICON_TEXT_BASE_DARK_BIN;
                switch_text_base_bonding_buds_2->off_pic_addr = ICON_TEXT_BASE_DARK_BIN;
                switch_text_base_bonding_buds_3->on_pic_addr = ICON_TEXT_BASE_DARK_BIN;
                switch_text_base_bonding_buds_3->off_pic_addr = ICON_TEXT_BASE_DARK_BIN;
            }
            else
            {
                switch_text_base_bonding_buds_3->on_pic_addr = ICON_TEXT_BASE_RED_BIN;
                switch_text_base_bonding_buds_3->off_pic_addr = ICON_TEXT_BASE_RED_BIN;
            }
            break;
        }
    default:
        break;
    }
}


void device_buds_confirm_yes_action(void *obj)
{
    gui_log("device_buds_confirm_yes_action, obj = 0x%x\n", (uint32_t *)obj);
}

void device_buds_confirm_no_action(void *obj)
{
    gui_log("device_buds_confirm_no_action, obj = 0x%x\n", (uint32_t *)obj);
    win_buds_device->base.not_show = false;

    gui_tree_free(win_confirm);
    win_confirm = NULL;
}

void win_device_buds_confirm_text_display(void)
{
    gui_log("win_device_buds_confirm_text_display\n");
}

static void switch_delete_bonding_buds_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_delete_bonding_buds_touch_cb, event = %d\n", event);
    push_current_widget(win_buds_device);
    gui_obj_show(win_buds_device, false);

    gui_app_t *app = get_app_watch_ui();
    //if (win_confirm == NULL)
    {
        win_confirm = gui_win_create(&(app->screen), "win_confirm", 0, 0, 454, 454);
        extern void set_confirm_info(void *yes_action, void *no_action, void *text_display);

        set_confirm_yes(device_buds_confirm_yes_action, obj);
        set_confirm_no(device_buds_confirm_no_action, obj);
        set_confirm_text(txet_delete_buds, 123, 131, 7);

        extern void design_win_confirm(void *parent);
        design_win_confirm(win_confirm);
    }
}

void design_win_buds_device(void *parent)
{
    switch_back_device_buds = gui_switch_create(parent, 131, 24, 48, 48, ICON_BACK_BIN, ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_back_device_buds, (gui_event_cb_t)switch_back_device_buds_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_back_device_buds, (gui_event_cb_t)switch_back_device_buds_touch_cb,
                         GUI_EVENT_2, NULL);

    switch_text_base_bonding_buds_1 = gui_switch_create(parent, 83, 111, 288, 64,
                                                        ICON_TEXT_BASE_DARK_BIN, ICON_TEXT_BASE_DARK_BIN);
    switch_text_base_bonding_buds_1->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_bonding_buds_1->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_bonding_buds_1->long_touch_state_pic_addr = ICON_TEXT_BASE_RED_BIN;
    switch_text_base_bonding_buds_1->long_touch_enable = true;
    gui_obj_add_event_cb(switch_text_base_bonding_buds_1,
                         (gui_event_cb_t)switch_text_base_bonding_buds_1_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_text_base_bonding_buds_1,
                         (gui_event_cb_t)switch_text_base_bonding_buds_1_touch_cb, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(switch_text_base_bonding_buds_1,
                         (gui_event_cb_t)switch_text_base_bonding_buds_1_touch_cb, GUI_EVENT_TOUCH_LONG, NULL);

    switch_text_base_bonding_buds_2 = gui_switch_create(parent, 83, 193, 288, 64,
                                                        ICON_TEXT_BASE_DARK_BIN, ICON_TEXT_BASE_DARK_BIN);
    switch_text_base_bonding_buds_2->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_bonding_buds_2->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_bonding_buds_2->long_touch_state_pic_addr = ICON_TEXT_BASE_RED_BIN;
    switch_text_base_bonding_buds_2->long_touch_enable = true;
    gui_obj_add_event_cb(switch_text_base_bonding_buds_2,
                         (gui_event_cb_t)switch_text_base_bonding_buds_2_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_text_base_bonding_buds_2,
                         (gui_event_cb_t)switch_text_base_bonding_buds_2_touch_cb, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(switch_text_base_bonding_buds_2,
                         (gui_event_cb_t)switch_text_base_bonding_buds_2_touch_cb, GUI_EVENT_TOUCH_LONG, NULL);

    switch_text_base_bonding_buds_3 = gui_switch_create(parent, 83, 275, 288, 64,
                                                        ICON_TEXT_BASE_DARK_BIN, ICON_TEXT_BASE_DARK_BIN);
    switch_text_base_bonding_buds_3->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_bonding_buds_3->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_bonding_buds_3->long_touch_state_pic_addr = ICON_TEXT_BASE_RED_BIN;
    switch_text_base_bonding_buds_3->long_touch_enable = true;
    gui_obj_add_event_cb(switch_text_base_bonding_buds_3,
                         (gui_event_cb_t)switch_text_base_bonding_buds_3_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_text_base_bonding_buds_3,
                         (gui_event_cb_t)switch_text_base_bonding_buds_3_touch_cb, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(switch_text_base_bonding_buds_3,
                         (gui_event_cb_t)switch_text_base_bonding_buds_3_touch_cb, GUI_EVENT_TOUCH_LONG, NULL);

    switch_delete_bonding_buds = gui_switch_create(parent, 88, 373, 277, 81, ICON_DISCONNECT_BIN,
                                                   ICON_DISCONNECT_BIN);
    switch_delete_bonding_buds->off_hl_pic_addr = ICON_DISCONNECT_TOUCH_BIN;
    switch_delete_bonding_buds->on_hl_pic_addr = ICON_DISCONNECT_TOUCH_BIN;
    gui_obj_add_event_cb(switch_delete_bonding_buds,
                         (gui_event_cb_t)switch_delete_bonding_buds_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_delete_bonding_buds,
                         (gui_event_cb_t)switch_delete_bonding_buds_touch_cb, GUI_EVENT_2, NULL);
    switch_delete_bonding_buds->base.not_show = true;

    int16_t font_size = 32;
    text_device_buds_title = gui_text_create(parent, "text_device_buds_title", 179, 24, 128, font_size);
    gui_text_set(text_device_buds_title, "绑定信息", "rtk_font_mem", APP_COLOR_WHITE, 12,
                 font_size);
    gui_text_type_set(text_device_buds_title, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_bonding_buds_1 = gui_text_create(parent, "text_bonding_buds_1", 154, 122, 160, font_size);
    gui_text_set(text_bonding_buds_1, "绑定耳机1", "rtk_font_mem", APP_COLOR_WHITE, 13, font_size);
    gui_text_type_set(text_bonding_buds_1, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_bonding_buds_2 = gui_text_create(parent, "text_bonding_buds_2", 154, 204, 160, font_size);
    gui_text_set(text_bonding_buds_2, "绑定耳机2", "rtk_font_mem", APP_COLOR_WHITE, 13, font_size);
    gui_text_type_set(text_bonding_buds_2, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_bonding_buds_3 = gui_text_create(parent, "text_bonding_buds_3", 154, 284, 160, font_size);
    gui_text_set(text_bonding_buds_3, "绑定耳机3", "rtk_font_mem", APP_COLOR_WHITE, 13, font_size);
    gui_text_type_set(text_bonding_buds_3, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_delete_bonding_buds = gui_text_create(parent, "text_disconnect", 163, 392, 128, font_size);
    gui_text_set(text_delete_bonding_buds, "删除绑定", "rtk_font_mem", APP_COLOR_WHITE, 12,
                 font_size);
    gui_text_type_set(text_delete_bonding_buds, SIMKAI_SIZE32_BITS1_FONT_BIN);
    text_delete_bonding_buds->base.not_show = true;
}