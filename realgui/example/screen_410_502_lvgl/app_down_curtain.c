#include "lvgl.h"
#include "app_main.h"

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
static lv_obj_t *img_phone, *img_nobother, *img_mute;
// void scr_down_curtain_cb(lv_event_t *e)
// {
//     lv_event_code_t event_code = lv_event_get_code(e);

//     if (event_code == LV_EVENT_SCREEN_LOAD_START)
//     {
//         // left_Animation(ui_hour_group, 0);
//         // right_Animation(ui_label_min, 0);
//         // opa_on_Animation(ui_weather_group_1, 300);
//         // opa_on_Animation(ui_date_group, 500);
//         // opa_on_Animation(ui_weather_title_group_1, 400);
//     }
//     if (event_code == LV_EVENT_GESTURE &&
//         lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_BOTTOM)
//     {
//         lv_indev_wait_release(lv_indev_get_act());
//         _ui_screen_change(&scr_watchface, &scr_down_curtain, LV_SCR_LOAD_ANIM_OUT_BOTTOM, 500, 0,
//                           &lv_watchface_init, 1);
//     }
// }

void button_lte_event_cb(lv_event_t *e)
{
    lv_obj_t *img = lv_event_get_target(e);
    sw_state.sw_1 ^= 1;
    // Toggle between checked and default state
    if (sw_state.sw_1)
    {
        lv_image_set_src(img, &control_lte_on);
    }
    else
    {
        lv_image_set_src(img, &control_lte_off);
    }
}

void button_wifi_event_cb(lv_event_t *e)
{
    lv_obj_t *img = lv_event_get_target(e);
    sw_state.sw_2 ^= 1;
    // Toggle between checked and default state
    if (sw_state.sw_2)
    {
        lv_image_set_src(img, &control_wifi_on);
    }
    else
    {
        lv_image_set_src(img, &control_wifi_off);
    }
}

void button_phone_event_cb(lv_event_t *e)
{
    lv_obj_t *img = lv_event_get_target(e);
    sw_state.sw_3 ^= 1;
    // Toggle between checked and default state
    if (sw_state.sw_3)
    {
        lv_image_set_src(img, &control_phone_on);
        lv_image_set_src(img_phone, &phone_on_icon);
    }
    else
    {
        lv_image_set_src(img, &control_phone_off);
        lv_image_set_src(img_phone, &phone_off_icon);
    }
}

void button_mute_event_cb(lv_event_t *e)
{
    lv_obj_t *img = lv_event_get_target(e);
    sw_state.sw_4 ^= 1;
    // Toggle between checked and default state
    if (sw_state.sw_4)
    {
        lv_image_set_src(img, &control_mute_on);
        lv_image_set_src(img_mute, &mute_on_icon);
    }
    else
    {
        lv_image_set_src(img, &control_mute_off);
        lv_image_set_src(img_mute, &mute_off_icon);
    }
}

void button_nobother_event_cb(lv_event_t *e)
{
    lv_obj_t *img = lv_event_get_target(e);
    sw_state.sw_5 ^= 1;
    // Toggle between checked and default state
    if (sw_state.sw_5)
    {
        lv_image_set_src(img, &control_nobother_on);
        lv_image_set_src(img_nobother, &nobother_on_icon);
    }
    else
    {
        lv_image_set_src(img, &control_nobother_off);
        lv_image_set_src(img_nobother, &nobother_off_icon);
    }
}

void lv_down_curtain_init(void)
{
    // scr_down_curtain = lv_obj_create(NULL);
    // lv_obj_add_event_cb(scr_down_curtain, (lv_event_cb_t)scr_down_curtain_cb, LV_EVENT_ALL, NULL);

    lv_obj_set_style_bg_color(scr_down_curtain, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_bg_opa(scr_down_curtain, LV_OPA_COVER, 0);
    lv_obj_set_scrollbar_mode(scr_down_curtain, LV_SCROLLBAR_MODE_OFF);

    // capsule on top
    lv_obj_t *capsule = lv_obj_create(scr_down_curtain);
    lv_obj_set_style_bg_color(capsule, lv_color_make(196, 196, 196), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(capsule, 20, LV_PART_MAIN);
    lv_obj_set_size(capsule, 138, 47);
    lv_obj_set_pos(capsule, 136, 20);
    lv_obj_clear_flag(capsule, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(capsule, 0, LV_PART_MAIN); // No border
    lv_obj_set_style_pad_all(capsule, 0, 0); // Clear all padding

    img_phone = lv_image_create(capsule);
    lv_image_set_src(img_phone, &phone_off_icon);
    lv_obj_set_pos(img_phone, 15, 9);
    img_nobother = lv_image_create(capsule);
    lv_image_set_src(img_nobother, &nobother_off_icon);
    lv_obj_set_pos(img_nobother, 56, 9);
    img_mute = lv_image_create(capsule);
    lv_image_set_src(img_mute, &mute_off_icon);
    lv_obj_set_pos(img_mute, 98, 9);

    // button
    lv_obj_t *button_lte = lv_image_create(scr_down_curtain);
    lv_image_set_src(button_lte, &control_lte_off);
    lv_obj_set_pos(button_lte, 20, 100);
    lv_obj_add_flag(button_lte, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(button_lte, button_lte_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *button_wifi = lv_image_create(scr_down_curtain);
    lv_image_set_src(button_wifi, &control_wifi_off);
    lv_obj_set_pos(button_wifi, 207, 100);
    lv_obj_add_flag(button_wifi, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(button_wifi, button_wifi_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *button_phone = lv_image_create(scr_down_curtain);
    lv_image_set_src(button_phone, &control_phone_off);
    lv_obj_set_pos(button_phone, 20, 100 + 125 * 1);
    lv_obj_add_flag(button_phone, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(button_phone, button_phone_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *button_mute = lv_image_create(scr_down_curtain);
    lv_image_set_src(button_mute, &control_mute_off);
    lv_obj_set_pos(button_mute, 20, 100 + 125 * 2);
    lv_obj_add_flag(button_mute, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(button_mute, button_mute_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *button_nobother = lv_image_create(scr_down_curtain);
    lv_image_set_src(button_nobother, &control_nobother_off);
    lv_obj_set_pos(button_nobother, 207, 100 + 125 * 2);
    lv_obj_add_flag(button_nobother, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(button_nobother, button_nobother_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *button_charge = lv_image_create(scr_down_curtain);
    lv_image_set_src(button_charge, &control_pad);
    lv_obj_set_pos(button_charge, 207, 100 + 125 * 1);
    lv_obj_t *charge_num = lv_image_create(button_charge);
    lv_image_set_src(charge_num, &ui_text_9);
    lv_obj_set_pos(charge_num, 35, 40);
    lv_obj_set_style_transform_scale(charge_num, 256 * 0.8f, 256 * 0.8f);
    charge_num = lv_image_create(button_charge);
    lv_image_set_src(charge_num, &ui_text_6);
    lv_obj_set_pos(charge_num, 65, 40);
    lv_obj_set_style_transform_scale(charge_num, 256 * 0.8f, 256 * 0.8f);
    charge_num = lv_image_create(button_charge);
    lv_image_set_src(charge_num, &ui_text_percent);
    lv_obj_set_pos(charge_num, 95, 40);
    lv_obj_set_style_transform_scale(charge_num, 256 * 0.8f, 256 * 0.8f);
}

