#include "root_image_hongkong/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include <math.h>
#include "cJSON.h"
#include "gui_canvas_img.h"
#include "gui_canvas_rect.h"
#include "guidef.h"
#include "gui_view.h"
#include "app_hongkong.h"
#include "tp_algo.h"

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502

#define CURRENT_VIEW_NAME "activity_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
const static gui_view_descriptor_t *heartrate_view = NULL;
void activity_app(gui_view_t *view);
void clear_activity();

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = activity_app,
    .on_switch_out = clear_activity,
};

static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    menu_view = gui_view_descriptor_get("menu_view");
    watchface_view = gui_view_descriptor_get("watchface_view");
    heartrate_view = gui_view_descriptor_get("heartrate_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

extern char *cjson_content;
static uint8_t *img_data = NULL;
static gui_img_t *img;
static float progress = 0;
static bool draw_flag = 0;
static size_t buffer_size = 0;
static char move_content[30], ex_content[30], stand_content[30];

static gui_text_t *move_text, *ex_text, *stand_text;

void clear_activity()
{
    if (img_data)
    {
        gui_lower_free(img_data);
        img_data = NULL;
    }
}

static void arc_activity_cb(NVGcontext *vg)
{
    cJSON *root;
    static int move_val = 0;
    static int ex_val = 0;
    static int stand_val = 0;

    if (!draw_flag)
    {
        draw_flag = 1;
        if (!cjson_content)
        {
            return;
        }
        else
        {
            root = cJSON_Parse(cjson_content);
            if (!root)
            {
                gui_log("Error parsing JSON!\r\n");
                return;
            }
        }
        // parse activity array
        cJSON *activity_array = cJSON_GetObjectItemCaseSensitive(root, "activity");
        if (cJSON_IsArray(activity_array))
        {
            cJSON *act = cJSON_GetArrayItem(activity_array, 0);
            if (!act)
            {
                gui_log("get activity_array unsuccessful\n");
            }
            else
            {
                cJSON *move = cJSON_GetObjectItemCaseSensitive(act, "move");
                cJSON *ex = cJSON_GetObjectItemCaseSensitive(act, "exercise");
                cJSON *stand = cJSON_GetObjectItemCaseSensitive(act, "stand");
                move_val = move->valueint;
                ex_val = ex->valueint;
                stand_val = stand->valueint;

                {
                    sprintf(move_content, "Move:%d/20000steps", move_val);
                    gui_text_content_set((gui_text_t *)move_text, move_content, strlen(move_content));
                    // gui_text_convert_to_img((gui_text_t *)move_text, RGB565);
                }
                {
                    sprintf(ex_content, "Exercise:%d/60min", ex_val);
                    gui_text_content_set(ex_text, ex_content, strlen(ex_content));
                    // gui_text_convert_to_img(ex_text, RGB565);
                }
                {
                    sprintf(stand_content, "Stand:%d/30times", stand_val);
                    gui_text_content_set(stand_text, stand_content, strlen(stand_content));
                    // gui_text_convert_to_img(stand_text, RGB565);
                }
            }
        }
        // clear
        cJSON_Delete(root);
    }

    uint8_t line_width = 16;
    uint8_t radius_max = 100 - line_width / 2;
    uint8_t interval = 5;

    nvgBeginPath(vg);
    nvgArc(vg, 100, 100, radius_max, 3 * M_PI / 2,
           M_PI * 3.5f, NVG_CW);
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(58, 23, 29));
    nvgStroke(vg);
    nvgBeginPath(vg);
    nvgArc(vg, 100, 100, radius_max, 3 * M_PI / 2,
           M_PI * (1.5f + 2.0f * move_val * progress / 20000.0f), NVG_CW);  // cap 20000 steps
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(230, 67, 79));
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgArc(vg, 100, 100, radius_max - (line_width + interval), 3 * M_PI / 2,
           M_PI * 3.5f, NVG_CW);
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(30, 55, 25));
    nvgStroke(vg);
    nvgBeginPath(vg);
    nvgArc(vg, 100, 100, radius_max - (line_width + interval), 3 * M_PI / 2,
           M_PI * (1.5f + 2.0f * ex_val * progress / 60.0f), NVG_CW);  // cap 60 min.
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(186, 253, 79));
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgArc(vg, 100, 100, radius_max - 2 * (line_width + interval), 3 * M_PI / 2,
           M_PI * 3.5f, NVG_CW);
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(22, 50, 47));
    nvgStroke(vg);
    nvgBeginPath(vg);
    nvgArc(vg, 100, 100, radius_max - 2 * (line_width + interval), 3 * M_PI / 2,
           M_PI * (1.5f + 2.0f * stand_val * progress / 30.0f), NVG_CW); // cap 30 times
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(117, 230, 229));
    nvgStroke(vg);
    gui_fb_change();
    // gui_log("progress: %f\n", progress);
}
// cppcheck-suppress syntaxError
static void activity_timer_cb(void *obj)
{
    static uint16_t count = 0;
    uint16_t count_max = 500;

    gui_obj_timer_t *timer = img->base.timer;

    count += timer->interval_ms;
    progress = count / (float)count_max;
    uint8_t *img_data = (void *)gui_img_get_image_data(img);
    memset(img_data, 0, buffer_size);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 200, 200, arc_activity_cb, img_data);
    gui_img_set_image_data(img, img_data);
    gui_img_refresh_size(img);
    if (count >= count_max)
    {
        count = 0;
        progress = 0;
        gui_obj_stop_timer(GUI_BASE(img));
    }
}

static void back2menu_cb()
{
    gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void return_timer_cb()
{
    touch_info_t *tp = tp_get_info();
    GUI_RETURN_HELPER(tp, SCREEN_WIDTH, back2menu_cb)
}

static void win_cb(void *obj)
{
    gui_view_t *view = gui_view_get_current();
    if (strcmp(GUI_BASE(view)->name, CURRENT_VIEW_NAME) == 0)
    {
        gui_obj_create_timer(GUI_BASE(img), 20, true, activity_timer_cb);
        gui_obj_start_timer(GUI_BASE(img));
        gui_obj_stop_timer(GUI_BASE(obj));
    }
}

void activity_app(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    gui_win_t *win = gui_win_create(obj, "win", 0, 0, 0, 0);

    // text
    {
        move_text = gui_text_create(obj, "move_text", 150, 300, 0, 0);
        gui_text_set(move_text, (void *)move_content, GUI_FONT_SRC_TTF, gui_rgb(230, 67, 79),
                     strlen(move_content), 32);
        gui_text_type_set(move_text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(move_text, LEFT);
        gui_text_rendermode_set(move_text, 2);
        // gui_text_convert_to_img(move_text, RGB565);

        ex_text = gui_text_create(obj, "ex_text", 150, 350, 0, 0);
        gui_text_set(ex_text, (void *)ex_content, GUI_FONT_SRC_TTF, gui_rgb(186, 253, 79),
                     strlen(ex_content), 32);
        gui_text_type_set(ex_text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(ex_text, LEFT);
        gui_text_rendermode_set(ex_text, 2);
        // gui_text_convert_to_img(ex_text, RGB565);

        stand_text = gui_text_create(obj, "stand_text", 150, 400, 0, 0);
        gui_text_set(stand_text, (void *)stand_content, GUI_FONT_SRC_TTF, gui_rgb(117, 230, 229),
                     strlen(stand_content), 32);
        gui_text_type_set(stand_text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(stand_text, LEFT);
        gui_text_rendermode_set(stand_text, 2);
        // gui_text_convert_to_img(stand_text, RGB565);
    }
    {
        int image_h = 200,
            image_w = 200,
            pixel_bytes = 4;
        buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        if (img_data == NULL)
        {
            img_data = gui_lower_malloc(buffer_size);
            // gui_log("enter gui_lower_malloc\n");
        }
        memset(img_data, 0, buffer_size);
        // gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, image_w, image_h, arc_activity_cb, img_data);
        img = gui_img_create_from_mem(obj, 0, (void *)img_data, 50,
                                      50, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    draw_flag = 0;

    const char *name = GUI_BASE(gui_view_get_current())->name;
    if (strcmp(name, "watchface_view") == 0 || strcmp(name, "heartrate_view") == 0)
    {
        gui_obj_create_timer(GUI_BASE(win), 100, true, win_cb);
        gui_obj_start_timer(GUI_BASE(win));
        gui_view_switch_on_event(view, watchface_view, SWITCH_OUT_TO_RIGHT_USE_CUBE,
                                 SWITCH_IN_FROM_LEFT_USE_CUBE,
                                 GUI_EVENT_TOUCH_MOVE_RIGHT);
        gui_view_switch_on_event(view, heartrate_view, SWITCH_OUT_TO_LEFT_USE_CUBE,
                                 SWITCH_IN_FROM_RIGHT_USE_CUBE,
                                 GUI_EVENT_TOUCH_MOVE_LEFT);
    }
    else
    {
        gui_obj_create_timer(GUI_BASE(img), 20, true, activity_timer_cb);
        gui_obj_start_timer(GUI_BASE(img));

        gui_obj_create_timer(GUI_BASE(win), 10, true, return_timer_cb);
        gui_obj_start_timer(GUI_BASE(win));
    }
}
