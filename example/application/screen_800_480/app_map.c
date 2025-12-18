//app_map.c
/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include "tp_algo.h"
#include "app_main_dashboard.h"
#include "gui_view.h"
#include "gui_canvas.h"
#include "gui_text.h"
#include "dashboard_adapt.h"
#include "app_navigation_adapter.h"

#ifndef _HONEYGUI_SIMULATOR_
#include "communicate_parse_navigation.h"
#endif

/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define X_START 142 + 130
#define Y_START 198
#define MAP_H 268 + 1
#define MAP_W 218

#define COLOR_GREEN nvgRGB(90, 180, 150)
#define COLOR_BLUE  nvgRGB(0, 220, 227)

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
gui_win_t *win_map = NULL;
gui_img_t *navi_icon_img = NULL;
static uint8_t last_bt_status = 0;
static uint8_t last_navi_status = 0;

static gui_text_t *text_navi_distance = NULL;
static gui_text_t *text_navi_unit = NULL;
static gui_text_t *text_navi_road = NULL;

static void *img_data = NULL;
// static char dis_str[8] = {0};

// static int16_t x_offset_array[] = {0, 32, 26, 15, -15, -23, -23, -45, -37, 0};
// static int16_t y_offset_array[] = {0, 51, 60, 64, 127, 125, 170, 177, 207, 282};
// static uint16_t distance[] = {0, 100, 20, 20, 110, 15, 50, 40, 50, 130};
// static uint8_t offset_index = 0;
// static int16_t x_offset = 0;
// static int16_t y_offset = 0;
static NVGcolor color = {0};
/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void exit_animation(void *p)
{
    static uint16_t cnt = 0;
    uint16_t cnt_max = 15;
    gui_obj_t *obj = GUI_BASE(p);
    cnt++;
    obj->opacity_value = (cnt_max - cnt) * 255 / cnt_max;

    if (cnt >= cnt_max)
    {
        cnt = 0;
        gui_obj_stop_timer(obj);
        gui_obj_child_free(obj);
    }
}

#if 0
static void run_animation(void *p)
{
    (void)p;
    GUI_WIDGET_POINTER_BY_NAME_ROOT(map_1, "map_1", win_map);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(map_2, "map_2", win_map);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(dist, "distance", win_map);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(dir, "arrow", win_map);
    // int8_t step = 0;
    int16_t distance_value = 0;

    if (x_offset == x_offset_array[offset_index] &&
        y_offset == y_offset_array[offset_index])
    {
        uint8_t size = sizeof(y_offset_array) / sizeof(y_offset_array[0]);
        if (offset_index == size - 1)
        {
            offset_index = 0;
            y_offset = 0;
            x_offset = 0;
            if (map_1->y >= MAP_H)
            {
                map_1->y = -MAP_H;
                map_2->y = 0;
            }
            else
            {
                map_2->y = -MAP_H;
                map_1->y = 0;
            }
        }
        offset_index++;
        size--;
        int16_t x_diff = x_offset_array[(offset_index + 1) % size] - x_offset_array[(offset_index) % size];
        if (x_diff < 0)
        {
            gui_img_set_image_data((gui_img_t *)dir, ARROW_RIGHT_BIN);
        }
        else if (x_diff > 0)
        {
            gui_img_set_image_data((gui_img_t *)dir, ARROW_LEFT_BIN);
        }
        else
        {
            gui_img_set_image_data((gui_img_t *)dir, ARROW_DIRECT_BIN);
        }
    }

    int16_t x_target = x_offset_array[offset_index];
    int16_t y_target = y_offset_array[offset_index];
    int16_t x_target_last = x_offset_array[offset_index - 1];
    int16_t y_target_last = y_offset_array[offset_index - 1];
    int16_t x_diff = x_target - x_target_last;
    int16_t y_diff = y_target - y_target_last;
    if (abs(x_diff) < abs(y_diff))
    {
        if (y_target > y_offset)
        {
            y_offset++;
        }
        else
        {
            y_offset--;
        }
        x_offset = x_target_last + x_diff * (y_offset - y_target_last) / y_diff;
        distance_value = distance[offset_index] * abs(y_target - y_offset) / abs(y_diff);
    }
    else
    {
        if (x_target > x_offset)
        {
            x_offset++;
        }
        else
        {
            x_offset--;
        }
        y_offset = y_target_last + y_diff * (x_offset - x_target_last) / x_diff;
        distance_value = distance[offset_index] * abs(x_target - x_offset) / abs(x_diff);
    }

    map_1->x = x_offset;
    map_2->x = x_offset;
    if (map_1->y >= 0)
    {
        map_1->y = y_offset;
        map_2->y = y_offset - MAP_H;
    }
    else
    {
        map_2->y = y_offset;
        map_1->y = y_offset - MAP_H;
    }

    sprintf(dis_str, "%dm", distance_value);
    gui_text_content_set((gui_text_t *)dist, (void *)dis_str, strlen(dis_str));
    // gui_log("x:%d y:%d \n", x_offset, y_offset);
}
#endif

static void entrance_animation(void *p)
{
    static uint16_t cnt = 0;
    uint16_t cnt_max = 20;
    gui_obj_t *obj = GUI_BASE(p);
    cnt++;

    obj->opacity_value = cnt * 255 / cnt_max;

    if (cnt >= cnt_max)
    {
        cnt = 0;
        gui_obj_stop_timer(obj);
        // gui_obj_create_timer(obj, 10, true, run_animation);
        // gui_obj_start_timer(obj);
    }
}

static void draw_cb(NVGcontext *vg)
{
    nvgBeginPath(vg);
    nvgCircle(vg, 6, 6, 6);  // capable of 20000 steps
    nvgFillColor(vg, color);
    nvgFill(vg);
}

static void pos_indicator_timer(void *p)
{
    NVGcolor start = {0};
    NVGcolor stop = {0};
    if (scene_flag == 1)
    {
        start = COLOR_GREEN;
        stop = COLOR_BLUE;
    }
    else
    {
        start = COLOR_BLUE;
        stop = COLOR_GREEN;
    }
    static uint16_t cnt = 0;
    uint16_t cnt_max = 30;
    cnt++;
    color.r = start.r + (stop.r - start.r) * cnt / cnt_max;
    color.g = start.g + (stop.g - start.g) * cnt / cnt_max;
    color.b = start.b + (stop.b - start.b) * cnt / cnt_max;

    int image_h = 12;
    int image_w = 12;
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, image_w, image_h, draw_cb, img_data);
    if (cnt == cnt_max)
    {
        cnt = 0;
        gui_obj_stop_timer(GUI_BASE(p));
    }
}

static void map_update_timer(void *p)
{
    GUI_UNUSED(p);
    gui_dashboard_t *info = get_dashboard_info();
    if (info == NULL || win_map == NULL)
    {
        return;
    }

    // Monitor bluetooth and navigation status
    uint8_t current_bt_status = info->bt_status;
    uint8_t current_navi_status = info->navi_active;

    // Map visibility logic:
    // Show map only when: bluetooth connected AND navigation active
    // Hide map when: bluetooth disconnected OR navigation inactive
    bool should_show_map = (current_bt_status == 1) && (current_navi_status == 1);

    if (current_bt_status != last_bt_status || current_navi_status != last_navi_status)
    {
        last_bt_status = current_bt_status;
        last_navi_status = current_navi_status;

        if (should_show_map)
        {
            // Show map
            gui_obj_hidden(GUI_BASE(win_map), false);
            gui_log("Map shown: BT=%d, NAVI=%d\n", current_bt_status, current_navi_status);
        }
        else
        {
            // Hide map
            gui_obj_hidden(GUI_BASE(win_map), true);
            gui_log("Map hidden: BT=%d, NAVI=%d\n", current_bt_status, current_navi_status);
        }
    }

    // Update navigation info only when map is visible
    if (should_show_map && text_navi_distance != NULL && text_navi_unit != NULL &&
        text_navi_road != NULL)
    {
        uint8_t *msg = NULL;
        uint8_t *unit = NULL;
        uint8_t *road = NULL;
        uint8_t msg_len = 0;
        uint8_t unit_len = 0;
        uint8_t road_len = 0;

        // Get navigation info
        app_navigation_get_distance_msg(&msg, &msg_len);
        app_navigation_get_unit(&unit, &unit_len);
        app_navigation_get_road_name(&road, &road_len);

        // Update navigation icon based on direction
        uint8_t icon_type = app_navigation_get_icon_type();
        if (navi_icon_img != NULL)
        {
            void *icon_data = NULL;
            switch (icon_type)
            {
            case 1: // Straight
                icon_data = NAVI_C1_BIN;
                break;
            case 2: // Left turn
                icon_data = NAVI_C2_BIN;
                break;
            case 3: // Right turn
                icon_data = NAVI_C3_BIN;
                break;
            case 4: // U-turn
                icon_data = NAVI_C4_BIN;
                break;
            default:
                icon_data = NAVI_C1_BIN;
                break;
            }

            if (icon_data != NULL)
            {
                gui_img_set_image_data(navi_icon_img, icon_data);
            }
        }

        // Update text widgets
        if (msg != NULL && msg_len > 0)
        {
            gui_text_content_set(text_navi_distance, msg, msg_len);
        }

        if (unit != NULL && unit_len > 0)
        {
            gui_text_content_set(text_navi_unit, unit, unit_len);
        }

        if (road != NULL && road_len > 0)
        {
            gui_text_content_set(text_navi_road, road, road_len);
        }
    }
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/
// void map_design(void)
// {
//     gui_win_t *win_map_img = gui_win_create(win_map, 0, X_START, Y_START, MAP_W, MAP_H);
//     gui_img_t *map_1 = gui_img_create_from_mem(win_map_img, "map_1", MAP_BIN, 0, 0, 0, 0);
//     gui_img_t *map_2 = gui_img_create_from_mem(win_map_img, "map_2", MAP_BIN, 0, -MAP_H, 0, 0);
//     map_1->need_clip = true;
//     map_2->need_clip = true;
//     gui_img_t *mask = gui_img_create_from_mem(win_map_img, 0, MAPFADING_BIN, 0, 0, 0, 0);
//     gui_img_set_mode(mask, IMG_SRC_OVER_MODE);

//     gui_img_create_from_mem(win_map, "arrow", ARROW_RIGHT_BIN, 390, 95, 0, 30);
//     gui_text_t *text = gui_text_create(win_map, "distance", 0, 160, 0, 0);
//     sprintf(dis_str, "%dm", distance[1]);
//     gui_text_set(text, (void *)dis_str, GUI_FONT_SRC_TTF,  APP_COLOR_WHITE, strlen(dis_str), 24);
//     gui_text_type_set(text, HARMONYOS_SANS_BOLD_BIN, FONT_SRC_MEMADDR);
//     gui_text_mode_set(text, CENTER);
//     gui_text_rendermode_set(text, 2);

//     // Position ball
//     {
//         color = COLOR_BLUE;
//         int image_h = 12;
//         int image_w = 12;
//         int pixel_bytes = 4;
//         size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
//         if (img_data == NULL)
//         {
//             img_data = gui_lower_malloc(buffer_size);
//         }
//         memset(img_data, 0, buffer_size);
//         gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, image_w, image_h, draw_cb, img_data);
//         gui_img_t *pos = gui_img_create_from_mem(win_map, "pos", img_data, 394, Y_START + 141, 0, 0);
//         gui_img_set_mode(pos, IMG_SRC_OVER_MODE);
//     }

//     offset_index = 0;
//     x_offset = 0;
//     y_offset = 0;
//     gui_obj_create_timer(GUI_BASE(win_map), 10, true, entrance_animation);
//     gui_obj_start_timer(GUI_BASE(win_map));
// }

void map_design(void)
{
    gui_win_t *win_map_img = gui_win_create(win_map, 0, X_START, Y_START, MAP_W, MAP_H);
    gui_obj_create_timer(GUI_BASE(win_map), 10, true, entrance_animation);
    gui_obj_start_timer(GUI_BASE(win_map));

    // Create navigation icon (will be updated dynamically)
    navi_icon_img = gui_img_create_from_mem(win_map_img, "navi_icon", NAVI_C1_BIN, 0, 0, 0, 0);

    gui_img_t *mask = gui_img_create_from_mem(win_map_img, 0, MAPFADING_BIN, 0, 0, 0, 0);
    gui_img_set_mode(mask, IMG_SRC_OVER_MODE);

    // Create navigation text widgets (positioned above the map)
    text_navi_distance = gui_text_create(win_map, "navi_distance", 180, 368, 150, 50);
    gui_text_set(text_navi_distance, "125", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 3, 56);
    gui_text_mode_set(text_navi_distance, LEFT);

    text_navi_unit = gui_text_create(win_map, "navi_unit", 280, 388, 100, 40);
    gui_text_set(text_navi_unit, "KM", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 2, 32);
    gui_text_mode_set(text_navi_unit, LEFT);

    text_navi_road = gui_text_create(win_map, "navi_road", 180, 428, 400, 40);
    gui_text_set(text_navi_road, "Enter XX Road", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 13, 32);
    gui_text_mode_set(text_navi_road, LEFT);


    // Initialize bluetooth and navigation status
    gui_dashboard_t *info = get_dashboard_info();
    if (info != NULL)
    {
        last_bt_status = info->bt_status;
        last_navi_status = info->navi_active;
        // Set initial visibility: show only when BT connected AND navigation active
        bool should_show = (info->bt_status == 1) && (info->navi_active == 1);
        gui_obj_hidden(GUI_BASE(win_map), !should_show);
    }

    // Create single timer for both bluetooth monitoring and navigation info update
    gui_obj_create_timer(GUI_BASE(win_map), 100, true, map_update_timer);
    gui_obj_start_timer(GUI_BASE(win_map));
}

void exit_map(void)
{
    gui_obj_create_timer(GUI_BASE(win_map), 10, true, exit_animation);
    gui_obj_start_timer(GUI_BASE(win_map));
}

void change_pos_indicator_color(void)
{
    GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "pos", win_map);
    gui_obj_create_timer(obj, 10, true, pos_indicator_timer);
    gui_obj_start_timer(obj);
}
void app_map_update_navi_icon(const uint8_t *image_data)
{
    if (win_map == NULL || navi_icon_img == NULL)
    {
        gui_log("Map: win_map or map_img is NULL\n");
        return;
    }

    if (image_data != NULL)
    {
        gui_img_set_image_data(navi_icon_img, image_data);
        gui_log("Map: Navigation icon updated\n");
    }
}
/*============================================================================*
 *                           Test Functions
 *============================================================================*/
#ifdef _HONEYGUI_SIMULATOR_
// Navigation direction constants are defined in app_navigation_adapter.h
void app_check_turn_signal_state(void)
{
    gui_dashboard_t *info = get_dashboard_info();
    if (info != NULL)
    {
        gui_log("=== Turn Signal State ===\n");
        gui_log("BT Status: %d\n", info->bt_status);
        gui_log("Navigation Active: %d\n", info->navi_active);
        gui_log("Left Turn: %d\n", info->led_turn_l_status);
        gui_log("Right Turn: %d\n", info->led_turn_r_status);
        gui_log("======================\n");
    }
}
static void test_navi_update_timer(void *p)
{
    GUI_UNUSED(p);
    static uint8_t test_step = 0;
    static uint16_t test_distance = 1500; // Start with 1.5km
    static uint8_t direction_index = 0;

    test_step++;

    // Simulate navigation data update every 2 seconds
    if (test_step % 20 == 0)
    {
        gui_dashboard_t *info = get_dashboard_info();
        if (info == NULL)
        {
            return;
        }

        // Decrease distance
        test_distance -= 100;
        if (test_distance < 50)
        {
            test_distance = 2000; // Reset to 2km
            direction_index = (direction_index + 1) % 4; // Cycle through 4 directions
        }

        // Select direction and navigation type
        uint8_t navi_type = SIM_GO_STRAIGHT;
        const char *dir_name = "STRAIGHT";

        uint8_t old_turn_l = info->led_turn_l_status;
        uint8_t old_turn_r = info->led_turn_r_status;

        switch (direction_index)
        {
        case 0:
            navi_type = SIM_TURN_LEFT;
            dir_name = "LEFT";
            gui_log("Test: Setting TURN_LEFT (type=%d)\n", navi_type);
            break;
        case 1:
            navi_type = SIM_TURN_RIGHT;
            dir_name = "RIGHT";
            gui_log("Test: Setting TURN_RIGHT (type=%d)\n", navi_type);
            break;
        case 2:
            navi_type = SIM_LEFT_FRONT;
            dir_name = "LEFT_FRONT";
            gui_log("Test: Setting LEFT_FRONT (type=%d)\n", navi_type);
            break;
        case 3:
            navi_type = SIM_GO_STRAIGHT;
            dir_name = "STRAIGHT";
            gui_log("Test: Setting GO_STRAIGHT (type=%d)\n", navi_type);
            break;
        }

        // Prepare test data: [direction, distance_high, distance_low, road_name...]
        uint8_t test_data[50] = {0};
        test_data[0] = navi_type; // Navigation type
        test_data[1] = (test_distance >> 8) & 0xFF; // Distance high byte
        test_data[2] = test_distance & 0xFF; // Distance low byte

        // Add road name
        const uint8_t road_name_utf8[] = {0xE8, 0x8B, 0x8F, 0xE5, 0xB7, 0x9E, 0xE8, 0xB7, 0xAF, 0x00};

        const char *road_name = (const char *)road_name_utf8;

        uint8_t road_len = strlen(road_name);
        memcpy(&test_data[3], road_name, road_len);

        gui_log("Test: Calling app_dashboard_data_update_navi_status with type=%d\n", navi_type);

        // Update navigation status
        app_dashboard_data_update_navi_status(test_data, 3 + road_len);

        if (info != NULL)
        {
            gui_log("Test: Turn signals - old(L:%d,R:%d) -> new(L:%d,R:%d)\n",
                    old_turn_l, old_turn_r,
                    info->led_turn_l_status, info->led_turn_r_status);

            gui_log("Test: Navigation updated - type=%d, distance=%dm, direction=%s\n",
                    navi_type, test_distance, dir_name);
        }
        static uint8_t check_counter = 0;
        check_counter++;
        if (check_counter == 2)
        {
            app_check_turn_signal_state();
            check_counter = 0;
        }
    }
}

/**
 * @brief Start navigation test mode
 * This function simulates navigation data updates for UI testing
 */
void app_map_test_navigation(void)
{
    static bool test_started = false;

    // Prevent multiple test instances
    if (test_started)
    {
        gui_log("Test: Navigation test already running\n");
        return;
    }

    if (win_map == NULL)
    {
        gui_log("Test: Map not initialized\n");
        return;
    }

    // Enable navigation and bluetooth for testing
    gui_dashboard_t *info = get_dashboard_info();
    if (info != NULL)
    {
        info->bt_status = 1;
        info->navi_active = 1;
        gui_log("Test: Enabled BT and Navigation\n");
    }

    // Show map
    gui_obj_hidden(GUI_BASE(win_map), false);

    // Start test timer
    gui_obj_create_timer(GUI_BASE(win_map), 100, true, test_navi_update_timer);
    gui_obj_start_timer(GUI_BASE(win_map));

    test_started = true;
    gui_log("Test: Navigation test mode started\n");
}
#endif