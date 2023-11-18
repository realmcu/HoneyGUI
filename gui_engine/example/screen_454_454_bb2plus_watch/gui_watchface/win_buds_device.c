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

static void switch_back_device_buds_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_back_device_buds_touch_cb, event = %d\n", event);

    extern gui_win_t *win_menu_buds;
    win_menu_buds->base.not_show = false;
    extern gui_win_t *win_buds_device;
    win_buds_device->base.not_show = true;

    gui_tree_free(win_buds_device);
    win_buds_device = NULL;
}

static void switch_text_base_bonding_buds_1_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_text_base_bonding_buds_1_touch_cb, event = %d\n", event);
    switch (event)
    {
    case GUI_EVENT_1: //widget switch is on
    case GUI_EVENT_2: //widget switch is off
        {
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


void win_device_buds_confirm_yes_action(void)
{
    gui_log("win_device_buds_confirm_yes_action\n");
}

void win_device_buds_confirm_no_action(void)
{
    gui_log("win_device_buds_confirm_no_action\n");
}

void win_device_buds_confirm_text_display(void)
{
    gui_log("win_device_buds_confirm_text_display\n");
}

static void switch_delete_bonding_buds_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_delete_bonding_buds_touch_cb, event = %d\n", event);

    //extern void push_current_screen(void *obj);
    extern gui_win_t *win_buds_device;
    //push_current_screen(win_buds_device);
    win_buds_device->base.not_show = true;

    extern gui_win_t *win_confirm;
    gui_app_t *app = get_app_watch_ui();
    if (win_confirm == NULL)
    {
        win_confirm = gui_win_create(&(app->screen), "win_confirm", 0, 0, 454, 454);
        extern void set_confirm_info(void *yes_action, void *no_action, void *text_display);
        // set_confirm_info(win_device_buds_confirm_yes_action, win_device_buds_confirm_no_action,
        //                  win_device_buds_confirm_text_display);
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
    gui_text_set(text_device_buds_title, "绑定信息", "rtk_font_mem", 0xffffffff, 12, font_size);
    gui_text_type_set(text_device_buds_title, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_bonding_buds_1 = gui_text_create(parent, "text_bonding_buds_1", 154, 122, 160, font_size);
    gui_text_set(text_bonding_buds_1, "绑定耳机1", "rtk_font_mem", 0xffffffff, 13, font_size);
    gui_text_type_set(text_bonding_buds_1, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_bonding_buds_2 = gui_text_create(parent, "text_bonding_buds_2", 154, 204, 160, font_size);
    gui_text_set(text_bonding_buds_2, "绑定耳机2", "rtk_font_mem", 0xffffffff, 13, font_size);
    gui_text_type_set(text_bonding_buds_2, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_bonding_buds_3 = gui_text_create(parent, "text_bonding_buds_3", 154, 284, 160, font_size);
    gui_text_set(text_bonding_buds_3, "绑定耳机3", "rtk_font_mem", 0xffffffff, 13, font_size);
    gui_text_type_set(text_bonding_buds_3, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_delete_bonding_buds = gui_text_create(parent, "text_disconnect", 163, 392, 128, font_size);
    gui_text_set(text_delete_bonding_buds, "删除绑定", "rtk_font_mem", 0xffffffff, 12, font_size);
    gui_text_type_set(text_delete_bonding_buds, SIMKAI_SIZE32_BITS1_FONT_BIN);
    text_delete_bonding_buds->base.not_show = true;
}