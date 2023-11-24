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

gui_switch_t *switch_back_search_buds = NULL;
gui_switch_t *switch_text_base_search_buds_1 = NULL;
gui_switch_t *switch_text_base_search_buds_2 = NULL;
gui_switch_t *switch_text_base_search_buds_3 = NULL;
gui_text_t *text_search_buds_title = NULL;
gui_text_t *text_search_buds_1 = NULL;
gui_text_t *text_search_buds_2 = NULL;
gui_text_t *text_search_buds_3 = NULL;
extern gui_win_t *win_search_buds;
extern gui_win_t *win_menu_buds;
extern gui_win_t *win_search_buds;

static void switch_back_buds_searching_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_back_buds_searching_touch_cb, event = %d\n", event);
    win_menu_buds->base.not_show = false;
    win_search_buds->base.not_show = true;

    gui_tree_free(win_search_buds);
    win_search_buds = NULL;
}

static void img_success_animate_callback(gui_img_t *img)
{
    gui_log("img_success_animate_callback");
    if (img->animate->progress_percent == 1.0f)
    {
        img->animate->dur = 0;
        win_search_buds->base.not_show = true;
        gui_obj_t *object_return = pop_current_widget();
        gui_obj_show(object_return, true);
        // todo free win confirm
    }
}

static void img_loading_animate_callback(gui_img_t *img)
{
    gui_log("img_loading_animate_callback");
    gui_img_translate(img, img->base.w / 2, img->base.h / 2);
    gui_img_rotation(img, 360 * img->animate->progress_percent, img->base.w / 2, img->base.h / 2);
    if (img->animate->progress_percent == 1.0f)
    {
        bool test_flag = true;//only for test
        img->animate->dur = 0;
        if (test_flag)
        {
            gui_img_t *img_success = gui_img_create_from_mem(win_search_buds, "img_success", ICON_SUCCESS_BIN,
                                                             163, 163,
                                                             128, 128);
            gui_img_set_animate(img_success, 500, 0, img_success_animate_callback, img_success);
        }
        else
        {
            gui_img_t *img_fail = gui_img_create_from_mem(win_search_buds, "img_fail", ICON_FAIL_BIN, 163, 163,
                                                          128, 128);
        }
    }
}

static void searching_buds_general_touch_cb(void)
{
    gui_img_t *img_base_menu_background = gui_img_create_from_mem(win_search_buds,
                                                                  "img_base_menu_background",
                                                                  WATCH_BASE_GREY_70_ALPHA_BIN, 0, 0,
                                                                  454, 454);
    gui_img_t *img_back_search_buds = gui_img_create_from_mem(win_search_buds, "img_back_search_buds",
                                                              ICON_BACK_BIN, 131, 24, 48, 48);
    gui_img_t *img_search_buds_1 = gui_img_create_from_mem(win_search_buds, "img_search_buds_1",
                                                           ICON_TEXT_BASE_DARK_BIN, 83, 111, 288, 64);
    img_search_buds_1->draw_img.opacity_value = 128;
    gui_img_t *img_search_buds_2 = gui_img_create_from_mem(win_search_buds, "img_search_buds_2",
                                                           ICON_TEXT_BASE_DARK_BIN, 83, 195, 288, 64);
    img_search_buds_2->draw_img.opacity_value = 128;
    gui_img_t *img_search_buds_3 = gui_img_create_from_mem(win_search_buds, "img_search_buds_3",
                                                           ICON_TEXT_BASE_DARK_BIN, 83, 275, 288, 64);
    img_search_buds_3->draw_img.opacity_value = 128;

    gui_obj_show(switch_back_search_buds, false);
    gui_obj_show(switch_text_base_search_buds_1, false);
    gui_obj_show(switch_text_base_search_buds_2, false);
    gui_obj_show(switch_text_base_search_buds_3, false);
// todo
    int16_t font_size = 32;
    text_search_buds_title = gui_text_create(win_search_buds, "text_search_buds_title", 179, 24, 128,
                                             font_size);
    gui_text_set(text_search_buds_title, "搜索耳机", "rtk_font_mem", 0xffffffff, 12, font_size);
    gui_text_type_set(text_search_buds_title, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_search_buds_1 = gui_text_create(win_search_buds, "text_bonding_buds_1", 138, 122, 178,
                                         font_size);
    gui_text_set(text_search_buds_1, "搜索到设备1", "rtk_font_mem", 0xffffffff, 16, font_size);
    gui_text_type_set(text_search_buds_1, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_search_buds_2 = gui_text_create(win_search_buds, "text_search_buds_2", 138, 204, 178,
                                         font_size);
    gui_text_set(text_search_buds_2, "搜索到设备2", "rtk_font_mem", 0xffffffff, 16, font_size);
    gui_text_type_set(text_search_buds_2, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_search_buds_3 = gui_text_create(win_search_buds, "text_search_buds_3", 138, 286, 178,
                                         font_size);
    gui_text_set(text_search_buds_3, "搜索到设备3", "rtk_font_mem", 0xffffffff, 16, font_size);
    gui_text_type_set(text_search_buds_3, SIMKAI_SIZE32_BITS1_FONT_BIN);

    gui_img_t *img_loading = gui_img_create_from_mem(win_search_buds, "img_loading", ICON_LOADING_BIN,
                                                     179, 179, 96, 96);
    gui_img_set_animate(img_loading, 1000, 0, img_loading_animate_callback, img_loading);
}

static void switch_text_base_searching_buds_1_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_text_base_searching_buds_1_touch_cb, event = %d\n", event);
    switch (event)
    {
    case GUI_EVENT_1: //widget switch is on
    case GUI_EVENT_2: //widget switch is off
        {
            searching_buds_general_touch_cb();
            break;
        }
    default:
        break;
    }
}

static void switch_text_base_searching_buds_2_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_text_base_searching_buds_2_touch_cb, event = %d\n", event);
    switch (event)
    {
    case GUI_EVENT_1: //widget switch is on
    case GUI_EVENT_2: //widget switch is off
        {
            searching_buds_general_touch_cb();
            break;
        }
    default:
        break;
    }
}

static void switch_text_base_searching_buds_3_touch_cb(void *obj, gui_event_t event)
{
    gui_log("switch_text_base_searching_buds_3_touch_cb, event = %d\n", event);
    switch (event)
    {
    case GUI_EVENT_1: //widget switch is on
    case GUI_EVENT_2: //widget switch is off
        {
            searching_buds_general_touch_cb();
            break;
        }

    default:
        break;
    }
}

void design_win_buds_searching(void *parent)
{
    switch_back_search_buds = gui_switch_create(parent, 131, 24, 48, 48, ICON_BACK_BIN, ICON_BACK_BIN);
    gui_obj_add_event_cb(switch_back_search_buds, (gui_event_cb_t)switch_back_buds_searching_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_back_search_buds, (gui_event_cb_t)switch_back_buds_searching_touch_cb,
                         GUI_EVENT_2, NULL);

    switch_text_base_search_buds_1 = gui_switch_create(parent, 83, 111, 288, 64,
                                                       ICON_TEXT_BASE_DARK_BIN, ICON_TEXT_BASE_DARK_BIN);
    switch_text_base_search_buds_1->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_search_buds_1->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;

    gui_obj_add_event_cb(switch_text_base_search_buds_1,
                         (gui_event_cb_t)switch_text_base_searching_buds_1_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_text_base_search_buds_1,
                         (gui_event_cb_t)switch_text_base_searching_buds_1_touch_cb, GUI_EVENT_2, NULL);

    switch_text_base_search_buds_2 = gui_switch_create(parent, 83, 195, 288, 64,
                                                       ICON_TEXT_BASE_DARK_BIN, ICON_TEXT_BASE_DARK_BIN);
    switch_text_base_search_buds_2->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_search_buds_2->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;

    gui_obj_add_event_cb(switch_text_base_search_buds_2,
                         (gui_event_cb_t)switch_text_base_searching_buds_2_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_text_base_search_buds_2,
                         (gui_event_cb_t)switch_text_base_searching_buds_2_touch_cb, GUI_EVENT_2, NULL);


    switch_text_base_search_buds_3 = gui_switch_create(parent, 83, 275, 288, 64,
                                                       ICON_TEXT_BASE_DARK_BIN, ICON_TEXT_BASE_DARK_BIN);
    switch_text_base_search_buds_3->off_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;
    switch_text_base_search_buds_3->on_hl_pic_addr = ICON_TEXT_BASE_BLUE_BIN;

    gui_obj_add_event_cb(switch_text_base_search_buds_3,
                         (gui_event_cb_t)switch_text_base_searching_buds_3_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_text_base_search_buds_3,
                         (gui_event_cb_t)switch_text_base_searching_buds_3_touch_cb, GUI_EVENT_2, NULL);

    int16_t font_size = 32;
    text_search_buds_title = gui_text_create(parent, "text_search_buds_title", 179, 24, 128, font_size);
    gui_text_set(text_search_buds_title, "搜索耳机", "rtk_font_mem", 0xffffffff, 12, font_size);
    gui_text_type_set(text_search_buds_title, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_search_buds_1 = gui_text_create(parent, "text_bonding_buds_1", 138, 122, 178, font_size);
    gui_text_set(text_search_buds_1, "搜索到设备1", "rtk_font_mem", 0xffffffff, 16, font_size);
    gui_text_type_set(text_search_buds_1, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_search_buds_2 = gui_text_create(parent, "text_search_buds_2", 138, 204, 178, font_size);
    gui_text_set(text_search_buds_2, "搜索到设备2", "rtk_font_mem", 0xffffffff, 16, font_size);
    gui_text_type_set(text_search_buds_2, SIMKAI_SIZE32_BITS1_FONT_BIN);

    text_search_buds_3 = gui_text_create(parent, "text_search_buds_3", 138, 286, 178, font_size);
    gui_text_set(text_search_buds_3, "搜索到设备3", "rtk_font_mem", 0xffffffff, 16, font_size);
    gui_text_type_set(text_search_buds_3, SIMKAI_SIZE32_BITS1_FONT_BIN);
}