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
#include<stdio.h>
#include<time.h>
#define APP_HEART_RATE
#define APP_CLOCK
#define APP_WATCH_FACE
#define APP_CALCULATOR
#define APP_SPORT
#define APP_MENU
#define APP_STOPWATCH
#define APP_MAP
#define APP_CARDVIEW
#define APP_BOX2D
GUI_APP_DEFINE(APP_HEART_RATE, app_hr_ui_design) // cppcheck-suppress syntaxError
GUI_APP_DEFINE(APP_CLOCK,      app_hr_ui_design)
GUI_APP_DEFINE(APP_WATCH_FACE, app_hr_ui_design)
GUI_APP_DEFINE(APP_CALCULATOR, app_hr_ui_design)
GUI_APP_DEFINE(APP_SPORT,      app_hr_ui_design)
GUI_APP_DEFINE_NAME(APP_STOPWATCH)
GUI_APP_DEFINE(APP_MENU,       app_menu)
GUI_APP_DEFINE_NAME(APP_MAP)
GUI_APP_DEFINE_NAME(APP_CARDVIEW)

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
    gui_win_set_animate(app_win, 1000, 0, app_win_cb, app_win);
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
            gui_text_type_set(t, addr1);
        }
        {
            char *text = "132";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4 * 4 - 30, 200,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1);
        }
        {
            char *text = "66";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4 * 4 - 30, 300,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1);
        }
        {
            char *text = "12AM";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10, 400 - 18, gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1);
        }
        {
            char *text = " 6AM";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4, 400 - 18,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1);
        }
        {
            char *text = "12PM";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4 * 2, 400 - 18,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1);
        }
        {
            char *text = " 6PM";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4 * 3, 400 - 18,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1);
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
    gui_win_set_animate(win, 1000, -1, heart_ani_cb, win);
    gui_img_set_mode(heart_ani, IMG_BYPASS_MODE);
    {
        char *text = "Current";
        int font_size = 16;
        gui_text_t *t = gui_text_create(page, "txt", 100, 300, gui_get_screen_width(), font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1);
    }
    {
        char *text = "68";
        int font_size = 48;
        gui_text_t *t = gui_text_create(page, "txt", 100, 320, gui_get_screen_width(), font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIAL_SIZE48_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1);
    }
    {
        char *text = "BPM";
        int font_size = 16;
        gui_text_t *t = gui_text_create(page, "txt", 157, 347, gui_get_screen_width(), font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_RED, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1);
    }
    {
        char *text = "75 BPM, 2m ago";
        int font_size = 16;
        gui_text_t *t = gui_text_create(page, "txt", 100, 370, gui_get_screen_width(), font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1);
    }
    status_bar(GUI_APP_ROOT_SCREEN, (void *)page);
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)page);
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
    GUI_APP_SWAP_HANDLE(gui_current_app(), get_app_watch_ui())
    gui_tabview_t *tabview = 0;
    gui_obj_tree_get_widget_by_name(&(get_app_watch_ui()->screen), "tabview", (void *)&tabview);
    if (tabview)
    {
        gui_tabview_jump_tab(tabview, -1, 0);
    }


}

#define TIME_SCALE_RATE (0.35F)
#define STATUS_BAR_HEIGHT 20
#define STATUS_BAR_TIME_TEXT "STATUS_BAR_TIME_TEXT"
#define STATUS_BAR_DATE_TEXT "STATUS_BAR_DATE_TEXT"
#define STATUS_BAR_WINDOW "STATUS_BAR_WINDOW"
static gui_canvas_rect_t *rect;
static void status_bar(void *parent, gui_obj_t *ignore_gesture)
{
    gui_win_t *status_bar = gui_win_create(parent, 0, 0, 0, SCREEN_W, SCREEN_H);
    rect = gui_canvas_rect_create((void *)status_bar, "canvas_rect", 0, 0, SCREEN_W, SCREEN_H,
                                  COLOR_SILVER_OPACITY(230));
    GET_BASE(rect)->not_show = 1;
    gui_canvas_rect_set_opacity(rect, 0);
    gui_win_set_animate(status_bar, 1000, -1, status_bar_ani, ignore_gesture);
    {
        char *text = "07:55";
        int font_size = 48;
        gui_text_t *t = gui_text_create(status_bar, STATUS_BAR_TIME_TEXT, SCREEN_W / 2 - 56, 2,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIAL_SIZE48_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1);
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
            gui_text_type_set(t, addr1);
            gui_text_mode_set(t, CENTER);

        }
        {
            char *text = "NO NOTIFICATIONS";
            int font_size = 16;
            gui_text_t *t = gui_text_create(win, 0, 0, 200, gui_get_screen_width(),
                                            font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_text_type_set(t, addr1);
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
            if (opacity > 255)
            {
                opacity = 255;
            }
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
        gui_img_scale(time_txt->scale_img, TIME_SCALE_RATE, TIME_SCALE_RATE);
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
#include "gui_menu_cellular.h"

static void app_menu(gui_app_t *app)
{
    /**
     * @link https://docs.realmcu.com/Honeygui/latest/widgets/gui_menu_cellular.html#example
    */
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
    gui_menu_cellular_t *cell = gui_menu_cellular_create(GUI_APP_ROOT_SCREEN, 100, array,
                                                         sizeof(array) / sizeof(uint32_t *));
    gui_menu_cellular_offset((void *)cell, -36, -216);
    status_bar(GUI_APP_ROOT_SCREEN, (void *)cell);
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)cell);
}
#include "gui_seekbar.h"
#include "gui_img.h"
#define RETURN_HEIGHT 100
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
static void stop_watch_win_ani_cb(void);
#define STOPWATCHTEXT "STOPWATCHTEXT"
GUI_APP_ENTRY(APP_STOPWATCH)
{
    gui_win_t *stop = gui_win_create(GUI_APP_ROOT_SCREEN, 0, 0, 0, SCREEN_W, SCREEN_H);
    gui_win_set_animate(stop, 1000, -1, stop_watch_win_ani_cb, 0);
    {
        char *text = "07:55";
        int font_size = 48;
        gui_text_t *t = gui_text_create(stop, STOPWATCHTEXT,  0, 200,
                                        gui_get_screen_width(),
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIAL_SIZE48_BITS4_FONT_BIN;
        gui_text_type_set(t, addr1);
        gui_text_convert_to_img(t, ARGB8888);
        gui_text_mode_set(t, CENTER);

    }
    status_bar(GUI_APP_ROOT_SCREEN, (void *)0);
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)0);
}
static void get_stopwatch_string(char *buffer)
{
#if _WIN32
    static bool enter;
    static struct timeval start;
    if (!enter)
    {
        enter = 1;
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
        gui_text_type_set(t, addr1);
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
        gui_text_type_set(t, addr1);
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
        gui_text_type_set(t, addr1);
        gui_text_mode_set(t, MULTI_LEFT);
    }
#endif
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)0);
}












