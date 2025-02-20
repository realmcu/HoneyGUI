#include "lvgl.h"
#include "app_main.h"

static bool is_at_bottom  = false;

static void page_scroll_event_cb(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_SCROLL_END)
    {
        lv_obj_t *obj = lv_event_get_target(e);
        lv_coord_t scrollable = lv_obj_get_scroll_bottom(obj);
        // lv_log("scrollable=%d\n", scrollable);
        if (scrollable < -50)
        {
            is_at_bottom = true;
            lv_indev_wait_release(lv_indev_get_act());
            _ui_screen_change(&scr_watchface, &scr_up_curtain, LV_SCR_LOAD_ANIM_OUT_TOP, 500, 0,
                              &lv_watchface_init, 1);
        }
        else
        {
            is_at_bottom = false;
        }
    }
}

void scr_up_curtain_cb(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_SCREEN_LOAD_START)
    {
        // left_Animation(ui_hour_group, 0);
        // right_Animation(ui_label_min, 0);
        // opa_on_Animation(ui_weather_group_1, 300);
        // opa_on_Animation(ui_date_group, 500);
        // opa_on_Animation(ui_weather_title_group_1, 400);
    }
    if (event_code == LV_EVENT_GESTURE &&
        lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP)
    {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&scr_watchface, &scr_up_curtain, LV_SCR_LOAD_ANIM_OUT_TOP, 500, 0,
                          &lv_watchface_init, 1);
    }
}

void lv_up_curtain_init(void)
{
    // scr_up_curtain = lv_obj_create(NULL);
    // lv_obj_add_event_cb(scr_up_curtain, (lv_event_cb_t)scr_up_curtain_cb, LV_EVENT_ALL, NULL);

    lv_obj_t *page = lv_obj_create(scr_up_curtain);
    lv_obj_set_size(page, 410, 502);
    lv_obj_set_pos(page, 0, 0);
    lv_obj_set_style_bg_color(page, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(page, 76, 0);
    lv_obj_add_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(page, 0, 0);
    // lv_obj_add_event_cb(page, (lv_event_cb_t)page_scroll_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_t *label = lv_label_create(page);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_label_set_text(label, "This is a multiline\n"
                      "scrollable text content.\n"
                      "Add enough lines or reduce\n"
                      "the size of the page.\n"
                      "\n"
                      "\n"
                      "This text must be longer\n"
                      "than the page height to\n"
                      "enable scrolling.\n"
                      "\n"
                      "Keep adding lines...\n"
                      "The more lines, the better.\n"
                      "\n"
                      "\n"
                      "\n"
                      "Keep adding...\n"
                      "\n"
                      "Almost there...\n"
                      "\n"
                      "Scrolling should now work!"
                      "This is a multiline\n"
                      "scrollable text content.\n"
                      "Add enough lines or reduce\n"
                      "the size of the page.\n"
                      "\n"
                      "\n"
                      "This text must be longer\n"
                      "than the page height to\n"
                      "enable scrolling.\n"
                      "\n"
                      "Keep adding lines...\n"
                      "The more lines, the better.\n"
                      "\n"
                      "\n"
                      "\n"
                      "Keep adding...\n"
                      "\n"
                      "Almost there...\n"
                      "\n"
                      "Scrolling should now work!"
                     );
}

