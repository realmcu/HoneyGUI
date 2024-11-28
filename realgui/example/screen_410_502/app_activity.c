#include "root_image_hongkong/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_app.h"
#include "time.h"
#include "gui_curtainview.h"
#include <math.h>
#include "cJSON.h"
#include "gui_canvas_img.h"
#include "gui_curtain.h"
#include "gui_canvas_rect.h"
#include "guidef.h"
#include <gui_tabview.h>

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502

extern char *cjson_content;
static float progress = 0;
static bool animate_flag = 0;
static bool draw_flag = 0;

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
            }
        }
        // clear
        cJSON_Delete(root);
    }

    float line_width = 25.0f;
    float radius_max = SCREEN_WIDTH / 2.0f - 40.0f;
    float interval = 10.0f;

    nvgBeginPath(vg);
    nvgArc(vg, SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, radius_max, 3 * M_PI / 2,
           M_PI * 3.5f, NVG_CW);
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(58, 23, 29));
    nvgStroke(vg);
    nvgBeginPath(vg);
    nvgArc(vg, SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, radius_max, 3 * M_PI / 2,
           M_PI * (1.5f + 2.0f * move_val * progress / 20000.0f), NVG_CW);  // cap 20000 steps
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(230, 67, 79));
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgArc(vg, SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, radius_max - (line_width + interval), 3 * M_PI / 2,
           M_PI * 3.5f, NVG_CW);
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(30, 55, 25));
    nvgStroke(vg);
    nvgBeginPath(vg);
    nvgArc(vg, SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, radius_max - (line_width + interval), 3 * M_PI / 2,
           M_PI * (1.5f + 2.0f * ex_val * progress / 60.0f), NVG_CW);  // cap 60 min.
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(186, 253, 79));
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgArc(vg, SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, radius_max - 2 * (line_width + interval),
           3 * M_PI / 2,
           M_PI * 3.5f, NVG_CW);
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(22, 50, 47));
    nvgStroke(vg);
    nvgBeginPath(vg);
    nvgArc(vg, SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, radius_max - 2 * (line_width + interval),
           3 * M_PI / 2,
           M_PI * (1.5f + 2.0f * stand_val * progress / 30.0f), NVG_CW); // cap 30 times
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(117, 230, 229));
    nvgStroke(vg);
    gui_fb_change();
    // gui_log("progress: %f\n", progress);
}

static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(canvas_activity_animation)
{
    if (animate_flag)
    {
        draw_flag = 0;
        progress = ((gui_img_t *)this_widget)->animate->progress_percent;
        if (progress == 1.0f)
        {
            animate_flag = 0;
        }
        uint8_t *img_data = (void *)gui_img_get_image_data(this_widget);
        memset(img_data, 0, (size_t)p);
        gui_canvas_output_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, SCREEN_WIDTH, SCREEN_WIDTH, arc_activity_cb,
                                 img_data);
        gui_img_set_image_data(this_widget, img_data);
    }
}

void activity_app(gui_obj_t *obj)
{
    // gui_win_t *win = gui_win_create(obj, "app_activity_win", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    {
        int image_h = SCREEN_WIDTH,
            image_w = SCREEN_WIDTH,
            pixel_bytes = 4;
        size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        uint8_t *img_data = gui_lower_malloc(buffer_size);
        memset(img_data, 0, buffer_size);
        gui_canvas_output_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, image_w, image_h, arc_activity_cb, img_data);
        gui_img_t *img = gui_img_create_from_mem(obj, 0, (void *)img_data, 0,
                                                 45, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_img_set_animate(img, 1000, 0, canvas_activity_animation, (void *)buffer_size);
    }
    animate_flag = 1;
}