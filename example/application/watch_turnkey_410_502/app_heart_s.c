/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <time.h>
#include <math.h>
#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "cJSON.h"
#include "gui_canvas_img.h"
#include "gui_rect.h"
#include "app_main_watch.h"
#include "gui_view.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define COLOR_RED gui_rgb(255,0,0)
#define COLOR_SILVER gui_rgb(192,192,192)
#define COLOR_SILVER_OPACITY(opacity) gui_rgba(192,192,192, opacity)
#define CURRENT_VIEW_NAME "heartrate_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void heartrate_design(gui_view_t *view);
static void clear_heartrate_cache(gui_view_t *view);
extern void time_update_cb(void *p);
extern void heart_drawCircles_cb(NVGcontext *vg);
/*============================================================================*
 *                            Variables
 *============================================================================*/
extern gui_text_t *time_text;
extern gui_img_t *img_dot;
extern uint8_t *img_dot_data;
extern gui_win_t *win_heart_menu;
extern uint8_t activeIndex;
// static char time_text_content[10];
// static gui_text_t *time_text;


static gui_img_t *img_icon_information = NULL;
static gui_win_t *win_hb = NULL;
static uint8_t *img_data = NULL;
static size_t buffer_size = 0;

static char *heartrate_json_content = NULL;
static uint8_t heartrate_refresh_flag = 0;
typedef struct
{
    int AM12;
    int AM6;
    int PM12;
    int PM6;
} heart_rate_data_t;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/


static void generate_heartrate_data(void)
{
    if (heartrate_json_content == NULL)
    {
        heartrate_json_content = gui_malloc(4096);
    }

    char buffer[2048];
    heart_rate_data_t hr_data;
    snprintf(heartrate_json_content, 4096, "{\"heart_rate\":[");

    // Generate multiple sets of heart rate data throughout the day
    for (int i = 0; i < 70; i++)
    {
        // Generate reasonable heart rate data
        hr_data.AM12 = 60 + (rand() % 40); // Designed for a random heart rate ranging from 60 to 100
        snprintf(buffer, 2048, "{\"time\":%d,\"bpm\":%d}%s", i, hr_data.AM12, i < 69 ? "," : "");
        strcat(heartrate_json_content, buffer);
    }

    strcat(heartrate_json_content, "]}");
    heartrate_refresh_flag = 1;
}
static void parse_heartrate_data(float *samples, size_t *sample_count)
{
    *sample_count = 0;

    if (heartrate_json_content == NULL)
    {
        return;
    }

    const char *ptr = heartrate_json_content;
    while ((ptr = strstr(ptr, "\"bpm\":")))
    {
        ptr += 6;
        if (*sample_count <
            500) // Assuming a maximum of 500 points, over 24 hours, with a maximum of 4 points per hour
        {
            samples[*sample_count] = atoi(ptr);
            (*sample_count)++;
        }
    }
}
static void draw_heartrate_graph(NVGcontext *vg)
{
    float x = 36.0f;//122.0f;
    float y = 78.0f;
    float w = 344.0f;//258.0f;
    float h = 200.0f;

    // draw split line
    {
        nvgBeginPath(vg);
        nvgRect(vg, 31 + 86 * 0, 100, 3,
                190); // the X/Y-axis coordinate relative to parent widget
        nvgFillColor(vg, nvgRGBA(192, 192, 192, 255)); //silver
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgRect(vg, 31 + 86 * 1, 100, 3,
                190); // the X/Y-axis coordinate relative to parent widget
        nvgFillColor(vg, nvgRGBA(192, 192, 192, 255)); //silver
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgRect(vg, 31 + 86 * 2, 100, 3,
                190); // the X/Y-axis coordinate relative to parent widget
        nvgFillColor(vg, nvgRGBA(192, 192, 192, 255)); //silver
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgRect(vg, 31 + 86 * 3, 100, 3,
                190); // the X/Y-axis coordinate relative to parent widget
        nvgFillColor(vg, nvgRGBA(192, 192, 192, 255)); //silver
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgRect(vg, 31 + 86 * 4, 100, 3,
                190); // the X/Y-axis coordinate relative to parent widget
        nvgFillColor(vg, nvgRGBA(192, 192, 192, 255)); //silver
        nvgFill(vg);
    }

    float samples[500] = {0}; // Capable of storing multiple data points throughout the day
    size_t sample_count = 0;

    parse_heartrate_data(samples, &sample_count);

    float dx = w / 24.0f; // x-axis width per hour
    for (size_t i = 0; i < sample_count; i++)
    {
        float sx = x + (i % 24) * dx + rand() % (int)(dx / 2); // Random offset within each hour
        float sy = y + h * (1 - (samples[i] - 40.0f) / 120.0f);

        // scatter plot
        nvgBeginPath(vg);
        nvgCircle(vg, sx, sy, 3.0f);
        nvgFillColor(vg, nvgRGBA(255, 0, 0, 255));
        nvgFill(vg);
    }
}

static void hr_timer_cb(void *obj)
{
    gui_img_t *img = (gui_img_t *)obj;
    static uint8_t count = 0;
    count++;
    // if (count >= 10) { // The data is updated every 10 seconds
    //     count = 0;
    //     generate_heartrate_data();
    // }
    if (heartrate_refresh_flag)
    {
        heartrate_refresh_flag = 0;
        uint8_t *img_data = (void *)gui_img_get_image_data(img);
        memset(img_data, 0, buffer_size);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, SCREEN_WIDTH, 300,
                                          draw_heartrate_graph,
                                          img_data);
        gui_img_set_image_data(img, img_data);
    }
}
static void clear_heartrate_cache(gui_view_t *view)
{
    (void)view;
    if (img_data)
    {
        gui_lower_free(img_data);
        img_data = NULL;
    }
    if (heartrate_json_content)
    {
        gui_free(heartrate_json_content);
        heartrate_json_content = NULL;
    }
}

void resting_heartrate_design(gui_obj_t *obj)
{
    activeIndex = 1;
    generate_heartrate_data();

    win_hb = gui_win_create(obj, "hb_win", 0, 0, SCREEN_WIDTH,
                            SCREEN_HEIGHT);
    int pixel_bytes = 4;
    size_t buffer_size = 0;

    // img_ring dot
    buffer_size = 12 * 96 * pixel_bytes + sizeof(gui_rgb_data_head_t);
    if (img_dot_data == NULL)
    {
        img_dot_data = gui_lower_malloc(buffer_size);
    }
    memset(img_dot_data, 0, buffer_size);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 12, 96,
                                      heart_drawCircles_cb, img_dot_data);
    img_dot = gui_img_create_from_mem(win_heart_menu, "dot_img", (void *)img_dot_data,
                                      393, 82, 0, 0);
    gui_img_set_mode(img_dot, IMG_SRC_OVER_MODE);
    gui_img_set_quality(img_dot, true);
    {
        char *text = "Range";
        int font_size = 36;
        gui_text_t *t = gui_text_create(win_hb, "txt", 31, 318, 0,
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255), strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, INTER_28PT_BOLD_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        char *text = "60-100";
        int font_size = 80;
        gui_text_t *t = gui_text_create(win_hb, "txt", 31, 350, 0,
                                        96);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255), strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, INTER_24PT_REGULAR_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        char *text = "BPM";
        int font_size = 38;
        gui_text_t *t = gui_text_create(win_hb, "txt", 245, 380, 0, 96);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_RED, strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, INTER_24PT_REGULAR_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        char *text = "Today";
        int font_size = 36;
        gui_text_t *t = gui_text_create(win_hb, "txt", 40, 430, 0, 96);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, gui_rgb(150, 150, 150), strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, INTER_28PT_BOLD_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        int image_h = 300;
        int image_w = SCREEN_WIDTH;
        int pixel_bytes = 4;
        buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        if (img_data == NULL)
        {
            img_data = gui_lower_malloc(buffer_size);
        }
        memset(img_data, 0, buffer_size);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, image_w, image_h, draw_heartrate_graph,
                                          img_data);
        gui_img_t *img = gui_img_create_from_mem(win_hb, 0, (void *)img_data, 0, 0, SCREEN_WIDTH,
                                                 SCREEN_HEIGHT);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_img_set_quality(img, true);
        gui_obj_create_timer(GUI_BASE(img), 1000, true, hr_timer_cb);
        gui_obj_start_timer(GUI_BASE(img));
    }
    {
        char *text = "160";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", -35, 78 + 70 * 0, 0,
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_RED, strlen(text), font_size);
        gui_text_mode_set(t, RIGHT);
        gui_text_type_set(t, INTER_28PT_BOLD_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        char *text = "120";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", -35, 78 + 70 * 1, 0,
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_RED, strlen(text), font_size);
        gui_text_mode_set(t, RIGHT);
        gui_text_type_set(t, INTER_28PT_BOLD_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        char *text = "80";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", -35, 78 + 70 * 2, 0,
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_RED, strlen(text), font_size);
        gui_text_mode_set(t, RIGHT);
        gui_text_type_set(t, INTER_28PT_BOLD_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        char *text = "12AM";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", 36 + 87 * 0,
                                        288 - font_size, 0, font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_SILVER, strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, INTER_28PT_BOLD_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        char *text = "6AM";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", 36 + 87 * 1,
                                        288 - font_size, 0, font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_SILVER, strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, INTER_28PT_BOLD_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        char *text = "12PM";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", 36 + 87 * 2,
                                        288 - font_size, 0, font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_SILVER, strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, INTER_28PT_BOLD_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    {
        char *text = "6PM";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", 36 + 87 * 3,
                                        288 - font_size, 0, font_size);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, COLOR_SILVER, strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, INTER_28PT_BOLD_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(t, 2);
    }
    img_icon_information = gui_img_create_from_mem(win_hb, "icon_information",
                                                   (void *)ICON_INFORMATION_BIN,
                                                   19, 23, 0, 0);

    time_update_cb(NULL);

}

