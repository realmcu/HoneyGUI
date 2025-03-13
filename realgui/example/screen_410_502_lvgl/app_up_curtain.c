#include "lvgl.h"
#include "app_main.h"
#include <time.h>
#include <stdio.h>

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502
#define TV_HEIGHT 220
#define TV_INTERVAL 30
#define TV_START 130
#define TV_ARRAY_NUM 4

typedef enum
{
    MESSAGE = 0,
    OS,
} app_name;

typedef struct information
{
    const char *informer;
    const char *content;
    const char *time;
    app_name app;
} information_t;


static lv_obj_t *page;
static lv_obj_t *tv_array[TV_ARRAY_NUM] = {0};
static lv_obj_t *screen_array[TV_ARRAY_NUM] = {0};

static bool close_flag = 1;

// static bool is_at_bottom = false;
// static void page_scroll_event_cb(lv_event_t *e)
// {
//     lv_event_code_t event_code = lv_event_get_code(e);
//     if (event_code == LV_EVENT_SCROLL_END)
//     {
//         lv_obj_t *obj = lv_event_get_target(e);
//         lv_coord_t scrollable = lv_obj_get_scroll_bottom(obj);
//         // lv_log("scrollable=%d\n", scrollable);
//         if (scrollable < -50)
//         {
//             is_at_bottom = true;
//             lv_indev_wait_release(lv_indev_get_act());
//             _ui_screen_change(&scr_watchface, &scr_up_curtain, LV_SCR_LOAD_ANIM_OUT_TOP, 500, 0,
//                               &lv_watchface_init, 1);
//         }
//         else
//         {
//             is_at_bottom = false;
//         }
//     }
// }
// void scr_up_curtain_cb(lv_event_t *e)
// {
//     lv_event_code_t event_code = lv_event_get_code(e);

//     if (event_code == LV_EVENT_SCREEN_LOAD_START)
//     {
//         // left_Animation(ui_hour_group, 0);
//         // right_Animation(ui_label_min, 0);
//         // opa_on_Animation(ui_weather_group_1, 300);
//         // opa_on_Animation(ui_date_group, 500);
//         // opa_on_Animation(ui_weather_title_group_1, 400);
//     }
//     if (event_code == LV_EVENT_GESTURE &&
//         lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP)
//     {
//         lv_indev_wait_release(lv_indev_get_act());
//         _ui_screen_change(&scr_watchface, &scr_up_curtain, LV_SCR_LOAD_ANIM_OUT_TOP, 500, 0,
//                           &lv_watchface_init, 1);
//     }
// }

static bool is_bottom_start(lv_point_t *point)
{
    lv_coord_t height = lv_disp_get_ver_res(NULL); // Get screen height
    lv_coord_t threshold = height * 5 / 6; // Define bottom area as the lower 1/5
    return (point->y > threshold);
}

static void page_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_add_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_indev_t *indev = lv_indev_get_act();  // Get the current input device
    if (indev && lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER)
    {
        lv_point_t point;
        lv_indev_get_point(indev, &point); // Get touch point coordinates
        // Check if the slide starts from the bottom
        if (is_bottom_start(&point))
        {
            clear_flag_recursive(page, LV_OBJ_FLAG_SCROLLABLE);
        }
        return;
    }
    add_flag_recursive(page, LV_OBJ_FLAG_SCROLLABLE);
}

static void cancel_cb(lv_event_t *e)
{
    // Load the previous screen with a slide-out-to-left animation
    lv_scr_load_anim(tileview, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
}

static void view_more_click_tab_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_PRESSED)
    {
        lv_image_set_src(obj, &option_bar_white_bg);
        LV_LOG("Click tab!\n");
    }
    else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST)
    {
        lv_image_set_src(obj, &option_bar_bg);
    }
}


static void view_more_cb(lv_event_t *e)
{
    lv_obj_t *screen = lv_event_get_user_data(e);
    // Load the new screen with a slide-in-from-right and fade-in animation
    lv_scr_load_anim(screen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
}

// Animation callback for sliding the tabview to the left
static void move_left_anim_cb(void *var, int32_t v)
{
    lv_obj_set_x(var, v);
}

// Animation callback for moving the bottom view upward
static void move_up_anim_cb(void *var, int32_t v)
{
    lv_obj_set_y(var, v);
}

static void clear_anim_finished_cb(lv_anim_t *a)
{
    close_flag = 0;
}

// Start rightward movement after leftward movement
static void move_left_anim_done_cb(lv_anim_t *a)
{
    lv_obj_t *tabview = (lv_obj_t *)a->var;
    uint8_t index = 0;
    while (1)
    {
        if (tv_array[index] == tabview)
        {
            break;
        }
        index++;
    }
    lv_obj_delete(tabview);
    tv_array[index] = NULL;
    if (screen_array[index])
    {
        lv_obj_delete(screen_array[index]);
        screen_array[index] = NULL;
    }
    if (index == TV_ARRAY_NUM - 1 || !tv_array[index + 1])
    {
        close_flag = 0;
        return;
    }
    index++;
    while (tv_array[index])
    {
        // Animation for moving the bottom view upward
        lv_anim_t anim;
        lv_anim_init(&anim);
        lv_anim_set_var(&anim, tv_array[index]);
        lv_anim_set_values(&anim, lv_obj_get_y(tv_array[index]),
                           TV_START + (index - 1) * (TV_HEIGHT + TV_INTERVAL)); // Move to the original tabview position
        lv_anim_set_time(&anim, 300);
        lv_anim_set_exec_cb(&anim, move_up_anim_cb);
        lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
        lv_anim_set_ready_cb(&anim, clear_anim_finished_cb); // Delete when animation finishes
        lv_anim_start(&anim);
        tv_array[index - 1] = tv_array[index];
        screen_array[index - 1] = screen_array[index];
        index++;
        // gui_log("y = %d\r\n", tv_os->y);
        if (index == TV_ARRAY_NUM)
        {
            tv_array[index - 1] = NULL;
            break;
        }
    }
}

static void clear_all_anim_finished_cb(lv_anim_t *a)
{
    uint8_t index = 0;
    while (index < TV_ARRAY_NUM && tv_array[index])
    {
        lv_obj_delete(tv_array[index]);
        if (screen_array[index])
        {
            lv_obj_delete(screen_array[index]);
        }
        tv_array[index] = NULL;
        index++;
    }
    close_flag = 0;
}

static void clear_all_cb(lv_event_t *e)
{
    close_flag = 1;
    uint8_t index = 0;
    while (index < TV_ARRAY_NUM && tv_array[index])
    {
        // Animation for moving the bottom view upward
        lv_anim_t anim;
        lv_anim_init(&anim);
        lv_anim_set_var(&anim, tv_array[index]);
        lv_anim_set_values(&anim, lv_obj_get_x(tv_array[index]), -lv_obj_get_width(tv_array[index]));
        lv_anim_set_time(&anim, 300);
        lv_anim_set_exec_cb(&anim, move_left_anim_cb);
        lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
        lv_anim_set_ready_cb(&anim, clear_all_anim_finished_cb); // Delete when animation finishes
        lv_anim_start(&anim);
        index++;
    }
}

static void clear_tv_cb(lv_event_t *e)
{
    close_flag = 1;
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *current_tv = lv_obj_get_parent(lv_obj_get_parent(lv_obj_get_parent(obj)));
    // Animation for sliding the tabview to the left
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, current_tv);
    lv_anim_set_values(&anim, lv_obj_get_x(current_tv), -lv_obj_get_width(current_tv));
    lv_anim_set_time(&anim, 500);
    lv_anim_set_exec_cb(&anim, move_left_anim_cb);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
    lv_anim_set_ready_cb(&anim, move_left_anim_done_cb); // Delete when animation finishes
    lv_anim_start(&anim);
}

static void pagelist_clear(lv_obj_t *parent)
{
    lv_obj_t *bg = lv_image_create(parent);
    lv_image_set_src(bg, &clear_bg);
    lv_obj_align(bg, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_add_flag(bg, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *label = lv_label_create(bg);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, "clear");
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(label, 255, 0);
    lv_obj_set_style_text_font(label, &SourceHanSansSC_size24_bits1_font,
                               0);

    lv_obj_add_event_cb(bg, clear_all_cb, LV_EVENT_CLICKED, NULL);
}

// Initialize the view_more screen
static void init_view_more(lv_obj_t *parent, information_t *payload)
{
    const char *informer = payload->informer;
    const char *content = payload->content;
    const char *time = payload->time;
    app_name app = payload->app;

    lv_obj_set_style_bg_color(parent, lv_color_hex(0x0), LV_PART_MAIN);

    // Create a Cancel label
    lv_obj_t *cancel_label = lv_label_create(parent);
    lv_label_set_text(cancel_label, "Cancel");
    lv_obj_align(cancel_label, LV_ALIGN_TOP_LEFT, 20, 15);
    lv_obj_set_style_text_color(cancel_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(cancel_label, 255, 0);
    lv_obj_set_style_text_font(cancel_label, &SourceHanSansSC_size24_bits1_font,
                               0);
    lv_obj_add_flag(cancel_label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(cancel_label, cancel_cb, LV_EVENT_CLICKED, NULL);

    // Time text in the top right corner
    lv_obj_t *time_label = lv_label_create(parent);
    lv_label_set_text(time_label, time);
    lv_obj_align(time_label, LV_ALIGN_TOP_RIGHT, -60, 15); // Align to top right
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(time_label, 255, 0);
    lv_obj_set_style_text_font(time_label, &SourceHanSansSC_size24_bits1_font,
                               0);

    char source[40];
    lv_obj_t *label = lv_label_create(parent);
    lv_obj_set_pos(label, 37, 70);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(label, 255, 0);
    lv_obj_set_style_text_font(label, &SourceHanSansSC_size24_bits1_font,
                               0);
    lv_obj_t *icon = lv_image_create(parent);
    lv_obj_set_pos(icon, 360, 20);
    switch (app)
    {
    case MESSAGE:
        {
            sprintf(source, "\"message\" dialog\n%s", informer);
            lv_label_set_text(label, source);
            lv_image_set_src(icon, &ui_person_icon);
        }
        break;
    case OS:
        {
            sprintf(source, "\"OS\" dialog");
            lv_label_set_text(label, source);
            lv_image_set_src(icon, &ui_iwatch_icon);
        }
        break;
    default:
        break;
    }
    const char *options[] =
    {
        "Don't remind for an hour",
        "Don't remind today",
        "Add this to Summary"
    };
    // Create option labels
    for (int i = 0; i < 3; i++)
    {
        lv_obj_t *bg = lv_image_create(parent);
        lv_image_set_src(bg, &option_bar_bg);
        lv_obj_set_pos(bg, 30, 160 + i * 90);
        lv_obj_add_flag(bg, LV_OBJ_FLAG_CLICKABLE);

        lv_obj_t *opt_label = lv_label_create(bg);
        lv_obj_set_style_bg_color(opt_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(opt_label, LV_OPA_0, 0);
        lv_obj_set_align(opt_label, LV_ALIGN_CENTER);
        lv_label_set_text(opt_label, options[i]);
        lv_obj_set_style_text_color(opt_label, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_text_font(opt_label, &SourceHanSansSC_size24_bits1_font, 0);
        lv_obj_add_event_cb(bg, view_more_click_tab_cb, LV_EVENT_ALL, NULL);
    }
}

void pagelist_create(information_t *payload)
{
    if (close_flag)
    {
        // LV_LOG("During close animation, can't add tab!!! %s, %d\n", __FUNCTION__, __LINE__);
        return;
    }

    // Move current information, then add new information on the top
    uint8_t index = 0;
    while (1)
    {
        if (!tv_array[index])
        {
            break;
        }
        index++;
        if (index == TV_ARRAY_NUM)
        {
            // gui_log("Can't add more tab!!! %s, %d\n", __FUNCTION__, __LINE__);
            return;
        }
    }
    if (index > 0)
    {
        while (1)
        {
            uint32_t y = TV_START + index * (TV_HEIGHT + TV_INTERVAL);
            lv_obj_align(tv_array[index - 1], LV_ALIGN_TOP_MID, 0, y);
            tv_array[index] = tv_array[index - 1];
            screen_array[index] = screen_array[index - 1];
            if ((index - 1) == 0)
            {
                break;
            }
            index--;
        }
    }
    const char *informer = payload->informer;
    const char *content = payload->content;
    const char *time = payload->time;
    app_name app = payload->app;

    // Create tabview with no top tab bar
    lv_obj_t *tabview = lv_tabview_create(page);
    lv_tabview_set_tab_bar_size(tabview, 0); // Set tab bar height to 0
    lv_obj_set_size(tabview, SCREEN_WIDTH, TV_HEIGHT);
    lv_obj_align(tabview, LV_ALIGN_TOP_MID, 0, TV_START);
    lv_obj_set_style_bg_opa(tabview, LV_OPA_0, 0);
    lv_obj_set_scrollbar_mode(tabview, LV_SCROLLBAR_MODE_OFF); // Hide scroll bar
    tv_array[0] = tabview;
    // Add the first tab to create message box
    lv_obj_t *msg_tab = lv_tabview_add_tab(tabview, "Tab1");
    lv_obj_set_style_bg_opa(msg_tab, LV_OPA_0, 0);
    lv_obj_set_scrollbar_mode(msg_tab, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(msg_tab, 0, 0); // Clear all padding for msg_tab

    lv_obj_t *msg_box = lv_image_create(msg_tab);
    lv_image_set_src(msg_box, &msg_tab_bg);
    lv_obj_set_pos(msg_box, 30, 0);
    lv_obj_set_size(msg_box, 350, TV_HEIGHT);
    lv_obj_clear_flag(msg_box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *icon = lv_image_create(msg_box);
    lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 0, 0);
    switch (app)
    {
    case MESSAGE:
        {
            lv_image_set_src(icon, &ui_person_icon);
        }
        break;
    case OS:
        {
            lv_image_set_src(icon, &ui_iwatch_icon);
        }
        break;
    default:
        break;
    }
    // Time text in the top right corner
    lv_obj_t *time_label = lv_label_create(msg_box);
    lv_label_set_text(time_label, time);
    lv_obj_align(time_label, LV_ALIGN_TOP_RIGHT, -10, 10); // Align to top right
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(time_label, 255, 0);
    lv_obj_set_style_text_font(time_label, &SourceHanSansSC_size12_bits1_font, 0);

    // source
    lv_obj_t *source_label = lv_label_create(msg_box);
    lv_label_set_text(source_label, informer);
    lv_obj_align(source_label, LV_ALIGN_TOP_LEFT, 0, 30);
    lv_obj_set_style_text_color(source_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(source_label, 255, 0);
    lv_obj_set_style_text_font(source_label, &SourceHanSansSC_size24_bits1_font, 0);

    // Scrollable container for source_label
    lv_obj_t *scroll_container = lv_obj_create(msg_box);
    lv_obj_set_size(scroll_container, 330, TV_HEIGHT - 60);
    lv_obj_set_pos(scroll_container, 0, 60);
    lv_obj_set_style_bg_opa(scroll_container, LV_OPA_0, 0);
    lv_obj_set_scroll_dir(scroll_container, LV_DIR_VER);
    lv_obj_set_style_border_width(scroll_container, 0, LV_PART_MAIN); // No border
    lv_obj_set_style_pad_all(scroll_container, 0, 0);
    lv_obj_set_scrollbar_mode(scroll_container, LV_SCROLLBAR_MODE_OFF); // Hide scroll bar

    // content
    lv_obj_t *content_label = lv_label_create(scroll_container);
    lv_label_set_text(content_label, content);
    lv_obj_add_flag(content_label, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_width(content_label, lv_pct(95)); // Content width
    lv_obj_align(content_label, LV_ALIGN_TOP_MID, 20, 0);
    lv_obj_set_scroll_dir(content_label, LV_DIR_VER);
    lv_obj_set_style_text_color(content_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(content_label, 255, 0);
    lv_obj_set_style_text_font(content_label, &SourceHanSansSC_size24_bits1_font, 0);

    lv_obj_t *option_tab = lv_tabview_add_tab(tabview, "Tab2");
    lv_obj_set_style_bg_opa(option_tab, LV_OPA_0, 0);
    lv_obj_set_scrollbar_mode(option_tab, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(option_tab, 0, 0); // Clear all padding for option_tab

    lv_obj_t *left_bg = lv_image_create(option_tab);
    lv_image_set_src(left_bg, &option_tab_bg);
    lv_obj_set_pos(left_bg, 30, 0);
    lv_obj_add_flag(left_bg, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *left_label = lv_label_create(left_bg);
    lv_obj_set_align(left_label, LV_ALIGN_CENTER);
    lv_label_set_text(left_label, "...");
    lv_obj_set_style_text_color(left_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(left_label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(left_label, &SourceHanSansSC_size32_bits1_font, 0);

    lv_obj_t *right_bg = lv_image_create(option_tab);
    lv_image_set_src(right_bg, &option_tab_bg);
    lv_obj_set_pos(right_bg, 210, 0);
    lv_obj_add_flag(right_bg, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *right_label = lv_label_create(right_bg);
    lv_obj_set_align(right_label, LV_ALIGN_CENTER);
    lv_label_set_text(right_label, "X");
    lv_obj_set_style_text_color(right_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(right_label, 255, 0);
    lv_obj_set_style_text_font(right_label, &SourceHanSansSC_size32_bits1_font, 0);

    lv_obj_t *view_more = lv_obj_create(NULL);
    screen_array[0] = view_more;
    init_view_more(view_more, payload);
    // Click events
    lv_obj_add_event_cb(left_bg, view_more_cb, LV_EVENT_CLICKED, view_more);
    lv_obj_add_event_cb(right_bg, clear_tv_cb, LV_EVENT_CLICKED, NULL);
}

void lv_up_curtain_init(void)
{
    // scr_up_curtain = lv_obj_create(NULL);
    // lv_obj_add_event_cb(scr_up_curtain, (lv_event_cb_t)scr_up_curtain_cb, LV_EVENT_ALL, NULL);
    page = lv_obj_create(scr_up_curtain);
    lv_obj_set_scroll_dir(page, LV_DIR_VER);
    // lv_obj_set_style_bg_color(page, lv_color_make(76, 76, 76), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(page, 0, 0);
    lv_obj_set_size(page, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_pos(page, 0, 0);
    lv_obj_add_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(page, 0, 0);
    lv_obj_add_event_cb(page, page_event_cb, LV_EVENT_ALL, NULL);
    for (uint8_t index = 0; index < TV_ARRAY_NUM; index++)
    {
        tv_array[index] = NULL;
    }

    pagelist_clear(page);
    close_flag = 0;

    // time_t rawtime;
    // time(&rawtime);
    // struct tm *timeinfo = localtime(&rawtime);
    // char time[10];
    // if (timeinfo)
    // {
    //     sprintf(time, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    // }
    // char *content =
    //     "Never gonna give you up. Never gonna let you down. Never gonna run around and desert you. Never gonna give you up. Never gonna let you down. Never gonna run around and desert you.";

    // information_t payload =
    // {
    //     "watchOS 10.3.1",
    //     content,
    //     time,
    //     MESSAGE
    // };
    // gui_msg_t msg =
    // {
    //     .event = 0,
    //     .payload = &payload,
    //     .cb = (gui_msg_cb)pagelist_create,
    // };
    // pagelist_create(&msg);
}