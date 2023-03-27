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

#include "gui_magic_img.h"

static void watchface1(void *parent)
{
    gui_win_t *win = gui_win_create(parent, "win", 0, 0, 454, 454);


#ifdef MODULE_VG_LITE
    gui_vg_lite_clock_t *path_clock = gui_vg_lite_clock_create(parent,  "path_clock", NULL, 0, 0,
                                                               454, 454);
#else
    gui_button_create(parent, 252, 57, 100, 100, ACTIVITY_SVG, NULL, NULL, 2, 5134);
#endif

}
static void watchface2(void *parent)
{
//   gui_magic_img_create_from_mem(parent, "sun", PATH850_BIN, 0, 0, 0, 0);
}
static float  end_angle1, end_angle2, end_angle3;
void static sport_chart_draw(gui_canvas_t *c)
{
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
static void sport_chart_cb(gui_win_t *win)
{
    float pro = win->animate->progress_percent;
    static bool end_flag;
    if (pro == 1.0f)
    {
        end_flag = !end_flag;
    }
    if (!(pro == 1.0f || pro == 0.0f))
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
    }


}
static void sport_chart(gui_obj_t *parent)
{
    gui_win_t *win = gui_win_create(parent, "win", 0, 0, 454, 454);
    gui_canvas_t *c = gui_canvas_create(parent, "c", 0, 0, 454, 454, 0);
    c->draw = sport_chart_draw;
    win->set_animate(win, 2000, -1, sport_chart_cb, win);
}
void static hr_chart_draw(gui_canvas_t *c)
{

//gui_canvas_api.svg(c, music_big, 1866, 0, 0, 1);


}
static void *hr_video_array[] =
{
    A1_SVG, A2_SVG, A3_SVG, A4_SVG, A5_SVG, A6_SVG,
};

static void heart_rate_chart(gui_obj_t *parent)
{
    gui_win_t *win = gui_win_create(parent, "awin", 0, 0, 454, 454);

    gui_cube_create(parent, "cube", CHARGE10_BIN, 0, 0, 454, 454);

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
gui_button_t *sport_button;
gui_button_t *cube_button;
gui_button_t *hr_button;
gui_tabview_t *tv;

static void text_color_cb(gui_event_t e)
{

    gui_tab_jump(tv, 1, 0);
}
static void cube_button_cb(gui_event_t e)
{

    gui_tab_jump(tv, 0, 1);
}
static void hr_button_cb(gui_event_t e)
{

    gui_tab_jump(tv, 2, 0);
}
static void curtain1(void *curtian)
{
    gui_canvas_t *canvas1 = gui_canvas_create(curtian, "canvas1", 0, 0, 454, 454, 0);
    canvas1->draw = canvas1_draw;
    static void *charge_video_array[] =
    {
        CHARGE2_BIN, CHARGE4_BIN, CHARGE6_BIN, CHARGE8_BIN, CHARGE10_BIN, CHARGE12_BIN, CHARGE14_BIN, CHARGE16_BIN, CHARGE18_BIN, CHARGE20_BIN, CHARGE22_BIN, CHARGE24_BIN, CHARGE26_BIN, CHARGE28_BIN, CHARGE30_BIN, CHARGE32_BIN, CHARGE34_BIN, CHARGE36_BIN, CHARGE38_BIN, CHARGE40_BIN, CHARGE42_BIN, CHARGE44_BIN, CHARGE46_BIN, CHARGE48_BIN, CHARGE50_BIN, CHARGE52_BIN, CHARGE54_BIN, CHARGE56_BIN, CHARGE58_BIN, CHARGE60_BIN, CHARGE62_BIN, CHARGE64_BIN, CHARGE66_BIN, CHARGE68_BIN, CHARGE70_BIN, CHARGE72_BIN, CHARGE74_BIN, CHARGE76_BIN, CHARGE78_BIN, CHARGE80_BIN, CHARGE82_BIN, CHARGE84_BIN, CHARGE86_BIN, CHARGE88_BIN, CHARGE90_BIN, CHARGE92_BIN, CHARGE94_BIN, CHARGE96_BIN, CHARGE98_BIN, CHARGE100_BIN, CHARGE102_BIN, CHARGE104_BIN, CHARGE106_BIN, CHARGE108_BIN, CHARGE110_BIN, CHARGE112_BIN, CHARGE114_BIN, CHARGE116_BIN, CHARGE118_BIN, CHARGE120_BIN,
    };
    void *charge_img = (void *)gui_dynamic_create_from_mem((void *)curtian, "g", charge_video_array,
                                                           470 - 120 * 2, 0, 59, 40, INT32_MAX);
    gui_img_scale(charge_img, 2, 2);
}
static void curtain2(void *curtian)
{
    gui_canvas_t *canvas1 = gui_canvas_create(curtian, "canvas2", 0, 0, 454, 454, 0);
    canvas1->draw = canvas2_draw;


    sport_button = gui_button_create(curtian, 252, 57, 100, 100, ACTIVITY_SVG, NULL, NULL, 2,
                                     5134);
    hr_button = gui_button_create(curtian, 265, 205, 150, 150, SETTING_SVG, NULL, NULL, 2,
                                  2056);
    cube_button = gui_button_create(curtian, 5, 108, 250, 250, MESSAGE_SVG, NULL, NULL, 2,
                                    2666);
    gui_button_click(sport_button, (gui_event_cb_t)text_color_cb);
    gui_button_click(hr_button, (gui_event_cb_t)hr_button_cb);
    gui_button_click(cube_button, (gui_event_cb_t)cube_button_cb);


}
static void grid_icon(void *parent)
{
    void *svg_array[] = {ACTIVITY_SVG, ALARM_SVG, HEARTRATE_SVG, MESSAGE_SVG, MUSIC_SVG, SETTING_SVG, CONTACTS_SVG, TELEPHONE_SVG, TIME_SVG, WEATHER_SVG, SLEEP_SVG};
    void *svg_size_array[] = {5134, 2980, 2890, 2666, 2746, 2056,  3052, 2882, 2097, 3629, 2539};
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
static void text_a(gui_text_t *text)
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
    gui_text_size_set(text, width, width);

}
static void read_page(void *parent)
{
    static char *t = "R";
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
    gui_text_t *text = gui_text_create(parent, "icon_text", 150, 200, RTK_GUI_DEFAULT_FONT_SIZE * 7,
                                       RTK_GUI_DEFAULT_FONT_SIZE * 7);
    gui_text_set(text, t, "rtk_font_stb", BUTTON_COLOR, strlen(t),
                 RTK_GUI_DEFAULT_FONT_SIZE * 7);
    gui_text_set_animate(text, 4000, -1, text_a, text);

}
static void app_launcher2_ui_design(gui_app_t *app)
{
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
    heart_rate_chart((void *)tb3);
    read_page(tb4);


}
