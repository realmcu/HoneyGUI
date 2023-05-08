#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_curtain.h>
#include <gui_iconlist.h>
#include "gui_canvas.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include "rtk_gui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "gui_grid.h"
#include <gui_dynamic_img.h>
#include "tp_algo.h"
#include <math.h>
#ifdef MODULE_VG_LITE
#include "gui_vg_lite_clock.h"
#include "gui_cube.h"
#else
#include "gui_cube.h"
#endif
static void app_launcher2_ui_design(gui_app_t *app);
static gui_app_t app_launcher2 =
{
    .screen =
    {
        .name = "app_launcher2",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_launcher2_ui_design,
};

void *get_app_launcher2(void)
{
    return &app_launcher2;
}


static void win_exit_cb(void *obj, gui_event_t e)
{

}
#if 0
static void alien_center(gui_img_t *image)
{
    gui_img_get_height(image);
    draw_img_t *draw_img = &image->draw_img;
    GET_BASE(image)->x = (gui_get_screen_width() - draw_img->img_w) / 2;
    GET_BASE(image)->y = (gui_get_screen_height() - draw_img->img_h) / 2;
}
static void alien_clock(gui_img_t *image, int offset)
{
    gui_img_get_height(image);
    draw_img_t *draw_img = &image->draw_img;
    GET_BASE(image)->y = (gui_get_screen_height() - draw_img->img_h) / 2;
    GET_BASE(image)->x = (gui_get_screen_width()) / 2 + offset - draw_img->img_w;
}
static float get_scale_offset_x(gui_obj_t *img, float scale_x)
{
    /**
     * @note (1-scale)(center-x)
     *
     */
    return (1.0f - scale_x) * (((float)(gui_get_screen_width() / 2)) - (float)(img->x));
}
static int get_scale_offset_y(gui_obj_t *img, float scale_y)
{
    return (1.0f - scale_y) * (((float)(gui_get_screen_width() / 2)) - (float)(img->y));
}

static uint16_t hour_width, hour_center_height, minute_width, minute_center_height,
       second_width, second_center_height;

static uint16_t screen_width, screen_hight;
static char t[10][100];
static float hour_hand_degree, minute_hand_degree, second_hand_degree;
#include <time.h>
static char **get_date()
{
    time_t time1;
    time1 = time(NULL);

    struct tm *timeptr = localtime(&time1);



    /*
        sprintf(t[0], "%d", timeptr->tm_hour);
        if (timeptr->tm_hour < 10)
        {
            sprintf(t[0], "%d%d", 0, timeptr->tm_hour);
        }
        sprintf(t[1], "%d", timeptr->tm_min);
        if (timeptr->tm_min < 10)
        {
            sprintf(t[1], "%d%d", 0, timeptr->tm_min);
        }
        sprintf(t[2], "%d", timeptr->tm_sec);
        if (timeptr->tm_sec < 10)
        {
            sprintf(t[2], "%d%d", 0, timeptr->tm_sec);
        }
        sprintf(t[3], "%d", timeptr->tm_mday);
        sprintf(t[4], "%d", timeptr->tm_mon);
        sprintf(t[5], "%d", timeptr->tm_yday);
        sprintf(t[6], "%s%d", "Week", timeptr->tm_wday);
        sprintf(t[7], "%s%s%s", t[0], ":", t[1]);
        sprintf(t[8], "%s%s%s%s%s", t[0], ":", t[1], ":", t[2]);
        sprintf(t[9], "%d%s%d%s%d", timeptr->tm_year + 1900, "/", timeptr->tm_mon, "/", timeptr->tm_mday);*/
    float hour = (float)(timeptr->tm_hour % 12);
    float min = (float)(timeptr->tm_min);
    float second = (float)(timeptr->tm_sec);
    second_hand_degree = second * 6.0f;
    minute_hand_degree = min * 6.0f + second * 0.1f;
    hour_hand_degree = hour * 30.0f + min * 0.5f + second / 1200.0f;
    char **r = (char **)t;
    return r;
}

#include "gui_magic_img.h"
gui_img_t *image_0;
gui_img_t *image_1;
gui_img_t *image_2;
gui_img_t *image_3;
static void clock_cb(gui_win_t *win)
{

    get_date();
    gui_img_rotation((void *)image_3, hour_hand_degree + 90,
                     gui_get_screen_width() / 2 - image_3->base.x,
                     image_3->draw_img.img_h / 2);
    gui_img_rotation((void *)image_1, minute_hand_degree + 90,
                     gui_get_screen_width() / 2 - image_1->base.x,
                     image_1->draw_img.img_h / 2);
    gui_img_rotation((void *)image_2,
                     win->animate->progress_percent * 360, gui_get_screen_width() / 2 - image_2->base.x,
                     image_2->draw_img.img_h / 2);



}
static void watchface2(void *parent)
{
//   gui_magic_img_create_from_mem(parent, "sun", PATH850_BIN, 0, 0, 0, 0);
}
#endif


#include "gui_magic_img.h"












static gui_tabview_t *tv;

void text_color_cb(gui_event_t e)
{

    gui_tab_jump(tv, 1, 0);
}
void cube_button_cb(gui_event_t e)
{

    gui_tab_jump(tv, 0, 1);
}
void hr_button_cb(gui_event_t e)
{

    gui_tab_jump(tv, 2, 0);
}










void text_a(gui_text_t *text)
{
    int width;
    if (text->animate->progress_percent < 0.5f)
    {
        width = text->animate->progress_percent * RTK_GUI_DEFAULT_FONT_SIZE * 14;
    }
    else
    {
        width = RTK_GUI_DEFAULT_FONT_SIZE * 14 - text->animate->progress_percent * RTK_GUI_DEFAULT_FONT_SIZE
                * 14;
    }
    if (width > 3 && width < 200)
    {
        gui_text_size_set(text, width, width);
    }

}



#include "draw_font.h"
static void app_launcher2_ui_design(gui_app_t *app)
{
    gui_log("app_launcher2_ui_design\n");
    gui_font_stb_init(TANGYUANTI_TTF);
    gui_win_t *win = gui_win_create(&(app->screen), "win", 0, 0, 320, 320);
    gui_obj_add_event_cb(win, win_exit_cb, GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE, NULL);
    tv = gui_tabview_create(win, "tabview", 0, 0, 0, 0);
    tv->style = SLIDE_SCALE_FADE;
    gui_tab_t *tb0 = gui_tab_create(tv, "tb0", 0, 0, 0, 0, 0, 0);
    gui_tab_t *tb1 = gui_tab_create(tv, "tb1", 0, 0, 0, 0, 1, 0);
    gui_tab_t *tb2 = gui_tab_create(tv, "tb1", 0, 0, 0, 0, 0, 1);
    gui_tab_t *tb3 = gui_tab_create(tv, "tb0", 0, 0, 0, 0, 2, 0);
    gui_tab_t *tb4 = gui_tab_create(tv, "tb1", 0, 0, 0, 0, 3, 0);
    gui_tab_t *tb5 = gui_tab_create(tv, "tb1", 0, 0, 0, 0, 4, 0);
    gui_tab_t *tb6 = gui_tab_create(tv, "tb1", 0, 0, 0, 0, 5, 0);
    gui_tab_t *tb7 = gui_tab_create(tv, "tb1", 0, 0, 0, 0, 6, 0);
    //gui_tab_t *tb3 = gui_tab_create(tv, "tb1", 0, 0, 0, 0, 2, 0);
    gui_curtainview_t *cv = gui_curtainview_create(tb0, "cv", 0, 0, 0, 0);
    gui_curtain_t *c1 = gui_curtain_create(cv, "c1", 0, 0, 0, 0, CURTAIN_MIDDLE, 1);
    gui_curtain_t *c2 = gui_curtain_create(cv, "c2", 0, 0, 0, 0, CURTAIN_UP, 1);
    gui_curtain_t *c3 = gui_curtain_create(cv, "c3", 0, 0, 0, 0, CURTAIN_LEFT, 0.5f);
    extern void watchface1(void *parent);
    watchface1(c1);
    extern void sport_chart(gui_obj_t *parent);
    sport_chart((void *)tb1);
    extern void curtain1(void *curtian);
    curtain1(c3);
    extern void curtain2(void *curtian);
    curtain2(c2);
    extern void grid_icon(void *parent);
    grid_icon(tb2);
    extern void heart_rate_chart(gui_obj_t *parent);
    heart_rate_chart((void *)tb4);
    extern void read_page(void *parent);
    read_page(tb7);
    extern void create_selector(gui_obj_t *screen);
    create_selector((void *)tb6);
    extern void music(gui_obj_t *screen);
    music((void *)tb5);
    extern void palette_wheel(gui_obj_t *screen);
    palette_wheel((void *)tb3);
}
