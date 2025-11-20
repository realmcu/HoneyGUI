/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <time.h>
#include <math.h>
#include "gui_win.h"
#include "gui_api.h"
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_video.h"
#include "gui_text.h"
#include "app_main_watch.h"
#include "gui_view_instance.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH  (int16_t)gui_get_width_height()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "watchface_video_view"
#define X_TARGET  37
#define Y_TARGET  42
#define X_ORIGIN -300
#define Y_ORIGIN 50
#define SCALE_ORINGIN 2.f
#define COUNT_MAX 30

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
extern void *text_num_array[11];
extern const char *day[7];
static gui_video_t *video = NULL;
static char date_text_content[10] = {0};
static char time_content[6] = {0};
static char compass_content[20] = {0};
static char weather_content[20] = {0};
static uint16_t compass_degree = 0;  // compass angle
static const char *compass_directions[8] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static const char *get_compass_direction(uint16_t degree)
{
    int index = ((degree + 22) % 360) / 45;  // 45° one direction
    return compass_directions[index % 8];
}
static void simulate_data_update_cb(void *p)
{
    GUI_UNUSED(p);
    //extern struct tm *timeinfo;
    if (!timeinfo)
    {
        return;
    }
    //update date and time
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "date_text", current_view);
        sprintf(date_text_content, "%d", timeinfo->tm_mday);
        gui_text_content_set((gui_text_t *)text, date_text_content, strlen(date_text_content));

        GUI_WIDGET_POINTER_BY_NAME_ROOT(t_time, "t_time", current_view);
        sprintf(time_content, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
        gui_text_content_set((gui_text_t *)t_time, time_content, strlen(time_content));
    }

    // simulate compass angle change (change 3 degrees per second)
    {
        compass_degree = (compass_degree + 3) % 360;
        sprintf(compass_content, "%s %d°", get_compass_direction(compass_degree), compass_degree);

        GUI_WIDGET_POINTER_BY_NAME_ROOT(text_compass, "text_compass", current_view);
        if (text_compass)
        {
            gui_text_content_set((gui_text_t *)text_compass, compass_content, strlen(compass_content));
        }
    }

    // weather
    {
        const char *weather_conditions[] = {"Sunny", "Cloudy", "Overcast", "Light Rain", "Rain", "Heavy Rain"};
        int condition_index = (timeinfo->tm_hour / 4) % 6;  // 4 hour change once
        int temperature = 15 + (timeinfo->tm_hour % 10);

        sprintf(weather_content, "%s %d°", weather_conditions[condition_index], temperature);

        GUI_WIDGET_POINTER_BY_NAME_ROOT(text_weather, "text_weather", current_view);
        if (text_weather)
        {
            gui_text_content_set((gui_text_t *)text_weather, weather_content, strlen(weather_content));
        }
    }

}
static void video_enter_cb(void *p)
{
    static uint8_t count = 0;
    int16_t x_offset = X_ORIGIN + (X_TARGET - X_ORIGIN) * count / COUNT_MAX;
    int16_t y_offset = Y_ORIGIN + (Y_TARGET - Y_ORIGIN) * count / COUNT_MAX;
    gui_img_translate(video->img, x_offset, y_offset);

    float scale = SCALE_ORINGIN + (1.f - SCALE_ORINGIN) * count / COUNT_MAX;
    gui_img_scale(video->img, scale, scale);

    count++;
    if (count > COUNT_MAX)
    {
        count = 0;
        gui_video_set_frame_rate(video, 30.f);
        gui_obj_stop_timer(GUI_BASE(p));
    }
}
static void earth_change_cb(void *obj, gui_event_t e, void *param)
{
    (void)e;
    (void)param;
    {
        if (!obj)
        {
            return;
        }
        gui_obj_t *p = GUI_BASE(obj)->parent;
        if (obj)
        {
            gui_obj_tree_free(GUI_BASE(obj));
        }
        video = gui_video_create_from_mem(p, "earth",
                                          (void *)EARTH_420_336_416_MJPG,
                                          X_TARGET, Y_TARGET, 410, 502);
        gui_video_set_state(video, GUI_VIDEO_STATE_PLAYING);
        gui_video_set_repeat_count(video, 0);
        gui_video_set_frame_rate(video, 60.f);
    }
    gui_obj_add_event_cb(video, earth_change_cb, GUI_EVENT_TOUCH_CLICKED, 0);
}
void create_watchface_earth(gui_view_t *view)
{
    current_view = view;
    gui_win_t *win_video = gui_win_create(view, "win_video", 0, 0, 0, 0);
    gui_win_t *win_main = gui_win_create(view, "win_main", 0, 0, 0, 0);

    // create video in earth video window
    video = gui_video_create_from_mem(win_video, "earth", (void *)EARTH_420_336_416_MJPG, 0, 0, 410,
                                      502);
    gui_video_set_state(video, GUI_VIDEO_STATE_PLAYING);
    gui_img_translate(video->img, X_ORIGIN, Y_ORIGIN);
    gui_img_scale(video->img, 1.5f, 1.5f);
    gui_video_set_frame_rate(video, 60.f);

    gui_obj_add_event_cb(video, earth_change_cb, GUI_EVENT_TOUCH_CLICKED, 0);

    // start video enter animation timer
    gui_obj_create_timer(GUI_BASE(win_video), 17, true, video_enter_cb);

    // weather text init
    sprintf(weather_content, "Sunny 22°");
    gui_text_t *text_weather = gui_text_create(win_main, "text_weather", -40, 35, 0, 0);
    gui_text_set(text_weather, weather_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(weather_content), 36);
    gui_text_type_set(text_weather, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_weather, RIGHT);
    gui_text_rendermode_set(text_weather, 2);

    // date text init
    sprintf(date_text_content, "16");
    gui_text_t *text = gui_text_create(win_main, "date_text", -40, 140, 0, 0);
    gui_text_set(text, (void *)date_text_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(date_text_content), 80);
    gui_text_type_set(text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, RIGHT);
    gui_text_rendermode_set(text, 2);

    // time text init
    extern struct tm *timeinfo;
    sprintf(time_content, "%02d:%02d", timeinfo ? timeinfo->tm_hour : 12,
            timeinfo ? timeinfo->tm_min : 0);
    gui_text_t *t_time = gui_text_create(win_main, "t_time", -40, 60, 0, 0);
    gui_text_set(t_time, time_content, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                 strlen(time_content), 80);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, RIGHT);
    gui_text_rendermode_set(t_time, 2);

    // compass text init
    sprintf(compass_content, "N 0°");
    gui_text_t *text_compass = gui_text_create(win_main, "text_compass", -40, 400, 0, 0);
    gui_text_set(text_compass, compass_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(compass_content), 36);
    gui_text_type_set(text_compass, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_compass, RIGHT);
    gui_text_rendermode_set(text_compass, 2);

    gui_obj_create_timer(GUI_BASE(win_main), 1000, true, simulate_data_update_cb);
}

/*============================================================================*
 *                           GUI_VIEW_INSTANCE
 *============================================================================*/

GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, create_watchface_earth, NULL);