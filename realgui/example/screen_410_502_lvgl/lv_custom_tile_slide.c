#include "lv_custom_tile_slide.h"
#include "lv_area.h"

#include "lv_img_dsc_list.h"
#include "app_main.h"
#define TILE_SLIDE_USE_SNAPSHORT WATCH_DEMO_USE_SNAPSHOT

static bool lv_tile_is_scrolling(lv_obj_t *tile)
{
    if (tile == lv_tileview_get_tile_active(lv_obj_get_parent(tile)))
    {
        LV_LOG_TRACE("scrolling cause lv_tileview_get_tile_active");
        return true;
    }

    lv_area_t tile_area;
    lv_obj_get_coords(tile, &tile_area);

    lv_area_t scr_area;
    lv_obj_get_coords(lv_scr_act(), &scr_area);

    if (lv_area_is_on(&tile_area, &scr_area))
    {
        LV_LOG_TRACE("scrolling cause lv_area_is_on");
        return true;
    }
    return false;
}

void tileview_custom_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_current_target(e);

    lv_tileview_t *tv = (lv_tileview_t *) obj;

    if (code == LV_EVENT_SCROLL_BEGIN)
    {
        bool *tileview_scrolling = lv_event_get_user_data(e);
        if (!*tileview_scrolling)
        {
            *tileview_scrolling = true;
#if TILE_SLIDE_USE_SNAPSHORT
            for (int i = 0; i < lv_obj_get_child_count(obj); i++)
            {
                lv_obj_t *tile_obj = lv_obj_get_child(obj, i);
                if (lv_tile_is_scrolling(tile_obj))
                {
                    lv_obj_send_event(lv_obj_get_child(tile_obj, 1), event_snapshot_creat, NULL);
                    lv_obj_add_flag(lv_obj_get_child(tile_obj, 0), LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(lv_obj_get_child(tile_obj, 1), LV_OBJ_FLAG_HIDDEN);
                }
            }
#endif
            LV_LOG_INFO("REAL LV_EVENT_SCROLL_BEGIN \n");
        }
        // LV_LOG("tileview_custom_cb LV_EVENT_SCROLL_BEGIN \n");
    }
    else if (code == LV_EVENT_SCROLL)
    {
        lv_point_t center =
        {
            .x = lv_obj_get_width(obj) / 2,
            .y = lv_obj_get_height(obj) / 2
        };

        for (int i = 0; i < lv_obj_get_child_count(obj); i++)
        {
            lv_obj_t *tile_obj = lv_obj_get_child(obj, i);
            if (!lv_tile_is_scrolling(tile_obj))
            {
                continue;
            }
#if TILE_SLIDE_USE_SNAPSHORT
            if (lv_obj_has_flag(lv_obj_get_child(tile_obj, 1), LV_OBJ_FLAG_HIDDEN))
            {
                lv_obj_send_event(lv_obj_get_child(tile_obj, 1), event_snapshot_creat, NULL);
                lv_obj_add_flag(lv_obj_get_child(tile_obj, 0), LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(lv_obj_get_child(tile_obj, 1), LV_OBJ_FLAG_HIDDEN);
            }
#endif
            lv_area_t coords;
            lv_obj_get_coords(tile_obj, &coords);
            lv_point_t tile_center =
            {
                .x = (coords.x1 + coords.x2) / 2,
                .y = (coords.y1 + coords.y2) / 2
            };

            float scale_x = (float)(abs(tile_center.x - center.x)) / center.x;
            float scale_y = (float)(abs(tile_center.y - center.y)) / center.y;

            float scale =  LV_MAX(scale_x, scale_y);
            scale =  1 - scale / 2;
            int32_t scaleint  = LV_CLAMP(128, scale * 256, 256);

#if TILE_SLIDE_USE_SNAPSHORT
            lv_obj_set_style_transform_pivot_x(lv_obj_get_child(tile_obj, 1), LV_PCT(50), 0);
            lv_obj_set_style_transform_pivot_y(lv_obj_get_child(tile_obj, 1), LV_PCT(50), 0);
            lv_obj_set_style_transform_scale(lv_obj_get_child(tile_obj, 1), scaleint, 0);
            lv_obj_set_style_opa(lv_obj_get_child(tile_obj, 1), scaleint, 0);
#else
            lv_obj_set_style_transform_pivot_x(tile_obj, LV_PCT(50), 0);
            lv_obj_set_style_transform_pivot_y(tile_obj, LV_PCT(50), 0);
            lv_obj_set_style_transform_scale(tile_obj, scaleint, 0);
            lv_obj_set_style_opa(tile_obj, scaleint, 0);
#endif
        }
        // LV_LOG("tileview_custom_cb LV_EVENT_SCROLL \n");
    }
    else if (code == LV_EVENT_SCROLL_END)
    {
        bool *tileview_scrolling = lv_event_get_user_data(e);
        if (!lv_obj_is_scrolling(obj) && *tileview_scrolling)
        {
            *tileview_scrolling = false;
            for (int i = 0; i < lv_obj_get_child_count(obj); i++)
            {
                lv_obj_t *tile_obj = lv_obj_get_child(obj, i);
#if TILE_SLIDE_USE_SNAPSHORT
                lv_obj_set_style_transform_scale(lv_obj_get_child(tile_obj, 1), 256, 0);
                lv_obj_set_style_opa(lv_obj_get_child(tile_obj, 1), 255, 0);
                lv_obj_add_flag(lv_obj_get_child(tile_obj, 1), LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(lv_obj_get_child(tile_obj, 0), LV_OBJ_FLAG_HIDDEN);
                lv_obj_send_event(lv_obj_get_child(tile_obj, 1), event_snapshot_delete, NULL);
#else
                lv_obj_set_style_transform_scale(tile_obj, 256, 0);
                lv_obj_set_style_opa(tile_obj, 255, 0);
#endif
            }
            LV_LOG_INFO("REAL LV_EVENT_SCROLL_END \n");
        }
        // LV_LOG("tileview_custom_cb LV_EVENT_SCROLL_END \n");
    }
}

extern bool tileview_scrolling;

void create_cross_tileview(void)
{
    lv_obj_t *tv = lv_tileview_create(lv_screen_active());
    lv_obj_set_style_bg_color(tv, lv_color_black(), 0);
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    //center
    lv_obj_t *center_tile = lv_tileview_add_tile(tv, 1, 1, LV_DIR_ALL);
    lv_obj_t *rect = lv_obj_create(center_tile);
    lv_obj_set_size(rect, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(rect, lv_palette_main(LV_PALETTE_YELLOW), 0);

    lv_obj_t *label = lv_label_create(center_tile);
    lv_label_set_text(label, "Center");
    lv_obj_center(label);

    //top
    lv_obj_t *top_tile = lv_tileview_add_tile(tv, 1, 0, LV_DIR_BOTTOM);
    rect = lv_obj_create(top_tile);
    lv_obj_set_size(rect, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(rect, lv_palette_main(LV_PALETTE_ORANGE), 0);

    label = lv_label_create(top_tile);
    lv_label_set_text(label, "Top");
    lv_obj_center(label);

    //bottom
    lv_obj_t *bottom_tile = lv_tileview_add_tile(tv, 1, 2, LV_DIR_TOP);
    rect = lv_obj_create(bottom_tile);
    lv_obj_set_size(rect, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(rect, lv_palette_main(LV_PALETTE_GREEN), 0);

    label = lv_label_create(bottom_tile);
    lv_label_set_text(label, "Bottom");
    lv_obj_center(label);

    //left
    lv_obj_t *left_tile = lv_tileview_add_tile(tv, 0, 1, LV_DIR_RIGHT);
    rect = lv_obj_create(left_tile);
    lv_obj_set_size(rect, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(rect, lv_palette_main(LV_PALETTE_BLUE), 0);

    label = lv_label_create(left_tile);
    lv_label_set_text(label, "Left");
    lv_obj_center(label);

    //right
    lv_obj_t *right_tile = lv_tileview_add_tile(tv, 2, 1, LV_DIR_LEFT);
    rect = lv_obj_create(right_tile);
    lv_obj_set_size(rect, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(rect, lv_palette_main(LV_PALETTE_PINK), 0);

    label = lv_label_create(right_tile);
    lv_label_set_text(label, "Right");
    lv_obj_center(label);

    lv_tileview_set_tile_by_index(tv, 1, 1, LV_ANIM_OFF);
    lv_obj_add_event_cb(tv, tileview_custom_cb, LV_EVENT_ALL, &tileview_scrolling);
}
