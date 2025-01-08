#include "lvgl.h"
#include "app_main.h"
#include "lv_img_dsc_list.h"
#include <time.h>
#include "cJSON.h"
#include "stdio.h"

extern char *cjson_content;

lv_obj_t *date_label;

lv_image_dsc_t const *text_num_array[] =
{
    &ui_text_0,
    &ui_text_1,
    &ui_text_2,
    &ui_text_3,
    &ui_text_4,
    &ui_text_5,
    &ui_text_6,
    &ui_text_7,
    &ui_text_8,
    &ui_text_9,
    &ui_text_colon,
};

char *day[7] =
{
    "SUN",
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT"
};

void time_update_cb(lv_timer_t *timer)
{
    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo = localtime(&rawtime);

    lv_obj_t *parent = (lv_obj_t *)lv_timer_get_user_data(timer);

    lv_obj_t *hour_decimal = lv_obj_get_child(parent, 0);
    lv_image_set_src(hour_decimal, text_num_array[timeinfo->tm_hour / 10]);
    lv_obj_t *hour_singel = lv_obj_get_sibling(hour_decimal, 1);
    lv_image_set_src(hour_singel, text_num_array[timeinfo->tm_hour % 10]);
    lv_obj_t *minute_decimal = lv_obj_get_sibling(hour_singel, 1);
    lv_image_set_src(minute_decimal, text_num_array[timeinfo->tm_min / 10]);
    lv_obj_t *minute_singel = lv_obj_get_sibling(minute_decimal, 1);
    lv_image_set_src(minute_singel, text_num_array[timeinfo->tm_min % 10]);

    char date_text_content[10] = "SUN 0";
    sprintf(date_text_content, "%s %d", day[timeinfo->tm_wday], timeinfo->tm_mday);
    lv_label_set_text(date_label, date_text_content);
}

void arc_activity_cb(lv_timer_t *timer)
{
    cJSON *root;
    if (!cjson_content)
    {
        return;
    }
    else
    {
        root = cJSON_Parse(cjson_content);
        if (!root)
        {
            LV_LOG("Error parsing JSON!\r\n");
            return;
        }
    }
    // parse activity array
    cJSON *activity_array = cJSON_GetObjectItemCaseSensitive(root, "activity");
    if (cJSON_IsArray(activity_array))
    {
        cJSON *act = cJSON_GetArrayItem(activity_array, 0);
        if (!act)
        {
            LV_LOG("get activity_array unsuccessful\n");
        }
        else
        {
            cJSON *move = cJSON_GetObjectItemCaseSensitive(act, "move");
            cJSON *ex = cJSON_GetObjectItemCaseSensitive(act, "exercise");
            cJSON *stand = cJSON_GetObjectItemCaseSensitive(act, "stand");

            lv_obj_t *arc_move = (lv_obj_t *)lv_timer_get_user_data(timer);
            lv_obj_t *arc_ex = lv_obj_get_sibling(arc_move, 1);
            lv_obj_t *arc_stand = lv_obj_get_sibling(arc_ex, 1);
            uint16_t start_angel = 270;
            lv_arc_set_end_angle(arc_move,
                                 start_angel + (uint16_t)(360 * move->valueint / 20000)); // cap 20000 steps
            lv_arc_set_end_angle(arc_ex, start_angel + (uint16_t)(360 * ex->valueint / 60)); // cap 60 min
            lv_arc_set_end_angle(arc_stand,
                                 start_angel + (uint16_t)(360 * stand->valueint / 30)); // cap 30 times
        }
    }
    // clear
    cJSON_Delete(root);
}

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

    lv_obj_set_style_bg_color(scr_watchface, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_bg_opa(scr_watchface, LV_OPA_COVER, 0);
    lv_obj_clear_flag(scr_watchface, LV_OBJ_FLAG_SCROLLABLE);

    // extern const lv_image_dsc_t ui_clock_face_main;
    // lv_obj_t *img1 = lv_image_create(scr_watchface);
    // lv_image_set_src(img1, &ui_clock_face_main);
    // lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);

    // date_label & time text
    {
        date_label = lv_label_create(scr_watchface);
        lv_label_set_text(date_label, "SUN 0");
        lv_obj_set_pos(date_label, 250, 33);
        lv_obj_set_style_text_color(date_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(date_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(date_label, &SourceHanSansSC_size48_bits1_font,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t *time_img_container = lv_obj_create(scr_watchface);
        lv_obj_set_pos(time_img_container, 211, 88);
        lv_obj_set_size(time_img_container, 200, 200);
        lv_obj_clear_flag(time_img_container, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_border_width(time_img_container, 0, 0);
        lv_obj_set_style_bg_opa(time_img_container, LV_OPA_TRANSP, 0);

        lv_obj_t *hour_decimal = lv_image_create(time_img_container);
        lv_image_set_src(hour_decimal, text_num_array[0]);
        lv_obj_set_pos(hour_decimal, 0, 0);
        lv_obj_t *hour_single = lv_image_create(time_img_container);
        lv_image_set_src(hour_single, text_num_array[0]);
        lv_obj_set_pos(hour_single, 35, 0);
        lv_obj_t *minute_decimal = lv_image_create(time_img_container);
        lv_image_set_src(minute_decimal, text_num_array[0]);
        lv_obj_set_pos(minute_decimal, 92, 0);
        lv_obj_t *minute_singel = lv_image_create(time_img_container);
        lv_image_set_src(minute_singel, text_num_array[0]);
        lv_obj_set_pos(minute_singel, 127, 0);

        lv_obj_t *colon = lv_image_create(time_img_container);
        lv_image_set_src(colon, text_num_array[10]);
        lv_obj_set_pos(colon, 75, 5);

        lv_timer_t *timer = lv_timer_create(time_update_cb, 3000, time_img_container);
        lv_timer_set_repeat_count(timer, -1);
    }

    // activity icon
    {
        lv_obj_t *arc_1 = lv_arc_create(scr_watchface);
        uint8_t radius = 50;
        lv_obj_set_pos(arc_1, 87 - radius, 128 - radius);
        lv_obj_set_size(arc_1, radius * 2, radius * 2);
        lv_arc_set_start_angle(arc_1, 270);
        lv_arc_set_bg_angles(arc_1, 0, 360);
        lv_obj_set_style_arc_width(arc_1, 8, LV_PART_INDICATOR);
        lv_obj_set_style_arc_width(arc_1, 8, LV_PART_MAIN);
        lv_obj_set_style_arc_color(arc_1, lv_color_make(230, 67, 79), LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(arc_1, lv_color_make(58, 23, 29), LV_PART_MAIN);
        lv_obj_remove_style(arc_1, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
        lv_obj_remove_flag(arc_1, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/

        lv_obj_t *arc_2 = lv_arc_create(scr_watchface);
        radius = 38;
        lv_obj_set_pos(arc_2, 87 - radius, 128 - radius);
        lv_obj_set_size(arc_2, radius * 2, radius * 2);
        lv_arc_set_start_angle(arc_2, 270);
        lv_arc_set_bg_angles(arc_2, 0, 360);
        lv_obj_set_style_arc_width(arc_2, 8, LV_PART_INDICATOR);
        lv_obj_set_style_arc_width(arc_2, 8, LV_PART_MAIN);
        lv_obj_set_style_arc_color(arc_2, lv_color_make(186, 253, 79), LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(arc_2, lv_color_make(30, 55, 25), LV_PART_MAIN);
        lv_obj_remove_style(arc_2, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
        lv_obj_remove_flag(arc_2, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/

        lv_obj_t *arc_3 = lv_arc_create(scr_watchface);
        radius = 26;
        lv_obj_set_pos(arc_3, 87 - radius, 128 - radius);
        lv_obj_set_size(arc_3, radius * 2, radius * 2);
        lv_arc_set_start_angle(arc_3, 270);
        lv_arc_set_bg_angles(arc_3, 0, 360);
        lv_obj_set_style_arc_width(arc_3, 8, LV_PART_INDICATOR);
        lv_obj_set_style_arc_width(arc_3, 8, LV_PART_MAIN);
        lv_obj_set_style_arc_color(arc_3, lv_color_make(117, 230, 229), LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(arc_3, lv_color_make(22, 50, 47), LV_PART_MAIN);
        lv_obj_remove_style(arc_3, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
        lv_obj_remove_flag(arc_3, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/

        lv_timer_t *timer = lv_timer_create(arc_activity_cb, 2000, arc_1);
        lv_timer_set_repeat_count(timer, -1);
    }

    lv_obj_add_event_cb(scr_watchface, (lv_event_cb_t)scr_watchface_cb, LV_EVENT_ALL, NULL);
}

