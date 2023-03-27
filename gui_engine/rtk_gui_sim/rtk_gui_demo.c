#if 1
#include "guidef.h"
#include "gui_img.h"
#include "gui_magic_img.h"
#include "gui_dynamic_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>
#include "rtk_gui_resource.h"
#include "gui_text.h"
#include "gui_scroll_text.h"
#include "gui_stb_img.h"
#include "gui_button.h"
#include "gui_scroll_wheel.h"
#include "gui_page.h"
#include "gui_canvas.h"

gui_app_t *get_rtk_gui_demo(void);

static void watch_bg_event_cb(gui_event_t e)
{
    gui_log("e = %d \n", e);
    if (e == GUI_EVENT_TOUCH_CLICKED)
    {
        gui_tree_free(&get_rtk_gui_demo()->screen);
    }
    else if (e == GUI_EVENT_KB_UP_PRESSED)
    {
        gui_log("GUI_EVENT_KB_UP = %d \n", __LINE__);
    }

}

// static void *addr_list[] =
// {
//     NUMBER_0_BIN,
//     NUMBER_1_BIN,
//     NUMBER_2_BIN,
//     NUMBER_3_BIN,
//     NUMBER_4_BIN,
//     NUMBER_5_BIN,
//     NUMBER_6_BIN,
//     NUMBER_7_BIN,
// };
static char str_arrived[] = {0xE5, 0x88, 0xB0, 0xE8, 0xBE, 0xBE};
static char stb_text[] = {0x53, 0x54, 0x42};
static char stb_text1[] = {0xe4, 0xbc, 0x80};
static char str_test[] = "000\n111\n222\n333\n444";

static void gui_font_init()
{
    // gui_set_font_mem_resourse(32, GBK_32_32_DOT_BIN, GBK_UNICODE_TABLE_BIN);
    // gui_set_font_mem_resourse(24, GBK_24_24_DOT_BIN, GBK_UNICODE_TABLE_BIN);
    gui_font_stb_init(TANGYUANTI_TTF);
}
static gui_button_t *button;
static void text_color_cb(gui_event_t e)
{
    if (button->text->color != 0x0bffff)
    {
        gui_button_text_color(button, 0x0bffff);
    }
    else
    {
        gui_button_text_color(button, 0xff6347);
    }
}
void static sport_chart_draw(gui_canvas_t *canvas)
{
#if 0
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
    gui_canvas_api.arc(canvas, &arc1);

    canvas_circle_t ciacle =
    {
        .cx = canvas->base.base.base.dx,
        .cy = canvas->base.base.base.dy,
        .fill.color_data.rgba = 0xffff0099,
        .r = 50,
    };
    gui_get_acc()->draw_circle(&ciacle, gui_get_dc());

    canvas_line_t line =
    {
        .x1 = 100,
        .x2 = 200,
        .y1 = 100,
        .y2 = 100,
        .stroke.fill.color_data.rgba = 0xffffff88,
        .stroke.stroke_width = 1,
    };
    gui_get_acc()->draw_line(&line, gui_get_dc());

    canvas_line_t line1 =
    {
        .x1 = 100,
        .x2 = 200,
        .y1 = 200,
        .y2 = 200,
        .stroke.fill.color_data.rgba = 0xffffff88,
        .stroke.stroke_width = 1,
    };
    gui_get_acc()->draw_line(&line1, gui_get_dc());
#endif
}
static void app_dialing_ui_design(gui_app_t *app)
{
    gui_font_init();

    gui_img_t *img = gui_img_create_from_mem(&app->screen,  "center", CHARGE10_BIN, 0, 0, 0, 0);

    gui_log("%d \n", gui_img_get_width(img));
    gui_log("%d \n", gui_img_get_height(img));

    // gui_scroll_wheel_t *wheel = gui_scroll_wheel_create(&(app->screen), TELEPHONE_BIN, 50, 50, 100, 100);
    // gui_scrollwheel_append(wheel, SLEEP_BIN, SPORT_BIN);
    // gui_scrollwheel_append(wheel, STEP_BIN, SPORT_BIN);
    // gui_scrollwheel_append(wheel, TELEPHONE_BIN, SPORT_BIN);


    // gui_img_t *img_num_0 = gui_img_create_from_mem(&app->screen,  "img_num_0", NUMBER_0_BIN, 0, 0, 0,
    //                                                0);

    // gui_img_t *img_num_1 = gui_img_create_from_mem(img_num_0,  "img_num_1", NUMBER_0_BIN, 100, 100, 0,
    //                                                0);
    // gui_img_create_from_mem(img_num_1,  "img_num_3", NUMBER_3_BIN, 100, 100, 0, 0);
    // gui_img_create_from_mem(img_num_1,  "img_num_4", NUMBER_4_BIN, 100, 100, 0, 0);


    // gui_img_t *img_num_2 = gui_img_create_from_mem(img_num_0,  "img_num_2", NUMBER_0_BIN, 200, 200, 0,
    //                                                0);
    // gui_img_create_from_mem(img_num_2,  "img_num_5", NUMBER_5_BIN, 200, 200, 0, 0);
    // gui_img_create_from_mem(img_num_2,  "img_num_6", NUMBER_6_BIN, 200, 200, 0, 0);


    // // gui_obj_add_event_cb(img_num_0, watch_bg_event_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    // gui_obj_add_event_cb(img_num_0, watch_bg_event_cb, GUI_EVENT_KB_UP_PRESSED, NULL);

    // button = gui_button_create(&app->screen, 50, 200, 100, 100, NUMBER_0_BIN, NUMBER_1_BIN, "nice");
    // gui_button_click(button, text_color_cb);

#if 0 //stb_img test
    gui_stb_img_t *jpg = gui_stbimg_create_from_mem(&app->screen, "jpg", TEST_JPG,
                                                    TEST_PNG - TEST_JPG, 7, 0, 0);
    gui_stb_img_t *bmp = gui_stbimg_create_from_mem(&app->screen, "bmp", TEST_BMP,
                                                    TEST_JPG - TEST_BMP, 7, 170, 0);
    gui_stb_img_t *png = gui_stbimg_create_from_mem(&app->screen, "png", TEST_PNG,
                                                    ZEND_TXT - TEST_PNG, 9, 170, 170);

    gui_stb_img_t *gif = gui_stbimg_create_from_mem(&app->screen, "gif", COLOR_GIF,
                                                    MINI_GIF - COLOR_GIF, 11, 200, 0);
    gui_stbimg_set_attribute(gif, MINI_GIF, CRAB_LIME_PNG - MINI_GIF, 11, 200, 200);
    gui_stb_img_t *gif1 = gui_stbimg_create_from_mem(&app->screen, "gif1", MINI_GIF,
                                                     CRAB_LIME_PNG - MINI_GIF, 11, 100, 0);
    gif1->gif_info->cycle_index = 3;

#endif

#if 1 // text test
    // gui_text_t *text = gui_text_create(&app->screen,  "text",  50, 32, 200, 50);
    // gui_text_set(text, "STB", "rtk_font_mem32", 0x0000, 3, 24);
    // gui_text_mode_set(text, LEFT);

    // gui_text_t *text1 = gui_text_create(&app->screen,  "text1",  0, 168, 320, 30);
    // gui_text_set(text1, "000000", "rtk_font_mem", 0xffff00, 6, 32);
    // gui_text_mode_set(text1, CENTER);
    // gui_text_t *text2 = gui_text_create(&app->screen,  "text",  0, 90, 320, 32);
    // gui_text_set(text2, str_arrived, "rtk_font_mem", 0x80808080, sizeof(str_arrived), 32);
    // gui_text_mode_set(text2, RIGHT);

    // gui_text_t *text3 = gui_text_create(&app->screen,  "text3",  0, 200, 320, 100);
    // gui_text_set(text3, str_test, "rtk_font_mem", 0x80808080, sizeof(str_test) - 1, 32);
    // gui_text_mode_set(text3, MUTI_CENTER);

    // gui_text_t *text_stb = gui_text_create(&app->screen,  "text_stb",  0, 0, 200, 200);
    // gui_text_set(text_stb, "_", "rtk_font_stb", 0x800080ff, 1, 64);
    gui_text_t *text_stb1 = gui_text_create(&app->screen,  "text_stb",  60, 0, 300, 200);
    gui_text_set(text_stb1, "ABC", "rtk_font_stb", 0x800080ff, 3, 128);

    gui_canvas_t *canvas = gui_canvas_create(&app->screen, "canvas", 200, 200, 100, 100, 0x00CDCDFF);
    canvas->draw = sport_chart_draw;

    // gui_text_t *text_stb1 = gui_text_create(&app->screen,  "text_stb",  0, 100, 200, 200);
    // gui_text_set(text_stb1, str_arrived, "rtk_font_stb", 0x800080ff, sizeof(str_arrived), 32);

#endif
#if 0 //scroll test
    gui_scroll_text_t *text4 = gui_scrolltext_create(&app->screen,  "text4", 100, 60, 120, 30);
    gui_scrolltext_scroll_set(text4, SCROLL_X, 100, 0, 3000, 1500000);
    gui_scrolltext_text_set(text4, "gui_scroll_text_t", "rtk_font_mem32", 0xffffffff, 17, 24);

    gui_scroll_text_t *text5 = gui_scrolltext_create(&app->screen,  "text4",  0, 100, 320, 50);
    gui_scrolltext_scroll_set(text5, SCROLL_Y, 24, 24, 10000, 1500000);
    gui_scrolltext_text_set(text5, str_test, "rtk_font_mem32", 0xffffffff, sizeof(str_test) - 1, 24);
#endif
    // gui_dynamic_img_t *dynamic = gui_dynamic_create_from_mem(&app->screen, "center", addr_list, 100, 100, sizeof(addr_list)/sizeof(void *), 100, 100000);
    // gui_dynamic_img_t *dynamic1 = gui_dynamic_create_from_mem(&app->screen, "center", addr_list, 150, 150, sizeof(addr_list)/sizeof(void *), 1000, 100000);

    gui_magic_img_t *magic_img = gui_magic_img_create_from_mem(&app->screen,  "center", CHARGE10_BIN, 0,
                                                               0, 0, 0);
    gui_img_scale(magic_img, 0.5, 0.5);
    gui_img_rotation(magic_img, 30, 0, 0);
    // printf("gui_text size : %d \n",sizeof(gui_scroll_text_t));
}


static gui_app_t rtk_gui_demo =
{
    .screen =
    {
        .name = "rtk_gui_demo",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_dialing_ui_design,
};

gui_app_t *get_rtk_gui_demo(void)
{
    return &rtk_gui_demo;
}


#endif

