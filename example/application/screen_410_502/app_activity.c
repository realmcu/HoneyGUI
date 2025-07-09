/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include <time.h>
#include "root_image_hongkong/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "cJSON.h"
#include "gui_canvas_img.h"
#include "gui_canvas_rect.h"
#include "guidef.h"
#include "gui_view.h"
#include "app_hongkong.h"
#include "tp_algo.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#ifdef M_PI
#undef M_PI
#define M_PI 3.1415926f
#else
#define M_PI 3.1415926f
#endif

#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "activity_view"
#define COUNT_MAX 250
#define RADIUS 200

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void activity_design(gui_view_t *view);
static void clear_activity(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
const static gui_view_descriptor_t *heartrate_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = activity_design,
    .on_switch_out = clear_activity,
};

extern char *cjson_content;
static uint8_t *img_data = NULL;
static gui_img_t *img;
static uint16_t count = COUNT_MAX; //for timer
static bool draw_flag = 0; //0: get new json data
static bool has_draw_bg = 0; // only draw background once
static char move_content[30] = {0};
static char ex_content[30] = {0};
static char stand_content[30] = {0};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
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

static void clear_activity(gui_view_t *view)
{
    if (img_data)
    {
        gui_lower_free(img_data);
        img_data = NULL;

        count = COUNT_MAX;
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
                    GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "move_text", current_view);
                    sprintf(move_content, "Move:%d/20000steps", move_val);
                    gui_text_content_set((gui_text_t *)obj, move_content, strlen(move_content));
                }
                {
                    GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "ex_text", current_view);
                    sprintf(ex_content, "Exercise:%d/60min", ex_val);
                    gui_text_content_set((gui_text_t *)obj, ex_content, strlen(ex_content));
                }
                {
                    GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "stand_text", current_view);
                    sprintf(stand_content, "Stand:%d/30times", stand_val);
                    gui_text_content_set((gui_text_t *)obj, stand_content, strlen(stand_content));
                }
            }
        }
        // clear
        cJSON_Delete(root);
    }

    uint8_t line_width = 32;
    uint8_t radius_max = RADIUS - line_width;
    uint8_t interval = 6;
    float progress = count / (float)COUNT_MAX;

    if (!has_draw_bg)
    {
        has_draw_bg = true;
        nvgBeginPath(vg);
        nvgArc(vg, RADIUS, RADIUS, radius_max, 3 * M_PI / 2,
               M_PI * 3.5f, NVG_CW);
        nvgStrokeWidth(vg, line_width);
        nvgStrokeColor(vg, nvgRGB(58, 23, 29));
        nvgStroke(vg);

        nvgBeginPath(vg);
        nvgArc(vg, RADIUS, RADIUS, radius_max - (line_width + interval), 3 * M_PI / 2,
               M_PI * 3.5f, NVG_CW);
        nvgStrokeWidth(vg, line_width);
        nvgStrokeColor(vg, nvgRGB(30, 55, 25));
        nvgStroke(vg);

        nvgBeginPath(vg);
        nvgArc(vg, RADIUS, RADIUS, radius_max - 2 * (line_width + interval), 3 * M_PI / 2,
               M_PI * 3.5f, NVG_CW);
        nvgStrokeWidth(vg, line_width);
        nvgStrokeColor(vg, nvgRGB(22, 50, 47));
        nvgStroke(vg);
    }

    nvgBeginPath(vg);
    nvgArc(vg, RADIUS, RADIUS, radius_max, 3 * M_PI / 2,
           M_PI * (1.5f + 2.0f * move_val * progress / 20000.0f), NVG_CW);  // cap 20000 steps
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(230, 67, 79));
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgArc(vg, RADIUS, RADIUS, radius_max - (line_width + interval), 3 * M_PI / 2,
           M_PI * (1.5f + 2.0f * ex_val * progress / 60.0f), NVG_CW);  // cap 60 min.
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(186, 253, 79));
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgArc(vg, RADIUS, RADIUS, radius_max - 2 * (line_width + interval), 3 * M_PI / 2,
           M_PI * (1.5f + 2.0f * stand_val * progress / 30.0f), NVG_CW); // cap 30 times
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(117, 230, 229));
    nvgStroke(vg);
    gui_fb_change();
    // gui_log("progress: %f\n", progress);
}

static void activity_timer_cb(void *obj)
{
    if (current_view->current_transition_style < SWITCH_OUT_NONE_ANIMATION &&
        gui_view_get_next() != NULL)
    {
        return;
    }

    gui_obj_timer_t *timer = img->base.timer;

    count += timer->interval_ms;
    uint8_t *img_data = (void *)gui_img_get_image_data(img);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, RADIUS * 2, RADIUS * 2,
                                      arc_activity_cb, img_data);
    gui_img_set_image_data(img, img_data);
    gui_img_refresh_size(img);
    if (count >= COUNT_MAX)
    {
        count = 0;
        gui_obj_stop_timer(GUI_BASE(img));
    }
}

static void enter_timer_cb(void *obj)
{
    gui_view_t *view = gui_view_get_current();
    if (strcmp(GUI_BASE(view)->name, CURRENT_VIEW_NAME) == 0)
    {
        count = 0;
        has_draw_bg = false;
        uint8_t *img_data = (void *)gui_img_get_image_data(img);

        int image_h = RADIUS * 2;
        int image_w = RADIUS * 2;
        int pixel_bytes = 4;
        size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        memset(img_data, 0, buffer_size);
        gui_obj_create_timer(GUI_BASE(img), 10, true, activity_timer_cb);
        gui_obj_start_timer(GUI_BASE(img));
        gui_obj_stop_timer(GUI_BASE(obj));
    }
}

static void activity_design(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    has_draw_bg = false;

    // text
    {
        gui_text_t *move_text = gui_text_create(obj, "move_text", 150, 400, 0, 0);
        gui_text_set(move_text, (void *)move_content, GUI_FONT_SRC_TTF, gui_rgb(230, 67, 79),
                     strlen(move_content), 32);
        gui_text_type_set(move_text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(move_text, LEFT);
        gui_text_rendermode_set(move_text, 2);

        gui_text_t *ex_text = gui_text_create(obj, "ex_text", 150, 430, 0, 0);
        gui_text_set(ex_text, (void *)ex_content, GUI_FONT_SRC_TTF, gui_rgb(186, 253, 79),
                     strlen(ex_content), 32);
        gui_text_type_set(ex_text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(ex_text, LEFT);
        gui_text_rendermode_set(ex_text, 2);
        gui_text_t *stand_text = gui_text_create(obj, "stand_text", 150, 460, 0, 0);
        gui_text_set(stand_text, (void *)stand_content, GUI_FONT_SRC_TTF, gui_rgb(117, 230, 229),
                     strlen(stand_content), 32);
        gui_text_type_set(stand_text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(stand_text, LEFT);
        gui_text_rendermode_set(stand_text, 2);
    }
    // img
    int image_h = RADIUS * 2;
    int image_w = RADIUS * 2;
    int pixel_bytes = 4;
    size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
    if (img_data == NULL)
    {
        img_data = gui_lower_malloc(buffer_size);
    }
    memset(img_data, 0, buffer_size);
    img = gui_img_create_from_mem(obj, 0, (void *)img_data, SCREEN_WIDTH / 2 - RADIUS, 0, 0, 0);
    gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    gui_img_set_quality(img, true);
    draw_flag = 0;

    // view layout
    const char *name = GUI_BASE(gui_view_get_current())->name;
    if (strcmp(name, "watchface_view") == 0 || strcmp(name, "heartrate_view") == 0)
    {
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, image_w, image_h, arc_activity_cb,
                                          img_data);
        gui_img_refresh_size(img);
        gui_obj_create_timer(obj, 10, true, enter_timer_cb);
        gui_view_switch_on_event(view, watchface_view, SWITCH_OUT_TO_RIGHT_USE_ROTATE,
                                 SWITCH_IN_FROM_LEFT_USE_ROTATE,
                                 GUI_EVENT_TOUCH_MOVE_RIGHT);
        gui_view_switch_on_event(view, heartrate_view, SWITCH_OUT_TO_LEFT_USE_ROTATE,
                                 SWITCH_IN_FROM_RIGHT_USE_ROTATE,
                                 GUI_EVENT_TOUCH_MOVE_LEFT);
    }
    else
    {
        count = 0;
        gui_obj_create_timer(GUI_BASE(img), 10, true, activity_timer_cb);

        gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                                 SWITCH_IN_ANIMATION_FADE,
                                 GUI_EVENT_KB_SHORT_CLICKED);
    }
}
