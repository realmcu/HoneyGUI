#include "lv_custom_tile_snapshot.h"

void delete_snapshot(lv_obj_t *widget, lv_obj_t *img_snapshot)
{
    lv_draw_buf_t *snapshot = (lv_draw_buf_t *)lv_image_get_src(img_snapshot);
    if (snapshot)
    {
        lv_draw_buf_destroy(snapshot);
        lv_img_set_src(img_snapshot, NULL);
    }
}

void update_snapshot(lv_obj_t *widget, lv_obj_t *img_snapshot)
{
    lv_draw_buf_t *snapshot = (lv_draw_buf_t *)lv_image_get_src(img_snapshot);
    if (snapshot)
    {
        lv_draw_buf_destroy(snapshot);
        lv_img_set_src(img_snapshot, NULL);
    }
    snapshot = lv_snapshot_take(widget, lv_display_get_color_format(NULL));
    lv_image_set_src(img_snapshot, snapshot);
}

void creat_snapshot(lv_obj_t *widget, lv_obj_t *img_snapshot)
{
    // lv_obj_set_style_bg_opa(img_snapshot, LV_OPA_100, 0);
    // lv_obj_set_style_bg_color(img_snapshot, lv_color_hex(0x000000), 0);
    update_snapshot(widget, img_snapshot);
}

void snapshot_custom_cb_delete(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *img_snapshot = lv_event_get_current_target(e);
    lv_obj_t *widget = lv_event_get_user_data(e);
    delete_snapshot(widget, img_snapshot);
}
void snapshot_custom_cb_creat(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *img_snapshot = lv_event_get_current_target(e);
    lv_obj_t *widget = lv_event_get_user_data(e);
    creat_snapshot(widget, img_snapshot);
}