/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <math.h>
#include <time.h>
#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "cJSON.h"
#include "gui_canvas.h"
#include "guidef.h"
#include "gui_view.h"
#include "app_main_watch.h"
#include "gui_list.h"
#include "tp_algo.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#ifdef M_PI
#undef M_PI
#define M_PI 3.1415926f
#else
#define M_PI 3.1415926f
#endif

#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "weather_view"

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef struct
{
    char *weekday;    // Weekday name
    uint8_t condition_index;    // Index of weather condition
    char *low_temp;     // Low temperature
    char *high_temp;    // High temperature
} weekly_weather_icon_t;

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void weather_design(gui_view_t *view);
static void clear_weather(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
// const static gui_view_descriptor_t *menu_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = weather_design,
    .on_switch_out = clear_weather,
};


static gui_list_t *weather_list = NULL;

static char text_time_content[10];
static gui_text_t *text_time;
static char text_city_content[10];
static char text_temp_content[5];
static char text_condition_content[10];
static char text_temp_range_content[20];
static char text_degree_symbol[5];

static uint8_t *img_UVI_data = NULL;
static uint8_t UVI_value;
static char text_UVI_content1[5];
static char text_UVI_content2[5];

static uint8_t *img_AQI_data = NULL;
static uint8_t AQI_value;
static char text_AQI_content1[5];
static char text_AQI_content2[5];

static uint8_t MPH_value;
static char text_MPH_content1[5];
static char text_MPH_content2[5];

static void *weather_condition[4];

static uint16_t dial_weather_position[12][2] =
{
    {251, 127},
    {294, 169},
    {311, 234},
    {294, 300},
    {251, 345},
    {184, 353},
    {115, 338},
    {82, 297},
    {63, 234},
    {81, 169},
    {120, 127},
    {184, 112}
};
static uint8_t dial_weather_condition[12] =
{
    1, 0, 0, 3, 3, 1, 1, 2, 2, 0, 0, 1
};

static weekly_weather_icon_t weekly_weather_icon[7] =
{
    {"Today", 0, "57°", "88°"},
    {"Thu", 1, "60°", "75°"},
    {"Fri", 0, "51°", "65°"},
    {"Sat", 0, "64°", "90°"},
    {"Sun", 0, "68°", "91°"},
    {"Mon", 3, "55°", "65°"},
    {"Tue", 3, "52°", "66°"}
};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    // menu_view = gui_view_descriptor_get("menu_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void clear_weather(gui_view_t *view)
{
    GUI_UNUSED(view);
    if (img_UVI_data)
    {
        gui_lower_free(img_UVI_data);
        img_UVI_data = NULL;
    }
    if (img_AQI_data)
    {
        gui_lower_free(img_AQI_data);
        img_AQI_data = NULL;
    }
}

static void weather_data_init(void)
{
    sprintf(text_time_content, "14:21");

    sprintf(text_city_content, "Sonoma");
    sprintf(text_temp_content, "76");
    sprintf(text_degree_symbol, "°");
    sprintf(text_condition_content, "Sunny");
    sprintf(text_temp_range_content, "H:88° L:57°");

    UVI_value = 4;
    MPH_value = 7;
    AQI_value = 32;
    sprintf(text_UVI_content1, "%d", UVI_value);
    sprintf(text_UVI_content2, "UVI");
    sprintf(text_MPH_content1, "%d", MPH_value);
    sprintf(text_MPH_content2, "MPH");
    sprintf(text_AQI_content1, "%d", AQI_value);
    sprintf(text_AQI_content2, "AQI");
}

static void weather_condition_init(void)
{
    weather_condition[0] = WEATHER_CONDITION_SUNNY_BIN;
    weather_condition[1] = WEATHER_CONDITION_PARTLY_CLOUDY_BIN;
    weather_condition[2] = WEATHER_CONDITION_CLOUDY_BIN;
    weather_condition[3] = WEATHER_CONDITION_WINDY_BIN;
}

static void weather_time_update_cb(void *p)
{
    GUI_UNUSED(p);
    time_t rawtime;
    struct tm timeinfo;

#ifdef _HONEYGUI_SIMULATOR_
    struct tm *tmp;
    time(&rawtime);
    tmp = localtime(&rawtime);
    if (tmp != NULL)
    {
        timeinfo = *tmp;
    }
#else
    time(&rawtime);
    localtime_r(&rawtime, &timeinfo);
#endif

    sprintf(text_time_content, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);

    gui_text_set(text_time, (void *)text_time_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_time_content), 35);
    gui_text_type_set(text_time, SF_COMPACT_TEXT_SEMIBOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_time, CENTER);
    gui_text_rendermode_set(text_time, 2);
}

// Note 1
static void arc_UVI_cb(NVGcontext *vg)
{
    float center_x = 42, center_y = 42, radius = 34;
    float start_angle = M_PI * 5 / 6; // 150°
    float end_angle = M_PI * 1 / 6;  // 30°
    float total_angle_range = 2 * M_PI - (start_angle - end_angle);

    nvgBeginPath(vg);
    nvgArc(vg, center_x, center_y, radius, start_angle, end_angle, NVG_CW);
    nvgStrokeWidth(vg, center_x - radius);
    nvgStrokeColor(vg, nvgRGBA(255, 255, 255, 178));
    nvgStroke(vg);

    float current_angle = start_angle + (UVI_value / 10.0f) * total_angle_range;
    if (current_angle < 0)
    {
        current_angle += 2 * M_PI;
    }
    float dot_x = center_x + cosf(current_angle) * radius;
    float dot_y = center_y + sinf(current_angle) * radius;
    nvgBeginPath(vg);
    nvgCircle(vg, dot_x, dot_y, 5.0f);
    nvgFillColor(vg, nvgRGB(255, 255, 255));
    nvgFill(vg);
}

static void arc_AQI_cb(NVGcontext *vg)
{
    float center_x = 42, center_y = 42, radius = 34;
    float start_angle = M_PI * 5 / 6; // 150°
    float end_angle = M_PI * 1 / 6;  // 30°
    float total_angle_range = 2 * M_PI - (start_angle - end_angle);

    nvgBeginPath(vg);
    nvgArc(vg, center_x, center_y, radius, start_angle, end_angle, NVG_CW);
    nvgStrokeWidth(vg, center_x - radius);
    nvgStrokeColor(vg, nvgRGBA(255, 255, 255, 178));
    nvgStroke(vg);

    float current_angle = start_angle + (AQI_value / 60.0f) * total_angle_range;
    if (current_angle < 0)
    {
        current_angle += 2 * M_PI;
    }
    float dot_x = center_x + cosf(current_angle) * radius;
    float dot_y = center_y + sinf(current_angle) * radius;
    nvgBeginPath(vg);
    nvgCircle(vg, dot_x, dot_y, 5.0f);
    nvgFillColor(vg, nvgRGB(255, 255, 255));
    nvgFill(vg);
}

static void note1_design(gui_list_note_t *note)
{
    gui_img_t *img_bg1 = gui_img_create_from_mem(note, "bg1", WEATHER_BACKGROUND1_BIN, 0, 0,
                                                 SCREEN_WIDTH, SCREEN_HEIGHT);

    // City Text
    gui_text_t *text_city = gui_text_create(img_bg1, "text_city", 0, 130, 0, 0);
    gui_text_set(text_city, (void *)text_city_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_city_content), 43);
    gui_text_type_set(text_city, SF_COMPACT_TEXT_SEMIBOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_city, CENTER);
    gui_text_rendermode_set(text_city, 2);

    // Temperature Text
    gui_text_t *text_temp = gui_text_create(img_bg1, "text_temp", 0, 170, 0, 0);
    gui_text_set(text_temp, (void *)text_temp_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_temp_content), 103);
    gui_text_type_set(text_temp, SF_COMPACT_TEXT_LIGHT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_temp, CENTER);
    gui_text_rendermode_set(text_temp, 2);

    gui_text_t *text_temp_degree = gui_text_create(img_bg1, "text_temp_degree", 255, 170, 0, 0);
    gui_text_set(text_temp_degree, (void *)text_degree_symbol, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_degree_symbol), 83);
    gui_text_type_set(text_temp_degree, SF_COMPACT_TEXT_LIGHT_BIN, FONT_SRC_MEMADDR);
    gui_text_rendermode_set(text_temp_degree, 2);

    // Condition Text
    gui_text_t *text_condition = gui_text_create(img_bg1, "text_condition", 0, 270, 0, 0);
    gui_text_set(text_condition, (void *)text_condition_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_condition_content), 33);
    gui_text_type_set(text_condition, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_condition, CENTER);
    gui_text_rendermode_set(text_condition, 2);

    // Temperature Range Text
    gui_text_t *text_temp_range = gui_text_create(img_bg1, "text_temp_range", 0, 310, 0, 0);
    gui_text_set(text_temp_range, (void *)text_temp_range_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_temp_range_content), 33);
    gui_text_type_set(text_temp_range, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_temp_range, CENTER);
    gui_text_rendermode_set(text_temp_range, 2);

    // UVI
    uint16_t image_w = 85;
    uint16_t image_h = 85;

    size_t buffer_size = image_h * image_w * 4 + sizeof(gui_rgb_data_head_t);
    if (img_UVI_data == NULL)
    {
        img_UVI_data = gui_lower_malloc(buffer_size);
    }
    memset(img_UVI_data, 0, buffer_size);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, image_w, image_h,
                                      arc_UVI_cb, img_UVI_data);
    gui_img_t *img_UVI = gui_img_create_from_mem(img_bg1, "img_UVI", (void *)img_UVI_data,
                                                 13, 395, 0, 0);
    gui_img_set_mode(img_UVI, IMG_SRC_OVER_MODE);

    gui_text_t *text_UVI_value = gui_text_create(img_UVI, "text_UVI_value", 0, 22, 0, 0);
    gui_text_set(text_UVI_value, (void *)text_UVI_content1, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_UVI_content1), 31);
    gui_text_type_set(text_UVI_value, SF_COMPACT_TEXT_SEMIBOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_UVI_value, CENTER);
    gui_text_rendermode_set(text_UVI_value, 2);

    gui_text_t *text_UVI = gui_text_create(img_UVI, "text_UVI", 0, 50, 0, 0);
    gui_text_set(text_UVI, (void *)text_UVI_content2, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_UVI_content2), 16);
    gui_text_type_set(text_UVI, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_UVI, CENTER);
    gui_text_rendermode_set(text_UVI, 2);

    // MPH
    gui_img_t *img_wind_arrow = gui_img_create_from_mem(img_bg1, "img_wind_arrow", WIND_ARROW_BIN,
                                                        0, 0, 0, 0);
    gui_img_set_focus(img_wind_arrow, 39, 17);
    gui_img_translate(img_wind_arrow, 207, 437);
    gui_img_t *img_wind_blur = gui_img_create_from_mem(img_bg1, "img_wind_blur", WIND_BLUR_BIN,
                                                       185, 418, 0, 0);

    gui_text_t *text_MPH_value = gui_text_create(img_wind_blur, "text_MPH_value", 0, 2, 0, 0);

    gui_text_set(text_MPH_value, (void *)text_MPH_content1, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_MPH_content1), 20);
    gui_text_type_set(text_MPH_value, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_MPH_value, CENTER);
    gui_text_rendermode_set(text_MPH_value, 2);

    gui_text_t *text_MPH = gui_text_create(img_wind_blur, "text_AQI", 0, 23, 0, 0);
    gui_text_set(text_MPH, (void *)text_MPH_content2, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_MPH_content2), 15);
    gui_text_type_set(text_MPH, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_MPH, CENTER);
    gui_text_rendermode_set(text_MPH, 2);

    // AQI
    if (img_AQI_data == NULL)
    {
        img_AQI_data = gui_lower_malloc(buffer_size);
    }
    memset(img_AQI_data, 0, buffer_size);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, image_w, image_h,
                                      arc_AQI_cb, img_AQI_data);
    gui_img_t *img_AQI = gui_img_create_from_mem(img_bg1, "img_AQI", (void *)img_AQI_data,
                                                 314, 395, 0, 0);
    gui_img_set_mode(img_AQI, IMG_SRC_OVER_MODE);

    gui_text_t *text_AQI_value = gui_text_create(img_AQI, "text_AQI_value", 0, 22, 0, 0);
    gui_text_set(text_AQI_value, (void *)text_AQI_content1, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_AQI_content1), 31);
    gui_text_type_set(text_AQI_value, SF_COMPACT_TEXT_SEMIBOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_AQI_value, CENTER);
    gui_text_rendermode_set(text_AQI_value, 2);

    gui_text_t *text_AQI = gui_text_create(img_AQI, "text_AQI", 0, 50, 0, 0);
    gui_text_set(text_AQI, (void *)text_AQI_content2, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_AQI_content2), 16);
    gui_text_type_set(text_AQI, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_AQI, CENTER);
    gui_text_rendermode_set(text_AQI, 2);

}

// Note 2
static void note2_design(gui_list_note_t *note)
{
    gui_img_t *img_bg2 = gui_img_create_from_mem(note, "bg2", WEATHER_BACKGROUND2_BIN, 0, 0,
                                                 SCREEN_WIDTH, SCREEN_HEIGHT);

    gui_img_t *img_dial = gui_img_create_from_mem(img_bg2, "img_dial", WEATHER_DIAL_BIN,
                                                  21, 72, 0, 0);
    gui_img_set_mode(img_dial, IMG_SRC_OVER_MODE);

    // Weather Condition
    for (int i = 0; i < 12; i++)
    {
        uint8_t condition_index = dial_weather_condition[i];
        gui_img_t *img_weather = gui_img_create_from_mem(img_bg2, "img_weather",
                                                         weather_condition[condition_index],
                                                         dial_weather_position[i][0], dial_weather_position[i][1], 0, 0);
        gui_img_set_mode(img_weather, IMG_SRC_OVER_MODE);
    }

    // Temperature Text
    gui_text_t *text_temp = gui_text_create(img_bg2, "text_temp", 0, 190, 0, 0);
    gui_text_set(text_temp, (void *)text_temp_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_temp_content), 83);
    gui_text_type_set(text_temp, SF_COMPACT_TEXT_LIGHT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_temp, CENTER);
    gui_text_rendermode_set(text_temp, 2);

    gui_text_t *text_temp_degree = gui_text_create(img_bg2, "text_temp_degree", 245, 190, 0, 0);
    gui_text_set(text_temp_degree, (void *)text_degree_symbol, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_degree_symbol), 83);
    gui_text_type_set(text_temp_degree, SF_COMPACT_TEXT_LIGHT_BIN, FONT_SRC_MEMADDR);
    gui_text_rendermode_set(text_temp_degree, 2);

    // Temperature Range Text
    gui_text_t *text_temp_range = gui_text_create(img_bg2, "text_temp_range", 0, 270, 0, 0);
    gui_text_set(text_temp_range, (void *)text_temp_range_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_temp_range_content), 23);
    gui_text_type_set(text_temp_range, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_temp_range, CENTER);
    gui_text_rendermode_set(text_temp_range, 2);

    // Condition Text
    gui_text_t *text_condition = gui_text_create(img_bg2, "text_condition", 0, 440, 0, 0);
    gui_text_set(text_condition, (void *)text_condition_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_condition_content), 29);
    gui_text_type_set(text_condition, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_condition, CENTER);
    gui_text_rendermode_set(text_condition, 2);
}

// Note 3
static void slide_win_cb(void *p)
{
    gui_win_t *win = (gui_win_t *)p;
    touch_info_t *tp = tp_get_info();

    if (weather_list->offset <= -1004)
    {
        if ((win->base.y + tp->deltaY) > -10 && (win->base.y + tp->deltaY) < 160)
        {
            win->base.y += tp->deltaY;
        }
    }

}
static void note3_design(gui_list_note_t *note)
{
    gui_img_create_from_mem(note, "bg3", WEATHER_BACKGROUND3_BIN, 0, 0,
                            SCREEN_WIDTH, SCREEN_HEIGHT);

    gui_win_t *win_list = gui_win_create(note, "win_list", 0, 160, SCREEN_WIDTH, SCREEN_HEIGHT);

    for (uint8_t index = 0; index < 7; index++)
    {
        gui_win_t *win_list_item = gui_win_create((void *)win_list, "win_list_item", 0, index * 76,
                                                  SCREEN_WIDTH, 76);

        gui_text_t *text_weekday = gui_text_create((void *)win_list_item, "text_weekday", 25, 0, 0, 0);
        gui_text_set(text_weekday, (void *)weekly_weather_icon[index].weekday, GUI_FONT_SRC_TTF,
                     APP_COLOR_WHITE,
                     strlen(weekly_weather_icon[index].weekday), 31);
        gui_text_type_set(text_weekday, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(text_weekday, 2);

        gui_img_t *img_weather = gui_img_create_from_mem(win_list_item, "img_weather",
                                                         weather_condition[weekly_weather_icon[index].condition_index],
                                                         185, 0, 0, 0);
        gui_img_set_mode(img_weather, IMG_SRC_OVER_MODE);

        gui_text_t *text_low_temp = gui_text_create(win_list_item, "text_low_temp", 300, 0, 0, 0);
        gui_text_set(text_low_temp, (void *)weekly_weather_icon[index].low_temp, GUI_FONT_SRC_TTF,
                     gui_rgba(142, 185, 241, 255),
                     strlen(weekly_weather_icon[index].low_temp), 31);
        gui_text_type_set(text_low_temp, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(text_low_temp, 2);

        gui_text_t *text_high_temp = gui_text_create(win_list_item, "text_high_temp", 350, 0, 0, 0);
        gui_text_set(text_high_temp, (void *)weekly_weather_icon[index].high_temp, GUI_FONT_SRC_TTF,
                     APP_COLOR_WHITE,
                     strlen(weekly_weather_icon[index].high_temp), 31);
        gui_text_type_set(text_high_temp, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(text_high_temp, 2);

        gui_img_create_from_mem(win_list_item, "img_separator", WEATHER_SEPARATOR_BIN, 25, 45, 0, 0);

    }

    gui_obj_create_timer(GUI_BASE(win_list), 16, true, slide_win_cb);

}

static void note_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    gui_list_note_t *note = (gui_list_note_t *)obj;
    uint16_t index = note->index;
    gui_log("index: %d\n", index);
    if (index == 0)
    {
        note1_design(note);
    }
    else if (index == 1)
    {
        note2_design(note);
    }
    else if (index == 2)
    {
        note3_design(note);
    }
    return;
}

static void switch_app_cb(void *obj)
{
    const char *obj_name = ((gui_obj_t *)obj)->name;
    if (strcmp(obj_name, "BIG_NUM") == 0)
    {
        current_watchface_type = WATCHFACE_BIG_NUM;

        gui_view_switch_direct(current_view, "watchface_view",
                               SWITCH_OUT_ANIMATION_ZOOM,
                               SWITCH_IN_ANIMATION_ZOOM);
    }
    else if (strcmp(obj_name, "SPORT") == 0)
    {
        current_watchface_type = WATCHFACE_SPORT;

        gui_view_switch_direct(current_view, "watchface_view",
                               SWITCH_OUT_ANIMATION_ZOOM,
                               SWITCH_IN_ANIMATION_ZOOM);
    }

}
static void weather_design(gui_view_t *view)
{
    const char *obj_name = gui_view_get_current()->descriptor->name;
    VIEW_SWITCH_STYLE swtich_in = SWITCH_IN_ANIMATION_FADE;
    VIEW_SWITCH_STYLE swtich_out = SWITCH_OUT_ANIMATION_FADE;
    if (strcmp(obj_name, "bottom_view") == 0)
    {
        swtich_in = SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION;
        swtich_out = SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION;
    }
    if (strcmp(obj_name, "menu_view") == 0)
    {
        gui_view_switch_on_event(current_view, "menu_view",
                                 swtich_out,
                                 swtich_in,
                                 GUI_EVENT_KB_SHORT_CLICKED);
    }
    else
    {
        gui_view_switch_on_event(view, "watchface_view",
                                 swtich_out,
                                 swtich_in,
                                 GUI_EVENT_KB_SHORT_CLICKED);
    }
    weather_data_init();
    weather_condition_init();
    weather_list = gui_list_create(view, "weather_list", 0, 0, 0, 0, 502, 0, VERTICAL,
                                   note_design, NULL, false);
    gui_list_set_style(weather_list, LIST_FADE);
    gui_list_set_note_num(weather_list, 3);
    gui_list_set_auto_align(weather_list, true);
    gui_list_set_inertia(weather_list, false);
    gui_list_set_out_scope(weather_list, 0);

    // Common Top Window
    gui_win_t *win_weather_up = gui_win_create((void *)view, "win_weather_up", 0, 0, SCREEN_WIDTH,
                                               SCREEN_HEIGHT);

    text_time = gui_text_create(win_weather_up, "text_time", 0, 14, 0, 0);
    gui_text_set(text_time, (void *)text_time_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(text_time_content), 35);
    gui_text_type_set(text_time, SF_COMPACT_TEXT_SEMIBOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_time, CENTER);
    gui_text_rendermode_set(text_time, 2);

    gui_img_t *img_icon_left = gui_img_create_from_mem(win_weather_up, "img_icon_left",
                                                       WEATHER_LEFT_CONTROL_BIN, 13,
                                                       14, 0, 0);
    gui_img_set_mode(img_icon_left, IMG_SRC_OVER_MODE);
    gui_img_t *img_icon_right = gui_img_create_from_mem(win_weather_up, "img_icon_right",
                                                        WEATHER_RIGHT_CONTROL_BIN, 322,
                                                        14, 0, 0);
    gui_img_set_mode(img_icon_right, IMG_SRC_OVER_MODE);

    weather_time_update_cb(NULL);


    return;
}
