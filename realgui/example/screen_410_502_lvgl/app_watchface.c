#include "lvgl.h"
#include "app_main.h"
#include "lv_img_dsc_list.h"

void scr_watchface_cb(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT)
    {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&scr_right_curtain, LV_SCR_LOAD_ANIM_OVER_LEFT, 500, 0, &lv_right_curtain_init);
    }
    if (event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT)
    {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&scr_left_curtain, LV_SCR_LOAD_ANIM_OVER_RIGHT, 500, 0, &lv_left_curtain_init);
    }
    if (event_code == LV_EVENT_SCREEN_LOAD_START)
    {
        // left_Animation(ui_hour_group, 0);
        // right_Animation(ui_label_min, 0);
        // opa_on_Animation(ui_weather_group_1, 300);
        // opa_on_Animation(ui_date_group, 500);
        // opa_on_Animation(ui_weather_title_group_1, 400);
    }
    if (event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP)
    {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&scr_down_curtain, LV_SCR_LOAD_ANIM_OVER_TOP, 500, 0, &lv_down_curtain_init);
    }
    if (event_code == LV_EVENT_GESTURE &&
        lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_BOTTOM)
    {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&scr_up_curtain, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 500, 0, &lv_up_curtain_init);
    }
}

void lv_watchface_init(void)
{
    scr_watchface = lv_obj_create(NULL);

    // lv_obj_set_style_bg_color(scr_watchface, lv_color_make(0, 0, 0), 0);
    // lv_obj_set_style_bg_opa(scr_watchface, LV_OPA_COVER, 0);

    extern const lv_image_dsc_t ui_clock_face_main;
    lv_obj_t *img1 = lv_image_create(scr_watchface);
    lv_image_set_src(img1, &ui_clock_face_main);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_set_width(img1, ui_clock_face_main.header.w);
    // lv_obj_set_height(img1, ui_clock_face_main.header.h);
    // lv_image_set_scale(img1, 128);
    lv_obj_clear_flag(scr_watchface, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label1 = lv_label_create(scr_watchface);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_CLIP);     /*Break the long lines*/
    lv_label_set_text(label1, "watchface.");
    lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);

    lv_obj_add_event_cb(scr_watchface, (lv_event_cb_t)scr_watchface_cb, LV_EVENT_ALL, NULL);
}

