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
#include "nanovg_agge.h"
float app_launcher2_wave_t = 0;
static void read_page_draw(gui_canvas_t *c)
{

    canvas_wave_t w = {0};
    w.h = 454;
    w.w = 454;
    w.point_count = 6;
    float dx = w.w / 5.0f;
    float samples[w.point_count];
    static float sx[6];
    static float sy[6];

    samples[0] = (1.0f + sinf(app_launcher2_wave_t * 1.2345f + cosf(app_launcher2_wave_t * 0.33457f) *
                              0.44f)) * 0.5f;
    samples[1] = (1.0f + sinf(app_launcher2_wave_t * 0.68363f + cosf(app_launcher2_wave_t * 1.3f) *
                              1.55f)) * 0.5f;
    samples[2] = (1.0f + sinf(app_launcher2_wave_t * 1.1642f + cosf(app_launcher2_wave_t * 0.33457f) *
                              1.24f)) * 0.5f;
    samples[3] = (1.0f + sinf(app_launcher2_wave_t * 0.56345f + cosf(app_launcher2_wave_t * 1.63f) *
                              0.14f)) * 0.5f;
    samples[4] = (1.0f + sinf(app_launcher2_wave_t * 1.6245f + cosf(app_launcher2_wave_t * 0.254f) *
                              0.3f)) * 0.5f;
    samples[5] = (1.0f + sinf(app_launcher2_wave_t * 0.345f + cosf(app_launcher2_wave_t * 0.03f) *
                              0.6f)) * 0.5f;
    //if(t_flag)
    //app_launcher2_wave_t -= 0.05f;
    //else
    app_launcher2_wave_t += 0.05f;
    int i;
    for (i = 0; i < w.point_count; i++)
    {
        sx[i] = w.x + i * dx;
        sy[i] = w.y + w.h * samples[i] * 0.8f;
    }
    w.point_x = sx;
    w.point_y = sy;
    extern NVGcolor nvgHSLA(float h, float s, float l, unsigned char a);
    NVGcolor color = nvgHSLA(sinf(2.0F * M_PI / 12.0F * 0.12f * (app_launcher2_wave_t)), 1.0f, 0.55f,
                             255);
    w.fill.color_data.rgba = (uint32_t)((uint32_t)(color.r * 255.0f) << 24) + (uint32_t)((uint32_t)(
                                 color.g * 255.0f) << 16) + (uint32_t)((uint32_t)(color.b * 255.0f) << 8) + (uint32_t)((uint32_t)(
                                             color.a * 255.0f));
    gui_canvas_api.wave(c, &w);
    if (app_launcher2_wave_t > 4058.454476f) { app_launcher2_wave_t = 0; }
}
void wave_page(void *parent)
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
    //gui_svg_create_from_mem(parent, HEARTRATE_SVG, 2890, 445, 445, 10, 10);
    //gui_svg_create_from_mem(parent, MUSIC_SVG, 2746, 450,450, 5, 5);
    gui_canvas_t *canvas = gui_canvas_create(parent, "c", 0, 0, 454, 454, 0);
    canvas->draw = read_page_draw;
//    gui_text_t *text = gui_text_create(parent, "icon_text", 150, 200, RTK_GUI_DEFAULT_FONT_SIZE * 7,
//                                       RTK_GUI_DEFAULT_FONT_SIZE * 7);
//    gui_text_set(text, t, "rtk_font_stb", BUTTON_COLOR, strlen(t),
//                 RTK_GUI_DEFAULT_FONT_SIZE * 7);
//    gui_text_set_animate(text, 4000, -1, text_a, text);

}
