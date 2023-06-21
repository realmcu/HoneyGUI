#include "ezxml.h"
#include "app_launcher.h"
#include "string.h"
#include "stdio.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "ezxml.h"
#include <sys/stat.h>
#include <fcntl.h>
struct widget_create
{
    char *name;
    obj_type_t type;
};
#define WIDGETS_NUM 18
struct widget_create widget[] =
{
    { "win", WINDOW }, //
    { "textbox", TEXTBOX },//
    { "img", IMAGE },//
    { "progressbar", PROGRESSBAR },
    { "seekbar", SEEKBAR },
    { "tabview", TABVIEW },//
    { "tab", TAB },
    { "curtainview", CURTAINVIEW },//
    { "curtain", CURTAIN },
    //{ "iconlist", ICONLIST },
    { "icon", ICON },//
    { "script", JAVASCRIPT },
    { "switch", CLICKSWITCH },
    { "page", PAGE },//
    //{ "pagebar", PAGEBAR },
    { "screen", SCREEN },
    { "grid",  GRID},
    {"radio", RADIO},
    {"appendTexts", RADIOSWITCH},
    {"arc", ARC},
};
char *get_space_string_head(const char *string)
{
    char *s = gui_strdup(string);
    size_t length = strlen(s);
    for (int i = length - 1; i >= 0; i--)
    {
        if (!((s[i] >= 8) && (s[i] <= 32)))
        {
            s[i + 1] = '\0';
            break;
        };
    }
    return s;
}
int line = 0;
char *js;

#define RGB888_RED      0x00ff0000
#define RGB888_GREEN    0x0000ff00
#define RGB888_BLUE     0x000000ff

#define RGB565_RED      0xf800
#define RGB565_GREEN    0x07e0
#define RGB565_BLUE     0x001f

unsigned short RGB888ToRGB565(unsigned int n888Color)
{
    unsigned short n565Color = 0;


    unsigned char cRed   = (n888Color & RGB888_RED)   >> 19;
    unsigned char cGreen = (n888Color & RGB888_GREEN) >> 10;
    unsigned char cBlue  = (n888Color & RGB888_BLUE)  >> 3;


    n565Color = (cRed << 11) + (cGreen << 5) + (cBlue << 0);
    return n565Color;
}
unsigned short string_rgb888_to_rgb565(const char *color)
{
    if (*color != '#')
    {
        gui_log("color format wrong");
        return 0;
    }
    uint32_t c = strtol(color + 1, NULL, 16);
    gui_log("color%s:%x,%x\n", color + 1, c, RGB888ToRGB565(c));
    return RGB888ToRGB565(c);
}
uint32_t string_rgb888(const char *color)
{
    if (*color != '#')
    {
        gui_log("color format wrong");
        return 0;
    }
    uint32_t c = strtoul(color + 1, NULL, 16);

    return (c);
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
    if (button->style == SLIDE_FADE || button->style == SLIDE_SCALE_FADE)
    {
        gui_img_set_opacity((void *)button->img,
                            (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    }
    if (button->style == SLIDE_SCALE || button->style == SLIDE_SCALE_FADE)
    {
        gui_img_scale((void *)button->img, scale,  scale);
        //gui_img_translate((void *)button->img, 25.0f * (from - scale),  25.0f * (from - scale));
    }



    //gui_log("o:%f\n", 25.0f * (from - scale));
}
static void draw_arc(gui_canvas_t *c)
{
    canvas_arc_t a = {0};
    a.cx = c->cx;
    a.cy = c->cy;
    a.r = c->r;
    a.stroke.fill.color_data.rgba = c->s;
    a.stroke.stroke_width = c->sw;
    a.start_angle = c->sd;
    a.end_angle = c->ed;
    gui_canvas_api.arc(c, &a);

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


    //gui_img_translate((void *)button->img, 25.0f * (1.0f - scale),  25.0f * (1.0f - scale));
    if (button->style == SLIDE_FADE || button->style == SLIDE_SCALE_FADE)
    {
        gui_img_set_opacity((void *)button->img,
                            (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    }
    if (button->style == SLIDE_SCALE || button->style == SLIDE_SCALE_FADE)
    {
        gui_img_scale((void *)button->img, scale,  scale);
    }
}
static void sport_button_press(gui_button_t *b)
{
    gui_button_api.set_animate(b, 200, 0, sport_button_press_ani_cb, b);
}
static void sport_button_release(gui_button_t *b)
{
    gui_button_api.set_animate(b, 200, 0, sport_button_release_ani_cb, b);
}
gui_obj_t *widget_create_handle(ezxml_t p, gui_obj_t *parent)
{
    char *name = p->name;
    gui_log("%s\n", name);
    for (size_t i = 0; i < WIDGETS_NUM; i++)
    {
        if (!strcmp(widget[i].name, name))
        {
            switch (widget[i].type)
            {
            case TEXTBOX:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = gui_get_screen_width();
                    int16_t h = 50;
                    const char *text = "text";
                    const char *font = "rtk_font_stb";
                    size_t color = 0xffffffff;
                    int fontSize = 30;
                    int style = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "x"))
                        {
                            x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "y"))
                        {
                            y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "w"))
                        {
                            uint32_t w_temp = atoi(p->attr[++i]);
                            if (w_temp)
                            {
                                w = w_temp;
                            }
                        }
                        else if (!strcmp(p->attr[i], "h"))
                        {
                            uint32_t w_temp = atoi(p->attr[++i]);
                            if (w_temp)
                            {
                                h = w_temp;
                            }
                        }
                        else if (!strcmp(p->attr[i], "text"))
                        {
                            text = p->attr[++i];
                        }
                        else if (!strcmp(p->attr[i], "font"))
                        {
                            font = p->attr[++i];
                        }
                        else if (!strcmp(p->attr[i], "fontSize"))
                        {
                            fontSize = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "color"))
                        {
                            color = string_rgb888(p->attr[++i]);
                            gui_log("color %s,%x\n", p->attr[i], color);
                        }
                        else if (!strcmp(p->attr[i], "mode"))
                        {
                            char *s = p->attr[++i];
                            if (!strcmp(p->attr[i], "truncate"))
                            {
                                style = 0;
                            }
                            else if (!strcmp(p->attr[i], "verticalscroll"))
                            {
                                style = SCROLL_Y;
                            }
                            else if (!strcmp(p->attr[i], "horizontalscroll"))
                            {
                                style = SCROLL_X;
                            }

                        }
                        i++;
                    }
                    //gui_log("x:%d,y:%d,w:%dh:%d,font:%s,text:%s,color:%x\n", x, y, w, h, font, text, color);
                    char *ptxt = get_space_string_head(p->txt);
                    //gui_log("p->txt2 = %s,\n", ptxt);
                    if (text && font)
                    {
                        gui_text_t *t = 0;
                        if (style == 0)
                        {
                            t = gui_text_create(parent, ptxt, x, y, gui_get_screen_width(), h);
                            gui_text_set(t, gui_strdup(text), "rtk_font_stb", color, strlen(text), fontSize);
                        }
                        else
                        {
                            gui_scroll_text_t *scroll_text = gui_scrolltext_create(parent,  ptxt, x, y,
                                                                                   strlen(text) * (fontSize / 4), h);
                            gui_scrolltext_scroll_set(scroll_text, style, 100, 0, 3000, 1500000);
                            gui_scrolltext_text_set(scroll_text, gui_strdup(text), "rtk_font_stb", color, strlen(text),
                                                    fontSize);
                            t = (void *)scroll_text;
                        }




                        parent = (void *)t;
                    }
                }
                break;
            case IMAGE:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    char *file = NULL;
                    float scalex = 1;
                    float scaley = 1;
                    float angle = 0;
                    int mode = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s,\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "x"))
                        {
                            x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "y"))
                        {
                            y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "w"))
                        {
                            w = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "h"))
                        {
                            h = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "scaleX"))
                        {
                            scalex = atof(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "scaleY"))
                        {
                            scaley = atof(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "rotationAngle"))
                        {
                            angle = atof(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "mode"))
                        {
                            char *s = p->attr[++i];
                            if (!strcmp(p->attr[i], "normal"))
                            {
                                mode = IMG_FILTER_BLACK;
                            }
                            else if (!strcmp(p->attr[i], "blendSrcBlack"))
                            {
                                mode = IMG_BYPASS_MODE;
                            }

                        }
                        else if (!strcmp(p->attr[i], "file"))
                        {
                            char *filetemp = p->attr[++i];
                            if (strlen(filetemp) == 0)
                            {
                                file = NULL;
                            }
                            else
                            {
                                file = gui_strdup(filetemp);
                            }
                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    //gui_log("x:%d,y:%d,w:%dh:%d,file:%s\n", x, y, w, h, file);
                    if (file)
                    {

                        void *imgbuf = gui_get_file_address(file);
                        parent = (void *)gui_magic_img_create_from_mem(parent, gui_strdup(p->txt), imgbuf, x, y, w, h);
                        if (scalex != 1 || scaley != 1)
                        {
                            gui_img_scale((void *)parent, scalex, scaley);
                        }
                        if (angle != 0)
                        {
                            gui_img_get_height((void *)parent);
                            gui_img_rotation((void *)parent, angle, parent->w / 2, parent->h / 2);
                        }



                    }
                }
                break;
            case WINDOW:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "x"))
                        {
                            x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "y"))
                        {
                            y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "w"))
                        {
                            w = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "h"))
                        {
                            h = atoi(p->attr[++i]);
                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    //gui_log("x:%d,y:%d,w:%dh:%d\n", x, y, w, h);
                    {
                        parent = (void *)gui_win_create(parent, ptxt, x, y, w, h);
                    }
                }
                break;
            /*case CLICKSWITCH:
            {
                size_t i = 0;
                int16_t x = 0;
                int16_t y = 0;
                while (true)
                {
                    if (!(p->attr[i]))
                    {
                        break;
                    }
                    //gui_log("p->attr[i]:%s\n", p->attr[i]);
                    if (!strcmp(p->attr[i], "x"))
                    {
                        x = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "y"))
                    {
                        y = atoi(p->attr[++i]);
                    }
                    i++;
                }
                char *ptxt = get_space_string_head(p->txt);
                //gui_log("x:%d,y:%d,w:%dh:%d\n", x, y, w, h);
                {
                    parent = gui_switch_create(parent, ptxt, x, y,0,0);
                }
            }
            break;*/
            case TABVIEW:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    int style = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "x"))
                        {
                            x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "y"))
                        {
                            y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "w"))
                        {
                            w = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "h"))
                        {
                            h = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "transition"))
                        {
                            char *s = p->attr[++i];
                            if (!strcmp(p->attr[i], "Normal"))
                            {
                                style = SLIDE_CLASSIC;
                            }
                            else if (!strcmp(p->attr[i], "Fade"))
                            {
                                style = SLIDE_FADE;
                            }
                            else if (!strcmp(p->attr[i], "Scale"))
                            {
                                style = SLIDE_SCALE;
                            }
                            else if (!strcmp(p->attr[i], "Fade&Scale"))
                            {
                                style = SLIDE_SCALE_FADE;
                            }

                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    parent = (void *)gui_tabview_create(parent, ptxt, x, y, w, h);
                    GUI_TYPE(gui_tabview_t, parent)->style = style;
                    ((gui_tabview_t *)parent)->style = style;
                }
                break;
            case ARC:
                {
                    size_t i = 0;
                    int16_t cx = 100;
                    int16_t cy = 100;
                    int16_t r = 100;
                    int16_t stroke_width = 10;
                    int cap = 0;
                    uint32_t fill = 0xff0000ff;
                    uint32_t stroke = 0xff0000ff;
                    float sd = 0;
                    float ed = 100;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "cx") || !strcmp(p->attr[i], "centralX"))
                        {
                            cx = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "cy") || !strcmp(p->attr[i], "centralY"))
                        {
                            cy = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "r") || !strcmp(p->attr[i], "radius"))
                        {
                            r = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "startDegree"))
                        {
                            sd = atof(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "endDegree"))
                        {
                            ed = atof(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "strokeWidth") || !strcmp(p->attr[i], "stroke-width"))
                        {
                            stroke_width = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "stroke-linecap") || !strcmp(p->attr[i], "capMode"))
                        {
                            char *s = p->attr[++i];
                            if (!strcmp(p->attr[i], "butt"))
                            {
                                cap = CAP_BUTT;
                            }
                            else if (!strcmp(p->attr[i], "round"))
                            {
                                cap = CAP_ROUND;
                            }
                            else if (!strcmp(p->attr[i], "square"))
                            {
                                cap = CAP_SQUARE;
                            }


                        }
                        else if (!strcmp(p->attr[i], "fill"))
                        {
                            fill = string_rgb888(p->attr[++i]);
                            gui_log("color %s,%x\n", p->attr[i], fill);
                        }
                        else if (!strcmp(p->attr[i], "stroke") || !strcmp(p->attr[i], "color"))
                        {
                            stroke = string_rgb888(p->attr[++i]);
                            gui_log("color %s,%x\n", p->attr[i], stroke);
                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    parent = (void *)gui_canvas_create(parent, ptxt, 0, 0, 454, 454, 0xffffffff);
                    GUI_TYPE(gui_canvas_t, parent)->draw = draw_arc;
                    GUI_TYPE(gui_canvas_t, parent)->cx = cx;
                    GUI_TYPE(gui_canvas_t, parent)->cy = cy;
                    GUI_TYPE(gui_canvas_t, parent)->r = r;
                    GUI_TYPE(gui_canvas_t, parent)->sw = stroke_width;
                    GUI_TYPE(gui_canvas_t, parent)->s = stroke;
                    GUI_TYPE(gui_canvas_t, parent)->ed = ed;
                    GUI_TYPE(gui_canvas_t, parent)->sd = sd;


                }
                break;
            case PROGRESSBAR:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    uint32_t color = 0xffffffaa;
                    uint32_t highlightColor = 0xffffffff;
                    bool vh = false;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "x"))
                        {
                            x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "y"))
                        {
                            y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "w"))
                        {
                            w = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "h"))
                        {
                            h = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "color"))
                        {
                            color = string_rgb888(p->attr[++i]);
                            gui_log("color %s,%x\n", p->attr[i], color);
                        }
                        else if (!strcmp(p->attr[i], "highlightColor"))
                        {
                            highlightColor = string_rgb888(p->attr[++i]);
                            gui_log("color %s,%x\n", p->attr[i], highlightColor);
                        }
                        else if (!strcmp(p->attr[i], "VH"))
                        {
                            char *s = p->attr[++i];
                            if (!strcmp(p->attr[i], "vertical"))
                            {
                                vh = true;
                            }
                            else if (!strcmp(p->attr[i], "V"))
                            {
                                vh = true;
                            }
                            else if (!strcmp(p->attr[i], "horizontal"))
                            {
                                vh = false;
                            }
                            else if (!strcmp(p->attr[i], "H"))
                            {
                                vh = false;
                            }

                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    if (vh)
                    {
                        parent = (void *)gui_progressbar_v_create(parent, ptxt, x, y, w, h);
                    }
                    else
                    {
                        parent = (void *)gui_progressbar_create(parent, ptxt, x, y, w, h);
                    }



                    GUI_TYPE(gui_progressbar_t, parent)->color = color;
                    GUI_TYPE(gui_progressbar_t, parent)->color_hl = highlightColor;
                }
                break;
            case SEEKBAR:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    uint32_t color = 0xffffffaa;
                    uint32_t highlightColor = 0xffffffff;
                    bool vh = false;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "x"))
                        {
                            x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "y"))
                        {
                            y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "w"))
                        {
                            w = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "h"))
                        {
                            h = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "color"))
                        {
                            color = string_rgb888(p->attr[++i]);
                            gui_log("color %s,%x\n", p->attr[i], color);
                        }
                        else if (!strcmp(p->attr[i], "highlightColor"))
                        {
                            highlightColor = string_rgb888(p->attr[++i]);
                            gui_log("color %s,%x\n", p->attr[i], highlightColor);
                        }
                        else if (!strcmp(p->attr[i], "VH"))
                        {
                            char *s = p->attr[++i];
                            if (!strcmp(p->attr[i], "vertical"))
                            {
                                vh = true;
                            }
                            else if (!strcmp(p->attr[i], "V"))
                            {
                                vh = true;
                            }
                            else if (!strcmp(p->attr[i], "horizontal"))
                            {
                                vh = false;
                            }
                            else if (!strcmp(p->attr[i], "H"))
                            {
                                vh = false;
                            }

                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    if (vh)
                    {
                        parent = (void *)gui_seekbar_create(parent, ptxt, x, y, w, h);
                    }
                    else
                    {
                        parent = (void *)gui_seekbar_h_create(parent, ptxt, x, y, w, h);
                    }



                    GUI_TYPE(gui_progressbar_t, parent)->color = color;
                    GUI_TYPE(gui_progressbar_t, parent)->color_hl = highlightColor;
                }
                break;
            /*case PROGRESSBAR:
            {
                size_t i = 0;
                int16_t x = 0;
                int16_t y = 0;
                int16_t w = 0;
                int16_t h = 0;
                while (true)
                {
                    if (!(p->attr[i]))
                    {
                        break;
                    }
                    //gui_log("p->attr[i]:%s\n", p->attr[i]);
                    if (!strcmp(p->attr[i], "x"))
                    {
                        x = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "y"))
                    {
                        y = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "w"))
                    {
                        w = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "h"))
                    {
                        h = atoi(p->attr[++i]);
                    }
                    i++;
                }
                char *ptxt = get_space_string_head(p->txt);
                parent = gui_progressbar_create(parent, ptxt, x, y, w, h);
            }
            break;
            case SEEKBAR:
            {
                size_t i = 0;
                int16_t x = 0;
                int16_t y = 0;
                int16_t w = 0;
                int16_t h = 0;
                while (true)
                {
                    if (!(p->attr[i]))
                    {
                        break;
                    }
                    //gui_log("p->attr[i]:%s\n", p->attr[i]);
                    if (!strcmp(p->attr[i], "x"))
                    {
                        x = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "y"))
                    {
                        y = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "w"))
                    {
                        w = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "h"))
                    {
                        h = atoi(p->attr[++i]);
                    }
                    i++;
                }
                char *ptxt = get_space_string_head(p->txt);
                parent = gui_seekbar_create(parent, ptxt, x, y, w, h);
            }
            break;*/
            case TAB:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    int16_t idx = 0;
                    int16_t idy = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "x"))
                        {
                            x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "y"))
                        {
                            y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "w"))
                        {
                            w = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "h"))
                        {
                            h = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "idx"))
                        {
                            idx = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "idy"))
                        {
                            idy = atoi(p->attr[++i]);
                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    //gui_log("x:%d,y:%d,w:%dh:%d,idx:%d,idy:%d\n", x, y, w, h, idx, idy);
                    parent = (void *)gui_tab_create(parent, ptxt, x, y, w, h, idx, idy);
                }
                break;
            case CURTAINVIEW:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    int style = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "x"))
                        {
                            x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "y"))
                        {
                            y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "w"))
                        {
                            w = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "h"))
                        {
                            h = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "transition"))
                        {
                            char *s = p->attr[++i];
                            if (!strcmp(p->attr[i], "normal"))
                            {
                                style = SLIDE_CLASSIC;
                            }
                            else if (!strcmp(p->attr[i], "fade"))
                            {
                                style = SLIDE_FADE;
                            }
                            else if (!strcmp(p->attr[i], "scale"))
                            {
                                style = SLIDE_SCALE;
                            }
                            else if (!strcmp(p->attr[i], "fade&scale"))
                            {
                                style = SLIDE_SCALE_FADE;
                            }

                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    //gui_log("x:%d,y:%d,w:%dh:%d,scope:%f\n", x, y, w, h);
                    parent = (void *)gui_curtainview_create(parent, ptxt, x, y, w, h);
                    GUI_TYPE(gui_curtainview_t, parent)->style = style;
                }
                break;
            case PAGE:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "x"))
                        {
                            x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "y"))
                        {
                            y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "w"))
                        {
                            w = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "h"))
                        {
                            h = atoi(p->attr[++i]);
                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    //gui_log("x:%d,y:%d,w:%dh:%d,scope:%f\n", x, y, w, h);
                    parent = (void *)gui_page_create(parent, ptxt, x, y, w, h);
                }
                break;
            case GRID:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t rowNumber = 4;
                    int16_t colNumber = 4;
                    int16_t rowGap = 100;
                    int16_t colGap = 100;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "x"))
                        {
                            x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "y"))
                        {
                            y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "rowNumber"))
                        {
                            rowNumber = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "colNumber"))
                        {
                            colNumber = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "rowGap"))
                        {
                            rowGap = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "colGap"))
                        {
                            colGap = atoi(p->attr[++i]);
                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    //gui_log("x:%d,y:%d,w:%dh:%d,scope:%f\n", x, y, w, h);
                    parent = (void *)gui_grid_create(parent, x, y, colNumber, rowNumber, colGap, rowGap);
                    parent->name = ptxt;
                }
                break;
            case SCREEN:
                {
                    size_t i = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s\n", p->attr[i]);
                        else if (!strcmp(p->attr[i], "w"))
                        {
                            w = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "h"))
                        {
                            h = atoi(p->attr[++i]);
                        }
                        i++;
                    }
                }
                break;
            /*case PAGEBAR:
            {
                size_t i = 0;
                int16_t x = 0;
                int16_t y = 0;
                int16_t w = 0;
                int16_t h = 0;
                BAR_LAYOUT_T bar_layout = UP;
                while (true)
                {
                    if (!(p->attr[i]))
                    {
                        break;
                    }
                    //gui_log("p->attr[i]:%s\n", p->attr[i]);
                    if (!strcmp(p->attr[i], "x"))
                    {
                        x = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "y"))
                    {
                        y = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "w"))
                    {
                        w = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "h"))
                    {
                        h = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "orientation"))
                    {
                        const char *o = p->attr[++i];
                        if (!strcmp(o, "MIDDLE"))
                            bar_layout = MIDDLE;
                        else if (!strcmp(o, "UP"))
                        {
                            bar_layout = UP;
                        }
                        else if (!strcmp(o, "DOWN"))
                        {
                            bar_layout = DOWN;
                        }
                        else if (!strcmp(o, "LEFT"))
                        {
                            bar_layout = LEFT;
                        }
                        else if (!strcmp(o, "RIGHT"))
                        {
                            bar_layout = RIGHT;
                        }
                    }
                    i++;
                }
                char *ptxt = get_space_string_head(p->txt);
                //gui_log("x:%d,y:%d,w:%dh:%d,scope:%f\n", x, y, w, h);
                //parent = gui_pagebar_create(parent, ptxt, x, y, w, h, bar_layout);
            }
            break;*/
            case CURTAIN:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    float scope = 1.0f;
                    gui_curtain_enum_t orientation = CURTAIN_MIDDLE;
                    int style = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "x"))
                        {
                            x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "y"))
                        {
                            y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "w"))
                        {
                            w = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "h"))
                        {
                            h = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "scope"))
                        {
                            scope = atof(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "orientation"))
                        {
                            const char *o = p->attr[++i];
                            if (!strcmp(o, "middle"))
                            {
                                orientation = CURTAIN_MIDDLE;
                            }
                            else if (!strcmp(o, "up"))
                            {
                                orientation = CURTAIN_UP;
                            }
                            else if (!strcmp(o, "down"))
                            {
                                orientation = CURTAIN_DOWN;
                            }
                            else if (!strcmp(o, "left"))
                            {
                                orientation = CURTAIN_LEFT;
                            }
                            else if (!strcmp(o, "right"))
                            {
                                orientation = CURTAIN_RIGHT;
                            }
                        }
                        else if (!strcmp(p->attr[i], "transition"))
                        {
                            char *s = p->attr[++i];
                            if (!strcmp(p->attr[i], "normal"))
                            {
                                style = SLIDE_CLASSIC;
                            }
                            else if (!strcmp(p->attr[i], "fade"))
                            {
                                style = SLIDE_FADE;
                            }
                            else if (!strcmp(p->attr[i], "scale"))
                            {
                                style = SLIDE_SCALE;
                            }
                            else if (!strcmp(p->attr[i], "fade&scale"))
                            {
                                style = SLIDE_SCALE_FADE;
                            }

                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    //gui_log("x:%d,y:%d,w:%dh:%d,orientation:%d\n", x, y, w, h, orientation);
                    parent = (void *)gui_curtain_create(parent, ptxt, x, y, w, h, orientation, scope);
                }
                break;
            /*case ICONLIST:
            {
                size_t i = 0;
                int16_t x = 0;
                int16_t y = 0;
                int16_t w = 320;
                int16_t h = gui_get_screen_height();
                uint32_t space_between_icons = 10;
                uint32_t space_in_icon = 2;
                void *back_img_src = NULL;
                void *back_img_highlight_src = NULL;
                char *font_type = "rtk_font_fs32";
                uint32_t font_color = 0XFFFF;
                uint32_t font_size = 40;
                ICON_LAYOUT_T icon_layout = RIGHT;
                while (true)
                {
                    if (!(p->attr[i]))
                    {
                        break;
                    }
                    //gui_log("p->attr[i]:%s\n", p->attr[i]);
                    if (!strcmp(p->attr[i], "x"))
                    {
                        x = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "y"))
                    {
                        y = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "w"))
                    {
                        uint32_t w_temp = atoi(p->attr[++i]);
                        if (w_temp)
                        {
                            w = w_temp;
                        }

                    }
                    else if (!strcmp(p->attr[i], "h"))
                    {
                        uint32_t w_temp = atoi(p->attr[++i]);
                        if (w_temp)
                        {
                            h = w_temp;
                        }
                    }
                    else if (!strcmp(p->attr[i], "font"))
                    {
                        font_type = gui_strdup(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "fontColor"))
                    {
                        font_color = string_rgb888_to_rgb565(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "fontSize"))
                    {
                        font_size = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "spaceBetweenIcons"))
                    {
                        space_between_icons = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "spaceInIcon"))
                    {
                        space_in_icon = atoi(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "backImg"))
                    {
                        back_img_src = gui_strdup(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "backImgHighlight"))
                    {
                        back_img_highlight_src = gui_strdup(p->attr[++i]);
                    }
                    else if (!strcmp(p->attr[i], "orientation"))
                    {
                        const char *o = p->attr[++i];
                        if (!strcmp(o, "MIDDLE"))
                            icon_layout = CURTAIN_MIDDLE;
                        else if (!strcmp(o, "UP"))
                        {
                            icon_layout = CURTAIN_UP;
                        }
                        else if (!strcmp(o, "DOWN"))
                        {
                            icon_layout = CURTAIN_DOWN;
                        }
                        else if (!strcmp(o, "LEFT"))
                        {
                            icon_layout = CURTAIN_LEFT;
                        }
                        else if (!strcmp(o, "RIGHT"))
                        {
                            icon_layout = CURTAIN_RIGHT;
                        }
                    }
                    i++;
                }
                //gui_log("x:%d,y:%d,w:%dh:%d\n", x, y, w, h);
                char *ptxt = get_space_string_head(p->txt);
                parent = gui_iconlist_create_theme1(parent, ptxt, x, y, w, h, NULL, 0);
                gui_iconlist_theme_1_t *il = (gui_iconlist_theme_1_t *)parent;
                il->font_color = font_color;
                il->font_size = font_size;
                il->font_type = font_type;
                il->icon_gap = space_between_icons;
                il->icon_layout = icon_layout;
                il->gap_in_icon = space_in_icon;
                if (back_img_src)
                {
                    il->back_img_src = back_img_src;
                }
                if (back_img_highlight_src)
                {
                    il->back_img_highlight_src = back_img_highlight_src;
                }


            }
            break;*/
            case ICON:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    char *font_type = "rtk_font_fs32";
                    char *text = NULL;
                    int text_x = 0;
                    int text_y = 0;
                    uint32_t font_color = 0Xf0f0;
                    uint32_t font_size = 40;
                    int picture_x = 0;
                    int picture_y = 0;
                    int transition = 0;
                    char *picture = NULL;
                    char *hl_picture = NULL;
                    int style = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s,\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "x"))
                        {
                            x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "y"))
                        {
                            y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "w"))
                        {
                            w = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "h"))
                        {
                            h = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "font"))
                        {
                            font_type = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "picture"))
                        {
                            picture = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "highlightPicture"))
                        {
                            hl_picture = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "fontColor"))
                        {
                            font_color = string_rgb888_to_rgb565(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "fontSize"))
                        {
                            font_size = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "text"))
                        {
                            text = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "textX"))
                        {
                            text_x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "textY"))
                        {
                            text_y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "pictureX"))
                        {
                            picture_x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "pictureY"))
                        {
                            picture_y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "mode"))
                        {
                            char *s = p->attr[++i];
                            if (!strcmp(p->attr[i], "normal"))
                            {
                                style = SLIDE_CLASSIC;
                            }
                            else if (!strcmp(p->attr[i], "fade"))
                            {
                                style = SLIDE_FADE;
                            }
                            else if (!strcmp(p->attr[i], "scale"))
                            {
                                style = SLIDE_SCALE;
                            }
                            else if (!strcmp(p->attr[i], "fade&scale"))
                            {
                                style = SLIDE_SCALE_FADE;
                            }

                        }
                        i++;
                    }
                    void *img1;
                    void *img2;
                    {
                        img1 = gui_get_file_address(picture);
                    }
                    {
                        img2 = gui_get_file_address(hl_picture);;
                    }
                    char *ptxt = get_space_string_head(p->txt);

                    parent = (void *)gui_button_create(parent, x, y, w, h, img1, img2, text, 0, 0);
                    GUI_TYPE(gui_button_t, parent)->style = style;
                    parent->name = ptxt;
                    gui_button_img_move((void *)parent, picture_x, picture_y);
                    gui_button_text_move((void *)parent, text_x, text_y);
                    gui_button_api.onPress((void *)parent, sport_button_press, parent);
                    gui_button_api.onRelease((void *)parent, sport_button_release, parent);




                }
            case RADIO:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    char *font_type = "rtk_font_fs32";
                    char *text = NULL;
                    int text_x = 0;
                    int text_y = 0;
                    uint32_t font_color = 0Xf0f0;
                    uint32_t font_size = 40;
                    int picture_x = 0;
                    int picture_y = 0;
                    int transition = 0;
                    char *picture = NULL;
                    char *hl_picture = NULL;
                    int style = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s,\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "x"))
                        {
                            x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "y"))
                        {
                            y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "w"))
                        {
                            w = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "h"))
                        {
                            h = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "picture"))
                        {
                            picture = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "highlightPicture"))
                        {
                            hl_picture = gui_strdup(p->attr[++i]);
                        }

                        i++;
                    }
                    void *img1;
                    void *img2;
                    {
                        img1 = gui_get_file_address(picture);
                    }
                    {
                        img2 = gui_get_file_address(hl_picture);;
                    }
                    parent = (void *)gui_radio_create(parent, x, y, w, h, img1, img2);
                    parent->name = get_space_string_head(p->txt);
                    //parent = gui_win_create(parent, "1", 0,0,0,0);
                }
                break;
            case CLICKSWITCH:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    char *picture = NULL;
                    char *hl_picture = NULL;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s,\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "x"))
                        {
                            x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "y"))
                        {
                            y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "w"))
                        {
                            w = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "h"))
                        {
                            h = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "picture"))
                        {
                            picture = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "highlightPicture"))
                        {
                            hl_picture = gui_strdup(p->attr[++i]);
                        }

                        i++;
                    }
                    void *img1;
                    void *img2;
                    {
                        img1 = gui_get_file_address(picture);
                    }
                    {
                        img2 = gui_get_file_address(hl_picture);
                    }
                    parent = (void *)gui_switch_create(parent, x, y, w, h, img1, img2);
                    parent->name = get_space_string_head(p->txt);

                }
                break;
            case RADIOSWITCH:
                {
                    size_t i = 0;
                    char *text = "radio";
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s,\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "text"))
                        {
                            text = gui_strdup(p->attr[++i]);
                        }

                        i++;
                    }
                    if (parent->type == RADIO)
                    {
                        gui_radio_api.append((void *)parent, text);

                    }
                    //parent = gui_win_create(parent, "1", 0,0,0,0);
                }
                break;
            case JAVASCRIPT:
                {
                    if (!strcmp(p->attr[0], "file"))
                    {
                        js = gui_strdup(p->attr[1]);
                    }

                }
            default:
                break;
            }
        }
    }
    return parent;


}
void foreach_create(ezxml_t p, gui_obj_t *parent)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
        foreach_create(i->child, widget_create_handle(i, parent));
    }
}
void foreach_scan(ezxml_t p, const char *element, ezxml_t *target)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
        if (strcmp(i->name, "screen") == 0)
        {

            *target = i;
            return;
        }
        foreach_scan(i->child, element, target);
    }
}
void level_scan(ezxml_t p, char **pic, char **text)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {

        if (strcmp(i->name, "app") == 0)
        {

            *text = get_space_string_head(i->txt); gui_log("level:%s\n", *text);
            size_t j = 0;
            while (true)
            {
                if (!(i->attr[j]))
                {
                    break;
                }
                //gui_log("p->attr[i]:%s\n", p->attr[i]);
                if (!strcmp(i->attr[j], "iconPicture"))
                {
                    *pic = gui_strdup(i->attr[++j]);
                    break;
                }

                j++;
            }
            break;
        }
    }
}
void foreach_count(ezxml_t p, size_t *widget_count)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
        (*widget_count)++;
        foreach_count(i->child, widget_count);
    }
}
#include "gui_server.h"
/*void app_init(void *p)
{
    gui_app_t *app = p;
    gui_list_init(&(app->screen->child_list));
    gui_list_init(&(app->screen->brother_list));

    ezxml_t f1 = ezxml_parse_file(app->xml);
    js = NULL;
    foreach_create(f1, app->screen);
    ezxml_free(f1);
    gui_return_create(app->screen);
    app->inited = true;
    js_run_file_on_server(js, p);

    gui_exec(app->screen);
    while (1)
    {
        gui_thread_mdelay(1000000);
        //handle_launcher_msg();
    }
}*/
void create_tree(gui_app_t *app)
{
    ezxml_t f1 = ezxml_parse_file(app->xml);
    js = NULL;
    foreach_create(f1, &app->screen);
    extern void js_run_file(const char *file, gui_app_t  *app);
    if (js)
    {
        char *path = gui_malloc(strlen(js) + strlen(GUI_ROOT_FOLDER) + 1);
        sprintf(path, "%s%s", GUI_ROOT_FOLDER, js);
        js_run_file(path, app);
        gui_free(path);
    }


    ezxml_free(f1);
}
void get_app(gui_app_t *app, char *pic, char *text)
{
    ezxml_t f1 = ezxml_parse_file(app->xml);


    level_scan(f1, (void *)pic, (void *)text);
    ezxml_free(f1);
}
void get_system_screen(int *w, int *h)
{
    ezxml_t f1 =
        ezxml_parse_file("example/root_image/root_image_sim_vscode/root/app/system/launcher.xml");
    ezxml_t t;
    foreach_scan(f1, "screen", &t);
    size_t i = 0;

    while (true)
    {
        if (!(t->attr[i]))
        {
            break;
        }
        //gui_log("p->attr[i]:%s\n", p->attr[i]);
        else if (!strcmp(t->attr[i], "w"))
        {
            *w = atoi(t->attr[++i]);
        }
        else if (!strcmp(t->attr[i], "h"))
        {
            *h = atoi(t->attr[++i]);
        }
        i++;
    }
    ezxml_free(f1);
}
/*void album_startup(void *this)
{
    gui_app_startup_base(this);
}
static void album_close(void *this)
{
    gui_app_close_base(this);
}
void album_ctor(void *this)
{
    gui_app_ctor_base(this, "album", app_init, album_startup, album_close);
}*/
#include <gui_api.h>

char *get_app_name(const char *xml)
{
    ezxml_t f1 = ezxml_parse_file(xml);
    GUI_ASSERT(f1);
    if (!strcmp("app", f1->name))
    {
        char *s = gui_strdup(f1->txt);
        ezxml_free(f1);
        return s;
    }
    return NULL;
}
/*static gui_list_t app_list;
static void screen_backfunc(gui_app_t *app)
{
    if (app->installed)
    {
        gui_app_close_base(app);
        memset(app, 0, sizeof(gui_app_t));
        gui_free(app);
    }
    open_xml_app("app/system/launcher.xml");
}
static void set_sreeen_back_function(gui_app_t *app, void *backfunc)
{
    app->screen->w = gui_get_screen_width();
    app->screen->h = gui_get_screen_height();
    gui_cb_set_with_param(app->screen, backfunc, RIGHT_SLIDE_CB, app);
}
static void screen_prepare(gui_obj_t *obj, struct gui_dispdev *dc, struct rtgui_rect *rect,
                             touch_info_t *tp)
{
    if (obj->callback.link_cb)
    {
        if ((tp->type == TOUCH_RIGHT_SLIDE && (tp->x>0&&tp->x<20))|| (tp->type == TOUCH_LEFT_SLIDE&& (tp->x>(gui_get_screen_width()-20)&&tp->x<gui_get_screen_width())))
        {
                rtgui_msg_t msg;
                msg.type = GUI_SRV_CB;
                msg.cb = obj->callback.link_cb;
                msg.u.payload = obj->callback.param;
                send_msg_to_gui_server(&msg);
        }

    }

}
void open_xml_app(const char *xml)
{
    gui_app_t *app = gui_malloc(sizeof(gui_app_t));

    memset(app, 0, sizeof(gui_app_t));
    char *app_name = NULL;
    bool startup = false;
    gui_log("ezxml_parse_file:%s\n",xml);
    ezxml_t f1 = ezxml_parse_file(xml);
    GUI_ASSERT(f1);
    if (!strcmp("app", f1->name))
    {
        app_name = get_space_string_head(f1->txt);
        size_t i = 0;
        while (true)
        {
            if (!(f1->attr[i]))
            {
                break;
            }
            //gui_log("p->attr[i]:%s\n", f1->attr[i]);
            //gui_log("p->attr[i+1]:%s\n", f1->attr[i + 1]);
            if (!strcmp(f1->attr[i], "startup"))
            {
                if (!strcmp(f1->attr[++i], "true"))
                {
                    startup = true;
                    break;
                }
            }
            i++;
        }
        ezxml_free(f1);
    }
    //gui_log("startup:%d\n", startup);
    gui_app_ctor_base(app, app_name, app_init, gui_app_startup_base, gui_app_close_base);
    //gui_list_append(&app_list, &(app->list_node));
    app->xml = xml;
    //if (startup)
    {
        gui_app_startup_base(app);
    }

}
*/
