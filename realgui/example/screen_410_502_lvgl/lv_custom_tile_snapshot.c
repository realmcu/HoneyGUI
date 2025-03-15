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

void creat_snapshot_img(lv_obj_t *widget, lv_obj_t *img_snapshot)
{
    update_snapshot(widget, img_snapshot);
}

void snapshot_custom_cb_delete(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *img_snapshot = lv_event_get_current_target(e);
    lv_obj_t *widget = lv_event_get_user_data(e);
    delete_snapshot(widget, img_snapshot);
}
void snapshot_custom_cb_create(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *img_snapshot = lv_event_get_current_target(e);
    lv_obj_t *widget = lv_event_get_user_data(e);
    creat_snapshot_img(widget, img_snapshot);
}

lv_obj_t *create_snapshot_obj_directly(lv_obj_t *parent, lv_obj_t *target)
{
    lv_obj_t *snapshot = lv_image_create(parent);
    lv_obj_set_size(snapshot, lv_obj_get_width(target), lv_obj_get_height(target));
    creat_snapshot_img(target, snapshot);
    lv_obj_add_flag(target, LV_OBJ_FLAG_HIDDEN);
    return snapshot;
}

lv_obj_t *create_snapshot_obj_with_enent(lv_obj_t *parent, lv_obj_t *target,
                                         uint32_t create_enent_id, uint32_t delete_enent_id)
{
    lv_obj_t *snapshot = lv_image_create(parent);
    lv_obj_set_size(snapshot, lv_obj_get_width(target), lv_obj_get_height(target));
    lv_obj_add_flag(snapshot, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_event_cb(snapshot, snapshot_custom_cb_create, create_enent_id, target);
    lv_obj_add_event_cb(snapshot, snapshot_custom_cb_delete, delete_enent_id, target);
    return snapshot;
}
