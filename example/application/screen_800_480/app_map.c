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

/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define X_START 142
#define Y_START 198
#define MAP_H 282 + 1
#define MAP_W 517

#define COLOR_GREEN nvgRGB(90, 180, 150)
#define COLOR_BLUE  nvgRGB(0, 220, 227)

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
gui_win_t *win_map = NULL;

static void *img_data = NULL;
static char dis_str[8] = {0};

static int16_t x_offset_array[] = {0, 32, 26, 15, -15, -23, -23, -45, -37, 0};
static int16_t y_offset_array[] = {0, 51, 60, 64, 127, 125, 170, 177, 207, 282};
static uint16_t distance[] = {0, 100, 20, 20, 110, 15, 50, 40, 50, 130};
static uint8_t offset_index = 0;
static int16_t x_offset = 0;
static int16_t y_offset = 0;
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
    nvgCircle(vg, 6, 6, 6);  // cap 20000 steps
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
    gui_img_create_from_mem(win_map_img, "map", MAP_BIN, 0, 0, 0, 0);
    gui_img_t *mask = gui_img_create_from_mem(win_map_img, 0, MAPFADING_BIN, 0, 0, 0, 0);
    gui_img_set_mode(mask, IMG_SRC_OVER_MODE);
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