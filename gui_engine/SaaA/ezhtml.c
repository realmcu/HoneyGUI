#include "ezxml.h"
#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_curtain.h>
#include "gui_button.h"
#include <gui_seekbar.h>
#include <gui_page.h>
#include "gui_gallery.h"
#include "gui_keyboard.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include <gui_img.h>
#include <gui_app.h>
#include <stdlib.h>
#include "gui_grid.h"
#include "gui_scroll_text.h"
#include "gui_switch.h"
#include "gui_tab.h"
#include "string.h"
#include "stdio.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "ezxml.h"
#include "gui_return.h"
#include "gui_slider.h"
#include "gui_scroll_wheel_new.h"
#include "gui_calendar.h"
#ifdef __arm__
#include "romfs.h"
#else
#include <sys/stat.h>
#include <fcntl.h>
#endif

#if defined __WIN32
#include <dirent.h>
#else

#endif

#include "gui_api.h"
#include "gui_multi_level.h"
//char *GUI_ROOT_FOLDER = GUI_ROOT_FOLDER;
struct widget_create
{
    char *name;
    T_OBJ_TYPE type;
};

#define BUTTON_HIGHLIGHT_ARRAY INT8_MAX

static struct widget_create widget[] =
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
    {"gallery", GALLERY},
    {"radio", RADIO},
    {"appendTexts", RADIOSWITCH},
    {"arc", ARC},
    {"movie", MOVIE},
    {"animateTransform", MACRO_ANIMATETRANSFORM},
    {"motorizedCurtain", MACRO_MOTORIZED_CURTAIN},
    {"keyboard", KEYBOARD},
    {"multiLevel", MULTI_LEVEL},
    {"onClick", MACRO_ONCLICK},
    {"backIcon", MACRO_BACKICON},
    {"slider", SLIDER},
    {"onChange", MACRO_ONCHANGE},
    {"onOn", MACRO_ONON},
    {"onOff", MACRO_ONOFF},
    {"onLoad", MACRO_ONLOAD},
    {"roller", TYPE_SCROLL_WHEEL_NEW},
    {"calendar", MACRO_CALENDAR},
};

typedef struct
{
    int x;
    int y;
    int translate_from_x;
    int translate_from_y;
    int translate_to_x;
    int translate_to_y;
    int rotate_x;
    int rotate_y;
    int rotate_from;
    int rotate_to;
    float scale_x;
    float scale_y;
    float scale_x_from;
    float scale_y_from;
    int scale_x_center;
    int scale_y_center;
    uint8_t opacity;
    uint8_t opacity_from;
    void **image_arr;
    int image_count;
    char *img_name;
    char *animate_type;
    gui_img_t *img;
} image_animate_params_t;
static void pause_animation_cb(gui_obj_t *this, void *null, char *to_name[]);
static void start_animation_cb(gui_obj_t *this, void *null, char *to_name[]);
static void foreach_create_animate(ezxml_t p, gui_obj_t *parent, const char *animate_name);
static ezxml_t f1;
static void img_rotate_cb(image_animate_params_t *animate_params, void *null,
                          gui_animate_t *animate)
{
    static  int i;

    gui_img_translate(animate_params->img, animate_params->rotate_x,
                      animate_params->rotate_y);
    // gui_log("img_rotate_cb%f\n", animate_params->rotate_from + (animate_params->rotate_to -
    //                                                             animate_params->rotate_from)*
    //         animate->progress_percent);
    gui_img_rotation(animate_params->img,
                     animate_params->rotate_from + (animate_params->rotate_to - animate_params->rotate_from)*
                     animate->progress_percent,
                     animate_params->rotate_x,
                     animate_params->rotate_y);
}

static void img_translate_cb(image_animate_params_t *animate_params, void *null,
                             gui_animate_t *animate)
{
    float t_x = animate_params->translate_from_x + animate->progress_percent *
                (animate_params->translate_to_x -
                 animate_params->translate_from_x);
    float t_y = animate_params->translate_from_y + animate->progress_percent *
                (animate_params->translate_to_y -
                 animate_params->translate_from_y);

    gui_img_translate(animate_params->img, t_x, t_y);
}

static void img_scale_cb(image_animate_params_t *animate_params, void *null, gui_animate_t *animate)
{
    float scale_x = animate_params->scale_x_from - animate->progress_percent *
                    (animate_params->scale_x_from - animate_params->scale_x);
    float scale_y = animate_params->scale_y_from - animate->progress_percent *
                    (animate_params->scale_y_from - animate_params->scale_y);

    gui_img_scale(animate_params->img, scale_x, scale_y);
    gui_img_translate(animate_params->img, animate_params->scale_x_center,
                      animate_params->scale_y_center);

    gui_img_rotation(animate_params->img, 0, animate_params->scale_x_center,
                     animate_params->scale_y_center);
}

static void img_opacity_cb(image_animate_params_t *animate_params, void *null,
                           gui_animate_t *animate)
{
    uint8_t opacity = animate_params->opacity_from - animate->progress_percent *
                      (animate_params->opacity_from - animate_params->opacity);
    //gui_log("img_opacity_cb%d\n", opacity);
    gui_img_set_opacity(animate_params->img, opacity);
}
static void image_animate_callback(void *params, void *null, gui_animate_t *animate);
static void multi_animate_callback(void *params, void *null, gui_animate_t *animate)
{
    image_animate_params_t *animate_params = (image_animate_params_t *)params;
    //gui_log("multi_animate_callback:%x,%s\n", animate_params, animate_params->animate_type);
    if (!strcmp(animate_params->animate_type, "rotate"))
    {
        img_rotate_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "translate"))
    {
        img_translate_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "scale"))
    {
        img_scale_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "opacity"))
    {
        img_opacity_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "rotate_translate"))
    {
        img_rotate_cb(params, null, animate);
        img_translate_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "rotate_scale"))
    {
        img_rotate_cb(params, null, animate);
        img_scale_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "rotate_opacity"))
    {
        img_rotate_cb(params, null, animate);
        img_opacity_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "translate_scale"))
    {
        img_translate_cb(params, null, animate);
        img_scale_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "translate_opacity"))
    {
        img_translate_cb(params, null, animate);
        img_opacity_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "scale_opacity"))
    {
        img_scale_cb(params, null, animate);
        img_opacity_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "rotate_translate_scale"))
    {
        img_rotate_cb(params, null, animate);
        img_translate_cb(params, null, animate);
        img_scale_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "rotate_translate_opacity"))
    {
        img_rotate_cb(params, null, animate);
        img_translate_cb(params, null, animate);
        img_opacity_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "translate_scale_opacity"))
    {
        img_translate_cb(params, null, animate);
        img_scale_cb(params, null, animate);
        img_opacity_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "rotate_translate_scale_opacity"))
    {
        img_rotate_cb(params, null, animate);
        img_translate_cb(params, null, animate);
        img_scale_cb(params, null, animate);
        img_opacity_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "image frame"))
    {
        image_animate_callback(params, null, animate);
    }
}

static void image_animate_callback(void *params, void *null, gui_animate_t *animate)
{
    int current_image_index = 0;
    image_animate_params_t *animate_params = (image_animate_params_t *)params;

    if (animate_params->image_count > 0)
    {
        current_image_index = animate->progress_percent * (animate_params->image_count - 1);
        gui_img_set_attribute(animate_params->img, GET_BASE(animate_params->img)->name,
                              animate_params->image_arr[current_image_index], GET_BASE(animate_params->img)->x,
                              GET_BASE(animate_params->img)->y);
    }

}

#define LIGHTS_NUM 20
char light_value_text[30];
typedef struct
{
    int id;
    bool state;
    bool has_brightness;
    bool has_colorTemperature;
    int brightness;
    int colorTemperature;
    char *prefix;
    char *postfix;
    gui_text_t *text;
} light_param_t;

light_param_t lights[LIGHTS_NUM];

static char *read_file(const char *path)
{
    FILE *file = fopen(path, "r");
    if (!file)
    {
        perror("fopen");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content = (char *)malloc(length + 1);
    if (content)
    {
        fread(content, 1, length, file);
        content[length] = '\0';
    }
    fclose(file);
    return content;
}

void parse_json(const char *json_str)
{
    const char *ptr = json_str;
    int index = 0;

    while ((ptr = strstr(ptr, "\"id\"")) != NULL && index < LIGHTS_NUM)
    {
        char state_buffer[10];
        bool has_brightness = false;
        bool has_colorTemperature = false;

        sscanf(ptr, "\"id\": %d", &lights[index].id);

        ptr = strstr(ptr, "\"state\"");
        sscanf(ptr, "\"state\": \"%9[^\"]\"", state_buffer);
        lights[index].state = (strcmp(state_buffer, "on") == 0);

        const char *br_ptr = strstr(ptr, "\"brightness\"");
        if (br_ptr != NULL && (br_ptr - ptr) < (strchr(ptr, '}') - ptr))
        {
            sscanf(br_ptr, "\"brightness\": %d", &lights[index].brightness);
            has_brightness = true;
        }
        lights[index].has_brightness = has_brightness;

        const char *ct_ptr = strstr(ptr, "\"colorTemperature\"");
        if (ct_ptr != NULL && (ct_ptr - ptr) < (strchr(ptr, '}') - ptr))
        {
            sscanf(ct_ptr, "\"colorTemperature\": %d", &lights[index].colorTemperature);
            has_colorTemperature = true;
        }
        lights[index].has_colorTemperature = has_colorTemperature;

        index++;
    }
}

void write_file(const char *filename, const char *content)
{
    FILE *file = fopen(filename, "w");
    if (file != NULL)
    {
        fwrite(content, 1, strlen(content), file);
        fclose(file);
    }
}

void update_light_config(int index, const char *attribute, int value)
{
    char *config_file_path = "./gui_engine/example/web/peripheral_simulation/json/light_config.json";
    char *json_str = read_file(config_file_path);
    if (json_str)
    {
        parse_json(json_str);
        free(json_str);
    }

    for (int i = 0; i < LIGHTS_NUM; i++)
    {
        if (lights[i].id == index)
        {
            if (strcmp(attribute, "state") == 0)
            {
                lights[i].state = value;
            }
            else if (strcmp(attribute, "brightness") == 0)
            {
                lights[i].brightness = value;
            }
            else if (strcmp(attribute, "colorTemperature") == 0)
            {
                lights[i].colorTemperature = value;
            }
            break;
        }
    }

    char config_str[2048] = "{\n    \"lights\": [\n";
    for (int i = 0; i < LIGHTS_NUM; i++)
    {
        char light_str[256];
        snprintf(light_str, sizeof(light_str),
                 "        {\"id\": %d, \"state\": \"%s\"",
                 lights[i].id, lights[i].state ? "on" : "off");
        strcat(config_str, light_str);

        if (lights[i].has_brightness)
        {
            char brightness_str[50];
            snprintf(brightness_str, sizeof(brightness_str), ", \"brightness\": %d", lights[i].brightness);
            strcat(config_str, brightness_str);
        }

        if (lights[i].has_colorTemperature)
        {
            char color_temp_str[50];
            snprintf(color_temp_str, sizeof(color_temp_str), ", \"colorTemperature\": %d",
                     lights[i].colorTemperature);
            strcat(config_str, color_temp_str);
        }
        strcat(config_str, (i < LIGHTS_NUM - 1) ? "},\n" : "}\n");
    }
    strcat(config_str, "    ]\n}");

    write_file(config_file_path, config_str);
}

static void light_control_cb(void *obj, gui_event_t e, light_param_t *light)
{
    gui_log("light->id: %d, light->state: %d\n", light->id, light->state);
    update_light_config(light->id, "state", light->state);
}

static void light_switch_on_cb(void *obj, gui_event_t e, light_param_t *light)
{
    light->state = true;
    gui_log("light->id: %d, light->state: %d\n", light->id, light->state);
    update_light_config(light->id, "state", light->state);
}

static void light_switch_off_cb(void *obj, gui_event_t e, light_param_t *light)
{
    light->state = false;
    gui_log("light->id: %d, light->state: %d\n", light->id, light->state);
    update_light_config(light->id, "state", light->state);
}

static void light_brightness_cb(void *obj, gui_event_t e, light_param_t *light)
{
    gui_seekbar_t *this = (gui_seekbar_t *)obj;
    light->brightness = 100 * GUI_API(gui_seekbar_t).get_progress(this);
    gui_log("light->id: %d  light->brightness: %d\n", light->id, light->brightness);
    update_light_config(light->id, "brightness", light->brightness);
}

static void light_colorTemp_cb(void *obj, gui_event_t e, light_param_t *light)
{
    gui_slider_t *this = (gui_slider_t *)obj;
    light->colorTemperature = GUI_API(gui_slider_t).get_currentValue(this);
    update_light_config(light->id,  "colorTemperature", light->colorTemperature);
}
static void slider_write_text_cb(void *obj, gui_event_t e, char *text_name)
{
    gui_slider_t *this = (gui_slider_t *)obj;
    gui_text_t *text = 0;
    gui_obj_tree_get_widget_by_type((gui_obj_t *)gui_current_app(), TEXTBOX, (gui_obj_t **)&text);

    snprintf(light_value_text, sizeof(light_value_text), "%d",
             GUI_API(gui_slider_t).get_currentValue(this));
    gui_text_content_set(text, light_value_text, strlen(light_value_text));
}
static char *open_switch_name;
static char *pause_switch_name;
static char *close_switch_name;
static void create_tree_in_multi_level(gui_app_t *app, gui_multi_level_t *parent);
static void setting_return_cb(void *obj, gui_event_t e, void *param);
static void **image_array;
static void *return_image;
static void *return_image_hl;
static int file_count;
static void seekbar_ani_cb(int args, gui_seekbar_t *this)
{
    switch (args)
    {
    case 0:
        {
            GUI_API(gui_seekbar_t).set_progress(this, this->animate->progress_percent);
        }
        break;
    case 1:
        {
            GUI_API(gui_seekbar_t).set_progress(this, 1.0f - this->animate->progress_percent);
        }
        break;
    default:
        break;
    }
}
static void switch_cb_for_mororized_curtain(gui_switch_t *sw, gui_event_t event_code)
{
    size_t args = 0;
    gui_event_dsc_t *event = GUI_BASE(sw)->event_dsc;

    // same event only handle the first register one
    for (size_t i = 0; i < GUI_BASE(sw)->event_dsc_cnt; i++)
    {
        if (event[i].filter == event_code)
        {
            args = (size_t)event[i].user_data;
        }
    }
    gui_seekbar_t *seekbar = 0;
    gui_obj_tree_get_widget_by_type(GUI_BASE(sw)->parent, SEEKBAR, (gui_obj_t **)&seekbar);
    switch (args)
    {
    case 3:
        {
            if (seekbar->animate)
            {
                seekbar->animate->animate = 1;
            }
        }
        break;
    case 2:
        {
            if (seekbar->animate)
            {
                seekbar->animate->animate = 0;
            }


        }
        break;
    default:
        {
            GUI_API(gui_seekbar_t).animate(seekbar, 10000, 0, seekbar_ani_cb, (void *)args);
        }
        break;
    }
}
gui_img_t *xml_gui_img_create_from_mem(void *parent,  const char *name, void *addr,
                                       int16_t x,
                                       int16_t y)
{
    if (addr == NULL)
    {
        addr = gui_get_file_address("app/system/resource/icMenuBird.bin");
    }

    return gui_img_create_from_mem(parent, name, addr,
                                   x,
                                   y, 0, 0);
}

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
    uint32_t c = strtoul(color + 1, NULL, 16);
    gui_log("color%s:%x,%x\n", color, c, RGB888ToRGB565(c));
    return RGB888ToRGB565(c);
}
static gui_color_t string_rgb888(const char *color)
{
    return gui_color_css(color);
}
static void sport_button_press_ani_cb(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    float alpha_to = 100;
    static float alpha_from = 0;
    static float from = 0;
    if (per == 0.0f)
    {
        from = gui_img_get_transform_scale_x((gui_img_t *)(button->img));
        alpha_from = button->img->draw_img->opacity_value;
    }
    from = 1.0f;
    float scale = (-0.5f * from) * per + from;
    //circle_color_tab[1] = change_opacity_of_rgba(circle_color_tab[1],
    //                                             (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    if (button->style == WIDGET_FADE || button->style == WIDGET_SCALE_FADE)
    {
        gui_img_set_opacity((void *)button->img,
                            (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    }
    if (button->style == WIDGET_SCALE || button->style == WIDGET_SCALE_FADE)
    {
        gui_img_scale((void *)button->img, scale,  scale);
        //gui_img_translate((void *)button->img, 25.0f * (from - scale),  25.0f * (from - scale));
    }



    //gui_log("o:%f\n", 25.0f * (from - scale));
}
static void reverse_array(void *arr[], int n)
{
    void *temp;
    for (int i = 0; i < n / 2; i++)
    {
        temp = arr[i];
        arr[i] = arr[n - i - 1];
        arr[n - i - 1] = temp;
    }
}
static void draw_arc(gui_canvas_t *c)
{

    nvgBeginPath(c->vg);
    nvgLineCap(c->vg, NVG_ROUND);
    /*can't get arc data*/
    //nvgArc(c->vg, this->cx, this->cy, this->r, this->a0, this->a1, NVG_CW);
    //nvgStrokeColor(c->vg, this->color);
    //nvgStrokeWidth(c->vg, this->width);
    nvgArc(c->vg, 100, 100, 20, 1, 2, NVG_CW);
    nvgStrokeColor(c->vg, nvgRGBA(255, 255, 255, 255));
    nvgStrokeWidth(c->vg, 3);
    nvgStroke(c->vg);

}
static void button_press_ani_cb_array(int p, gui_button_t *button)
{
    float per = button->animate->progress_percent;
    if (p == -1)
    {
        button->img->data = button->off_pic_addr;
    }
    else if (button->style == BUTTON_HIGHLIGHT_ARRAY)
    {
        float per = button->animate->progress_percent;
        void **image_array = (void **)button->data;
        button->img->data = image_array[(int)((p - 1) * per)];
    }
}
static void switch_press_ani_cb_array(int p, gui_switch_t *button)
{
    float per = button->animate->progress_percent;
    if (p == -1)
    {
        button->switch_picture->data = button->off_pic_addr;
    }
    else if (button->style == SWITCH_HIGHLIGHT_ARRAY)
    {
        float per = button->animate->progress_percent;
        void **image_array = (void **)button->data;
        button->switch_picture->data = image_array[(int)((p - 1) * per)];
    }
}
static void sport_button_release_ani_cb(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    float alpha_to = UINT8_MAX;
    static float alpha_from = 0;
    static float from = 0;
    if (per == 0.0f)
    {
        from = gui_img_get_transform_scale_x((gui_img_t *)(button->img));
        alpha_from = button->img->draw_img->opacity_value;
    }
    //from = 1.25f;
    float scale = (1.0f - from) * per + from;
    //circle_color_tab[1] = change_opacity_of_rgba(circle_color_tab[1],
    //                                             (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));


    //gui_img_translate((void *)button->img, 25.0f * (1.0f - scale),  25.0f * (1.0f - scale));
    if (button->style == WIDGET_FADE || button->style == WIDGET_SCALE_FADE)
    {
        gui_img_set_opacity((void *)button->img,
                            (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    }
    if (button->style == WIDGET_SCALE || button->style == WIDGET_SCALE_FADE)
    {
        gui_img_scale((void *)button->img, scale,  scale);
    }
}
static void sport_button_press(gui_button_t *b)
{
    gui_button_set_animate(b, 200, 0, sport_button_press_ani_cb, b);
}
static void button_press_array(gui_button_t *b, void *code, int p)
{
    GUI_API(gui_button_t).animate(b, 200, 0, button_press_ani_cb_array, (void *)(uintptr_t)p);
}
static void switch_press_array(gui_switch_t *b, void *code, int p)
{
    if (!b->ifon)
    {
        GUI_API(gui_switch_t).animate(b, b->animate->dur, 0, switch_press_ani_cb_array,
                                      (void *)(uintptr_t)p);
    }


}
static void switch_release_array(gui_switch_t *b, void *code, int p)
{
    b->switch_picture->data = ((void **)(b->data))[p - 1];
}
static void sport_button_release(gui_button_t *b)
{
    gui_button_set_animate(b, 200, 0, sport_button_release_ani_cb, b);
}
static void multi_level_ui_design(gui_multi_level_t *obj)
{
    create_tree_in_multi_level(gui_current_app(), obj);
}
struct on_click_jump_cb_param
{
    int id1;
    int id2;
};

static void on_click_jump_cb(void *obj, gui_event_t e, struct on_click_jump_cb_param *param)
{
    if (param->id1 < 0)
    {
        setting_return_cb(obj, e, (void *)param);
    }
    else
    {
        GUI_API(gui_multi_level_t).jump(obj, param->id1, param->id2);
    }


}
gui_obj_t *widget_create_handle(ezxml_t p, gui_obj_t *parent)
{
    char *name = p->name;
    gui_log("widget_create_handle:%s: %s", name, p->txt);
    for (size_t i = 0; i < sizeof(widget) / sizeof(widget[0]); i++)
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
#ifdef __WIN32
                    char *font = "app/system/resource/font/tangyuanti.ttf";
#elif defined RTL8772F
                    char *font = "app/system/resource/font/tangyuanti.ttf";
#else
                    char *font =
                        "app/system/resource/font/gbk_32_32_dot.bin;app/system/resource/font/gbk_unicode_table.bin";
#endif
                    gui_color_t color = APP_COLOR_WHITE;
                    int fontSize = 32;
                    TEXT_MODE style = 0;
                    uint8_t inputable = false;
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
                        else if (!strcmp(p->attr[i], "inputable"))
                        {
                            inputable = atoi(p->attr[++i]);
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
                            t = gui_text_create(parent, ptxt, x, y, w, h);
                            gui_color_t color_temporary;
                            color_temporary = color;
                            gui_text_set(t, gui_strdup(text), GUI_FONT_SRC_BMP, color_temporary, strlen(text), 32);
                            if (inputable)
                            {
                                gui_text_input_set(t, inputable);
                                gui_text_click(t, gui_keyboard_launch_by_widget, t);
                            }


                            {
                                FONT_SRC_TYPE font_type2; GUI_UNUSED(font_type2);
                                char *font_type = font;
                                if (strstr(font_type, ".bin") != NULL)
                                {
                                    font_type2 = GUI_FONT_SRC_BMP;
                                    void *addr1 = gui_get_file_address(font_type);
                                    //gui_font_mem_init(addr1);
                                    t->font_height = fontSize;
                                    t->path = 0;
                                    t->font_type = GUI_FONT_SRC_BMP;
                                    gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
                                    gui_text_mode_set(t, LEFT);
                                    // t->font_height = fontSize;
                                    //t->path = 0;
                                }
                                else if ((strstr(font_type, ".ttf") != NULL) || (strstr(font_type, ".TTF") != NULL))
                                {
#ifdef __WIN32
                                    font_type2 = GUI_FONT_SRC_TTF;
                                    t->path = gui_get_file_address(font);
                                    t->font_height = fontSize;
                                    t->font_type = GUI_FONT_SRC_TTF;
#elif defined RTL8772F
                                    font_type2 = GUI_FONT_SRC_TTF;
                                    t->path = gui_get_file_address(font);
                                    t->font_height = fontSize;
                                    t->font_type = GUI_FONT_SRC_TTF;
#else
                                    font_type =
                                        "app/system/resource/font/gbk_32_32_dot.bin;app/system/resource/font/gbk_unicode_table.bin";
                                    {
                                        font_type2 = GUI_FONT_SRC_BMP;
                                        char b[100] = {0};
                                        strncpy(b, font_type, strstr(font_type, ".bin;") - font_type + strlen(".bin"));
                                        void *addr1 = gui_get_file_address(b);
                                        memset(b, 0, sizeof(b));
                                        char *a = font_type;
                                        strncpy(b, strstr(a, ".bin;") + strlen(".bin;"), strlen(a) - (strstr(a,
                                                                                                             ".bin;") - a + strlen(".bin;")));
                                        void *addr2 = gui_get_file_address(b);
                                        // gui_set_font_mem_resourse(fontSize, addr1,  addr2);
                                        t->path = 0;
                                        t->font_type = GUI_FONT_SRC_BMP;
                                    }
#endif
                                }

                            }
                        }
                        else
                        {
                            gui_scroll_text_t *scroll_text = gui_scroll_text_create(parent,  ptxt, x, y,
                                                                                    strlen(text) * (fontSize / 4), h);
                            gui_scroll_text_scroll_set(scroll_text, style, 100, 0, 3000, 1500000);
                            gui_color_t color_temporary;
                            color_temporary = color;
                            gui_scroll_text_set(scroll_text, gui_strdup(text), GUI_FONT_SRC_TTF, color_temporary,
                                                strlen(text),
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
                    int16_t w = 0; GUI_UNUSED(w);
                    int16_t h = 0; GUI_UNUSED(h);
                    char *file = NULL;
                    char *folder = NULL;
                    int count = 0;
                    uint32_t duration;
                    float scalex = 1;
                    float scaley = 1;
                    float angle = 0;
                    // default image blend_mode
                    BLEND_MODE_TYPE blendMode = IMG_FILTER_BLACK;
                    uint8_t opacity = 255;
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
                        else if (!strcmp(p->attr[i], "blendMode"))
                        {
                            i++;
                            if (!strcmp(p->attr[i], "imgBypassMode"))
                            {
                                blendMode = IMG_BYPASS_MODE;
                            }
                            else if (!strcmp(p->attr[i], "imgFilterBlack"))
                            {
                                blendMode = IMG_FILTER_BLACK;
                            }
                            else if (!strcmp(p->attr[i], "imgSrcOverMode"))
                            {
                                blendMode = IMG_SRC_OVER_MODE;
                            }
                            else if (!strcmp(p->attr[i], "imgCoverMode"))
                            {
                                blendMode = IMG_COVER_MODE;
                            }
                        }
                        else if (!strcmp(p->attr[i], "opacity"))
                        {
                            opacity = atof(p->attr[++i]);
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
                        else if (!strcmp(p->attr[i], "folder"))
                        {
                            folder = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "duration"))
                        {
                            duration = atoi(p->attr[++i]);
                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    //gui_log("x:%d,y:%d,w:%dh:%d,file:%s\n", x, y, w, h, file);
                    if (folder)
                    {
                        int file_count = 0;
                        {
                            DIR *dir = NULL;
                            struct dirent *entry;
                            char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                            sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                            if ((dir = opendir(path)) == NULL)
                            {
                                gui_free(path);
                                return 0;
                            }
                            gui_free(path);
                            while ((entry = readdir(dir)) != NULL)
                            {
                                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                {
                                    file_count++;
                                }
                            }
                            closedir(dir);
                        }

                        void **image_array = gui_malloc(file_count * sizeof(void *));
                        {
                            DIR *dir = NULL;
                            struct dirent *entry;
                            char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                            sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                            if ((dir = opendir(path)) == NULL)
                            {
                                gui_free(path);
                                return 0;
                            }


                            while ((entry = readdir(dir)) != NULL)
                            {
                                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                {
                                    char *path2 = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                                    sprintf(path2, "%s/%s", folder, entry->d_name);
                                    image_array[count++] = gui_get_file_address(path2);
                                }
                            }
                            gui_free(path);
                            closedir(dir);
                        }
                        parent = (void *)xml_gui_img_create_from_mem(parent, ptxt, image_array[0], x, y);

                        image_animate_params_t *params = gui_malloc(sizeof(image_animate_params_t));
                        params->image_arr = image_array;
                        params->image_count = count;
                        params->img = (gui_img_t *)parent;
                        params->img_name = ptxt;

                        gui_img_set_animate((gui_img_t *)parent, duration, -1, image_animate_callback, params);
                        gui_img_set_mode((gui_img_t *)parent, blendMode);
                        gui_img_set_opacity((gui_img_t *)parent, opacity);
                    }
                    else if (file)
                    {
                        void *imgbuf = gui_get_file_address(file);
                        parent = (void *)xml_gui_img_create_from_mem(parent, ptxt, imgbuf, x, y);
                        if (scalex != 1 || scaley != 1)
                        {
                            gui_img_scale((void *)parent, scalex, scaley);
                        }
                        if (angle != 0)
                        {
                            gui_img_get_height((void *)parent);
                            gui_img_rotation((void *)parent, angle, parent->w / 2, parent->h / 2);
                        }
                        gui_img_set_mode((gui_img_t *)parent, blendMode);
                        gui_img_set_opacity((gui_img_t *)parent, opacity);
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
                    bool hide = false;
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
                        else if (!strcmp(p->attr[i], "hidden"))
                        {
                            hide = true;
                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    //gui_log("x:%d,y:%d,w:%dh:%d\n", x, y, w, h);
                    {
                        parent = (void *)gui_win_create(parent, ptxt, x, y, w, h);
                        if (hide)
                        {
                            parent->not_show = 1;
                        }


                    }
                }
                break;
            case TABVIEW:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    int style = 0; GUI_UNUSED(style);
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
                                style = CLASSIC;
                            }
                            else if (!strcmp(p->attr[i], "fade"))
                            {
                                style = FADE;
                            }
                            else if (!strcmp(p->attr[i], "scale"))
                            {
                                style = REDUCTION;
                            }
                            else if (!strcmp(p->attr[i], "fadeScale"))
                            {
                                style = REDUCTION_FADE;
                            }

                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    parent = (void *)gui_tabview_create(parent, ptxt, x, y, w, h);
                    gui_tabview_loop_x((void *)parent, true);
                }
                break;
            case ARC:
                {
                    size_t i = 0;
                    int16_t cx = 100; GUI_UNUSED(cx);
                    int16_t cy = 100; GUI_UNUSED(cy);
                    int16_t r = 100; GUI_UNUSED(r);
                    int16_t stroke_width = 10; GUI_UNUSED(stroke_width);
                    int cap = 0; GUI_UNUSED(cap);
                    gui_color_t fill = APP_COLOR_RED;
                    gui_color_t stroke = APP_COLOR_RED;
                    float sd = 0; GUI_UNUSED(sd);
                    float ed = 100; GUI_UNUSED(ed);
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
                                cap = NVG_BUTT;
                            }
                            else if (!strcmp(p->attr[i], "round"))
                            {
                                cap = NVG_ROUND;
                            }
                            else if (!strcmp(p->attr[i], "square"))
                            {
                                cap = NVG_SQUARE;
                            }
                        }
                        else if (!strcmp(p->attr[i], "fill"))
                        {
                            fill = string_rgb888(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "stroke") || !strcmp(p->attr[i], "color"))
                        {
                            stroke = string_rgb888(p->attr[++i]);
                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    parent = (void *)gui_canvas_create(parent, ptxt, 0, 0, 454, 454, 0);
                    gui_canvas_set_canvas_cb((void *)parent, draw_arc);
                    /*GUI_TYPE(gui_canvas_t, parent)->draw = draw_arc;
                    GUI_TYPE(gui_canvas_t, parent)->cx = cx;
                    GUI_TYPE(gui_canvas_t, parent)->cy = cy;
                    GUI_TYPE(gui_canvas_t, parent)->r = r;
                    GUI_TYPE(gui_canvas_t, parent)->sw = stroke_width;
                    GUI_TYPE(gui_canvas_t, parent)->s = stroke;
                    GUI_TYPE(gui_canvas_t, parent)->ed = ed;
                    GUI_TYPE(gui_canvas_t, parent)->sd = sd;*/


                }
                break;
            case PROGRESSBAR:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0; GUI_UNUSED(w);
                    int16_t h = 0; GUI_UNUSED(h);
                    gui_color_t color = APP_COLOR_WHITE_OPACITY;
                    gui_color_t highlightColor = APP_COLOR_WHITE;
                    bool vh = false;
                    bool canvas = false;
                    char *picture = "app/system/resource/Progress bar_full.bin";
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
                        else if (!strcmp(p->attr[i], "canvasOrPicture"))
                        {
                            if (!strcmp(p->attr[++i], "canvas"))
                            {
                                canvas = true;
                            }
                            else
                            {
                                canvas = false;
                            }

                        }
                        else if (!strcmp(p->attr[i], "picture"))
                        {
                            picture = gui_strdup(p->attr[++i]);
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
                        else if (!strcmp(p->attr[i], "orientation"))
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
                    if (canvas == true)
                    {
#if 0
                        if (vh)
                        {
                            //parent = (void *)gui_progressbar_v_create(parent, ptxt, x, y, w, h);
                        }
                        else
                        {
                            //parent = (void *)gui_progressbar_create(parent, ptxt, x, y, w, h);
                        }

                        GUI_TYPE(gui_progressbar_t, parent)->color = color;
                        GUI_TYPE(gui_progressbar_t, parent)->color_hl = highlightColor;
#endif
                    }
                    else
                    {
                        if (vh)
                        {
                            parent = (void *)gui_progressbar_img_v_create(parent, gui_get_file_address(picture), x, y);
                        }
                        else
                        {
                            parent = (void *)gui_progressbar_img_h_create(parent, gui_get_file_address(picture), x, y);
                        }
                    }
                    parent->name = ptxt;




                }
                break;
            case SEEKBAR:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0; GUI_UNUSED(w);
                    int16_t h = 0; GUI_UNUSED(h);
                    gui_color_t color = APP_COLOR_WHITE_OPACITY;
                    gui_color_t highlightColor = APP_COLOR_WHITE;
                    bool canvas = false;
                    bool arc = false;
                    char *picture = "app/system/resource/Progress bar_full.bin";
                    char *folder = NULL;
                    bool vh = false;
                    int16_t cx = 0;
                    int16_t cy = 0;

                    float sd = 0;
                    float ed = 0;
                    // default image blend_mode
                    BLEND_MODE_TYPE blendMode = IMG_FILTER_BLACK;
                    uint8_t opacity = 255;
                    bool reverse = 0;
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
                        else if (!strcmp(p->attr[i], "canvasOrPicture"))
                        {
                            if (!strcmp(p->attr[++i], "canvas"))
                            {
                                canvas = true;
                            }
                            else
                            {
                                canvas = false;
                            }

                        }
                        else if (!strcmp(p->attr[i], "folder"))
                        {
                            folder = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "picture"))
                        {
                            picture = gui_strdup(p->attr[++i]);
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
                        else if (!strcmp(p->attr[i], "orientation"))
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
                            else if (!strcmp(p->attr[i], "arc"))
                            {
                                arc = true;
                            }

                        }
                        else if (!strcmp(p->attr[i], "cx") || !strcmp(p->attr[i], "centralX"))
                        {
                            cx = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "cy") || !strcmp(p->attr[i], "centralY"))
                        {
                            cy = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "startDegree"))
                        {
                            sd = atof(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "endDegree"))
                        {
                            ed = atof(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "reverse"))
                        {
                            reverse = 1;
                        }
                        else if (!strcmp(p->attr[i], "blendMode"))
                        {
                            i++;
                            if (!strcmp(p->attr[i], "imgBypassMode"))
                            {
                                blendMode = IMG_BYPASS_MODE;
                            }
                            else if (!strcmp(p->attr[i], "imgFilterBlack"))
                            {
                                blendMode = IMG_FILTER_BLACK;
                            }
                            else if (!strcmp(p->attr[i], "imgSrcOverMode"))
                            {
                                blendMode = IMG_SRC_OVER_MODE;
                            }
                            else if (!strcmp(p->attr[i], "imgCoverMode"))
                            {
                                blendMode = IMG_COVER_MODE;
                            }
                        }
                        else if (!strcmp(p->attr[i], "opacity"))
                        {
                            opacity = atof(p->attr[++i]);
                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    if (canvas)
                    {
#if 0
                        /* code */


                        if (vh)
                        {
                            //parent = (void *)gui_seekbar_create(parent, ptxt, x, y, w, h);
                        }
                        else
                        {
                            //parent = (void *)gui_seekbar_h_create(parent, ptxt, x, y, w, h);
                        }

                        GUI_TYPE(gui_progressbar_t, parent)->color = color;
                        GUI_TYPE(gui_progressbar_t, parent)->color_hl = highlightColor;
#endif
                    }
                    else if (!folder)
                    {

                        if (vh)
                        {
                            parent = (void *)gui_seekbar_create_img_v(parent, gui_get_file_address(picture), x, y);
                        }
                        else
                        {
                            parent = (void *)gui_seekbar_create_img_h(parent, gui_get_file_address(picture), x, y);
                        }
                        gui_img_set_mode(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), blendMode);
                        gui_img_set_opacity(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), opacity);
                    }
                    else if (folder)
                    {
                        int file_count = 0;

                        {
                            DIR *dir = 0;
                            struct dirent *entry;
                            char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                            sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                            if ((dir = opendir(path)) == NULL)
                            {
                                gui_free(path);
                                return 0;
                            }
                            gui_free(path);
                            while ((entry = readdir(dir)) != NULL)
                            {
                                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                {
                                    file_count++;
                                }


                            }
                            closedir(dir);
                        }
                        void **image_array = gui_malloc(file_count * sizeof(void *));
                        {
                            DIR *dir = 0;
                            struct dirent *entry;
                            char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                            sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                            if ((dir = opendir(path)) == NULL)
                            {
                                gui_free(path);
                                return 0;
                            }

                            int count = 0;
                            while ((entry = readdir(dir)) != NULL)
                            {
                                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                {
                                    char *path2 = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                                    sprintf(path2, "%s/%s", folder, entry->d_name);
                                    image_array[count++] = gui_get_file_address(path2);
                                }

                            }
                            gui_free(path);
                            closedir(dir);
                            if (reverse)
                            {
                                reverse_array(image_array, count);
                            }


                        }
                        bool if_widget = 0;
                        if (p->parent)
                        {
                            if (!strcmp(widget[21].name, p->parent->name))
                            {
                                if_widget = 1;
                            }
                        }
                        if (if_widget)
                        {
                            parent = (void *)gui_seekbar_create_movie_h_double(parent, image_array, file_count, x, y);
                            gui_img_set_mode(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), blendMode);
                            gui_img_set_opacity(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), opacity);
                        }
                        else
                        {
                            if (arc)
                            {
                                parent = (void *)gui_seekbar_create_movie_arc(parent, image_array, file_count, x, y,
                                                                              cx, cy, 100, 100, sd, ed);
                                gui_img_set_mode(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), blendMode);
                                gui_img_set_opacity(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), opacity);
                            }
                            else
                            {
                                if (vh)
                                {
                                    parent = (void *)gui_seekbar_create_movie_v(parent, image_array, file_count, x, y);
                                }
                                else
                                {
                                    parent = (void *)gui_seekbar_create_movie_h(parent, image_array, file_count, x, y);
                                }
                                gui_img_set_mode(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), blendMode);
                                gui_img_set_opacity(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), opacity);
                            }
                        }





                    }

                    parent->name = ptxt;



                }
                break;
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
                                style = WIDGET_CLASSIC;
                            }
                            else if (!strcmp(p->attr[i], "fade"))
                            {
                                style = WIDGET_FADE;
                            }
                            else if (!strcmp(p->attr[i], "scale"))
                            {
                                style = WIDGET_SCALE;
                            }
                            else if (!strcmp(p->attr[i], "fadeScale"))
                            {
                                style = WIDGET_SCALE_FADE;
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
                    parent = (void *)gui_grid_create(parent, x, y, rowNumber, colNumber, colGap, rowGap);
                    parent->name = ptxt;
                }
                break;
            case SCREEN:
                {
                    size_t i = 0;
                    int16_t w = 0; GUI_UNUSED(w);
                    int16_t h = 0; GUI_UNUSED(h);
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
            case CURTAIN:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    float scope = 1.0f;
                    T_GUI_CURTAIN_ENUM orientation = CURTAIN_MIDDLE;
                    int style = 0; GUI_UNUSED(style);
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
                                style = WIDGET_CLASSIC;
                            }
                            else if (!strcmp(p->attr[i], "fade"))
                            {
                                style = WIDGET_FADE;
                            }
                            else if (!strcmp(p->attr[i], "scale"))
                            {
                                style = WIDGET_SCALE;
                            }
                            else if (!strcmp(p->attr[i], "fadeScale"))
                            {
                                style = WIDGET_SCALE_FADE;
                            }

                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    //gui_log("x:%d,y:%d,w:%dh:%d,orientation:%d\n", x, y, w, h, orientation);
                    parent = (void *)gui_curtain_create(parent, ptxt, x, y, w, h, orientation, scope);
                }
                break;
            case ICON:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    char *font_type = "app/system/resource/font/tangyuanti.ttf";
                    char *text = NULL;
                    int text_x = 0;
                    int text_y = 0;
                    gui_color_t font_color = APP_COLOR_RED;
                    uint32_t font_size = 32;
                    int picture_x = 0;
                    int picture_y = 0;
                    int transition = 0; GUI_UNUSED(transition);
                    char *picture = NULL;
                    char *hl_picture = NULL;
                    int style = 0;
                    // default image blend_mode
                    BLEND_MODE_TYPE blendMode = IMG_FILTER_BLACK;
                    uint8_t opacity = 255;
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
                            //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                            font_color = string_rgb888(p->attr[++i]);
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
                                style = WIDGET_CLASSIC;
                            }
                            else if (!strcmp(p->attr[i], "fade"))
                            {
                                style = WIDGET_FADE;
                            }
                            else if (!strcmp(p->attr[i], "scale"))
                            {
                                style = WIDGET_SCALE;
                            }
                            else if (!strcmp(p->attr[i], "fadeScale"))
                            {
                                style = WIDGET_SCALE_FADE;
                            }
                            else if (!strcmp(p->attr[i], "array"))
                            {
                                style = BUTTON_HIGHLIGHT_ARRAY;
                            }
                        }
                        else if (!strcmp(p->attr[i], "blendMode"))
                        {
                            i++;
                            if (!strcmp(p->attr[i], "imgBypassMode"))
                            {
                                blendMode = IMG_BYPASS_MODE;
                            }
                            else if (!strcmp(p->attr[i], "imgFilterBlack"))
                            {
                                blendMode = IMG_FILTER_BLACK;
                            }
                            else if (!strcmp(p->attr[i], "imgSrcOverMode"))
                            {
                                blendMode = IMG_SRC_OVER_MODE;
                            }
                            else if (!strcmp(p->attr[i], "imgCoverMode"))
                            {
                                blendMode = IMG_COVER_MODE;
                            }
                        }
                        else if (!strcmp(p->attr[i], "opacity"))
                        {
                            opacity = atof(p->attr[++i]);
                        }
                        i++;
                    }
                    void *img1;
                    void *img2;
                    {
                        img1 = gui_get_file_address(picture);
                        img2 = img1;
                    }
                    if (style != BUTTON_HIGHLIGHT_ARRAY)
                    {
                        img2 = gui_get_file_address(hl_picture);
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    //font_size = 32;
                    parent = (void *)gui_button_create(parent, x, y, w, h, img1, img2, text, BUTTON_BG_ICON, 0);
                    GUI_TYPE(gui_button_t, parent)->style = style;
                    parent->name = ptxt;
                    gui_button_img_move((void *)parent, picture_x, picture_y);
                    gui_button_text_move((void *)parent, text_x, text_y);
                    gui_img_set_mode(GUI_TYPE(gui_button_t, parent)->img, blendMode);
                    gui_img_set_opacity(GUI_TYPE(gui_button_t, parent)->img, opacity);

                    gui_color_t color_temporary;
                    color_temporary = font_color;
                    GUI_TYPE(gui_button_t, parent)->text->color = color_temporary;
                    if (style)
                    {
                        if (style == BUTTON_HIGHLIGHT_ARRAY)
                        {
                            int file_count = 0;
                            char *folder = hl_picture;
                            {
                                DIR *dir = 0;
                                struct dirent *entry;
                                char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                                if ((dir = opendir(path)) == NULL)
                                {
                                    GUI_ASSERT(0);
                                }
                                gui_free(path);
                                while ((entry = readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        file_count++;
                                    }


                                }
                                closedir(dir);
                            }
                            void **image_array = gui_malloc(file_count * sizeof(void *));
                            {
                                DIR *dir = 0;
                                struct dirent *entry;
                                char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                                if ((dir = opendir(path)) == NULL)
                                {
                                    gui_free(path);
                                    return 0;
                                }

                                int count = 0;
                                while ((entry = readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        char *path2 = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                                        sprintf(path2, "%s/%s", folder, entry->d_name);
                                        image_array[count++] = gui_get_file_address(path2);
                                    }

                                }
                                gui_free(path);
                                closedir(dir);
                                GUI_API(gui_button_t).on_press((void *)parent, (gui_event_cb_t)button_press_array,
                                                               (void *)(uintptr_t)file_count);
                                GUI_TYPE(gui_button_t, parent)->data = image_array;
                                GUI_API(gui_button_t).on_release((void *)parent, (gui_event_cb_t)button_press_array, (void *) - 1);
                            }
                        }
                        else
                        {
                            gui_button_press((void *)parent, (gui_event_cb_t)sport_button_press, parent);
                            gui_button_release((void *)parent, (gui_event_cb_t)sport_button_release, parent);
                        }

                    }

                    {
                        FONT_SRC_TYPE font_type2; GUI_UNUSED(font_type2);
                        if (strstr(font_type, ".bin") != NULL)
                        {
                            font_type2 = GUI_FONT_SRC_BMP;
                            void *addr1 = gui_get_file_address(font_type);
                            //gui_font_mem_init(addr1);
                            GUI_TYPE(gui_button_t, parent)->text->font_height = font_size;
                            GUI_TYPE(gui_button_t, parent)->text->path = 0;
                            gui_text_type_set(GUI_TYPE(gui_button_t, parent)->text, addr1, FONT_SRC_MEMADDR);
                            gui_text_mode_set(GUI_TYPE(gui_button_t, parent)->text, LEFT);
                            // t->font_height = fontSize;
                            //t->path = 0;
                        }
                        else if ((strstr(font_type, ".ttf") != NULL) || (strstr(font_type, ".TTF") != NULL))
                        {
#ifdef __WIN32
                            font_type2 = GUI_FONT_SRC_TTF;
                            GUI_TYPE(gui_button_t, parent)->text->path = gui_get_file_address(font_type);
                            GUI_TYPE(gui_button_t, parent)->text->font_type = GUI_FONT_SRC_TTF;
#elif defined RTL8772F
                            font_type2 = GUI_FONT_SRC_TTF;
                            GUI_TYPE(gui_button_t, parent)->text->path = gui_get_file_address(font_type);
                            GUI_TYPE(gui_button_t, parent)->text->font_type = GUI_FONT_SRC_TTF;
#else
                            font_type =
                                "app/system/resource/font/gbk_32_32_dot.bin;app/system/resource/font/gbk_unicode_table.bin";
                            {
                                font_type2 = GUI_FONT_SRC_BMP;
                                char b[100] = {0};
                                strncpy(b, font_type, strstr(font_type, ".bin;") - font_type + strlen(".bin"));
                                void *addr1 = gui_get_file_address(b);
                                memset(b, 0, sizeof(b));
                                char *a = font_type;
                                strncpy(b, strstr(a, ".bin;") + strlen(".bin;"), strlen(a) - (strstr(a,
                                                                                                     ".bin;") - a + strlen(".bin;")));
                                void *addr2 = gui_get_file_address(b);
                                // gui_set_font_mem_resourse(32, addr1,  addr2);
                                GUI_TYPE(gui_button_t, parent)->text->path = 0;
                                GUI_TYPE(gui_button_t, parent)->text->font_type = GUI_FONT_SRC_BMP;
                            }
#endif
                        }
                    }
                    {
                        bool if_widget = 0;
                        if (p->parent)
                        {
                            if (!strcmp(widget[21].name, p->parent->name))
                            {
                                if_widget = 1;
                            }
                        }
                        if (if_widget)
                        {
                            if (open_switch_name && !strcmp(parent->name, open_switch_name))
                            {
                                gui_free(open_switch_name);
                                open_switch_name = 0;
                                GUI_API(gui_button_t).on_click((gui_button_t *)parent,
                                                               (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)0);
                            }
                            else if (close_switch_name && !strcmp(parent->name, close_switch_name))
                            {
                                gui_free(close_switch_name);
                                close_switch_name = 0;
                                GUI_API(gui_button_t).on_click((gui_button_t *)parent,
                                                               (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)1);
                            }
                            else if (pause_switch_name && !strcmp(parent->name, pause_switch_name))
                            {
                                gui_free(pause_switch_name);
                                pause_switch_name = 0;
                                GUI_API(gui_button_t).on_click((gui_button_t *)parent,
                                                               (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)2);
                            }
                        }
                    }

                    break;
                }
            case RADIO:
                {
                    size_t i = 0;
                    int16_t x = 0; GUI_UNUSED(x);
                    int16_t y = 0; GUI_UNUSED(y);
                    int16_t w = 0; GUI_UNUSED(w);
                    int16_t h = 0; GUI_UNUSED(h);
                    char *font_type = "rtk_font_fs32"; GUI_UNUSED(font_type);
                    char *text = NULL; GUI_UNUSED(text);
                    int text_x = 0; GUI_UNUSED(text_x);
                    int text_y = 0; GUI_UNUSED(text_y);
                    uint32_t font_color = 0Xf0f0; GUI_UNUSED(font_color);
                    uint32_t font_size = 40; GUI_UNUSED(font_size);
                    int picture_x = 0; GUI_UNUSED(picture_x);
                    int picture_y = 0; GUI_UNUSED(picture_y);
                    int transition = 0; GUI_UNUSED(transition);
                    char *picture = NULL; GUI_UNUSED(picture);
                    char *hl_picture = NULL; GUI_UNUSED(hl_picture);
                    int style = 0; GUI_UNUSED(style);

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
                    void *img1; GUI_UNUSED(img1);
                    void *img2; GUI_UNUSED(img2);
                    {
                        img1 = gui_get_file_address(picture);
                    }
                    {
                        img2 = gui_get_file_address(hl_picture);;
                    }
                    //parent = (void *)gui_radio_create(parent, x, y, w, h, img1, img2);
                    //parent->name = get_space_string_head(p->txt);
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
                    char *pictureHl = NULL;
                    char *hl_pictureHl = NULL;
                    int picture_x = 0;
                    int picture_y = 0;
                    int dur = 1000;
                    // default image blend_mode
                    BLEND_MODE_TYPE blendMode = IMG_FILTER_BLACK;
                    uint8_t opacity = 255;
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
                        else if (!strcmp(p->attr[i], "clickedPicture"))
                        {
                            pictureHl = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "clickedHighlightPicture"))
                        {
                            hl_pictureHl = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "pictureX"))
                        {
                            picture_x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "pictureY"))
                        {
                            picture_y = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "blendMode"))
                        {
                            i++;
                            if (!strcmp(p->attr[i], "imgBypassMode"))
                            {
                                blendMode = IMG_BYPASS_MODE;
                            }
                            else if (!strcmp(p->attr[i], "imgFilterBlack"))
                            {
                                blendMode = IMG_FILTER_BLACK;
                            }
                            else if (!strcmp(p->attr[i], "imgSrcOverMode"))
                            {
                                blendMode = IMG_SRC_OVER_MODE;
                            }
                            else if (!strcmp(p->attr[i], "imgCoverMode"))
                            {
                                blendMode = IMG_COVER_MODE;
                            }
                        }
                        else if (!strcmp(p->attr[i], "opacity"))
                        {
                            opacity = atof(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "mode"))
                        {
                            char *s = p->attr[++i];
                            if (!strcmp(p->attr[i], "array"))
                            {
                                style = SWITCH_HIGHLIGHT_ARRAY;
                            }
                        }
                        else if (!strcmp(p->attr[i], "duration"))
                        {
                            dur = atoi(p->attr[++i]);
                        }
                        i++;
                    }
                    void *img1;
                    void *img2;
                    {
                        img1 = gui_get_file_address(picture);
                        img2 = img1;
                    }
                    if (style != SWITCH_HIGHLIGHT_ARRAY)
                    {
                        img2 = gui_get_file_address(hl_picture);
                    }
                    if (!img1)
                    {
                        img1 = img2;
                    }
                    if (!img2)
                    {
                        img2 = img1;
                    }
                    if (!img2 && !img1)
                    {
                        img1 = gui_get_file_address("app/system/resource/switchOff.bin");
                        img2 = gui_get_file_address("app/system/resource/switchOn.bin");
                        int16_t *scale = img1;
                        scale++;
                        w = *scale;
                        scale++;
                        h = *scale;
                    }


                    parent = (void *)gui_switch_create(parent, x, y, w, h, img1, img2);
                    parent->name = get_space_string_head(p->txt);
                    if (hl_pictureHl)
                    {
                        GUI_TYPE(gui_switch_t, parent)->on_hl_pic_addr = gui_get_file_address(hl_pictureHl);
                    }
                    if (pictureHl)
                    {
                        GUI_TYPE(gui_switch_t, parent)->off_hl_pic_addr = gui_get_file_address(pictureHl);
                    }
                    GUI_TYPE(gui_switch_t, parent)->switch_picture->base.x = picture_x;
                    GUI_TYPE(gui_switch_t, parent)->switch_picture->base.y = picture_y;
                    gui_img_set_mode(GUI_TYPE(gui_switch_t, parent)->switch_picture, blendMode);
                    gui_img_set_opacity(GUI_TYPE(gui_switch_t, parent)->switch_picture, opacity);
                    if (style)
                    {
                        if (style == SWITCH_HIGHLIGHT_ARRAY)
                        {
                            GUI_TYPE(gui_switch_t, parent)->style = style;
                            int file_count = 0;
                            char *folder = hl_picture;
                            {
                                DIR *dir = 0;
                                struct dirent *entry;
                                char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                                if ((dir = opendir(path)) == NULL)
                                {
                                    GUI_ASSERT(0);
                                }
                                gui_free(path);
                                while ((entry = readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        file_count++;
                                    }


                                }
                                closedir(dir);
                            }
                            void **image_array = gui_malloc(file_count * sizeof(void *));
                            {
                                DIR *dir = 0;
                                struct dirent *entry;
                                char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                                if ((dir = opendir(path)) == NULL)
                                {
                                    gui_free(path);
                                    return 0;
                                }

                                int count = 0;
                                while ((entry = readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        char *path2 = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                                        sprintf(path2, "%s/%s", folder, entry->d_name);
                                        image_array[count++] = gui_get_file_address(path2);
                                    }

                                }
                                gui_free(path);
                                closedir(dir);
                                GUI_API(gui_switch_t).on_press((void *)parent, (gui_event_cb_t)switch_press_array,
                                                               (void *)(uintptr_t)file_count);
                                GUI_API(gui_switch_t).animate((void *)parent, dur, 0, switch_press_ani_cb_array, 0);
                                GUI_TYPE(gui_switch_t, parent)->animate->animate = 0;
                                GUI_TYPE(gui_switch_t, parent)->data = image_array;
                                GUI_TYPE(gui_switch_t, parent)->on_pic_addr = image_array[file_count - 1];

                            }
                        }


                    }
                    bool if_widget = 0;
                    if (p->parent)
                    {
                        if (!strcmp(widget[21].name, p->parent->name))
                        {
                            if_widget = 1;
                        }
                    }
                    if (if_widget)
                    {
                        if (open_switch_name && !strcmp(parent->name, open_switch_name))
                        {
                            gui_free(open_switch_name);
                            open_switch_name = 0;
                            GUI_API(gui_switch_t).on_turn_on((gui_switch_t *)parent,
                                                             (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)0);
                            GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
                                                              (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)0);
                        }
                        else if (close_switch_name && !strcmp(parent->name, close_switch_name))
                        {
                            gui_free(close_switch_name);
                            close_switch_name = 0;
                            GUI_API(gui_switch_t).on_turn_on((gui_switch_t *)parent,
                                                             (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)1);
                            GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
                                                              (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)1);
                        }
                        else if (pause_switch_name && !strcmp(parent->name, pause_switch_name))
                        {
                            gui_free(pause_switch_name);
                            pause_switch_name = 0;
                            GUI_API(gui_switch_t).on_turn_on((gui_switch_t *)parent,
                                                             (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)2);
                            GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
                                                              (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)3);
                        }
                    }

                }
                break;
            case GALLERY:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;

                    gui_gallery_config_t config = {0};
                    char *picture = "app/system/resource/Progress bar_full.bin";
                    char *folder = NULL;

                    // default image blend_mode
                    uint8_t blendMode = IMG_FILTER_BLACK;
                    uint8_t opacity = 255;

                    memset(&config, 0, sizeof(config));
                    while (true)
                    {
                        // #define IS_STR(str) (!strcmp(p->attr[i], str))
                        if (!(p->attr[i]))
                        {
                            break;
                        }
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
                        else if (!strcmp(p->attr[i], "folder"))
                        {
                            folder = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "mainBg"))
                        {
                            config.data_main_bg = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "centerBg"))
                        {
                            config.data_center_bg = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "centerPercent"))
                        {
                            config.center_percent = atof(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "sideScale"))
                        {
                            config.side_scale = atof(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "sidePosPercent"))
                        {
                            config.side_pos_percent = atof(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "blendMode"))
                        {
                            i++;
                            if (!strcmp(p->attr[i], "imgBypassMode"))
                            {
                                blendMode = IMG_BYPASS_MODE;
                            }
                            else if (!strcmp(p->attr[i], "imgFilterBlack"))
                            {
                                blendMode = IMG_FILTER_BLACK;
                            }
                            else if (!strcmp(p->attr[i], "imgSrcOverMode"))
                            {
                                blendMode = IMG_SRC_OVER_MODE;
                            }
                            else if (!strcmp(p->attr[i], "imgCoverMode"))
                            {
                                blendMode = IMG_COVER_MODE;
                            }
                        }
                        else if (!strcmp(p->attr[i], "opacity"))
                        {
                            opacity = atof(p->attr[++i]);
                        }
                        i++;
                    }

                    char *ptxt = get_space_string_head(p->txt);
                    if (!folder)
                    {
                        gui_log("gallery folder is not found!\n");
                    }
                    else if (folder)
                    {
                        int file_count = 0;
                        DIR *dir = 0;
                        struct dirent *entry;
                        char *dir_path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);

                        // init image array
                        sprintf(dir_path, "%s%s", GUI_ROOT_FOLDER, folder);
                        if ((dir = opendir(dir_path)) == NULL)
                        {
                            gui_free(dir_path);
                            return 0;
                        }
                        gui_free(dir_path);

                        while ((entry = readdir(dir)) != NULL)
                        {
                            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                            {
                                char *file_path = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                                void *addr = NULL;
                                void *data = NULL;

                                file_count++;
                                sprintf(file_path, "%s/%s", folder, entry->d_name);
                                addr = gui_get_file_address(file_path);
                                if (!config.img_array)
                                {
                                    data = gui_malloc(sizeof(void *));
                                }
                                else
                                {
                                    data = gui_realloc(config.img_array, file_count * (sizeof(void *)));
                                }
                                config.img_array = data;
                                config.img_array[file_count - 1] = addr;

                                gui_free(file_path);
                            }
                        }
                        closedir(dir);

                        config.num_pic = file_count;
                    }

                    if (config.data_main_bg)
                    {
                        config.data_main_bg = gui_get_file_address(config.data_main_bg);
                    }
                    if (config.data_center_bg)
                    {
                        config.data_center_bg = gui_get_file_address(config.data_center_bg);
                    }

                    gui_gallery_create(parent, ptxt, &config,
                                       x, y, w, h);
                }
                break;
            case KEYBOARD:
                {
                    size_t i = 0;
                    gui_kb_config_t config = {0};
                    char *picture = "app/system/resource/Progress bar_full.bin";
                    // char *folder = NULL;

                    // default image blend_mode
                    memset(&config, 0, sizeof(config));

                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        if (!strcmp(p->attr[i], "ime"))
                        {
                            i++;
                            if (!strcmp(p->attr[i], "pinyin"))
                            {
                                config.ime = KB_METHOD_PINYIN;
                            }
                        }
                        i++;
                    }

                    {
#define PATH_PRE "app/system/resource/"
                        char *floder_kb = PATH_PRE"keyboard";
                        char *floder_letter = PATH_PRE"keyboard/0_letter/";
                        char *floder_letter_upper = PATH_PRE"keyboard/1_letter_upper/";
                        char *floder_num = PATH_PRE"keyboard/2_number/";
                        char *floder_symbol = PATH_PRE"keyboard/3_symbol/";
                        char *floder_func = PATH_PRE"keyboard/4_func/";
                        char *floder_other = PATH_PRE"keyboard/5_other/";

                        config.layout = KB_LAYOUT_BASIC;
                        config.mode = KB_MODE_BASIC_ENG_LOWWER;

                        uint16_t file_count = 0;
                        DIR *dir = NULL;
                        struct dirent *entry;
                        char *folder_array[] = {floder_letter, floder_letter_upper, floder_num, floder_symbol, floder_func, floder_other};
                        for (int i = 0; i < sizeof(folder_array) / sizeof(folder_array[0]); i++)
                        {
                            char *folder = folder_array[i];
                            char *dir_path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);

                            // init image array
                            sprintf(dir_path, "%s%s", GUI_ROOT_FOLDER, folder);
                            if ((dir = opendir(dir_path)) == NULL)
                            {
                                gui_free(dir_path);
                                continue;
                                //perror("opendir() failed"); return;
                            }
                            // gui_log("folder: %d %s\n", dir, folder);
                            gui_free(dir_path);


                            while ((entry = readdir(dir)) != NULL)
                            {
                                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                {
                                    char *file_path = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                                    void *addr = NULL;
                                    void *data = NULL;

                                    file_count++;
                                    sprintf(file_path, "%s/%s", folder, entry->d_name);
                                    // gui_log("file_count %d, file: %s\n", file_count, entry->d_name);
                                    addr = gui_get_file_address(file_path);
                                    if (!config.img_array)
                                    {
                                        data = gui_malloc(sizeof(void *));
                                    }
                                    else
                                    {
                                        data = gui_realloc(config.img_array, file_count * (sizeof(void *)));
                                    }
                                    config.img_array = data;
                                    config.img_array[file_count - 1] = addr;

                                    gui_free(file_path);
                                }

                            }
                            closedir(dir);
                            // gui_log("file_count %d, folder: %s\n", file_count, folder);
                            if (0 == strcmp(folder, floder_symbol))
                            {
                                config.file_mark[0] = file_count;
                            }
                            else if (0 == strcmp(folder, floder_func))
                            {
                                config.file_mark[1] = file_count;
                            }
                        }
                        config.num_pic = file_count;
                    }

                    gui_keyboard_create(parent, "test", &config,
                                        0, 0, gui_get_screen_width(), gui_get_screen_height());
                }
                break;
            case JAVASCRIPT:
                {
                    if (!strcmp(p->attr[0], "file"))
                    {
                        js = gui_strdup(p->attr[1]);
                    }

                }
                break;
            case MACRO_MOTORIZED_CURTAIN:
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
                        if (!strcmp(p->attr[i], "switchOpen"))
                        {
                            open_switch_name = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "switchClose"))
                        {
                            close_switch_name = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "switchPause"))
                        {
                            pause_switch_name = gui_strdup(p->attr[++i]);
                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    //gui_log("x:%d,y:%d,w:%dh:%d,orientation:%d\n", x, y, w, h, orientation);
                    parent = (void *)gui_win_create(parent, ptxt, x, y, w, h);
                }
                break;
            case MULTI_LEVEL:
                {
                    char *ptxt = get_space_string_head(p->txt);
                    //gui_log("x:%d,y:%d,w:%dh:%d,orientation:%d\n", x, y, w, h, orientation);
                    parent = (void *)gui_multi_level_create(parent, ptxt, (void(*)(gui_obj_t *))multi_level_ui_design);

                }
                break;
            case MACRO_ONCLICK:
                {
                    char *type = 0;
                    char *to = 0;
                    char *id = 0;
                    int x = 0;
                    int y = 0;
                    size_t i = 0;
                    while (true)
                    {
                        //gui_log("p->attr[i]:%x\n",p->attr[i]);
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s,\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "type"))
                        {
                            type = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "to"))
                        {
                            to = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "id1"))
                        {
                            x = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "id"))
                        {
                            id = p->attr[++i];
                        }
                        else if (!strcmp(p->attr[i], "id2"))
                        {
                            y = atoi(p->attr[++i]);
                        }
                        i++;
                    }
                    int to_widget = 0;
                    if (type && to)
                    {
                        if (!strcmp(type, "jump"))
                        {
                            {
                                //to
                                if (!strcmp(to, "multiLevel"))
                                {
                                    //GUI_API(gui_multi_level_t).jump(parent, x, y);
                                    struct on_click_jump_cb_param *param;
                                    param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                                    param->id1 = x;
                                    param->id2 = y;
                                    if (parent->type == BUTTON)
                                    {
                                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_cb, param);
                                    }
                                    else if (parent->type == WINDOW)
                                    {
                                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_cb, param);
                                    }
                                }

                                gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                            }

                        }
                        else if (!strcmp(type, "control"))
                        {
                            if (!strcmp(to, "light"))
                            {
                                light_param_t *light;
                                light = gui_malloc(sizeof(light_param_t));
                                light->id = x;
                                light->state = (bool)y;
                                if (parent->type == BUTTON)
                                {
                                    GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)light_control_cb, light);
                                }
                                else if (parent->type == WINDOW)
                                {
                                    gui_win_click((gui_win_t *)parent, (gui_event_cb_t)light_control_cb, light);
                                }
                                else if (parent->type == CLICKSWITCH)
                                {
                                    GUI_API(gui_switch_t).on_turn_on((gui_switch_t *)parent,
                                                                     (gui_event_cb_t)light_switch_on_cb, light);
                                    GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
                                                                      (gui_event_cb_t)light_switch_off_cb, light);
                                }
                            }

                            gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

                        }
                        else if ((!strcmp(type, "animatePause")) || (!strcmp(type, "animate")))
                        {
                            char **param = gui_malloc(sizeof(char *) * 2);
                            param[0] = gui_strdup(to);
                            param[1] = gui_strdup(id);
                            if (!strcmp(type, "animatePause"))
                            {
                                if (parent->type == BUTTON)
                                {
                                    GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
                                }
                                else if (parent->type == WINDOW)
                                {
                                    gui_win_click((gui_win_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
                                }
                                else if (parent->type == CLICKSWITCH)
                                {
                                    GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)pause_animation_cb,
                                                                     (param));
                                    GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
                                                                      (gui_event_cb_t)pause_animation_cb,
                                                                      (param));
                                }

                            }
                            else if (!strcmp(type, "animate"))
                            {
                                if (parent->type == BUTTON)
                                {
                                    GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)start_animation_cb, param);
                                }
                                else if (parent->type == WINDOW)
                                {
                                    gui_win_click((gui_win_t *)parent, (gui_event_cb_t)start_animation_cb, param);
                                }
                                else if (parent->type == CLICKSWITCH)
                                {
                                    GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)start_animation_cb,
                                                                     (param));
                                    GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
                                                                      (gui_event_cb_t)start_animation_cb,
                                                                      (param));
                                }

                            }
                        }
                    }
                }
                break;
            case MACRO_BACKICON:
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
                    void *img1 = return_image;
                    void *img2 = return_image_hl;
                    if (picture)
                    {
                        img1 = gui_get_file_address(picture);
                        img2 = img1;
                    }
                    if (hl_picture)
                    {
                        img2 = gui_get_file_address(hl_picture);
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    //font_size = 32;
                    parent = (void *)gui_button_create(parent, x, y, w, h, img1, img2, 0, 0, 0);
                    parent->name = ptxt;
                    gui_button_t *button = (void *)parent;
                    button->img->blend_mode = IMG_SRC_OVER_MODE;
                    GUI_API(gui_button_t).on_click(button, (gui_event_cb_t)setting_return_cb, 0);


                    break;
                }
            case SLIDER:
                {
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    char *slider_picture = NULL;
                    char *bg_picture = NULL;
                    int16_t minValue = 0;
                    int16_t maxValue = 0;
                    int16_t currentValue = 0;
                    int16_t slider_size = 0;
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
                        else if (!strcmp(p->attr[i], "bgPicture"))
                        {
                            bg_picture = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "sliderPicture"))
                        {
                            slider_picture = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "minValue"))
                        {
                            minValue = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "maxValue"))
                        {
                            maxValue = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "currentValue"))
                        {
                            currentValue = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "sliderSize"))
                        {
                            slider_size = atoi(p->attr[++i]);
                        }
                        i++;
                    }
                    void *bg_buf;
                    void *slider_buf;
                    if (bg_picture)
                    {
                        bg_buf = gui_get_file_address(bg_picture);
                    }
                    if (slider_picture)
                    {
                        slider_buf = gui_get_file_address(slider_picture);
                    }

                    parent = (void *)gui_slider_create(parent, bg_buf, x, y, w, h, minValue, maxValue, slider_buf,
                                                       currentValue, slider_size);
                }
                break;
            case MACRO_ONCHANGE:
                {
                    char *type = 0;
                    char *to = 0;
                    int id = 0;
                    size_t i = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        if (!strcmp(p->attr[i], "type"))
                        {
                            type = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "to"))
                        {
                            to = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "id"))
                        {
                            id = atoi(p->attr[++i]);
                        }
                        i++;
                    }
                    if (type)
                    {

                        if (!strcmp(type, "control"))
                        {
                            light_param_t *light;
                            light = gui_malloc(sizeof(light_param_t));
                            light->id = (id);
                            if (!strcmp(to, "controlLightBT"))
                            {
                                if (parent->type == SEEKBAR)
                                {
                                    GUI_API(gui_seekbar_t).on_change((gui_seekbar_t *)parent, (gui_event_cb_t)light_brightness_cb,
                                                                     light);
                                }
                            }
                            else if (!strcmp(to, "controlLightCT"))
                            {
                                light->text = gui_malloc(sizeof(gui_text_t));
                                if (parent->type == SLIDER)
                                {
                                    gui_slider_t *slider = (gui_slider_t *)parent;
                                    GUI_API(gui_slider_t).on_change(slider, (gui_event_cb_t)light_colorTemp_cb, light);
                                }

                            }
                        }
                        else if (!strcmp(type, "write"))
                        {
                            if (parent->type == SLIDER)
                            {
                                gui_slider_t *slider = (gui_slider_t *)parent;
                                GUI_API(gui_slider_t).on_change(slider, (gui_event_cb_t)slider_write_text_cb, gui_strdup(to));
                            }
                        }
                    }
                }
                break;
            case MACRO_ONON:
                {
                    char *type = 0;
                    char *to = 0;
                    char *id = 0;
                    size_t i = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        if (!strcmp(p->attr[i], "type"))
                        {
                            type = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "to"))
                        {
                            to = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "id"))
                        {
                            id = (p->attr[++i]);
                        }
                        i++;
                    }
                    if (type)
                    {
                        char **param = gui_malloc(sizeof(char *) * 2);
                        param[0] = gui_strdup(to);
                        param[1] = gui_strdup(id);
                        if (!strcmp(type, "animatePause"))
                        {

                            GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)pause_animation_cb,
                                                             (param));
                        }
                        else if (!strcmp(type, "animate"))
                        {
                            GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)start_animation_cb,
                                                             param);
                        }
                    }
                }
                break;
            case MACRO_ONOFF:
                {
                    char *type = 0;
                    char *to = 0;
                    char *id = 0;
                    size_t i = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        if (!strcmp(p->attr[i], "type"))
                        {
                            type = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "to"))
                        {
                            to = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "id"))
                        {
                            id = (p->attr[++i]);
                        }
                        i++;
                    }
                    if (type)
                    {
                        char **param = gui_malloc(sizeof(char *) * 2);
                        param[0] = gui_strdup(to);
                        param[1] = gui_strdup(id);
                        if (!strcmp(type, "animatePause"))
                        {

                            GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
                                                              (gui_event_cb_t)pause_animation_cb,
                                                              (param));
                        }
                        else if (!strcmp(type, "animate"))
                        {
                            GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
                                                              (gui_event_cb_t)start_animation_cb,
                                                              param);
                        }
                    }
                }
                break;
            case MACRO_ONLOAD:
                {
                    char *type = 0;
                    char *to = 0;
                    char *id = 0;
                    size_t i = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        if (!strcmp(p->attr[i], "type"))
                        {
                            type = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "to"))
                        {
                            to = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "id"))
                        {
                            id = (p->attr[++i]);
                        }
                        i++;
                    }
                    if (type)
                    {

                        if (!strcmp(type, "animate"))
                        {
                            ezxml_t f = 0;
                            if (f1 != 0)
                            {
                                f = f1;
                            }
                            else
                            {
                                f = ezxml_parse_file(((gui_app_t *)gui_current_app())->xml);
                            }
                            foreach_create_animate(f, parent, gui_strdup(id));
                            gui_log(" ");
                            if (f1 == 0)
                            {
                                ezxml_free(f);
                            }
                        }

                    }
                }
                break;
            case TYPE_SCROLL_WHEEL_NEW:
                {
                    /*<roller
                        id="scrollWheel1"
                        x="50"               <!-- X coordinate -->
                        y="100"              <!-- Y coordinate -->
                        w="200"              <!-- Width -->
                        h="300"              <!-- Height -->
                        items="Item1,Item2,Item3,Item4,Item5,Item6,Item7,Item8,Item9,Item10,Item11 "  <!-- Items displayed in each row of the scroll wheel -->
                        itemCount="11"        <!-- Total number of items -->
                        rowCount="3"         <!-- Number of visible rows in the scroll wheel -->
                        rowSpacing="20"      <!-- Spacing between each row -->
                    ></roller>*/

                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    int16_t row_count = 0;
                    int16_t row_spacing = 0;
                    int16_t item_count = 0;
                    const char *items = NULL;
                    int column_offset = 0;
                    const char *font = 0;
                    int16_t font_size = 0;
                    gui_color_t  font_color = {0};
                    gui_color_t  font_color_highlight = {0};
                    gui_color_t  item_color = {0};
                    gui_color_t  item_color_highlight = {0};
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }

                        gui_log("p->attr[i]:%s\n", p->attr[i]);

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
                        else if (!strcmp(p->attr[i], "itemCount"))
                        {
                            item_count = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "rowCount"))
                        {
                            row_count = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "rowSpacing"))
                        {
                            row_spacing = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "items"))
                        {
                            items = p->attr[++i];
                        }
                        else if (!strcmp(p->attr[i], "font"))
                        {
                            font = p->attr[++i];
                        }
                        else if (!strcmp(p->attr[i], "fontSize"))
                        {
                            font_size = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "fontColor"))
                        {
                            //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                            font_color = string_rgb888(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "fontColorHighlight"))
                        {
                            //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                            font_color_highlight = string_rgb888(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "itemColor"))
                        {
                            //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                            item_color = string_rgb888(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "itemColorHighlight"))
                        {
                            //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                            item_color_highlight = string_rgb888(p->attr[++i]);
                        }
                        i++;
                    }

                    // Split items into an array of strings
                    char *items_copy = gui_strdup(items); // Make a copy of items to use with strtok
                    const char **string_array = gui_malloc(item_count * sizeof(char *));
                    char *token = strtok(items_copy, ",");
                    int j = 0;
                    while (token != NULL && j < item_count)
                    {
                        string_array[j++] = token;
                        token = strtok(NULL, ",");
                    }
                    // Create the scroll wheel widget
                    gui_scroll_wheel_new_t *scroll_wheel = gui_scroll_wheel_new_create(
                                                               parent, x, y, w, row_spacing, row_count, string_array, item_count
                                                           );
                    scroll_wheel->font_color = font_color;
                    scroll_wheel->font_color_highlight = font_color_highlight;

                    gui_scroll_wheel_new_render_text(scroll_wheel, gui_get_file_address(font), font_size);
                    if (scroll_wheel == NULL)
                    {
                        gui_log("Failed to create scroll wheel widget.\n");
                        continue;;
                    }
                    parent = GUI_BASE(scroll_wheel);

                }
                break;
            case MACRO_CALENDAR:
                {

                    /*    <calendar
                    x="0"
                    y="360"
                    w="800"
                    h="400"
                    fontColor="#ff000000"
                    fontSize="16"
                    font="app/box/resource/font/arialbd_size16_bits4_font.bin">calendar
                    </calendar>*/

                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    gui_color_t color_p = gui_color_css("DarkSlateGray");
                    gui_color_t font_color_highlight = gui_color_css("#FFFFFFFF");;
                    gui_color_t item_color = gui_color_css("#FFFFFF");;
                    gui_color_t item_color_highlight = gui_color_css("rgba(135, 206, 250, 1.0)");;
                    gui_color_t title_color = gui_color_css("rgb(255, 255, 255)");;
                    gui_color_t title_background_color = gui_color_css("hsl(203, 92%, 75%)");;
                    const char *font = 0;
                    int16_t font_size = 0;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }

                        gui_log("p->attr[i]:%s\n", p->attr[i]);

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
                        else if (!strcmp(p->attr[i], "fontColor"))
                        {
                            //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                            color_p = string_rgb888(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "fontColorHighlight"))
                        {
                            //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                            font_color_highlight = string_rgb888(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "itemColor"))
                        {
                            //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                            item_color = string_rgb888(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "itemColorHighlight"))
                        {
                            //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                            item_color_highlight = string_rgb888(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "titleColor"))
                        {
                            //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                            title_color = string_rgb888(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "titleBackgroundColor"))
                        {
                            //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                            title_background_color = string_rgb888(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "font"))
                        {
                            font = p->attr[++i];
                        }
                        else if (!strcmp(p->attr[i], "fontSize"))
                        {
                            font_size = atoi(p->attr[++i]);
                        }
                        i++;
                    }
                    gui_calender_create(parent, x, y, w, h, gui_get_file_address(font), font_size, color_p,
                                        font_color_highlight, item_color, item_color_highlight, title_color, title_background_color);

                }
                break;



            /*default*/
            default:
                break;
            }
        }
    }
    return parent;


}
gui_obj_t *animate_create_handle(ezxml_t p, gui_obj_t *parent, const char *aniamte_name)
{
    if (!parent)
    {
        return 0;
    }
    char *name = p->name;

    for (size_t i = 0; i < sizeof(widget) / sizeof(widget[0]); i++)
    {
        if (!strcmp(widget[i].name, name))
        {
            switch (widget[i].type)
            {
            case MACRO_ANIMATETRANSFORM:
                {
                    char *ptxt = get_space_string_head(p->txt);
                    if (strcmp(aniamte_name, ptxt))
                    {
                        continue;
                    }
                    gui_log("animate_create_handle:%s: %s", name, p->txt);
                    char *type = 0;
                    char *from = 0;
                    char *to = 0;
                    char *dur = 0;
                    char *repeatCount = 0;
                    char *pause = 0;
                    float scale_x = 1;
                    float scale_y = 1;
                    uint8_t opacity = 255;
                    size_t i = 0;
                    while (true)
                    {
                        //gui_log("p->attr[i]:%x\n",p->attr[i]);
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s,\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "type"))
                        {
                            type = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "from"))
                        {
                            from = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "to"))
                        {
                            to = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "dur"))
                        {
                            dur = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "repeatCount"))
                        {
                            repeatCount = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "pause"))
                        {
                            pause = (p->attr[++i]);
                        }
                        i++;
                    }
                    float from_num_f[4];
                    float to_num_f[4];
                    memset(from_num_f, 0, sizeof(from_num_f));
                    memset(to_num_f, 0, sizeof(to_num_f));
                    void **image_array = 0;
                    int count = 0;
                    if (type && from && to && dur && repeatCount)
                    {
                        if (!strcmp(type, "rotate") || !strcmp(type, "scale") || !strcmp(type, "opacity") ||
                            !strcmp(type, "translate"))
                        {
                            int dur_num = 0;
                            int repeat_num = 0;
                            {
                                //from
                                int from_length = strlen(from);
                                int index[4 + 1];
                                index[0] = 0;
                                int idcount = 1;
                                for (size_t i = 0; i < from_length + 1; i++)
                                {
                                    if (from[i] == ' ' || from[i] == '\0')
                                    {
                                        index[idcount] = i;
                                        int num_length = index[idcount] - index[idcount - 1] + 1;
                                        char num_char[num_length + 1];
                                        num_char[num_length] = '\0';
                                        memcpy(num_char, from + index[idcount - 1], num_length);
                                        from_num_f[idcount - 1] = (float)atof(num_char);
                                        idcount++;
                                    }
                                }
                                gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                            }
                            {
                                //to
                                int from_length = strlen(to);
                                int index[5];
                                index[0] = 0;

                                int idcount = 1;
                                for (size_t i = 0; i < from_length + 1; i++)
                                {
                                    if (to[i] == ' ' || to[i] == '\0')
                                    {
                                        index[idcount] = i;
                                        int num_length = index[idcount] - index[idcount - 1] + 1;
                                        char num_char[num_length + 1];
                                        num_char[num_length] = '\0';
                                        memcpy(num_char, to + index[idcount - 1], num_length);
                                        to_num_f[idcount - 1] = (float)atof(num_char);
                                        idcount++;
                                    }
                                }
                                gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                            }
                        }
                        if (!strcmp(type, "image frame") && from)
                        {
                            int file_count = 0;
                            {
                                DIR *dir = NULL;
                                struct dirent *entry;
                                char *path = gui_malloc(strlen(from) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, from);
                                if ((dir = opendir(path)) == NULL)
                                {
                                    gui_free(path);
                                    return 0;
                                }
                                gui_free(path);
                                while ((entry = readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        file_count++;
                                    }
                                }
                                closedir(dir);
                            }

                            image_array = gui_malloc(file_count * sizeof(void *));
                            {
                                DIR *dir = NULL;
                                struct dirent *entry;
                                char *path = gui_malloc(strlen(from) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, from);
                                if ((dir = opendir(path)) == NULL)
                                {
                                    gui_free(path);
                                    return 0;
                                }


                                while ((entry = readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        char *path2 = gui_malloc(strlen(entry->d_name) + strlen(from) + 2);
                                        sprintf(path2, "%s/%s", from, entry->d_name);
                                        image_array[count++] = gui_get_file_address(path2);
                                    }
                                }
                                gui_free(path);
                                closedir(dir);
                            }
                        }


                        int dur_num = 0;

                        int repeat_num = 0;


                        {
                            //dur
                            int from_length = strlen(dur);
                            int ms = 0;
                            for (size_t i = 0; i < from_length + 1; i++)
                            {
                                if (dur[i] == 'm')
                                {
                                    ms = 1;
                                    break;
                                }

                            }
                            dur_num = atoi(dur);
                            if (!ms)
                            {
                                dur_num = dur_num * 1000;
                            }

                            gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                        }
                        {
                            //repeatCount
                            int repeatCount_num = 0;
                            if (!strcmp(repeatCount, "indefinite"))
                            {
                                repeatCount_num = -1;
                            }
                            else
                            {
                                repeatCount_num = atoi(repeatCount);
                            }
                            repeat_num = repeatCount_num;

                            gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                        }

                        image_animate_params_t *params = gui_malloc(sizeof(image_animate_params_t));
                        params->img = (gui_img_t *)parent;
                        params->animate_type = gui_strdup(type);

                        params->x = (int)to_num_f[1];
                        params->y = (int)to_num_f[2];

                        params->scale_x = to_num_f[0];
                        params->scale_y = to_num_f[1];
                        params->opacity = (int)to_num_f[0];

                        params->scale_x_from = from_num_f[0];
                        params->scale_y_from = from_num_f[1];
                        params->scale_x_center = from_num_f[2];
                        params->scale_y_center = from_num_f[3];
                        params->opacity_from = (int)from_num_f[0];
                        params->rotate_from = (int)from_num_f[0];
                        params->rotate_to = (int)to_num_f[0];
                        params->rotate_x = (int)from_num_f[1];
                        params->rotate_y = (int)from_num_f[2];
                        params->translate_from_x = from_num_f[0];
                        params->translate_from_y = from_num_f[1];
                        params->translate_to_x = to_num_f[0];
                        params->translate_to_y = to_num_f[1];
                        params->image_arr = image_array;
                        params->image_count = count;
                        gui_img_append_animate((gui_img_t *)parent, dur_num, repeat_num, multi_animate_callback, params,
                                               gui_strdup(aniamte_name));


                    }
                    return 0;
                }
                break;
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
        if (parent->type == MULTI_LEVEL && strncmp(i->name, "multiLevel", strlen("multiLevel")) != 0)
        {
            gui_log("%s,%s\n", i->name, i->txt);
            foreach_create(i->child, parent);
        }
        else
        {
            foreach_create(i->child, widget_create_handle(i, parent));
        }
    }
}
static void foreach_create_animate(ezxml_t p, gui_obj_t *parent, const char *animate_name)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
        {
            foreach_create_animate(i->child, animate_create_handle(i, parent, animate_name), animate_name);
        }
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
void foreach_scan_with_content(ezxml_t p, const char *element, ezxml_t *target, const char *content)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
        if (strncmp(i->name, element, strlen(element)) == 0 &&
            strncmp(i->txt, content, strlen(content)) == 0)
        {

            *target = i;
            return;
        }
        foreach_scan_with_content(i->child, element, target, content);
    }
}
static int level, order;
void foreach_create_in_multilevel(ezxml_t p, gui_obj_t *parent)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
        if (strncmp(i->name, "multiLevel", strlen("multiLevel")) != 0)
        {
            gui_log("%s,%s\n", i->name, i->txt);
            foreach_create_in_multilevel(i->child, widget_create_handle(i, parent));
        }


    }
}
static void foreach_create_for_multilevel(ezxml_t p, gui_obj_t *parent)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {

        if (strncmp(i->name, "multiLevel", strlen("multiLevel")) == 0 &&
            strncmp(i->txt, parent->name, strlen(parent->name)) == 0)
        {
            foreach_create_in_multilevel(i->child, parent);
            return;
        }

        foreach_create_for_multilevel(i->child, parent);
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
//#include "gui_server.h"
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

static void load_return_array()
{
    const char *folder = "app/system/resource/return_array";
    {
        DIR *dir = 0;
        struct dirent *entry;

        char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
        sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
        if ((dir = opendir(path)) == NULL)
        {
            gui_free(path);
            //perror("opendir() failed");
            return;
        }
        gui_free(path);
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                file_count++;
            }


        }
        closedir(dir);
    }
    image_array = gui_malloc(file_count * sizeof(void *));
    {
        DIR *dir = 0;
        struct dirent *entry;
        char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
        sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
        if ((dir = opendir(path)) == NULL)
        {
            gui_free(path);
            //perror("opendir() failed");
            return;
        }

        int count = 0;
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                char *path2 = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                sprintf(path2, "%s/%s", folder, entry->d_name);
                image_array[count++] = gui_get_file_address(path2);
            }

        }
        gui_free(path);
        closedir(dir);


    }

}
static void load_return_image()
{
    return_image = gui_get_file_address("app/system/resource/return.bin");
    return_image_hl = gui_get_file_address("app/system/resource/returnhl.bin");
}
static void system_load()
{
    load_return_array();
    load_return_image();


}
void create_tree(gui_app_t *app)
{
    system_load();
    f1 = ezxml_parse_file(app->xml);
    js = NULL;
    foreach_create(f1, &app->screen);
    ezxml_free(f1);
    f1 = 0;
#ifdef ENABLE_RTK_GUI_SCRIPT_AS_A_APP
    extern void js_run_file(const char *file, gui_app_t  *app);
    extern void js_isReady(void);
    if (js)
    {
        char *path = gui_malloc(strlen(js) + strlen(GUI_ROOT_FOLDER) + 1);

        sprintf(path, "%s%s", GUI_ROOT_FOLDER, js); gui_log("js_path:%s", path);
        js_run_file(path, app);

        gui_free(path);
        js_isReady();
    }
#endif
}
static void create_tree_in_multi_level(gui_app_t *app, gui_multi_level_t *parent)
{
    gui_obj_tree_print((gui_obj_t *)parent);
    ezxml_t title = 0;
    ezxml_t f = 0;
    if (f1 != 0)
    {
        f = f1;
    }
    else
    {
        f = ezxml_parse_file(app->xml);
    }
    foreach_create_for_multilevel(f, (gui_obj_t *)parent);
    gui_log(" ");
    if (f1 == 0)
    {
        ezxml_free(f);
    }
    gui_obj_tree_print((gui_obj_t *)parent);




}
const char *get_child_ele_attr(char *xml, char *ele, char *txt, char *chile_ele, char *attr)
{
    ezxml_t f1 = ezxml_parse_file(xml);
    ezxml_t t;
    foreach_scan_with_content(f1, ele, &t, txt);
    ezxml_t title = ezxml_get(t, chile_ele, -1);
    return ezxml_attr(title, attr);
}
ezxml_t get_child_ele(char *xml, char *ele, char *txt, char *chile_ele)
{
    ezxml_t f1 = ezxml_parse_file(xml);
    ezxml_t t;
    foreach_scan_with_content(f1, ele, &t, txt);
    ezxml_t title = ezxml_get(t, chile_ele, -1);
    return title;
}
const char *get_tag_by_widget_type(int type)
{
    if (type == BUTTON)
    {
        type = ICON;
    }

    for (size_t i = 0; i < sizeof(widget) / sizeof(struct widget_create); i++)
    {
        if (widget[i].type == type)
        {
            return widget[i].name;
        }
    }
    return NULL;
}
void get_app(gui_app_t *app, char *pic, char *text)
{
    ezxml_t f1 = ezxml_parse_file(app->xml);


    level_scan(f1, (void *)pic, (void *)text);
    ezxml_free(f1);
}
void level_scan_width_and_hight(ezxml_t p, int *width, int *hight)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
        //gui_log("%s\n",i->child->next->name);
        if (strcmp(i->name, "app") == 0)
        {
            ezxml_t title = ezxml_get(i, "screen", -1);
//gui_log("%s,%s,%s,%s\n",title->name, title->attr[0], title->attr[1],title->attr[2]);

            size_t j = 0;
            while (true)
            {
                if (!(title->attr[j]))
                {
                    break;
                }
                //gui_log("p->attr[i]:%s\n", p->attr[i]);
                if (!strcmp(title->attr[j], "w"))
                {
                    *width = atoi(title->attr[++j]); gui_log("widgt:%d\n", *width);
                }
                else if (!strcmp(title->attr[j], "h"))
                {
                    *hight = atoi(title->attr[++j]); gui_log("hight:%d\n", *hight);
                }

                j++;
            }
            break;
        }
    }
}
void get_app_by_file(char *xml, char *pic, char *text)
{
    ezxml_t f1 = ezxml_parse_file(xml);


    level_scan(f1, (void *)pic, (void *)text);
    ezxml_free(f1);
}
void get_screen_size(char *xml, int *widgt, int *hight)
{
    ezxml_t f1 = ezxml_parse_file(xml);


    level_scan_width_and_hight(f1, widgt, hight);
    ezxml_free(f1);
}
void get_system_screen(int *w, int *h)
{
    char *ezxmlpath = "app/system/launcher.xml";
    char *path = gui_malloc(strlen(ezxmlpath) + strlen(GUI_ROOT_FOLDER) + 1);
    sprintf(path, "%s%s", GUI_ROOT_FOLDER, ezxmlpath);
    ezxml_t f1 =
        ezxml_parse_file(path);
    gui_free(path);
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
void create_tree_nest(char *xml, void *obj)
{
    GUI_WIDGET_TRY_EXCEPT(obj);
    gui_app_t *app = gui_obj_tree_get_app(obj);
    app->xml = xml;
    ezxml_t f1 = ezxml_parse_file(xml);
    foreach_create(f1, obj);
}
static void setting_return_cb(void *obj, gui_event_t e, void *param)
{
    gui_obj_t *this = (gui_obj_t *)obj;
    if (this->parent && this->parent->type == MULTI_LEVEL)
    {
        if (this->parent->parent && this->parent->parent->type == MULTI_LEVEL)
        {
            uint8_t index = GUI_TYPE(gui_multi_level_t, this->parent->parent)->index;
            uint8_t level = GUI_TYPE(gui_multi_level_t, this->parent->parent)->level;
            GUI_API(gui_multi_level_t).jump((void *)this->parent, level, index);
        }
    }
}
static void pause_animation_cb(gui_obj_t *this, void *null, char *to_name[])
{
    if (to_name[0])
    {
        gui_obj_t *to = 0;
        gui_obj_tree_get_widget_by_name((void *)gui_current_app(), to_name[0], &to);

        GUI_WIDGET_TRY_EXCEPT(to)
        {
            /* pause */
            if (to->type == IMAGE_FROM_MEM)
            {
                if (GUI_TYPE(gui_img_t, to)->animate)
                {
                    //GUI_TYPE(gui_img_t, to)->animate->animate = 0;
                    for (size_t i = 0; i < GUI_TYPE(gui_img_t, to)->animate_array_length; i++)
                    {
                        ((gui_animate_t **)(GUI_TYPE(gui_img_t, to)->animate))[i]->animate = 0;
                    }

                }


            }
        }
    }
}
static void start_animation_cb(gui_obj_t *this, void *null, char *to_name[])
{
    if (to_name[0])
    {
        gui_obj_t *to = 0;
        gui_obj_tree_get_widget_by_name((void *)gui_current_app(), to_name[0], &to);
        GUI_WIDGET_TRY_EXCEPT(to)
        {
            /* pause */
            if (to->type == IMAGE_FROM_MEM)
            {
                gui_img_t *img = GUI_TYPE(gui_img_t, to);
                bool create = 1;
                {
                    for (size_t i = 0; i < img->animate_array_length; i++)
                    {
                        if (GUI_STRINGS_EQUAL(((gui_animate_t **)(img->animate))[i]->name, to_name[1]))
                        {
                            create = 0;
                        }
                    }
                }
                if (create)
                {
                    ezxml_t f = 0;
                    if (f1 != 0)
                    {
                        f = f1;
                    }
                    else
                    {
                        f = ezxml_parse_file(((gui_app_t *)gui_current_app())->xml);
                    }
                    foreach_create_animate(f, to, to_name[1]);
                    gui_log(" ");
                    if (f1 == 0)
                    {
                        ezxml_free(f);
                    }
                }
                else
                {
                    for (size_t i = 0; i < GUI_TYPE(gui_img_t, to)->animate_array_length; i++)
                    {
                        ((gui_animate_t **)(img->animate))[i]->animate = 1;
                    }
                }
            }
        }
    }
}
