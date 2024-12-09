/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file app_clock.c
  * @brief clock
  * @details clock
  * @author triton_yu@realsil.com.cn
  * @date 2024/08/30
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_canvas_rect.h"
#include "gui_multi_level.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_image_array.h"
#include "gui_switch.h"
#include "gui_scroll_wheel_new.h"
#include "../root_image/ui_resource.h"
#include "gui_app.h"
#include<stdio.h>
#include<time.h>
#include "gui_img.h"
#include "gui_return.h"
/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */


/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */

#define SCREEN_W ((int)gui_get_screen_width())
#define SCREEN_H ((int)gui_get_screen_height())
#define STOPWATCHTEXT "STOPWATCHTEXT"
#define SETTING_WIN_NAME "setting"
#define TIMER_WIN_NAME "timer"
#define WHEEL_NAME "WHEEL IN SETTING WINDOW"
#define WHEEL_NAME2 "WHEEL 2 IN SETTING WINDOW"
#define WHEEL_NAME3 "WHEEL 3 IN SETTING WINDOW"
#define TIMER_TEXT_NAME1 "TEXT 1 IN TIMER WINDOW"
#define TIMER_TEXT_NAME2 "TEXT 2 IN TIMER WINDOW"
#define TIMER_TEXT_NAME3 "TEXT 3 IN TIMER WINDOW"
#define TIMER_TEXT_NAME4 "TEXT 4 IN TIMER WINDOW"
#define TIMER_TEXT_NAME5 "TEXT 5 IN TIMER WINDOW"
#define STOPWATCH_RECT_NAME "STOPWATCH_RECT_NAME"
#define TIMER_RECT_NAME "TIMER_RECT_NAME"
#define FONT ARIALBD_SIZE16_BITS4_FONT_BIN
#define FONT_SIZE 16
#define TEXT_SZIE 20
#define IMAGE_ARRAY_MAP app_text_image_map
#define IMAGE_ARRAY_MAP_LENGTH 15
#define ROW_COUNT 5
#define TEXT1_X 60
#define TEXT2_X 211
#define TEXT3_X 362
#define TEXT_GAP 100
#define COLOR_PURPLE gui_rgb(0X8d,0X5f,0Xd3)
/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */
static unsigned char render_mode = GUI_SCROLL_WHEEL_NEW_RENDER_IMAGE_ARRAY;
const static struct gui_text_image_map app_text_image_map[] =
{
    {"0", TEXT0_BIN},
    {"1", TEXT1_BIN},
    {"2", TEXT2_BIN},
    {"3", TEXT3_BIN},
    {"4", TEXT4_BIN},
    {"5", TEXT5_BIN},
    {"6", TEXT6_BIN},
    {"7", TEXT7_BIN},
    {"8", TEXT8_BIN},
    {"9", TEXT9_BIN},
    {"PM", TEXTPM_BIN},
    {"AM", TEXTAM_BIN},
    {".", TEXTDIAN_BIN},
    {":", TEXTMAOHAO_BIN},
    {"/", TEXTXIEGANG_BIN},
};
const static char *string_array[] = {"00", "01", "02", "03",
                                     "04",
                                     "05",
                                     "06",
                                     "07",
                                     "08",
                                     "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"
                                    };
const static char *minutes_string_array[] =
{
    "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
};
static int hour_index, minute_index, second_index;
static bool stopwatch_enter;
/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */

static void stop_watch_win_ani_cb(void);
static void stop_watch_ml0(gui_obj_t *parent);
static void stop_watch_ml1_0(gui_obj_t *parent);
static void stop_watch_ml1_1(gui_obj_t *parent);
static void win_stop_watch_cb(void *null1, void *null2, void *param);
static void win_timer_cb(void *null1, void *null2, void *param);
static void switch_on_cb(void *null1, void *null2, void *param);
static void switch_off_cb(void);
const static int gap = 50;


static void stop_watch_ml0(gui_obj_t *parent)
{

}

static void stopwatch_switch_on_cb(void *null1, void *null2, gui_win_t *stop)
{
    gui_win_start_animation(stop);
    stopwatch_enter = 0;
}
static void stopwatch_switch_off_cb(void *null1, void *null2, gui_win_t *stop)
{
    gui_win_stop_animation(stop);
    char *buffer = "00:00:00";
    GUI_WIDGET_POINTER_BY_NAME(time_txt, STOPWATCHTEXT)
    gui_text_content_set(GUI_TYPE(gui_text_t, time_txt), buffer, strlen(buffer));
    gui_text_convert_to_img(GUI_TYPE(gui_text_t, time_txt), ARGB8888);
}
static void stop_watch_ml1_0(gui_obj_t *parent)
{

    gui_win_t *stop = gui_win_create(parent, 0, 0, 0, SCREEN_W, SCREEN_H);
    gui_win_set_animate(stop, 1000, -1, (gui_animate_callback_t)stop_watch_win_ani_cb, 0);
    gui_win_stop_animation(stop);
    {
        char *text = "00:00:00";
        int font_size = 48;
        gui_text_t *t = gui_text_create(stop, STOPWATCHTEXT,  0, 200,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), font_size);
        void *addr1 = ARIAL_SIZE48_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, CENTER);
        gui_text_convert_to_img(t, ARGB8888);


    }
    gui_switch_t *sw = gui_switch_create(parent, 454 / 2 - 150 / 2 + 30, 300, 100, 50, STARTBUTTON_BIN,
                                         STOPBUTTON_BIN);
    gui_img_set_mode(sw->switch_picture, IMG_SRC_OVER_MODE);
    GUI_BASE(sw->switch_picture)->y = -27;
    GUI_BASE(sw->switch_picture)->x = -30;
    GUI_API(gui_switch_t).on_turn_on(sw, (gui_event_cb_t)stopwatch_switch_on_cb, stop);
    GUI_API(gui_switch_t).on_turn_off(sw, (gui_event_cb_t)stopwatch_switch_off_cb, stop);
}
static void hour_destory_overwrite(gui_obj_t *obj)
{
    gui_win_t *this = (void *)obj;
    if (this->animate)
    {
        gui_free(this->animate);
        this->animate = NULL;
    }
    hour_index = GUI_API(gui_scroll_wheel_new_t).get_index(GUI_TYPE(gui_scroll_wheel_new_t, obj));
}
static void minute_destory_overwrite(gui_obj_t *obj)
{
    gui_win_t *this = (void *)obj;
    if (this->animate)
    {
        gui_free(this->animate);
        this->animate = NULL;
    }
    minute_index = GUI_API(gui_scroll_wheel_new_t).get_index(GUI_TYPE(gui_scroll_wheel_new_t, obj));
}
static void second_destory_overwrite(gui_obj_t *obj)
{
    gui_win_t *this = (void *)obj;
    if (this->animate)
    {
        gui_free(this->animate);
        this->animate = NULL;
    }
    second_index = GUI_API(gui_scroll_wheel_new_t).get_index(GUI_TYPE(gui_scroll_wheel_new_t, obj));
}
static void hour_overwrite(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_win_prepare(obj);
            break;

        case OBJ_DESTORY:
            hour_destory_overwrite(obj);
            break;

        default:
            break;
        }
    }
}
static void minute_overwrite(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_win_prepare(obj);
            break;

        case OBJ_DESTORY:
            minute_destory_overwrite(obj);
            break;

        default:
            break;
        }
    }
}
static void second_overwrite(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_win_prepare(obj);
            break;

        case OBJ_DESTORY:
            second_destory_overwrite(obj);
            break;

        default:
            break;
        }
    }
}
static void stop_watch_ml1_1(gui_obj_t *parent)
{
    const int width = SCREEN_W / 3;
    const int col_offset = 60;
    {
        //setting window
        gui_win_t *setting = gui_win_create(parent, SETTING_WIN_NAME, 0, 0, SCREEN_W, SCREEN_H);
        {
            gui_scroll_wheel_new_t *wheel = gui_scroll_wheel_new_create(setting, 0, 50, width, 50, ROW_COUNT,
                                                                        string_array, GUI_ARRAY_SIZE(string_array));
            gui_scroll_wheel_new_set_col_offset(wheel, col_offset);
            if (render_mode == GUI_SCROLL_WHEEL_NEW_RENDER_IMAGE_ARRAY)
            {
                gui_scroll_wheel_new_render_image_array(wheel, IMAGE_ARRAY_MAP, IMAGE_ARRAY_MAP_LENGTH);
            }
            else if (render_mode == GUI_SCROLL_WHEEL_NEW_RENDER_TEXT)
            {
                gui_scroll_wheel_new_render_text(wheel, ARIALBD_SIZE16_BITS4_FONT_BIN, 16);
            }
            gui_widget_name(GUI_BASE(wheel), WHEEL_NAME);
            gui_canvas_rect_create((void *)wheel, 0, width - 2, 0, 2, gap * 5 - 25, APP_COLOR_BLACK);
            gui_img_t *img = gui_img_create_from_mem(wheel, 0, STOPWATCHMASK_BIN, col_offset, 0, 0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        }
        {
            gui_scroll_wheel_new_t *wheel = gui_scroll_wheel_new_create(setting, SCREEN_W / 3, 50, width, 50,
                                                                        ROW_COUNT,
                                                                        minutes_string_array, GUI_ARRAY_SIZE(minutes_string_array));
            gui_scroll_wheel_new_set_col_offset(wheel, col_offset);
            if (render_mode == GUI_SCROLL_WHEEL_NEW_RENDER_IMAGE_ARRAY)
            {
                gui_scroll_wheel_new_render_image_array(wheel, IMAGE_ARRAY_MAP, IMAGE_ARRAY_MAP_LENGTH);
            }
            else if (render_mode == GUI_SCROLL_WHEEL_NEW_RENDER_TEXT)
            {
                gui_scroll_wheel_new_render_text(wheel, ARIALBD_SIZE16_BITS4_FONT_BIN, 16);
            }
            gui_widget_name(GUI_BASE(wheel), WHEEL_NAME2);
            gui_canvas_rect_create((void *)wheel, 0, width - 2, 0, 2, gap * 5 - 25, APP_COLOR_BLACK);
            gui_img_t *img = gui_img_create_from_mem(wheel, 0, STOPWATCHMASK_BIN, col_offset, 0, 0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        }
        {
            gui_scroll_wheel_new_t *wheel = gui_scroll_wheel_new_create(setting, SCREEN_W * 2 / 3, 50, width,
                                                                        50, ROW_COUNT,
                                                                        minutes_string_array, GUI_ARRAY_SIZE(minutes_string_array));
            gui_scroll_wheel_new_set_col_offset(wheel, col_offset);
            GET_BASE(wheel)->obj_cb = second_overwrite;
            if (render_mode == GUI_SCROLL_WHEEL_NEW_RENDER_IMAGE_ARRAY)
            {
                gui_scroll_wheel_new_render_image_array(wheel, IMAGE_ARRAY_MAP, IMAGE_ARRAY_MAP_LENGTH);
            }
            else if (render_mode == GUI_SCROLL_WHEEL_NEW_RENDER_TEXT)
            {
                gui_scroll_wheel_new_render_text(wheel, ARIALBD_SIZE16_BITS4_FONT_BIN, 16);
            }
            gui_widget_name(GUI_BASE(wheel), WHEEL_NAME3);
            gui_img_t *img = gui_img_create_from_mem(wheel, 0, STOPWATCHMASK_BIN, col_offset, 0, 0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        }
    }
    {
        //timer window
        gui_win_t *timer = gui_win_create(parent, TIMER_WIN_NAME, 0, 0, SCREEN_W, SCREEN_H);
        gui_obj_hidden(GUI_BASE(timer), 1);
#define Y_OFFSET 150
        switch (render_mode)
        {
        case GUI_SCROLL_WHEEL_NEW_RENDER_TEXT:
            {

                {
                    const char *text = string_array[0];
                    gui_text_t *t = gui_text_create(timer, TIMER_TEXT_NAME1, 80, Y_OFFSET, width, TEXT_SZIE);
                    gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), FONT_SIZE);
                    const void *addr1 = FONT;
                    gui_text_type_set(t, (void *)addr1, FONT_SRC_MEMADDR);
                    gui_text_mode_set(t, LEFT);
                    gui_text_convert_to_img(t, ARGB8888);
                }
                {
                    const char *text = minutes_string_array[0];
                    gui_text_t *t = gui_text_create(timer, TIMER_TEXT_NAME2, 200, Y_OFFSET, width, TEXT_SZIE);
                    gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), FONT_SIZE);
                    const void *addr1 = FONT;
                    gui_text_type_set(t, (void *)addr1, FONT_SRC_MEMADDR);
                    gui_text_mode_set(t, LEFT);
                    gui_text_convert_to_img(t, ARGB8888);
                }
                {
                    const char *text = minutes_string_array[0];
                    gui_text_t *t = gui_text_create(timer, TIMER_TEXT_NAME3, 350, Y_OFFSET, width, TEXT_SZIE);
                    gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), FONT_SIZE);
                    const void *addr1 = FONT;
                    gui_text_type_set(t, (void *)addr1, FONT_SRC_MEMADDR);
                    gui_text_mode_set(t, LEFT);
                    gui_text_convert_to_img(t, ARGB8888);
                }
                {
                    const char *text = ":";
                    gui_text_t *t = gui_text_create(timer, TIMER_TEXT_NAME4, 180, Y_OFFSET, width, TEXT_SZIE);
                    gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), FONT_SIZE);
                    const void *addr1 = FONT;
                    gui_text_type_set(t, (void *)addr1, FONT_SRC_MEMADDR);
                    gui_text_mode_set(t, LEFT);
                    gui_text_convert_to_img(t, ARGB8888);
                }
                {
                    const char *text = ":";
                    gui_text_t *t = gui_text_create(timer, TIMER_TEXT_NAME5, 330, Y_OFFSET, width, TEXT_SZIE);
                    gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), FONT_SIZE);
                    const void *addr1 = FONT;
                    gui_text_type_set(t, (void *)addr1, FONT_SRC_MEMADDR);
                    gui_text_mode_set(t, LEFT);
                    gui_text_convert_to_img(t, ARGB8888);
                }
            }
            break;
        case GUI_SCROLL_WHEEL_NEW_RENDER_IMAGE_ARRAY:
            {
                {
                    const char *text = string_array[0];
                    const char *input = text;
                    size_t num_pointers;
                    void **image_pointers = gui_scroll_wheel_new_get_image_pointers(input, &num_pointers,
                                                                                    IMAGE_ARRAY_MAP_LENGTH, IMAGE_ARRAY_MAP);

                    // Print results
                    gui_log("Found %zu image pointers for input \"%s\"\n", num_pointers, input);
                    for (size_t i = 0; i < num_pointers; ++i)
                    {
                        gui_log("Image pointer %zu: %p\n", i + 1, image_pointers[i]);
                    }
                    gui_image_array_create(timer, 60, Y_OFFSET, image_pointers, num_pointers,
                                           TIMER_TEXT_NAME1);
                    // Free allocated memory
                    gui_free(image_pointers);
                }
                {
                    const char *text = minutes_string_array[0];
                    const char *input = text;
                    size_t num_pointers;
                    void **image_pointers = gui_scroll_wheel_new_get_image_pointers(input, &num_pointers,
                                                                                    IMAGE_ARRAY_MAP_LENGTH, IMAGE_ARRAY_MAP);

                    // Print results
                    gui_log("Found %zu image pointers for input \"%s\"\n", num_pointers, input);
                    for (size_t i = 0; i < num_pointers; ++i)
                    {
                        gui_log("Image pointer %zu: %p\n", i + 1, image_pointers[i]);
                    }
                    gui_image_array_create(timer, 211, Y_OFFSET, image_pointers, num_pointers,
                                           TIMER_TEXT_NAME2);
                    // Free allocated memory
                    gui_free(image_pointers);
                }
                {
                    const char *text = minutes_string_array[0];
                    const char *input = text;
                    size_t num_pointers;
                    void **image_pointers = gui_scroll_wheel_new_get_image_pointers(input, &num_pointers,
                                                                                    IMAGE_ARRAY_MAP_LENGTH, IMAGE_ARRAY_MAP);

                    // Print results
                    gui_log("Found %zu image pointers for input \"%s\"\n", num_pointers, input);
                    for (size_t i = 0; i < num_pointers; ++i)
                    {
                        gui_log("Image pointer %zu: %p\n", i + 1, image_pointers[i]);
                    }
                    gui_image_array_create(timer, 362, Y_OFFSET, image_pointers, num_pointers,
                                           TIMER_TEXT_NAME3);
                    // Free allocated memory
                    gui_free(image_pointers);
                }
                {
                    const char *text = ":";
                    const char *input = text;
                    size_t num_pointers;
                    void **image_pointers = gui_scroll_wheel_new_get_image_pointers(input, &num_pointers,
                                                                                    IMAGE_ARRAY_MAP_LENGTH, IMAGE_ARRAY_MAP);

                    // Print results
                    gui_log("Found %zu image pointers for input \"%s\"\n", num_pointers, input);
                    for (size_t i = 0; i < num_pointers; ++i)
                    {
                        gui_log("Image pointer %zu: %p\n", i + 1, image_pointers[i]);
                    }
                    gui_image_array_create(timer, 60 + 40 + TEXT_GAP, Y_OFFSET + 5, image_pointers, num_pointers,
                                           TIMER_TEXT_NAME4);
                    // Free allocated memory
                    gui_free(image_pointers);
                }
                {
                    const char *text = ":";
                    const char *input = text;
                    size_t num_pointers;
                    void **image_pointers = gui_scroll_wheel_new_get_image_pointers(input, &num_pointers,
                                                                                    IMAGE_ARRAY_MAP_LENGTH, IMAGE_ARRAY_MAP);

                    // Print results
                    gui_log("Found %zu image pointers for input \"%s\"\n", num_pointers, input);
                    for (size_t i = 0; i < num_pointers; ++i)
                    {
                        gui_log("Image pointer %zu: %p\n", i + 1, image_pointers[i]);
                    }
                    gui_image_array_create(timer, 362 - TEXT_GAP - 10, Y_OFFSET + 5, image_pointers, num_pointers,
                                           TIMER_TEXT_NAME5);
                    // Free allocated memory
                    gui_free(image_pointers);
                }
            }


            break;
        default:
            break;
        }



    }
    gui_switch_t *sw = gui_switch_create(parent, 454 / 2 - 150 / 2 + 30, 300, 100, 50, STARTBUTTON_BIN,
                                         STOPBUTTON_BIN);
    gui_img_set_mode(sw->switch_picture, IMG_SRC_OVER_MODE);
    GUI_BASE(sw->switch_picture)->y = -27;
    GUI_BASE(sw->switch_picture)->x = -30;
    GUI_API(gui_switch_t).on_turn_on(sw, (gui_event_cb_t)switch_on_cb, 0);
    GUI_API(gui_switch_t).on_turn_off(sw, (gui_event_cb_t) switch_off_cb, 0);
}
static void win_stop_watch_cb(void *null1, void *null2, void *param)
{
    gui_multi_level_t *ml = param;
    GUI_API(gui_multi_level_t).jump(ml, 1, 0);
    GUI_WIDGET_POINTER_BY_NAME(rect, STOPWATCH_RECT_NAME);
    GUI_TYPE(gui_canvas_rect_t, rect)->color = COLOR_PURPLE;

}
static void win_release_cb(void *null1, void *null2, const char *param)
{
    GUI_WIDGET_POINTER_BY_NAME(rect, param);
    GUI_TYPE(gui_canvas_rect_t, rect)->color = APP_COLOR_SILVER;
}
static void win_timer_cb(void *null1, void *null2, void *param)
{
    gui_multi_level_t *ml = param;
    GUI_API(gui_multi_level_t).jump(ml, 1, 1);
    GUI_WIDGET_POINTER_BY_NAME(rect, TIMER_RECT_NAME);
    GUI_TYPE(gui_canvas_rect_t, rect)->color = COLOR_PURPLE;
}

static void get_stopwatch_string(char *buffer)
{
#if _WIN32

    static struct timeval start;
    if (!stopwatch_enter)
    {
        stopwatch_enter = 1;
        mingw_gettimeofday(&start, NULL);
    }
    struct timeval end;
    long mtime, secs, usecs;
    {
        mingw_gettimeofday(&end, NULL);
        secs  = end.tv_sec  - start.tv_sec;
        usecs = end.tv_usec - start.tv_usec;
        mtime = ((secs) * 1000 + usecs / 1000.0) + 0.5;
        sprintf(buffer, "\r%02ld:%02ld:%02ld", secs / 60, secs % 60, (mtime % 1000) / 10);
    }
#endif
}
static void stop_watch_win_ani_cb()
{

    static char buffer[9];
    get_stopwatch_string(buffer);
    gui_text_t *time_txt = 0;
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), STOPWATCHTEXT, (void *)&time_txt);
    gui_text_content_set(time_txt, buffer, strlen(buffer));
    gui_text_convert_to_img(time_txt, ARGB8888);
}
void parse_time_string(const char *time_string, int *hours, int *minutes, int *seconds)
{
    sscanf(time_string, "%d:%d:%d", hours, minutes, seconds);
}
void format_time_string(int hours, int minutes, int seconds, char *buffer)
{
    sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
}
const char *get_text_from_widget(gui_obj_t *widget)
{
    if (render_mode == GUI_SCROLL_WHEEL_NEW_RENDER_IMAGE_ARRAY)
    {
        return GUI_TYPE(gui_image_array_t, widget)->text;
    }
    else if (render_mode == GUI_SCROLL_WHEEL_NEW_RENDER_TEXT)
    {
        return GUI_TYPE(gui_text_t, widget)->content;
    }
    return 0;
}
static bool enter;
static void timer_start_window_callback(gui_win_t *win)
{
    GUI_WIDGET_POINTER_BY_NAME(text1, TIMER_TEXT_NAME1);
    GUI_WIDGET_POINTER_BY_NAME(text2, TIMER_TEXT_NAME2);
    GUI_WIDGET_POINTER_BY_NAME(text3, TIMER_TEXT_NAME3);

    static int total_seconds;
    static int hours, minutes, seconds;
    static time_t start_time;
    static time_t end_time;
    if (!enter)
    {
        enter = 1;

        hours = atoi(get_text_from_widget(text1));
        minutes = atoi(get_text_from_widget(text2));
        seconds = atoi(get_text_from_widget(text3));
        total_seconds = hours * 3600 + minutes * 60 + seconds;
        start_time = time(NULL);
        end_time   = start_time + total_seconds;
    }
    if (total_seconds > 0)
    {
        time_t current_time = time(NULL);
        total_seconds = difftime(end_time, current_time);
        if (total_seconds < 0)
        {
            gui_win_stop_animation(win);
            enter = 0;
            return;
        }
        hours = total_seconds / 3600;
        minutes = (total_seconds % 3600) / 60;
        seconds = total_seconds % 60;
        char formatted_time[9];
        format_time_string(hours, minutes, seconds, formatted_time);
        gui_log("\r%s", formatted_time);
        sprintf(formatted_time, "%02d", hours);
        gui_scroll_wheel_new_render(formatted_time, text1, render_mode, IMAGE_ARRAY_MAP_LENGTH,
                                    IMAGE_ARRAY_MAP);
        sprintf(formatted_time, "%02d", minutes);
        gui_scroll_wheel_new_render(formatted_time, text2, render_mode, IMAGE_ARRAY_MAP_LENGTH,
                                    IMAGE_ARRAY_MAP);
        sprintf(formatted_time, "%02d", seconds);
        gui_scroll_wheel_new_render(formatted_time, text3, render_mode, IMAGE_ARRAY_MAP_LENGTH,
                                    IMAGE_ARRAY_MAP);
    }

}
static void timer_show_window_callback(gui_win_t *win)
{
    gui_log("per:%f\n", win->animate->progress_percent);
    GUI_WIDGET_POINTER_BY_NAME(text1, TIMER_TEXT_NAME1);
    GUI_BASE(text1)->x = TEXT1_X + win->animate->progress_percent * TEXT_GAP;
    GUI_WIDGET_POINTER_BY_NAME(text3, TIMER_TEXT_NAME3);
    GUI_BASE(text3)->x = TEXT3_X - win->animate->progress_percent * TEXT_GAP;
    if (gui_win_is_animation_end_frame(win))
    {
        /* show : */
        GUI_WIDGET_POINTER_BY_NAME(text4, TIMER_TEXT_NAME4);
        gui_obj_hidden(text4, 0);
        GUI_WIDGET_POINTER_BY_NAME(text5, TIMER_TEXT_NAME5);
        gui_obj_hidden(text5, 0);
        gui_win_set_animate(win, 1000, -1, (gui_animate_callback_t)timer_start_window_callback, win);
        enter = 0;
    }
}
static void switch_off_cb()
{
    GUI_WIDGET_POINTER_BY_NAME(win_setting, SETTING_WIN_NAME);
    gui_obj_hidden(win_setting, 0);
    GUI_WIDGET_POINTER_BY_NAME(win_timer, TIMER_WIN_NAME);
    gui_obj_hidden(win_timer, 1);
    gui_win_stop_animation(GUI_TYPE(gui_win_t, win_setting));
    enter = 0;
}
static void switch_on_cb(void *null1, void *null2, void *param)
{
    GUI_WIDGET_POINTER_BY_NAME(win, SETTING_WIN_NAME);
    gui_win_set_animate(GUI_TYPE(gui_win_t, win), 1000, 0,
                        (gui_animate_callback_t)timer_show_window_callback, win);
    gui_obj_hidden(win, 1);
    GUI_WIDGET_POINTER_BY_NAME(win_timer, TIMER_WIN_NAME);
    gui_obj_hidden(win_timer, 0);
    GUI_WIDGET_POINTER_BY_NAME(text4, TIMER_TEXT_NAME4);
    gui_obj_hidden(text4, 1);
    GUI_WIDGET_POINTER_BY_NAME(text5, TIMER_TEXT_NAME5);
    gui_obj_hidden(text5, 1);
    {
        GUI_WIDGET_POINTER_BY_NAME(wheel, WHEEL_NAME);
        int index = GUI_API(gui_scroll_wheel_new_t).get_index((void *)wheel);
        GUI_WIDGET_POINTER_BY_NAME(text1, TIMER_TEXT_NAME1);
        gui_scroll_wheel_new_render(string_array[index], text1, render_mode, IMAGE_ARRAY_MAP_LENGTH,
                                    IMAGE_ARRAY_MAP);
    }
    {
        GUI_WIDGET_POINTER_BY_NAME(wheel, WHEEL_NAME2);
        int index = GUI_API(gui_scroll_wheel_new_t).get_index((void *)wheel);
        GUI_WIDGET_POINTER_BY_NAME(text1, TIMER_TEXT_NAME2);
        gui_scroll_wheel_new_render(minutes_string_array[index], text1, render_mode, IMAGE_ARRAY_MAP_LENGTH,
                                    IMAGE_ARRAY_MAP);
    }
    {
        GUI_WIDGET_POINTER_BY_NAME(wheel, WHEEL_NAME3);
        int index = GUI_API(gui_scroll_wheel_new_t).get_index((void *)wheel);
        GUI_WIDGET_POINTER_BY_NAME(text1, TIMER_TEXT_NAME3);
        gui_scroll_wheel_new_render(minutes_string_array[index], text1, render_mode, IMAGE_ARRAY_MAP_LENGTH,
                                    IMAGE_ARRAY_MAP);
    }
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void app_clock_ui_design(gui_obj_t *parent)
{
    gui_canvas_rect_create(parent, 0, 0, 0, SCREEN_W, SCREEN_H, APP_COLOR_SILVER);
    const int win_height = 50;
    gui_win_t *win_stop_watch = gui_win_create(parent, 0, 0, SCREEN_H - win_height,
                                               SCREEN_W / 2, win_height);
    gui_win_t *win_timer = gui_win_create(parent, 0, SCREEN_W / 2 + 1,
                                          SCREEN_H - win_height, SCREEN_W / 2, win_height);
    gui_multi_level_t *ml0 = gui_multi_level_create(parent, 0, stop_watch_ml0);
    gui_multi_level_create(ml0, 0, stop_watch_ml1_0);
    gui_multi_level_create(ml0, 0, stop_watch_ml1_1);
    GUI_API(gui_multi_level_t).jump(ml0, 1, 0);
    gui_canvas_rect_create(GUI_BASE(win_stop_watch), STOPWATCH_RECT_NAME, 0, 0, SCREEN_W / 2,
                           win_height, APP_COLOR_SILVER);
    gui_win_press(win_stop_watch, (gui_event_cb_t)win_stop_watch_cb, ml0);
    gui_win_release(win_stop_watch, (gui_event_cb_t)win_release_cb, STOPWATCH_RECT_NAME);
    {
        char *text = "Stopwatch";
        int font_size = 16;
        gui_text_t *t = gui_text_create(win_stop_watch, 0, 0, 0, gui_get_screen_width() / 2,
                                        win_height);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_CENTER);
    }
    gui_canvas_rect_create(GUI_BASE(win_timer), TIMER_RECT_NAME, 0, 0, SCREEN_W / 2, win_height,
                           APP_COLOR_SILVER);
    gui_win_press(win_timer, (gui_event_cb_t)win_timer_cb, ml0);
    gui_win_release(win_timer, (gui_event_cb_t)win_release_cb, TIMER_RECT_NAME);
    {
        char *text = "Timer";
        int font_size = 16;
        gui_text_t *t = gui_text_create(win_timer, 0, 0, 0, gui_get_screen_width() / 2,
                                        win_height);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_CENTER);
    }
    extern void gui_app_create_status_bar(void *parent, gui_obj_t *ignore_gesture);
    gui_app_create_status_bar(parent, (void *)0);
    extern const uint32_t *gui_app_return_array[17];
    extern void gui_return_cb(gui_win_t *win);
    gui_return_create(parent, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), gui_return_cb, (void *)0);
}
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
