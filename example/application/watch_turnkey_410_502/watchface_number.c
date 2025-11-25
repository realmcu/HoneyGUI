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
/*============================================================================*
 *                           Function Declaration
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_win_t *win_dot0 = NULL;
static gui_img_t *img_dot0 = NULL;
static gui_img_t *img_dot1 = NULL;
static uint8_t *img_dot_data = NULL;
static uint8_t *img_bg_data = NULL;
static uint8_t dot_alpha = 255;  // opacity time-dot
static int fade_counter = 0;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void clear_num_view(void)
{
    if (img_dot_data != NULL)
    {
        gui_free(img_dot_data);
        img_dot_data = NULL;
    }
    if (img_bg_data != NULL)
    {
        gui_free(img_bg_data);
        img_bg_data = NULL;
    }

    fade_counter = 0;
    dot_alpha = 255;
}
static void time_update_cb(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    if (!timeinfo)
    {
        return;
    }

    GUI_WIDGET_POINTER_BY_NAME_ROOT(t_time, "t_time", obj);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(t_min, "t_min", obj);
    gui_text_content_set((gui_text_t *)t_time, time_str, 2);
    gui_text_content_set((gui_text_t *)t_min, time_str + 3, 2);
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
    gui_img_t *dot0 = (void *)gui_list_entry(GUI_BASE(p)->child_list.next, gui_obj_t, brother_list);
    uint8_t opacity = dot0->opacity_value;
    opacity -= 4;
    dot0->opacity_value = opacity;
}
static void time_dot1_cb(void *p)
{
    gui_img_t *dot0 = (void *)gui_list_entry(GUI_BASE(p)->child_list.next, gui_obj_t, brother_list);
    uint8_t opacity = dot0->opacity_value;
    opacity -= 4;
    dot0->opacity_value = opacity;
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
    watchface_clear_mem = clear_num_view;
    gui_obj_t *parent = GUI_BASE(view);
    gui_win_t *win = gui_win_create(parent, "win", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    size_t buffer_bg_size = SCREEN_WIDTH * SCREEN_HEIGHT * 2 + sizeof(gui_rgb_data_head_t);
    if (!img_bg_data)
    {
        img_bg_data = gui_malloc(buffer_bg_size);
    }
    memset(img_bg_data, 0, buffer_bg_size);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg_cb,
                                      img_bg_data);
    gui_img_create_from_mem(win, "watchface", (void *)img_bg_data, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // create time-text
    gui_text_t *t_time = gui_text_create(win, "t_time", -58, 10, 0, 0);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(87, 55, 65), 2,
                 250);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, RIGHT);
    gui_text_rendermode_set(t_time, 2);

    gui_text_t *t_min = gui_text_create(win, "t_min", -58, 200, 0, 0);
    gui_text_set(t_min, time_str + 3, GUI_FONT_SRC_TTF, gui_rgb(87, 55, 65), 2,
                 250);
    gui_text_type_set(t_min, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_min, RIGHT);
    gui_text_rendermode_set(t_min, 2);

    // create time-dot
    win_dot0 = gui_win_create(parent, "win_dot0", 0, 200, 50, 50);
    gui_win_t *win_dot1 = gui_win_create(parent, "win_dot1", 0, 250, 50, 50);
    size_t buffer_img_size = 50 * 50 * 4 + sizeof(gui_rgb_data_head_t);
    if (!img_dot_data)
    {
        img_dot_data = gui_malloc(buffer_img_size);
    }
    memset(img_dot_data, 0, buffer_img_size);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 50, 50, Circles_cb, img_dot_data);

    {
        img_dot0 = gui_img_create_from_mem(win_dot0, "img_dot0", (void *)img_dot_data, 50, 80, 0, 0);
        gui_img_set_mode(img_dot0, IMG_SRC_OVER_MODE);
        gui_img_set_quality(img_dot0, true);

        img_dot1 = gui_img_create_from_mem(win_dot1, "img_dot1", (void *)img_dot_data, 50, 110, 0, 0);
        gui_img_set_mode(img_dot1, IMG_SRC_OVER_MODE);
        gui_img_set_quality(img_dot1, true);
    }

    gui_obj_create_timer(GUI_BASE(win), 30000, true, time_update_cb);
    gui_obj_create_timer(GUI_BASE(win_dot0), 10, true, time_dot_cb);
    gui_obj_create_timer(GUI_BASE(win_dot1), 10, true, time_dot1_cb);
}