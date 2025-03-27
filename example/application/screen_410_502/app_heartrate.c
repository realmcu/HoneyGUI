#include "root_image_hongkong/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_watch_gradient_spot.h"
#include "gui_text.h"
#include "gui_watchface_gradient.h"
#include "gui_cardview.h"
#include "gui_card.h"
#include "gui_tab.h"
#include "gui_app.h"
#include "time.h"
#include "gui_curtainview.h"
#include "tp_algo.h"
#include <math.h>
#include "cJSON.h"
#include "gui_canvas_img.h"
#include "gui_curtain.h"
#include "gui_canvas_rect.h"
#include "app_hongkong.h"
#include "gui_return.h"

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502
#define COLOR_RED gui_rgb(255,0,0)
#define COLOR_SILVER gui_rgb(192,192,192)
#define COLOR_SILVER_OPACITY(opacity) gui_rgba(192,192,192, opacity)

#define CURRENT_VIEW_NAME "heartrate_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *music_view = NULL;
const static gui_view_descriptor_t *activity_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
void heart_rate_app(gui_view_t *view);

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .design_cb = heart_rate_app,
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
    activity_view = gui_view_descriptor_get("activity_view");
    music_view = gui_view_descriptor_get("music_view");
    menu_view = gui_view_descriptor_get("menu_view");
    watchface_view = gui_view_descriptor_get("watchface_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static gui_win_t *win_hb;
static uint8_t *img_data = NULL;

extern void *text_num_array[];
extern char *cjson_content;
extern uint8_t canvas_update_flag;

// heart_rate
static void win_hb_cb(gui_win_t *win)
{
#ifdef __WIN32
    extern char *read_file(const char *file_path);
    if (win->animate->Beginning_frame)
    {
        if (strcmp(gui_current_app()->screen.name, "app_hongkong") != 0)
        {
            char *temp = cjson_content;
            cjson_content = read_file(NULL);
            if (!cjson_content)
            {
                cjson_content = temp;
                perror("fopen");
            }
            else
            {
                free(temp);
            }
        }
        canvas_update_flag = 0b1111;
    }
#endif
}

static void heartrate_graph(NVGcontext *vg)
{
    // draw split line
    {
        nvgBeginPath(vg);
        nvgRect(vg, 31 + 86 * 0, 78, 5,
                210); // the X/Y-axis coordinate relative to parent widget
        nvgFillColor(vg, nvgRGBA(192, 192, 192, 255)); //silver
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgRect(vg, 31 + 86 * 1, 78, 5,
                210); // the X/Y-axis coordinate relative to parent widget
        nvgFillColor(vg, nvgRGBA(192, 192, 192, 255)); //silver
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgRect(vg, 31 + 86 * 2, 78, 5,
                210); // the X/Y-axis coordinate relative to parent widget
        nvgFillColor(vg, nvgRGBA(192, 192, 192, 255)); //silver
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgRect(vg, 31 + 86 * 3, 78, 5,
                210); // the X/Y-axis coordinate relative to parent widget
        nvgFillColor(vg, nvgRGBA(192, 192, 192, 255)); //silver
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgRect(vg, 31 + 86 * 4, 78, 5,
                210); // the X/Y-axis coordinate relative to parent widget
        nvgFillColor(vg, nvgRGBA(192, 192, 192, 255)); //silver
        nvgFill(vg);
    }

    float x = 36.0f;
    float y = 78.0f;
    float w = 344.0f;
    float h = 210.0f;

    float samples[4] = {0};
    float sx[4], sy[4];
    float dx = w / 4.0f;
    int i;

    cJSON *root;
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
    // parse heartbreak array
    cJSON *hb_array = cJSON_GetObjectItemCaseSensitive(root, "heart_rate");
    if (cJSON_IsArray(hb_array))
    {
        cJSON *hb = cJSON_GetArrayItem(hb_array, 0);
        if (!hb)
        {
            gui_log("get hb_array unsuccessful\n");
        }
        else
        {
            cJSON *AM12 = cJSON_GetObjectItemCaseSensitive(hb, "AM12");
            cJSON *AM6 = cJSON_GetObjectItemCaseSensitive(hb, "AM6");
            cJSON *PM12 = cJSON_GetObjectItemCaseSensitive(hb, "PM12");
            cJSON *PM6 = cJSON_GetObjectItemCaseSensitive(hb, "PM6");
            samples[0] = AM12->valueint;
            samples[1] = AM6->valueint;
            samples[2] = PM12->valueint;
            samples[3] = PM6->valueint;
        }
    }
    // clear
    cJSON_Delete(root);

    // sprintf(hr_content, "%d", (uint8_t)samples[2]);
    // GUI_WIDGET_POINTER_BY_NAME(t, "HR_TXT");
    // gui_text_content_set((gui_text_t *)t, hr_content, strlen(hr_content));
    uint8_t number = (uint8_t)samples[2];
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(img_percentile, "hr_content_percentile", win_hb);
        if (number / 100)
        {
            img_percentile->not_show = 0;
        }
        else
        {
            img_percentile->not_show = 1;
        }
        // gui_log("number = %d\n", number);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(img_decimal, "hr_content_decimal", win_hb);
        gui_img_set_attribute((gui_img_t *)img_decimal, img_decimal->name,
                              text_num_array[(number % 100) / 10], img_decimal->x, img_decimal->y);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(img_single, "hr_content_single", win_hb);
        gui_img_set_attribute((gui_img_t *)img_single, img_single->name, text_num_array[number % 10],
                              img_single->x, img_single->y);
    }

    for (i = 0; i < 4; i++)
    {
        sx[i] = x + i * dx;
        // sy[i] = y + h * samples[i] * 0.7f;
        sy[i] = y + h * (1 - (samples[i] - 40.0f) / 120.0f);
    }
    // Graph background
    NVGpaint bg = nvgLinearGradient(vg, x, y, x, y + h, nvgRGBA(255, 0, 0, 0), nvgRGBA(255, 0, 0, 150));
    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0]);
    for (i = 1; i < 4; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1], sx[i] - dx * 0.5f, sy[i], sx[i], sy[i]);
    }

    nvgLineTo(vg, x + w - dx, y + h);
    nvgLineTo(vg, x, y + h);
    nvgFillPaint(vg, bg);
    nvgFill(vg);
    // Graph line
    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0] + 2);
    for (i = 1; i < 4; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1] + 2, sx[i] - dx * 0.5f, sy[i] + 2, sx[i],
                    sy[i] + 2);
    }
    nvgStrokeColor(vg, nvgRGBA(255, 0, 0, 255));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0]);
    for (i = 1; i < 4; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1], sx[i] - dx * 0.5f, sy[i], sx[i], sy[i]);
    }

    nvgStrokeColor(vg, nvgRGBA(255, 0, 0, 255));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);

    // Graph sample pos
    nvgBeginPath(vg);
    for (i = 0; i < 4; i++)
    {
        nvgCircle(vg, sx[i], sy[i], 4.0f);
    }

    nvgFillColor(vg, nvgRGBA(255, 0, 0, 255));
    nvgFill(vg);
    nvgBeginPath(vg);
    for (i = 0; i < 4; i++)
    {
        nvgCircle(vg, sx[i], sy[i], 2.0f);
    }

    nvgFillColor(vg, nvgRGBA(220, 220, 220, 255));
    nvgFill(vg);

    nvgStrokeWidth(vg, 1.0f);
}

static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(hr_animation)
{
    if (canvas_update_flag & 0x01)
    {
        canvas_update_flag &= 0b1110;
        uint8_t *img_data = (void *)gui_img_get_image_data(this_widget);
        memset(img_data, 0, (size_t)p);
        gui_canvas_output_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, SCREEN_WIDTH, 300, heartrate_graph, img_data);
        gui_img_set_image_data(this_widget, img_data);
    }
}
void clear_heart_rate_cache(void)
{
    if (img_data)
    {
        gui_lower_free(img_data);
        img_data = NULL;
    }
}
static void return_to_menu()
{
    gui_view_switch_direct(current_view, menu_view, VIEW_ANIMATION_8, VIEW_ANIMATION_5);
}
static void return_to_watchface()
{
    gui_view_switch_direct(current_view, watchface_view, VIEW_ANIMATION_8, VIEW_ANIMATION_5);
}

void heart_rate_app(gui_view_t *view)
{
    clear_mem();

    gui_obj_t *obj = GUI_BASE(view);

    // gui_log("current app:%s\n", gui_current_app()->screen.name);

    win_hb = gui_win_create(obj, "hb_win", 0, 0, SCREEN_WIDTH,
                            SCREEN_HEIGHT);
    gui_canvas_rect_create(GUI_BASE(win_hb), "hb_background", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                           gui_rgb(0, 0, 0));

    gui_win_set_animate(win_hb, 2000, -1, (gui_animate_callback_t)win_hb_cb, win_hb);
    {
        char *text = "Current heartrate";
        int font_size = 32;
        gui_text_t *t = gui_text_create(win_hb, "txt", 51, 318, 0,
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255), strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        // sprintf(hr_content, "69");
        int text_w = 33;
        // uint8_t number = atoi(hr_content);
        gui_img_t *img = gui_img_create_from_mem(win_hb, "hr_content_percentile", text_num_array[1],
                                                 51, 368, 0, 0);
        GUI_BASE(img)->not_show = 1;
        img = gui_img_create_from_mem(win_hb, "hr_content_decimal", text_num_array[6],
                                      51 + text_w, 368, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win_hb, "hr_content_single", text_num_array[9],
                                      51 + text_w * 2, 368, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    {
        char *text = "times/min";
        int font_size = 48;
        gui_text_t *t = gui_text_create(win_hb, "txt", 156, 388, 0,
                                        96);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_RED, strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        int image_h = 300,
            image_w = SCREEN_WIDTH,
            pixel_bytes = 4;
        size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        if (img_data == NULL)
        {
            img_data = gui_lower_malloc(buffer_size);
        }
        memset(img_data, 0, buffer_size);
        gui_canvas_output_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, image_w, image_h, heartrate_graph, img_data);
        gui_img_t *img = gui_img_create_from_mem(win_hb, 0, (void *)img_data, 0, 0, SCREEN_WIDTH,
                                                 SCREEN_HEIGHT);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_img_set_animate(img, 1000, -1, hr_animation, (void *)buffer_size);
    }
    {
        char *text = "160";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", -35, 78 + 70 * 0, 0,
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_RED, strlen(text), font_size);
        gui_text_mode_set(t, RIGHT);
        gui_text_type_set(t, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        char *text = "120";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", -35, 78 + 70 * 1, 0,
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_RED, strlen(text), font_size);
        gui_text_mode_set(t, RIGHT);
        gui_text_type_set(t, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        char *text = "80";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", -35, 78 + 70 * 2, 0,
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_RED, strlen(text), font_size);
        gui_text_mode_set(t, RIGHT);
        gui_text_type_set(t, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        char *text = "12AM";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", 36 + 87 * 0,
                                        288 - font_size, 0, font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_SILVER, strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        char *text = "6AM";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", 36 + 87 * 1,
                                        288 - font_size, 0, font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_SILVER, strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        char *text = "12PM";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", 36 + 87 * 2,
                                        288 - font_size, 0, font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_SILVER, strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        char *text = "6PM";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", 36 + 87 * 3,
                                        288 - font_size, 0, font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_SILVER, strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    const char *name = GUI_BASE(gui_view_get_current_view())->name;
    if (strcmp(name, "music_view") == 0 || strcmp(name, "activity_view") == 0)
    {
        gui_view_switch_on_event(view, music_view, VIEW_CUBE, VIEW_CUBE,
                                 GUI_EVENT_TOUCH_MOVE_LEFT);
        gui_view_switch_on_event(view, activity_view, VIEW_CUBE, VIEW_CUBE,
                                 GUI_EVENT_TOUCH_MOVE_RIGHT);
    }
    else if (strcmp(name, "menu_view") == 0)
    {
        extern const uint32_t *gui_app_return_array[17];
        gui_return_create(view, gui_app_return_array,
                          sizeof(gui_app_return_array) / sizeof(uint32_t *), return_to_menu, 0);
    }
    else if (strcmp(name, "watchface_view") == 0)
    {
        extern const uint32_t *gui_app_return_array[17];
        gui_return_create(view, gui_app_return_array,
                          sizeof(gui_app_return_array) / sizeof(uint32_t *), return_to_watchface, 0);
    }
}