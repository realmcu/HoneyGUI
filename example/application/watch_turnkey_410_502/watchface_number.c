/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <time.h>
#include "gui_win.h"
#include "gui_api.h"
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_video.h"
#include "gui_text.h"
#include "app_main_watch.h"
#include "gui_canvas.h"
#include "gui_view_instance.h"
/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "watchface_big_num_view"
/*============================================================================*
 *                           Function Declaration
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
static char hour_content[6] = {0};
static char minute_content[10] = {0};

static gui_win_t *win_dot = NULL;
static gui_img_t *img_dot = NULL;
static uint8_t *img_dot_data = NULL;
static uint8_t dot_alpha = 255;  // opacity time-dot
static int fade_counter = 0;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void time_update_cb(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    if (!timeinfo)
    {
        return;
    }

    sprintf(hour_content, "%02d", timeinfo->tm_hour);
    sprintf(minute_content, "%02d", timeinfo->tm_min);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(t_time, "t_time", obj);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(t_min, "t_min", obj);
    gui_text_content_set((gui_text_t *)t_time, hour_content, strlen(hour_content));
    gui_text_content_set((gui_text_t *)t_min, minute_content, strlen(minute_content));
}

static void Circles_cb(NVGcontext *vg)
{
    // gui_log("Rendering with dot_alpha: %d\n", dot_alpha);
    float circleDiameter = 24.0f;
    float circleRadius = circleDiameter / 2.0f;
    float spacing = 50.0f;

    float startX = 0;
    float startY = 0;

    NVGcolor circleColor = nvgRGBA(120, 80, 90, dot_alpha);
    for (int i = 0; i < 2; ++i)
    {
        nvgBeginPath(vg);
        float y = startY + i * (circleDiameter + spacing) + circleRadius;
        nvgCircle(vg, startX + circleRadius, y, circleRadius);
        nvgFillColor(vg, circleColor);
        nvgFill(vg);
    }
}

static void time_dot_cb(void *p)
{
    GUI_UNUSED(p);

    fade_counter++;
    // gui_log("Timer triggered: fade_counter = %d\n", fade_counter);
    if (fade_counter <= 20)
    {
        dot_alpha = 255 - (fade_counter * 255) / 20;
    }
    else if (fade_counter <= 40)
    {
        dot_alpha = ((fade_counter - 20) * 255) / 20;
    }

    if (fade_counter >= 40)
    {
        fade_counter = 0;
    }
    // gui_log("Before rendering: dot_alpha = %d\n", dot_alpha);
    // update image
    img_dot_data = gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 50, 200,
                                                     Circles_cb, img_dot_data);
    gui_img_set_image_data(img_dot, img_dot_data);
    // gui_img_refresh_size(img_dot);

    // gui_log("Image updated: alpha = %d, counter = %d\n", dot_alpha, fade_counter);
}


static void bg_cb(NVGcontext *vg)
{
    NVGcolor bgColor = nvgRGB(219, 122, 147); // pink background color
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    nvgFillColor(vg, bgColor);
    nvgFill(vg);
}
void create_watchface_number(gui_view_t *view)
{

    gui_obj_t *parent = GUI_BASE(view);
    gui_win_t *win = gui_win_create(parent, "win", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    size_t *buffer_bg_size = SCREEN_WIDTH * SCREEN_HEIGHT * 2 + sizeof(gui_rgb_data_head_t);
    uint8_t *img_bg_data = NULL;
    img_bg_data = gui_lower_malloc(buffer_bg_size);
    memset(img_bg_data, 0, buffer_bg_size);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg_cb,
                                      img_bg_data);
    gui_img_create_from_mem(win, "watchface", (void *)img_bg_data, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // create time-text
    sprintf(hour_content, "%02d", timeinfo->tm_hour);
    sprintf(minute_content, "%02d", timeinfo->tm_min);
    gui_text_t *t_time = gui_text_create(win, "t_time", -58, 10, 0, 0);
    gui_text_set(t_time, hour_content, GUI_FONT_SRC_TTF, gui_rgb(87, 55, 65), strlen(hour_content),
                 250);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, RIGHT);
    gui_text_rendermode_set(t_time, 2);

    gui_text_t *t_min = gui_text_create(win, "t_min", -58, 200, 0, 0);
    gui_text_set(t_min, minute_content, GUI_FONT_SRC_TTF, gui_rgb(87, 55, 65), strlen(minute_content),
                 250);
    gui_text_type_set(t_min, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_min, RIGHT);
    gui_text_rendermode_set(t_min, 2);

    // create time-dot
    win_dot = gui_win_create(parent, "win_dot", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    size_t *buffer_img_size = 50 * 200 * 4 + sizeof(gui_rgb_data_head_t);
    if (!img_dot_data)
    {
        img_dot_data = gui_lower_malloc(buffer_img_size);
    }
    memset(img_dot_data, 0, buffer_img_size);
    img_dot = gui_img_create_from_mem(win_dot, "img_dot", (void *)img_dot_data, 70, 300, 0, 0);

    gui_img_set_mode(img_dot, IMG_SRC_OVER_MODE);
    gui_img_set_quality(img_dot, true);

    gui_obj_create_timer(GUI_BASE(win), 30000, true, time_update_cb);
    gui_obj_create_timer(GUI_BASE(win_dot), 50, true, time_dot_cb);
}
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, create_watchface_number, NULL);
