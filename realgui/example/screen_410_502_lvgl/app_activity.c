#include "lvgl.h"
#include "app_main.h"

#if LVGL_USE_CJSON
#include "cJSON.h"
#endif

static lv_obj_t *arc_container, *label_move, *label_ex, *label_stand;

static void arc_anim_cb(void *obj, int32_t value)
{
    lv_arc_set_end_angle((lv_obj_t *)obj, value);
}

void update_arc_activity(void)
{
    const uint16_t start_angle = 270;
    lv_anim_t a;

    lv_obj_t *arc_move = lv_obj_get_child(arc_container, 0);
    lv_obj_t *arc_ex = lv_obj_get_child(arc_container, 1);
    lv_obj_t *arc_stand = lv_obj_get_child(arc_container, 2);
    uint16_t move_value = 10000;
    uint16_t ex_value = 30;
    uint16_t stand_value = 15;
#if LVGL_USE_CJSON
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

            move_value = move->valueint;
            ex_value = ex->valueint;
            stand_value = stand->valueint;
        }
    }
#endif
    lv_anim_init(&a);
    lv_anim_set_var(&a, arc_move);
    lv_anim_set_exec_cb(&a, arc_anim_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_set_values(&a, start_angle, start_angle + 360 * move_value / 20000);
    lv_anim_set_duration(&a, 1000);
    lv_anim_start(&a);

    lv_anim_init(&a);
    lv_anim_set_var(&a, arc_ex);
    lv_anim_set_exec_cb(&a, arc_anim_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_set_values(&a, start_angle, start_angle + 360 * ex_value / 60);
    lv_anim_set_duration(&a, 1000);
    lv_anim_set_delay(&a, 0);
    lv_anim_start(&a);

    lv_anim_init(&a);
    lv_anim_set_var(&a, arc_stand);
    lv_anim_set_exec_cb(&a, arc_anim_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_set_values(&a, start_angle, start_angle + 360 * stand_value / 30);
    lv_anim_set_duration(&a, 1000);
    lv_anim_set_delay(&a, 0);
    lv_anim_start(&a);

    char content[30];
    sprintf(content, "Move: %d/20000steps", move_value);
    lv_label_set_text(label_move, content);
    sprintf(content, "Exercise: %d/60min", ex_value);
    lv_label_set_text(label_ex, content);
    sprintf(content, "Stand: %d/30times", stand_value);
    lv_label_set_text(label_stand, content);
}

void app_activity(lv_obj_t *parent)
{
    const uint16_t start_angle = 90;
    const uint16_t arc_x = 130, arc_y = 150, width = 16;

    arc_container = lv_obj_create(parent);
    lv_obj_remove_style_all(arc_container);
    lv_obj_set_pos(arc_container, 0, 0);
    lv_obj_set_size(arc_container, 300, 300);
    lv_obj_clear_flag(arc_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(arc_container, 0, 0);
    lv_obj_set_style_bg_opa(arc_container, LV_OPA_TRANSP, 0);

    lv_obj_t *arc_1 = lv_arc_create(arc_container);
    uint8_t radius = 100;
    lv_obj_set_pos(arc_1, arc_x - radius, arc_y - radius);
    lv_obj_set_size(arc_1, radius * 2, radius * 2);
    lv_arc_set_start_angle(arc_1, 270);
    lv_arc_set_bg_angles(arc_1, 0, 360);
    lv_obj_set_style_arc_width(arc_1, width, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc_1, width, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_1, lv_color_make(230, 67, 79), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_1, lv_color_make(58, 23, 29), LV_PART_MAIN);
    lv_obj_remove_style(arc_1, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
    lv_obj_remove_flag(arc_1, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
    // lv_arc_set_end_angle(arc_1, start_angle + (uint16_t)(360 * 10000 / 20000)); // cap 20000 steps

    lv_obj_t *arc_2 = lv_arc_create(arc_container);
    radius = 76;
    lv_obj_set_pos(arc_2, arc_x - radius, arc_y - radius);
    lv_obj_set_size(arc_2, radius * 2, radius * 2);
    lv_arc_set_start_angle(arc_2, 270);
    lv_arc_set_bg_angles(arc_2, 0, 360);
    lv_obj_set_style_arc_width(arc_2, width, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc_2, width, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_2, lv_color_make(186, 253, 79), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_2, lv_color_make(30, 55, 25), LV_PART_MAIN);
    lv_obj_remove_style(arc_2, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
    lv_obj_remove_flag(arc_2, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
    // lv_arc_set_end_angle(arc_2, start_angle + (uint16_t)(360 * 30 / 60)); // cap 60 min

    lv_obj_t *arc_3 = lv_arc_create(arc_container);
    radius = 52;
    lv_obj_set_pos(arc_3, arc_x - radius, arc_y - radius);
    lv_obj_set_size(arc_3, radius * 2, radius * 2);
    lv_arc_set_start_angle(arc_3, 270);
    lv_arc_set_bg_angles(arc_3, 0, 360);
    lv_obj_set_style_arc_width(arc_3, width, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc_3, width, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_3, lv_color_make(117, 230, 229), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_3, lv_color_make(22, 50, 47), LV_PART_MAIN);
    lv_obj_remove_style(arc_3, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
    lv_obj_remove_flag(arc_3, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
    // lv_arc_set_end_angle(arc_3, start_angle + (uint16_t)(360 * 15 / 30)); // cap 30 times

    // text
    label_move = lv_label_create(parent);
    lv_obj_remove_style_all(label_move);
    lv_obj_set_pos(label_move, 130, 300);
    lv_label_set_text(label_move, "Move: 0/20000steps");
    lv_obj_set_style_text_color(label_move, lv_color_make(230, 67, 79),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label_move, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label_move, &SourceHanSansSC_size24_bits1_font,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    label_ex = lv_label_create(parent);
    lv_obj_remove_style_all(label_ex);
    lv_obj_set_pos(label_ex, 130, 350);
    lv_label_set_text(label_ex, "Exercise: 0/60min");
    lv_obj_set_style_text_color(label_ex, lv_color_make(186, 253, 79), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label_ex, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label_ex, &SourceHanSansSC_size24_bits1_font,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    label_stand = lv_label_create(parent);
    lv_obj_remove_style_all(label_stand);
    lv_obj_set_pos(label_stand, 130, 400);
    lv_label_set_text(label_stand, "Stand: 0/30times");
    lv_obj_set_style_text_color(label_stand, lv_color_make(117, 230, 229),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label_stand, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label_stand, &SourceHanSansSC_size24_bits1_font,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void tileview_event_cb(lv_event_t *e)
{
    lv_obj_t *act_tile = lv_tileview_get_tile_active(tileview);
    static bool flag = false; //prevent enter animate twice
    if (act_tile == tile_right_2 && !flag)
    {
        update_arc_activity();
        flag = true;
    }
    else
    {
        flag = false;
    }
}

void lv_right_curtain_2_init(void)
{
    app_activity(scr_right_curtain_2);
    lv_obj_add_event_cb(tileview, tileview_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

