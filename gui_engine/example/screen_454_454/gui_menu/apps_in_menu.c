#include "../root_image/ui_resource.h"
#include <gui_app.h>
#include "gui_page.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_text.h"
#include "gui_win.h"
#include "gui_tabview.h"
#include "gui_return.h"
#include "gui_canvas_rect.h"
#include "gui_multi_level.h"
#include "gui_button.h"
#include "gui_soccer.h"
#include "gui_fps.h"
#include<stdio.h>
#include<time.h>
//Please search app name macro for entry
#define APP_CLOCK
#define APP_WATCH_FACE
#define APP_CALCULATOR
#define APP_SPORT
#define APP_MAP
#define APP_CARDVIEW
#define APP_BOX2D
#define APP_COMPASS
#define APP_SETTING
#define APP_VOLUME
#define APP_CYCLE_TRACKING
#define APP_MUSIC
#define APP_WEB
#define APP_BLOCK
#define APP_FRUIT_NINJA
#define APP_HEART_RATE
#define APP_SOCCER
#define APP_BOX2D_RING
#define APP_CALENDAR
GUI_APP_DEFINE(APP_HEART_RATE, app_hr_ui_design) // cppcheck-suppress syntaxError
/*Define a app with name APP_STOPWATCH*/
#define APP_STOPWATCH
GUI_APP_DEFINE_NAME(APP_STOPWATCH)
/*Define a app with name APP_STOPWATCH end*/
/*Define a app with name APP_MENU and entry func app_menu*/
#define APP_MENU
GUI_APP_DEFINE(APP_MENU,       app_menu)
/*Define a app with name APP_MENU and entry func app_menu end*/
GUI_APP_DEFINE_NAME(APP_MAP)
GUI_APP_DEFINE_NAME(APP_CARDVIEW)
GUI_APP_DEFINE_NAME(APP_COMPASS)
GUI_APP_DEFINE_NAME(APP_SETTING)
GUI_APP_DEFINE_NAME(APP_VOLUME)
GUI_APP_DEFINE_NAME(APP_CYCLE_TRACKING)
GUI_APP_DEFINE_NAME(APP_MUSIC)
GUI_APP_DEFINE_NAME(APP_WEB)
GUI_APP_DEFINE_NAME(APP_BLOCK)
GUI_APP_DEFINE_NAME(APP_FRUIT_NINJA)
GUI_APP_DEFINE_NAME(APP_CALCULATOR)
GUI_APP_DEFINE_NAME(APP_SOCCER)
GUI_APP_DEFINE_NAME(APP_BOX2D_RING)
GUI_APP_DEFINE_NAME(APP_CALENDAR)
#define SCREEN_W ((int)gui_get_screen_width())
#define SCREEN_H ((int)gui_get_screen_height())
/**
 * @link https://www.rapidtables.com/web/color/RGB_Color.html
*/
#define COLOR_RED gui_rgb(255, 0, 0)
#define COLOR_CRIMSON gui_rgb(220,20,60)
#define COLOR_FIREBRICK gui_rgb(178,34,34)
#define COLOR_WHITE gui_rgb(255,255,255)
#define COLOR_WHITE_OPACITY gui_rgba(255,255,255,150)
#define COLOR_SILVER gui_rgb(192,192,192)
#define COLOR_SILVER_OPACITY(opacity) gui_rgba(192,192,192, opacity)
#define HEART_ANI_NAME "_HEART_ANI"
#define HEART_ANI_W 180
#define PAGE_NAME "_heart_rate_page"
#define HR_BK_RECT_NAME "_HR_BK_RECT_NAME"
#define STOPWATCHTEXT "STOPWATCHTEXT"
#define APP_SWAP_ANIMATION_DUR 2000
static void heart_ani_cb(gui_win_t *img);
static void page_cb(gui_page_t *page);
static void win_cb(gui_win_t *win);
static void status_bar(void *parent, gui_obj_t *ignore_gesture);
static void status_bar_ani(gui_obj_t *ignore_gesture);
static void app_win_cb(gui_win_t *win);
extern gui_app_t *get_app_watch_ui(void);
#define RETURN_ARRAY_SIZE 17
extern const uint32_t *gui_app_return_array[RETURN_ARRAY_SIZE];
static void app_hr_ui_design(gui_app_t *app)
{
    gui_win_t *app_win = gui_win_create(GUI_APP_ROOT_SCREEN, 0, SCREEN_W, 0, SCREEN_W, SCREEN_H);
    gui_canvas_rect_create((void *)app_win, HR_BK_RECT_NAME, 0, 0, SCREEN_W, SCREEN_H, gui_rgba(0, 0, 0,
                           255));
    gui_win_set_animate(app_win, 1000, 0, (gui_animate_callback_t)app_win_cb, app_win);
    gui_page_t *page = gui_page_create(app_win, PAGE_NAME, 0, 0, 0, 0);
    gui_page_set_animate(page, 1000, -1, page_cb, page);
    gui_page_center_alignment(page, SCREEN_H);

    {
        gui_canvas_rect_t *rect = gui_canvas_rect_create((void *)page, "canvas_rect", 0, SCREEN_H, SCREEN_W,
                                                         SCREEN_H, COLOR_RED);
        gui_canvas_rect_create((void *)rect, "canvas_rect", 10, 100, SCREEN_W - 10 * 2, 2,
                               COLOR_SILVER_OPACITY(100));
        gui_canvas_rect_create((void *)rect, "canvas_rect", 10, 200, SCREEN_W - 10 * 2, 2,
                               COLOR_SILVER_OPACITY(100));
        gui_canvas_rect_create((void *)rect, "canvas_rect", 10, 300, SCREEN_W - 10 * 2, 2,
                               COLOR_SILVER_OPACITY(100));
        gui_canvas_rect_create((void *)rect, "canvas_rect", 10, 400, SCREEN_W - 10 * 2, 2,
                               COLOR_SILVER_OPACITY(100));
        gui_canvas_rect_create((void *)rect, "canvas_rect", 10, 100, 2, 400 - 100,
                               COLOR_SILVER_OPACITY(100));
        gui_canvas_rect_create((void *)rect, "canvas_rect", 10 + (SCREEN_W - 10 * 2) / 4, 100, 2, 400 - 100,
                               COLOR_SILVER_OPACITY(100));
        gui_canvas_rect_create((void *)rect, "canvas_rect", 10 + (SCREEN_W - 10 * 2) / 4 * 2, 100, 2,
                               400 - 100,
                               COLOR_SILVER_OPACITY(100));
        gui_canvas_rect_create((void *)rect, "canvas_rect", 10 + (SCREEN_W - 10 * 2) / 4 * 3, 100, 2,
                               400 - 100,
                               COLOR_SILVER_OPACITY(100));
        gui_canvas_rect_create((void *)rect, "canvas_rect", 10 + (SCREEN_W - 10 * 2) / 4 * 4, 100, 2,
                               400 - 100,
                               COLOR_SILVER_OPACITY(100));
        {
            char *text = "198";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4 * 4 - 30, 100,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        }
        {
            char *text = "132";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4 * 4 - 30, 200,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        }
        {
            char *text = "66";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4 * 4 - 30, 300,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        }
        {
            char *text = "12AM";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10, 400 - 18, gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        }
        {
            char *text = " 6AM";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4, 400 - 18,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        }
        {
            char *text = "12PM";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4 * 2, 400 - 18,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        }
        {
            char *text = " 6PM";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4 * 3, 400 - 18,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        }
        gui_canvas_rect_create((void *)rect, "canvas_rect", 10 + 10, 300, 5, 400 - 300, COLOR_SILVER);
        gui_canvas_rect_create((void *)rect, "canvas_rect", 10 + 10 + 100, 160, 5, 400 - 160, COLOR_SILVER);
        gui_canvas_rect_create((void *)rect, "canvas_rect", 10 + 10 + 200, 280, 5, 400 - 280, COLOR_SILVER);
        gui_canvas_rect_create((void *)rect, "canvas_rect", 10 + 10 + 280, 275, 5, 400 - 275, COLOR_SILVER);
    }
    {
        gui_canvas_rect_t *rect = gui_canvas_rect_create((void *)page, "canvas_rect", 0, SCREEN_H * 2,
                                                         SCREEN_W,
                                                         SCREEN_H, COLOR_CRIMSON);
        gui_canvas_rect_create((void *)rect, "canvas_rect", 10, 100, SCREEN_W - 10 * 2, 1, COLOR_SILVER);

    }
    gui_canvas_rect_create((void *)page, "canvas_rect", 0, SCREEN_H * 3, SCREEN_W, SCREEN_H,
                           COLOR_FIREBRICK);

    gui_win_t *win = gui_win_create(app_win, 0, 0, 0, SCREEN_W, SCREEN_H);
    gui_img_t *heart_ani = gui_img_create_from_mem(win, HEART_ANI_NAME, HEARTRATE04_BIN,
                                                   (SCREEN_W - HEART_ANI_W) / 2, 100, 0, 0);
    gui_win_set_animate(win, 1000, -1, (gui_animate_callback_t)heart_ani_cb, win);
    gui_img_set_mode(heart_ani, IMG_BYPASS_MODE);
    {
        char *text = "Current";
        int font_size = 16;
        gui_text_t *t = gui_text_create(page, "txt", 100, 300, gui_get_screen_width(), font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    }
    {
        char *text = "68";
        int font_size = 48;
        gui_text_t *t = gui_text_create(page, "txt", 100, 320, gui_get_screen_width(), font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIAL_SIZE48_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    }
    {
        char *text = "BPM";
        int font_size = 16;
        gui_text_t *t = gui_text_create(page, "txt", 157, 347, gui_get_screen_width(), font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_RED, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    }
    {
        char *text = "75 BPM, 2m ago";
        int font_size = 16;
        gui_text_t *t = gui_text_create(page, "txt", 100, 370, gui_get_screen_width(), font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
    }
    status_bar(GUI_APP_ROOT_SCREEN, (void *)page);

    /*gui_return_create*/
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)page);
    /*gui_return_create end*/
}
static void app_win_cb(gui_win_t *win)
{
    GUI_BASE(win)->x = (1.0f - win->animate->progress_percent) * SCREEN_W;
    gui_canvas_rect_t *img = 0;
    gui_obj_tree_get_widget_by_name((void *)win, HR_BK_RECT_NAME, (void *)&img);
    if (img)
    {
        gui_canvas_rect_set_opacity(img, (255 - 100)*win->animate->progress_percent + 100);
    }

    if (win->animate->progress_percent >= 1.0f)
    {
        gui_app_shutdown(get_app_watch_ui());
    }

}
static void heart_ani_cb(gui_win_t *win)
{
    static bool odd;
    if (win->animate->progress_percent == 0)
    {
        odd = !odd;
    }
    uint32_t *heart_ani_array[] =
    {
        HEARTRATE04_BIN,
        HEARTRATE08_BIN,
        HEARTRATE12_BIN,
        HEARTRATE16_BIN,
        HEARTRATE20_BIN,
        HEARTRATE24_BIN,
        HEARTRATE28_BIN,
        HEARTRATE32_BIN,
        HEARTRATE36_BIN,
        HEARTRATE40_BIN,
        HEARTRATE44_BIN,
        HEARTRATE48_BIN,
    };
    gui_img_t *img = 0;
    gui_obj_tree_get_widget_by_name((void *)win, HEART_ANI_NAME, (void *)&img);
    if (odd)
    {
        gui_img_set_attribute(img, HEART_ANI_NAME,
                              heart_ani_array[(int)((1.0f - win->animate->progress_percent) *
                                                                (float)(sizeof(heart_ani_array) / sizeof(uint32_t *) - 2)) + 1], GET_BASE(img)->x,
                              GET_BASE(img)->y);
    }
    else
    {
        gui_img_set_attribute(img, HEART_ANI_NAME,
                              heart_ani_array[(int)(win->animate->progress_percent * (float)(sizeof(
                                                                                             heart_ani_array) / sizeof(uint32_t *) - 1))], GET_BASE(img)->x, GET_BASE(img)->y);
    }
}
static void page_cb(gui_page_t *page)
{
    gui_img_t *img = 0;
    gui_app_t *app = GUI_APP_HANDLE(APP_HEART_RATE);
    gui_obj_tree_get_widget_by_name(GUI_APP_ROOT_SCREEN, HEART_ANI_NAME, (void *)&img);

    int offset = page->base.y;
    if (offset < -SCREEN_H)
    {
        offset = -SCREEN_H;
    }

    float scale = (0.5f - 1.0f) / ((float)SCREEN_H) * (float)(-offset) + 1.0f;
    if (scale < 0.5f)
    {
        scale = 0.5f;
    }
    float translate = ((float)((SCREEN_W - HEART_ANI_W) / 2)) / ((float)SCREEN_H) * (float)offset;
    gui_img_scale(img, scale, scale);
    gui_img_translate(img, translate, ((float)((100)) / ((float)SCREEN_H) * (float)offset));
}
static void win_cb(gui_win_t *win)
{
    GUI_APP_SWAP_HANDLE(gui_current_app(), get_app_watch_ui());
    gui_tabview_t *tabview = 0;
    gui_obj_tree_get_widget_by_name(&(get_app_watch_ui()->screen), "tabview", (void *)&tabview);
    if (tabview)
    {
        gui_tabview_jump_tab(tabview, -1, 0);
    }


}
void gui_return_cb(gui_win_t *win)
{
    win_cb(win);
}
#define TIME_SCALE_RATE (0.35F)
#define STATUS_BAR_HEIGHT 20
#define STATUS_BAR_TIME_TEXT "STATUS_BAR_TIME_TEXT"
#define STATUS_BAR_DATE_TEXT "STATUS_BAR_DATE_TEXT"
#define STATUS_BAR_WINDOW "STATUS_BAR_WINDOW"
static gui_canvas_rect_t *rect;
void gui_app_create_status_bar(void *parent, gui_obj_t *ignore_gesture)
{
    status_bar(parent, ignore_gesture);
}
static void status_bar(void *parent, gui_obj_t *ignore_gesture)
{
    gui_win_t *status_bar = gui_win_create(parent, 0, 0, 0, SCREEN_W, SCREEN_H);
    rect = gui_canvas_rect_create((void *)status_bar, "canvas_rect", 0, 0, SCREEN_W, SCREEN_H,
                                  COLOR_SILVER_OPACITY(230));
    GET_BASE(rect)->not_show = 1;
    gui_canvas_rect_set_opacity(rect, 0);
    gui_win_set_animate(status_bar, 1000, -1, (gui_animate_callback_t)status_bar_ani, ignore_gesture);
    {
        char *text = "07:55";
        int font_size = 48;
        gui_text_t *t = gui_text_create(status_bar, STATUS_BAR_TIME_TEXT, SCREEN_W / 2 - 56, 2,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIAL_SIZE48_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_convert_to_img(t, ARGB8888);

        gui_img_scale(t->scale_img, TIME_SCALE_RATE, TIME_SCALE_RATE);
        gui_img_rotation(t->scale_img, 0, gui_img_get_width(t->scale_img) / 2, 0);
        gui_img_translate(t->scale_img, gui_img_get_width(t->scale_img) / 2, 0);
    }
    {
        gui_win_t *win = gui_win_create(status_bar, STATUS_BAR_WINDOW, 0, 0, SCREEN_W, SCREEN_H);
        GET_BASE(win)->not_show = 1;

        {
            char *text = "Tue, Apr 16";
            int font_size = 16;
            gui_text_t *t = gui_text_create(win, STATUS_BAR_DATE_TEXT, 0, 52, gui_get_screen_width(),
                                            font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, CENTER);

        }
        {
            char *text = "NO NOTIFICATIONS";
            int font_size = 16;
            gui_text_t *t = gui_text_create(win, 0, 0, 200, gui_get_screen_width(),
                                            font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, CENTER);

        }
    }


}
#include "tp_algo.h"
static void status_bar_ani(gui_obj_t *ignore_gesture)
{
    gui_text_t *time_txt = 0;
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), STATUS_BAR_TIME_TEXT,
                                    (void *)&time_txt);
    gui_text_t *date_txt = 0;
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), STATUS_BAR_DATE_TEXT,
                                    (void *)&date_txt);
    gui_win_t *win = 0;
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), STATUS_BAR_WINDOW, (void *)&win);
    if (time_txt)
    {

        static char buffer_text[6] = "07:55";
#if _WIN32
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer_text, 6, "%H:%M", timeinfo);
        //puts(buffer_text);
#endif
        gui_text_content_set(time_txt, buffer_text, strlen(buffer_text));
        gui_text_convert_to_img(time_txt, ARGB8888);
    }
    if (date_txt)
    {
        static char buffer[20] = "Tue, Apr 16";
#if _WIN32
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(buffer, 20, "%a, %b %d", timeinfo); // output date in "Tue, Apr 16" format
        //puts(buffer);
#endif
        gui_text_content_set(date_txt, buffer, strlen(buffer));
    }
    touch_info_t *tp = tp_get_info();
    static bool press, expand, shrink, expand_press;
    if (tp->y < STATUS_BAR_HEIGHT && !expand)
    {
        if (tp->pressed)
        {
            press = 1;
        }
    }
    int deltaY = tp->deltaY;

    if (press)
    {
        if (ignore_gesture)
        {
            ignore_gesture->gesture = 1;
        }
        GET_BASE(rect)->not_show = 0;
        if (deltaY > 0)
        {
            if (deltaY >= 100)
            {
                deltaY = 100;
                //GET_BASE(win)->not_show = 0;
            }


            int opacity = deltaY * 255 / 100;
            gui_canvas_rect_set_opacity(rect, opacity);
            float scale = deltaY * deltaY * ((1.0f - TIME_SCALE_RATE) / 10000.0f) + TIME_SCALE_RATE;
            if (scale > 1)
            {
                scale = 1;
            }
            if (time_txt)
            {
                gui_img_scale(time_txt->scale_img, scale, scale);
            }

        }
        if (tp->released)
        {
            if (deltaY >= 100)
            {
                expand = 1;
                gui_canvas_rect_set_opacity(rect, 255);
            }
            else
            {
                shrink = 1;
                //GET_BASE(win)->not_show = 1;
            }
            press = 0;
        }

    }
    if (shrink)
    {
        gui_canvas_rect_set_opacity(rect, 0);
        GET_BASE(rect)->not_show = 1;
        if (time_txt)
        {
            gui_img_scale(time_txt->scale_img, TIME_SCALE_RATE, TIME_SCALE_RATE);
        }

        shrink = 0;
        if (ignore_gesture)
        {
            ignore_gesture->gesture = 0;
        }
    }
    if (expand)
    {
        if (tp->pressed)
        {
            expand_press = 1;
        }
        if (ignore_gesture)
        {
            ignore_gesture->gesture = 1;
        }
        if (expand_press)
        {
            if (deltaY < 0)
            {
                if (deltaY < -100)
                {
                    deltaY = -100;
                }
                int opacity = (100 - (-deltaY)) * 255 / 100;
                gui_canvas_rect_set_opacity(rect, opacity);
                float scale = (100 - (-deltaY)) * ((1.0f - TIME_SCALE_RATE) / 100.0f) + TIME_SCALE_RATE;
                gui_img_scale(time_txt->scale_img, scale, scale);
                //GET_BASE(win)->not_show = 1;
            }



            if (tp->released)
            {
                if (deltaY < 0)
                {
                    expand = 0;
                    gui_canvas_rect_set_opacity(rect, 0);
                    GET_BASE(rect)->not_show = 1;
                    gui_img_scale(time_txt->scale_img, TIME_SCALE_RATE, TIME_SCALE_RATE);
                    if (ignore_gesture)
                    {
                        ignore_gesture->gesture = 0;
                    }
                }
                expand_press = 0;
            }

        }

    }
    if (win)
    {
        if ((deltaY >= 100 && (press)) || (deltaY >= 0 && expand))
        {
            GET_BASE(win)->not_show = 0;
        }
        else
        {
            GET_BASE(win)->not_show = 1;
        }
    }



}
/* app swap animation callback of the next app*/
#include "gui_menu_cellular.h"
#include "math.h"
static void menu_win_ani_cb(void *args, gui_win_t *win)
{
    float pro = gui_win_get_animation_progress_percent(win);
    gui_win_set_scale_rate(win, sinf(pro * (M_PI / 2 - 0.2f) + 0.2f),
                           sinf(pro * (M_PI / 2 - 0.2f) + 0.2f));
    gui_win_set_opacity(win, (pro) * UINT8_MAX);
    if (gui_win_is_animation_end_frame(win))
    {
        gui_win_set_scale_rate(win, 0, 0);//reset scale
    }
}
#define APP_WATCH_WIN_NAME "menu win name"
static void menu_win_ani_cb_return(void *args, gui_win_t *win)
{
    float pro = gui_win_get_animation_progress_percent(win);
    gui_win_set_opacity(win, (1.0f - pro) * UINT8_MAX);
    gui_win_set_scale_rate(win, cosf(pro * M_PI / 2), cosf(pro * M_PI / 2));
    if (gui_win_is_animation_end_frame(win))
    {
        GUI_APP_SHUTDOWN(APP_MENU)
    }
}
static void app_wathc_win_ani_cb_return(void *args, gui_win_t *win)
{
    float pro = gui_win_get_animation_progress_percent(win);
    gui_win_set_opacity(win, (pro) * UINT8_MAX);
    gui_win_set_scale_rate(win, 1 + cosf(pro * M_PI / 2), 1 + cosf(pro * M_PI / 2));
    if (gui_win_is_animation_end_frame(win))
    {
        /*Overwrite app watch's aniamtion as the shut down animation*/
        extern void app_watch_mune_win_ani_cb(void *args, gui_win_t *win);
        gui_win_set_animate(win, APP_SWAP_ANIMATION_DUR, 0,
                            (gui_animate_callback_t)app_watch_mune_win_ani_cb,
                            0);
        gui_win_stop_animation(win);
    }
}
#define APP_MENU_WIN_NAME "APP_MENU_WIN_NAME"
static void app_menu_win_cb(gui_obj_t *this)//this widget, event code, parameter
{
    gui_app_layer_buttom();
    gui_app_startup(get_app_watch_ui());
    gui_tabview_t *tabview = 0;
    gui_obj_tree_get_widget_by_name(&(get_app_watch_ui()->screen), "tabview", (void *)&tabview);
    if (tabview)
    {
        gui_tabview_jump_tab(tabview, -1, 0);
    }
    {
        gui_win_t *win = 0;
        gui_obj_tree_get_widget_by_name(&(get_app_watch_ui()->screen), APP_WATCH_WIN_NAME, (void *)&win);
        if (win)
        {
            /*Overwrite app watch's aniamtion as the start animation*/
            gui_win_set_animate(win, APP_SWAP_ANIMATION_DUR, 0,
                                (gui_animate_callback_t)app_wathc_win_ani_cb_return,
                                0);
        }
    }
    gui_win_t *win = 0;
    gui_obj_tree_get_widget_by_name(&(GUI_APP_HANDLE(APP_MENU)->screen), APP_MENU_WIN_NAME,
                                    (void *)&win);
    if (win)
    {
        gui_win_set_animate(win, APP_SWAP_ANIMATION_DUR, 0, (gui_animate_callback_t) menu_win_ani_cb_return,
                            0);
    }
}

static void app_menu_cb(void *obj, gui_event_t e, void *param)
{
    gui_log("%d,%p\n", GUI_TYPE(gui_obj_t, obj)->type, param);
}
/*Define APP_MENU's entry func */
static void app_menu(gui_app_t *app)
{
    /**
     * @link https://docs.realmcu.com/Honeygui/latest/widgets/gui_menu_cellular.html#example
    */
    gui_win_t *win = gui_win_create(GUI_APP_ROOT_SCREEN, APP_MENU_WIN_NAME, 0, 0, 0, 0);
    gui_win_set_animate(win, APP_SWAP_ANIMATION_DUR, 0, (gui_animate_callback_t)menu_win_ani_cb,
                        0);//aniamtion start to play at app startup
    /* app swap animation configration of the next app*/
    uint32_t *array[] =
    {
        I4500009_BIN,
        I4500011_BIN,
        I4500015_BIN,
        I4500020_BIN,
        I4500028_BIN,
        I4500034_BIN,
        I4500035_BIN,
        I4500037_BIN,
        I4500040_BIN,
        I4500042_BIN,
        I4500047_BIN,
        I4500049_BIN,
        I4500052_BIN,
        I4500064_BIN,
        I4500009_BIN,
        I4500011_BIN,
        I4500015_BIN,
        I4500020_BIN,
        I4500028_BIN,
        I4500034_BIN,
        I4500035_BIN,
        I4500037_BIN,
        I4500040_BIN,
        I4500042_BIN,
        I4500047_BIN,
        I4500049_BIN,
        I4500052_BIN,
        I4500064_BIN,
        I4500009_BIN,
        I4500011_BIN,
        I4500015_BIN,
        I4500020_BIN,
        I4500028_BIN,
        I4500034_BIN,
        I4500035_BIN,
        I4500037_BIN,
        I4500040_BIN,
        I4500042_BIN,
        I4500047_BIN,
        I4500049_BIN,
        I4500052_BIN,
        I4500064_BIN,
        I4500009_BIN,
        I4500011_BIN,
        I4500015_BIN,
        I4500020_BIN,

    };

    gui_menu_cellular_t *cell = gui_menu_cellular_create(win, 100, array,
                                                         sizeof(array) / sizeof(uint32_t *));
    gui_menu_cellular_offset((void *)cell, -36, -216);
    {
        struct gui_menu_cellular_gesture_parameter gesture_parameter_array[] =
        {
            {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1},
            {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1},
            {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1},
            {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1},
            {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1},
            {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1},
            {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1},
            {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1},
            {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1}, {app_menu_cb, (void *)1},
            {app_menu_cb, (void *)1},
        };
        gui_menu_cellular_on_click(cell, gesture_parameter_array,
                                   sizeof(gesture_parameter_array) / sizeof(gesture_parameter_array[0]));

    }
    //status_bar(win, (void *)cell);
    gui_return_create(win, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), app_menu_win_cb, (void *)cell);
}
/*Define APP_MENU's entry func end*/
#include "gui_seekbar.h"
#include "gui_img.h"
#define RETURN_HEIGHT 100
/*Define gui_app_return_array*/
const uint32_t *gui_app_return_array[] =
{
    PATH04_BIN,
    PATH05_BIN,
    PATH07_BIN,
    PATH08_BIN,
    PATH09_BIN,
    PATH11_BIN,
    PATH12_BIN,
    PATH14_BIN,
    PATH15_BIN,
    PATH16_BIN,
    PATH18_BIN,
    PATH19_BIN,
    PATH20_BIN,
    PATH22_BIN,
    PATH23_BIN,
    PATH24_BIN,
    PATH25_BIN,
};
/*Define gui_app_return_array end*/

/*define the app's ui design*/
GUI_APP_ENTRY(APP_STOPWATCH)
{
    extern void app_clock_ui_design(gui_obj_t *parent);
    app_clock_ui_design(GUI_APP_ROOT_SCREEN);
}
/*define the app's ui design end*/

#include "gui_map.h"
GUI_APP_ENTRY(APP_MAP)
{
    gui_obj_t *map = (void *)gui_map_create(GUI_APP_ROOT_SCREEN);
    {
        char *text = "MAP";
        int font_size = 16;
        gui_text_t *t = gui_text_create(map, text, 0, 52, gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, CENTER);

    }
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)map);
}
#include "gui_cardview.h"
#include "gui_card.h"
#include "gui_curtainview.h"
#include "gui_curtain.h"
GUI_APP_ENTRY(APP_CARDVIEW)
{
    gui_curtainview_t *ct = gui_curtainview_create(GUI_APP_ROOT_SCREEN, 0, 0, 0, 454, 454);
    gui_curtain_t *ct_middle = gui_curtain_create(ct, 0, 0, 0, 454, 454, CURTAIN_MIDDLE, 1.0f);

    gui_curtain_t *ct_down = gui_curtain_create(ct, 0, 0, 0, 454, 454, CURTAIN_DOWN, 1.0f);
    gui_canvas_rect_create((gui_obj_t *)ct_middle, "canvas_rect", 454 / 2, 454 - 50, 10, 10,
                           COLOR_WHITE);
    gui_canvas_rect_create((gui_obj_t *)ct_middle, "canvas_rect", 454 / 2, 454 - 30, 10, 10,
                           COLOR_WHITE);
    gui_canvas_rect_create((gui_obj_t *)ct_middle, "canvas_rect", 454 / 2, 454 - 10, 10, 10,
                           COLOR_WHITE);
    gui_cardview_t *cv = gui_cardview_create(ct_down, "cardview", 0, 0, 0, 0);
    uint32_t *image_array[] =
    {
        A1_BIN,
        A2_BIN,
        A3_BIN,
        A4_BIN,
        A5_BIN,
        A6_BIN,
        A5_BIN,
        A4_BIN,
        A3_BIN,
        A2_BIN,
    };
    for (size_t i = 0; i < 10; i++)
    {
        gui_card_t *card = gui_card_create(cv, 0, 0, 0, 0, 140);
        gui_img_create_from_mem(card, 0, image_array[i], (454 - 128) / 2, 0, 0, 0);
    }
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)0);
}

GUI_APP_DEFINE_NAME(APP_BOX2D)

GUI_APP_ENTRY(APP_BOX2D)
{
#ifdef ENABLE_RTK_GUI_BOX2D
    extern void app_box2d_ui_design(gui_obj_t *obj);
    app_box2d_ui_design(GUI_APP_ROOT_SCREEN);
#else
    {
        char *text = "Please uncomment CONFIG_REALTEK_BUILD_GUI_BOX2D";
        int font_size = 16;
        gui_text_t *t = gui_text_create(GUI_APP_ROOT_SCREEN, 0,  0, 200,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
    {
        char *text = "in win32_sim/menu_config.h";
        int font_size = 16;
        gui_text_t *t = gui_text_create(GUI_APP_ROOT_SCREEN, 0,  0, 220,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
#endif
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)0);
}
/**
 * @brief setting app (multi level)
*/
/* define of ui_design_0 of (0,0)*/

#define HIGHLIGHT_BLUE gui_rgb(0, 100, 255)
struct button_args
{
    gui_multi_level_t *parent;
    unsigned char index;
    unsigned char level;
};

static void multi_levle_button_cb(gui_obj_t *this, void *event, struct button_args *args)
{
    GUI_API(gui_multi_level_t).jump(args->parent, args->level, args->index);
}
static void ui_design_0(gui_obj_t *parent)
{
    {
        gui_button_t *button = gui_button_create(parent, 0, 0, 454, 100, MYDEVICE_BIN, MYDEVICEHL_BIN, 0, 0,
                                                 0);
        static struct button_args args;
        args.parent = (void *)parent;
        args.level = 1;
        args.index = 0;
        GUI_API(gui_button_t).on_click(button, (gui_event_cb_t)multi_levle_button_cb, &args);
    }
    {
        gui_button_t *button = gui_button_create(parent, 0, 102 * 1, 454, 100, BATTERY_BIN, BATTERYHL_BIN,
                                                 0, 0, 0);
        static struct button_args args;
        args.parent = (void *)parent;
        args.level = 1;
        args.index = 1;
        GUI_API(gui_button_t).on_click(button, (gui_event_cb_t)multi_levle_button_cb, &args);
    }
    {
        gui_button_t *button = gui_button_create(parent, 0, 102 * 2, 454, 100, BLUETOOTH_BIN,
                                                 BLUETOOTHHL_BIN, 0, 0, 0);
        static struct button_args args;
        args.parent = (void *)parent;
        args.level = 1;
        args.index = 2;
        GUI_API(gui_button_t).on_click(button, (gui_event_cb_t)multi_levle_button_cb, &args);
    }
    gui_return_create(parent, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)0);
}
/* define of ui_design_1_0 of (1,0)*/
static void mydevice_button_press_cb(gui_win_t *this, void *event, gui_canvas_rect_t *rect)
{
    rect->color = HIGHLIGHT_BLUE;
}
static void mydevice_button_release_cb(gui_win_t *this, void *event, gui_canvas_rect_t *rect)
{
    rect->color = APP_COLOR_WHITE;
}
static void setting_return_cb(gui_obj_t *this)
{
    if (this->parent && this->parent->type == MULTI_LEVEL)
    {
        if (this->parent->parent && this->parent->parent->type == MULTI_LEVEL)
        {
            uint8_t index = GUI_TYPE(gui_multi_level_t, this->parent->parent)->index;
            uint8_t level = GUI_TYPE(gui_multi_level_t, this->parent->parent)->level;
            GUI_API(gui_multi_level_t).jump((void *)this->parent, level, index);
        }
    }
}

#include "gui_grid.h"
static char *device_name = "Triton's watch";
static char *storage = "123.9 GB/256GB";
static void ui_design_1_0(gui_obj_t *parent)
{
    {
        gui_win_t *win = gui_win_create(parent, 0, 0, 0, 454, 100);
        static struct button_args args;
        args.parent = (void *)parent;
        args.level = 2;
        args.index = 0;
        gui_win_click(win, (gui_event_cb_t)multi_levle_button_cb, &args);
        gui_canvas_rect_t *rect = gui_canvas_rect_create((void *)win, 0, 0, 0, GUI_BASE(win)->w,
                                                         GUI_BASE(win)->h,
                                                         APP_COLOR_WHITE);
        gui_win_press(win, (gui_event_cb_t)mydevice_button_press_cb, rect);
        gui_win_release(win, (gui_event_cb_t)mydevice_button_release_cb, rect);
        {
            char *text = "Device name";
            int font_size = 16;
            gui_text_t *t = gui_text_create(win, 0,  0, 42,
                                            gui_get_screen_width(),
                                            font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, MULTI_LEFT);
        }
        {
            char *text = device_name;
            int font_size = 16;
            gui_text_t *t = gui_text_create(win, 0,  300, 42,
                                            gui_get_screen_width(),
                                            font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, APP_COLOR_GRAY, strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, MULTI_LEFT);
        }
        {
            char *text = ">";
            int font_size = 16;
            gui_text_t *t = gui_text_create(win, 0,  454 - 20, 42,
                                            gui_get_screen_width(),
                                            font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, APP_COLOR_GRAY, strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, MULTI_LEFT);
        }
    }
    {
        gui_win_t *win = gui_win_create(parent, 0, 0, 102, 454, 100);
        static struct button_args args;
        args.parent = (void *)parent;
        args.level = 2;
        args.index = 1;
        gui_win_click(win, (gui_event_cb_t)multi_levle_button_cb, &args);
        gui_canvas_rect_t *rect = gui_canvas_rect_create((void *)win, 0, 0, 0, GUI_BASE(win)->w,
                                                         GUI_BASE(win)->h,
                                                         APP_COLOR_WHITE);
        gui_win_press(win, (gui_event_cb_t)mydevice_button_press_cb, rect);
        gui_win_release(win, (gui_event_cb_t)mydevice_button_release_cb, rect);
        {
            char *text = "Storage";
            int font_size = 16;
            gui_text_t *t = gui_text_create(win, 0,  0, 42,
                                            gui_get_screen_width(),
                                            font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, MULTI_LEFT);
        }
        {
            char *text = storage;
            int font_size = 16;
            gui_text_t *t = gui_text_create(win, 0,  300, 42,
                                            gui_get_screen_width(),
                                            font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, APP_COLOR_GRAY, strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, MULTI_LEFT);
        }
        {
            char *text = ">";
            int font_size = 16;
            gui_text_t *t = gui_text_create(win, 0,  454 - 20, 42,
                                            gui_get_screen_width(),
                                            font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, APP_COLOR_GRAY, strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, MULTI_LEFT);
        }
    }
    gui_grid_t *grid = gui_grid_create(parent, 10, 204, 10, 1, 50, 50);
    {
        char *text = "1.0.21.0";
        int font_size = 16;
        gui_text_t *t = gui_text_create(grid, 0, 0, 0,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
        {
            char *text = "OS version";
            int font_size = 16;
            gui_text_t *t2 = gui_text_create(t, 0, 0, 20,
                                             gui_get_screen_width(),
                                             font_size);
            gui_text_set(t2, text, GUI_FONT_SRC_BMP, COLOR_SILVER, strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t2, addr1, FONT_SRC_MEMADDR);
            gui_text_mode_set(t2, MULTI_LEFT);
        }
    }
    {
        char *text = "XXXXXXXX";
        int font_size = 16;
        gui_text_t *t = gui_text_create(grid, 0, 100, 0,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
        {
            char *text = "CPU";
            int font_size = 16;
            gui_text_t *t2 = gui_text_create(t, 0, 0, 20,
                                             gui_get_screen_width(),
                                             font_size);
            gui_text_set(t2, text, GUI_FONT_SRC_BMP, COLOR_SILVER, strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t2, addr1, FONT_SRC_MEMADDR);
            gui_text_mode_set(t2, MULTI_LEFT);
        }
    }
    {
        char *text = "16GB";
        int font_size = 16;
        gui_text_t *t = gui_text_create(grid, 0, 0, 0,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
        {
            char *text = "RAM";
            int font_size = 16;
            gui_text_t *t2 = gui_text_create(t, 0, 0, 20,
                                             gui_get_screen_width(),
                                             font_size);
            gui_text_set(t2, text, GUI_FONT_SRC_BMP, COLOR_SILVER, strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t2, addr1, FONT_SRC_MEMADDR);
            gui_text_mode_set(t2, MULTI_LEFT);
        }
    }
    {
        char *text = "5000 mAh(typ)";
        int font_size = 16;
        gui_text_t *t = gui_text_create(grid, 0, 0, 0,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
        {
            char *text = "Battery capacity";
            int font_size = 16;
            gui_text_t *t2 = gui_text_create(t, 0, 0, 20,
                                             gui_get_screen_width(),
                                             font_size);
            gui_text_set(t2, text, GUI_FONT_SRC_BMP, COLOR_SILVER, strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t2, addr1, FONT_SRC_MEMADDR);
            gui_text_mode_set(t2, MULTI_LEFT);
        }
    }
    gui_return_create(parent, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), setting_return_cb, (void *)0);
}
static void ui_design_1_1(gui_obj_t *parent)
{
    gui_button_t *button = gui_button_create(parent, 0, 102 * 1, 454, 100, BATTERY_BIN, BATTERYHL_BIN,
                                             0, 0, 0);
    gui_return_create(parent, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), setting_return_cb, (void *)0);
}
static void ui_design_1_2(gui_obj_t *parent)
{
    gui_button_t *button = gui_button_create(parent, 0, 102 * 2, 454, 100, BLUETOOTH_BIN,
                                             BLUETOOTHHL_BIN, 0, 0, 0);
    gui_return_create(parent, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), setting_return_cb, (void *)0);
}
static void ui_design_2_0(gui_obj_t *parent)
{
    gui_canvas_rect_create(parent, 0, 0, 0, SCREEN_W, SCREEN_H, COLOR_WHITE);
    {
        char *text = "Rename device";
        int font_size = 16;
        gui_text_t *t = gui_text_create(parent, 0,  10, 10,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
    gui_win_t *win = gui_win_create(parent, 0, 10, 30, 400, 100);
    gui_canvas_rect_create((void *)win, 0, 0, 0, GUI_BASE(win)->w, GUI_BASE(win)->h, COLOR_SILVER);
    gui_canvas_rect_create((void *)win, 0, 0, 0, GUI_BASE(win)->w, 2, HIGHLIGHT_BLUE);
    gui_canvas_rect_create((void *)win, 0, 0, GUI_BASE(win)->h, GUI_BASE(win)->w, 2, HIGHLIGHT_BLUE);
    gui_canvas_rect_create((void *)win, 0, 0, 0, 2, GUI_BASE(win)->h, HIGHLIGHT_BLUE);
    gui_canvas_rect_create((void *)win, 0, GUI_BASE(win)->w, 0, 2, GUI_BASE(win)->h + 1,
                           HIGHLIGHT_BLUE);
    {
        char *text = device_name;
        int font_size = 16;
        gui_text_t *t = gui_text_create(win, 0,  10, 42,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
    gui_return_create(parent, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), setting_return_cb, (void *)0);
}
static void ui_design_2_1(gui_obj_t *parent)
{
    gui_canvas_rect_create(parent, 0, 0, 0, SCREEN_W, SCREEN_H, COLOR_WHITE);
    {
        char *text = "Storage space";
        int font_size = 16;
        gui_text_t *t = gui_text_create(parent, 0,  10, 10,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, APP_COLOR_BLACK, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
    gui_win_t *win = gui_win_create(parent, 0, 40, 40, 0, 0);
    gui_canvas_rect_create((void *)win, 0, 0, 0, 60, 10, COLOR_SILVER);
    gui_canvas_rect_create((void *)win, 0, 0, 10, 60, 20, COLOR_RED);
    gui_canvas_rect_create((void *)win, 0, 0, 10 + 20, 60, 30, APP_COLOR_GREEN);
    gui_canvas_rect_create((void *)win, 0, 0, 10 + 20 + 30, 60, 40, APP_COLOR_BLUE);
    gui_canvas_rect_create((void *)win, 0, 0, 10 + 20 + 30 + 40, 60, 50, APP_COLOR_GRAY);
    gui_return_create(parent, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), setting_return_cb, (void *)0);
}
/* Create multi levels in 0~2 level*/
GUI_APP_ENTRY(APP_SETTING)
{
    gui_multi_level_t *m0 = gui_multi_level_create(GUI_APP_ROOT_SCREEN, 0, ui_design_0);//main
    gui_multi_level_t *m1_0 = gui_multi_level_create(m0, 0, ui_design_1_0);//mydevice
    gui_multi_level_t *m1_1 = gui_multi_level_create(m0, 0, ui_design_1_1);//battery
    gui_multi_level_t *m1_2 = gui_multi_level_create(m0, 0, ui_design_1_2);//bluetooth
    gui_multi_level_t *m2_0 = gui_multi_level_create(m1_0, 0, ui_design_2_0);//rename
    gui_multi_level_t *m2_1 = gui_multi_level_create(m1_0, 0, ui_design_2_1);//storage
}
/* GUI_APP_ENTRY(APP_SETTING) end */
#include "math.h"
#include <stdio.h>

// Function to convert degrees to direction
const char *getDirection(double deg)
{
    const char *directions[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};

    // Ensure that the degree is between 0 (inclusive) to 360 (exclusive)
    while (deg < 0) { deg += 360; }
    while (deg >= 360) { deg -= 360; }

    // Divide the circle into 8 parts and return corresponding direction
    int index = (int)((deg + 22.5) / 45) % 8;
    return directions[index];
}



static void compass_win_cb(gui_img_t *img_array[])
{
    touch_info_t *tp = tp_get_info();
    static bool hold = 1;
    static int img_x1, img_x2;
    static bool edge_flag;
    static bool change_flag;
    static int offset;
    int img_w = gui_img_get_width(img_array[0]);
    int screen_w = gui_get_screen_width();
    // if (tp->pressed)
    // {
    //     hold = 1;
    //     img_x1 = GUI_BASE(img_array[0])->x;
    //     img_x2 = GUI_BASE(img_array[1])->x;
    // }else if (tp->released)
    // {
    //     hold = 0;
    //     img_x1 = GUI_BASE(img_array[0int])->x;
    //     img_x2 = GUI_BASE(img_array[1])->x;
    // }
    if (hold)
    {
        static float time;
        float displacement = sinf(time++ * 0.01f) * 200.0 * M_PI;
        offset = (int)displacement;
        int degree = offset / 7 % 360;
        if (degree < 0)
        {
            degree += 360;
        }

        gui_log("degree:%d\n", degree);
        {
            static char degree_text[4];
            sprintf(degree_text, "%d", degree);
            degree_text[3] = 0;
            gui_text_set((void *)img_array[2], degree_text,  GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(degree_text),
                         16);
        }
        {
            const char *degree_text = getDirection((double)degree);

            gui_text_set((void *)img_array[3], (char *)degree_text,  GUI_FONT_SRC_BMP, gui_rgba(0xff, 0x66,
                         0x00, 0xff), strlen(degree_text), 16);
        }
        gui_img_set_location(img_array[0], offset + img_x1, img_array[0]->base.y);
        gui_img_set_location(img_array[1], offset + img_x2, img_array[1]->base.y);
        {
            if (GUI_BASE(img_array[0])->x < 10 && GUI_BASE(img_array[0])->x >= -10)
            {
                edge_flag = 1;
            }
            else
            {
                edge_flag = 0;
                change_flag = 0;
            }
            if (edge_flag && !change_flag)
            {
                GUI_BASE(img_array[1])->x = GUI_BASE(img_array[0])->x - img_w - 4;
                change_flag = 1;
                img_x2 = GUI_BASE(img_array[1])->x - offset;
            }
        }
        {
            static bool edge_flag2;
            static bool change_flag2;
            if (GUI_BASE(img_array[1])->x < 10 && GUI_BASE(img_array[1])->x >= -10)
            {
                edge_flag2 = 1;
            }
            else
            {
                edge_flag2 = 0;
                change_flag2 = 0;
            }
            if (edge_flag2 && !change_flag2)
            {
                GUI_BASE(img_array[0])->x = GUI_BASE(img_array[1])->x - img_w - 4;
                change_flag2 = 1;
                img_x1 = GUI_BASE(img_array[0])->x - offset;
            }
        }

        {
            static bool edge_flag3;
            static bool change_flag3;
            if (GUI_BASE(img_array[1])->x + img_w < 10 + screen_w &&
                GUI_BASE(img_array[1])->x + img_w >= -10 + screen_w)
            {
                edge_flag3 = 1;
            }
            else
            {
                edge_flag3 = 0;
                change_flag3 = 0;
            }
            if (edge_flag3 && !change_flag3)
            {
                GUI_BASE(img_array[0])->x = GUI_BASE(img_array[1])->x + img_w + 4;
                change_flag3 = 1;
                img_x1 = GUI_BASE(img_array[0])->x - offset;
            }
        }
        {
            static bool edge_flag4;
            static bool change_flag4;
            if (GUI_BASE(img_array[0])->x + img_w < 10 + screen_w &&
                GUI_BASE(img_array[0])->x + img_w >= -10 + screen_w)
            {
                edge_flag4 = 1;
            }
            else
            {
                edge_flag4 = 0;
                change_flag4 = 0;
            }
            if (edge_flag4 && !change_flag4)
            {
                GUI_BASE(img_array[1])->x = GUI_BASE(img_array[0])->x + img_w + 4;
                change_flag4 = 1;
                img_x2 = GUI_BASE(img_array[1])->x - offset;
            }
        }
        gui_log("%d,%d\n", GUI_BASE(img_array[0])->x, GUI_BASE(img_array[1])->x);
    }




}
GUI_APP_ENTRY(APP_COMPASS)
{
    gui_win_t *win = gui_win_create(GUI_APP_ROOT_SCREEN, 0, 0, 0, 0, 0);

    gui_img_t *img = gui_img_create_from_mem(win, 0, TICKMARK_BIN, 200, 200, 0, 0);
    int img_w = gui_img_get_width(img);
    gui_img_t *img2 = gui_img_create_from_mem(win, 0, TICKMARK_BIN, -img_w + 200 - 4, 200, 0, 0);
    gui_img_t *img3 = gui_img_create_from_mem(win, 0, HAND_BIN, gui_get_screen_width() / 2 - 39 / 2,
                                              260, 0, 0);
    gui_img_set_mode(img3, IMG_SRC_OVER_MODE);
    static gui_img_t *img_array[4];
    img_array[0] = img;
    img_array[1] = img2;
    gui_win_set_animate(win, 1000, -1, (gui_animate_callback_t)compass_win_cb, img_array);
    {
        char *text = "0";
        int font_size = 16;
        gui_text_t *t = gui_text_create(GUI_APP_ROOT_SCREEN, "compass text", 0, 180, gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, CENTER);
        img_array[2] = (void *)t;

    }
    {
        char *text = "N";
        int font_size = 16;
        gui_text_t *t = gui_text_create(GUI_APP_ROOT_SCREEN, "direction text", 0, 160,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgba(0xff, 0x66, 0x00, 0xff), strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, CENTER);
        img_array[3] = (void *)t;

    }
}
static void volume_cb(gui_img_t *volume, gui_win_t *win)
{
    GUI_TOUCHPAD_IMPORT_AS_TP
    static bool hold;
    int img_w = gui_img_get_width(volume);
    int img_h = gui_img_get_height(volume);
    int win_x = GUI_BASE(win)->x;
    int win_y = GUI_BASE(win)->y;
    static int scope_top;
    static float per;
    if (tp->pressed && (tp->x >= win_x && tp->x <= win_x + img_w) && (tp->y > win_y &&
                                                                      tp->y < win_y + img_h))
    {
        hold = 1;
        scope_top = volume->scope_y1;
    }
    else if (tp->released)
    {
        hold = 0;

    }
    if (hold)
    {
        int scope_x2 = scope_top + tp->deltaY;
        if (scope_x2 < 0)
        {
            scope_x2 = 0;
        }
        if (scope_x2 > img_h)
        {
            scope_x2 = img_h;
        }
        volume->scope_y1 = scope_x2;
        gui_log("percentage:%f\n", per);
    }
    if (img_h == 0)
    {
        img_h = 454;
    }
    per = ((float)(img_h - volume->scope_y1)) / ((float)img_h);
}
static void volume2_cb(gui_img_t *volume, gui_win_t *win)
{
    GUI_TOUCHPAD_IMPORT_AS_TP
    static bool hold;
    int img_w = gui_img_get_width(volume);
    int img_h = gui_img_get_height(volume);
    int win_x = GUI_BASE(win)->x;
    int win_y = GUI_BASE(win)->y;
    static int scope_top;
    static float per;
    if (tp->pressed && (tp->x >= win_x && tp->x <= win_x + img_w) && (tp->y > win_y &&
                                                                      tp->y < win_y + img_h))
    {
        hold = 1;
        scope_top = volume->scope_x2;
    }
    else if (tp->released)
    {
        hold = 0;
    }
    if (hold)
    {
        int scope_x2 = scope_top + tp->deltaX;
        if (scope_x2 < 0)
        {
            scope_x2 = 0;
        }
        if (scope_x2 > img_w)
        {
            scope_x2 = img_w;
        }
        volume->scope_x2 = scope_x2;
        gui_log("percentage:%f\n", per);
    }
    if (img_w == 0)
    {
        img_w = 454;
    }
    per = ((float)(volume->scope_x2)) / ((float)img_w);
    gui_text_t *t = 0;
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "volume text", (void *)&t);
    if (t)
    {
        static char text[sizeof("VOLUME:100.00%%")];
        sprintf(text, "VOLUME:%.2f%%", per);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, t->color, strlen(text), t->font_height);
    }

}
GUI_APP_ENTRY(APP_VOLUME)
{
    {
        //bar 1
        gui_win_t *win = gui_win_create(GUI_APP_ROOT_SCREEN, 0, 100, 20, SCREEN_W, SCREEN_H);
        gui_img_t *back = gui_img_create_from_mem(win, 0, VOLUMEBACK_BIN, 0, 0, 0, 0);
        gui_img_t *volume = gui_img_create_from_mem(win, 0, VOLUME_BIN, 0, 0, 0, 0);
        gui_img_set_mode(back, IMG_SRC_OVER_MODE);
        gui_img_set_mode(volume, IMG_SRC_OVER_MODE);
        gui_win_set_animate(win, 1000, -1, (gui_animate_callback_t)volume_cb, volume);
        volume->scope_flag = 1;
        volume->scope_y1 = 200;
    }
    {
        //bar 2
        gui_win_t *win = gui_win_create(GUI_APP_ROOT_SCREEN, 0, 240, 200, SCREEN_W, SCREEN_H);
        gui_img_t *back = gui_img_create_from_mem(win, 0, VOLUMEBACK2_BIN, 0, 0, 0, 0);
        gui_img_t *volume = gui_img_create_from_mem(win, 0, VOLUME2_BIN, 0, 0, 0, 0);
        gui_img_set_mode(back, IMG_SRC_OVER_MODE);
        gui_img_set_mode(volume, IMG_SRC_OVER_MODE);
        gui_win_set_animate(win, 1000, -1, (gui_animate_callback_t)volume2_cb, volume);
        volume->scope_flag = 1;
        volume->scope_x2 = 100;
        {
            //percentage text
            char *text = "VOLUME:100%";
            int font_size = 16;
            gui_text_t *t = gui_text_create(win, "volume text", 0, -20,
                                            gui_get_screen_width(),
                                            font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, LEFT);
        }
    }

    {
        //status_bar&return handle
        status_bar(GUI_APP_ROOT_SCREEN, (void *)0);
        gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                          sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)0);
    }
}


/**
 * @brief APP cycle tracking
*/
#include <math.h>

static double F(double x, double a, double b, double c)
{
    return cos((a + x) / b) * c;
}

static double Solve(double guess, double a, double b, double c)
{
    const double EPSILON = 0.1;
    double x = guess;
    while (fabs(x - F(x, a, b, c)) > EPSILON)
    {
        x = F(x, a, b, c);
    }
    return x;
}
static void get_img_array(gui_img_t **img_array, gui_win_t *root, int img_array_count)
{
    gui_list_t *node = NULL;
    gui_obj_t *obj = GUI_BASE(root);
    int count = 0;
    memset(img_array, 0, sizeof(*img_array)*img_array_count);
    gui_list_for_each(node, &obj->child_list)
    {
        if (count >= img_array_count)
        {
            return;
        }
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->type == IMAGE_FROM_MEM)
        {
            img_array[count++] = (void *)obj;
        }
    }

}
static void cycle_image_process(int delta_x, gui_img_t **img_array)
{
    //gui_log("dx:%d\n",delta_x);
    float offset = delta_x;
    float half_screen_w = 370 / 2;
    float img_w = 120;
    float gap = 5;
    float scale = cosf((float)offset / (float)half_screen_w * M_PI / 2);
    float half_w_3 =  img_w * scale / 2;
    img_array[3]->base.x = offset + half_screen_w - img_w / 2;
    img_array[3]->base.y = 180 * (1.0f - scale) / 2;
    gui_log("medium%f\n", scale);
    gui_img_translate(img_array[3], img_w / 2 * (1.0f - (half_w_3 / (img_w / 2))), 0);
    gui_img_scale(img_array[3], half_w_3 / (img_w / 2), half_w_3 / (img_w / 2));
    offset += half_w_3 + gap;
    for (size_t i = 0; i < 3; i++)
    {
        float solution = Solve(60, offset, half_screen_w / M_PI * 2, (img_w / 2));
        float scale = solution / (img_w / 2);
        //gui_log("Solution: %lf\n", solution);
        img_array[4 + i]->base.x = offset + half_screen_w;
        img_array[4 + i]->base.y = 180 * (1.0f - scale) / 2;
        gui_img_scale(img_array[4 + i], scale, scale);
        offset += 2 * solution + gap;
    }
    offset = delta_x - (half_w_3 + gap);
    for (size_t i = 0; i < 3; i++)
    {
        double solution = Solve(60, -offset, half_screen_w / M_PI * 2, (img_w / 2));
        //printf("Solution: %lf\n", solution);
        float scale = solution / (img_w / 2);
        img_array[2 - i]->base.x = offset + half_screen_w - solution * 2;
        img_array[2 - i]->base.y = 180 * (1.0f - scale) / 2;
        gui_img_scale(img_array[2 - i], scale, scale);
        offset -= 2 * solution + gap;
    }
}

static void cycle_tracking_win_cb(gui_win_t *win)
{
    GUI_TOUCHPAD_IMPORT_AS_TP
    static bool hold;
    int img_w = GUI_BASE(win)->w;
    int img_h = GUI_BASE(win)->h;
    int win_x = GUI_BASE(win)->x;
    int win_y = GUI_BASE(win)->y;
    static int scope_top;
    static float per;
    static bool swap, swap_flag, alien_flag;
    gui_img_t *img_array[7];
    static int id ;
    static bool exception = 0;
    static int dx_old;
    get_img_array(img_array, win, sizeof(img_array) / sizeof(img_array[0]));
    const void *data_array[] = {KEHUZHOUQI_BIN, KEHUZHOUQI_BIN, KEHUZHOUQI_BIN, KEHUZHOUQI_BIN, KEHUZHOUQI_BIN,
                                KEHUZHOUQI_BIN, KEHUZHOUQI_BIN, KEHUZHOUQI_BIN, KEHUZHOUQI_BIN,
                                KEHUZHOUQI_BIN, KEHUZHOUQI_BIN, KEHUZHOUQI_BIN, KEHUZHOUQI_BIN,
                                KEHUZHOUQI_BIN, KEHUZHOUQI_BIN, KEHUZHOUQI_BIN, KEHUZHOUQI_BIN,
                                KEHUZHOUQI_BIN, KEHUZHOUQI_BIN, KEHUZHOUQI_BIN, KEHUZHOUQI_BIN,
                                KEHUZHOUQI_BIN, KEHUZHOUQI_BIN, KEHUZHOUQI_BIN, KEHUZHOUQI_BIN,
                               };
    static int swap_count = sizeof(data_array) / sizeof(data_array[0]) / 2;
    int max_swap_count = sizeof(data_array) / sizeof(data_array[0]) - sizeof(img_array) / sizeof(
                             img_array[0]);
    if (tp->pressed && (tp->x >= win_x && tp->x <= win_x + img_w) && (tp->y > win_y &&
                                                                      tp->y < win_y + img_h))
    {
        hold = 1;
    }
    else if (tp->released)
    {
        //if (!swap)
        {
            alien_flag = 1;
        }
        hold = 0;
        swap = 0;
        swap_flag = 0;
        id = 0;
        exception = 0;
    }
    if (hold)
    {
//180,160,103 +-
        gui_log("tp:%d %d\n", tp->deltaX, swap);
        {

            int dx[] = {0, 87, 190, 274};
            int dx_right[] = {0, -87, -190, -274};

            //dx = tp->deltaX;
            //cycle_image_process(tp->deltaX+dx, img_array);
            if (gui_img_get_transform_scale_x(img_array[2]) >= 0.98f ||
                gui_img_get_transform_scale_x(img_array[4]) >= 0.98f)
            {
                swap_flag = 1;
                swap_count += (tp->deltaX > 0) ? -1 : 1;
                swap_count = (swap_count < 0) ? 0 : ((swap_count > max_swap_count) ? max_swap_count : swap_count);
                for (size_t i = 0; i < sizeof(img_array) / sizeof(img_array[0]); i++)
                {
                    img_array[i]->data = (void *)data_array[swap_count + i];
                }
                gui_log("img_array[2]\n");
                id++;
                if (id >= 4)
                {
                    id = 3;
                };
            }
            else if (gui_img_get_transform_scale_x(img_array[1]) >= 0.98f ||
                     gui_img_get_transform_scale_x(img_array[5]) >= 0.98f)
            {
                swap_flag = 1;
                swap_count += (tp->deltaX > 0) ? -1 : 1;
                swap_count += (tp->deltaX > 0) ? -1 : 1;
                swap_count = (swap_count < 0) ? 0 : ((swap_count > max_swap_count) ? max_swap_count : swap_count);
                for (size_t i = 0; i < sizeof(img_array) / sizeof(img_array[0]); i++)
                {
                    img_array[i]->data = (void *)data_array[swap_count + i];
                } gui_log("img_array[1]\n");
                //gui_log("tp:%d\n",tp->deltaX);
                id++;
                id++;
                if (id >= 4)
                {
                    id = 3;
                };
            }
            else if (gui_img_get_transform_scale_x(img_array[0]) ||
                     gui_img_get_transform_scale_x(img_array[6]) >= 0.98f)
            {
                swap_flag = 1;
                swap_count += (tp->deltaX > 0) ? -1 : 1;
                swap_count += (tp->deltaX > 0) ? -1 : 1;
                swap_count += (tp->deltaX > 0) ? -1 : 1;
                swap_count = (swap_count < 0) ? 0 : ((swap_count > max_swap_count) ? max_swap_count : swap_count);
                for (size_t i = 0; i < sizeof(img_array) / sizeof(img_array[0]); i++)
                {
                    img_array[i]->data = (void *)data_array[swap_count + i];
                } gui_log("img_array[0]\n");
                //gui_log("tp:%d\n",tp->deltaX);
                id++;
                id++;
                id++;
                if (id >= 4)
                {
                    id = 3;
                };
            }
            else
            {
                gui_log("else[0]:%f\n", gui_img_get_transform_scale_x(img_array[3]));


            }
            if (!exception)
            {
                if (dx_old >= 0 && tp->deltaX <= 0)
                {
                    id = 0;
                }
                if (dx_old <= 0 && tp->deltaX >= 0)
                {
                    id = 0;
                }
                if (tp->deltaX > 0)
                {
                    cycle_image_process(tp->deltaX - dx[id], img_array);
                    gui_log("1308\n");
                }
                else
                {
                    cycle_image_process(tp->deltaX - dx_right[id], img_array);
                    gui_log("1313:%d\n", id);
                }
                if (img_array[3]->base.y > 90)
                {
                    cycle_image_process(0, img_array); gui_log("1317\n");
                }


            }
            else
            {
                cycle_image_process(0, img_array);
            }
            if (swap == 1)
            {
                cycle_image_process(0, img_array);
            }


        }
        dx_old = tp->deltaX;
    }
    else
    {
        if (alien_flag)
        {
            if (gui_img_get_transform_scale_x(img_array[2]) > gui_img_get_transform_scale_x(img_array[3]))
            {
                swap_count += -1;
                swap_count = (swap_count < 0) ? 0 : ((swap_count > max_swap_count) ? max_swap_count : swap_count);
                for (size_t i = 0; i < sizeof(img_array) / sizeof(img_array[0]); i++)
                {
                    img_array[i]->data = (void *)data_array[swap_count + i];
                }

            }
            else if (gui_img_get_transform_scale_x(img_array[4]) > gui_img_get_transform_scale_x(img_array[3]))
            {
                swap_count += 1;
                swap_count = (swap_count < 0) ? 0 : ((swap_count > max_swap_count) ? max_swap_count : swap_count);
                for (size_t i = 0; i < sizeof(img_array) / sizeof(img_array[0]); i++)
                {
                    img_array[i]->data = (void *)data_array[swap_count + i];
                }
            }
            cycle_image_process(0, img_array);
            alien_flag = 0;
        }

    }

}

GUI_APP_ENTRY(APP_CYCLE_TRACKING)
{
    gui_win_t *win = gui_win_create(GUI_APP_ROOT_SCREEN, "WIN", 0, 200, 0, 0);
    gui_win_set_animate(win, 1000, -1, (gui_animate_callback_t)cycle_tracking_win_cb, win);
    {
        gui_img_t *img = gui_img_create_from_mem(win, 0, KEHUZHOUQI_BIN, 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(win, 0, KEHUZHOUQI_BIN, 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(win, 0, KEHUZHOUQI_BIN, 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(win, 0, KEHUZHOUQI_BIN, 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(win, 0, KEHUZHOUQI_BIN, 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(win, 0, KEHUZHOUQI_BIN, 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(win, 0, KEHUZHOUQI_BIN, 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    gui_canvas_rect_create(GUI_APP_ROOT_SCREEN, 0, 368 / 2 - 1, 0, 2, 448, COLOR_RED);
    gui_img_t *img_array[7];
    get_img_array(img_array, win, sizeof(img_array) / sizeof(img_array[0]));
    cycle_image_process(0, img_array);

    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)win);
}

GUI_APP_ENTRY(APP_WEB)
{
#ifdef ENABLE_RTK_GUI_WEB
    extern int gui_web_open_html(const char *html_file, gui_obj_t *parent);
    gui_web_open_html("gui_engine\\example\\web\\mechanical-clock-canvas\\index.html",
                      GUI_APP_ROOT_SCREEN);

#else
    {
        char *text = "Please uncomment CONFIG_REALTEK_BUILD_WEB";
        int font_size = 16;
        gui_text_t *t = gui_text_create(GUI_APP_ROOT_SCREEN, 0,  0, 200,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
    {
        char *text = "in win32_sim/menu_config.h";
        int font_size = 16;
        gui_text_t *t = gui_text_create(GUI_APP_ROOT_SCREEN, 0,  0, 220,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
#endif
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)0);
}
struct app_block
{
    int x, y;
    void *render;
    bool curtain;
};
void ui_0_0(gui_obj_t *parent)
{
    gui_img_create_from_mem(parent, 0, CLOCK_BASE_BIN, 0, 0, 0, 0);
    {
        char *text = "0,0";
        int font_size = 16;
        gui_text_t *t = gui_text_create(parent, 0,  454 / 2, 454 / 2,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
}
void ui_0_1(gui_obj_t *parent)
{
    gui_img_create_from_mem(parent, 0, CLOCK_BASE_BIN, 0, 0, 0, 0);
    {
        char *text = "0,1";
        int font_size = 16;
        gui_text_t *t = gui_text_create(parent, 0,  454 / 2, 454 / 2,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
}
void ui_1_1(gui_obj_t *parent)
{
    gui_img_create_from_mem(parent, 0, CLOCK_BASE_BIN, 0, 0, 0, 0);
    {
        char *text = "1,1";
        int font_size = 16;
        gui_text_t *t = gui_text_create(parent, 0,  454 / 2, 454 / 2,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
}
void ui_0_m1(gui_obj_t *parent)
{
    gui_img_create_from_mem(parent, 0, CLOCK_BASE_BIN, 0, 0, 0, 0);
    {
        char *text = "0,-1";
        int font_size = 16;
        gui_text_t *t = gui_text_create(parent, 0,  454 / 2, 454 / 2,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
}
void ui_m1_0(gui_obj_t *parent)
{
    gui_img_create_from_mem(parent, 0, CLOCK_BASE_BIN, 0, 0, 0, 0);
    {
        char *text = "-1, 0";
        int font_size = 16;
        gui_text_t *t = gui_text_create(parent, 0,  454 / 2, 454 / 2,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
}
void ui_m1_m1(gui_obj_t *parent)
{
    gui_img_create_from_mem(parent, 0, CLOCK_BASE_BIN, 0, 0, 0, 0);
    {
        char *text = "-1, -1";
        int font_size = 16;
        gui_text_t *t = gui_text_create(parent, 0,  454 / 2, 454 / 2,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
}
void ui_m1_1(gui_obj_t *parent)
{
    gui_img_create_from_mem(parent, 0, CLOCK_BASE_BIN, 0, 0, 0, 0);
    {
        char *text = "-1, 1";
        int font_size = 16;
        gui_text_t *t = gui_text_create(parent, 0,  454 / 2, 454 / 2,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
}
void ui_1_m1(gui_obj_t *parent)
{
    gui_img_create_from_mem(parent, 0, CLOCK_BASE_BIN, 0, 0, 0, 0);
    {
        char *text = "1, -1";
        int font_size = 16;
        gui_text_t *t = gui_text_create(parent, 0,  454 / 2, 454 / 2,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
}
void ui_1_0(gui_obj_t *parent)
{
    gui_img_create_from_mem(parent, 0, CLOCK_BASE_BIN, 0, 0, 0, 0);
    {
        char *text = "1, 0";
        int font_size = 16;
        gui_text_t *t = gui_text_create(parent, 0,  454 / 2, 454 / 2,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
}
struct app_block block_array[] =
{
    {0, 0, ui_0_0}, {0, 1, ui_0_1, 1}, {1, 1, ui_1_1}, {0, -1, ui_0_m1}, {-1, 0, ui_m1_0, 1}, {-1, -1, ui_m1_m1},
    {-1, 1, ui_m1_1}, {1, -1, ui_1_m1}, {1, 0, ui_1_0, 1}, {-2, 0, ui_1_1},
};
static gui_win_t *win_index;
static gui_win_t *win_next;
static int offset_x,  offset_y,  tp_x,  tp_y;
static int offset_x_index,  offset_y_index,  tp_x_index,  tp_y_index;

gui_vertex_t normal, normal_index;
#define SLIDE 0
#define CUBE 1
static int style = CUBE;
static void block_transform_cube(gui_win_t *block, int offset_x, int offset_y, int tp_x, int tp_y);
static void block_transform_sync(gui_win_t *block, int offset_x, int offset_y, int tp_x, int tp_y)
{
    switch (style)
    {
    case SLIDE:
        {
            matrix_translate(offset_x * (int)block->base.w + tp_x, \
                             offset_y * (int)block->base.h + tp_y, \
                             block->base.matrix);
        }
        break;
    case CUBE:
        {
            //gui_log("%d,%d,%d,%d\n",offset_x,  offset_y,  tp_x, tp_y);
            if (offset_y != 0 || tp_y != 0) //vertical
            {
                matrix_translate(offset_x * (int)block->base.w + tp_x, \
                                 offset_y * (int)block->base.h + tp_y, \
                                 block->base.matrix);
            }
            else
            {
                block_transform_cube(block, offset_x,  offset_y,  tp_x, tp_y);
            }


        }
        break;
    default:
        break;
    }

}

static void block_transform_cube(gui_win_t *block, int offset_x, int offset_y, int tp_x, int tp_y)
{
    gui_matrix_t temp;
    gui_matrix_t rotate_3D;
    int16_t release_x = tp_x;
    gui_obj_t *obj = (void *)block;
    float w = SCREEN_W;
    float h = SCREEN_H;
    float d = (w + h) / 2;
    float rotate_degree;
    float xoff;
    float yoff;
    float zoff;

    gui_vertex_t v0 = {-w, -h, d};
    gui_vertex_t v1 = {w,  -h, d};
    gui_vertex_t v2 = {w,  h,  d};
    gui_vertex_t v3 = {-w, h,  d};

    gui_vertex_t tv0, tv1, tv2, tv3;
    gui_vertex_t rv0, rv1, rv2, rv3;

    rotate_degree = 90 * release_x / (w) + 90.0 * (offset_x);

    matrix_compute_rotate(0, rotate_degree, 0, &rotate_3D);

    //matrix_multiply_normal(&rotate_3D, &this->normal);

    matrix_transfrom_rotate(&rotate_3D, &v0, &tv0, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v1, &tv1, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v2, &tv2, 0, 0, 0);
    matrix_transfrom_rotate(&rotate_3D, &v3, &tv3, 0, 0, 0);

    matrix_compute_rotate(0, 0, 0, &rotate_3D);
    xoff = (float)w / 2;
    yoff = (float)h / 2 ;
    zoff = -(xoff + yoff) * 2;

    matrix_transfrom_rotate(&rotate_3D, &tv0, &rv0, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv1, &rv1, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv2, &rv2, xoff, yoff, zoff);
    matrix_transfrom_rotate(&rotate_3D, &tv3, &rv3, xoff, yoff, zoff);

    gui_vertex_t p = {(float)(w) / 2, (float)(h) / 2, xoff + yoff};

    matrix_transfrom_blit(w,  h, &p, &rv0, &rv1, &rv2, &rv3,
                          &temp);

    if (rv0.x > rv1.x)
    {
        obj->not_show = true;
    }
    else
    {
        obj->not_show = false;
    }

    if (rotate_degree > 90)
    {
        matrix_translate((offset_x) * (int)w, \
                         (offset_y) * (int)h, \
                         obj->matrix);
    }

    if (rotate_degree < -90)
    {
        matrix_translate((offset_x) * (int)w, \
                         (offset_y) * (int)h, \
                         obj->matrix);
    }

    matrix_multiply(obj->matrix, &temp);
}
static void block_win_cb(struct app_block *config, gui_win_t *win)
{
    if (win == win_index)
    {
        block_transform_sync(win, offset_x_index,  offset_y_index,  tp_x_index,  tp_y_index);
    }
    else if (win == win_next)
    {
        block_transform_sync(win, offset_x,  offset_y,  tp_x,  tp_y);
    }


}
static bool next_curtain;
static bool win_curtain;
static bool curtain_left;
static bool curtain_right;
static bool curtain_up;
static bool curtain_down;
static bool touch_left;
static bool touch_right;
static bool touch_up;
static bool touch_down;
static void block_transform(gui_win_t *block, int offset_x_, int offset_y_, int tp_x_, int tp_y_)
{
    gui_log("next_curtain:%d,win_curtain:%d,%p,%p,%p\n", next_curtain, win_curtain, win_index, win_next,
            block);
    if (block == win_index)
    {
        if (!next_curtain)
        {
            offset_x_index = offset_x_;  offset_y_index = offset_y_;  tp_x_index = tp_x_;  tp_y_index = tp_y_;
        }
    }
    else if (block == win_next)
    {
        if (!win_curtain)
        {
            offset_x = offset_x_;  offset_y = offset_y_;  tp_x = tp_x_;  tp_y = tp_y_;
        }

    }


}
static gui_win_t *block_render(gui_obj_t *parent, int idx, int idy, bool next)
{
    win_next = 0;
    for (size_t i = 0; i < sizeof(block_array) / sizeof(struct app_block); i++)
    {
        if (block_array[i].x == idx && block_array[i].y == idy)
        {
            gui_win_t *win = gui_win_create(parent, 0, 0, 0, 0, 0);
            gui_win_set_animate(win, 1000, -1, (gui_animate_callback_t)block_win_cb, &block_array[i]);
            ((void (*)(gui_obj_t *parent))(block_array[i].render))((gui_obj_t *)win);
            if (next)
            {
                next_curtain = block_array[i].curtain;
            }
            else
            {
                win_curtain = block_array[i].curtain;
            }

            return win;
        }
    }
    return 0;
}

static void block_cb(gui_win_t *win)
{
    GUI_TOUCHPAD_IMPORT_AS_TP
    static bool hold;
    static int horizontal = -1;
    static bool load;
    static int idx, idy;
    static bool jump;
    static bool release_animation;
    static int direction;
    int deltaX = tp->deltaX;
    int deltaY = tp->deltaY;
    static int offset;
    static bool release_animation_end;
    if (tp->x + tp->deltaX > SCREEN_W)
    {
        deltaX = SCREEN_W - tp->x;
    }
    else if (tp->x + tp->deltaX < 0)
    {
        deltaX = 0 - tp->x;
    }
    if (tp->y + tp->deltaY > SCREEN_H)
    {
        deltaY = SCREEN_H - tp->y;
    }
    else if (tp->y + tp->deltaY < 0)
    {
        deltaY = 0 - tp->y;
    }


    if (tp->pressed)
    {
        hold = 1;
        horizontal = -1;


        if (release_animation)
        {
            if (jump)
            {
                if (!next_curtain)
                {
                    gui_obj_tree_free(win_index);
                }


                block_transform(win_next, 0, 0, 0, 0);
                release_animation_end = 0;
                if (direction == 1)
                {
                    idx--;
                }
                else if (direction == 2)
                {
                    idx++;
                }
                else if (direction == 3)
                {
                    idy--;
                }
                else if (direction == 4)
                {
                    idy++;
                }
            }
            else
            {
                gui_obj_tree_free(win_next);
                block_transform(win_index, 0, 0, 0, 0);
                release_animation_end = 0;
            }
        }

        release_animation = 0;
        load = 0;
        if (jump)
        {
            gui_log("jump\n");
            touch_left = 0;
            touch_right = 0;
            touch_up = 0;
            touch_down = 0;
            gui_win_t *temp = win_index;
            win_index = win_next;

            if (!next_curtain)
            {
                win_next = 0;
            }
            else
            {
                win_next = temp;
                if (curtain_down)
                {
                    touch_left = 1;
                    touch_right = 1;
                    touch_down = 1;
                    curtain_down = 0;
                }
                if (curtain_left)
                {
                    touch_down = 1;
                    touch_left = 1;
                    touch_up = 1;
                    curtain_left = 0;
                }
                if (curtain_right)
                {
                    touch_right = 1;
                    touch_down = 1;
                    touch_up = 1;
                    curtain_right = 0;
                }
                if (curtain_up)
                {
                    touch_left = 1;
                    touch_right = 1;
                    touch_up = 1;
                    curtain_up = 0;
                }

            }

            if (win_curtain)
            {
                win_curtain = 0;
            }
            win_curtain = next_curtain;


            next_curtain = 0;
            jump = 0;

            horizontal = -1;
            block_transform(win_index, 0, 0, 0, 0);
        }

    }
    else if (tp->released)
    {
        hold = 0;
        if (load)
        {
            release_animation = 1;
        }

        offset = deltaX;
    }

    if (hold)
    {
        if (tp->type == TOUCH_HOLD_X)
        {
            horizontal = 1;
        }
        else if (tp->type == TOUCH_HOLD_Y)
        {
            horizontal = 0;
        }
        if (horizontal == 1)
        {
            if (tp->deltaX > 0 && !touch_right) //->
            {
                if (!load)
                {
                    if (!win_curtain)
                    {
                        win_next = block_render((gui_obj_t *)win, idx - 1, idy, 1);

                    }
                    if (win_next)
                    {
                        load = 1;
                        if (next_curtain)
                        {
                            curtain_right = 1;
                        }


                    }

                }
                if (load)
                {
                    gui_log("block_transform\n");
                    block_transform(win_next, -1, 0, deltaX, 0);
                    block_transform(win_index, 0, 0, deltaX, 0);
                    if (deltaX > SCREEN_W / 3)
                    {
                        jump = 1;

                    }
                    else
                    {
                        jump = 0;
                    }


                }


            }
            else if (tp->deltaX < 0 && !touch_left) //<-
            {
                if (!load)
                {
                    if (!win_curtain)
                    {
                        win_next = block_render((gui_obj_t *)win, idx + 1, idy, 1);

                    }
                    if (win_next)
                    {
                        load = 1;
                        if (next_curtain)
                        {
                            curtain_left = 1;
                        }
                    }
                }
                if (load)
                {
                    block_transform(win_next, 1, 0, deltaX, 0);
                    block_transform(win_index, 0, 0, deltaX, 0);
                    if (_UI_ABS(deltaX) > SCREEN_W / 3)
                    {
                        jump = 1;
                    }
                    else
                    {
                        jump = 0;
                    }


                }
            }


        }
        else if (horizontal == 0)
        {
            if (tp->deltaY > 0 && !touch_down) //v
            {
                if (!load)
                {
                    if (!win_curtain)
                    {
                        win_next = block_render((gui_obj_t *)win, idx, idy - 1, 1);
                        gui_log("block_render :%d\n", idx);

                    }
                    if (win_next)
                    {
                        load = 1;
                        if (next_curtain)
                        {
                            curtain_down = 1;
                        }
                    }
                }
                if (load)
                {
                    gui_log("block_transform :%d\n", deltaX);
                    block_transform(win_next, 0, -1, 0, deltaY);
                    block_transform(win_index, 0, 0, 0, deltaY);
                    if (deltaY > SCREEN_H / 3)
                    {
                        jump = 1;

                    }
                    else
                    {
                        jump = 0;
                    }


                }


            }
            else if (tp->deltaY < 0 && !touch_up) //^
            {
                if (!load)
                {
                    if (!win_curtain)
                    {
                        win_next = block_render((gui_obj_t *)win, idx, idy + 1, 1);
                        gui_log("block_render :%d\n", idx);

                    }
                    if (win_next)
                    {
                        load = 1;
                        if (next_curtain)
                        {
                            curtain_up = 1;
                        }
                    }
                }
                if (load)
                {
                    //gui_log("block_transform :%d\n",deltaX );
                    block_transform(win_next, 0, 1, 0, deltaY);
                    block_transform(win_index, 0, 0, 0, deltaY);
                    if (_UI_ABS(deltaY) > SCREEN_H / 3)
                    {
                        jump = 1;
                    }
                    else
                    {
                        jump = 0;
                    }


                }
            }
        }


    }
    else if (release_animation)
    {

        static float count = 3;
        if (horizontal == 1)
        {
            if (jump)
            {
                if (deltaX > 0)
                {
#define STEP 8
                    offset = sinf(count * M_PI / 2 / (SCREEN_W - deltaX)) * (SCREEN_W - deltaX) * 2 + deltaX;
                    gui_log("%f\n", sinf(count * M_PI / 2 / (SCREEN_W - deltaX)));
                    if (offset > SCREEN_W)
                    {
                        offset = SCREEN_W;
                        release_animation_end = 1;
                        count = 3;
                        idx--;
                    }
                    count += STEP;
                    block_transform(win_next, -1, 0, offset, 0);
                    block_transform(win_index, 0, 0, offset, 0);
                    direction = 1;
                }
                else if (deltaX < 0)
                {
                    offset = -sinf(count * M_PI / 2 / (SCREEN_W - _UI_ABS(deltaX))) * (SCREEN_W - _UI_ABS(
                                                                                           deltaX)) * 2 + deltaX;
                    gui_log("%f\n", sinf(count * M_PI / 2 / (SCREEN_W - deltaX)));
                    if (_UI_ABS(offset) > SCREEN_W)
                    {
                        offset = -SCREEN_W;
                        release_animation_end = 1;
                        count = 3;
                        idx++;
                    }
                    count += STEP;
                    block_transform(win_next, 1, 0, offset, 0);
                    block_transform(win_index, 0, 0, offset, 0);
                    direction = 2;
                }


            }
            else
            {
                if (deltaX > 0)
                {

                    offset = deltaX - sinf(count * M_PI / 2 / (deltaX)) * (deltaX) * 2;
                    gui_log("%f\n", sinf(count * M_PI / 2 / (SCREEN_W - deltaX)));
                    if (offset < 0)
                    {
                        offset = 0;
                        release_animation_end = 1;
                        count = 3;
                    }
                    count += STEP;
                    block_transform(win_next, -1, 0, offset, 0);
                    block_transform(win_index, 0, 0, offset, 0);
                }
                else if (deltaX < 0)
                {

                    offset = deltaX + sinf(count * M_PI / 2 / (_UI_ABS(deltaX))) * (_UI_ABS(deltaX)) * 2;
                    gui_log("%f\n", sinf(count * M_PI / 2 / (SCREEN_W - deltaX)));
                    if (offset > 0)
                    {
                        offset = 0;
                        release_animation_end = 1;
                        count = 3;
                    }
                    count += STEP;
                    block_transform(win_next, -1, 0, offset, 0);
                    block_transform(win_index, 0, 0, offset, 0);
                }
            }


        }
        else if (horizontal == 0)
        {
            if (jump)
            {
                if (deltaY > 0)
                {

                    offset = sinf(count * M_PI / 2 / (SCREEN_H - deltaY)) * (SCREEN_H - deltaY) * 2 + deltaY;
                    gui_log("%f\n", sinf(count * M_PI / 2 / (SCREEN_W - deltaX)));
                    if (offset > SCREEN_H)
                    {
                        offset = SCREEN_H;
                        release_animation_end = 1;
                        count = 3;
                        idy--;
                    }
                    count += STEP;
                    block_transform(win_next, 0, -1, 0, offset);
                    block_transform(win_index, 0, 0, 0, offset);
                    direction = 3;
                }
                else if (deltaY < 0)
                {
                    offset = -sinf(count * M_PI / 2 / (SCREEN_H - _UI_ABS(deltaY))) * (SCREEN_H - _UI_ABS(
                                                                                           deltaY)) * 2 + deltaY;
                    gui_log("%f\n", sinf(count * M_PI / 2 / (SCREEN_H - deltaY)));
                    if (_UI_ABS(offset) > SCREEN_H)
                    {
                        offset = -SCREEN_H;
                        release_animation_end = 1;
                        count = 3;
                        idy++;
                    }
                    count += STEP;
                    block_transform(win_next,  0, 1, 0, offset);
                    block_transform(win_index, 0, 0, 0, offset);
                    direction = 4;
                }


            }
            else
            {
                if (deltaY > 0)
                {

                    offset = deltaY - sinf(count * M_PI / 2 / (deltaY)) * (deltaY) * 2;
                    gui_log("%f\n", sinf(count * M_PI / 2 / (SCREEN_W - deltaX)));
                    if (offset < 0)
                    {
                        offset = 0;
                        release_animation_end = 1;
                        count = 3;
                    }
                    count += STEP;
                    block_transform(win_next, 0, -1, 0, offset);
                    block_transform(win_index, 0, 0, 0, offset);
                }
                else if (deltaY < 0)
                {

                    offset = deltaY + sinf(count * M_PI / 2 / (_UI_ABS(deltaY))) * (_UI_ABS(deltaY)) * 2;
                    gui_log("%f\n", sinf(count * M_PI / 2 / (SCREEN_W - deltaY)));
                    if (offset > 0)
                    {
                        offset = 0;
                        release_animation_end = 1;
                        count = 3;
                    }
                    count += STEP;
                    block_transform(win_next,  0, -1, 0, offset);
                    block_transform(win_index, 0, 0, 0, offset);
                }
            }


        }

        if (release_animation_end)
        {
            if (jump)
            {
                if (!next_curtain)
                {
                    gui_obj_tree_free(win_index);
                }
                block_transform(win_next, 0, 0, 0, 0);
                release_animation = 0;
                release_animation_end = 0;
            }
            else
            {
                gui_obj_tree_free(win_next);
                block_transform(win_index, 0, 0, 0, 0);
                release_animation = 0;
                release_animation_end = 0;
            }


        }



    }

}

GUI_APP_ENTRY(APP_BLOCK)
{
    gui_win_t *win = gui_win_create(GUI_APP_ROOT_SCREEN, 0, 0, 0, 0, 0);
    gui_win_set_animate(win, 1000, -1, (gui_animate_callback_t)block_cb, win);
    win_index = block_render((gui_obj_t *)win, 0, 0, 0);
}
/**
 * @brief APP_FRUIT_NINJA
 * Please code here @shel_deng@realsil.com.cn
 */

GUI_APP_ENTRY(APP_FRUIT_NINJA)
{
    extern void app_fruit_ninja_design(gui_obj_t *obj);
    app_fruit_ninja_design(GUI_APP_ROOT_SCREEN);

    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)0);
}

/**
 * @brief APP_CALCULATOR
 *
 */
#include "app_calculator.h"

GUI_APP_ENTRY(APP_CALCULATOR)
{
    gui_app_t *calculator = get_app_calculator();
    calculator->ui_design((gui_app_t *)GUI_APP_ROOT_SCREEN);

    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)0);
}

/**
 * @brief APP_SOCCER
 *
 */
uint32_t *gui_soccer_array[] =
{
    SOCCER_P0001_CALL_BIN,
    SOCCER_P0002_SPORTS_BIN,
    SOCCER_P0003_HEARTRATE_BIN,
    SOCCER_P0004_SLEEP_BIN,
    SOCCER_P0005_WEATHER_BIN,
    SOCCER_P0006_ACTIVITIES_BIN,
    SOCCER_P0007_STRESS_BIN,
    SOCCER_P0008_SPO2_BIN,
    SOCCER_P0009_MUSIC_BIN,
    SOCCER_P0010_VOICE_BIN,
    SOCCER_P0011_NOTIFICATION_BIN,
    SOCCER_P0012_SETTINGS_BIN,
    SOCCER_P0013_SPORT_CORECD_BIN,
    SOCCER_P0014_MEASURE_BIN,
    SOCCER_P0015_MOOD_BIN,
    SOCCER_P0016_BREATHE_BIN,
    SOCCER_P0017_ALARM_BIN,
    SOCCER_P0018_PERIOD_BIN,
    SOCCER_P0019_HOME_BIN,
    SOCCER_P0020_MORE_BIN,
};
static void app_call_ui_design(gui_app_t *app);
static gui_app_t app_call =
{
    .screen =
    {
        .name = "app_call",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_call_ui_design,
    .active_ms = 1000 * 5,
};

gui_app_t *get_call_app(void)
{
    return &app_call;
}
static void app_call_ui_design(gui_app_t *app)
{
    gui_img_create_from_mem(&(app->screen), "call", SOCCER_P0001_CALL_BIN, 100, 100, 100, 100);
}
static void app_soccer_cb(void *obj, gui_event_t e, void *param)
{
    gui_soccer_t *soccer = (gui_soccer_t *)obj;
    int index = soccer->press_face;
    switch (soccer->press_face)
    {
    case 0:
        gui_switch_app(gui_current_app(), get_call_app());
        break;
    default:
        break;
    }
}
GUI_APP_ENTRY(APP_SOCCER)
{
    gui_soccer_t *soccer = gui_soccer_create(&(app->screen), "soccer", gui_soccer_array, 0, 0);
    gui_soccer_set_center(soccer, 227, 227);
    gui_soccer_on_click(soccer, app_soccer_cb, NULL);

    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)0);
}

GUI_APP_ENTRY(APP_BOX2D_RING)
{
#ifdef ENABLE_RTK_GUI_BOX2D
    extern void app_box2d_ring_ui_design(gui_obj_t *obj);
    app_box2d_ring_ui_design(GUI_APP_ROOT_SCREEN);
#else
    {
        char *text = "Please uncomment CONFIG_REALTEK_BUILD_GUI_BOX2D";
        int font_size = 16;
        gui_text_t *t = gui_text_create(GUI_APP_ROOT_SCREEN, 0,  0, 200,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
    {
        char *text = "in win32_sim/menu_config.h";
        int font_size = 16;
        gui_text_t *t = gui_text_create(GUI_APP_ROOT_SCREEN, 0,  0, 220,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
#endif
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)0);
}
GUI_APP_ENTRY(APP_MUSIC)
{
    extern void app_music_ui_design(gui_obj_t *obj);
    app_music_ui_design(GUI_APP_ROOT_SCREEN);
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)0);
    gui_fps_create(GUI_APP_ROOT_SCREEN);
}
GUI_APP_ENTRY(APP_CALENDAR)
{
    extern void app_calender_ui_design(gui_obj_t *obj);
    app_calender_ui_design(GUI_APP_ROOT_SCREEN);
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)0);
    gui_fps_create(GUI_APP_ROOT_SCREEN);
}
