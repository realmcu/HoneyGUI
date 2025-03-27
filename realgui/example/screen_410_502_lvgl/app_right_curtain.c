#include "lvgl.h"
#include "app_main.h"
#include "app_music.h"

void lv_right_curtain_init(void)
{
    // tile_right = lv_obj_create(NULL);

    // lv_obj_set_style_bg_color(tile_right, lv_color_make(0, 0, 0), 0);
    // lv_obj_set_style_bg_opa(tile_right, LV_OPA_COVER, 0);
    app_music(scr_right_curtain);

    // lv_obj_add_event_cb(tile_right, (lv_event_cb_t)tile_right_cb, LV_EVENT_ALL, NULL);
}

