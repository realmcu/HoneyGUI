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
GUI_APP_DEFINE(APP_HEART_RATE, app_hr_ui_design)
GUI_APP_DEFINE(APP_CLOCK,      app_hr_ui_design)
GUI_APP_DEFINE(APP_WATCH_FACE, app_hr_ui_design)
GUI_APP_DEFINE(APP_CALCULATOR, app_hr_ui_design)
GUI_APP_DEFINE(APP_SPORT,      app_hr_ui_design)
#define SCREEN_W ((int)gui_get_screen_width())
#define SCREEN_H ((int)gui_get_screen_height())
/**
 * @link https://www.rapidtables.com/web/color/RGB_Color.html
*/
#define COLOR_RED gui_rgb(0, 0, 255)
#define COLOR_CRIMSON gui_rgb(60,20,220)
#define COLOR_FIREBRICK gui_rgb(34,34,178)
#define COLOR_WHITE gui_rgb(255,255,255)
#define HEART_ANI_NAME "_HEART_ANI"
#define HEART_ANI_W 180
static void heart_ani_cb(gui_img_t *img);
static void page_cb(gui_page_t *page);
static void win_cb(gui_win_t *win);
static void app_hr_ui_design(gui_app_t *app)
{
    gui_page_t *page = gui_page_create(GUI_APP_ROOT_SCREEN, 0, 0, 0, 0, 0);
    gui_page_set_animate(page, 1000, -1, page_cb, page);
    gui_page_center_alignment(page, SCREEN_H);
    gui_rect(page, 0, SCREEN_H, SCREEN_W, SCREEN_H, COLOR_RED);
    gui_rect(page, 0, SCREEN_H * 2, SCREEN_W, SCREEN_H, COLOR_CRIMSON);
    gui_rect(page, 0, SCREEN_H * 3, SCREEN_W, SCREEN_H, COLOR_FIREBRICK);
    gui_img_t *heart_ani = gui_img_create_from_mem(GUI_APP_ROOT_SCREEN, HEART_ANI_NAME, HEARTRATE04_BIN,
                                                   (SCREEN_W - HEART_ANI_W) / 2, 100, 0, 0);
    gui_img_set_animate(heart_ani, 1000, -1, heart_ani_cb, heart_ani);
    gui_img_set_mode(heart_ani, IMG_BYPASS_MODE);
    {
        char *text = "Current";
        int font_size = 16;
        gui_text_t *t = gui_text_create(page, "txt", 50, 300, gui_get_screen_width(), font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_WHITE, strlen(text), font_size);
        void *addr1 = ARIALBD_SIZE16_BITS4_FONT_BIN;
        gui_font_mem_init(addr1);
        gui_text_type_set(t, addr1);
    }
    gui_win_t *win = gui_win_create(GUI_APP_ROOT_SCREEN, 0, 0, 0, SCREEN_W, SCREEN_H);
    gui_win_onRight(win, win_cb, win);
}
static uint32_t *heart_ani_array[] =
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
static void heart_ani_cb(gui_img_t *img)
{
    static bool odd;
    if (img->animate->progress_percent == 0)
    {
        odd = !odd;
    }
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
    GUI_APP_SWAP_HANDLE(GUI_APP_HANDLE(APP_HEART_RATE), get_app_watch_ui())
    gui_tabview_t *tabview = 0;
    gui_tree_get_widget_by_name(&(get_app_watch_ui()->screen), "tabview", &tabview);
    if (tabview)
    {
        gui_tabview_jump_tab(tabview, -1, 0);
    }


}

