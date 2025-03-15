/**
 * @file lv_custom_tile_slide.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_custom_tile_slide.h"
#include "lv_area.h"
#include "lv_obj_pos.h"

#include "lv_img_dsc_list.h"
#include "app_main.h"

/*********************
 *      DEFINES
 *********************/
#define TILE_SLIDE_USE_SNAPSHORT WATCH_DEMO_USE_SNAPSHOT

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_tile_is_scrolling(lv_obj_t *tile);
static void apply_slide_effect(lv_obj_t *obj, SLIDE_EFFECT effect);
static void reset_slide_effect(lv_obj_t *obj, SLIDE_EFFECT effect);

static void apply_fade_effect(lv_obj_t *obj);
static void reset_fade_effect(lv_obj_t *obj);
static void apply_scale_effect(lv_obj_t *obj);
static void reset_scale_effect(lv_obj_t *obj);
static void apply_scale_fade_effect(lv_obj_t *obj);
static void reset_scale_fade_effect(lv_obj_t *obj);

#if LV_DRAW_TRANSFORM_USE_MATRIX
static void apply_cube_effect(lv_obj_t *obj);
static void reset_cube_effect(lv_obj_t *obj);
static void apply_spiral_notebook_effect(lv_obj_t *obj);
static void reset_spiral_notebook_effect(lv_obj_t *obj);
#endif
/**********************
 *  STATIC VARIABLES
 **********************/
SLIDE_EFFECT global_slide = SCALE;
extern bool tileview_scrolling;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
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
        for (int i = 0; i < lv_obj_get_child_count(obj); i++)
        {
            lv_obj_t *tile_obj = lv_obj_get_child(obj, i);
            if (!lv_tile_is_scrolling(tile_obj))
            {
                continue;
            }
            lv_obj_t *target = tile_obj;
#if TILE_SLIDE_USE_SNAPSHORT
            if (lv_obj_has_flag(lv_obj_get_child(tile_obj, 1), LV_OBJ_FLAG_HIDDEN))
            {
                lv_obj_send_event(lv_obj_get_child(tile_obj, 1), event_snapshot_creat, NULL);
                lv_obj_add_flag(lv_obj_get_child(tile_obj, 0), LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(lv_obj_get_child(tile_obj, 1), LV_OBJ_FLAG_HIDDEN);
            }
            target = lv_obj_get_child(tile_obj, 1);
#endif
            apply_slide_effect(target, global_slide);
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
                lv_obj_t *target = tile_obj;
#if TILE_SLIDE_USE_SNAPSHORT
                lv_obj_add_flag(lv_obj_get_child(tile_obj, 1), LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(lv_obj_get_child(tile_obj, 0), LV_OBJ_FLAG_HIDDEN);
                lv_obj_send_event(lv_obj_get_child(tile_obj, 1), event_snapshot_delete, NULL);
                target = lv_obj_get_child(tile_obj, 1);
#endif
                reset_slide_effect(target, global_slide);
            }
            LV_LOG_INFO("REAL LV_EVENT_SCROLL_END \n");
        }
        // LV_LOG("tileview_custom_cb LV_EVENT_SCROLL_END \n");
    }
}


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

/**********************
 *   STATIC FUNCTIONS
 **********************/
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

static void apply_scale_fade_effect(lv_obj_t *obj)
{
    lv_point_t screen_center =
    {
        .x = lv_display_get_horizontal_resolution(NULL) / 2,
        .y = lv_display_get_vertical_resolution(NULL) / 2
    };

    lv_area_t obj_coords;
    lv_obj_get_coords(obj, &obj_coords);
    lv_point_t obj_center =
    {
        .x = (obj_coords.x1 + obj_coords.x2) / 2,
        .y = (obj_coords.y1 + obj_coords.y2) / 2
    };

    float scale_x = (float)(LV_ABS(obj_center.x - screen_center.x)) / screen_center.x;
    float scale_y = (float)(LV_ABS(obj_center.y - screen_center.y)) / screen_center.y;

    float scale = 1 - LV_MAX(scale_x, scale_y) / 2;
    int32_t scaleint  = LV_CLAMP(128, scale * 256, 256);

    lv_obj_set_style_transform_pivot_x(obj, LV_PCT(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, LV_PCT(50), 0);
    lv_obj_set_style_transform_scale(obj, scaleint, 0);
    lv_obj_set_style_opa(obj, scaleint, 0);
}

static void reset_scale_fade_effect(lv_obj_t *obj)
{
    lv_obj_set_style_transform_scale(obj, 256, 0);
    lv_obj_set_style_opa(obj, 255, 0);
}

static void apply_scale_effect(lv_obj_t *obj)
{
    lv_point_t screen_center =
    {
        .x = lv_display_get_horizontal_resolution(NULL) / 2,
        .y = lv_display_get_vertical_resolution(NULL) / 2
    };

    lv_area_t obj_coords;
    lv_obj_get_coords(obj, &obj_coords);
    lv_point_t obj_center =
    {
        .x = (obj_coords.x1 + obj_coords.x2) / 2,
        .y = (obj_coords.y1 + obj_coords.y2) / 2
    };

    float scale_x = (float)(LV_ABS(obj_center.x - screen_center.x)) / screen_center.x;
    float scale_y = (float)(LV_ABS(obj_center.y - screen_center.y)) / screen_center.y;

    float scale = 1 - LV_MAX(scale_x, scale_y) / 2;
    int32_t scaleint  = LV_CLAMP(128, scale * 256, 256);

    lv_obj_set_style_transform_pivot_x(obj, LV_PCT(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, LV_PCT(50), 0);
    lv_obj_set_style_transform_scale(obj, scaleint, 0);
}

static void reset_scale_effect(lv_obj_t *obj)
{
    lv_obj_set_style_transform_scale(obj, 256, 0);
}

static void apply_fade_effect(lv_obj_t *obj)
{
    lv_point_t screen_center =
    {
        .x = lv_display_get_horizontal_resolution(NULL) / 2,
        .y = lv_display_get_vertical_resolution(NULL) / 2
    };

    lv_area_t obj_coords;
    lv_obj_get_coords(obj, &obj_coords);
    lv_point_t obj_center =
    {
        .x = (obj_coords.x1 + obj_coords.x2) / 2,
        .y = (obj_coords.y1 + obj_coords.y2) / 2
    };

    float scale_x = (float)(LV_ABS(obj_center.x - screen_center.x)) / screen_center.x;
    float scale_y = (float)(LV_ABS(obj_center.y - screen_center.y)) / screen_center.y;

    float scale = 1 - LV_MAX(scale_x, scale_y) / 2;
    int32_t scaleint  = LV_CLAMP(128, scale * 256, 256);

    lv_obj_set_style_opa(obj, scaleint, 0);
}

static void reset_fade_effect(lv_obj_t *obj)
{
    lv_obj_set_style_opa(obj, 255, 0);
}

#if LV_DRAW_TRANSFORM_USE_MATRIX
static void apply_cube_effect(lv_obj_t *obj)
{
    lv_point_t screen_center =
    {
        .x = lv_display_get_horizontal_resolution(NULL) / 2,
        .y = lv_display_get_vertical_resolution(NULL) / 2
    };

    lv_area_t obj_coords;
    lv_obj_get_coords(obj, &obj_coords);
    lv_point_t obj_center =
    {
        .x = (obj_coords.x1 + obj_coords.x2) / 2,
        .y = (obj_coords.y1 + obj_coords.y2) / 2
    };

    float scale_x = (float)(LV_ABS(obj_center.x - screen_center.x)) / screen_center.x;
    float scale_y = (float)(LV_ABS(obj_center.y - screen_center.y)) / screen_center.y;

    float scale = 1 - LV_MAX(scale_x, scale_y) / 2;
    int32_t scaleint  = LV_CLAMP(128, scale * 256, 256);

    float rotate_degree = 90 * (scaleint / 256.0f - 1.0f);

    lv_matrix_t matrix;
    lv_matrix_identity(&matrix);

    lv_matrix_rotate(&matrix, rotate_degree);

    lv_obj_get_style_transform_rotation(obj, rotate_degree);
    lv_obj_set_style_transform_pivot_x(obj, LV_PCT(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, LV_PCT(50), 0);
    lv_obj_set_transform(obj, &matrix);
}

static void reset_cube_effect(lv_obj_t *obj)
{
    lv_matrix_t identity;
    lv_matrix_identity(&identity);
    lv_obj_set_transform(obj, &identity);
}

static void apply_spiral_notebook_effect(lv_obj_t *obj)
{
    lv_point_t screen_center =
    {
        .x = lv_display_get_horizontal_resolution(NULL) / 2,
        .y = lv_display_get_vertical_resolution(NULL) / 2
    };

    lv_area_t obj_coords;
    lv_obj_get_coords(obj, &obj_coords);
    lv_point_t obj_center =
    {
        .x = (obj_coords.x1 + obj_coords.x2) / 2,
        .y = (obj_coords.y1 + obj_coords.y2) / 2
    };

    float scale_x = (float)(LV_ABS(obj_center.x - screen_center.x)) / screen_center.x;
    float scale_y = (float)(LV_ABS(obj_center.y - screen_center.y)) / screen_center.y;

    float scale = 1 - LV_MAX(scale_x, scale_y) / 2;
    int32_t scaleint  = LV_CLAMP(128, scale * 256, 256);

    float rotate_degree = 90 * (scaleint / 256.0f - 1.0f);

    lv_matrix_t matrix;
    lv_matrix_identity(&matrix);

    lv_matrix_scale(&matrix, scaleint / 256.0f, scaleint / 256.0f);
    lv_matrix_rotate(&matrix, rotate_degree);

    lv_obj_set_transform(obj, &matrix);
}

static void reset_spiral_notebook_effect(lv_obj_t *obj)
{
    lv_matrix_t identity;
    lv_matrix_identity(&identity);
    lv_obj_set_transform(obj, &identity);
}
#endif
static void apply_slide_effect(lv_obj_t *obj, SLIDE_EFFECT effect)
{
    switch (effect)
    {
    case CLASSIC:
        break;
    case FADE:
        apply_fade_effect(obj);
        break;
    case SCALE:
        apply_scale_effect(obj);
        break;
    case SCALE_FADE:
        apply_scale_fade_effect(obj);
        break;
#if LV_DRAW_TRANSFORM_USE_MATRIX
    case CUBE_ROTATION:
        apply_cube_effect(obj);
        break;
    case SPIRAL_NOTEBOOK:
        apply_spiral_notebook_effect(obj);
        break;
#endif
    default:
        break;
    }
}

static void reset_slide_effect(lv_obj_t *obj, SLIDE_EFFECT effect)
{
    switch (effect)
    {
    case CLASSIC:
        break;
    case FADE:
        reset_fade_effect(obj);
        break;
    case SCALE:
        reset_scale_effect(obj);
        break;
    case SCALE_FADE:
        reset_scale_fade_effect(obj);
        break;
#if LV_DRAW_TRANSFORM_USE_MATRIX
    case CUBE_ROTATION:
        reset_cube_effect(obj);
        break;
    case SPIRAL_NOTEBOOK:
        reset_spiral_notebook_effect(obj);
        break;
#endif
    default:
        break;
    }
}

