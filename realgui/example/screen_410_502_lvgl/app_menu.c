#include <math.h>
#include "lvgl.h"
#include "app_main.h"

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502
#define ITEM_HEIGHT 120
#define ITEM_INTERVAL 10
#define OFFSET_X 20

// Define APP structure
typedef struct
{
    const char *name;
    const lv_img_dsc_t *icon;
} app_item_t;

// Sample APP data
static app_item_t app_list[] =
{
    {"Heart Rate", &ui_clock_heartrate_icon},
    {"Music", &ui_clock_music_icon},
    {"Fruit Ninja", &ui_clock_fruit_ninja_icon},
    {"Box2d Ring", &ui_clock_box2d_ring_icon},
    {"Activity", &ui_clock_activity_icon},
    {"Heart Rate", &ui_clock_heartrate_icon},
    {"Music", &ui_clock_music_icon},
    {"Fruit Ninja", &ui_clock_fruit_ninja_icon},
    {"Box2d Ring", &ui_clock_box2d_ring_icon},
    {"Activity", &ui_clock_activity_icon}
};
#define APP_COUNT (sizeof(app_list) / sizeof(app_list[0]))

static int16_t page_menu_y_his = 0;

// Exit menu function
static void exit_menu(void)
{
    // LV_LOG("enter exit_menu func\n");
    lv_scr_load_anim(tileview, LV_SCR_LOAD_ANIM_FADE_OUT, 300, 0, false);
}

static void enter_app_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    uint8_t index = lv_obj_get_index(obj);
    index %= (APP_COUNT / 2);
    if (index == 1)
    {
        lv_tileview_set_tile_by_index(tileview, 2, 1, LV_ANIM_OFF);
        lv_scr_load_anim(tileview, LV_SCR_LOAD_ANIM_FADE_OUT, 300, 0, false);
    }
    else if (index == 4)
    {
        lv_tileview_set_tile_by_index(tileview, 3, 1, LV_ANIM_OFF);
        lv_scr_load_anim(tileview, LV_SCR_LOAD_ANIM_FADE_OUT, 300, 0, false);
    }
}

// Update button positions based on scroll
static void update_button_pos(lv_obj_t *page, lv_coord_t scroll_y)
{
    // Calculate the position of each button
    for (int i = 0; i < APP_COUNT; i++)
    {
        lv_obj_t *btn = lv_obj_get_child(page, i);
        if (btn == NULL)
        {
            continue;
        }

        lv_coord_t base_y = i * (ITEM_HEIGHT + ITEM_INTERVAL) - scroll_y;
        // Calculate the offset relative to the screen center
        lv_coord_t diff_y = base_y + ITEM_HEIGHT / 2 - SCREEN_HEIGHT / 2;
        diff_y = LV_ABS(diff_y);

        /*Get the x of diff_y on a circle.*/
        int32_t x;
        int32_t r = SCREEN_WIDTH;
        /*If diff_y is out of the circle use the last point of the circle (the radius)*/
        if (diff_y >= r)
        {
            x = r;
        }
        else
        {
            /*Use Pythagoras theorem to get x from radius and y*/
            uint32_t x_sqr = r * r - diff_y * diff_y;
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000);   /*Use lvgl's built in sqrt root function*/
            x = r - res.i;
        }

        /*Translate the item by the calculated X coordinate*/
        lv_obj_set_x(btn, x + OFFSET_X);

        /*Use some opacity with larger translations*/
        // lv_opa_t opa = lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
        // lv_obj_set_style_opa(btn, LV_OPA_COVER - opa, 0);
    }
}

// Page scroll event callback
static void page_event_cb(lv_event_t *e)
{
    lv_obj_t *page = lv_event_get_target(e);
    lv_coord_t scroll_y = lv_obj_get_scroll_y(page);
    page_menu_y_his = scroll_y;
    update_button_pos(page, page_menu_y_his);
}

// Initialize the app menu
void lv_app_menu_init(void)
{
    scr_app_menu = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_app_menu, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_bg_opa(scr_app_menu, LV_OPA_COVER, 0);
    lv_obj_set_scrollbar_mode(scr_app_menu, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *page = lv_obj_create(scr_app_menu);
    lv_obj_scroll_to_y(page, page_menu_y_his, LV_ANIM_OFF);
    lv_obj_set_style_border_width(page, 0, LV_PART_MAIN); // No border
    lv_obj_set_style_bg_color(page, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_bg_opa(page, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(page, 0, 0);
    lv_obj_set_size(page, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_pos(page, 0, 0);
    lv_obj_add_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(page, LV_DIR_VER);
    lv_obj_add_event_cb(page, page_event_cb, LV_EVENT_SCROLL, NULL);

    // Add all APP items
    for (uint32_t i = 0; i < APP_COUNT; i++)
    {
        lv_obj_t *bg = lv_image_create(page);
        lv_image_set_src(bg, &menu_bar_bg);
        lv_obj_set_pos(bg, 0, 0);
        lv_obj_set_pos(bg, 0, (ITEM_HEIGHT + ITEM_INTERVAL) * i);
        lv_obj_clear_flag(bg, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(bg, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(bg, (lv_event_cb_t)enter_app_cb, LV_EVENT_CLICKED, NULL);

        // Add icon
        lv_obj_t *img = lv_img_create(bg);
        lv_img_set_src(img, app_list[i].icon);
        lv_obj_align(img, LV_ALIGN_LEFT_MID, 20, 0);
        // Add text
        lv_obj_t *label = lv_label_create(bg);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_label_set_text(label, app_list[i].name);
        lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(label, &SourceHanSansSC_size24_bits1_font,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    update_button_pos(page, page_menu_y_his);

    return_create(page, exit_menu);
}