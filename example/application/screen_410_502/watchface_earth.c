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
#include "tp_algo.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH  (int16_t)gui_get_width_height()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "earth_clock_view"
#define X_TARGET  37
#define Y_TARGET  42
#define X_ORINGIN -300
#define Y_ORINGIN 50
#define SCALE_ORINGIN 2.f
#define COUNT_MAX 30

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void create_watchface_earth(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = create_watchface_earth,
    .keep = false,
};

extern void *text_num_array[11];
extern const char *day[7];
static gui_video_t *video = NULL;
static char date_text_content[10] = {0};

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
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

// static void return_to_menu()
// {
//     gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
//                            SWITCH_IN_ANIMATION_FADE);
// }

// static void return_timer_cb()
// {
//     touch_info_t *tp = tp_get_info();
//     GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, return_to_menu)
// }

static void time_update_cb(void *p)
{
    extern struct tm *timeinfo;
    if (!timeinfo)
    {
        return;
    }

    GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "date_text", current_view);
    sprintf(date_text_content, "%s %d", day[timeinfo->tm_wday], timeinfo->tm_mday);
    gui_text_content_set((gui_text_t *)text, date_text_content, strlen(date_text_content));

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_hour_decimal, "watch_hour_decimal", current_view);
    gui_img_set_image_data((gui_img_t *)img_hour_decimal, text_num_array[timeinfo->tm_hour / 10]);
    gui_img_refresh_size((gui_img_t *)img_hour_decimal);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_hour_single, "watch_hour_single", current_view);
    gui_img_set_image_data((gui_img_t *)img_hour_single, text_num_array[timeinfo->tm_hour % 10]);
    gui_img_refresh_size((gui_img_t *)img_hour_single);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_minute_decimal, "watch_minute_decimal", current_view);
    gui_img_set_image_data((gui_img_t *)img_minute_decimal, text_num_array[timeinfo->tm_min / 10]);
    gui_img_refresh_size((gui_img_t *)img_minute_decimal);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_minute_single, "watch_minute_single", current_view);
    gui_img_set_image_data((gui_img_t *)img_minute_single, text_num_array[timeinfo->tm_min % 10]);
    gui_img_refresh_size((gui_img_t *)img_minute_single);
}

static void enter_cb(void *p)
{
    static uint8_t count = 0;
    int16_t x_offset = X_ORINGIN + (X_TARGET - X_ORINGIN) * count / COUNT_MAX;
    int16_t y_offset = Y_ORINGIN + (Y_TARGET - Y_ORINGIN) * count / COUNT_MAX;
    gui_img_translate(video->img, x_offset, y_offset);

    float scale = SCALE_ORINGIN + (1.f - SCALE_ORINGIN) * count / COUNT_MAX;
    gui_img_scale(video->img, scale, scale);

    count++;
    if (count > COUNT_MAX)
    {
        gui_obj_create_timer(GUI_BASE(p), 30000, true, time_update_cb);
        count = 0;
        gui_video_set_frame_rate(video, 30.f);
    }
}

static void create_watchface_earth(gui_view_t *view)
{
    gui_obj_hidden(&(gui_view_get_current()->base), true);
    gui_win_t *win = gui_win_create(view, "win", 0, 0, 0, 0);
    video = gui_video_create_from_mem(win, "earth", (void *)EARTH_420_336_416_MJPG, 0, 0, 410,
                                      502);
    gui_video_set_state(video, GUI_VIDEO_STATE_PLAYING);
    gui_video_set_repeat_count(video, GUI_VIDEO_REPEAT_INFINITE);
    gui_img_translate(video->img, X_ORINGIN, Y_ORINGIN);
    gui_img_scale(video->img, 1.5f, 1.5f);
    gui_video_set_frame_rate(video, 60.f);
    gui_obj_create_timer(GUI_BASE(win), 17, true, enter_cb);

    // date & time text
    sprintf(date_text_content, "FRI 16");
    gui_text_t *text = gui_text_create(win, "date_text", -40, 33, 0, 0);
    gui_text_set(text, (void *)date_text_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(date_text_content),
                 48);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, RIGHT);
    gui_text_rendermode_set(text, 2);
    {
        int text_w = 35;
        gui_img_t *img = gui_img_create_from_mem(win, "watch_hour_decimal", text_num_array[0],
                                                 211, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win, "watch_hour_single", text_num_array[0],
                                      211 + text_w, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win, "colon", text_num_array[10],
                                      211 + text_w * 2 + 5, 88 + 5, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win, "watch_minute_decimal", text_num_array[0],
                                      211 + text_w * 2 + 17, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win, "watch_minute_single", text_num_array[0],
                                      211 + text_w * 3 + 17, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    time_update_cb(NULL);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);
    // gui_obj_create_timer(GUI_BASE(view), 17, true, return_timer_cb);
}