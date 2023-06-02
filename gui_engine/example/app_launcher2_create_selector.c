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

void create_selector(gui_obj_t *screen)
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
