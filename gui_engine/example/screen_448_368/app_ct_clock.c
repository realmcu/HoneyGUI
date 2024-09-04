/* watchface_watch_gradient_spot demo start*/
#include "root_image_hongkong/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_watch_gradient_spot.h"
#include "gui_text.h"
#include "gui_watchface_gradient.h"
#include "gui_cardview.h"
#include "gui_card.h"
#include "gui_tab.h"
#include "gui_app.h"

// gui_win_t *win_watch;
// gui_img_t *img;
// gui_watch_gradient_spot_t *watch;
// gui_text_t *rate;
// static gui_watchface_gradient_t *canvas;
// gui_tabview_t *tablist_tab;
/* watchface_watch_gradient_spot demo end*/

// static void canvas_cb(gui_canvas_t *canvas)
// {
//     nvgRect(canvas->vg, 0, 0, 368, 448);
//     nvgFillColor(canvas->vg, nvgRGBA(30, 30, 30, 255));
//     nvgFill(canvas->vg);
// }

// static void callback_time()
// {
//     int idx = tablist_tab->cur_id.x;
//     gui_log("idx:%d\n", idx);
//     gui_app_t *app = (gui_app_t *)get_app_hongkong();
//     gui_obj_t *screen = &(app->screen);
//     gui_obj_tree_free(screen);
//     app->ui_design(app);
//     switch (idx)
//     {
//     case 0:
//         {
//             GET_BASE(img)->not_show = false;
//             GET_BASE(watch)->not_show = true;
//             GET_BASE(canvas)->not_show = true;
//         }
//         break;
//     case 1:
//         {
//             GET_BASE(img)->not_show = true;
//             GET_BASE(watch)->not_show = false;
//             GET_BASE(canvas)->not_show = true;
//         }
//         break;
//     case 2:
//         {
//             GET_BASE(img)->not_show = true;
//             GET_BASE(watch)->not_show = true;
//             GET_BASE(canvas)->not_show = false;
//         }
//         break;
//     default:
//         break;
//     }

// }

// /* callback_touch_long start*/
// static void callback_touch_long(void *obj, gui_event_t e)
// {
//     gui_log("win widget long touch enter cb\n");
//     gui_app_t *app = (gui_app_t *)get_app_hongkong();
//     gui_obj_t *screen = &(app->screen);
//     int idx = 0;
//     if (!GET_BASE(img)->not_show)
//     {
//         idx = 0;
//     }
//     if (!GET_BASE(watch)->not_show)
//     {
//         idx = 1;
//     }
//     if (!GET_BASE(canvas)->not_show)
//     {
//         idx = 2;
//     }

//     gui_obj_tree_free(screen);
//     gui_win_t *win = gui_win_create(screen, "win", 0, 0, 320, 320);
// #if ENABLE_RTK_GUI_WATCHFACE_UPDATE
//     extern void create_tree_nest(char *xml, void *obj);
//     create_tree_nest("gui_engine\\example\\screen_448_368\\root_image_hongkong\\watch_face_update\\app\\wf\\wf.xml",
//                      win);
//     return;
// #endif
//     gui_obj_add_event_cb(win, (gui_event_cb_t)callback_time, GUI_EVENT_TOUCH_CLICKED, NULL);

//     tablist_tab = gui_tabview_create(win, "tabview", 59, 84, 250, 300);
//     gui_tabview_set_style(tablist_tab, CLASSIC);
//     gui_tab_t *tb_watchface = gui_tab_create(tablist_tab, "tb_watchface",   0, 0, 250, 0, 2, 0);
//     gui_tab_t *tb_watch = gui_tab_create(tablist_tab, "tb_watch",           0, 0, 250, 0, 1, 0);

//     gui_tab_t *tb_clock = gui_tab_create(tablist_tab, "tb_clock",           0, 0, 250, 0, 0, 0);
//     gui_tabview_jump_tab(tablist_tab, idx, 0);
//     extern void tablist_clock(void *parent);
//     tablist_clock(tb_clock);
//     extern void tablist_watch(void *parent);
//     tablist_watch(tb_watch);
//     extern void tablist_watchface(void *parent);
//     tablist_watchface(tb_watchface);

// }
// /* callback_touch_long end*/

// static void callback_watchface()
// {
//     static char clock;
//     if (clock == 1)
//     {
//         GET_BASE(img)->not_show = false;
//         GET_BASE(watch)->not_show = true;
//         GET_BASE(canvas)->not_show = true;
//         clock = 2;
//     }
//     else if (clock == 2)
//     {
//         GET_BASE(img)->not_show = true;
//         GET_BASE(watch)->not_show = false;
//         GET_BASE(canvas)->not_show = true;
//         clock = 0;
//     }
//     else if (clock == 0)
//     {
//         GET_BASE(img)->not_show = true;
//         GET_BASE(watch)->not_show = true;
//         GET_BASE(canvas)->not_show = false;
//         clock = 1;
//     }
// }


/* page_ct_clock start*/
// void page_ct_clock(void *parent)
// {
//     win_watch = gui_win_create(parent, "win", 0, 0, 368, 448);

//     gui_obj_add_event_cb(win_watch, (gui_event_cb_t)callback_touch_long, GUI_EVENT_TOUCH_LONG, NULL);

//     img = gui_img_create_from_mem(parent, "page0", CLOCKN_BIN, 0, 0, 0, 0);
//     gui_img_set_mode(img, IMG_SRC_OVER_MODE);
//     watch = gui_watch_gradient_spot_create(win_watch, "watchface", 0, 0, 0, 0);
//     gui_watch_gradient_spot_set_center(watch, 368 / 2, 448 / 2);
//     canvas = gui_watchface_gradient_create(parent, "watchface_gradient", (368 - 368) / 2,
//                                            (448 - 448) / 2, 368, 448);
//     GET_BASE(watch)->not_show = true;
//     GET_BASE(canvas)->not_show = true;
// }
/* page_ct_clock end*/
#include "time.h"
#include "gui_curtainview.h"
#include "tp_algo.h"

#define SCREEN_WIDTH 368
#define SCREEN_HEIGHT 448

static void *font_size_96_bin_addr = SOURCEHANSANSSC_SIZE96_BITS4_FONT_BIN;
static void *font_size_64_bin_addr = SOURCEHANSANSSC_SIZE64_BITS4_FONT_BIN;
static void *font_size_48_bin_addr = SOURCEHANSANSSC_SIZE48_BITS4_FONT_BIN;
static void *font_size_40_bin_addr = SOURCEHANSANSSC_SIZE40_BITS4_FONT_BIN;
static void *font_size_32_bin_addr = SOURCEHANSANSSC_SIZE32_BITS4_FONT_BIN;
static void *font_size_24_bin_addr = SOURCEHANSANSSC_SIZE24_BITS4_FONT_BIN;

static gui_win_t *win_watch;
static gui_text_t *time_text, *date_text;
static char time_content[10] = "00:00", date_content[10] = "Thurs 15";
struct tm *timeinfo;

static char *tabview_array[] =
{
    "tv_message",
    "tv_os_information"
};

extern char *day[];
static void refreash_time()
{
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char temp_1[10];
    sprintf(temp_1, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    strcpy(time_content, temp_1);
    gui_text_content_set(time_text, time_content, strlen(time_content));

    char temp_2[10];
    sprintf(temp_2, "%s %d",  day[timeinfo->tm_wday], timeinfo->tm_mday);
    strcpy(date_content, temp_2);
    gui_text_content_set(date_text, date_content, strlen(date_content));
}

static void win_clock_cb(void)
{
    refreash_time();

    //  blur this curtain
    gui_curtainview_t *cv = 0;
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "ct_clock",
                                    (void *)&cv); //  can find "ct_clock" in "app_tb_clock.c"
    if (cv)
    {
        if (cv->cur_curtain == CURTAIN_UP || cv->cur_curtain == CURTAIN_DOWN)
        {
            GUI_BASE(win_watch)->opacity_value = 50;
        }
        else
        {
            GUI_BASE(win_watch)->opacity_value = 255;
        }
    }

    touch_info_t *tp = tp_get_info();
    static bool hold;
    if (tp->pressed)
    {
        hold = 1;
    }
    if (tp->released)
    {
        hold = 0;
    }
    if (hold)
    {
        if (tp->x > 50 && tp->x < 300 && tp->y > 220 && tp->y < 330)
        {
            gui_tabview_t *tv = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "clock_tv", (void *)&tv);
            if (tv)
            {
                GUI_BASE(tv)->gesture = 0;
            }
            gui_tabview_t *clock = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "hongkong_tabview",
                                            (void *)&clock); //  can find "hongkong_tabview" in "app_hongkong.c"
            if (clock)
            {
                GUI_BASE(clock)->gesture = 1;
            }
            // gui_curtainview_t *cv = 0;
            // gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "ct_clock",
            //                                 (void *)&cv); //  can find "ct_clock" in "app_tb_clock.c"
            if (cv)
            {
                GUI_BASE(cv)->gesture = 1;
            }
        }
        else
        {
            gui_tabview_t *tv = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "clock_tv", (void *)&tv);
            if (tv)
            {
                GUI_BASE(tv)->gesture = 1;
            }
            gui_tabview_t *clock = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "hongkong_tabview",
                                            (void *)&clock); //  can find "hongkong_tabview" in "app_hongkong.c"
            if (clock)
            {
                GUI_BASE(clock)->gesture = 0;
            }
            // gui_curtainview_t *cv = 0;
            // gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "ct_clock",
            //                                 (void *)&cv); //  can find "ct_clock" in "app_tb_clock.c"
            if (cv)
            {
                GUI_BASE(cv)->gesture = 0;
            }
        }
    }
}
void page_ct_clock(void *parent)
{
    win_watch = gui_win_create(parent, "win_clock", 0, 0, 368, 448);

    // date & time text
    date_text = gui_text_create(win_watch, "date_text",  -20, 15, 0, 0);
    gui_text_set(date_text, (void *)date_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(date_content),
                 40);
    gui_text_type_set(date_text, font_size_40_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(date_text, RIGHT);

    time_text = gui_text_create(win_watch, "time_text",  -20, 50, 0, 0);
    gui_text_set(time_text, (void *)time_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(time_content),
                 96);
    gui_text_type_set(time_text, font_size_96_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(time_text, RIGHT);

    // card
    gui_tabview_t *tv = gui_tabview_create(win_watch, "clock_tv", 20, 160, 328, 150);
    gui_tab_t *tb_0 = gui_tab_create(tv, "tb_0", 0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_1 = gui_tab_create(tv, "tb_1", 0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_2 = gui_tab_create(tv, "tb_2", 0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_3 = gui_tab_create(tv, "tb_3", 0, 0, 0, 0, 3, 0);

    gui_img_create_from_mem(tb_0, "CLOCK_CARD_COMPASS", UI_CLOCK_CARD_COMPASS_BIN, 0, 0, 0, 0);
    gui_img_create_from_mem(tb_1, "CLOCK_CARD_MUSIC", UI_CLOCK_CARD_MUSIC_BIN, 0, 0, 0, 0);
    gui_img_create_from_mem(tb_2, "CLOCK_CARD_ALARM", UI_CLOCK_CARD_ALARM_BIN, 0, 0, 0, 0);
    gui_img_create_from_mem(tb_3, "CLOCK_CARD_WORKOUT", UI_CLOCK_CARD_WORKOUT_BIN, 0, 0, 0, 0);

    // icon
    gui_img_create_from_mem(win_watch, "CLOCK_TEMPERATURE_ICON", UI_CLOCK_ACTIVITY_ICON_BIN, 17, 50, 0,
                            0);
    gui_img_create_from_mem(win_watch, "CLOCK_TEMPERATURE_ICON", UI_CLOCK_TEMPERATURE_ICON_BIN, 17, 330,
                            0, 0);
    gui_img_create_from_mem(win_watch, "CLOCK_COMPASS_ICON", UI_CLOCK_COMPASS_ICON_BIN, 134, 330, 0, 0);
    gui_img_create_from_mem(win_watch, "CLOCK_HEARTRATE_ICON", UI_CLOCK_HEARTRATE_ICON_BIN, 251, 330, 0,
                            0);

    gui_win_set_animate(win_watch, 1000, -1, win_clock_cb, NULL);
}
