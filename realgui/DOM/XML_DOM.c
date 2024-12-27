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

#include <string.h>
#include <ctype.h>
#include "ezxml.h"

#include "gui_slider.h"
#include "gui_scroll_wheel_new.h"
#include "gui_calendar.h"
#include "gui_combo.h"
#include "gui_wave.h"
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
    {"arc", MACRO_CANVAS_ARC},
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
    {"onTime", MACRO_ONTIME},
    {"combo", MACRO_COMBO},
    {"onPeripheral", MACRO_ON_PERIPHERAL},
    {"chart", MACRO_CHART},
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
typedef struct arc_animation_param
{
    gui_color_t stroke;
    float sd;
    float ed;
    int image_data_length;
    int image_width;
    int image_height;
    uint8_t *target_buffer;
    int16_t cx;
    int16_t cy;
    int16_t r;
    int16_t stroke_width;
    uint8_t cap;
    uint8_t aniamtion_type;
    uint8_t dir;

} arc_animation_param_t;
typedef struct chart_animation_param
{
    gui_color_t color;
    int image_data_length;
    int image_width;
    int image_height;
    uint8_t *target_buffer;

    int *data_array;
    int max, min;
    uint8_t aniamtion_type;
    uint8_t chart_type;
    int16_t stroke_width;
} chart_animation_param_t;
typedef struct
{
    int num1;
    int num2;
} two_integers;
#define TEXT_WEATHER_CUR_ANIMATION 1
#define ARC_ACTIVITY_EX_ANIMATION 2
#define CHART_HEART_RATE_DATA_ANIMATION 3
#define TEXT_HEART_RATE_CUR_ANIMATION 4
#define TEXT_ACTIVITY_EX_ANIMATION 5
#define TEXT_BATTERY_CAPACITY_ANIMATION 6
static void pause_animation_cb(gui_obj_t *this, void *null, char *to_name[]);
static void start_animation_cb(gui_obj_t *this, void *null, char *to_name[]);
static void foreach_create_animate(ezxml_t p, gui_obj_t *parent, const char *animate_name);

static const uint8_t *gui_get_image_file_address(const char *image_file_path);

static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_watchface_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(img_animate_watchface_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(arc_animate_activity_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_weather_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(chart_animate_heartrate_data_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animation_hr_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_activity_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_battery_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(img_animate_watchface_callback_hour);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(img_animate_watchface_callback_minute);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(img_animate_watchface_callback_second);
static void get_2_int_from_string(const char *input, int *int1, int *int2);
static bool ends_with_xml(const char *str);
static ezxml_t f1;
static void gui_canvas_img_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
static void img_ontime_render(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
static void img_render(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
static void button_render(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
static void img_rotate_cb(image_animate_params_t *animate_params, void *null,
                          gui_animate_t *animate)
{


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
    if (length <= 0)
    {
        fclose(file);
        return 0;
    }
    char *content = (char *)malloc(length + 1);
    //if (content)
    {
        size_t size = fread(content, 1, length, file);
        if (size != length)
        {
            gui_log("%d\n", size);
            gui_free(content);
            fclose(file);
            return 0;
        }


        content[length] = '\0';
    }
    fclose(file);
    return content;
}

static void parse_json(const char *json_str)
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
        if (!ptr)
        {
            return;
        }

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

static void write_file(const char *filename, const char *content)
{
    FILE *file = fopen(filename, "w");
    if (file != NULL)
    {
        fwrite(content, 1, strlen(content), file);
        fclose(file);
    }
}

static void update_light_config(int index, const char *attribute, int value)
{
    char *config_file_path = "./realgui/example/web/peripheral_simulation/json/light_config.json";
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
static gui_img_t *xml_gui_img_create_from_mem(void *parent,  const char *name, void *addr,
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

static char *get_space_string_head(const char *string)
{
    char *s = gui_strdup(string);
    char *rst = 0;
    if (s == 0)
    {
        return rst;
    }
    size_t length = strlen(s);
    for (int i = 0; i <= length - 1; i++)
    {
        if (!((s[i] >= 32) && (s[i] <= 126)))
        {
            s[i] = '\0';
            break;
        };
    }
    if (strlen(s) > 0)
    {
        rst = gui_strdup(s);
    }
    gui_free(s);
    return rst;
}
static char *js;



static gui_color_t string_rgb888(const char *color)
{
    return gui_color_css(color);
}
static void sport_button_press_ani_cb(gui_button_t *button)
{
    GUI_UNUSED(js);
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

static void button_press_ani_cb_array(int p, gui_button_t *button)
{

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
    const char *to_widget_name;
    int id1;
    int id2;
};

static void on_click_jump_cb(void *obj, gui_event_t e, struct on_click_jump_cb_param *param)
{
    GUI_WIDGET_POINTER_BY_TYPE(ml, MULTI_LEVEL, &(gui_current_app()->screen))
    if (param->id1 < 0)
    {
        setting_return_cb(obj, e, (void *)param);
    }
    else
    {
        GUI_API(gui_multi_level_t).jump((void *)ml, param->id1, param->id2);
    }


}
static void on_click_jump_cb_tabview(void *obj, gui_event_t e, struct on_click_jump_cb_param *param)
{
    if (param->to_widget_name)
    {
        GUI_WIDGET_POINTER_BY_NAME(tabview, param->to_widget_name);
        gui_tabview_jump_tab((void *)tabview, param->id1, param->id2);
    }
    else
    {
        GUI_WIDGET_POINTER_BY_TYPE(tabview, TABVIEW, &(gui_current_app()->screen));
        gui_tabview_jump_tab((void *)tabview, param->id1, param->id2);
    }



}
static void on_click_jump_to_app_cb(void *obj, gui_event_t e, const char *new_xml)
{
    if (new_xml)
    {
        extern void *get_app_xml(void);
        gui_app_t *app2 = (gui_app_t *)get_app_xml();
        app2->xml = new_xml;
        gui_switch_app(gui_current_app(), app2);
    }
}
static void on_click_jump_to_capp_cb(void *obj, gui_event_t e, gui_app_t *handle)
{
    if (handle)
    {
        gui_switch_app(gui_current_app(), handle);
    }
}
gui_obj_t *widget_create_handle(ezxml_t p, gui_obj_t *parent)
{
    char *name = p->name;
    //gui_log("widget_create_handle:%s: %s", name, p->txt);
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
#else
                    char *font =
                        "app/system/resource/font/gbk_32_32_dot.bin;app/system/resource/font/gbk_unicode_table.bin";
#endif
                    gui_color_t color = APP_COLOR_WHITE;
                    int fontSize = 32;
                    TEXT_MODE style = (TEXT_MODE)0;
                    uint8_t inputable = false;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
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
                                style = (TEXT_MODE)0;
                            }
                            else if (!strcmp(p->attr[i], "verticalscroll"))
                            {
                                style = SCROLL_Y;
                            }
                            else if (!strcmp(p->attr[i], "left"))
                            {
                                style = LEFT;
                            }
                            else if (!strcmp(p->attr[i], "center"))
                            {
                                style = CENTER;
                            }
                            else if (!strcmp(p->attr[i], "right"))
                            {
                                style = RIGHT;
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
                        if (style == 0 || style == CENTER || style == RIGHT || style == LEFT)
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
                                    gui_text_mode_set(t, style);
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
                    uint32_t duration = 1000;
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
                            gui_fs_dir *dir = NULL;
                            struct gui_fs_dirent *entry;
                            char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                            sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                            if ((dir = gui_fs_opendir(path)) == NULL)
                            {
                                gui_free(path);
                                return 0;
                            }
                            gui_free(path);
                            while ((entry = gui_fs_readdir(dir)) != NULL)
                            {
                                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                {
                                    file_count++;
                                }
                            }
                            gui_fs_closedir(dir);
                        }

                        void **image_array = gui_malloc(file_count * sizeof(void *));
                        {
                            gui_fs_dir *dir = NULL;
                            struct gui_fs_dirent *entry;
                            char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                            sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                            if ((dir = gui_fs_opendir(path)) == NULL)
                            {
                                gui_free(path);
                                return 0;
                            }


                            while ((entry = gui_fs_readdir(dir)) != NULL)
                            {
                                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                {
                                    char *path2 = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                                    sprintf(path2, "%s/%s", folder, entry->d_name);
                                    image_array[count++] = (void *)gui_get_image_file_address(path2);
                                }
                            }
                            gui_free(path);
                            gui_fs_closedir(dir);
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
                        void *imgbuf = (void *)gui_get_image_file_address(file);
                        gui_free(file);
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
                        parent->obj_cb = img_render;
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
                        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
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
                        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
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
            case MACRO_CANVAS_ARC:
                {
                    size_t i = 0;
                    int16_t cx = 100; GUI_UNUSED(cx);
                    int16_t cy = 100; GUI_UNUSED(cy);
                    int16_t r = 100; GUI_UNUSED(r);
                    int16_t stroke_width = 10; GUI_UNUSED(stroke_width);
                    int cap = 0; GUI_UNUSED(cap);
                    gui_color_t stroke = APP_COLOR_RED;
                    float sd = 0; GUI_UNUSED(sd);
                    float ed = 100; GUI_UNUSED(ed);
                    int dir = NVG_CW;
                    while (true)
                    {
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "cx") || !strcmp(p->attr[i], "centralX") || !strcmp(p->attr[i], "centerX"))
                        {
                            cx = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "cy") || !strcmp(p->attr[i], "centerY") ||
                                 !strcmp(p->attr[i], "centralY"))
                        {
                            cy = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "r") || !strcmp(p->attr[i], "radius"))
                        {
                            r = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "startDegree") || !strcmp(p->attr[i], "startAngle"))
                        {
                            sd = atof(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "endDegree") || !strcmp(p->attr[i], "endAngle"))
                        {
                            ed = atof(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "strokeWidth") || !strcmp(p->attr[i], "stroke-width"))
                        {
                            stroke_width = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "stroke-linecap") || !strcmp(p->attr[i], "capMode") ||
                                 !strcmp(p->attr[i], "cap"))
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
                        else if (!strcmp(p->attr[i], "strokeColor") || !strcmp(p->attr[i], "color"))
                        {
                            stroke = string_rgb888(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "direction") || !strcmp(p->attr[i], "clockwise"))
                        {
                            if (!strcmp(p->attr[++i], "CCW") || !strcmp(p->attr[i], "count clockwise"))
                            {
                                dir = NVG_CCW;
                            }
                            else if (!strcmp(p->attr[i], "CW") || !strcmp(p->attr[i], "clockwise"))
                            {
                                dir = NVG_CW;
                            }

                        }
                        i++;
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    //arc animation by user's buffer
                    {
                        int image_h = r * 2 + stroke_width * 2,
                            image_w = image_h,
                            pixel_bytes = 4,
                            buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
                        uint8_t *imgdata = gui_lower_malloc(buffer_size);
                        memset(imgdata, 0, buffer_size);
                        int format = GUI_CANVAS_OUTPUT_RGBA;  int image_width = image_w;
                        int image_height = image_h;  uint8_t *target_buffer = imgdata;
                        {
                            int pixel_length = 4;
                            int data_length = 0;
                            uint8_t *buffer = 0;
                            uint8_t *output_data = 0;
                            switch (format)
                            {
                            case GUI_CANVAS_OUTPUT_PNG:
                            case GUI_CANVAS_OUTPUT_JPG:
                                {
                                    data_length = image_width * image_height * pixel_length;
                                    buffer = gui_lower_malloc(data_length);
                                    memset(buffer, 0, data_length);
                                }
                                break;
                            case GUI_CANVAS_OUTPUT_RGBA:
                                {
                                    output_data = target_buffer;
                                    buffer = output_data + sizeof(gui_rgb_data_head_t);
                                    memset(output_data, 0, sizeof(gui_rgb_data_head_t));
                                    gui_rgb_data_head_t *head = (void *)output_data;
                                    head->type = ARGB8888;
                                    head->w = image_width;
                                    head->h = image_height;
                                }
                                break;
                            case GUI_CANVAS_OUTPUT_RGB565:
                                {
                                    pixel_length = 2;
                                    output_data = target_buffer;
                                    memset(output_data, 0, sizeof(gui_rgb_data_head_t));
                                    buffer = output_data + sizeof(gui_rgb_data_head_t);
                                    gui_rgb_data_head_t *head = (void *)output_data;
                                    head->type = RGB565;
                                    head->w = image_width;
                                    head->h = image_height;
                                }
                                break;
                            default:
                                break;
                            }

                            {
                                NVGcontext *vg = 0;
                                extern NVGcontext *nvgCreateAGGE(uint32_t w,
                                                                 uint32_t h,
                                                                 uint32_t stride,
                                                                 enum     NVGtexture format,
                                                                 uint8_t *data);
                                extern void nvgDeleteAGGE(NVGcontext * ctx);
                                vg = nvgCreateAGGE(image_width, image_height, image_width * (pixel_length),
                                                   (pixel_length) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, buffer);
                                nvgBeginFrame(vg, image_width, image_height, 1);

                                nvgResetTransform(vg);
                                float a0,  a1;
                                a0 = sd;
                                a1 = ed;
                                if (a0 != a1)
                                {
                                    nvgArc(vg, r + stroke_width, r + stroke_width, r, sd, ed, dir);
                                    nvgStrokeWidth(vg, stroke_width);
                                    nvgStrokeColor(vg, nvgRGBA(stroke.color.rgba.r, stroke.color.rgba.g, stroke.color.rgba.b,
                                                               stroke.color.rgba.a));
                                    nvgStroke(vg);
                                }


                                nvgEndFrame(vg);
                                nvgDeleteAGGE(vg);
                            }
                        }
                        gui_img_t *img = gui_img_create_from_mem(parent, ptxt, imgdata, cx - image_w / 2, cy - image_h / 2,
                                                                 0, 0);
                        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
                        parent = (void *)img;
                        parent->obj_cb = gui_canvas_img_cb;
                    }


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
                        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
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
                            //gui_log("color %s,%x\n", p->attr[i], color);
                        }
                        else if (!strcmp(p->attr[i], "highlightColor"))
                        {
                            highlightColor = string_rgb888(p->attr[++i]);
                            //gui_log("color %s,%x\n", p->attr[i], highlightColor);
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
                            parent = (void *)gui_progressbar_img_v_create(parent, (void *)gui_get_image_file_address(picture),
                                                                          x, y);
                        }
                        else
                        {
                            parent = (void *)gui_progressbar_img_h_create(parent, (void *)gui_get_image_file_address(picture),
                                                                          x, y);
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
                        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
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
                            //gui_log("color %s,%x\n", p->attr[i], color);
                        }
                        else if (!strcmp(p->attr[i], "highlightColor"))
                        {
                            highlightColor = string_rgb888(p->attr[++i]);
                            //gui_log("color %s,%x\n", p->attr[i], highlightColor);
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
                            parent = (void *)gui_seekbar_create_img_v(parent, (void *)gui_get_image_file_address(picture), x,
                                                                      y);
                        }
                        else
                        {
                            parent = (void *)gui_seekbar_create_img_h(parent, (void *)gui_get_image_file_address(picture), x,
                                                                      y);
                        }
                        gui_img_set_mode(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), blendMode);
                        gui_img_set_opacity(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), opacity);
                    }
                    else if (folder)
                    {
                        int file_count = 0;

                        {
                            gui_fs_dir *dir = 0;
                            struct gui_fs_dirent *entry;
                            char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                            sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                            if ((dir = gui_fs_opendir(path)) == NULL)
                            {
                                gui_free(path);
                                return 0;
                            }
                            gui_free(path);
                            while ((entry = gui_fs_readdir(dir)) != NULL)
                            {
                                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                {
                                    file_count++;
                                }


                            }
                            gui_fs_closedir(dir);
                        }
                        void **image_array = gui_malloc(file_count * sizeof(void *));
                        {
                            gui_fs_dir *dir = 0;
                            struct gui_fs_dirent *entry;
                            char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                            sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                            if ((dir = gui_fs_opendir(path)) == NULL)
                            {
                                gui_free(path);
                                return 0;
                            }

                            int count = 0;
                            while ((entry = gui_fs_readdir(dir)) != NULL)
                            {
                                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                {
                                    char *path2 = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                                    sprintf(path2, "%s/%s", folder, entry->d_name);
                                    image_array[count++] = (void *)gui_get_image_file_address(path2);
                                }

                            }
                            gui_free(path);
                            gui_fs_closedir(dir);
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
                        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
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
                        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
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
                        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
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
                        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
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
                        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
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
                        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
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
                    if (orientation == CURTAIN_UP)
                    {
                        parent = (void *)gui_win_create(parent, 0,  0, (1 - scope) * gui_get_screen_height(), w, h * scope);
                    }

                    else if (orientation == CURTAIN_LEFT)
                    {
                        parent = (void *)gui_win_create(parent, 0, (1 - scope) * gui_get_screen_width(), 0, w * scope, h);
                    }


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
                        img1 = (void *)gui_get_image_file_address(picture);
                        gui_free(picture);
                        img2 = img1;
                    }
                    if (style != BUTTON_HIGHLIGHT_ARRAY)
                    {
                        img2 = (void *)gui_get_image_file_address(hl_picture);
                        gui_free(hl_picture);
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
                    if (text && GUI_TYPE(gui_button_t, parent)->text)
                    {
                        gui_color_t color_temporary;
                        color_temporary = font_color;
                        GUI_TYPE(gui_button_t, parent)->text->color = color_temporary;
                    }
                    parent->obj_cb = button_render;
                    parent->has_destroy_cb = true;

                    if (style)
                    {
                        if (style == BUTTON_HIGHLIGHT_ARRAY)
                        {
                            int file_count = 0;
                            char *folder = hl_picture;
                            {
                                gui_fs_dir *dir = 0;
                                struct gui_fs_dirent *entry;
                                char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                                if ((dir = gui_fs_opendir(path)) == NULL)
                                {
                                    GUI_ASSERT(0);
                                }
                                gui_free(path);
                                while ((entry = gui_fs_readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        file_count++;
                                    }


                                }
                                gui_fs_closedir(dir);
                            }
                            void **image_array = gui_malloc(file_count * sizeof(void *));
                            {
                                gui_fs_dir *dir = 0;
                                struct gui_fs_dirent *entry;
                                char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                                if ((dir = gui_fs_opendir(path)) == NULL)
                                {
                                    gui_free(path);
                                    return 0;
                                }

                                int count = 0;
                                while ((entry = gui_fs_readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        char *path2 = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                                        sprintf(path2, "%s/%s", folder, entry->d_name);
                                        image_array[count++] = (void *)gui_get_image_file_address(path2);
                                    }

                                }
                                gui_free(path);
                                gui_fs_closedir(dir);
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
                    if (text && GUI_TYPE(gui_button_t, parent)->text)
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
                        img1 = (void *)gui_get_image_file_address(picture);
                    }
                    {
                        img2 = (void *)gui_get_image_file_address(hl_picture);;
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
                        img1 = (void *)gui_get_image_file_address("app/system/resource/switchOff.bin");
                        img2 = (void *)gui_get_image_file_address("app/system/resource/switchOn.bin");
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
                        GUI_TYPE(gui_switch_t, parent)->on_hl_pic_addr = (void *)gui_get_image_file_address(hl_pictureHl);
                    }
                    if (pictureHl)
                    {
                        GUI_TYPE(gui_switch_t, parent)->off_hl_pic_addr = (void *)gui_get_image_file_address(pictureHl);
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
                                gui_fs_dir *dir = 0;
                                struct gui_fs_dirent *entry;
                                char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                                if ((dir = gui_fs_opendir(path)) == NULL)
                                {
                                    GUI_ASSERT(0);
                                }
                                gui_free(path);
                                while ((entry = gui_fs_readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        file_count++;
                                    }


                                }
                                gui_fs_closedir(dir);
                            }
                            void **image_array = gui_malloc(file_count * sizeof(void *));
                            {
                                gui_fs_dir *dir = 0;
                                struct gui_fs_dirent *entry;
                                char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                                if ((dir = gui_fs_opendir(path)) == NULL)
                                {
                                    gui_free(path);
                                    return 0;
                                }

                                int count = 0;
                                while ((entry = gui_fs_readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        char *path2 = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                                        sprintf(path2, "%s/%s", folder, entry->d_name);
                                        image_array[count++] = (void *)gui_get_image_file_address(path2);
                                    }

                                }
                                gui_free(path);
                                gui_fs_closedir(dir);
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
                    char *picture = "app/system/resource/Progress bar_full.bin"; GUI_UNUSED(picture);
                    char *folder = NULL;

                    // default image blend_mode
                    uint8_t blendMode = IMG_FILTER_BLACK; GUI_UNUSED(blendMode);
                    uint8_t opacity = 255; GUI_UNUSED(opacity);

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
                        gui_fs_dir *dir = 0;
                        struct gui_fs_dirent *entry;
                        char *dir_path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);

                        // init image array
                        sprintf(dir_path, "%s%s", GUI_ROOT_FOLDER, folder);
                        if ((dir = gui_fs_opendir(dir_path)) == NULL)
                        {
                            gui_free(dir_path);
                            return 0;
                        }
                        gui_free(dir_path);

                        while ((entry = gui_fs_readdir(dir)) != NULL)
                        {
                            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                            {
                                char *file_path = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                                void *addr = NULL;
                                void *data = NULL;

                                file_count++;
                                sprintf(file_path, "%s/%s", folder, entry->d_name);
                                addr = (void *)gui_get_image_file_address(file_path);
                                if (!config.img_array)
                                {
                                    data = gui_malloc(sizeof(uint8_t *));
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
                        gui_fs_closedir(dir);

                        config.num_pic = file_count;
                    }

                    if (config.data_main_bg)
                    {
                        config.data_main_bg = (void *)gui_get_image_file_address(config.data_main_bg);
                    }
                    if (config.data_center_bg)
                    {
                        config.data_center_bg = (void *)gui_get_image_file_address(config.data_center_bg);
                    }

                    gui_gallery_create(parent, ptxt, &config,
                                       x, y, w, h);
                }
                break;
            case KEYBOARD:
                {
                    size_t i = 0;
                    gui_kb_config_t config = {0};
                    char *picture = "app/system/resource/Progress bar_full.bin"; GUI_UNUSED(picture);
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
                        char *floder_kb = PATH_PRE"keyboard"; GUI_UNUSED(floder_kb);
                        char *floder_letter = PATH_PRE"keyboard/0_letter/";
                        char *floder_letter_upper = PATH_PRE"keyboard/1_letter_upper/";
                        char *floder_num = PATH_PRE"keyboard/2_number/";
                        char *floder_symbol = PATH_PRE"keyboard/3_symbol/";
                        char *floder_func = PATH_PRE"keyboard/4_func/";
                        char *floder_other = PATH_PRE"keyboard/5_other/";

                        config.layout = KB_LAYOUT_BASIC;
                        config.mode = KB_MODE_BASIC_ENG_LOWWER;

                        uint16_t file_count = 0;
                        gui_fs_dir *dir = NULL;
                        struct gui_fs_dirent *entry;
                        char *folder_array[] = {floder_letter, floder_letter_upper, floder_num, floder_symbol, floder_func, floder_other};
                        for (int i = 0; i < sizeof(folder_array) / sizeof(folder_array[0]); i++)
                        {
                            char *folder = folder_array[i];
                            char *dir_path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);

                            // init image array
                            sprintf(dir_path, "%s%s", GUI_ROOT_FOLDER, folder);
                            if ((dir = gui_fs_opendir(dir_path)) == NULL)
                            {
                                gui_free(dir_path);
                                continue;
                                //perror("gui_fs_opendir() failed"); return;
                            }
                            // gui_log("folder: %d %s\n", dir, folder);
                            gui_free(dir_path);


                            while ((entry = gui_fs_readdir(dir)) != NULL)
                            {
                                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                {
                                    char *file_path = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                                    void *addr = NULL;
                                    void *data = NULL;

                                    file_count++;
                                    sprintf(file_path, "%s/%s", folder, entry->d_name);
                                    // gui_log("file_count %d, file: %s\n", file_count, entry->d_name);
                                    addr = (void *)gui_get_image_file_address(file_path);
                                    if (!config.img_array)
                                    {
                                        data = gui_malloc(sizeof(uint8_t *));
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
                            gui_fs_closedir(dir);
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
                        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
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
                    static unsigned char ml_count;
                    //gui_log("x:%d,y:%d,w:%dh:%d,orientation:%d\n", x, y, w, h, orientation);
                    if (ptxt && ptxt[0] == 0)
                    {
                        gui_free(ptxt);
                        ptxt = 0;
                    }
                    if (!ptxt)
                    {

                        char buffer[20];
                        sprintf(buffer, "__ml%d_", ml_count++);
                        ptxt = gui_strdup(buffer);
                    }

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
                    int to_widget = 0; GUI_UNUSED(to_widget);
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
                                    if (id)
                                    {
                                        get_2_int_from_string(id, &x, &y);
                                    }


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
                                else if (!strcmp(to, "app"))
                                {
                                    if (ends_with_xml(id))
                                    {
                                        if (parent->type == BUTTON)
                                        {
                                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_to_app_cb,
                                                                           (void *)gui_get_path_by_relative(id));
                                        }
                                        else if (parent->type == WINDOW)
                                        {
                                            gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_to_app_cb, gui_strdup(id));
                                        }
                                    }
                                    else
                                    {
                                        gui_log("[SaaA] error app jump format\n");
                                    }



                                }
                                else if (!strcmp(to, "C-APP"))
                                {
                                    if (parent->type == BUTTON)
                                    {
                                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb,
                                                                       gui_app_get_by_name(id));
                                    }
                                    else if (parent->type == WINDOW)
                                    {
                                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb, 0);
                                    }
                                }
                                else if (!strcmp(to, "tabview") || !strcmp(to, "tab"))
                                {
                                    //GUI_API(gui_multi_level_t).jump(parent, x, y);
                                    struct on_click_jump_cb_param *param;
                                    param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                                    char *tabview_name = 0;
                                    if (id)
                                    {
                                        get_2_int_from_string(id, &x, &y);

                                        {
                                            // Find the first comma in the string
                                            const char *first_comma = strchr(id, ',');

                                            if (first_comma != NULL)
                                            {
                                                // Find the second comma starting from the character after the first comma
                                                const char *second_comma = strchr(first_comma + 1, ',');

                                                // If the second comma was found and it's not at the end of the string
                                                if (second_comma != NULL && *(second_comma + 1) != '\0')
                                                {
                                                    tabview_name = gui_strdup(second_comma + 1);
                                                }
                                                else
                                                {
                                                    // Handle the case where the second comma is not found or is at the end
                                                    gui_log("The second comma does not exist or is at the end.\n");
                                                }
                                            }
                                            else
                                            {
                                                // Handle the case where the first comma is not found
                                                gui_log("The first comma is not found.\n");
                                            }
                                        }
                                    }

                                    param->id1 = x;
                                    param->id2 = y;
                                    param->to_widget_name = tabview_name;
                                    if (parent->type == BUTTON)
                                    {
                                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_cb_tabview,
                                                                       param);
                                    }
                                    else if (parent->type == WINDOW)
                                    {
                                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_cb_tabview, param);
                                    }
                                }

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

                            //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

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
                        img1 = (void *)gui_get_image_file_address(picture);
                        img2 = img1;
                    }
                    if (hl_picture)
                    {
                        img2 = (void *)gui_get_image_file_address(hl_picture);
                    }
                    char *ptxt = get_space_string_head(p->txt);
                    //font_size = 32;
                    parent = (void *)gui_button_create(parent, x, y, w, h, img1, img2, 0, (T_BUTTON_BG_TYPE)0, 0);
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
                        bg_buf = (void *)gui_get_image_file_address(bg_picture);
                    }
                    if (slider_picture)
                    {
                        slider_buf = (void *)gui_get_image_file_address(slider_picture);
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
                    char *to = 0; GUI_UNUSED(to);
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
                            //gui_log(" ");
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
                    int16_t h = 0; GUI_UNUSED(h);
                    int16_t row_count = 0;
                    int16_t row_spacing = 0;
                    int16_t item_count = 0;
                    const char *items = NULL;
                    int column_offset = 0; GUI_UNUSED(column_offset);
                    const char *font = 0;
                    int16_t font_size = 0;
                    gui_color_t  font_color = {0};
                    gui_color_t  font_color_highlight = {0};
                    gui_color_t  item_color = {0}; GUI_UNUSED(item_color);
                    gui_color_t  item_color_highlight = {0}; GUI_UNUSED(item_color_highlight);
                    bool loop = 1;
                    TEXT_MODE style = LEFT;
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
                        else if (!strcmp(p->attr[i], "loop"))
                        {
                            if (!strcmp(p->attr[++i], "loop"))
                            {
                                loop = 1;
                            }
                            else if (!strcmp(p->attr[i], "true"))
                            {
                                loop = 1;
                            }
                            else if (!strcmp(p->attr[i], "false"))
                            {
                                loop = 0;
                            }
                            else if (!strcmp(p->attr[i], "disable"))
                            {
                                loop = 0;
                            }
                            else if (!strcmp(p->attr[i], "not loop"))
                            {
                                loop = 0;
                            }
                        }
                        else if (!strcmp(p->attr[i], "fontMode"))
                        {
                            char *s = p->attr[++i];
                            if (!strcmp(p->attr[i], "truncate"))
                            {
                                style = (TEXT_MODE)0;
                            }
                            else if (!strcmp(p->attr[i], "verticalscroll"))
                            {
                                style = SCROLL_Y;
                            }
                            else if (!strcmp(p->attr[i], "left"))
                            {
                                style = LEFT;
                            }
                            else if (!strcmp(p->attr[i], "center"))
                            {
                                style = CENTER;
                            }
                            else if (!strcmp(p->attr[i], "right"))
                            {
                                style = RIGHT;
                            }
                        }
                        i++;
                    }
                    {
                        char *items_copy = gui_strdup(items); // Make a copy of items to use with strtok
                        char *token = strtok(items_copy, ",");
                        int j = 0;
                        while (token != NULL)
                        {
                            j++;
                            token = strtok(NULL, ",");
                        }
                        item_count = j;
                        gui_free(items_copy);
                    }
                    if (item_count == 0)
                    {
                        item_count = 1;
                        items = "NULL";
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
                    //gui_free(items_copy);
                    if (item_count - 2 < row_count)
                    {
                        int item_count_temp = item_count;
                        if (item_count > row_count)
                        {
                            item_count_temp = item_count * 2;
                        }
                        else
                        {
                            item_count_temp = (row_count / item_count + 2) * item_count;
                        }
                        const char **string_array_temp = gui_malloc(item_count_temp * sizeof(char *));
                        for (size_t i = 0; i < item_count_temp; i++)
                        {
                            string_array_temp[i] = string_array[i % item_count];
                        }
                        gui_free(string_array);
                        string_array = string_array_temp;
                        item_count = item_count_temp;
                    }

                    // Create the scroll wheel widget
                    gui_scroll_wheel_new_t *scroll_wheel = gui_scroll_wheel_new_create(
                                                               parent, x, y, w, row_spacing, row_count, string_array, item_count
                                                           );
                    if (scroll_wheel == NULL)
                    {
                        gui_log("Failed to create scroll wheel widget.\n");
                        continue;;
                    }
                    scroll_wheel->font_color = font_color;
                    scroll_wheel->font_color_highlight = font_color_highlight;
                    extern void gui_scroll_wheel_new_render_text_alien(gui_scroll_wheel_new_t *widget,
                                                                       const void *font_file_pointer,
                                                                       int font_size, TEXT_MODE mode);
                    gui_scroll_wheel_new_render_text_alien(scroll_wheel, gui_get_file_address(font), font_size, style);
                    scroll_wheel->loop = loop;

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
            case MACRO_ONTIME:
                {
                    char *type = 0;
                    char *to = 0; GUI_UNUSED(to);
                    char *id = 0; GUI_UNUSED(id);
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
                    switch (parent->type)
                    {
                    case IMAGE_FROM_MEM:
                        {
                            if (type)
                            {
                                if (id)
                                {
                                    char *token;
                                    int numbers[2];
                                    int index = 0;
                                    token = strtok(id, ",");
                                    while (token != NULL && index < 2)
                                    {
                                        numbers[index] = atoi(token);
                                        index++;
                                        token = strtok(NULL, ",");
                                    }

                                    two_integers *data = (two_integers *)gui_malloc(sizeof(two_integers));
                                    if (data)
                                    {


                                        data->num1 = numbers[0];
                                        data->num2 = numbers[1];

                                        if (!strcmp(type, "hour"))
                                        {

                                            gui_img_append_animate((void *)parent, 1000, -1, img_animate_watchface_callback_hour,
                                                                   (void *)data, "hour");
                                        }
                                        else if (!strcmp(type, "minute"))
                                        {
                                            gui_img_append_animate((void *)parent, 1000, -1, img_animate_watchface_callback_minute,
                                                                   (void *)data, "minute");
                                        }
                                        else if (!strcmp(type, "second"))
                                        {
                                            gui_img_append_animate((void *)parent, 1000, -1, img_animate_watchface_callback_second,
                                                                   (void *)data, "second");
                                        }
                                        parent->obj_cb = img_ontime_render;
                                    }
                                }
                                else
                                {
                                    if (!strcmp(type, "hour"))
                                    {
#define HOUR_ANIMATION 1
#define MINUTE_ANIMATION 2
#define SECOND_ANIMATION 3
                                        gui_img_append_animate((void *)parent, 1000, -1, img_animate_watchface_callback,
                                                               (void *)HOUR_ANIMATION, "hour");
                                    }
                                    else if (!strcmp(type, "minute"))
                                    {
                                        gui_img_append_animate((void *)parent, 1000, -1, img_animate_watchface_callback,
                                                               (void *)MINUTE_ANIMATION, "minute");
                                    }
                                    else if (!strcmp(type, "second"))
                                    {
                                        gui_img_append_animate((void *)parent, 1000, -1, img_animate_watchface_callback,
                                                               (void *)SECOND_ANIMATION, "second");
                                    }
                                }
                            }

                        }
                        break;
                    case TEXTBOX:
                        {
                            if (type)
                            {
                                gui_text_set_animate(parent, 1000, -1, text_animate_watchface_callback, gui_strdup(type));
                            }


                        }
                        break;
                    default:
                        break;
                    }



                }
                break;
            case MACRO_COMBO:
                {
                    /*<combo
                        x="450"
                        y="400"
                        w="200"
                        items="Item1,Item2,Item3,Item4,Item5"
                        rowSpacing="32"
                        fontSize="16"
                        font="app/box/resource/font/arialbd_size16_bits4_font.bin"
                        fontColor="#000000"
                        picture="app\\box\\resource\\combo\\rect20.bin"
                        highlightPicture="app\\box\\resource\\combo\\rect20hl.bin"
                        blendMode="imgBypassMode">combo1
                    </combo>*/

                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t row_spacing = 0;
                    int16_t item_count = 0;
                    const char *items = NULL;
                    const char *font = 0;
                    int16_t font_size = 0;
                    gui_color_t  font_color = {0};
                    char *picture = NULL;
                    char *hl_picture = NULL;
                    char *item_selection_picture = NULL;
                    char *selector_picture = NULL;
                    char *selector_picture_collapsed = NULL;
                    char *selector_picture_hl = NULL;
                    char *selector_picture_collapsed_hl = NULL;
                    const char *font_selection = 0;
                    int16_t font_size_selection = 0;
                    gui_color_t  font_color_selection = {0};
                    gui_color_t  font_color_highlight = {0};
                    gui_color_t  background_color = {0};
                    // default image blend_mode
                    BLEND_MODE_TYPE blendMode = IMG_FILTER_BLACK;
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
                        else if (!strcmp(p->attr[i], "fontSelection"))
                        {
                            font_selection = p->attr[++i];
                        }
                        else if (!strcmp(p->attr[i], "fontSizeSelection"))
                        {
                            font_size_selection = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "fontColorSelection"))
                        {
                            //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                            font_color_selection = string_rgb888(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "fontColorHighlight"))
                        {
                            //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                            font_color_highlight = string_rgb888(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "backgroundColor"))
                        {
                            //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                            background_color = string_rgb888(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "itemPicture"))
                        {
                            picture = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "itemPictureHighlight"))
                        {
                            hl_picture = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "itemPictureSelection"))
                        {
                            item_selection_picture = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "selectionPicture"))
                        {
                            selector_picture = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "selectionPictureCollapsed"))
                        {
                            selector_picture_collapsed = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "selectionPictureHighlight"))
                        {
                            selector_picture_hl = gui_strdup(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "selectionPictureCollapsedHighlight"))
                        {
                            selector_picture_collapsed_hl = gui_strdup(p->attr[++i]);
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
                        i++;
                    }
                    {
                        char *items_copy = gui_strdup(items); // Make a copy of items to use with strtok
                        char *token = strtok(items_copy, ",");
                        int j = 0;
                        while (token != NULL)
                        {
                            j++;
                            token = strtok(NULL, ",");
                        }
                        item_count = j;
                        gui_free(items_copy);
                    }
                    if (item_count == 0)
                    {
                        item_count = 1;
                        items = "NULL";
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
                    gui_combo_t *combo = gui_combo_create(parent, x, y, w, item_count,
                                                          row_spacing,
                                                          string_array, gui_get_image_file_address(picture),
                                                          gui_get_image_file_address(hl_picture),
                                                          gui_get_image_file_address(item_selection_picture),
                                                          gui_get_image_file_address(selector_picture),
                                                          gui_get_image_file_address(selector_picture_hl),
                                                          gui_get_image_file_address(selector_picture_collapsed),
                                                          gui_get_image_file_address(selector_picture_collapsed_hl),
                                                          background_color,
                                                          font_size,
                                                          font_color,
                                                          gui_get_file_address(font),
                                                          font_size_selection,
                                                          font_color_selection, gui_get_file_address(font_selection), font_color_highlight, blendMode);


                    // parent = GUI_BASE(combo);

                }
                break;
            case MACRO_ON_PERIPHERAL:
                {
                    char *type = 0;
                    char *to = 0; GUI_UNUSED(to);
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
                    switch (parent->type)
                    {
                    case IMAGE_FROM_MEM:
                        {
                            if (type)
                            {
                                if (!strcmp(type, "activity"))
                                {
                                    if (!strcmp(id, "exercise"))
                                    {
                                        arc_animation_param_t *param = 0;
                                        ezxml_t pt = p->parent;
                                        if (!strcmp(p->parent->name, "arc"))
                                        {
                                            ezxml_t p = pt;
                                            size_t i = 0;
                                            int16_t cx = 100; GUI_UNUSED(cx);
                                            int16_t cy = 100; GUI_UNUSED(cy);
                                            int16_t r = 100; GUI_UNUSED(r);
                                            int16_t stroke_width = 10; GUI_UNUSED(stroke_width);
                                            int cap = 0; GUI_UNUSED(cap);
                                            gui_color_t stroke = APP_COLOR_RED;
                                            float sd = 0; GUI_UNUSED(sd);
                                            float ed = 100; GUI_UNUSED(ed);
                                            int dir;
                                            while (true)
                                            {
                                                if (!(p->attr[i]))
                                                {
                                                    break;
                                                }
                                                ////gui_log("p->attr[i]:%s\n", p->attr[i]);
                                                if (!strcmp(p->attr[i], "cx") || !strcmp(p->attr[i], "centralX") || !strcmp(p->attr[i], "centerX"))
                                                {
                                                    cx = atoi(p->attr[++i]);
                                                }
                                                else if (!strcmp(p->attr[i], "cy") || !strcmp(p->attr[i], "centerY") ||
                                                         !strcmp(p->attr[i], "centralY"))
                                                {
                                                    cy = atoi(p->attr[++i]);
                                                }
                                                else if (!strcmp(p->attr[i], "r") || !strcmp(p->attr[i], "radius"))
                                                {
                                                    r = atoi(p->attr[++i]);
                                                }
                                                else if (!strcmp(p->attr[i], "startDegree") || !strcmp(p->attr[i], "startAngle"))
                                                {
                                                    sd = atof(p->attr[++i]);
                                                }
                                                else if (!strcmp(p->attr[i], "endDegree") || !strcmp(p->attr[i], "endAngle"))
                                                {
                                                    ed = atof(p->attr[++i]);
                                                }
                                                else if (!strcmp(p->attr[i], "strokeWidth") || !strcmp(p->attr[i], "stroke-width"))
                                                {
                                                    stroke_width = atoi(p->attr[++i]);
                                                }
                                                else if (!strcmp(p->attr[i], "stroke-linecap") || !strcmp(p->attr[i], "capMode") ||
                                                         !strcmp(p->attr[i], "cap"))
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
                                                else if (!strcmp(p->attr[i], "strokeColor") || !strcmp(p->attr[i], "color"))
                                                {
                                                    stroke = string_rgb888(p->attr[++i]);
                                                }
                                                else if (!strcmp(p->attr[i], "direction") || !strcmp(p->attr[i], "clockwise"))
                                                {
                                                    if (!strcmp(p->attr[++i], "CCW") || !strcmp(p->attr[i], "count clockwise"))
                                                    {
                                                        dir = NVG_CCW;
                                                    }
                                                    else if (!strcmp(p->attr[i], "CW") || !strcmp(p->attr[i], "clockwise"))
                                                    {
                                                        dir = NVG_CW;
                                                    }

                                                }
                                                i++;
                                            }
                                            int image_h = r * 2 + stroke_width * 2,
                                                image_w = image_h,
                                                pixel_bytes = 4,
                                                buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
                                            param = gui_malloc(sizeof(arc_animation_param_t));
                                            memset(param, 0, sizeof(arc_animation_param_t));
                                            param->aniamtion_type = ARC_ACTIVITY_EX_ANIMATION;
                                            param->cap = cap;
                                            param->cx = cx;
                                            param->cy = cy;
                                            param->dir = dir;
                                            param->ed = ed;
                                            param->image_data_length = buffer_size;
                                            param->r = r;
                                            param->sd = sd;
                                            param->stroke = stroke;
                                            param->stroke_width = stroke_width;
                                            param->image_width = image_w;
                                            param->image_height = image_h;
                                            param->target_buffer  = (void *)gui_img_get_image_data((void *)parent);

                                        }
                                        gui_img_append_animate((void *)parent, 1000, -1, arc_animate_activity_callback, (void *)param, 0);
                                        parent->obj_cb = img_ontime_render;
                                    }
                                }
                                else if (!strcmp(type, "heart_rate"))
                                {
                                    if (!strcmp(id, "array"))
                                    {
                                        chart_animation_param_t *param = 0;
                                        ezxml_t pt = p->parent;
                                        if (!strcmp(p->parent->name, "chart"))
                                        {
                                            ezxml_t p = pt;
                                            size_t i = 0;
                                            int16_t x = 0; GUI_UNUSED(x);
                                            int16_t y = 0; GUI_UNUSED(y);
                                            int16_t w = 0;
                                            int16_t h = 0;
                                            int16_t item_count = 0; GUI_UNUSED(item_count);
                                            const char *items = NULL; GUI_UNUSED(items);
                                            gui_color_t  color = {0};
                                            int16_t max = 0;
                                            int16_t min = 0;
                                            const char *style = "waveform";
                                            int16_t stroke_width = 4;
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
                                                else if (!strcmp(p->attr[i], "items"))
                                                {
                                                    items = p->attr[++i];
                                                }
                                                else if (!strcmp(p->attr[i], "color"))
                                                {
                                                    color = string_rgb888(p->attr[++i]);
                                                }
                                                else if (!strcmp(p->attr[i], "max"))
                                                {
                                                    max = atoi(p->attr[++i]);
                                                }
                                                else if (!strcmp(p->attr[i], "min"))
                                                {
                                                    min = atoi(p->attr[++i]);
                                                }
                                                else if (!strcmp(p->attr[i], "style"))
                                                {
                                                    style = (p->attr[++i]);
                                                }
                                                else if (!strcmp(p->attr[i], "strokeWidth"))
                                                {
                                                    stroke_width = atoi(p->attr[++i]);
                                                }
                                                i++;
                                            }
                                            int image_h = h,
                                                image_w = w,
                                                pixel_bytes = 4,
                                                buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
                                            param = gui_malloc(sizeof(chart_animation_param_t));
                                            memset(param, 0, sizeof(chart_animation_param_t));
                                            param->aniamtion_type = CHART_HEART_RATE_DATA_ANIMATION;
                                            param->image_data_length = buffer_size;
                                            param->image_width = image_w;
                                            param->image_height = image_h;
                                            param->target_buffer  = (void *)gui_img_get_image_data((void *)parent);
                                            param->color = color;
                                            param->max = max;
                                            param->min = min;
                                            param->stroke_width = stroke_width;
                                            if (!strcmp(style, "waveform"))
                                            {
                                                param->chart_type = 1;
                                            }
                                            else if (!strcmp(style, "bar"))
                                            {
                                                param->chart_type = 2;
                                            }
                                            else if (!strcmp(style, "line"))
                                            {
                                                param->chart_type = 3;
                                            }
                                            else
                                            {
                                                param->chart_type = 1;
                                            }

                                        }
                                        gui_img_append_animate((void *)parent, 1000, -1, chart_animate_heartrate_data_callback,
                                                               (void *)param, 0);
                                        parent->obj_cb = img_ontime_render;
                                    }
                                }
                            }
                        }
                        break;
                    case TEXTBOX:
                        {
                            if (type)
                            {
                                if (!strcmp(type, "Weather"))
                                {
                                    if (!strcmp(id, "current"))
                                    {
                                        gui_text_set_animate(parent, 1000, -1, text_animate_weather_callback,
                                                             (void *)TEXT_WEATHER_CUR_ANIMATION);
                                    }
                                }
                                else if (!strcmp(type, "heart_rate"))
                                {
                                    if (!strcmp(id, "current"))
                                    {
                                        gui_text_set_animate(parent, 1000, -1, text_animation_hr_callback,
                                                             (void *)TEXT_HEART_RATE_CUR_ANIMATION);
                                    }
                                }
                                else if (!strcmp(type, "activity"))
                                {
                                    if (!strcmp(id, "exercise"))
                                    {
                                        gui_text_set_animate(parent, 1000, -1, text_animate_activity_callback,
                                                             (void *)TEXT_ACTIVITY_EX_ANIMATION);
                                    }
                                }
                                else if (!strcmp(type, "battery"))
                                {
                                    if (!strcmp(id, "capacity"))
                                    {
                                        gui_text_set_animate(parent, 1000, -1, text_animate_battery_callback,
                                                             (void *)TEXT_BATTERY_CAPACITY_ANIMATION);
                                    }
                                }
                            }
                        }
                        break;
                    default:
                        break;
                    }



                }
                break;
            case MACRO_CHART:
                {
                    /*  <chart
                            w="55"
                            h="55"
                            x="342"
                            y="342"
                            color="yellow"
                            items="100,200,0,44,50, 1000, -200,20,-30,88"
                            max="400"
                            min="-300"
                            >chart1
                        </chart>  */
                    size_t i = 0;
                    int16_t x = 0;
                    int16_t y = 0;
                    int16_t w = 0;
                    int16_t h = 0;
                    int16_t item_count = 0;
                    const char *items = NULL;
                    gui_color_t  color = {0};
                    int16_t max = 0;
                    int16_t min = 0;
                    const char *style = "waveform";
                    int16_t stroke_width = 4;
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
                        else if (!strcmp(p->attr[i], "items"))
                        {
                            items = p->attr[++i];
                        }
                        else if (!strcmp(p->attr[i], "color"))
                        {
                            color = string_rgb888(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "max"))
                        {
                            max = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "min"))
                        {
                            min = atoi(p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "style"))
                        {
                            style = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "strokeWidth"))
                        {
                            stroke_width = atoi(p->attr[++i]);
                        }
                        i++;
                    }
                    {
                        char *items_copy = gui_strdup(items); // Make a copy of items to use with strtok
                        char *token = strtok(items_copy, ",");
                        int j = 0;
                        while (token != NULL)
                        {
                            j++;
                            token = strtok(NULL, ",");
                        }
                        item_count = j;
                        gui_free(items_copy);
                    }
                    if (item_count == 0)
                    {
                        item_count = 1;
                        items = "0";
                    }
                    float numbers[item_count];
                    {
                        const char delim[2] = ",";
                        char *token;
                        int i = 0;
                        char *str_copy = gui_strdup(items);
                        if (str_copy == NULL)
                        {
                            gui_log("strdup failed");
                            return 0;
                        }
                        token = strtok(str_copy, delim);
                        while (token != NULL)
                        {
                            numbers[i++] = atof(token);
                            token = strtok(NULL, delim);
                        }
                        // for (int j = 0; j < i; j++)
                        // {
                        //     gui_log("%d ", numbers[j]);
                        // }
                        // gui_log("\n");
                        gui_free(str_copy);
                    }
                    int
                    pixel_bytes = 4;
                    size_t buffer_size = h * w * pixel_bytes + sizeof(gui_rgb_data_head_t);
                    uint8_t *buffer = gui_lower_malloc(buffer_size);
                    memset(buffer, 0, buffer_size);
                    NVGcontext *vg = gui_canvas_output_buffer_blank(GUI_CANVAS_OUTPUT_RGBA, 0, w, h, buffer);
                    if (!strcmp(style, "waveform"))
                    {
                        gui_wave_render(vg, 0, 0, w,
                                        h,
                                        item_count,
                                        numbers,
                                        color,
                                        max,
                                        min);
                    }
                    else if (!strcmp(style, "bar"))
                    {
                        gui_bar_render(vg, 0, 0, w,
                                       h,
                                       item_count,
                                       numbers,
                                       color,
                                       max,
                                       min, stroke_width);
                    }
                    else if (!strcmp(style, "line"))
                    {
                        gui_line_render(vg, 0, 0, w,
                                        h,
                                        item_count,
                                        numbers,
                                        color,
                                        max,
                                        min, stroke_width);
                    }
                    else
                    {
                        gui_wave_render(vg, 0, 0, w,
                                        h,
                                        item_count,
                                        numbers,
                                        color,
                                        max,
                                        min);
                    }

                    gui_canvas_output_buffer_blank_close(vg);
                    char *ptxt = get_space_string_head(p->txt);
                    gui_img_t *img = gui_img_create_from_mem(parent, ptxt, buffer, x, y,
                                                             0, 0);
                    gui_img_set_mode(img, IMG_SRC_OVER_MODE);

                    parent = (void *)img;
                    parent->obj_cb = gui_canvas_img_cb;
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
// Define various formats
#define MON_FORMAT     0         // "MON"
#define Mon_FORMAT     1         // "Mon"
#define Mo_FORMAT      2         // "Mo"
#define FULL_DATE      3         // "Mon, January 1"
#define SHORT_DATE     4         // "Mon 1"
#define TIME_24HR      5         // "00:00"
#define PARTIAL_DATE   6         // "Mon, Jan 1"
#define NUM_DATE       7         // "1/JANUARY"
#define FULL_DAY       8         // "MONDAY"
#define JUST_HOUR       9         // "hour"
#define JUST_MINUTE       10      // "minute"
#define JUST_SECOND      11       // "second"
#define DEFAULT_FORMAT 100       // Default

static int is_format(const char *src)
{
    if (!src) { return DEFAULT_FORMAT; }
    if (strcmp(src, "MON") == 0) { return MON_FORMAT; }
    if (strcmp(src, "Mon") == 0) { return Mon_FORMAT; }
    if (strcmp(src, "Mo") == 0) { return Mo_FORMAT; }
    if (strcmp(src, "Mon, January 1") == 0) { return FULL_DATE; }
    if (strcmp(src, "Mon 1") == 0) { return SHORT_DATE; }
    if (strcmp(src, "00:00") == 0) { return TIME_24HR; }
    if (strcmp(src, "Mon, Jan 1") == 0) { return PARTIAL_DATE; }
    if (strcmp(src, "1/JANUARY") == 0) { return NUM_DATE; }
    if (strcmp(src, "MONDAY") == 0) { return FULL_DAY; }
    if (strcmp(src, "hour") == 0) { return JUST_HOUR; }
    if (strcmp(src, "minute") == 0) { return JUST_MINUTE; }
    if (strcmp(src, "second") == 0) { return JUST_SECOND; }
    return DEFAULT_FORMAT;
}

static size_t my_strftime(char *__restrict__ buf, size_t maxsize, const char *__restrict__ format,
                          const struct tm *__restrict__ tm)
{
    size_t pos = 0;

    const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char *full_days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    const char *full_months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    while (*format && pos < maxsize)
    {
        if (*format == '%')
        {
            format++;
            if (!*format) { break; }

            switch (*format)
            {
            case 'a': // Abbreviated weekday name
                pos += snprintf(buf + pos, maxsize - pos, "%s", days[tm->tm_wday]);
                break;
            case 'A': // Full weekday name
                pos += snprintf(buf + pos, maxsize - pos, "%s", full_days[tm->tm_wday]);
                break;
            case 'b': // Abbreviated month name
                pos += snprintf(buf + pos, maxsize - pos, "%s", months[tm->tm_mon]);
                break;
            case 'B': // Full month name
                pos += snprintf(buf + pos, maxsize - pos, "%s", full_months[tm->tm_mon]);
                break;
            case 'Y': // Year
                pos += snprintf(buf + pos, maxsize - pos, "%d", 1900 + tm->tm_year);
                break;
            case 'm': // Month (01-12)
                pos += snprintf(buf + pos, maxsize - pos, "%02d", tm->tm_mon + 1);
                break;
            case 'd': // Day (01-31)
                pos += snprintf(buf + pos, maxsize - pos, "%02d", tm->tm_mday);
                break;
            case 'H': // Hour (00-23)
                pos += snprintf(buf + pos, maxsize - pos, "%02d", tm->tm_hour);
                break;
            case 'M': // Minute (00-59)
                pos += snprintf(buf + pos, maxsize - pos, "%02d", tm->tm_min);
                break;
            case 'S': // Second (00-59)
                pos += snprintf(buf + pos, maxsize - pos, "%02d", tm->tm_sec);
                break;
            default: // Unknown specifier
                if (pos < maxsize) { buf[pos++] = '%'; }
                if (pos < maxsize) { buf[pos++] = *format; }
                break;
            }
        }
        else
        {
            buf[pos++] = *format;
        }
        format++;
    }

    if (pos >= maxsize)
    {
        if (maxsize > 0) { buf[maxsize - 1] = '\0'; }
        return 0;
    }

    buf[pos] = '\0';
    return pos;
}

static void text_animate_watchface_callback(void *p, void *this_widget, gui_animate_t *animate)
{
    const char *input_type = p;
    gui_text_t *date_txt = this_widget;
    if (date_txt)
    {
        size_t buffer_size = 100;
        char *buffer = date_txt->content;
        gui_free(buffer);
        buffer = (char *)gui_malloc(buffer_size);
        if (!buffer)
        {
            return;
        }

        int type = is_format(input_type);
        time_t rawtime; GUI_UNUSED(rawtime);
        struct tm *timeinfo; GUI_UNUSED(timeinfo);

#if __WIN32
        time(&rawtime);
        timeinfo = localtime(&rawtime);
#else
        // extern struct tm watch_clock_get(void);
        // struct tm watch_time = watch_clock_get();
        // timeinfo = &watch_time;
        // gui_log("mday: %d, month: %d\n", timeinfo->tm_mday, timeinfo->tm_mon);
#endif
        // Choose the formatting string based on the type
        switch (type)
        {
        case MON_FORMAT:
            my_strftime(buffer, buffer_size, "%a", timeinfo); // Display short week name (Mon)
            for (char *p = buffer; *p; ++p) { *p = toupper(*p); } // Convert to uppercase (MON)
            break;
        case Mon_FORMAT:
            my_strftime(buffer, buffer_size, "%a", timeinfo); // Display short week name (Mon)
            break;
        case Mo_FORMAT:
            my_strftime(buffer, buffer_size, "%a", timeinfo); // Display short week name (Mon)
            buffer[2] = '\0'; // Only display the first two letters (Mo)
            break;
        case FULL_DATE:
            my_strftime(buffer, buffer_size, "%a, %B %d", timeinfo); // Display full date (Mon, January 1)
            break;
        case SHORT_DATE:
            my_strftime(buffer, buffer_size, "%a %d", timeinfo); // Display short date (Mon 1)
            break;
        case TIME_24HR:
            my_strftime(buffer, buffer_size, "%H:%M", timeinfo); // Display 24-hour format time (00:00)
            break;
        case PARTIAL_DATE:
            my_strftime(buffer, buffer_size, "%a, %b %d", timeinfo); // Display partial date (Mon, Jan 1)
            break;
        case NUM_DATE:
            my_strftime(buffer, buffer_size, "%d/%B", timeinfo); // Display numeric date (1/JANUARY)
            // Convert the month part to uppercase
            for (char *p = strchr(buffer, '/') + 1; *p; ++p) { *p = toupper(*p); }
            break;
        case FULL_DAY:
            my_strftime(buffer, buffer_size, "%A", timeinfo); // Display full week name (Monday)
            for (char *p = buffer; *p; ++p) { *p = toupper(*p); } // Convert to uppercase (MONDAY)
            break;
        case JUST_HOUR:
            my_strftime(buffer, buffer_size, "%H", timeinfo); // Display 24-hour format time (00:00)
            break;
        case JUST_MINUTE:
            my_strftime(buffer, buffer_size, "%M", timeinfo); // Display 24-hour format time (00:00)
            break;
        case JUST_SECOND:
            my_strftime(buffer, buffer_size, "%S", timeinfo); // Display 24-hour format time (00:00)
            break;
        default:
            my_strftime(buffer, buffer_size, "%a, %b %d", timeinfo); // Default: (Mon, Jan 1)
            break;
        }
// #endif

        date_txt->content = buffer;
        gui_text_content_set(date_txt, buffer, strlen(buffer));
    }
}

#define GET_TIME \
    uint16_t seconds = 0;\
    uint16_t minute = 0;\
    uint16_t hour = 0;\
    int millisecond = 0;\
    get_time(&seconds, &minute, &hour, &millisecond);

static void get_time(uint16_t *seconds, uint16_t *minute, uint16_t *hour, int *millisecond)
{
#ifndef __WIN32
    // extern struct tm watch_clock_get(void);
    // extern uint16_t millisecond_cnt;
    // struct tm watch_time = watch_clock_get();
    // *millisecond = millisecond_cnt;
    // *seconds = watch_time.tm_sec;
    // *minute = watch_time.tm_min;
    // *hour = watch_time.tm_hour;
    *millisecond = 0;
    *seconds = 0;
    *minute = 0;
    *hour = 0;
#else
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    *seconds = timeinfo->tm_sec;
    *minute = timeinfo->tm_min;
    *hour = timeinfo->tm_hour;
    *millisecond = 0;
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    *millisecond = spec.tv_nsec / 1000000;
#endif
}
static void img_animate_watchface_callback(void *p, void *this_widget, gui_animate_t *animate)
{
    GET_TIME
    const float angle_per_second = 360.0f / 60.0f;
    gui_img_t *hand = this_widget;
    switch ((size_t)p)
    {
    case HOUR_ANIMATION:
        {
            float angle_hour = (hour % 12) * M_PI / 6 + minute * M_PI / 360;
            gui_img_translate(hand, hand->base.w / 2, gui_get_screen_height() / 2 - hand->base.y);
            gui_img_rotation(hand, angle_hour * 180 / M_PI,  hand->base.w / 2,
                             gui_get_screen_height() / 2 - hand->base.y);
        }
        break;
    case MINUTE_ANIMATION:
        {
            float angle_min  = minute * M_PI / 30 + seconds * M_PI / 1800;
            gui_img_translate(hand, hand->base.w / 2, gui_get_screen_height() / 2 - hand->base.y);
            gui_img_rotation(hand, angle_min * 180 / M_PI, hand->base.w / 2,
                             gui_get_screen_height() / 2 - hand->base.y);
        }
        break;
    case SECOND_ANIMATION:
        {
            float angle_sec = (seconds + millisecond / 1000.0f) * angle_per_second;
            gui_img_translate(hand, hand->base.w / 2, gui_get_screen_height() / 2 - hand->base.y);
            gui_img_rotation(hand, angle_sec,  hand->base.w / 2,
                             gui_get_screen_height() / 2 - hand->base.y);
        }
        break;
    default:
        break;
    }
}
static void img_animate_watchface_callback_hour(void *p, void *this_widget, gui_animate_t *animate)
{
    GET_TIME

    gui_img_t *hand = this_widget;
    {
        two_integers *cxcy = p;
        float angle_hour = (hour % 12) * M_PI / 6 + minute * M_PI / 360;
        gui_img_translate(hand, cxcy->num1, cxcy->num2);
        gui_img_rotation(hand, angle_hour * 180 / M_PI,  cxcy->num1, cxcy->num2);
    }
}
static void img_animate_watchface_callback_minute(void *p, void *this_widget,
                                                  gui_animate_t *animate)
{
    GET_TIME

    gui_img_t *hand = this_widget;
    {
        two_integers *cxcy = p;
        float angle_min  = minute * M_PI / 30 + seconds * M_PI / 1800;
        gui_img_translate(hand, cxcy->num1, cxcy->num2);
        gui_img_rotation(hand, angle_min * 180 / M_PI, cxcy->num1, cxcy->num2);
    }
}
static void img_animate_watchface_callback_second(void *p, void *this_widget,
                                                  gui_animate_t *animate)
{
    GET_TIME
    const float angle_per_second = 360.0f / 60.0f;
    gui_img_t *hand = this_widget;
    {
        two_integers *cxcy = p;
        float angle_sec = (seconds + millisecond / 1000.0f) * angle_per_second;
        gui_img_translate(hand, cxcy->num1, cxcy->num2);
        gui_img_rotation(hand, angle_sec,  cxcy->num1, cxcy->num2);
    }
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
                    //gui_log("animate_create_handle:%s: %s", name, p->txt);
                    char *type = 0;
                    char *from = 0;
                    char *to = 0;
                    char *dur = 0;
                    char *repeatCount = 0;
                    char *pause = 0; GUI_UNUSED(pause);
                    float scale_x = 1; GUI_UNUSED(scale_x);
                    float scale_y = 1; GUI_UNUSED(scale_y);
                    uint8_t opacity = 255; GUI_UNUSED(opacity);
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
                            int dur_num = 0; GUI_UNUSED(dur_num);
                            int repeat_num = 0; GUI_UNUSED(repeat_num);
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
                                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
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
                                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                            }
                        }
                        if (!strcmp(type, "image frame") && from)
                        {
                            int file_count = 0;
                            {
                                gui_fs_dir *dir = NULL;
                                struct gui_fs_dirent *entry;
                                char *path = gui_malloc(strlen(from) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, from);
                                if ((dir = gui_fs_opendir(path)) == NULL)
                                {
                                    gui_free(path);
                                    return 0;
                                }
                                gui_free(path);
                                while ((entry = gui_fs_readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        file_count++;
                                    }
                                }
                                gui_fs_closedir(dir);
                            }

                            image_array = gui_malloc(file_count * sizeof(void *));
                            {
                                gui_fs_dir *dir = NULL;
                                struct gui_fs_dirent *entry;
                                char *path = gui_malloc(strlen(from) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, from);
                                if ((dir = gui_fs_opendir(path)) == NULL)
                                {
                                    gui_free(path);
                                    return 0;
                                }


                                while ((entry = gui_fs_readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        char *path2 = gui_malloc(strlen(entry->d_name) + strlen(from) + 2);
                                        sprintf(path2, "%s/%s", from, entry->d_name);
                                        image_array[count++] = (void *)gui_get_image_file_address(path2);
                                    }
                                }
                                gui_free(path);
                                gui_fs_closedir(dir);
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

                            //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
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

                            //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
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
            //gui_log("%s,%s\n", i->name, i->txt);
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
    GUI_UNUSED(level); GUI_UNUSED(order);
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
        if (strncmp(i->name, "multiLevel", strlen("multiLevel")) != 0)
        {
            //gui_log("%s,%s\n", i->name, i->txt);
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

            *text = get_space_string_head(i->txt); //gui_log("level:%s\n", *text);
            size_t j = 0;
            while (true)
            {
                if (!(i->attr[j]))
                {
                    break;
                }
                ////gui_log("p->attr[i]:%s\n", p->attr[i]);
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
        gui_fs_dir *dir = 0;
        struct gui_fs_dirent *entry;

        char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
        sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
        if ((dir = gui_fs_opendir(path)) == NULL)
        {
            gui_free(path);
            //perror("gui_fs_opendir() failed");
            return;
        }
        gui_free(path);
        while ((entry = gui_fs_readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                file_count++;
            }


        }
        gui_fs_closedir(dir);
    }
    image_array = gui_malloc(file_count * sizeof(void *));
    {
        gui_fs_dir *dir = 0;
        struct gui_fs_dirent *entry;
        char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
        sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
        if ((dir = gui_fs_opendir(path)) == NULL)
        {
            gui_free(path);
            //perror("gui_fs_opendir() failed");
            return;
        }

        int count = 0;
        while ((entry = gui_fs_readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                char *path2 = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                sprintf(path2, "%s/%s", folder, entry->d_name);
                image_array[count++] = (void *)gui_get_image_file_address(path2);
            }

        }
        gui_free(path);
        gui_fs_closedir(dir);


    }

}
static void load_return_image()
{
    return_image = (void *)gui_get_image_file_address("app/system/resource/return.bin");
    return_image_hl = (void *)gui_get_image_file_address("app/system/resource/returnhl.bin");
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
    ezxml_t title = 0; GUI_UNUSED(title);
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
    //gui_log(" ");
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
                ////gui_log("p->attr[i]:%s\n", p->attr[i]);
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
        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
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
// Function that recursively searches for the <preview> tag at any level
static void find_preview_tags(ezxml_t node, char **file_output)
{
    if (!node) { return; }
    // If the node's name is "preview", print the file attribute
    if (strcmp(node->name, "preview") == 0)
    {
        const char *file = gui_strdup(ezxml_attr(node, "file"));
        if (file)
        {
            //gui_log("File attribute value: %s\n", file);
            *file_output = (char *)file;
            return;
        }
    }

    // Recurse down to all children
    ezxml_t child = node->child;
    while (child)
    {
        find_preview_tags(child, file_output);
        child = child->sibling;
    }
}
char *gui_dom_get_preview_image_file(const char *xml_file)
{
    ezxml_t xml = ezxml_parse_file(xml_file);
    if (!xml)
    {
        gui_log("Failed to load XML file.\n");
    }
    char *file_output = 0;
    find_preview_tags(xml, &file_output);
    ezxml_free(xml);
    return file_output;
}
void create_tree_nest(const char *xml, void *obj)
{
    GUI_WIDGET_TRY_EXCEPT(obj);
    gui_app_t *app = gui_obj_tree_get_app(obj);
    app->xml = xml;
    ezxml_t f1 = ezxml_parse_file(xml);
    if (f1)
    {
        foreach_create(f1, obj);
    }
    ezxml_free(f1);
}
void gui_dom_create_tree_nest(const char *xml, gui_obj_t *parent_widget)
{
    create_tree_nest(xml, parent_widget);
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
                    //gui_log(" ");
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
static const uint8_t *gui_get_image_file_address(const char *image_file_path)
{
    const uint8_t *addr = gui_get_file_address(image_file_path);
    if (addr == 0)
    {
        static const unsigned char image404[1571] =
        {
            0x10, 0x00, 0x32, 0x00, 0x32, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00,
            0x32, 0x00, 0x00, 0x00, 0xd8, 0x00, 0x00, 0x00, 0xdb, 0x00, 0x00, 0x00, 0xde, 0x00, 0x00, 0x00,
            0xe1, 0x00, 0x00, 0x00, 0xe4, 0x00, 0x00, 0x00, 0xe7, 0x00, 0x00, 0x00, 0xea, 0x00, 0x00, 0x00,
            0xed, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0xf9, 0x00, 0x00, 0x00, 0x47, 0x01, 0x00, 0x00,
            0x71, 0x01, 0x00, 0x00, 0x9e, 0x01, 0x00, 0x00, 0xd1, 0x01, 0x00, 0x00, 0xec, 0x01, 0x00, 0x00,
            0x07, 0x02, 0x00, 0x00, 0x28, 0x02, 0x00, 0x00, 0x4c, 0x02, 0x00, 0x00, 0x73, 0x02, 0x00, 0x00,
            0x97, 0x02, 0x00, 0x00, 0xc4, 0x02, 0x00, 0x00, 0xf4, 0x02, 0x00, 0x00, 0x21, 0x03, 0x00, 0x00,
            0x51, 0x03, 0x00, 0x00, 0x75, 0x03, 0x00, 0x00, 0x96, 0x03, 0x00, 0x00, 0xc0, 0x03, 0x00, 0x00,
            0xe7, 0x03, 0x00, 0x00, 0x0e, 0x04, 0x00, 0x00, 0x35, 0x04, 0x00, 0x00, 0x5c, 0x04, 0x00, 0x00,
            0x80, 0x04, 0x00, 0x00, 0xa7, 0x04, 0x00, 0x00, 0xce, 0x04, 0x00, 0x00, 0xf2, 0x04, 0x00, 0x00,
            0x13, 0x05, 0x00, 0x00, 0x2e, 0x05, 0x00, 0x00, 0x49, 0x05, 0x00, 0x00, 0x67, 0x05, 0x00, 0x00,
            0x82, 0x05, 0x00, 0x00, 0xb2, 0x05, 0x00, 0x00, 0xfa, 0x05, 0x00, 0x00, 0x03, 0x06, 0x00, 0x00,
            0x06, 0x06, 0x00, 0x00, 0x09, 0x06, 0x00, 0x00, 0x0c, 0x06, 0x00, 0x00, 0x0f, 0x06, 0x00, 0x00,
            0x12, 0x06, 0x00, 0x00, 0x15, 0x06, 0x00, 0x00, 0x18, 0x06, 0x00, 0x00, 0x1b, 0x06, 0x00, 0x00,
            0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32,
            0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x1e, 0xff, 0xff, 0x01, 0xdf, 0xff, 0x13, 0xff,
            0xff, 0x07, 0xff, 0xff, 0x01, 0xbe, 0xf7, 0x01, 0xdb, 0xde, 0x01, 0xba, 0xd6, 0x01, 0x79, 0xce,
            0x01, 0x38, 0xc6, 0x01, 0xf7, 0xbd, 0x01, 0xb6, 0xb5, 0x01, 0x75, 0xad, 0x01, 0x34, 0xa5, 0x01,
            0xf3, 0x9c, 0x01, 0x92, 0x94, 0x01, 0x51, 0x8c, 0x01, 0x6d, 0x6b, 0x01, 0x18, 0xc6, 0x08, 0xff,
            0xff, 0x01, 0xaa, 0x52, 0x02, 0x00, 0x00, 0x01, 0x82, 0x10, 0x01, 0xc7, 0x39, 0x01, 0x2c, 0x63,
            0x01, 0x71, 0x8c, 0x01, 0xf7, 0xbd, 0x01, 0x1c, 0xe7, 0x01, 0xdf, 0xff, 0x0b, 0xff, 0xff, 0x05,
            0xff, 0xff, 0x01, 0xbe, 0xf7, 0x01, 0xe7, 0x39, 0x0c, 0x00, 0x00, 0x01, 0x65, 0x29, 0x07, 0xff,
            0xff, 0x01, 0x5d, 0xef, 0x01, 0x61, 0x08, 0x0a, 0x00, 0x00, 0x01, 0x61, 0x08, 0x01, 0xa6, 0x31,
            0x01, 0x0c, 0x63, 0x01, 0xd3, 0x9c, 0x07, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0xbe, 0xf7, 0x0d,
            0x00, 0x00, 0x01, 0xe7, 0x39, 0x0a, 0xff, 0xff, 0x01, 0x5d, 0xef, 0x01, 0x38, 0xc6, 0x01, 0xd3,
            0x9c, 0x01, 0x6d, 0x6b, 0x01, 0x08, 0x42, 0x01, 0x24, 0x21, 0x01, 0x20, 0x00, 0x08, 0x00, 0x00,
            0x01, 0xd3, 0x9c, 0x05, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0xe7, 0x39, 0x02, 0x00, 0x00, 0x01,
            0x71, 0x8c, 0x01, 0x3c, 0xe7, 0x01, 0x7d, 0xef, 0x01, 0xbe, 0xf7, 0x19, 0xff, 0xff, 0x01, 0xdf,
            0xff, 0x01, 0x9a, 0xd6, 0x01, 0x14, 0xa5, 0x01, 0xae, 0x73, 0x01, 0x49, 0x4a, 0x01, 0xe3, 0x18,
            0x03, 0x00, 0x00, 0x01, 0xb6, 0xb5, 0x04, 0xff, 0xff, 0x04, 0xff, 0xff, 0x02, 0x00, 0x00, 0x01,
            0x71, 0x8c, 0x23, 0xff, 0xff, 0x01, 0xbe, 0xf7, 0x01, 0x61, 0x08, 0x01, 0x00, 0x00, 0x01, 0x82,
            0x10, 0x04, 0xff, 0xff, 0x03, 0xff, 0xff, 0x01, 0x1c, 0xe7, 0x02, 0x00, 0x00, 0x01, 0xfb, 0xde,
            0x24, 0xff, 0xff, 0x01, 0xf7, 0xbd, 0x02, 0x00, 0x00, 0x01, 0xbe, 0xf7, 0x03, 0xff, 0xff, 0x03,
            0xff, 0xff, 0x01, 0x3c, 0xe7, 0x02, 0x00, 0x00, 0x01, 0xba, 0xd6, 0x1a, 0xff, 0xff, 0x02, 0x34,
            0xa5, 0x08, 0xff, 0xff, 0x01, 0xdb, 0xde, 0x02, 0x00, 0x00, 0x01, 0x3c, 0xe7, 0x03, 0xff, 0xff,
            0x03, 0xff, 0xff, 0x01, 0x7d, 0xef, 0x02, 0x00, 0x00, 0x01, 0x9a, 0xd6, 0x18, 0xff, 0xff, 0x01,
            0x34, 0xa5, 0x04, 0x00, 0x00, 0x01, 0x14, 0xa5, 0x06, 0xff, 0xff, 0x01, 0x96, 0xb5, 0x02, 0x00,
            0x00, 0x04, 0xff, 0xff, 0x03, 0xff, 0xff, 0x01, 0xbe, 0xf7, 0x02, 0x00, 0x00, 0x01, 0x59, 0xce,
            0x17, 0xff, 0xff, 0x01, 0x3c, 0xe7, 0x06, 0x00, 0x00, 0x01, 0x3c, 0xe7, 0x05, 0xff, 0xff, 0x01,
            0x10, 0x84, 0x01, 0x00, 0x00, 0x01, 0x41, 0x08, 0x04, 0xff, 0xff, 0x04, 0xff, 0xff, 0x02, 0x00,
            0x00, 0x01, 0x18, 0xc6, 0x17, 0xff, 0xff, 0x01, 0xae, 0x73, 0x06, 0x00, 0x00, 0x01, 0x8e, 0x73,
            0x05, 0xff, 0xff, 0x01, 0xaa, 0x52, 0x01, 0x00, 0x00, 0x01, 0x45, 0x29, 0x04, 0xff, 0xff, 0x04,
            0xff, 0xff, 0x02, 0x00, 0x00, 0x01, 0xd7, 0xbd, 0x05, 0xff, 0xff, 0x01, 0xc7, 0x39, 0x01, 0x18,
            0xc6, 0x10, 0xff, 0xff, 0x01, 0x4d, 0x6b, 0x06, 0x00, 0x00, 0x01, 0x2c, 0x63, 0x05, 0xff, 0xff,
            0x01, 0x65, 0x29, 0x01, 0x00, 0x00, 0x01, 0x69, 0x4a, 0x04, 0xff, 0xff, 0x04, 0xff, 0xff, 0x02,
            0x00, 0x00, 0x01, 0x75, 0xad, 0x04, 0xff, 0xff, 0x01, 0xc7, 0x39, 0x02, 0x00, 0x00, 0x01, 0xf7,
            0xbd, 0x0f, 0xff, 0xff, 0x01, 0xf7, 0xbd, 0x06, 0x00, 0x00, 0x01, 0xf7, 0xbd, 0x05, 0xff, 0xff,
            0x01, 0x20, 0x00, 0x01, 0x00, 0x00, 0x01, 0xef, 0x7b, 0x04, 0xff, 0xff, 0x04, 0xff, 0xff, 0x02,
            0x00, 0x00, 0x01, 0x34, 0xa5, 0x03, 0xff, 0xff, 0x01, 0x28, 0x42, 0x04, 0x00, 0x00, 0x01, 0x55,
            0xad, 0x0f, 0xff, 0xff, 0x01, 0x8a, 0x52, 0x04, 0x00, 0x00, 0x01, 0x69, 0x4a, 0x06, 0xff, 0xff,
            0x02, 0x00, 0x00, 0x01, 0x55, 0xad, 0x04, 0xff, 0xff, 0x04, 0xff, 0xff, 0x02, 0x00, 0x00, 0x01,
            0xf3, 0x9c, 0x02, 0xff, 0xff, 0x01, 0x69, 0x4a, 0x06, 0x00, 0x00, 0x01, 0x34, 0xa5, 0x0f, 0xff,
            0xff, 0x01, 0x34, 0xa5, 0x02, 0x28, 0x42, 0x01, 0x34, 0xa5, 0x06, 0xff, 0xff, 0x01, 0x3c, 0xe7,
            0x02, 0x00, 0x00, 0x01, 0x79, 0xce, 0x04, 0xff, 0xff, 0x04, 0xff, 0xff, 0x02, 0x00, 0x00, 0x01,
            0xd3, 0x9c, 0x01, 0xff, 0xff, 0x01, 0x0c, 0x63, 0x08, 0x00, 0x00, 0x01, 0x51, 0x8c, 0x18, 0xff,
            0xff, 0x01, 0xb6, 0xb5, 0x02, 0x00, 0x00, 0x01, 0x9e, 0xf7, 0x04, 0xff, 0xff, 0x04, 0xff, 0xff,
            0x01, 0x20, 0x00, 0x01, 0x00, 0x00, 0x01, 0x55, 0xad, 0x01, 0xef, 0x7b, 0x0a, 0x00, 0x00, 0x01,
            0x30, 0x84, 0x17, 0xff, 0xff, 0x01, 0x71, 0x8c, 0x02, 0x00, 0x00, 0x05, 0xff, 0xff, 0x04, 0xff,
            0xff, 0x01, 0x61, 0x08, 0x01, 0x00, 0x00, 0x01, 0xe3, 0x18, 0x0c, 0x00, 0x00, 0x01, 0x6d, 0x6b,
            0x0c, 0xff, 0xff, 0x01, 0xb6, 0xb5, 0x01, 0xf7, 0xbd, 0x08, 0xff, 0xff, 0x01, 0xeb, 0x5a, 0x01,
            0x00, 0x00, 0x01, 0xc3, 0x18, 0x05, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0x82, 0x10, 0x0f, 0x00,
            0x00, 0x01, 0x0c, 0x63, 0x09, 0xff, 0xff, 0x01, 0x18, 0xc6, 0x03, 0x00, 0x00, 0x01, 0xc3, 0x18,
            0x07, 0xff, 0xff, 0x01, 0x86, 0x31, 0x01, 0x00, 0x00, 0x01, 0x49, 0x4a, 0x05, 0xff, 0xff, 0x04,
            0xff, 0xff, 0x01, 0xc3, 0x18, 0x10, 0x00, 0x00, 0x01, 0x69, 0x4a, 0x07, 0xff, 0xff, 0x01, 0x8e,
            0x73, 0x05, 0x00, 0x00, 0x01, 0x24, 0x21, 0x06, 0xff, 0xff, 0x01, 0x82, 0x10, 0x01, 0x00, 0x00,
            0x01, 0x8e, 0x73, 0x05, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0x04, 0x21, 0x11, 0x00, 0x00, 0x01,
            0x08, 0x42, 0x06, 0xff, 0xff, 0x01, 0xae, 0x73, 0x06, 0x00, 0x00, 0x01, 0xae, 0x73, 0x04, 0xff,
            0xff, 0x01, 0xdf, 0xff, 0x02, 0x00, 0x00, 0x01, 0x14, 0xa5, 0x05, 0xff, 0xff, 0x04, 0xff, 0xff,
            0x01, 0x24, 0x21, 0x12, 0x00, 0x00, 0x01, 0xc7, 0x39, 0x06, 0xff, 0xff, 0x01, 0xa6, 0x31, 0x06,
            0x00, 0x00, 0x01, 0x18, 0xc6, 0x03, 0xff, 0xff, 0x01, 0x3c, 0xe7, 0x02, 0x00, 0x00, 0x01, 0x79,
            0xce, 0x05, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0x65, 0x29, 0x13, 0x00, 0x00, 0x01, 0x04, 0x21,
            0x06, 0xff, 0xff, 0x07, 0x00, 0x00, 0x01, 0xbe, 0xf7, 0x02, 0xff, 0xff, 0x01, 0xf7, 0xbd, 0x02,
            0x00, 0x00, 0x01, 0x9e, 0xf7, 0x05, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0xa6, 0x31, 0x14, 0x00,
            0x00, 0x01, 0xc3, 0x18, 0x01, 0xbe, 0xf7, 0x04, 0xff, 0xff, 0x01, 0x3c, 0xe7, 0x06, 0x00, 0x00,
            0x01, 0x04, 0x21, 0x02, 0xff, 0xff, 0x01, 0x92, 0x94, 0x02, 0x00, 0x00, 0x06, 0xff, 0xff, 0x04,
            0xff, 0xff, 0x01, 0xc7, 0x39, 0x15, 0x00, 0x00, 0x01, 0x5d, 0xef, 0x04, 0xff, 0xff, 0x01, 0xf3,
            0x9c, 0x07, 0x00, 0x00, 0x01, 0x0c, 0x63, 0x01, 0xff, 0xff, 0x01, 0x2c, 0x63, 0x01, 0x00, 0x00,
            0x01, 0xe3, 0x18, 0x06, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0x08, 0x42, 0x14, 0x00, 0x00, 0x01,
            0x9e, 0xf7, 0x04, 0xff, 0xff, 0x01, 0x28, 0x42, 0x09, 0x00, 0x00, 0x01, 0x79, 0xce, 0x01, 0x28,
            0x42, 0x01, 0x00, 0x00, 0x01, 0xe7, 0x39, 0x06, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0x49, 0x4a,
            0x13, 0x00, 0x00, 0x01, 0x3c, 0xe7, 0x03, 0xff, 0xff, 0x01, 0xfb, 0xde, 0x0c, 0x00, 0x00, 0x01,
            0xa2, 0x10, 0x01, 0x00, 0x00, 0x01, 0x6d, 0x6b, 0x06, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0x8a,
            0x52, 0x12, 0x00, 0x00, 0x01, 0xdb, 0xde, 0x03, 0xff, 0xff, 0x01, 0xd3, 0x9c, 0x0f, 0x00, 0x00,
            0x01, 0xd3, 0x9c, 0x06, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0x2c, 0x63, 0x11, 0x00, 0x00, 0x01,
            0x79, 0xce, 0x03, 0xff, 0xff, 0x01, 0x8a, 0x52, 0x10, 0x00, 0x00, 0x01, 0x18, 0xc6, 0x06, 0xff,
            0xff, 0x04, 0xff, 0xff, 0x01, 0x75, 0xad, 0x10, 0x00, 0x00, 0x01, 0xf7, 0xbd, 0x02, 0xff, 0xff,
            0x01, 0x7d, 0xef, 0x01, 0x20, 0x00, 0x11, 0x00, 0x00, 0x01, 0xdf, 0xff, 0x06, 0xff, 0xff, 0x05,
            0xff, 0xff, 0x01, 0x61, 0x08, 0x0e, 0x00, 0x00, 0x01, 0x96, 0xb5, 0x02, 0xff, 0xff, 0x01, 0x30,
            0x84, 0x12, 0x00, 0x00, 0x01, 0x65, 0x29, 0x07, 0xff, 0xff, 0x05, 0xff, 0xff, 0x01, 0x9e, 0xf7,
            0x01, 0x82, 0x10, 0x0c, 0x00, 0x00, 0x01, 0x14, 0xa5, 0x03, 0xff, 0xff, 0x01, 0x5d, 0xef, 0x01,
            0x59, 0xce, 0x01, 0xf3, 0x9c, 0x01, 0x8e, 0x73, 0x01, 0x28, 0x42, 0x01, 0xc3, 0x18, 0x01, 0x20,
            0x00, 0x0c, 0x00, 0x00, 0x01, 0x9e, 0xf7, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x01, 0xd3, 0x9c,
            0x01, 0x49, 0x4a, 0x01, 0xc7, 0x39, 0x01, 0x08, 0x42, 0x01, 0x69, 0x4a, 0x01, 0x8a, 0x52, 0x01,
            0xeb, 0x5a, 0x01, 0x2c, 0x63, 0x01, 0x6d, 0x6b, 0x01, 0xae, 0x73, 0x01, 0xcf, 0x7b, 0x01, 0x75,
            0xad, 0x0b, 0xff, 0xff, 0x01, 0x7d, 0xef, 0x01, 0x9a, 0xd6, 0x01, 0x34, 0xa5, 0x01, 0xcf, 0x7b,
            0x01, 0x69, 0x4a, 0x01, 0x04, 0x21, 0x04, 0x00, 0x00, 0x01, 0xe7, 0x39, 0x01, 0xbe, 0xf7, 0x08,
            0xff, 0xff, 0x26, 0xff, 0xff, 0x01, 0x7d, 0xef, 0x0b, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff,
            0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff,
            0x32, 0xff, 0xff,
        };
        addr = image404;
    }
    return addr;
}
/**
 * @brief Retrieves the address of an image file.
 *
 * This function takes the file path of an image and returns the memory address
 * where the image data is stored. It is used to access image files in the GUI domain.
 *
 * @param image_file_path The path to the image file whose address is to be retrieved.
 *                        This should be a null-terminated string representing the file path.
 *
 * @return A pointer to a uint8_t representing the address of the image file data.
 *         If the file cannot be found or accessed, the function may return a error image data.
 */
const uint8_t *gui_dom_get_image_file_address(const char *image_file_path)
{
    return gui_get_image_file_address(image_file_path);
}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_weather_callback)
{
    if (!animate->Beginning_frame)
    {
        return;
    }

    switch ((size_t)p)
    {
    case TEXT_WEATHER_CUR_ANIMATION:
        {
            const char *path =
                gui_get_path_by_relative("app/system/peripheral_simulation/json/simulation_data.json");
            int fd = gui_fs_open(path, 0);
            gui_free((void *)path);
            if (fd < 1)
            {
                return;
            }
            int size = gui_fs_lseek(fd, 0, SEEK_END);
            gui_fs_lseek(fd, 0, SEEK_SET);
            char *json_string = gui_malloc(size + 1);
            gui_fs_read(fd, json_string, size);
            gui_fs_close(fd);
            json_string[size] = '\0';
            int move = 0;
            //gui_log("%s\n",json_string);
            gui_get_json_value(json_string, "weather", "current", &move);
            gui_free(json_string);
            static char move_string[4];
            memset(move_string, 0, GUI_ARRAY_SIZE(move_string));
            sprintf(move_string, "%d", move);
            gui_text_content_set(this_widget, move_string, strlen(move_string));
        }
        break;

    default:
        break;
    }

}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animation_hr_callback)
{
    if (!animate->Beginning_frame)
    {
        return;
    }

    switch ((size_t)p)
    {
    case TEXT_HEART_RATE_CUR_ANIMATION:
        {
            const char *path =
                gui_get_path_by_relative("app/system/peripheral_simulation/json/simulation_data.json");
            int fd = gui_fs_open(path, 0);
            if (fd < 1)
            {
                return;
            }
            gui_free((void *)path);
            int size = gui_fs_lseek(fd, 0, SEEK_END);
            gui_fs_lseek(fd, 0, SEEK_SET);
            char *json_string = gui_malloc(size + 1);
            gui_fs_read(fd, json_string, size);
            gui_fs_close(fd);
            json_string[size] = '\0';
            int array_length = 0;
            //gui_log("%s\n",json_string);
            float *array = gui_get_json_array(json_string, "heart_rate", "array", &array_length);
            gui_free(json_string);
            if (array_length == 0 || array == 0)
            {
                return;
            }
            static char move_string[4];
            memset(move_string, 0, GUI_ARRAY_SIZE(move_string));
            sprintf(move_string, "%d", (int)array[array_length - 1]);
            gui_text_content_set(this_widget, move_string, strlen(move_string));
            gui_free(array);
        }
        break;

    default:
        break;
    }

}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_activity_callback)
{
    if (!animate->Beginning_frame)
    {
        return;
    }

    switch ((size_t)p)
    {
    case TEXT_ACTIVITY_EX_ANIMATION:
        {
            const char *path =
                gui_get_path_by_relative("app/system/peripheral_simulation/json/simulation_data.json");
            int fd = gui_fs_open(path, 0);
            if (fd < 1)
            {
                return;
            }
            gui_free((void *)path);
            int size = gui_fs_lseek(fd, 0, SEEK_END);
            gui_fs_lseek(fd, 0, SEEK_SET);
            char *json_string = gui_malloc(size + 1);
            gui_fs_read(fd, json_string, size);
            gui_fs_close(fd);
            json_string[size] = '\0';
            int move = 0;
            //gui_log("%s\n",json_string);
            gui_get_json_value(json_string, "activity", "exercise", &move);
            gui_free(json_string);

            static char move_string[4];
            memset(move_string, 0, GUI_ARRAY_SIZE(move_string));
            sprintf(move_string, "%d", (int)move);
            gui_text_content_set(this_widget, move_string, strlen(move_string));
        }
        break;

    default:
        break;
    }

}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_battery_callback)
{
    if (!animate->Beginning_frame)
    {
        return;
    }

    switch ((size_t)p)
    {
    case TEXT_BATTERY_CAPACITY_ANIMATION:
        {
            const char *path =
                gui_get_path_by_relative("app/system/peripheral_simulation/json/simulation_data.json");
            int fd = gui_fs_open(path, 0);
            if (fd < 1)
            {
                return;
            }
            gui_free((void *)path);
            int size = gui_fs_lseek(fd, 0, SEEK_END);
            gui_fs_lseek(fd, 0, SEEK_SET);
            char *json_string = gui_malloc(size + 1);
            gui_fs_read(fd, json_string, size);
            gui_fs_close(fd);
            json_string[size] = '\0';
            int move = 0;
            //gui_log("%s\n",json_string);
            gui_get_json_value(json_string, "battery", "capacity", &move);
            gui_free(json_string);

            static char move_string[4];
            memset(move_string, 0, GUI_ARRAY_SIZE(move_string));
            sprintf(move_string, "%d", (int)move);
            gui_text_content_set(this_widget, move_string, strlen(move_string));
        }
        break;

    default:
        break;
    }

}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(arc_animate_activity_callback)
{
    if (!animate->Beginning_frame)
    {
        return;
    }
    arc_animation_param_t *param = p;
    switch (param->aniamtion_type)
    {
    case ARC_ACTIVITY_EX_ANIMATION:
        {
            const char *path =
                gui_get_path_by_relative("app/system/peripheral_simulation/json/simulation_data.json");
            int fd = gui_fs_open(path, 0);
            if (fd < 1)
            {
                return;
            }
            gui_free((void *)path);
            int size = gui_fs_lseek(fd, 0, SEEK_END);
            gui_fs_lseek(fd, 0, SEEK_SET);
            char *json_string = gui_malloc(size + 1);
            gui_fs_read(fd, json_string, size);
            gui_fs_close(fd);
            json_string[size] = '\0';
            int move = 0;
            //gui_log("%s\n",json_string);
            gui_get_json_value(json_string, "activity", "exercise", &move);
            gui_free(json_string);
            int max = 60;
            int min = 0;
            float per = (float)(move - min) / (float)(max - min);
            //gui_log("%d,%f\n", move, per);
            arc_animation_param_t *param = p;
            {
                memset(param->target_buffer, 0, param->image_data_length);
                //int format = GUI_CANVAS_OUTPUT_RGBA;
                bool compression = 0; GUI_UNUSED(compression);
                int image_width = param->image_width;
                int image_height = param->image_height;  uint8_t *target_buffer = param->target_buffer;
                {
                    int pixel_length = 4;
                    //int data_length = 0;
                    uint8_t *buffer = 0;
                    uint8_t *output_data = 0;
                    // switch (format)
                    // {
                    // case GUI_CANVAS_OUTPUT_PNG:
                    // case GUI_CANVAS_OUTPUT_JPG:
                    //     {
                    //         data_length = image_width * image_height * pixel_length;
                    //         buffer = gui_lower_malloc(data_length);
                    //         memset(buffer, 0, data_length);
                    //     }
                    //     break;
                    // case GUI_CANVAS_OUTPUT_RGBA:
                    //     {
                    //         output_data = target_buffer;
                    //         buffer = output_data + sizeof(gui_rgb_data_head_t);
                    //         memset(output_data, 0, sizeof(gui_rgb_data_head_t));
                    //         gui_rgb_data_head_t *head = (void *)output_data;
                    //         head->type = ARGB8888;
                    //         head->w = image_width;
                    //         head->h = image_height;
                    //     }
                    //     break;
                    // case GUI_CANVAS_OUTPUT_RGB565:
                    //     {
                    //         pixel_length = 2;
                    //         output_data = target_buffer;
                    //         memset(output_data, 0, sizeof(gui_rgb_data_head_t));
                    //         buffer = output_data + sizeof(gui_rgb_data_head_t);
                    //         gui_rgb_data_head_t *head = (void *)output_data;
                    //         head->type = RGB565;
                    //         head->w = image_width;
                    //         head->h = image_height;
                    //     }
                    //     break;
                    // default:
                    //     break;
                    // }
                    output_data = target_buffer;
                    buffer = output_data + sizeof(gui_rgb_data_head_t);
                    memset(output_data, 0, sizeof(gui_rgb_data_head_t));
                    gui_rgb_data_head_t *head = (void *)output_data;
                    head->type = ARGB8888;
                    head->w = image_width;
                    head->h = image_height;
                    {
                        NVGcontext *vg = 0;
                        extern NVGcontext *nvgCreateAGGE(uint32_t w,
                                                         uint32_t h,
                                                         uint32_t stride,
                                                         enum     NVGtexture format,
                                                         uint8_t *data);
                        extern void nvgDeleteAGGE(NVGcontext * ctx);
                        // vg = nvgCreateAGGE(image_width, image_height, image_width * (pixel_length),
                        //                    (pixel_length) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, buffer);
                        vg = nvgCreateAGGE(image_width, image_height, image_width * (pixel_length),
                                           NVG_TEXTURE_BGRA, buffer);
                        nvgBeginFrame(vg, image_width, image_height, 1);

                        nvgResetTransform(vg);
                        float a0 = 0,  a1 = 0;
                        a0 = param->sd;
                        if (param->dir == NVG_CW)
                        {
                            a1 = (param->ed - param->sd) * per + param->sd;
                        }
                        else if (param->dir == NVG_CCW)
                        {
                            a1 = ((2 * M_PI - (param->ed - param->sd)) * (1 - per) + param->ed);
                            if (per == 0)
                            {
                                a1 = a0;
                            }
                        }


                        if (a0 != a1)
                        {
                            nvgArc(vg, param->r + param->stroke_width, param->r + param->stroke_width, param->r, a0, a1,
                                   param->dir);
                            nvgStrokeWidth(vg, param->stroke_width);
                            nvgStrokeColor(vg, nvgRGBA(param->stroke.color.rgba.r, param->stroke.color.rgba.g,
                                                       param->stroke.color.rgba.b, param->stroke.color.rgba.a));
                            nvgStroke(vg);
                        }



                        nvgEndFrame(vg);
                        nvgDeleteAGGE(vg);
                    }
                }
            }
        }
        break;

    default:
        break;
    }
}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(chart_animate_heartrate_data_callback)
{
    if (!animate->Beginning_frame)
    {
        return;
    }
    chart_animation_param_t *param = p;
    switch (param->aniamtion_type)
    {
    case CHART_HEART_RATE_DATA_ANIMATION:
        {
            const char *path =
                gui_get_path_by_relative("app/system/peripheral_simulation/json/simulation_data.json");
            int fd = gui_fs_open(path, 0);
            if (fd < 1)
            {
                return;
            }
            gui_free((void *)path);
            int size = gui_fs_lseek(fd, 0, SEEK_END);
            gui_fs_lseek(fd, 0, SEEK_SET);
            char *json_string = gui_malloc(size + 1);
            gui_fs_read(fd, json_string, size);
            gui_fs_close(fd);
            json_string[size] = '\0';
            int array_length = 0;
            //gui_log("%s\n",json_string);
            float *array = gui_get_json_array(json_string, "heart_rate", "array", &array_length);
            int max = 0, min = 0;
            gui_get_json_value(json_string, "heart_rate", "range_min", &min);
            gui_get_json_value(json_string, "heart_rate", "range_max", &max);
            if (max != 0)
            {
                param->max = max;
            }
            if (min != 0)
            {
                param->min = min;
            }
            gui_free(json_string);
            if (array_length == 0 || array == 0)
            {
                return;
            }

            int w = param->image_width;
            int h = param->image_height;


            uint8_t *buffer = param->target_buffer;
            memset(buffer, 0, param->image_data_length);
            NVGcontext *vg = gui_canvas_output_buffer_blank(GUI_CANVAS_OUTPUT_RGBA, 0, w, h, buffer);
            if (param->chart_type == 1)
            {
                gui_wave_render(vg, 0, 0, w,
                                h,
                                array_length,
                                array,
                                param->color,
                                param->max,
                                param->min);
            }
            else if (param->chart_type == 2)
            {
                gui_bar_render(vg, 0, 0, w,
                               h,
                               array_length,
                               array,
                               param->color,
                               param->max,
                               param->min, param->stroke_width);
            }
            else if (param->chart_type == 3)
            {
                gui_line_render(vg, 0, 0, w,
                                h,
                                array_length,
                                array,
                                param->color,
                                param->max,
                                param->min, param->stroke_width);
            }
            else
            {
                gui_wave_render(vg, 0, 0, w,
                                h,
                                array_length,
                                array,
                                param->color,
                                param->max,
                                param->min);
            }
            gui_free(array);
            gui_canvas_output_buffer_blank_close(vg);
            gui_img_set_image_data(this_widget, buffer);
        }
    }

}
static void get_2_int_from_string(const char *input, int *int1, int *int2)
{
    char *token;
    int numbers[2];
    numbers[0] = 0;
    numbers[1] = 0;
    int index = 0;
    char *buffer = gui_strdup(input);
    token = strtok(buffer, ",");
    while (token != NULL && index < 2)
    {
        numbers[index] = atoi(token);
        index++;
        token = strtok(NULL, ",");
    }


    *int1 = numbers[0];
    *int2 = numbers[1];
    gui_free(buffer);
}
static bool ends_with_xml(const char *str)
{
    if (str == NULL)
    {
        return false; // Handle null pointer input gracefully
    }

    // Define the suffix we are looking for
    const char *suffix = ".xml";
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    // If the string is shorter than the suffix, it cannot end with the suffix
    if (str_len < suffix_len)
    {
        return false;
    }

    // Compare the end of the string with the suffix
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}
static void app_xml_ui_design(gui_app_t *app)
{
    extern void create_tree(gui_app_t *app);
    create_tree(app);
}
static gui_app_t app_xml =
{
    .screen =
    {
        .name = "app_xml",
        .x    = 0,
        .y    = 0,
    },
    /*
    * Default no sleep, no active time limitation.
    * Please set active time by api(gui_set_app_active_time) if power control is needed.
    */
    .active_ms = (uint32_t) - 1,
    .ui_design = app_xml_ui_design,
};

void *get_app_xml(void)
{
    return &app_xml;
}
extern void gui_img_destory(gui_obj_t *obj);
extern void gui_img_input_prepare(gui_obj_t *obj);
extern void gui_img_prepare(gui_obj_t *obj);
extern void gui_img_draw_cb(gui_obj_t *obj);
extern void gui_img_end(gui_obj_t *obj);
static void canvas_img_destory(gui_obj_t *obj)
{
    gui_img_destory(obj);
    gui_free(GUI_TYPE(gui_img_t, obj)->data);
}

static void gui_canvas_img_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{

    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                gui_img_input_prepare(obj);
            }
            break;
        case OBJ_PREPARE:
            {
                gui_img_prepare(obj);
            }
            break;

        case OBJ_DRAW:
            {
                gui_img_draw_cb(obj);
            }
            break;

        case OBJ_END:
            {
                gui_img_end(obj);
            }
            break;

        case OBJ_DESTORY:
            {
                canvas_img_destory(obj);
            }
            break;

        default:
            break;
        }
    }
}
static void img_ontime_destory(gui_obj_t *obj)
{
    gui_img_t *img = GUI_TYPE(gui_img_t, obj);
    for (size_t i = 0; i < img->animate_array_length; i++)
    {
        gui_animate_t *animate = ((gui_animate_t **)(img->animate))[i];
        gui_free(animate->p);
    }
    gui_img_destory(obj);
}
static void img_ontime_render(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{

    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                gui_img_input_prepare(obj);
            }
            break;
        case OBJ_PREPARE:
            {
                gui_img_prepare(obj);
            }
            break;

        case OBJ_DRAW:
            {
                gui_img_draw_cb(obj);
            }
            break;

        case OBJ_END:
            {
                gui_img_end(obj);
            }
            break;

        case OBJ_DESTORY:
            {
                img_ontime_destory(obj);
            }
            break;

        default:
            break;
        }
    }
}

static void img_destory(gui_obj_t *obj)
{
    gui_free((void *)obj->name);
    gui_img_destory(obj);
}
static void img_render(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{

    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                gui_img_input_prepare(obj);
            }
            break;
        case OBJ_PREPARE:
            {
                gui_img_prepare(obj);
            }
            break;

        case OBJ_DRAW:
            {
                gui_img_draw_cb(obj);
            }
            break;

        case OBJ_END:
            {
                gui_img_end(obj);
            }
            break;

        case OBJ_DESTORY:
            {
                img_destory(obj);
            }
            break;

        default:
            break;
        }
    }
}
static void gui_button_destory(gui_obj_t *obj)
{
    gui_button_t *button = GUI_TYPE(gui_button_t, obj);
    if (button && button->text && button->text->content)
    {
        gui_free(button->text->content);
    }
    if (obj)
    {
        gui_free((void *)obj->name);
    }



}
extern void gui_button_prepare(gui_obj_t *obj);
extern void gui_button_input_prepare(gui_obj_t *obj);
static void button_render(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_button_input_prepare(obj);
            break;
        case OBJ_PREPARE:
            gui_button_prepare(obj);
            break;
        case OBJ_DESTORY:
            {
                gui_button_destory(obj);
            }
            break;
        default:
            break;
        }
    }
}

