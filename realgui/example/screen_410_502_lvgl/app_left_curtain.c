#include "lvgl.h"
#include "app_main.h"

void scr_left_curtain_cb(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT)
    {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&scr_watchface, &scr_left_curtain, LV_SCR_LOAD_ANIM_OUT_LEFT, 500, 0,
                          &lv_watchface_init, 1);
    }
    if (event_code == LV_EVENT_SCREEN_LOAD_START)
    {
        // left_Animation(ui_hour_group, 0);
        // right_Animation(ui_label_min, 0);
        // opa_on_Animation(ui_weather_group_1, 300);
        // opa_on_Animation(ui_date_group, 500);
        // opa_on_Animation(ui_weather_title_group_1, 400);
    }
}

void lv_left_curtain_init(void)
{
    // scr_left_curtain = lv_obj_create(NULL);

    // lv_obj_set_style_bg_color(scr_left_curtain, lv_color_make(0, 0, 0), 0);
    // lv_obj_set_style_bg_opa(scr_left_curtain, LV_OPA_COVER, 0);

    extern const lv_image_dsc_t watchface_ring;
    lv_obj_t *img1 = lv_image_create(scr_left_curtain);
    lv_image_set_src(img1, &watchface_ring);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);
    lv_image_set_scale(img1, 512);
    lv_obj_clear_flag(img1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label1 = lv_label_create(scr_left_curtain);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_text(label1, "scr_left_curtain.");
    lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);

    // lv_obj_add_event_cb(scr_left_curtain, (lv_event_cb_t)scr_left_curtain_cb, LV_EVENT_ALL, NULL);
}

