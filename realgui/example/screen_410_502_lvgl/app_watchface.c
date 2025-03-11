#include "lvgl.h"
#include "app_main.h"
#include "lv_img_dsc_list.h"
#include <time.h>
#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#if LVGL_USE_CJSON
extern char *cjson_content;
static cJSON *root;
#endif
static lv_obj_t *date_label;
static lv_obj_t *time_img_container;
static lv_obj_t *activity_arc;
static lv_obj_t *weather_card;
static lv_obj_t *temperature_container;
static lv_obj_t *compass_dial;
static lv_obj_t *weather_current, *weather_range;

static lv_timer_t *timer = NULL;

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

void time_update_cb(void)
{
    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo = localtime(&rawtime);

    lv_obj_t *parent = time_img_container;

    lv_obj_t *hour_decimal = lv_obj_get_child(parent, 0);
    lv_image_set_src(hour_decimal, text_num_array[timeinfo->tm_hour / 10]);
    lv_obj_t *hour_singel = lv_obj_get_sibling(hour_decimal, 1);
    lv_image_set_src(hour_singel, text_num_array[timeinfo->tm_hour % 10]);
    lv_obj_t *minute_decimal = lv_obj_get_sibling(hour_singel, 1);
    lv_image_set_src(minute_decimal, text_num_array[timeinfo->tm_min / 10]);
    lv_obj_t *minute_singel = lv_obj_get_sibling(minute_decimal, 1);
    lv_image_set_src(minute_singel, text_num_array[timeinfo->tm_min % 10]);

    char date_text_content[10];
    sprintf(date_text_content, "%s %d", day[timeinfo->tm_wday], timeinfo->tm_mday);
    lv_label_set_text(date_label, date_text_content);
}
#if LVGL_USE_CJSON
static void weather_icon_set(char *condition, lv_obj_t *obj)
{
    if (strcmp(condition, "Sunny") == 0)
    {
        lv_image_set_src(obj, &ui_weather_sunny);
    }
    else if (strcmp(condition, "Light rain") == 0)
    {
        lv_image_set_src(obj, &ui_weather_rain_s);
    }
    else if (strcmp(condition, "Showers") == 0)
    {
        lv_image_set_src(obj, &ui_weather_rain_s);
    }
    else if (strcmp(condition, "Moderate rain") == 0)
    {
        lv_image_set_src(obj, &ui_weather_rain_m);
    }
    else if (strcmp(condition, "Heavy rain") == 0)
    {
        lv_image_set_src(obj, &ui_weather_rain_l);
    }
    else if (strcmp(condition, "Cloudy") == 0)
    {
        lv_image_set_src(obj, &ui_weather_cloudy);
    }
    else // need to add more weather icon
    {
        lv_image_set_src(obj, &ui_weather_cloudy);
    }
}

static void weather_update_cb(void)
{
    lv_obj_t *parent = weather_card;
    // parse weather array
    cJSON *weather_array = cJSON_GetObjectItemCaseSensitive(root, "weather");
    if (cJSON_IsArray(weather_array))
    {
        cJSON *weather = cJSON_GetArrayItem(weather_array, 0);
        if (!weather)
        {
            LV_LOG("get weather_array unsuccessful\n");
        }
        else
        {
            cJSON *low = cJSON_GetObjectItemCaseSensitive(weather, "low");
            cJSON *high = cJSON_GetObjectItemCaseSensitive(weather, "high");
            cJSON *cur = cJSON_GetObjectItemCaseSensitive(weather, "current");

            char content_cur[5];
            sprintf(content_cur, "%d°", cur->valueint);
            lv_label_set_text(weather_current, content_cur);
            char content_range[15];
            sprintf(content_range, "H:%d° L:%d°", high->valueint, low->valueint);
            lv_label_set_text(weather_range, content_range);
            for (uint8_t i = 1; i <= 5; i++)
            {
                char key[15];
                sprintf(key, "condition_%d", i);
                cJSON *condition = cJSON_GetObjectItemCaseSensitive(weather, key);
                weather_icon_set(condition->valuestring, lv_obj_get_child(parent, i - 1));
            }
        }
    }

    // date
    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo = localtime(&rawtime);
    char weather_date_content[5];
    uint8_t index = timeinfo->tm_wday;
    for (uint8_t i = 0; i < 4; i++)
    {
        lv_obj_t *obj = lv_obj_get_child(parent, i + 6);
        sprintf(weather_date_content, "%s.", day[(index + 1 + i) % 7]);
        lv_label_set_text(obj, weather_date_content);
    }
}

void arc_activity_cb(void)
{
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

            lv_obj_t *arc_move = activity_arc;
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
}

static void temperature_update_cb()
{
    lv_obj_t *parent = temperature_container;
    // parse temperature array
    cJSON *weather_array = cJSON_GetObjectItemCaseSensitive(root, "weather");
    if (cJSON_IsArray(weather_array))
    {
        cJSON *weather = cJSON_GetArrayItem(weather_array, 0);
        if (!weather)
        {
            LV_LOG("get weather_array unsuccessful\n");
        }
        else
        {
            cJSON *low = cJSON_GetObjectItemCaseSensitive(weather, "low");
            cJSON *high = cJSON_GetObjectItemCaseSensitive(weather, "high");
            cJSON *cur = cJSON_GetObjectItemCaseSensitive(weather, "current");
            uint8_t low_val = low->valueint < 10 ? 10 : low->valueint;
            uint8_t high_val = high->valueint > 37 ? 37 : high->valueint;
            uint8_t cur_val = cur->valueint;

            char content_cur[5];
            sprintf(content_cur, "%d", cur->valueint);
            lv_obj_t *label_current = lv_obj_get_child(parent, 0);
            lv_label_set_text(label_current, content_cur);
            char content_low[5];
            sprintf(content_low, "%d", low->valueint);
            lv_obj_t *label_low = lv_obj_get_child(parent, 1);
            lv_label_set_text(label_low, content_low);
            char content_high[5];
            sprintf(content_high, "%d", high->valueint);
            lv_obj_t *label_high = lv_obj_get_child(parent, 2);
            lv_label_set_text(label_high, content_high);

            lv_obj_t *knob = lv_obj_get_child(parent, 4);
            float temp = ((float)cur_val - (float)low_val) / ((float)high_val - (float)low_val);
            float a0 = M_PI * 5.0f / 6.0f + temp * 4.0f / 3.0f * M_PI; // 5/6pi ~ 13/6pi CW
            float ax, ay;
            float r_arc = 50.0f;
            float width = 8.0f;
            uint8_t r_circle = 6;
            ax = r_arc + cosf(a0) * (r_arc - width / 2);
            ay = r_arc + sinf(a0) * (r_arc - width / 2);
            lv_obj_set_pos(knob, ax - r_circle, ay - r_circle);
        }
    }
}

static void compass_update_cb()
{
    lv_obj_t *parent = compass_dial;
    // parse compass array
    cJSON *compass_array = cJSON_GetObjectItemCaseSensitive(root, "compass");
    if (cJSON_IsArray(compass_array))
    {
        cJSON *comp = cJSON_GetArrayItem(compass_array, 0);
        if (!comp)
        {
            LV_LOG("get compass_array unsuccessful\n");
        }
        else
        {
            cJSON *degree = cJSON_GetObjectItemCaseSensitive(comp, "degree");
            uint16_t degree_val = degree->valueint;
            float x = 100 / 2;
            float y = 100 / 2;
            float r = 36; // radius
            // moving triangle
            float a0 = M_PI * degree_val / 360 * 2; // 0 ~ 2pi CW
            float ax, ay;
            ax = x + cosf(a0 + M_PI * 3 / 2) * r - 5;
            ay = y + sinf(a0 + M_PI * 3 / 2) * r - 5;

            lv_obj_t *compass_pointer = lv_obj_get_child(parent, 0);
            lv_obj_set_pos(compass_pointer, (int32_t)ax, (int32_t)ay);
            lv_image_set_rotation(compass_pointer, degree_val * 10);

            char degree_content[5], orien_content[5];
            lv_obj_t *compass_degree = lv_obj_get_child(parent, 1);
            lv_obj_t *compass_orien = lv_obj_get_child(parent, 2);
            sprintf(degree_content, "%d°", degree_val);
            lv_label_set_text(compass_degree, degree_content);

            uint16_t progress_compass = degree_val;
            if (progress_compass == 0)                                 {sprintf(orien_content, "N");}
            else if (progress_compass > 0 && progress_compass < 90)    {sprintf(orien_content, "NE");}
            else if (progress_compass == 90)                           {sprintf(orien_content, "E");}
            else if (progress_compass > 90 && progress_compass < 180)  {sprintf(orien_content, "SE");}
            else if (progress_compass == 180)                          {sprintf(orien_content, "S");}
            else if (progress_compass > 180 && progress_compass < 270) {sprintf(orien_content, "SW");}
            else if (progress_compass == 270)                          {sprintf(orien_content, "W");}
            else if (progress_compass > 270 && progress_compass < 360) {sprintf(orien_content, "NW");}
            lv_label_set_text(compass_orien, orien_content);
        }
    }
}
#endif
static void timer_cb(lv_timer_t *timer)
{
    if (!scr_watchface)
    {
        return;
    }
    time_update_cb();
#if LVGL_USE_CJSON
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
    arc_activity_cb();
    weather_update_cb();
    temperature_update_cb();
    compass_update_cb();
    // clear
    cJSON_Delete(root);
#endif
}
static void scr_watchface_cb(lv_event_t *event)
{
    lv_event_code_t event_code = lv_event_get_code(event);

    if (event_code == LV_EVENT_SCREEN_LOAD_START)
    {
        // left_Animation(ui_hour_group, 0);
        // right_Animation(ui_label_min, 0);
        // opa_on_Animation(&ui_weather_group_1, 300);
        // opa_on_Animation(ui_date_group, 500);
        // opa_on_Animation(&ui_weather_title_group_1, 400);
    }
    if (event_code == LV_EVENT_GESTURE)
    {
        switch (lv_indev_get_gesture_dir(lv_indev_get_act()))
        {
        case LV_DIR_LEFT:
            {
                lv_indev_wait_release(lv_indev_get_act());
                _ui_screen_change(&scr_right_curtain, &scr_watchface, LV_SCR_LOAD_ANIM_OVER_LEFT, 500, 0,
                                  &lv_right_curtain_init, 1);
                lv_timer_del(timer);
                break;
            }
        case LV_DIR_RIGHT:
            {
                lv_indev_wait_release(lv_indev_get_act());
                _ui_screen_change(&scr_left_curtain, &scr_watchface, LV_SCR_LOAD_ANIM_OVER_RIGHT, 500, 0,
                                  &lv_left_curtain_init, 1);
                lv_timer_del(timer);
                break;
            }
        case LV_DIR_TOP:
            {
                lv_indev_wait_release(lv_indev_get_act());
                _ui_screen_change(&scr_down_curtain, &scr_watchface, LV_SCR_LOAD_ANIM_OVER_TOP, 500, 0,
                                  &lv_down_curtain_init, 1);
                lv_timer_del(timer);
                break;
            }
        case LV_DIR_BOTTOM:
            {
                lv_indev_wait_release(lv_indev_get_act());
                _ui_screen_change(&scr_up_curtain, &scr_watchface, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 500, 0,
                                  &lv_up_curtain_init, 1);
                lv_timer_del(timer);
                break;
            }
        default:
            break;
        }
    }
}

static void heartrate_cb(lv_event_t *event)
{
    LV_LOG("change to heartrate screen\n");
    return;
}

void lv_watchface_init(void)
{
    // scr_watchface = lv_obj_create(NULL);
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
        lv_obj_set_pos(date_label, 270, 50);
        lv_obj_set_style_text_color(date_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(date_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(date_label, &SourceHanSansSC_size32_bits1_font,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);

        time_img_container = lv_obj_create(scr_watchface);
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
    }

    // activity icon
    {
        uint16_t start_angel = 270;

        lv_obj_t *arc_1 = lv_arc_create(scr_watchface);
        activity_arc = arc_1;
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
        lv_arc_set_end_angle(arc_1, start_angel + (uint16_t)(360 * 10000 / 20000)); // cap 20000 steps

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
        lv_arc_set_end_angle(arc_2, start_angel + (uint16_t)(360 * 30 / 60)); // cap 60 min

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
        lv_arc_set_end_angle(arc_3, start_angel + (uint16_t)(360 * 15 / 30)); // cap 30 times
    }

    // weather condition
    {
        weather_card = lv_image_create(scr_watchface);
        lv_image_set_src(weather_card, &ui_clock_card_weather);
        lv_obj_set_pos(weather_card, 37, 185);
        lv_obj_clear_flag(weather_card, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *img = lv_image_create(weather_card);
        lv_image_set_src(img, &ui_weather_cloudy);
        lv_obj_set_pos(img, 28, 73);
        img = lv_image_create(weather_card);
        lv_image_set_src(img, &ui_weather_rain_l);
        lv_obj_set_pos(img, 89, 73);
        img = lv_image_create(weather_card);
        lv_image_set_src(img, &ui_weather_rain_m);
        lv_obj_set_pos(img, 150, 73);
        img = lv_image_create(weather_card);
        lv_image_set_src(img, &ui_weather_rain_s);
        lv_obj_set_pos(img, 208, 73);
        img = lv_image_create(weather_card);
        lv_image_set_src(img, &ui_weather_sunny);
        lv_obj_set_pos(img, 272, 73);

        {
            lv_obj_t *weather_date = lv_label_create(weather_card);
            lv_label_set_text(weather_date, "TODAY");
            lv_obj_set_pos(weather_date, 24, 110);
            lv_obj_set_style_text_color(weather_date, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(weather_date, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(weather_date, &SourceHanSansSC_size12_bits1_font,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
            weather_date = lv_label_create(weather_card);
            lv_label_set_text(weather_date, "MON.");
            lv_obj_set_pos(weather_date, 88, 110);
            lv_obj_set_style_text_color(weather_date, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(weather_date, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(weather_date, &SourceHanSansSC_size12_bits1_font,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
            weather_date = lv_label_create(weather_card);
            lv_label_set_text(weather_date, "TUE.");
            lv_obj_set_pos(weather_date, 150, 110);
            lv_obj_set_style_text_color(weather_date, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(weather_date, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(weather_date, &SourceHanSansSC_size12_bits1_font,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
            weather_date = lv_label_create(weather_card);
            lv_label_set_text(weather_date, "WED.");
            lv_obj_set_pos(weather_date, 210, 110);
            lv_obj_set_style_text_color(weather_date, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(weather_date, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(weather_date, &SourceHanSansSC_size12_bits1_font,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
            weather_date = lv_label_create(weather_card);
            lv_label_set_text(weather_date, "THU.");
            lv_obj_set_pos(weather_date, 273, 110);
            lv_obj_set_style_text_color(weather_date, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(weather_date, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(weather_date, &SourceHanSansSC_size12_bits1_font,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        weather_current = lv_label_create(weather_card);
        lv_label_set_text(weather_current, "22°");
        lv_obj_set_pos(weather_current, 37, 7);
        lv_obj_set_style_text_color(weather_current, lv_color_make(124, 199, 243),
                                    LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(weather_current, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(weather_current, &SourceHanSansSC_size24_bits1_font,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
        weather_range = lv_label_create(weather_card);
        lv_label_set_text(weather_range, "H:37° L:18°");
        lv_obj_set_pos(weather_range, 80, 7);
        lv_obj_set_style_text_color(weather_range, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(weather_range, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(weather_range, &SourceHanSansSC_size24_bits1_font,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    // temperature icon
    {
        lv_obj_t *container = lv_obj_create(scr_watchface);
        temperature_container = container;
        lv_obj_set_pos(container, 15, 335);
        lv_obj_set_size(container, 120, 120);
        lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_border_width(container, 0, 0);
        lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);

        // text
        lv_obj_t *temperature_current = lv_label_create(container);
        lv_label_set_text(temperature_current, "27");
        // lv_obj_set_pos(temperature_current, 24, 26);
        lv_obj_align(temperature_current, LV_ALIGN_CENTER, 5, -5);
        lv_obj_set_style_text_color(temperature_current, lv_color_hex(0xFFFFFF),
                                    LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(temperature_current, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(temperature_current, &SourceHanSansSC_size32_bits1_font,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_t *temperature_low = lv_label_create(container);
        lv_label_set_text(temperature_low, "18");
        lv_obj_set_pos(temperature_low, 14, 72);
        lv_obj_set_style_text_color(temperature_low, lv_color_hex(0xFFFFFF),
                                    LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(temperature_low, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(temperature_low, &SourceHanSansSC_size24_bits1_font,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_t *temperature_high = lv_label_create(container);
        lv_label_set_text(temperature_high, "37");
        lv_obj_set_pos(temperature_high, 57, 72);
        lv_obj_set_style_text_color(temperature_high, lv_color_hex(0xFFFFFF),
                                    LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(temperature_high, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(temperature_high, &SourceHanSansSC_size24_bits1_font,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);

        // arc
        {
            lv_obj_t *arc = lv_arc_create(container);
            lv_obj_remove_style(arc, NULL, LV_PART_MAIN);
            uint8_t radius = 50;
            lv_obj_set_pos(arc, 0, 0);
            // lv_obj_align(arc, LV_ALIGN_TOP_MID, 0, 0);
            lv_obj_set_size(arc, radius * 2, radius * 2);
            lv_arc_set_angles(arc, 150, 390);
            // lv_arc_set_bg_angles(arc, 150, 390);
            lv_obj_set_style_arc_width(arc, 8, LV_PART_INDICATOR);
            // lv_obj_set_style_arc_width(arc, 8, LV_PART_MAIN);
            lv_obj_set_style_arc_color(arc, lv_color_make(230, 67, 79), LV_PART_INDICATOR);
            lv_obj_remove_style(arc, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
            lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/

            lv_obj_t *knob = lv_obj_create(container);
            lv_obj_set_size(knob, 12, 12);
            lv_obj_set_pos(knob, radius - 6, -2);
            lv_obj_set_style_border_color(knob, lv_color_make(0, 0, 0), 0);
            lv_obj_set_style_border_width(knob, 2, 0);
            lv_obj_set_style_radius(knob, 6, 0);
            lv_obj_set_style_bg_color(knob, lv_color_make(255, 0, 0), 0);
        }
    }

    // compass icon
    {
        compass_dial = lv_image_create(scr_watchface);
        lv_image_set_src(compass_dial, &ui_clock_compass_dial_icon);
        lv_obj_set_pos(compass_dial, 154, 348);
        lv_obj_set_size(compass_dial, 100, 100);
        lv_obj_clear_flag(compass_dial, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_border_width(compass_dial, 0, 0);
        lv_obj_set_style_bg_opa(compass_dial, LV_OPA_TRANSP, 0);

        uint16_t degree_val = 90;
        float x = 100 / 2;
        float y = 100 / 2;
        float r = 36; // radius
        // move triangle
        float a0 = M_PI * degree_val / 360 * 2; // 0 ~ 2pi CW
        float ax, ay;
        ax = x + cosf(a0 + M_PI * 3 / 2) * r - 5;
        ay = y + sinf(a0 + M_PI * 3 / 2) * r - 5;
        lv_obj_t *compass_pointer = lv_image_create(compass_dial);
        lv_image_set_src(compass_pointer, &ui_clock_compass_pointer_icon);
        lv_obj_set_pos(compass_pointer, (int32_t)ax, (int32_t)ay);
        lv_image_set_rotation(compass_pointer, degree_val * 10);

        lv_obj_t *compass_degree = lv_label_create(compass_dial);
        lv_label_set_text(compass_degree, "90°");
        lv_obj_align(compass_degree, LV_ALIGN_TOP_MID, 0, 23);
        lv_obj_set_style_text_color(compass_dial, lv_color_make(254, 106, 26),
                                    LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(compass_dial, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(compass_dial, &SourceHanSansSC_size24_bits1_font,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_t *compass_orien = lv_label_create(compass_dial);
        lv_label_set_text(compass_orien, "E");
        lv_obj_align(compass_orien, LV_ALIGN_TOP_MID, 0, 50);
        lv_obj_set_style_text_color(compass_orien, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(compass_orien, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(compass_orien, &SourceHanSansSC_size24_bits1_font,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    // heartrate icon
    {
        lv_obj_t *heartrate = lv_image_create(scr_watchface);
        lv_image_set_src(heartrate, &ui_clock_heartrate_icon);
        lv_obj_set_pos(heartrate, 276, 348);
        lv_obj_set_size(heartrate, 100, 100);
        lv_obj_clear_flag(heartrate, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(heartrate, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_border_width(heartrate, 0, 0);
        lv_obj_set_style_bg_opa(heartrate, LV_OPA_TRANSP, 0);
        lv_obj_add_event_cb(heartrate, (lv_event_cb_t)heartrate_cb, LV_EVENT_CLICKED, NULL);
    }
    // lv_obj_add_event_cb(scr_watchface, (lv_event_cb_t)scr_watchface_cb, LV_EVENT_ALL, NULL);
    timer = lv_timer_create(timer_cb, 2000, scr_watchface);
    lv_timer_set_repeat_count(timer, -1);
}

