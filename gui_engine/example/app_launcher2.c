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

static void watchface1(void *parent)
{



#ifdef MODULE_VG_LITE
    gui_vg_lite_clock_t *path_clock = gui_vg_lite_clock_create(parent,  "path_clock", NULL, 0, 0,
                                                               454, 454);
#else
    //gui_button_create(parent, 252, 57, 100, 100, ACTIVITY_SVG, NULL, NULL, 2, 5134);
#endif

}


void static sport_chart_draw(gui_canvas_t *c)
{
    float  end_angle1 = 0.0f;
    float  end_angle2 = 0.0f;
    float  end_angle3 = 0.0f;
    {
        static  float pro  = 0;
        static bool end_flag = false;
        if (pro >= 0.99f)
        {
            end_flag = !end_flag;
            pro = 0;
        }
        bool temp = (pro >= 0.99f || pro <= 0.01f);
        //if (!temp)
        {
            if (!end_flag)
            {
                end_angle1 = pro * (330 - 30) * 0.5f + 30;
                end_angle2 = pro * (330 - 30) * 0.75f + 30;
                end_angle3 = pro * (330 - 30) + 30;
            }
            else
            {
                end_angle1 = (1 - pro) * (330 - 30) * 0.5f + 30;
                end_angle2 = (1 - pro) * (330 - 30) * 0.75f + 30;
                end_angle3 = (1 - pro) * (330 - 30) + 30;
            }
            gui_log("end_angle1:%f,%d,%f\n", end_angle1, end_flag, pro);
        }
        pro += 0.03f;
    }

    int w = gui_get_screen_width();
    int h = gui_get_screen_height();
    int gap = 6;
    int stroke_width = 30;
    canvas_arc_t arc1 = {0};
    arc1.cx = w / 2;
    arc1.cy = h / 2;
    arc1.r = w / 2 - gap - stroke_width;
    arc1.stroke.fill.color_data.rgba = 0xffff0099;
    arc1.stroke.stroke_width = stroke_width;
    arc1.stroke.stroke_linecap = ROUND;
    arc1.start_angle = 30;
    arc1.end_angle = 330;
    gui_canvas_api.arc(c, &arc1);
    arc1.end_angle = end_angle1;
    arc1.stroke.fill.color_data.rgba = 0xffff00ff;
    gui_canvas_api.arc(c, &arc1);
    arc1.cx = w / 2;
    arc1.cy = h / 2;
    arc1.r = w / 2 - (gap + stroke_width) * 2;
    arc1.stroke.fill.color_data.rgba = 0xff00ff99;
    arc1.end_angle = 330;
    gui_canvas_api.arc(c, &arc1);
    arc1.end_angle = end_angle2;
    arc1.stroke.fill.color_data.rgba = 0xff00ffff;
    gui_canvas_api.arc(c, &arc1);
    arc1.cx = w / 2;
    arc1.cy = h / 2;
    arc1.r = w / 2 - (gap + stroke_width) * 3;
    arc1.stroke.fill.color_data.rgba = 0xffff99;
    arc1.end_angle = 330;
    gui_canvas_api.arc(c, &arc1);
    arc1.end_angle = end_angle3;
    arc1.stroke.fill.color_data.rgba = 0xffffff;
    gui_canvas_api.arc(c, &arc1);
}

static void sport_chart(gui_obj_t *parent)
{
    gui_canvas_t *c = gui_canvas_create(parent, "c", 0, 0, 454, 454, 0);
    c->draw = sport_chart_draw;
}


static void heart_rate_chart(gui_obj_t *parent)
{
    void *array[] = {CRAB_LIME_BIN, CRAB_LIME_BIN, RTL_ORIGIN_BIN, RTL_ORIGIN_BIN, WATCH_RED_BIN, WATCH_RED_BIN};
    gui_cube_create(parent, "cube", array, 0, 0, 454, 454);

}
static void canvas1_draw(gui_canvas_t *c)
{

    canvas_rectangle_t r = {0};
    memset(&r, 0, sizeof(r));

    r.fill.color_data.rgba = 0x000000ff;
    r.width = 454;
    r.height = 454;
    gui_canvas_api.rectangle(c, &r);

}
static void canvas2_draw(gui_canvas_t *c)
{
    canvas_rectangle_t r = {0};
    r.fill.color_data.rgba = 0x88;
    r.width = 454;
    r.height = 454;
    gui_canvas_api.rectangle(c, &r);

}




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
static void curtain1(void *curtian)
{
    gui_canvas_t *canvas1 = gui_canvas_create(curtian, "canvas1", 0, 0, 454, 454, 0);
    canvas1->draw = canvas1_draw;
    static void *launcher2_curtain1_charge_video_array[] =
    {
        CHARGE2_BIN, CHARGE4_BIN, CHARGE6_BIN, CHARGE8_BIN, CHARGE10_BIN, CHARGE12_BIN, CHARGE14_BIN, CHARGE16_BIN, CHARGE18_BIN, CHARGE20_BIN, CHARGE22_BIN, CHARGE24_BIN, CHARGE26_BIN, CHARGE28_BIN, CHARGE30_BIN, CHARGE32_BIN, CHARGE34_BIN, CHARGE36_BIN, CHARGE38_BIN, CHARGE40_BIN, CHARGE42_BIN, CHARGE44_BIN, CHARGE46_BIN, CHARGE48_BIN, CHARGE50_BIN, CHARGE52_BIN, CHARGE54_BIN, CHARGE56_BIN, CHARGE58_BIN, CHARGE60_BIN, CHARGE62_BIN, CHARGE64_BIN, CHARGE66_BIN, CHARGE68_BIN, CHARGE70_BIN, CHARGE72_BIN, CHARGE74_BIN, CHARGE76_BIN, CHARGE78_BIN, CHARGE80_BIN, /*CHARGE82_BIN, CHARGE84_BIN, CHARGE86_BIN, CHARGE88_BIN, CHARGE90_BIN, CHARGE92_BIN, CHARGE94_BIN, CHARGE96_BIN, CHARGE98_BIN, CHARGE100_BIN, CHARGE102_BIN, CHARGE104_BIN, CHARGE106_BIN, CHARGE108_BIN, CHARGE110_BIN, CHARGE112_BIN, CHARGE114_BIN, CHARGE116_BIN, CHARGE118_BIN, CHARGE120_BIN,*/
    };
    void *charge_img = (void *)gui_dynamic_create_from_mem((void *)curtian, "g",
                                                           launcher2_curtain1_charge_video_array,
                                                           470 - 120 * 2, 0, 59 - 20, 40, INT32_MAX);
    gui_img_scale(charge_img, 2, 2);
}

static void sport_button_press_ani_cb(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    float alpha_to = 100;
    static float alpha_from = 0;
    static float from = 0;
    if (per == 0.0f)
    {
        from = ((gui_magic_img_t *)(button->img))->scale_x;
        alpha_from = button->img->draw_img.opacity_value;
    }
    from = 1.0f;
    float scale = (-0.5f * from) * per + from;
    //circle_color_tab[1] = change_opacity_of_rgba(circle_color_tab[1],
    //                                             (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    gui_img_scale((void *)button->img, scale,  scale);
    gui_img_translate((void *)button->img, 25.0f * (from - scale),  25.0f * (from - scale));
    gui_img_set_opacity((void *)button->img,
                        (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    gui_log("o:%f\n", 25.0f * (from - scale));
}
static void sport_button_release_ani_cb(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    float alpha_to = UINT8_MAX;
    static float alpha_from = 0;
    static float from = 0;
    if (per == 0.0f)
    {
        from = ((gui_magic_img_t *)(button->img))->scale_x;
        alpha_from = button->img->draw_img.opacity_value;
    }
    //from = 1.25f;
    float scale = (1.0f - from) * per + from;
    //circle_color_tab[1] = change_opacity_of_rgba(circle_color_tab[1],
    //                                             (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    gui_img_set_opacity((void *)button->img,
                        (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    gui_img_scale((void *)button->img, scale,  scale);
    gui_img_translate((void *)button->img, 25.0f * (1.0f - scale),  25.0f * (1.0f - scale));
}

static void music_button_press_ani_cb(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    float alpha_to = 100;
    static float alpha_from = 0;
    static float from = 0;
    if (per == 0.0f)
    {
        from = ((gui_magic_img_t *)(button->img))->scale_x;
        alpha_from = button->img->draw_img.opacity_value;
    }
    from = 1.0f;
    float scale = (0.5f * from) * per + from;
    //circle_color_tab[1] = change_opacity_of_rgba(circle_color_tab[1],
    //                                             (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    gui_img_scale((void *)button->img, scale,  scale);
    gui_img_translate((void *)button->img, 25.0f * (from - scale),  25.0f * (from - scale));
    gui_img_set_opacity((void *)button->img,
                        (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    gui_log("o:%f\n", scale);
}
static void music_button_release_ani_cb(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    float alpha_to = UINT8_MAX;
    static float alpha_from = 0;
    static float from = 0;
    if (per == 0.0f)
    {
        from = ((gui_magic_img_t *)(button->img))->scale_x;
        alpha_from = button->img->draw_img.opacity_value;
    }
    //from = 1.25f;
    float scale = (1.0f - from) * per + from;
    //circle_color_tab[1] = change_opacity_of_rgba(circle_color_tab[1],
    //                                             (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    gui_img_set_opacity((void *)button->img,
                        (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    gui_img_scale((void *)button->img, scale,  scale);
    gui_img_translate((void *)button->img, 25.0f * (1.0f - scale),  25.0f * (1.0f - scale));
    gui_log("o:%f\n", scale);
}
static void cube_button_press_ani_cb(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    float alpha_to = 100;
    static float alpha_from = 0;
    static float from = 0;
    if (per == 0.0f)
    {
        from = ((gui_magic_img_t *)(button->img))->scale_x;
        alpha_from = button->img->draw_img.opacity_value;
    }
    from = 2.5f;
    float scale = (-0.5f * from) * per + from;
    //circle_color_tab[1] = change_opacity_of_rgba(circle_color_tab[1],
    //                                             (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    gui_img_scale((void *)button->img, scale,  scale);
    gui_img_translate((void *)button->img, 25.0f * (2.5f - scale),  25.0f * (2.5f - scale));
    gui_img_set_opacity((void *)button->img,
                        (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    gui_log("o:%f\n", 25.0f * (2.5f - scale));
}
static void cube_button_release_ani_cb(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    float alpha_to = UINT8_MAX;
    static float alpha_from = 0;
    static float from = 0;
    if (per == 0.0f)
    {
        from = ((gui_magic_img_t *)(button->img))->scale_x;
        alpha_from = button->img->draw_img.opacity_value;
    }
    //from = 1.25f;
    float scale = (2.5f - from) * per + from;
    //circle_color_tab[1] = change_opacity_of_rgba(circle_color_tab[1],
    //                                             (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    gui_img_set_opacity((void *)button->img,
                        (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    gui_img_scale((void *)button->img, scale,  scale);
    gui_img_translate((void *)button->img, 25.0f * (2.5f - scale),  25.0f * (2.5f - scale));
}
static void hr_button_press_ani_cb(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    float alpha_to = 100;
    static float alpha_from = 0;
    static float from = 0;
    if (per == 0.0f)
    {
        from = ((gui_magic_img_t *)(button->img))->scale_x;
        alpha_from = button->img->draw_img.opacity_value;
    }
    from = 1.5f;
    float scale = (-0.5f * from) * per + from;
    //circle_color_tab[1] = change_opacity_of_rgba(circle_color_tab[1],
    //                                             (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    gui_img_scale((void *)button->img, scale,  scale);
    gui_img_translate((void *)button->img, 25.0f * (from - scale),  25.0f * (from - scale));
    gui_img_set_opacity((void *)button->img,
                        (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
}
static void hr_button_release_ani_cb(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    float alpha_to = UINT8_MAX;
    static float alpha_from = 0;
    static float from = 0;
    if (per == 0.0f)
    {
        from = ((gui_magic_img_t *)(button->img))->scale_x;
        alpha_from = button->img->draw_img.opacity_value;
    }
    //from = 1.25f;
    float scale = (1.5f - from) * per + from;
    //circle_color_tab[1] = change_opacity_of_rgba(circle_color_tab[1],
    //                                             (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    gui_img_set_opacity((void *)button->img,
                        (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    gui_img_scale((void *)button->img, scale,  scale);
    gui_img_translate((void *)button->img, 25.0f * (1.5f - scale),  25.0f * (1.5f - scale));
}
static void sport_button_press(gui_button_t *b)
{
    gui_button_api.set_animate(b, 200, 0, sport_button_press_ani_cb, b);
}
static void sport_button_release(gui_button_t *b)
{
    gui_button_api.set_animate(b, 200, 0, sport_button_release_ani_cb, b);
}
static void music_button_press(gui_button_t *b)
{
    gui_button_api.set_animate(b, 100, 0, music_button_press_ani_cb, b);
}
static void music_button_release(gui_button_t *b)
{
    gui_button_api.set_animate(b, 100, 0, music_button_release_ani_cb, b);
}
#include "nanosvg.h"
//start_mp3(),stop_mp3()
static bool music_button_click_pause = true;
static void start_mp3(void)
{
    gui_log("static void start_mp3(void)\n");
    extern void audio_test_start_mp3(void);
#if defined (GUI_RTL8772F)
    audio_test_start_mp3();
#endif
}
static void stop_mp3(void)
{
    gui_log("static void stop_mp3(void)\n");
    extern void audio_test_stop_mp3(void);
#if defined (GUI_RTL8772F)
    audio_test_stop_mp3();
#endif
}
static void music_button_click(gui_button_t *b)
{
    gui_log("music_button_click\n");
    extern void nsvgDelete(NSVGimage * image);
    nsvgDelete(b->img->draw_img.data);
    gui_tree_free(b->img);

    if (!music_button_click_pause)
    {
        b->img = (void *)gui_svg_create_from_mem(b, PALY2_SVG, 610, 0, 0, 100, 100);
        stop_mp3();
    }
    else
    {
        b->img = (void *)gui_svg_create_from_mem(b, PAUSE2_SVG, 946, 0, 0, 100, 100);
        start_mp3();
    }


    music_button_click_pause = !music_button_click_pause;
}
static void music_button_pause_release(gui_button_t *b)
{
    music_button_click(b);
    gui_button_api.set_animate(b, 100, 0, music_button_release_ani_cb, b);
}

static void cube_button_press(gui_button_t *b)
{
    gui_button_api.set_animate(b, 200, 0, cube_button_press_ani_cb, b);
}
static void cube_button_release(gui_button_t *b)
{
    gui_button_api.set_animate(b, 200, 0, cube_button_release_ani_cb, b);
}
static void hr_button_press(gui_button_t *b)
{
    gui_button_api.set_animate(b, 200, 0, hr_button_press_ani_cb, b);
}
static void hr_button_release(gui_button_t *b)
{
    gui_button_api.set_animate(b, 200, 0, hr_button_release_ani_cb, b);
}
static void curtain2(void *curtian)
{
    gui_canvas_t *canvas1 = gui_canvas_create(curtian, "canvas2", 0, 0, 454, 454, 0);
    canvas1->draw = canvas2_draw;

    gui_button_t *sport_button;
    gui_button_t *hr_button;
    gui_button_t *cube_button;
    sport_button = gui_button_create(curtian, 252, 57, 100, 100, ACTIVITY_SVG, NULL, NULL, 2,
                                     5134);
    hr_button = gui_button_create(curtian, 265, 205, 150, 150, SETTING_SVG, NULL, NULL, 2,
                                  2056);
    cube_button = gui_button_create(curtian, 5, 108, 250, 250, MESSAGE_SVG, NULL, NULL, 2,
                                    2666);
    //gui_button_click(sport_button, (gui_event_cb_t)text_color_cb);
    //gui_button_click(hr_button, (gui_event_cb_t)hr_button_cb);
    //gui_button_click(cube_button, (gui_event_cb_t)cube_button_cb);
    gui_button_api.onPress(sport_button, sport_button_press, sport_button);
    gui_button_api.onRelease(sport_button, sport_button_release, sport_button);
    gui_button_api.onPress(cube_button, cube_button_press, sport_button);
    gui_button_api.onRelease(cube_button, cube_button_release, sport_button);
    gui_button_api.onPress(hr_button, hr_button_press, sport_button);
    gui_button_api.onRelease(hr_button, hr_button_release, sport_button);
}
static void grid_icon(void *parent)
{
    void *svg_array[] = {ACTIVITY_SVG, ALARM_SVG, HEARTRATE_SVG, MESSAGE_SVG, MUSIC_SVG, SETTING_SVG, CONTACTS_SVG, TELEPHONE_SVG, TIME_SVG, WEATHER_SVG, SLEEP_SVG};
    int svg_size_array[] = {5134, 2980, 2890, 2666, 2746, 2056,  3052, 2882, 2097, 3629, 2539};
    gui_grid_t *g1 = gui_grid_create(parent, 0, 0, 5, 5, 103, 103);
    g1->style = GRID_SCALE;
    size_t j = 0;
    size_t k = 1;
    for (size_t i = 0; i < 25; i++)
    {
        j++;
        if (j == 11)
        {
            j = 0;
        }
        k++;
        if (k == 11)
        {
            k = 0;
        }

        gui_svg_create_from_mem(g1, svg_array[j], svg_size_array[j], 0, 0, 100, 100);
    }
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
#include "nanovg_agge.h"
static void read_page_draw(gui_canvas_t *c)
{
    static float t = 0;

    canvas_wave_t w = {0};
    w.h = 454;
    w.w = 454;
    w.point_count = 6;
    float dx = w.w / 5.0f;
    float samples[w.point_count];
    static float sx[6];
    static float sy[6];

    samples[0] = (1.0f + sinf(t * 1.2345f + cosf(t * 0.33457f) * 0.44f)) * 0.5f;
    samples[1] = (1.0f + sinf(t * 0.68363f + cosf(t * 1.3f) * 1.55f)) * 0.5f;
    samples[2] = (1.0f + sinf(t * 1.1642f + cosf(t * 0.33457f) * 1.24f)) * 0.5f;
    samples[3] = (1.0f + sinf(t * 0.56345f + cosf(t * 1.63f) * 0.14f)) * 0.5f;
    samples[4] = (1.0f + sinf(t * 1.6245f + cosf(t * 0.254f) * 0.3f)) * 0.5f;
    samples[5] = (1.0f + sinf(t * 0.345f + cosf(t * 0.03f) * 0.6f)) * 0.5f;
    t += 0.05f;
    int i;
    for (i = 0; i < w.point_count; i++)
    {
        sx[i] = w.x + i * dx;
        sy[i] = w.y + w.h * samples[i] * 0.8f;
    }
    w.point_x = sx;
    w.point_y = sy;
    extern NVGcolor nvgHSLA(float h, float s, float l, unsigned char a);
    NVGcolor color = nvgHSLA(sinf(2.0F * M_PI / 12.0F * 0.12f * (t)), 1.0f, 0.55f, 255);
    w.fill.color_data.rgba = (uint32_t)((uint32_t)(color.r * 255.0f) << 24) + (uint32_t)((uint32_t)(
                                 color.g * 255.0f) << 16) + (uint32_t)((uint32_t)(color.b * 255.0f) << 8) + (uint32_t)((uint32_t)(
                                             color.a * 255.0f));
    gui_canvas_api.wave(c, &w);
}
void read_page(void *parent)
{
    //static char *t = "R";
    /*gui_text_t *text = gui_text_create(parent, "icon_text", 50, 272, RTK_GUI_DEFAULT_FONT_SIZE*4, RTK_GUI_DEFAULT_FONT_SIZE*4);
        gui_text_set(text, t, "rtk_font_stb", BUTTON_COLOR, strlen(t),
                     RTK_GUI_DEFAULT_FONT_SIZE*4);
    text = gui_text_create(parent, "icon_text", 50, 200, RTK_GUI_DEFAULT_FONT_SIZE*3, RTK_GUI_DEFAULT_FONT_SIZE*3);
        gui_text_set(text, t, "rtk_font_stb", BUTTON_COLOR, strlen(t),
                     RTK_GUI_DEFAULT_FONT_SIZE*3);
    text = gui_text_create(parent, "icon_text", 50, 150, RTK_GUI_DEFAULT_FONT_SIZE*2, RTK_GUI_DEFAULT_FONT_SIZE*2);
        gui_text_set(text, t, "rtk_font_stb", BUTTON_COLOR, strlen(t),
                     RTK_GUI_DEFAULT_FONT_SIZE*2);
    text = gui_text_create(parent, "icon_text", 50, 110, RTK_GUI_DEFAULT_FONT_SIZE, RTK_GUI_DEFAULT_FONT_SIZE);
        gui_text_set(text, t, "rtk_font_stb", BUTTON_COLOR, strlen(t),
                     RTK_GUI_DEFAULT_FONT_SIZE);
        text = gui_text_create(parent, "icon_text", 200, 100-50, RTK_GUI_DEFAULT_FONT_SIZE*5, RTK_GUI_DEFAULT_FONT_SIZE*5);
        gui_text_set(text, t, "rtk_font_stb", BUTTON_COLOR, strlen(t),
                     RTK_GUI_DEFAULT_FONT_SIZE*5);
    text = gui_text_create(parent, "icon_text", 200, 200-50, RTK_GUI_DEFAULT_FONT_SIZE*6, RTK_GUI_DEFAULT_FONT_SIZE*6);
        gui_text_set(text, t, "rtk_font_stb", BUTTON_COLOR, strlen(t),
                     RTK_GUI_DEFAULT_FONT_SIZE*6);  */
    gui_canvas_t *canvas = gui_canvas_create(parent, "c", 0, 0, 454, 454, 0);
    canvas->draw = read_page_draw;
//    gui_text_t *text = gui_text_create(parent, "icon_text", 150, 200, RTK_GUI_DEFAULT_FONT_SIZE * 7,
//                                       RTK_GUI_DEFAULT_FONT_SIZE * 7);
//    gui_text_set(text, t, "rtk_font_stb", BUTTON_COLOR, strlen(t),
//                 RTK_GUI_DEFAULT_FONT_SIZE * 7);
//    gui_text_set_animate(text, 4000, -1, text_a, text);

}
#include "gui_d_island.h"
static int selector_width_begin = 180;
static int selector_width_end = 418;
static int selector_width = 180;
static uint32_t selector_color = 0xd2dbc9ff;
static int circle_gap = 18;
#define APP_LAUNCHER_CIRCLE_R 40
static const int circle_r = APP_LAUNCHER_CIRCLE_R;
#define APP_LAUNCHER_SELECTOR_RX 62
static const int selector_rx = APP_LAUNCHER_SELECTOR_RX;
static int circle_cy = APP_LAUNCHER_SELECTOR_RX;
static int circle_cx = APP_LAUNCHER_SELECTOR_RX;
static const int circle_number = 4;
static uint32_t circle_color_tab[4] = {0xe67367ff, 0xeab6a2ff, 0x839672ff, 0xe8734cff};
static uint32_t circle_r_tab[4] = {APP_LAUNCHER_CIRCLE_R, APP_LAUNCHER_CIRCLE_R, APP_LAUNCHER_CIRCLE_R, APP_LAUNCHER_CIRCLE_R};
static bool circle_animate;
static int circle_ani_r_to = 60;
static int circle_ani_r[4];
static unsigned char circle_ani_opacity[4];
static gui_canvas_t *c1 ;
static gui_button_t *button_tab[4];
static gui_d_island_t *island;
static void selector_draw(gui_canvas_t *c)
{
    canvas_rectangle_t r = {0};
    r.fill.color_data.rgba = selector_color;
    r.height = selector_rx * 2;
    r.width = selector_width;
    r.rx = selector_rx;
    r.x = 0;
    r.y = 0;
    gui_canvas_api.rectangle(c, &r);
    for (size_t i = 0; i < circle_number; i++)
    {
        canvas_circle_t circle = {0};
        circle.fill.color_data.rgba = circle_color_tab[i];
        circle.cx = circle_cx + i * circle_gap;
        button_tab[i]->base.x = circle.cx - circle_r_tab[i];
        circle.cy = circle_cy;
        circle.r = circle_r_tab[i];
        gui_canvas_api.circle(c, &circle);
    }





}
static uint32_t change_opacity_of_rgba(uint32_t rgba, uint8_t opacity)
{
    canvas_color_t c = {0};
    c.color.rgba = rgba;
    c.color.channel.alpha = opacity;
    return c.color.rgba;
}
static void c1_animate()
{
    int x;
    int from = selector_width_begin;
    int to = selector_width_end;
    uint8_t alpha_from = 100;
    uint8_t alpha_to = 200;
    float per = (c1->animate->progress_percent);
    //printf("%f ", per);
    x = from + (to - from) * per;
    //y = from + (to - from) * per;
    selector_width = x;
    from = (selector_width_begin - selector_rx * 2) / (circle_number - 1);
    to = (selector_width_end - selector_rx * 2) / (circle_number - 1);
    circle_gap = from + (to - from) * per;
    //gui_log("circle_gap%d\n",circle_gap);
    selector_color = change_opacity_of_rgba(selector_color, (alpha_to - alpha_from) * per + alpha_from);
    alpha_to = UINT8_MAX;
    for (size_t i = 0; i < circle_number; i++)
    {
        circle_color_tab[i] = change_opacity_of_rgba(circle_color_tab[i],
                                                     (alpha_to - alpha_from) * per + alpha_from);
    }
    if (per == 1)
    {
        circle_animate = true;
    }

}
static void c1_animate2()
{
    int x;
    int from = selector_width_end;
    int to = selector_width_begin;
    uint8_t alpha_from = 200;
    uint8_t alpha_to = 100;
    float per = (c1->animate->progress_percent);
    //printf("%f ", per);
    x = from + (to - from) * per;
    //y = from + (to - from) * per;
    selector_width = x;
    from = (selector_width_end - selector_rx * 2) / (circle_number - 1);
    to = (selector_width_begin - selector_rx * 2) / (circle_number - 1);
    circle_gap = from + (to - from) * per;
    gui_log("circle_gap%d,%d\n", circle_gap, selector_width);
    selector_color = change_opacity_of_rgba(selector_color, (alpha_to - alpha_from) * per + alpha_from);
    alpha_to = UINT8_MAX;
    for (size_t i = 0; i < circle_number; i++)
    {
        circle_color_tab[i] = change_opacity_of_rgba(circle_color_tab[i],
                                                     (alpha_to - alpha_from) * per + alpha_from);
    }

}
static void button_ani_cb(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    circle_r_tab[0] = circle_r + (circle_ani_r_to - circle_r) * per;
    circle_ani_r[0] = circle_r_tab[0];
    float alpha_from = UINT8_MAX;
    float alpha_to = 100;
    //gui_log("opacity:%d\n", (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    circle_color_tab[0] = change_opacity_of_rgba(circle_color_tab[0],
                                                 (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    circle_ani_opacity[0] = (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from);
}
static void button_ani_cb2(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    int from = circle_ani_r[0];
    int to = circle_r;
    circle_r_tab[0] = from + (to - from) * per;
    float alpha_from = circle_ani_opacity[0];
    float alpha_to = UINT8_MAX;
    circle_color_tab[0] = change_opacity_of_rgba(circle_color_tab[0],
                                                 (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
}
static void button2_ani_cb(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    circle_r_tab[1] = circle_r + (circle_ani_r_to - circle_r) * per;
    circle_ani_r[1] = circle_r_tab[1];
    float alpha_from = UINT8_MAX;
    float alpha_to = 100;
    //gui_log("opacity:%d\n", (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    circle_color_tab[1] = change_opacity_of_rgba(circle_color_tab[1],
                                                 (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    circle_ani_opacity[1] = (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from);
}
static void button2_ani_cb2(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    int from = circle_ani_r[1];
    int to = circle_r;
    circle_r_tab[1] = from + (to - from) * per;
    float alpha_from = circle_ani_opacity[1];
    float alpha_to = UINT8_MAX;
    circle_color_tab[1] = change_opacity_of_rgba(circle_color_tab[1],
                                                 (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
}
static void button3_ani_cb(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    circle_r_tab[2] = circle_r + (circle_ani_r_to - circle_r) * per;
    circle_ani_r[2] = circle_r_tab[2];
    float alpha_from = UINT8_MAX;
    float alpha_to = 100;
    //gui_log("opacity:%d\n", (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    circle_color_tab[2] = change_opacity_of_rgba(circle_color_tab[2],
                                                 (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    circle_ani_opacity[2] = (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from);
}
static void button3_ani_cb2(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    int from = circle_ani_r[2];
    int to = circle_r;
    circle_r_tab[2] = from + (to - from) * per;
    float alpha_from = circle_ani_opacity[2];
    float alpha_to = UINT8_MAX;
    circle_color_tab[2] = change_opacity_of_rgba(circle_color_tab[2],
                                                 (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
}
static void button4_ani_cb(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    circle_r_tab[3] = circle_r + (circle_ani_r_to - circle_r) * per;
    circle_ani_r[3] = circle_r_tab[3];
    float alpha_from = UINT8_MAX;
    float alpha_to = 100;
    //gui_log("opacity:%d\n", (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    circle_color_tab[3] = change_opacity_of_rgba(circle_color_tab[3],
                                                 (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    circle_ani_opacity[3] = (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from);
}
static void button4_ani_cb2(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    int from = circle_ani_r[3];
    int to = circle_r;
    circle_r_tab[3] = from + (to - from) * per;
    float alpha_from = circle_ani_opacity[3];
    float alpha_to = UINT8_MAX;
    circle_color_tab[3] = change_opacity_of_rgba(circle_color_tab[3],
                                                 (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
}
static void selector_press_cb(gui_button_t *button)
{
    gui_log("selector_press_cb\n");
    if (!circle_animate)
    {
        gui_canvas_api.set_animate((void *)c1, 200, 0, c1_animate, c1);

    }
    else
    {
        circle_animate = false;
        gui_canvas_api.set_animate((void *)c1, 200, 0, c1_animate2, c1);

    }
    //memset(&(icm->base.animate), 0, sizeof(gui_animate_t));
    //icm->base.set_animate(icm, 100, 0, icm_animate, icm);
    gui_button_api.set_animate(button, 200, 0, button_ani_cb, button);
}
static void selector_release_cb(gui_button_t *button)
{
    //if (circle_animate)
    {
        gui_log("selector_release_cb\n");
        gui_button_api.set_animate(button, 200, 0, button_ani_cb2, button);
    }

}
static void selector_press_cb2(gui_button_t *button)
{
    if (circle_animate)
    {
        gui_log("selector_press_cb\n");
        gui_button_api.set_animate(button, 200, 0, button2_ani_cb, button);
        island->set_island(island, 270, 50);
        canvas_circle_t circle = {0};
        circle.fill.color_data.rgba = circle_color_tab[1];
        island->set_island_color(island, &(circle.fill));
    }

}
static void selector_release_cb2(gui_button_t *button)
{
    if (circle_animate)
    {
        //gui_log("selector_release_cb\n");
        gui_button_api.set_animate(button, 200, 0, button2_ani_cb2, button);
    }
}
static void selector_press_cb3(gui_button_t *button)
{
    if (circle_animate)
    {
        gui_log("selector_press_cb\n");
        gui_button_api.set_animate(button, 200, 0, button3_ani_cb, button);
        island->set_island(island, 270, 150);
        canvas_circle_t circle = {0};
        circle.fill.color_data.rgba = circle_color_tab[2];
        island->set_island_color(island, &(circle.fill));
    }
}
static void selector_release_cb3(gui_button_t *button)
{
    if (circle_animate)
    {
        gui_log("selector_release_cb\n");
        gui_button_api.set_animate(button, 200, 0, button3_ani_cb2, button);
    }
}
static void selector_press_cb4(gui_button_t *button)
{
    if (circle_animate)
    {
        gui_log("selector_press_cb\n");
        gui_button_api.set_animate(button, 200, 0, button4_ani_cb, button);
        island->set_island(island, 100, 50);
        canvas_circle_t circle = {0};
        circle.fill.color_data.rgba = circle_color_tab[3];
        island->set_island_color(island, &(circle.fill));
    }
}
static void selector_release_cb4(gui_button_t *button)
{
    if (circle_animate)
    {
        gui_log("selector_release_cb\n");
        gui_button_api.set_animate(button, 200, 0, button4_ani_cb2, button);
    }
}

static void create_selector(gui_obj_t *screen)
{
    c1 = gui_canvas_create(screen, "1", 10, 200, 320, 85, 0x00CDCDFF);
    c1->draw = selector_draw;
    int i = 0;
    gui_button_t *b = gui_button_create(c1, circle_cx + i * circle_gap - circle_r_tab[i],
                                        circle_cy - circle_r_tab[i], circle_r_tab[i] * 2, circle_r_tab[i] * 2, 0, 0, 0, 0, 0);
    gui_button_api.onRelease(b, selector_release_cb, b);
    gui_button_api.onPress(b, selector_press_cb, b);
    button_tab[i] = b;
    i = 1;
    b = gui_button_create(c1, circle_cx + i * circle_gap - circle_r_tab[i], circle_cy - circle_r_tab[i],
                          circle_r_tab[i] * 2, circle_r_tab[i] * 2, 0, 0, 0, 0, 0);
    gui_button_api.onRelease(b, selector_release_cb2, b);
    gui_button_api.onPress(b, selector_press_cb2, b);
    button_tab[i] = b;
    i = 2;
    b = gui_button_create(c1, circle_cx + i * circle_gap - circle_r_tab[i], circle_cy - circle_r_tab[i],
                          circle_r_tab[i] * 2, circle_r_tab[i] * 2, 0, 0, 0, 0, 0);
    gui_button_api.onRelease(b, selector_release_cb3, b);
    gui_button_api.onPress(b, selector_press_cb3, b);
    button_tab[i] = b;
    i = 3;
    b = gui_button_create(c1, circle_cx + i * circle_gap - circle_r_tab[i], circle_cy - circle_r_tab[i],
                          circle_r_tab[i] * 2, circle_r_tab[i] * 2, 0, 0, 0, 0, 0);
    gui_button_api.onRelease(b, selector_release_cb4, b);
    gui_button_api.onPress(b, selector_press_cb4, b);
    button_tab[i] = b;

    island = gui_d_island_create(screen);

}

gui_img_t *music_album_picture;

static void music_draw(gui_canvas_t *c)
{
    static float t = 0;


    if (!music_button_click_pause)
    {
        t += 0.05f;
//        gui_img_rotation((void *)music_album_picture, angle * 3.0f, gui_get_screen_width() / 2,
//                         gui_get_screen_height() / 2);
    }


    float dx = 454 / 5.0f;
    float samples[6];
    float sx[6], sy[6];

    samples[0] = (1.0f + sinf(t * 1.2345f + cosf(t * 0.33457f) * 0.44f)) * 0.5f;
    samples[1] = (1.0f + sinf(t * 0.68363f + cosf(t * 1.3f) * 1.55f)) * 0.5f;
    samples[2] = (1.0f + sinf(t * 1.1642f + cosf(t * 0.33457f) * 1.24f)) * 0.5f;
    samples[3] = (1.0f + sinf(t * 0.56345f + cosf(t * 1.63f) * 0.14f)) * 0.5f;
    samples[4] = (1.0f + sinf(t * 1.6245f + cosf(t * 0.254f) * 0.3f)) * 0.5f;
    samples[5] = (1.0f + sinf(t * 0.345f + cosf(t * 0.03f) * 0.6f)) * 0.5f;

    for (int i = 0; i < 6; i++)
    {
        sx[i] =  i * dx;
        sy[i] = 454 * samples[i] * 0.8f;
        canvas_rectangle_t r = {0};
        NVGcolor color = nvgHSLA(sinf(2.0F * M_PI / 12.0F * 0.12f * i), 1.0f, 0.55f, 255);
        r.fill.color_data.rgba = (uint32_t)((uint32_t)(color.r * 255.0f) << 24) + (uint32_t)((uint32_t)(
                                     color.g * 255.0f) << 16) + (uint32_t)((uint32_t)(color.b * 255.0f) << 8) + (uint32_t)((uint32_t)(
                                                 color.a * 255.0f));
        r.height = 454 - sy[i];
        r.width = 30;
        //r.rx = selector_rx;
        r.x = sx[i] / 2;
        r.y = sy[i];
        gui_canvas_api.rectangle(c, &r);
    }
    samples[0] = (1.0f + sinf(t * 1.0f + cosf(t * 0.33457f) * 0.44f)) * 0.5f;
    samples[1] = (1.0f + sinf(t * 0.8363f + cosf(t * 1.3f) * 1.55f)) * 0.5f;
    samples[2] = (1.0f + sinf(t * 1.642f + cosf(t * 0.33457f) * 1.24f)) * 0.5f;
    samples[3] = (1.0f + sinf(t * 0.345f + cosf(t * 1.63f) * 0.14f)) * 0.5f;
    samples[4] = (1.0f + sinf(t * 0.6245f + cosf(t * 0.254f) * 0.3f)) * 0.5f;
    samples[5] = (1.0f + sinf(t * 1.345f + cosf(t * 0.03f) * 0.6f)) * 0.5f;

    for (int i = 1; i < 6; i++)
    {
        sx[i] =  i * dx;
        sy[i] = 454 * samples[i] * 0.8f;
        canvas_rectangle_t r = {0};
        NVGcolor color = nvgHSLA(sinf(2.0F * M_PI / 12.0F * 0.12f * (i + 6)), 1.0f, 0.55f, 255);
        r.fill.color_data.rgba = (uint32_t)((uint32_t)(color.r * 255.0f) << 24) + (uint32_t)((uint32_t)(
                                     color.g * 255.0f) << 16) + (uint32_t)((uint32_t)(color.b * 255.0f) << 8) + (uint32_t)((uint32_t)(
                                                 color.a * 255.0f));

        r.height = 454 - sy[i];
        r.width = 30;
        //r.rx = selector_rx;
        r.x = sx[i] / 2 + 454 / 2;
        r.y = sy[i];
        gui_canvas_api.rectangle(c, &r);
    }
    /*canvas_wave_t w = {0};
    w.h = 454;
    w.w = 454;
    w.point_count = 6;
    float dx = w.w / 5.0f;
    float samples[w.point_count];
    float sx[w.point_count], sy[w.point_count];

    samples[0] = (1.0f + sinf(t * 1.2345f + cosf(t * 0.33457f) * 0.44f)) * 0.5f;
    samples[1] = (1.0f + sinf(t * 0.68363f + cosf(t * 1.3f) * 1.55f)) * 0.5f;
    samples[2] = (1.0f + sinf(t * 1.1642f + cosf(t * 0.33457f) * 1.24f)) * 0.5f;
    samples[3] = (1.0f + sinf(t * 0.56345f + cosf(t * 1.63f) * 0.14f)) * 0.5f;
    samples[4] = (1.0f + sinf(t * 1.6245f + cosf(t * 0.254f) * 0.3f)) * 0.5f;
    samples[5] = (1.0f + sinf(t * 0.345f + cosf(t * 0.03f) * 0.6f)) * 0.5f;
    int i;
    for (i = 0; i < w.point_count; i++)
    {
        sx[i] = w.x + i * dx;
        sy[i] = w.y + w.h * samples[i] * 0.8f;
    }
    w.point_x = sx;
    w.point_y = sy;
    w.fill.color_data.rgba = 0xeab6a2ff;
    gui_canvas_api.wave(c, &w);*/
}
static void music_draw2(gui_canvas_t *c)
{
    //static float t = 0;

    canvas_arc_t a = {0};
    a.cx = 150 / 2;
    a.cy = 150 / 2;
    a.r = 150 / 2;
    a.stroke.stroke_width = 10;
    a.stroke.fill.color_data.rgba = 0xe8734cff;
    a.start_angle = 0;
    static float angle = 0;
    a.end_angle = angle;
    if (!music_button_click_pause)
    {
        angle += 0.10f;
    }
    if (angle >= 360)
    {
        angle = 0;
    }

    gui_canvas_api.arc(c, &a);



}
static void music_draw3(gui_canvas_t *c)
{


    static float gap = 10;
    gap += 0.3f;
    if (gap >= 20)
    {
        gap = 10;
    }
    uint32_t color[5] = {0xffffff33, 0xffffff55, 0xffffff99, 0xffffffbb, 0xffffffff};
    for (size_t i = 0; i < 5; i++)
    {
        canvas_circle_t a = {0};
        a.cx = 454 / 2;
        a.cy = gap * i;
        a.r = 5;
        if (i == 4)
        {
            a.r = 5 + 2;
            a.cy += (5);
        }


        a.fill.color_data.rgba = color[i];

        gui_canvas_api.circle(c, &a);
    }




}
static void music_draw4(gui_canvas_t *c)
{


    static float gap = 0;
    gap += 0.6f;
    if (gap >= 30)
    {
        gap = 0;
    }
    static float t = 0;
    if (!music_button_click_pause)
    {
        t += 0.05f;
//        gui_img_rotation((void *)music_album_picture, angle * 3.0f, gui_get_screen_width() / 2,
//                         gui_get_screen_height() / 2);
    }
    float dx = 50.0f / 6.0f;
    float samples[6];
    float sx[6], sy[6];

    samples[0] = (1.0f + sinf(t * 1.2345f + cosf(t * 0.33457f) * 0.44f)) * 0.5f;
    samples[1] = (1.0f + sinf(t * 0.68363f + cosf(t * 1.3f) * 1.55f)) * 0.5f;
    samples[2] = (1.0f + sinf(t * 1.1642f + cosf(t * 0.33457f) * 1.24f)) * 0.5f;
    samples[3] = (1.0f + sinf(t * 0.56345f + cosf(t * 1.63f) * 0.14f)) * 0.5f;
    samples[4] = (1.0f + sinf(t * 1.6245f + cosf(t * 0.254f) * 0.3f)) * 0.5f;
    samples[5] = (1.0f + sinf(t * 0.345f + cosf(t * 0.03f) * 0.6f)) * 0.5f;

    for (int i = 0; i < 6; i++)
    {
        sx[i] =  i * dx;
        sy[i] = 50 * samples[i] * 0.8f;
        canvas_rectangle_t r = {0};
        r.fill.color_data.rgba = 0xffffffff;
        r.height = 50 - sy[i];
        r.width = 3;
        //r.rx = selector_rx;
        r.x = sx[i] / 2 + 454 / 2 - 15;
        r.y = sy[i] + 454 - 50 - gap;
        gui_canvas_api.rectangle(c, &r);
    }
}
static void music(gui_obj_t *screen)
{
    gui_curtainview_t *cv = gui_curtainview_create(screen, "cvmusic", 0, 0, 0, 0);
    gui_curtain_t *c1 = gui_curtain_create(cv, "c1", 0, 0, 0, 0, CURTAIN_MIDDLE, 1);
    gui_curtain_t *c2 = gui_curtain_create(cv, "c2", 0, 0, 0, 0, CURTAIN_UP, 1);
    gui_curtain_t *c3 = gui_curtain_create(cv, "c3", 0, 0, 0, 0, CURTAIN_DOWN, 1);

    music_album_picture = (void *)gui_svg_create_from_mem(c2, MUSIC_SVG, 2746, (454 - 150) / 2,
                                                          20, 150, 150);
    //music_album_picture = (void *)gui_magic_img_create_from_mem(screen, "1", CHARGE52_BIN,
    //                                                            (454 - 300) / 2, (454 - 300) / 2, 300, 300);
    //gui_img_scale((void *)music_album_picture, 300.0f / 112.0f, 300.0f / 200.0f);
    gui_canvas_t *canvas = gui_canvas_create(c3, "c", 0, 0, 454, 454, 0);
    canvas->draw = music_draw;
    gui_canvas_t *canvas2 = gui_canvas_create(music_album_picture, "c", 0, 0, 454, 454, 0);
    canvas2->draw = music_draw2;
    gui_canvas_t *canvas3 = gui_canvas_create(c2, "c", 0, 454, 454, 454, 0);
    canvas3->draw = music_draw3;
    gui_canvas_t *canvas4 = gui_canvas_create(c3, "c", 0, -454, 454, 454, 0);
    canvas4->draw = music_draw4;
    gui_button_t *music_button, *music_button_previous, *music_button_next;
    music_button_previous = gui_button_create(screen, 38 + 0 * (100 + 38), 454 / 2 - 50, 100, 100,
                                              BACK_SVG, NULL, NULL, 2,
                                              966);
    music_button = gui_button_create(screen, 38 + 1 * (100 + 38), 454 / 2 - 50, 100, 100, PALY2_SVG,
                                     NULL, NULL, 2,
                                     610);
    music_button_next = gui_button_create(screen, 38 + 2 * (100 + 38), 454 / 2 - 50, 100, 100,
                                          NEXT2_SVG,
                                          NULL, NULL, 2,
                                          1048);
    //gui_button_click(sport_button, (gui_event_cb_t)text_color_cb);
    //gui_button_click(hr_button, (gui_event_cb_t)hr_button_cb);
    //gui_button_click(cube_button, (gui_event_cb_t)cube_button_cb);
    gui_button_api.onPress(music_button, music_button_press, music_button);
    gui_button_api.onRelease(music_button, music_button_pause_release, music_button);
    gui_button_api.onPress(music_button_previous, music_button_press, music_button_previous);
    gui_button_api.onRelease(music_button_previous, music_button_release, music_button_previous);
    gui_button_api.onPress(music_button_next, music_button_press, music_button_next);
    gui_button_api.onRelease(music_button_next, music_button_release, music_button_next);
    gui_button_api.onClick(music_button, music_button_click, music_button);
//    gui_text_t *text = gui_text_create(screen, "icon_text", 100, 40, 454,
//                                       RTK_GUI_DEFAULT_FONT_SIZE * 7);
//    char *string = "f04short_raw.mp4";
//    gui_text_set(text, string, "rtk_font_stb", 0xffffffff, strlen(string),
//                 RTK_GUI_DEFAULT_FONT_SIZE);
}

static float angle_to_center(float x0, float y0, float x1, float y1)
{
    float vx = x1 - x0;
    float vy = y1 - y0;
    float mag_v = sqrt(vx * vx + vy * vy);
    return acos(vy / mag_v);
}
float selector_radian_old;
static void palette_wheel_draw(gui_canvas_t *c)
{
    canvas_palette_wheel_t pw = {0};
    touch_info_t *tp = (void *)tp_get_info();
    static bool pressing;
    static float selector_radian;
    if (!pressing && tp->pressed)
    {
        pressing = true;
    }
    if (pressing && tp->released)
    {
        pressing = false;
    }
    if (pressing)
    {
        //selector_radian += 0.1f;
        //if(l2&&l1)

        /* code */
        switch (tp->type)
        {
        //gui_log("tp->type:%d\n",tp->type);
        case TOUCH_HOLD_X:
            break;
        default:
            {
                float degree = angle_to_center((float)(454 / 2), (float)(454 / 2), (float)(tp->x + tp->deltaX),
                                               (float)(tp->y + tp->deltaY)) * 360.0f / 2.0f / 3.14159f;
                if (tp->x + tp->deltaX > 454 / 2)
                {
                    degree =  -degree + 360.0f;
                }
                degree += 90.0f;
                if (degree > 360.0f)
                {
                    degree -= 360.0f;
                }
                degree = degree * M_PI / 180.0f * 1.8f;
                //degree = degree*M_PI/180.0f;
                pw.selector_radian = degree;
                selector_radian = pw.selector_radian;


                //gui_log("%f,%d,%d\n", degree,  tp->x + tp->deltaX, tp->y + tp->deltaY);
            }




            break;
        }


    }
    else
    {
        pw.selector_radian = selector_radian;
        selector_radian_old = selector_radian;
    }





    //pw.selector_radian = 2;
    pw.w = gui_get_screen_width();
    pw.h = gui_get_screen_height();
    gui_canvas_api.palette_wheel(c, &pw);
}
static void palette_wheel(gui_obj_t *screen)
{
    gui_canvas_t *canvas = gui_canvas_create(screen, "c", 0, 0, 454, 454, 0);
    canvas->draw = palette_wheel_draw;

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
    watchface1(c1);
    sport_chart((void *)tb1);
    curtain1(c3);
    curtain2(c2);
    grid_icon(tb2);
    heart_rate_chart((void *)tb4);
    read_page(tb7);
    create_selector((void *)tb6);
    music((void *)tb5);
    palette_wheel((void *)tb3);
}
