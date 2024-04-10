#include "../root_image/ui_resource.h"
#include <gui_app.h>
#include "gui_page.h"
#include "gui_img.h"
#include "gui_rect.h"
#include "gui_obj.h"
#include "gui_text.h"
#include "gui_win.h"
#include "gui_tabview.h"
#define APP_HEART_RATE
#define APP_CLOCK
#define APP_WATCH_FACE
#define APP_CALCULATOR
#define APP_SPORT
#define APP_MENU
GUI_APP_DEFINE(APP_HEART_RATE, app_hr_ui_design)
GUI_APP_DEFINE(APP_CLOCK,      app_hr_ui_design)
GUI_APP_DEFINE(APP_WATCH_FACE, app_hr_ui_design)
GUI_APP_DEFINE(APP_CALCULATOR, app_hr_ui_design)
GUI_APP_DEFINE(APP_SPORT,      app_hr_ui_design)
GUI_APP_DEFINE(APP_MENU,       app_menu)
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
static void heart_ani_cb(gui_img_t *img);
static void page_cb(gui_page_t *page);
static void win_cb(gui_win_t *win);
static void status_bar(void *parent, gui_obj_t *ignore_gesture);
static status_bar_ani(gui_obj_t *ignore_gesture);
static void app_hr_ui_design(gui_app_t *app)
{
    gui_page_t *page = gui_page_create(GUI_APP_ROOT_SCREEN, PAGE_NAME, 0, 0, 0, 0);
    gui_page_set_animate(page, 1000, -1, page_cb, page);
    gui_page_center_alignment(page, SCREEN_H);

    {
        gui_img_t *rect = gui_rect(page, 0, SCREEN_H, SCREEN_W, SCREEN_H, COLOR_RED);
        gui_rect(rect, 10, 100, SCREEN_W - 10 * 2, 2, COLOR_SILVER_OPACITY(100));
        gui_rect(rect, 10, 200, SCREEN_W - 10 * 2, 2, COLOR_SILVER_OPACITY(100));
        gui_rect(rect, 10, 300, SCREEN_W - 10 * 2, 2, COLOR_SILVER_OPACITY(100));
        gui_rect(rect, 10, 400, SCREEN_W - 10 * 2, 2, COLOR_SILVER_OPACITY(100));
        gui_rect(rect, 10, 100, 2, 400 - 100, COLOR_SILVER_OPACITY(100));
        gui_rect(rect, 10 + (SCREEN_W - 10 * 2) / 4, 100, 2, 400 - 100, COLOR_SILVER_OPACITY(100));
        gui_rect(rect, 10 + (SCREEN_W - 10 * 2) / 4 * 2, 100, 2, 400 - 100, COLOR_SILVER_OPACITY(100));
        gui_rect(rect, 10 + (SCREEN_W - 10 * 2) / 4 * 3, 100, 2, 400 - 100, COLOR_SILVER_OPACITY(100));
        gui_rect(rect, 10 + (SCREEN_W - 10 * 2) / 4 * 4, 100, 2, 400 - 100, COLOR_SILVER_OPACITY(100));
        {
            char *text = "198";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4 * 4 - 30, 100,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_font_mem_init(addr1);
            gui_text_type_set(t, addr1);
        }
        {
            char *text = "132";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4 * 4 - 30, 200,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_font_mem_init(addr1);
            gui_text_type_set(t, addr1);
        }
        {
            char *text = "66";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4 * 4 - 30, 300,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_font_mem_init(addr1);
            gui_text_type_set(t, addr1);
        }
        {
            char *text = "12AM";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10, 400 - 18, gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_font_mem_init(addr1);
            gui_text_type_set(t, addr1);
        }
        {
            char *text = " 6AM";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4, 400 - 18,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_font_mem_init(addr1);
            gui_text_type_set(t, addr1);
        }
        {
            char *text = "12PM";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4 * 2, 400 - 18,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_font_mem_init(addr1);
            gui_text_type_set(t, addr1);
        }
        {
            char *text = " 6PM";
            int font_size = 16;
            gui_text_t *t = gui_text_create(rect, "txt", 10 + (SCREEN_W - 10 * 2) / 4 * 3, 400 - 18,
                                            gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER_OPACITY(100), strlen(text), font_size);
            void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
            gui_font_mem_init(addr1);
            gui_text_type_set(t, addr1);
        }
        gui_rect(rect, 10 + 10, 300, 5, 400 - 300, COLOR_SILVER);
        gui_rect(rect, 10 + 10 + 100, 160, 5, 400 - 160, COLOR_SILVER);
        gui_rect(rect, 10 + 10 + 200, 280, 5, 400 - 280, COLOR_SILVER);
        gui_rect(rect, 10 + 10 + 280, 275, 5, 400 - 275, COLOR_SILVER);
    }
    {
        gui_img_t *rect = gui_rect(page, 0, SCREEN_H * 2, SCREEN_W, SCREEN_H, COLOR_CRIMSON);
        gui_rect(rect, 10, 100, SCREEN_W - 10 * 2, 1, COLOR_SILVER);

    }
    gui_rect(page, 0, SCREEN_H * 3, SCREEN_W, SCREEN_H, COLOR_FIREBRICK);
    gui_img_t *heart_ani = gui_img_create_from_mem(GUI_APP_ROOT_SCREEN, HEART_ANI_NAME, HEARTRATE04_BIN,
                                                   (SCREEN_W - HEART_ANI_W) / 2, 100, 0, 0);
    gui_img_set_animate(heart_ani, 1000, -1, heart_ani_cb, heart_ani);
    gui_img_set_mode(heart_ani, IMG_BYPASS_MODE);
    {
        char *text = "Current";
        int font_size = 16;
        gui_text_t *t = gui_text_create(page, "txt", 100, 300, gui_get_screen_width(), font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1);
    }
    {
        char *text = "68";
        int font_size = 48;
        gui_text_t *t = gui_text_create(page, "txt", 100, 320, gui_get_screen_width(), font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIAL_SIZE48_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1);
    }
    {
        char *text = "BPM";
        int font_size = 16;
        gui_text_t *t = gui_text_create(page, "txt", 157, 347, gui_get_screen_width(), font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_RED, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1);
    }
    {
        char *text = "75 BPM, 2m ago";
        int font_size = 16;
        gui_text_t *t = gui_text_create(page, "txt", 100, 370, gui_get_screen_width(), font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1);
    }
    gui_win_t *win = gui_win_create(GUI_APP_ROOT_SCREEN, 0, 0, 0, SCREEN_W, SCREEN_H);
    gui_win_onRight(win, win_cb, win);
    status_bar(GUI_APP_ROOT_SCREEN, page);
}
static void heart_ani_cb(gui_img_t *img)
{
    static bool odd;
    if (img->animate->progress_percent == 0)
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
    if (odd)
    {
        gui_img_set_attribute(img, 0, heart_ani_array[(int)((1.0f - img->animate->progress_percent) *
                                                                        (float)(sizeof(heart_ani_array) / sizeof(uint32_t *) - 2)) + 1], GET_BASE(img)->x,
                              GET_BASE(img)->y);
    }
    else
    {
        gui_img_set_attribute(img, 0, heart_ani_array[(int)(img->animate->progress_percent * (float)(sizeof(
                                                                                                     heart_ani_array) / sizeof(uint32_t *) - 1))], GET_BASE(img)->x, GET_BASE(img)->y);
    }
}
static void page_cb(gui_page_t *page)
{
    gui_img_t *img = 0;
    gui_app_t *app = GUI_APP_HANDLE(APP_HEART_RATE);
    gui_tree_get_widget_by_name(GUI_APP_ROOT_SCREEN, HEART_ANI_NAME, &img);

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
    gui_tree_get_widget_by_name(&(get_app_watch_ui()->screen), "tabview", &tabview);
    if (tabview)
    {
        gui_tabview_jump_tab(tabview, -1, 0);
    }


}

#define TIME_SCALE_RATE (0.35F)
#define STATUS_BAR_HEIGHT 20
static gui_win_t *canvas_win;
static gui_img_t *rect;
static gui_text_t *t;
static void status_bar(void *parent, gui_obj_t *ignore_gesture)
{
    gui_win_t *status_bar = gui_win_create(parent, 0, 0, 0, SCREEN_W, SCREEN_H);
    canvas_win = gui_win_create(status_bar, 0, 0, 0, SCREEN_W, SCREEN_H);
    gui_win_set_animate(status_bar, 1000, -1, status_bar_ani, ignore_gesture);
    {
        char *text = "7:55";
        int font_size = 48;
        t = gui_text_create(status_bar, "txt", SCREEN_W / 2 - 40, 2, gui_get_screen_width(),
                            font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIAL_SIZE48_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1);
        gui_text_convert_to_img(t, RGBA8888);

        gui_img_scale(t->scale_img, TIME_SCALE_RATE, TIME_SCALE_RATE);
        gui_img_rotation(t->scale_img, 0, gui_img_get_width(t->scale_img) / 2,
                         0);
        gui_img_translate(t->scale_img, gui_img_get_width(t->scale_img) / 2,
                          0);
    }

    GET_BASE(canvas_win)->not_show = 1;
    rect = gui_rect(canvas_win, 0, 0, SCREEN_W, SCREEN_H, COLOR_WHITE_OPACITY);
    gui_img_set_opacity(rect, 0);

}
static status_bar_ani(gui_obj_t *ignore_gesture)
{

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
        GET_BASE(canvas_win)->not_show = 0;
        if (deltaY > 0)
        {
            if (deltaY > 100)
            {
                deltaY = 100;
            }

            int opacity = deltaY * 255 / 100;
            if (opacity > 255)
            {
                opacity = 255;
            }
            gui_img_set_opacity(rect, opacity);
            float scale = deltaY * deltaY * ((1.0f - TIME_SCALE_RATE) / 10000.0f) + TIME_SCALE_RATE;
            if (scale > 1)
            {
                scale = 1;
            }
            gui_img_scale(t->scale_img, scale, scale);
        }
        if (tp->released)
        {
            if (deltaY >= 100)
            {
                expand = 1;
                gui_img_set_opacity(rect, 255);
            }
            else
            {
                shrink = 1;
            }
            press = 0;
        }

    }
    if (shrink)
    {
        gui_img_set_opacity(rect, 0);
        GET_BASE(canvas_win)->not_show = 1;
        gui_img_scale(t->scale_img, TIME_SCALE_RATE, TIME_SCALE_RATE);
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
                gui_img_set_opacity(rect, opacity);
                float scale = (100 - (-deltaY)) * ((1.0f - TIME_SCALE_RATE) / 100.0f) + TIME_SCALE_RATE;
                gui_img_scale(t->scale_img, scale, scale);

            }



            if (tp->released)
            {
                if (deltaY < 0)
                {
                    expand = 0;
                    gui_img_set_opacity(rect, 0);
                    GET_BASE(canvas_win)->not_show = 1;
                    gui_img_scale(t->scale_img, TIME_SCALE_RATE, TIME_SCALE_RATE);
                    if (ignore_gesture)
                    {
                        ignore_gesture->gesture = 0;
                    }
                }
                expand_press = 0;
            }

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
        ICON_BLUETOOTH_OFF_BIN,
        ICON_BLUETOOTH_ON_BIN,
        ICON_BLUETOOTH_TOUCH_BIN,
        ICON_BUDS_OFF_BIN,
        ICON_BUDS_ON_BIN,
        ICON_BUDS_TOUCH_BIN,
        ICON_MORE_OFF_BIN,
        ICON_MORE_ON_BIN,
        ICON_MORE_TOUCH_BIN,
        ICON_PHONE_OFF_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_TOUCH_BIN,
        ICON_BLUETOOTH_OFF_BIN,
        ICON_BLUETOOTH_ON_BIN,
        ICON_BLUETOOTH_TOUCH_BIN,
        ICON_BUDS_OFF_BIN,
        ICON_BUDS_ON_BIN,
        ICON_BUDS_TOUCH_BIN,
        ICON_MORE_OFF_BIN,
        ICON_MORE_ON_BIN,
        ICON_MORE_TOUCH_BIN,
        ICON_PHONE_OFF_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_TOUCH_BIN,
        ICON_BLUETOOTH_OFF_BIN,
        ICON_BLUETOOTH_ON_BIN,
        ICON_BLUETOOTH_TOUCH_BIN,
        ICON_BUDS_OFF_BIN,
        ICON_BUDS_ON_BIN,
        ICON_BUDS_TOUCH_BIN,
        ICON_MORE_OFF_BIN,
        ICON_MORE_ON_BIN,
        ICON_MORE_TOUCH_BIN,
        ICON_PHONE_OFF_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_TOUCH_BIN,
        ICON_BLUETOOTH_OFF_BIN,
        ICON_BLUETOOTH_ON_BIN,
        ICON_BLUETOOTH_TOUCH_BIN,
        ICON_BUDS_OFF_BIN,
        ICON_BUDS_ON_BIN,
        ICON_BUDS_TOUCH_BIN,
        ICON_MORE_OFF_BIN,
        ICON_MORE_ON_BIN,
        ICON_MORE_TOUCH_BIN,
        ICON_PHONE_OFF_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_TOUCH_BIN,
    };
    gui_menu_cellular_t *cell = gui_menu_cellular_create(GUI_APP_ROOT_SCREEN, 100, array,
                                                         sizeof(array) / sizeof(uint32_t *));
    gui_menu_cellular_offset(cell, -36, -216);
    gui_win_t *win = gui_win_create(GUI_APP_ROOT_SCREEN, 0, 0, 0, SCREEN_W, SCREEN_H);
    gui_win_onRight(win, win_cb, win);
    status_bar(GUI_APP_ROOT_SCREEN, cell);
}























