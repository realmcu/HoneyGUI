#include "lvgl.h"
#include "app_main.h"
#include "lv_custom_tile_slide.h"


#define EFFECT_NUM 6
static lv_style_t style_effect;
static lv_style_t style_effect_chk;
static lv_style_t style_font;
static uint32_t active_index = 0;
static const char *effet_name_list[EFFECT_NUM] =
{
    "CLASSIC",
    "FADE",
    "SCALE",
    "SCALE_FADE",
    "CUBE_ROTATION",
    "SPIRAL_NOTEBOOK",
};

static SLIDE_EFFECT effect_list[EFFECT_NUM] =
{
    CLASSIC,
    FADE,
    SCALE,
    SCALE_FADE,
    CUBE_ROTATION,
    SPIRAL_NOTEBOOK
};
extern SLIDE_EFFECT global_slide;
static void effect_event_handler(lv_event_t *e)
{
    uint32_t *active_id = lv_event_get_user_data(e);
    lv_obj_t *cont = lv_event_get_current_target(e);
    lv_obj_t *act_cb = lv_event_get_target(e);
    lv_obj_t *old_cb = lv_obj_get_child(cont, *active_id);

    /*Do nothing if the container was clicked*/
    if (act_cb == cont) { return; }

    lv_obj_remove_state(old_cb, LV_STATE_CHECKED);   /*Uncheck the previous effect button*/
    lv_obj_add_state(act_cb, LV_STATE_CHECKED);     /*Uncheck the current effect button*/

    *active_id = lv_obj_get_index(act_cb);
    global_slide = effect_list[*active_id];
}

void create_slide_effect_checkbox(lv_obj_t *parent)
{
    lv_style_init(&style_effect);
    lv_style_set_radius(&style_effect, LV_RADIUS_CIRCLE);

    lv_style_init(&style_effect_chk);
    lv_style_set_bg_image_src(&style_effect_chk, NULL);

    lv_style_init(&style_font);
    lv_style_set_text_color(&style_font, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_text_font(&style_font, &lv_font_montserrat_26);

    lv_obj_t *cont1 = lv_obj_create(parent);
    lv_obj_set_flex_flow(cont1, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont1, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_size(cont1, LV_PCT(100), LV_PCT(100));
    lv_obj_center(cont1);
    lv_obj_add_event_cb(cont1, effect_event_handler, LV_EVENT_CLICKED, &active_index);
    lv_obj_set_style_bg_color(cont1, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_bg_opa(cont1, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cont1, 0, 0);

    for (uint32_t i = 0; i < EFFECT_NUM; i++)
    {
        lv_obj_t *obj = lv_checkbox_create(cont1);
        lv_checkbox_set_text(obj, effet_name_list[i]);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);

        lv_obj_add_style(obj, &style_effect, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &style_effect_chk, LV_PART_INDICATOR | LV_STATE_CHECKED);
        lv_obj_add_style(obj, &style_font, LV_PART_MAIN | LV_STATE_DEFAULT);
#if !LV_DRAW_TRANSFORM_USE_MATRIX
        if (effect_list[i] >= CUBE_ROTATION)
        {
            lv_obj_add_state(obj, LV_STATE_DISABLED);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
        }
#endif
    }
    for (uint32_t i = 0; i < lv_obj_get_child_count(cont1); i++)
    {
        if (global_slide == effect_list[i])
        {
            lv_obj_add_state(lv_obj_get_child(cont1, i), LV_STATE_CHECKED);
            active_index = i;
        }
    }
}

void lv_left_curtain_init(void)
{
    // scr_left_curtain = lv_obj_create(NULL);

    // lv_obj_set_style_bg_color(scr_left_curtain, lv_color_make(0, 0, 0), 0);
    // lv_obj_set_style_bg_opa(scr_left_curtain, LV_OPA_COVER, 0);

    LV_IMG_DECLARE(w3w);
    lv_obj_t *img1 = lv_image_create(scr_left_curtain);
    lv_image_set_src(img1, &w3w);
    lv_obj_center(img1);

    create_slide_effect_checkbox(scr_left_curtain);

    // lv_obj_add_event_cb(scr_left_curtain, (lv_event_cb_t)tile_left_cb, LV_EVENT_ALL, NULL);
}

