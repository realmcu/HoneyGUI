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
static uint8_t *img_data = NULL;
static float progress = 0;
static bool animate_flag = 0;
static bool draw_flag = 0;
static char move_content[30], ex_content[30], stand_content[30];

static gui_text_t *move_text, *ex_text, *stand_text;
static gui_img_t *img;

void clear_activity(void)
{
    move_text = NULL;
    ex_text = NULL;
    stand_text = NULL;
    img = NULL;
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
                    sprintf(move_content, "Move: %d/20000 steps", move_val);
                    gui_text_content_set((gui_text_t *)move_text, move_content, strlen(move_content));
                    // gui_text_convert_to_img((gui_text_t *)move_text, RGB565);
                }
                {
                    sprintf(ex_content, "Exercise: %d/60 min", ex_val);
                    gui_text_content_set(ex_text, ex_content, strlen(ex_content));
                    // gui_text_convert_to_img(ex_text, RGB565);
                }
                {
                    sprintf(stand_content, "Stand: %d/30 times", stand_val);
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
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(canvas_activity_animation)
{
    if (animate_flag)
    {
        progress = ((gui_img_t *)this_widget)->animate->progress_percent;
        if (progress == 1.0f)
        {
            animate_flag = 0;
        }
        uint8_t *img_data = (void *)gui_img_get_image_data(this_widget);
        memset(img_data, 0, (size_t)p);
        gui_canvas_output_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 200, 200, arc_activity_cb, img_data);
        gui_img_set_image_data(this_widget, img_data);
    }
}

void activity_app(gui_obj_t *obj)
{
    // text
    if (!move_text)
    {
        move_text = gui_text_create(obj, "move_text", 150, 300, 0, 0);
        gui_text_set(move_text, (void *)move_content, GUI_FONT_SRC_BMP, gui_rgb(230, 67, 79),
                     strlen(move_content), 32);
        gui_text_type_set(move_text, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(move_text, LEFT);
        // gui_text_convert_to_img(move_text, RGB565);

        ex_text = gui_text_create(obj, "ex_text", 150, 350, 0, 0);
        gui_text_set(ex_text, (void *)ex_content, GUI_FONT_SRC_BMP, gui_rgb(186, 253, 79),
                     strlen(ex_content), 32);
        gui_text_type_set(ex_text, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(ex_text, LEFT);
        // gui_text_convert_to_img(ex_text, RGB565);

        stand_text = gui_text_create(obj, "stand_text", 150, 400, 0, 0);
        gui_text_set(stand_text, (void *)stand_content, GUI_FONT_SRC_BMP, gui_rgb(117, 230, 229),
                     strlen(stand_content), 32);
        gui_text_type_set(stand_text, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(stand_text, LEFT);
        // gui_text_convert_to_img(stand_text, RGB565);
    }
    {
        int image_h = 200,
            image_w = 200,
            pixel_bytes = 4;
        size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        if (img_data == NULL)
        {
            img_data = gui_lower_malloc(buffer_size);
            gui_log("enter gui_lower_malloc\n");
        }
        memset(img_data, 0, buffer_size);
        gui_canvas_output_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, image_w, image_h, arc_activity_cb, img_data);
        if (img)
        {
            gui_obj_tree_free(img);
        }
        img = gui_img_create_from_mem(obj, 0, (void *)img_data, 50,
                                      50, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_img_set_animate(img, 1000, 0, canvas_activity_animation, (void *)buffer_size);
    }
    animate_flag = 1;
    draw_flag = 0;
}