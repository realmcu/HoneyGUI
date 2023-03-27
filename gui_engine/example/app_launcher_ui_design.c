#include "app_launcher.h"
#include "draw_font.h"
#include <time.h>
/*----------------->x
|
|
|
|
|
V
y*/
gui_tabview_t *tabview_0;
static uint16_t icon_width = 65;
static uint16_t icon_hight = 65;
static const char *string;
static const char *iconlist_parent_name = "iconlist_window1";

static void design_curtain_middle(gui_curtainview_t *curtainview);
static void design_curtain_up(gui_curtainview_t *curtainview);
static void design_curtain_left(gui_curtainview_t *curtainview);
static void design_tab_center(gui_tabview_t *tabview);
static void design_tab_right1(gui_tabview_t *tabview);
static void design_tab_right2(gui_tabview_t *tabview);
static void design_tab_right2(gui_tabview_t *tabview);
static void design_tab_down1(gui_tabview_t *tabview);
static void create_clock(gui_obj_t *parent);





static gui_app_t app_launcher =
{
    .screen =
    {
        .name = "app_launcher",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = design_launcher_ui,
};

gui_app_t *get_launcher_app(void)
{
    return &app_launcher;
}

void launcher_draw_circle(gui_canvas_t *c)
{
    canvas_circle_t circle = {0};
    circle.cx = 160;
    circle.cy = 160;
    //circle.fill.color_data.rgba = 0xffff00ff;
    circle.r = 50;
    circle.stroke.fill.color_data.rgba = 0xff00ffaa;
    circle.stroke.stroke_width = 10;
    gui_canvas_api.circle(c, &circle);
    canvas_rectangle_t r = {0};
    r.x = 90;
    r.y = 90;
    r.width = 100;
    r.height = 90;
    r.fill.color_data.rgba = 0xff00ff88;
    gui_canvas_api.rectangle(c, &r);

    canvas_arc_t a = {0};
    a.cx = 80;
    a.cy = 80;
    a.r = 50;
    a.start_angle = 10;
    a.end_angle = 300;
    //a.fill.color_data.rgba = 0x00ffffff;
    a.stroke.fill.color_data.rgba = 0xffffff;
    a.stroke.stroke_width = 10;
    a.stroke.stroke_linecap = ROUND;
    gui_canvas_api.arc(c, &a);

    canvas_line_t l = {0};
    l.x1 = 100;
    l.y1 = 100;
    l.x2 = 300;
    l.y2 = 150;
    l.stroke.stroke_width = 30;
    l.stroke.fill.color_data.rgba = 0xffffff88;
    gui_canvas_api.line(c, &l);

}

void design_launcher_ui(gui_app_t *app)
{
    gui_set_font_mem_resourse(32, GBK_32_32_DOT_BIN, GBK_UNICODE_TABLE_BIN);
    gui_set_font_mem_resourse(24, GBK_24_24_DOT_BIN, GBK_UNICODE_TABLE_BIN);
    gui_font_stb_init(TANGYUANTI_TTF);
    gui_obj_t *screen = &app->screen;
    tabview_0 = gui_tabview_create(screen, "tabview", 0, 0, 0, 0);
    /****create middle tab  idx=0,idy=0;watch page*/
    design_tab_center(tabview_0);
    /****create right 1 tab  idx=1,idy=0;Today activity page*/
    design_tab_right1(tabview_0);
    /***create right 2 tab  idx=2,idy=0;Heart rate page*/
    design_tab_right2(tabview_0);
    /****create down 1 tab  idx=0,idy=1;Icon list page*/
    design_tab_down1(tabview_0);


}
static void design_curtain_middle(gui_curtainview_t *curtainview)
{
    uint16_t x = 0;
    uint16_t y = 0;
    gui_app_t *launcher_app = get_launcher_app();
    gui_curtain_t *curtain_center = gui_curtain_create(curtainview, "curtain_center", 0, 0,
                                                       0, 0, CURTAIN_MIDDLE, 1);
    /*create image*/
    gui_img_t *image_center_icon_0 = gui_img_create_from_mem(curtain_center,  "center", WCH6_BG_BIN, 0,
                                                             0, 0, 0);
    x = (gui_get_screen_width() - gui_img_get_width(image_center_icon_0)) / 2;
    y = (gui_get_screen_height() - gui_img_get_height(image_center_icon_0)) / 2;
    gui_img_set_location(image_center_icon_0, x, y);

    gui_img_set_mode(image_center_icon_0, IMG_BYPASS_MODE);
    gui_img_t *image_center_icon_1 = gui_img_create_from_mem(curtain_center,  "center",
                                                             WCH6_POINTARROW_BIN, 128,
                                                             128, 0, 0);
    x = (gui_get_screen_width() - gui_img_get_width(image_center_icon_1)) / 2;
    y = (gui_get_screen_height() - gui_img_get_height(image_center_icon_1)) / 2;
    gui_img_set_location(image_center_icon_1, x, y);
    create_clock((void *)curtain_center);
    /*gui_canvas_t *c = gui_canvas_create(curtain_center, "1", 0, 0, 300, 300, 0xffff);
    c->draw = launcher_draw_circle;*/
}
static void design_curtain_up(gui_curtainview_t *curtainview)
{
    gui_curtain_t *curtain_up = gui_curtain_create(curtainview, "curtain_up", 0, 0, 0, 0, CURTAIN_UP,
                                                   0.5f);
    gui_img_t *image_up = gui_img_create_from_mem(curtain_up, "curtain",
                                                  CURTAIN_HALF_BACKGROUND_90_BIN,
                                                  0, 160, 0, 0);
    gui_img_set_mode(image_up, IMG_BYPASS_MODE);
    gui_img_t *image_up_icon_1 = gui_img_create_from_mem(curtain_up, "curtain",
                                                         ICMENUNOTIFICATION_BIN, 40, 165,
                                                         0, 0);
    gui_img_t *image_up_icon_2 = gui_img_create_from_mem(curtain_up, "curtain", ICPHONEVIBRATION_BIN,
                                                         127, 165,
                                                         0, 0);
    gui_img_t *image_up_icon_3 = gui_img_create_from_mem(curtain_up, "curtain",
                                                         ICMENULIGHTCONTROL_BIN, 215, 165,
                                                         0, 0);
    gui_img_t *image_up_icon_4 = gui_img_create_from_mem(curtain_up, "curtain", ICMENUMUSIC_BIN,
                                                         40, 250, 0,
                                                         0);
}
static void design_curtain_left(gui_curtainview_t *curtainview)
{
    gui_app_t *launcher_app = get_launcher_app();
    gui_curtain_t *curtain_left = gui_curtain_create(curtainview, "curtain_left", 0, 0, 0, 0,
                                                     CURTAIN_LEFT,
                                                     0.5f);
    gui_img_t *image_curtain_left = gui_img_create_from_mem(curtain_left, "curtain",
                                                            CURTAIN_BACKGROUND_TRANSPARENT_BIN, 0, 0, 0, 0);
    gui_img_set_mode(image_curtain_left, IMG_BYPASS_MODE);
    gui_icon_t *left_icon_album = gui_icon_create(curtain_left, "left_icon_0", 170, 10,
                                                  icon_width,
                                                  icon_hight, 0);
    gui_icon_t *left_icon_setting = gui_icon_create(curtain_left, "left_icon_1", 245, 10,
                                                    icon_width,
                                                    icon_hight, 0);
    gui_icon_t *left_icon_flappybird = gui_icon_create(curtain_left, "left_icon_2", 170,
                                                       85, icon_width,
                                                       icon_hight, 0);
    gui_icon_t *left_icon_heartrate = gui_icon_create(curtain_left, "left_icon_3", 245,
                                                      85, icon_width,
                                                      icon_hight, 0);
    gui_icon_t *left_icon_iconbox = gui_icon_create(curtain_left, "left_icon_4", 170, 165,
                                                    icon_width * 2,
                                                    icon_hight, 0);
    gui_icon_t *left_icon_btcall = gui_icon_create(curtain_left, "left_icon_5", 170, 245,
                                                   icon_width * 2,
                                                   icon_hight, 0);
    gui_img_t *image_icon_album = gui_img_create_from_mem(left_icon_album, "icon", ICMENUALBUM_BIN, 0,
                                                          0, 0,
                                                          0);
    gui_img_t *image_icon_setting = gui_img_create_from_mem(left_icon_setting, "icon",
                                                            ICMENUSETTING_BIN, 0,
                                                            0, 0, 0);
    gui_img_t *image_icon_flappybird = gui_img_create_from_mem(left_icon_flappybird, "icon",
                                                               ICMENUBIRD_BIN,
                                                               0, 0,
                                                               0, 0);
    gui_img_t *image_icon_heartrate = gui_img_create_from_mem(left_icon_heartrate, "icon",
                                                              ICMENUHEARTRATE_BIN, 0, 0, 0, 0);
    gui_img_t *image_icon_iconbox = gui_img_create_from_mem(left_icon_iconbox, "icon",
                                                            ICMENUMASSAGEBOX_BIN,
                                                            0, 0, 0, 0);
    gui_img_t *image_icon_btcall = gui_img_create_from_mem(left_icon_btcall, "icon", ICMENUBTCALL_BIN,
                                                           0, 0,
                                                           0, 0);
    gui_obj_add_event_cb(left_icon_album, album_icon_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    gui_obj_add_event_cb(left_icon_setting, setting_icon_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    gui_obj_add_event_cb(left_icon_flappybird, flappybird_iocn_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    gui_obj_add_event_cb(left_icon_heartrate, heartrate_icon_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    gui_obj_add_event_cb(left_icon_iconbox, iconbox_icon_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
    gui_obj_add_event_cb(left_icon_btcall, btcall_icon_cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
}
static void design_tab_center(gui_tabview_t *tabview)
{
    gui_app_t *launcher_app = get_launcher_app();
    gui_tab_t *tab_center = gui_tab_create(tabview_0, "tab_center", 0, 0, 0, 0, 0,
                                           0);
    {
        /*create curtainview*/
        gui_curtainview_t *curtainview = gui_curtainview_create(tab_center, "curtainview", 0, 0, 0,
                                                                0);
        design_curtain_middle(curtainview);
        design_curtain_up(curtainview);
        design_curtain_left(curtainview);
    }
}
static void design_tab_right1(gui_tabview_t *tabview)
{
    gui_app_t *launcher_app = get_launcher_app();
    gui_tab_t *tab1 = gui_tab_create(tabview, "tab1", 0, 0, 0, 0, 1, 0);
    /*******create widgets in tab1*/
    gui_img_t *image1_0 = gui_img_create_from_mem(tab1, "icon", ICTODAYACTIVITY_BIN, 0, 0, 0, 0);
    gui_img_set_mode(image1_0, IMG_BYPASS_MODE);
    gui_img_t *image_tab1_icon_0 = gui_img_create_from_mem(tab1, "icon", ICACTIVITYSTEP_BIN, 60, 80,
                                                           0, 0);
    /*create text box start*/
    gui_text_t *text_0 = gui_text_create(tab1, "text_0", 115, 84, 240, 60);
    string = "3000 steps";
    gui_text_set(text_0, string, "rtk_font_mem32", 0xffff, strlen(string), 32);
    /*create text box end*/
    gui_img_t *image_tab1_icon_1 = gui_img_create_from_mem(tab1, "icon", ICACTIVITYCAL_BIN, 60, 140,
                                                           0, 0);
    gui_text_t *text_1 = gui_text_create(tab1, "text_1", 115, 144, 240, 60);
    string = "1000 kcal";
    gui_text_set(text_1, string, "rtk_font_mem32", 0xffff, strlen(string), 32);
    gui_img_t *image_tab1_icon_2 = gui_img_create_from_mem(tab1, "icon", ICACTIVITYSTAND_BIN, 60, 200,
                                                           0,
                                                           0);
    gui_text_t *text_2 = gui_text_create(tab1, "text_2", 115, 204, 240, 60);
    string = "5 hour";
    gui_text_set(text_2, string, "rtk_font_mem32", 0xffff, strlen(string), 32);

}
static void design_tab_right2(gui_tabview_t *tabview)
{
    gui_app_t *launcher_app = get_launcher_app();
    gui_tab_t *tab2 = gui_tab_create(tabview, "tab2", 0, 0, 0, 0, 2, 0);
    gui_img_t *image2_0 = gui_img_create_from_mem(tab2, "icon", ICHEARTRATE_BIN, 0, 0, 0, 0);
    gui_img_set_mode(image2_0, IMG_BYPASS_MODE);
    gui_img_t *image2_1 = gui_img_create_from_mem(tab2, "icon", ICHEART42_BIN, 80, 45, 0, 0);
    gui_text_t *text_heartrate_0 = gui_text_create(tab2, "text_heartrate_0", 140, 50, 240, 60);
    string = "70";
    gui_text_set(text_heartrate_0, string, "rtk_font_mem32", 0xffff,
                 strlen(string), 32);
    gui_img_t *image2_2 = gui_img_create_from_mem(tab2, "icon", ICHRMAX_BIN, 60, 270, 0, 0);
    gui_text_t *text_heartrate_1 = gui_text_create(tab2, "text_heartrate_1", 90, 264, 240, 60);
    string = "125";
    gui_text_set(text_heartrate_1, string, "rtk_font_mem32", 0xffff,
                 strlen(string), 32);
    gui_img_t *image2_3 = gui_img_create_from_mem(tab2, "icon", ICHRMIN_BIN, 200, 270, 0, 0);
    gui_text_t *text_heartrate_2 = gui_text_create(tab2, "text_heartrate_2", 230, 264, 240, 60);
    string = "56";
    gui_text_set(text_heartrate_2, string, "rtk_font_mem32", 0xffff,
                 strlen(string), 32);
}

struct iocnlist_param_theme0
{
    const char *icon_name;
    void *image_src;
    const char *image_name;
    gui_event_cb_t cb;
    const char *text_name;
    const char *text_box_string;
};
static struct iocnlist_param_theme0 config[12] =
{
    {"icon_appbox_0", ICMENUBIRD_BIN, "image_appbox_0", flappybird_iocn_cb, "text_appbox_0", "FlappyBird"},
    {"icon_appbox_1", ICMENUHEARTRATE_BIN, "image_appbox_1", heartrate_icon_cb, "text_appbox_1", "HeartRate"},
    {"icon_appbox_2", ICMENUALBUM_BIN, "image_appbox_2", album_icon_cb, "text_appbox_2", "Album"},
    {"icon_appbox_3", ICMENUSETTING_BIN, "image_appbox_3", setting_icon_cb, "text_appbox_3", "Setting"},
    {"icon_appbox_4", ICMENUALARM_BIN, "image_appbox_4", home_icon_cb, "text_appbox_4", "Home"},
    {"icon_appbox_5", ICMENUSTOPWATCH_BIN, "image_appbox_5", NULL, "text_appbox_5", "Stopwatch"},
    {"icon_appbox_6", ICMENUSPORT_BIN, "image_appbox_6", NULL, "text_appbox_6", "Sport"},
    {"icon_appbox_7", ICMENUSLEEP_BIN, "image_appbox_7", NULL, "text_appbox_7", "Sleep"},
    {"icon_appbox_8", ICMENUTIMER_BIN, "image_appbox_8", NULL, "text_appbox_8", "Timer"},
    {"icon_appbox_9", ICMENUSPPORTRECORD_BIN, "image_appbox_9", NULL, "text_appbox_9", "SportRecord"},
    {"icon_appbox_10", ICMENUWEATHER_BIN, "image_appbox_10", NULL, "text_appbox_10", "Weather"},
    {"icon_appbox_11", ICMENUMUSIC_BIN, "image_appbox_11", NULL, "text_appbox_11", "Music"},
};
void create_icon_list_theme0(void *parent)
{
    gui_app_t *launcher_app = get_launcher_app();
    /*  list style  */
    gui_iconlist_t *tab_iconlist = gui_iconlist_create(parent, "iconlist_0", 0, 0, 0, 0, NULL, NULL);
    tab_iconlist->set_background(tab_iconlist, ICONBACK_BIN, ICONBACKHIGHLIGHT_BIN);
    for (size_t i = 0; i < 12; i++)
    {
        gui_icon_t *icon_appbox = gui_icon_create(tab_iconlist, config[i].icon_name, 0, 0,
                                                  icon_width,
                                                  icon_hight, i);
        gui_img_t *image_appbox = gui_img_create_from_mem(icon_appbox, config[i].image_name,
                                                          config[i].image_src, 0, 0, 0, 0);
        if (config[i].cb)
        {
            gui_obj_add_event_cb(icon_appbox, config[i].cb, GUI_EVENT_TOUCH_CLICKED,  NULL);
        }
        gui_text_t *text_appbox = gui_text_create(icon_appbox, config[i].text_name, 110, 20, 240, 40);
        string = config[i].text_box_string;
        gui_text_set(text_appbox, string, "rtk_font_mem32", 0xffff,
                     strlen(string), 32);
    }
}

static void design_tab_down1(gui_tabview_t *tabview)
{
    gui_app_t *launcher_app = get_launcher_app();
    gui_tab_t *tab_down1 = gui_tab_create(tabview_0, "tab_down1", 0, 0, 0, 0, 0,
                                          1);
    gui_win_t *iconlist_window1 = gui_win_create(tab_down1, iconlist_parent_name,
                                                 0, 0, 320, 320);
    gui_obj_add_event_cb(iconlist_window1, iconlist_win_cb, GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE,  NULL);
    /*create iconlist*/
    switch (gui_iocnlist_get_theme((char *)(iconlist_window1->base.name)))
    {
    case ICON_LIST_THEME_0:
        create_icon_list_theme0(iconlist_window1);
        break;
    case ICON_LIST_THEME_1:
        //create_icon_list_theme1(iconlist_window1);
        break;
    case ICON_LIST_THEME_2:
        //create_icon_list_theme2(iconlist_window1);
        break;
    case ICON_LIST_THEME_3:
        //create_icon_list_theme3(iconlist_window1);
        break;
    default:
        break;
    }
}
struct img_p
{
    gui_magic_img_t *arc1;
    gui_magic_img_t *arc2;
    gui_magic_img_t *arc3;
    int img1w;
    int img2w;
    int img3w;
};

static uint16_t hour_width, hour_center_height, minute_width, minute_center_height,
       second_width, second_center_height;
static uint16_t screen_width, screen_hight;
static char t[10][100];
static float hour_hand_degree, minute_hand_degree, second_hand_degree;
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
        sprintf(t[6], "%s%d", "Weak", timeptr->tm_wday);
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
static void clock_cb(void *p)
{
    struct img_p *clock = p;
    get_date();
    gui_img_rotation(clock->arc1, hour_hand_degree, hour_width / 2, hour_center_height);
    gui_img_rotation(clock->arc2, minute_hand_degree, minute_width / 2, minute_center_height);
    gui_img_rotation(clock->arc3,
                     second_hand_degree, second_width / 2,
                     second_center_height);



}

static void create_clock(gui_obj_t *parent)
{
    static struct img_p rcr3img;
    rcr3img.img1w = 28;
    rcr3img.img2w = 30;
    rcr3img.img3w = 4;
    hour_width =                 28;
    hour_center_height =         92;
    minute_width =               30;
    minute_center_height =       132;
    second_width =               4;
    second_center_height =       141;
    screen_width = gui_get_screen_width();
    screen_hight = gui_get_screen_height();
    gui_log("WCH6_ARRHOUR0_FINE_BIN:%p\n", WCH6_ARRHOUR0_FINE_BIN);
    rcr3img.arc1 = gui_magic_img_create_from_mem(parent, "hour", WCH6_ARRHOUR0_FINE_BIN,
                                                 (screen_width - hour_width) / 2, screen_hight / 2 - hour_center_height, 0, 0);
    rcr3img.arc2 = gui_magic_img_create_from_mem(parent, "minute", WCH6_ARRMIN0_FINE_BIN,
                                                 (screen_width - minute_width) / 2, screen_hight / 2 - minute_center_height, 0, 0);
    rcr3img.arc3 = gui_magic_img_create_from_mem(parent, "second", WCH6_ARRSEC0_FINE_BIN,
                                                 (screen_width - second_width) / 2, screen_hight / 2 - second_center_height, 0, 0);
    get_date();
    gui_img_rotation(rcr3img.arc1, hour_hand_degree, hour_width / 2, hour_center_height);
    gui_img_rotation(rcr3img.arc2, minute_hand_degree, minute_width / 2, minute_center_height);
    gui_img_rotation(rcr3img.arc3,
                     second_hand_degree, second_width / 2,
                     second_center_height);
    gui_win_t *win = gui_win_create(parent, "window_for_animate", 0, 0, 0, 0);
    win->set_animate(win, 1000 * 60, -1, clock_cb, &rcr3img);

}
