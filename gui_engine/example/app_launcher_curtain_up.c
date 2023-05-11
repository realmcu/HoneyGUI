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

static void hr_button_press(gui_button_t *b)
{
    gui_button_api.set_animate(b, 200, 0, hr_button_press_ani_cb, b);
}
static void hr_button_release(gui_button_t *b)
{
    gui_button_api.set_animate(b, 200, 0, hr_button_release_ani_cb, b);
}
static void canvas2_draw(gui_canvas_t *c)
{
    canvas_rectangle_t r = {0};
    r.fill.color_data.rgba = 0x88;
    r.width = 454;
    r.height = 454;
    gui_canvas_api.rectangle(c, &r);

}
static void cube_button_press(gui_button_t *b)
{
    gui_button_api.set_animate(b, 200, 0, cube_button_press_ani_cb, b);
}
static void cube_button_release(gui_button_t *b)
{
    gui_button_api.set_animate(b, 200, 0, cube_button_release_ani_cb, b);
}
static void sport_button_press(gui_button_t *b)
{
    gui_button_api.set_animate(b, 200, 0, sport_button_press_ani_cb, b);
}
static void sport_button_release(gui_button_t *b)
{
    gui_button_api.set_animate(b, 200, 0, sport_button_release_ani_cb, b);
}


void curtain_up(void *curtian)
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

    gui_button_api.onPress(sport_button, sport_button_press, sport_button);
    gui_button_api.onRelease(sport_button, sport_button_release, sport_button);
    gui_button_api.onPress(cube_button, cube_button_press, sport_button);
    gui_button_api.onRelease(cube_button, cube_button_release, sport_button);
    gui_button_api.onPress(hr_button, hr_button_press, sport_button);
    gui_button_api.onRelease(hr_button, hr_button_release, sport_button);
}
