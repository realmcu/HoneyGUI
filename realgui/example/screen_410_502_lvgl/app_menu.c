#include <math.h>
#include "lvgl.h"
#include "app_main.h"

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502
#define ITEM_HEIGHT 120
#define DRAG_THRESHOLD 50  // Drag distance threshold
#define LEFT_EDGE_THRESHOLD 30  // Left edge detection range

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
    LV_LOG("enter exit_menu func\n");
    lv_scr_load_anim(tileview, LV_SCR_LOAD_ANIM_FADE_OUT, 300, 0, false);
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

        // Calculate the base Y position of the button
        lv_coord_t base_y = i * ITEM_HEIGHT - scroll_y;

        // Calculate the offset relative to the screen center
        lv_coord_t center_offset = base_y - (SCREEN_HEIGHT / 2);

        // Set the new position of the button
        uint8_t offX_range = 20;
        lv_coord_t new_x = (uint8_t)(offX_range * LV_ABS(center_offset) / (SCREEN_HEIGHT / 2));
        lv_obj_set_x(btn, new_x);
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
    lv_obj_set_style_bg_color(scr_app_menu, lv_color_make(0, 0, 0), 0);
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
        // Create button as a container
        lv_obj_t *btn = lv_btn_create(page);
        lv_obj_set_style_bg_color(btn, lv_color_make(0, 0, 0), 0);
        lv_obj_set_style_shadow_width(btn, 0, 0); // Remove shadow
        lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN); // No border
        lv_obj_set_style_pad_all(btn, 0, 0);
        lv_obj_set_style_radius(btn, 0, 0);
        lv_obj_set_pos(btn, 0, ITEM_HEIGHT * i);
        lv_obj_set_width(btn, SCREEN_WIDTH);
        lv_obj_set_height(btn, ITEM_HEIGHT);
        lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

        // Add icon
        lv_obj_t *img = lv_img_create(btn);
        lv_img_set_src(img, app_list[i].icon);
        lv_obj_align(img, LV_ALIGN_LEFT_MID, 0, 0);
        // Add text
        lv_obj_t *label = lv_label_create(btn);
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