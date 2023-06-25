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
#ifdef RTK_MODULE_VG_LITE
#include "gui_vg_lite_clock.h"
#include "gui_cube.h"
#else
#include "gui_cube.h"
#endif
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

static void music_button_click(gui_button_t *b)
{
    gui_log("music_button_click\n");
    extern void nsvgDelete(NSVGimage * image);
    nsvgDelete(b->img->draw_img.data);
    gui_tree_free(b->img);

    if (!music_button_click_pause)
    {
        b->img = (void *)gui_svg_create_from_mem(b, PALY2_SVG, 610, 0, 0, 100, 100);
        gui_log("static void stop_mp3(void)\n");
#if defined (GUI_RTL8772F) && defined OS_RTTHREAD
        gui_event_stop_mp3();
#endif
    }
    else
    {
        b->img = (void *)gui_svg_create_from_mem(b, PAUSE2_SVG, 946, 0, 0, 100, 100);
        gui_log("static void start_mp3(void)\n");
#if defined (GUI_RTL8772F) && defined OS_RTTHREAD
        gui_event_start_mp3();
#endif
    }


    music_button_click_pause = !music_button_click_pause;
}
static void music_button_pause_release(gui_button_t *b)
{
    music_button_click(b);
    gui_button_api.set_animate(b, 100, 0, music_button_release_ani_cb, b);
}
gui_img_t *music_album_picture;
#include "nanovg.h"
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
    a.stroke.stroke_linecap = ROUND;
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
void music_play(gui_obj_t *screen)
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
    gui_canvas_t *canvas2 = gui_canvas_create(music_album_picture, "c", 0, 0, 200, 200, 0);
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

