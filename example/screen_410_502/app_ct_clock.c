/* watchface_watch_gradient_spot demo start*/
#include "root_image_hongkong/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_watch_gradient_spot.h"
#include "gui_text.h"
#include "gui_watchface_gradient.h"
#include "gui_cardview.h"
#include "gui_card.h"
#include "gui_tab.h"
#include "gui_app.h"
#include "time.h"
#include "gui_curtainview.h"
#include "tp_algo.h"
#include <math.h>
#include "cJSON.h"
#include "gui_canvas_img.h"
#include "gui_curtain.h"
#include "gui_canvas_rect.h"
#include "guidef.h"
#include "app_hongkong.h"

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502
#define COLOR_RED gui_rgb(255,0,0)
#define COLOR_SILVER gui_rgb(192,192,192)
#define COLOR_SILVER_OPACITY(opacity) gui_rgba(192,192,192, opacity)

const static gui_view_descriptor_t *heartrate_view = NULL;
static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    heartrate_view = gui_view_descriptor_get("heartrate_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

// static void *SOURCEHANSANSSC_BIN = SOURCEHANSANSSC_SIZE48_BITS1_FONT_BIN;
// static void *SOURCEHANSANSSC_BIN = SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN;
// static void *SOURCEHANSANSSC_BIN = SOURCEHANSANSSC_SIZE24_BITS1_FONT_BIN;

gui_win_t *win_watch;
static gui_text_t *date_text;
static char date_text_content[10];
static char weekday_content[30];
extern struct tm *timeinfo;

bool return_to_watchface_flag; //true: return to watchface; false: return to app_menu

extern uint8_t canvas_update_flag;
static gui_text_t *temperature_cur, *temperature_low, *temperature_high;
static gui_text_t *weather_cur, *weather_range;
static bool weather_syn_flag = false;
static char content_cur[5];
static char content_range[15];
static char tempera_cur_content[3];
static char tempera_low_content[3];
static char tempera_high_content[3];

static gui_img_t *compass_pointer;
static gui_text_t *compass_degree, *compass_orien;
static char degree_content[5] = "0°", orien_content[3] = "N";

static gui_img_t *img_heart_rate;

static uint8_t *img_data_temperature;
static uint8_t *img_data_activity;

extern char *cjson_content;

/*Define watch_text_num_array*/
void *text_num_array[] =
{
    UI_TEXT_0_BIN,
    UI_TEXT_1_BIN,
    UI_TEXT_2_BIN,
    UI_TEXT_3_BIN,
    UI_TEXT_4_BIN,
    UI_TEXT_5_BIN,
    UI_TEXT_6_BIN,
    UI_TEXT_7_BIN,
    UI_TEXT_8_BIN,
    UI_TEXT_9_BIN,
    UI_TEXT_COLON_BIN,
};
extern char *day[];
static void refreash_time()
{
    if (!timeinfo)
    {
        return;
    }
    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_hour_decimal, "watch_hour_decimal", win_watch);
    gui_img_set_image_data((gui_img_t *)img_hour_decimal, text_num_array[timeinfo->tm_hour / 10]);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_hour_single, "watch_hour_single", win_watch);
    gui_img_set_image_data((gui_img_t *)img_hour_single, text_num_array[timeinfo->tm_hour % 10]);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_minute_decimal, "watch_minute_decimal", win_watch);
    gui_img_set_image_data((gui_img_t *)img_minute_decimal, text_num_array[timeinfo->tm_min / 10]);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_minute_single, "watch_minute_single", win_watch);
    gui_img_set_image_data((gui_img_t *)img_minute_single, text_num_array[timeinfo->tm_min % 10]);

    sprintf(date_text_content, "%s %d",  day[timeinfo->tm_wday], timeinfo->tm_mday);
    gui_text_content_set(date_text, date_text_content, strlen(date_text_content));
    // gui_text_convert_to_img(date_text, RGB565);

    // refreash weather date
    GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "weather_text", win_watch)
    uint8_t index = timeinfo->tm_wday;

    sprintf(weekday_content, "%s.  %s.  %s.  %s.", day[(index + 1) % 7], day[(index + 2) % 7],
            day[(index + 3) % 7],
            day[(index + 4) % 7]);
    // gui_log("%s\r\n", weekday_content);
    gui_text_content_set((gui_text_t *)obj, weekday_content, strlen(weekday_content));
}

static void win_clock_cb(gui_win_t *win)
{
    // bool slide_card_enable;
    if (win->animate->Beginning_frame)
    {
        refreash_time();
    }
}

void clear_clock(void)
{
    if (img_data_temperature)
    {
        gui_lower_free(img_data_temperature);
        img_data_temperature = NULL;
    }
    if (img_data_activity)
    {
        gui_lower_free(img_data_activity);
        img_data_activity = NULL;
    }
}

static void arc_activity_cb(NVGcontext *vg)
{
    cJSON *root;
    if (!cjson_content)
    {
        // automation
        // static float progress_activity = 0;
        // progress_activity += 0.001;
        // if (progress_activity >= 2)
        // {
        //     progress_activity = 0;
        // }
        // nvgBeginPath(canvas->vg);
        // nvgArc(canvas->vg, 16 + 100 / 2, 100 / 2, 50 - 8, 3 * M_PI / 2, M_PI * (progress_activity + 3 / 2),
        //     NVG_CW);
        // nvgStrokeWidth(canvas->vg, 8);
        // nvgStrokeColor(canvas->vg, nvgRGB(230, 67, 79));
        // nvgStroke(canvas->vg);

        // nvgBeginPath(canvas->vg);
        // nvgArc(canvas->vg, 16 + 100 / 2, 100 / 2, 50 - 21, 3 * M_PI / 2, M_PI * (progress_activity + 3 / 2),
        //     NVG_CW);
        // nvgStrokeWidth(canvas->vg, 8);
        // nvgStrokeColor(canvas->vg, nvgRGB(186, 253, 79));
        // nvgStroke(canvas->vg);

        // nvgBeginPath(canvas->vg);
        // nvgArc(canvas->vg, 16 + 100 / 2, 100 / 2, 50 - 33, 3 * M_PI / 2, M_PI * (progress_activity + 3 / 2),
        //     NVG_CW);
        // nvgStrokeWidth(canvas->vg, 8);
        // nvgStrokeColor(canvas->vg, nvgRGB(117, 230, 229));
        // nvgStroke(canvas->vg);
        return;
    }
    else
    {
        root = cJSON_Parse(cjson_content);
        if (!root)
        {
            gui_log("Error parsing JSON!\r\n");
            return;
        }
    }
    // parse activity array
    cJSON *activity_array = cJSON_GetObjectItemCaseSensitive(root, "activity");
    if (cJSON_IsArray(activity_array))
    {
        cJSON *act = cJSON_GetArrayItem(activity_array, 0);
        if (!act)
        {
            gui_log("get activity_array unsuccessful\n");
        }
        else
        {
            cJSON *move = cJSON_GetObjectItemCaseSensitive(act, "move");
            cJSON *ex = cJSON_GetObjectItemCaseSensitive(act, "exercise");
            cJSON *stand = cJSON_GetObjectItemCaseSensitive(act, "stand");

            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 8, 3 * M_PI / 2,
                   M_PI * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(58, 23, 29));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 8, 3 * M_PI / 2,
                   M_PI * (1.5f + 2.0f * move->valueint / 20000.0f), NVG_CW);  // cap 20000 steps
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(230, 67, 79));
            nvgStroke(vg);

            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 21, 3 * M_PI / 2,
                   M_PI * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(30, 55, 25));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 21, 3 * M_PI / 2,
                   M_PI * (1.5f + 2.0f * ex->valueint / 60.0f), NVG_CW);  // cap 60 min.
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(186, 253, 79));
            nvgStroke(vg);

            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 33, 3 * M_PI / 2,
                   M_PI * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(22, 50, 47));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 33, 3 * M_PI / 2,
                   M_PI * (1.5f + 2.0f * stand->valueint / 30.0f), NVG_CW); // cap 30 times
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(117, 230, 229));
            nvgStroke(vg);
        }
    }
    // clear
    cJSON_Delete(root);
}

static void weather_cb(gui_img_t *parent)
{
    cJSON *root;
    if (parent->animate->Beginning_frame)
    {
        if (!cjson_content)
        {
            return;
        }
        else
        {
            root = cJSON_Parse(cjson_content);
            if (!root)
            {
                gui_log("Error parsing JSON!\r\n");
                return;
            }
        }
        weather_syn_flag = true;
        // parse weather array
        cJSON *weather_array = cJSON_GetObjectItemCaseSensitive(root, "weather");
        if (cJSON_IsArray(weather_array))
        {
            cJSON *weather = cJSON_GetArrayItem(weather_array, 0);
            if (!weather)
            {
                gui_log("get weather_array unsuccessful\n");
            }
            else
            {
                cJSON *low = cJSON_GetObjectItemCaseSensitive(weather, "low");
                cJSON *high = cJSON_GetObjectItemCaseSensitive(weather, "high");
                cJSON *cur = cJSON_GetObjectItemCaseSensitive(weather, "current");

                sprintf(content_cur, "%d°", cur->valueint);
                gui_text_content_set(weather_cur, content_cur, strlen(content_cur));
                sprintf(content_range, "H:%d° L:%d°", high->valueint, low->valueint);
                gui_text_content_set(weather_range, content_range, strlen(content_range));
                for (uint8_t i = 1; i <= 5; i++)
                {
                    char key[15];
                    sprintf(key, "condition_%d", i);
                    // gui_log("%s\r\n", key);
                    // GUI_WIDGET_POINTER_BY_NAME(obj, key);
                    GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, key, win_watch);
                    cJSON *condition = cJSON_GetObjectItemCaseSensitive(weather, key);
                    if (strcmp(condition->valuestring, "Sunny") == 0)
                    {
                        gui_img_set_attribute((gui_img_t *)obj, obj->name, UI_WEATHER_SUNNY_BIN, obj->x, obj->y);
                    }
                    else if (strcmp(condition->valuestring, "Light rain") == 0)
                    {
                        gui_img_set_attribute((gui_img_t *)obj, obj->name, UI_WEATHER_RAIN_S_BIN, obj->x, obj->y);
                    }
                    else if (strcmp(condition->valuestring, "Showers") == 0)
                    {
                        gui_img_set_attribute((gui_img_t *)obj, obj->name, UI_WEATHER_RAIN_S_BIN, obj->x, obj->y);
                    }
                    else if (strcmp(condition->valuestring, "Moderate rain") == 0)
                    {
                        gui_img_set_attribute((gui_img_t *)obj, obj->name, UI_WEATHER_RAIN_M_BIN, obj->x, obj->y);
                    }
                    else if (strcmp(condition->valuestring, "Heavy rain") == 0)
                    {
                        gui_img_set_attribute((gui_img_t *)obj, obj->name, UI_WEATHER_RAIN_L_BIN, obj->x, obj->y);
                    }
                    else if (strcmp(condition->valuestring, "Cloudy") == 0)
                    {
                        gui_img_set_attribute((gui_img_t *)obj, obj->name, UI_WEATHER_CLOUDY_BIN, obj->x, obj->y);
                    }
                    else // need to add more weather icon
                    {
                        gui_img_set_attribute((gui_img_t *)obj, obj->name, UI_WEATHER_CLOUDY_BIN, obj->x, obj->y);
                    }
                }
            }
        }
        cJSON_Delete(root);
    }
}

static void arc_temperature_cb(NVGcontext *vg)
{
    cJSON *root;
    if (!cjson_content)
    {
        // automation
        // float x = 100 / 2;
        // float y = 100 / 2;
        // float r = 50 - 8; // radius
        // float r0 = M_PI * 5 / 6, r1 = M_PI * 13 / 6; // 5/6pi ~ 13/6pi CW
        // NVGpaint paint = nvgLinearGradient(vg, x + cosf(r0) * r, y + sinf(r0) * r, x + cosf(r1) * r,
        //                                    y + sinf(r1) * r, nvgRGB(191, 220, 48), nvgRGB(250, 17, 79));
        // nvgMoveTo(vg, x, y);
        // nvgLineTo(vg, x + cosf(r0) * r, y + sinf(r0) * r);
        // nvgArc(vg, x, y, r, r0, r1, NVG_CW);
        // nvgLineTo(vg, x, y);
        // nvgFillPaint(vg, paint);
        // nvgFill(vg);

        // nvgBeginPath(vg);
        // nvgArc(vg, x, y, r - 8.0f, 0, M_PI * 2.0f, NVG_CW);
        // nvgFillColor(vg, nvgRGB(0, 0, 0));
        // nvgFill(vg);

        // r -= 4;
        // nvgBeginPath(vg);
        // nvgArc(vg, x + cosf(r0 + 0.02f) * r, y + sinf(r0 + 0.02f) * r, 4.0f, r0, r0 + M_PI, NVG_CCW);
        // nvgFillColor(vg, nvgRGB(191, 220, 48));
        // nvgFill(vg);

        // nvgBeginPath(vg);
        // nvgArc(vg, x + cosf(r1 - 0.02f) * r, y + sinf(r1 - 0.02f) * r, 4.0f, r1, r1 - M_PI, NVG_CW);
        // nvgFillColor(vg, nvgRGB(250, 17, 79));
        // nvgFill(vg);

        // // moving dot
        // static float a0 = M_PI * 5 / 6; // 5/6pi ~ 13/6pi CW
        // float temp = (a0 - M_PI * 5 / 6) / (M_PI * 4 / 3);
        // float ax, ay;
        // nvgBeginPath(vg);
        // ax = x + cosf(a0) * r;
        // ay = y + sinf(a0) * r;
        // nvgCircle(vg, ax, ay, 6.5f);
        // nvgFillColor(vg, nvgRGB(0, 0, 0));
        // nvgFill(vg);
        // uint8_t dot_r = (int)(temp * (250 - 191) + 191 + 0.5);
        // uint8_t dot_g = (int)(temp * (17 - 220) + 220 + 0.5);
        // uint8_t dot_b = (int)(temp * (79 - 48) + 48 + 0.5);
        // nvgBeginPath(vg);
        // nvgCircle(vg, ax, ay, 5.0f);
        // nvgFillColor(vg, nvgRGB(dot_r, dot_g, dot_b));
        // nvgFill(vg);
        // sprintf(tempera_cur_content, "%d", (int)(temp * (37 - 18) + 18 + 0.5));
        // gui_text_content_set(temperature_cur, (void *)tempera_cur_content, strlen(tempera_cur_content));
        return;
    }
    else
    {
        root = cJSON_Parse(cjson_content);
        if (!root)
        {
            gui_log("Error parsing JSON!\r\n");
            return;
        }
    }
    // parse temperature array
    cJSON *weather_array = cJSON_GetObjectItemCaseSensitive(root, "weather");
    if (cJSON_IsArray(weather_array))
    {
        cJSON *weather = cJSON_GetArrayItem(weather_array, 0);
        if (!weather)
        {
            gui_log("get weather_array unsuccessful\n");
        }
        else
        {
            cJSON *low = cJSON_GetObjectItemCaseSensitive(weather, "low");
            cJSON *high = cJSON_GetObjectItemCaseSensitive(weather, "high");
            cJSON *cur = cJSON_GetObjectItemCaseSensitive(weather, "current");
            uint8_t low_val = low->valueint < 10 ? 10 : low->valueint;
            uint8_t high_val = high->valueint > 37 ? 37 : high->valueint;
            uint8_t cur_val = cur->valueint;

            // draw arc
            float x = 100 / 2; // x coordinate
            float y = 100 / 2; // y coordinate
            float r = 50 - 4; // radius 50 - 8
            // 5/6pi ~ 13/6pi CW -->temperature range: 10 ~ 37
            float r0 = M_PI * 5.0f / 6.0f;
            float r1 = M_PI * 13.0f / 6.0f;

            // nvgArc(vg, x, y, r, r0, r1, NVG_CW);
            // nvgStrokeWidth(vg, 8);
            // start:nvgRGB(191, 220, 48), stop:nvgRGB(250, 17, 79))
            uint8_t start_r = (int)(191 + (250 - 191) * (low_val - 10) / (37 - 10));
            uint8_t start_g = (int)(220 + (17 - 220) * (low_val - 10) / (37 - 10));
            uint8_t start_b = (int)(48 + (79 - 48) * (low_val - 10) / (37 - 10));
            uint8_t stop_r = (int)(191 + (250 - 191) * (high_val - 10) / (37 - 10));
            uint8_t stop_g = (int)(220 + (17 - 220) * (high_val - 10) / (37 - 10));
            uint8_t stop_b = (int)(48 + (79 - 48) * (high_val - 10) / (37 - 10));
            NVGpaint paint = nvgLinearGradient(vg, x + cosf(r0) * r, y + sinf(r0) * r, x + cosf(r1) * r,
                                               y + sinf(r1) * r, nvgRGB(start_r, start_g, start_b), nvgRGB(stop_r, stop_g, stop_b));
            // nvgStrokePaint(vg, paint);
            // nvgStroke(vg);

            // draw circular sector
            nvgMoveTo(vg, x, y);
            nvgLineTo(vg, x + cosf(r0) * r, y + sinf(r0) * r);
            nvgArc(vg, x, y, r, r0, r1, NVG_CW);
            nvgLineTo(vg, x, y);
            nvgFillPaint(vg, paint);
            nvgFill(vg);

            // draw black circle to formate arc
            nvgBeginPath(vg);
            nvgArc(vg, x, y, r - 8.0f, 0, M_PI * 2.0f, NVG_CW);
            nvgFillColor(vg, nvgRGB(0, 0, 0));
            nvgFill(vg);

            // draw edge semi_circle
            r -= 4;
            nvgBeginPath(vg);
            nvgArc(vg, x + cosf(r0 + 0.02f) * r, y + sinf(r0 + 0.02f) * r, 4.0f, r0, r0 + M_PI, NVG_CCW);
            nvgFillColor(vg, nvgRGB(start_r, start_g, start_b));
            nvgFill(vg);

            nvgBeginPath(vg);
            nvgArc(vg, x + cosf(r1 - 0.02f) * r, y + sinf(r1 - 0.02f) * r, 4.0f, r1, r1 - M_PI, NVG_CW);
            nvgFillColor(vg, nvgRGB(stop_r, stop_g, stop_b));
            nvgFill(vg);

            // draw dot
            float temp = ((float)cur_val - (float)low_val) / ((float)high_val - (float)low_val);
            float a0 = M_PI * 5.0f / 6.0f + temp * 4.0f / 3.0f * M_PI; // 5/6pi ~ 13/6pi CW
            float ax, ay;
            nvgBeginPath(vg);
            ax = x + cosf(a0) * r;
            ay = y + sinf(a0) * r;
            nvgCircle(vg, ax, ay, 6.5f);
            nvgFillColor(vg, nvgRGB(0, 0, 0));
            nvgFill(vg);

            uint8_t dot_r = (int)(temp * (stop_r - start_r) + start_r + 0.5f);
            uint8_t dot_g = (int)(temp * (stop_g - start_g) + start_g + 0.5f);
            uint8_t dot_b = (int)(temp * (stop_b - start_b) + start_b + 0.5f);
            nvgBeginPath(vg);
            nvgCircle(vg, ax, ay, 5.0f);
            nvgFillColor(vg, nvgRGB(dot_r, dot_g, dot_b));
            nvgFill(vg);

            if (weather_syn_flag)
            {
                sprintf(tempera_cur_content, "%d", cur_val);
                gui_text_content_set(temperature_cur, tempera_cur_content, strlen(tempera_cur_content));
                // gui_text_convert_to_img(temperature_cur, RGB565);
                sprintf(tempera_low_content, "%d", low_val);
                gui_text_content_set(temperature_low, tempera_low_content, strlen(tempera_low_content));
                sprintf(tempera_high_content, "%d", high_val);
                // gui_log("%s %d\r\n", temperature_content, strlen(temperature_content));
                gui_text_content_set(temperature_high, tempera_high_content, strlen(tempera_high_content));
                weather_syn_flag = false;
            }
        }
    }
    // clear
    cJSON_Delete(root);
}

static void compass_cb(void)
{
    if (!(canvas_update_flag & 0x08))
    {
        return;
    }
    canvas_update_flag &= 0b0111;
    cJSON *root;
    if (!cjson_content)
    {
        // automation
        // float x = 100 / 2;
        // float y = 100 / 2;
        // float r = 35; // radius
        // // moving triangle
        // static uint16_t progress_compass = 0;
        // float a0 = M_PI * (float)progress_compass / 360 * 2; // 0 ~ 2pi CW
        // float ax, ay;
        // ax = x + cosf(a0 + M_PI * 3 / 2) * r;
        // ay = y + sinf(a0 + M_PI * 3 / 2) * r;

        // gui_img_set_location(compass_pointer, (uint16_t)ax, (uint16_t)ay);
        // gui_img_rotation(compass_pointer, (float)progress_compass, gui_img_get_width(compass_pointer) / 2,
        //                 gui_img_get_height(compass_pointer) / 2);
        // sprintf(degree_content, "%d°", progress_compass);
        // gui_text_content_set(compass_degree, degree_content, strlen(degree_content));

        // if (progress_compass == 0)                                 {sprintf(orien_content, "N");}
        // else if (progress_compass > 0 && progress_compass < 90)    {sprintf(orien_content, "NE");}
        // else if (progress_compass == 90)                           {sprintf(orien_content, "E");}
        // else if (progress_compass > 90 && progress_compass < 180)  {sprintf(orien_content, "SE");}
        // else if (progress_compass == 180)                          {sprintf(orien_content, "S");}
        // else if (progress_compass > 180 && progress_compass < 270) {sprintf(orien_content, "SW");}
        // else if (progress_compass == 270)                          {sprintf(orien_content, "W");}
        // else if (progress_compass > 270 && progress_compass < 360) {sprintf(orien_content, "NW");}
        // gui_text_content_set(compass_orien, orien_content, strlen(orien_content));
        // progress_compass += 1;
        // if (progress_compass >= 360)
        // {
        //     progress_compass = 0;
        // }
        return;
    }
    else
    {
        root = cJSON_Parse(cjson_content);
        if (!root)
        {
            gui_log("Error parsing JSON!\r\n");
            return;
        }
    }
    // parse compass array
    cJSON *compass_array = cJSON_GetObjectItemCaseSensitive(root, "compass");
    if (cJSON_IsArray(compass_array))
    {
        cJSON *comp = cJSON_GetArrayItem(compass_array, 0);
        if (!comp)
        {
            gui_log("get compass_array unsuccessful\n");
        }
        else
        {
            cJSON *degree = cJSON_GetObjectItemCaseSensitive(comp, "degree");
            uint16_t degree_val = degree->valueint;
            float x = 100 / 2;
            float y = 100 / 2;
            float r = 35; // radius
            // moving triangle
            float a0 = M_PI * degree_val / 360 * 2; // 0 ~ 2pi CW
            float ax, ay;
            ax = x + cosf(a0 + M_PI * 3 / 2) * r;
            ay = y + sinf(a0 + M_PI * 3 / 2) * r;

            gui_img_set_location(compass_pointer, (uint16_t)ax, (uint16_t)ay);
            gui_img_rotation(compass_pointer, (float)degree_val, gui_img_get_width(compass_pointer) / 2,
                             gui_img_get_height(compass_pointer) / 2);
            sprintf(degree_content, "%d°", degree_val);
            gui_text_content_set(compass_degree, degree_content, strlen(degree_content));
            uint16_t progress_compass = degree_val;
            if (progress_compass == 0)                                 {sprintf(orien_content, "N");}
            else if (progress_compass > 0 && progress_compass < 90)    {sprintf(orien_content, "NE");}
            else if (progress_compass == 90)                           {sprintf(orien_content, "E");}
            else if (progress_compass > 90 && progress_compass < 180)  {sprintf(orien_content, "SE");}
            else if (progress_compass == 180)                          {sprintf(orien_content, "S");}
            else if (progress_compass > 180 && progress_compass < 270) {sprintf(orien_content, "SW");}
            else if (progress_compass == 270)                          {sprintf(orien_content, "W");}
            else if (progress_compass > 270 && progress_compass < 360) {sprintf(orien_content, "NW");}
            gui_text_content_set(compass_orien, orien_content, strlen(orien_content));
        }
    }
    // clear
    cJSON_Delete(root);
}

static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(canvas_activity_animation)
{
    if (canvas_update_flag & 0x04)
    {
        uint8_t *img_data = (void *)gui_img_get_image_data(this_widget);
        memset(img_data, 0, (size_t)p);
        gui_canvas_output_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 100, 100, arc_activity_cb, img_data);
        gui_img_set_image_data(this_widget, img_data);
        canvas_update_flag &= 0b1011;
    }
}

static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(canvas_temperature_animation)
{
    if (canvas_update_flag & 0x02)
    {
        uint8_t *img_data = (void *)gui_img_get_image_data(this_widget);
        memset(img_data, 0, (size_t)p);
        gui_canvas_output_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 100, 100, arc_temperature_cb, img_data);
        gui_img_set_image_data(this_widget, img_data);
        canvas_update_flag &= 0b1101;
    }
}

void switch_heartrate()
{
    gui_view_switch_direct(gui_view_get_current_view(), heartrate_view, VIEW_ANIMATION_7,
                           VIEW_ANIMATION_1);
}

void page_ct_clock(gui_view_t *view)
{
    clear_mem();

    gui_obj_t *parent = GUI_BASE(view);

    gui_canvas_create(parent, NULL, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); //fb_change
    win_watch = gui_win_create(parent, "win_clock", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    return_to_watchface_flag = true;

    // temperature
    {
        int image_h = 100,
            image_w = 100,
            pixel_bytes = 4;
        size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        if (!img_data_temperature)
        {
            img_data_temperature = gui_lower_malloc(buffer_size);
        }
        memset(img_data_temperature, 0, buffer_size);
        gui_img_t *img = gui_img_create_from_mem(win_watch, 0, (void *)img_data_temperature,
                                                 37,
                                                 348, 100, 100);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_img_set_animate(img, 2000, -1, canvas_temperature_animation, (void *)buffer_size);
        //text
        sprintf(tempera_cur_content, "22");
        temperature_cur = gui_text_create(img, "temperature_cur",  32, 16, 0, 0); //32
        gui_text_set(temperature_cur, (void *)tempera_cur_content, GUI_FONT_SRC_TTF,  APP_COLOR_WHITE,
                     strlen(tempera_cur_content),
                     48);
        gui_text_type_set(temperature_cur, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(temperature_cur, LEFT);
        gui_text_rendermode_set(temperature_cur, 2);
        // gui_canvas_output_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 100, 100, arc_temperature_cb,
        //                          img_data_temperature);
        sprintf(tempera_low_content, "18");
        temperature_low = gui_text_create(img, "temperature_low",  19, 70, 0, 0);
        gui_text_set(temperature_low, (void *)tempera_low_content, GUI_FONT_SRC_TTF,
                     APP_COLOR_WHITE, //gui_rgba(191, 220, 48, UINT8_MAX),
                     strlen(tempera_low_content),
                     32);
        gui_text_type_set(temperature_low, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(temperature_low, LEFT);
        gui_text_rendermode_set(temperature_low, 2);

        sprintf(tempera_high_content, "37");
        temperature_high = gui_text_create(img, "temperature_high",  56, 70, 0, 0);
        gui_text_set(temperature_high, (void *)tempera_high_content, GUI_FONT_SRC_TTF,
                     APP_COLOR_WHITE, //gui_rgba(250, 17, 79, UINT8_MAX),
                     strlen(tempera_high_content),
                     32);
        gui_text_type_set(temperature_high, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(temperature_high, LEFT);
        gui_text_rendermode_set(temperature_high, 2);
    }

    // weather condition
    {
        gui_img_t *img_weather = gui_img_create_from_mem(win_watch, "CLOCK_CARD_WEATHER",
                                                         UI_CLOCK_CARD_WEATHER_BIN, 41, 178, 0, 0);
        gui_img_create_from_mem(img_weather, "condition_1", UI_WEATHER_CLOUDY_BIN, 28, 73, 0, 0);
        gui_img_create_from_mem(img_weather, "condition_2", UI_WEATHER_RAIN_L_BIN, 89, 73, 0, 0);
        gui_img_create_from_mem(img_weather, "condition_3", UI_WEATHER_RAIN_M_BIN, 150, 73, 0, 0);
        gui_img_create_from_mem(img_weather, "condition_4", UI_WEATHER_RAIN_S_BIN, 208, 73, 0, 0);
        gui_img_create_from_mem(img_weather, "condition_5", UI_WEATHER_SUNNY_BIN, 272, 73, 0, 0);
        char *weather_content = "TODAY";
        gui_text_t *weather_text = gui_text_create(img_weather, "",  16, 110, 0, 0);
        gui_text_set(weather_text, (void *)weather_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                     strlen(weather_content),
                     24);
        gui_text_type_set(weather_text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(weather_text, LEFT);
        gui_text_rendermode_set(weather_text, 2);

        sprintf(weekday_content, "MON.  TUE.  WED.  THU.");
        weather_text = gui_text_create(img_weather, "weather_text",  88, 110, 0, 0);
        gui_text_set(weather_text, (void *)weekday_content, GUI_FONT_SRC_TTF, gui_rgba(242, 242, 242, 255),
                     strlen(weekday_content),
                     24);
        gui_text_type_set(weather_text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(weather_text, LEFT);

        sprintf(content_cur, "22°");
        weather_cur = gui_text_create(img_weather, "weather_cur",  37, 5, 0, 0);
        gui_text_set(weather_cur, (void *)content_cur, GUI_FONT_SRC_TTF, gui_rgba(124, 199, 243, 255),
                     strlen(content_cur),
                     32);
        gui_text_type_set(weather_cur, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(weather_cur, LEFT);
        gui_text_rendermode_set(weather_cur, 2);

        sprintf(content_range, "H:37° L:18°");
        weather_range = gui_text_create(img_weather, "weather_range",  80, 5, 0, 0);
        gui_text_set(weather_range, (void *)content_range, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                     strlen(content_range),
                     32);
        gui_text_type_set(weather_range, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(weather_range, LEFT);
        gui_text_rendermode_set(weather_range, 2);
        gui_img_set_animate(img_weather, 3000, -1, weather_cb, img_weather);
    }
// #endif

    // date & time text
    sprintf(date_text_content, "SUN 0");
    date_text = gui_text_create(win_watch, "date_text",  -35, 33, 0, 0);
    gui_text_set(date_text, (void *)date_text_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(date_text_content),
                 48);
    gui_text_type_set(date_text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(date_text, RIGHT);
    gui_text_rendermode_set(date_text, 2);
    // gui_text_convert_to_img(date_text, RGB565);
    {
        int text_w = 35;
        gui_img_t *img = gui_img_create_from_mem(win_watch, "watch_hour_decimal", text_num_array[0],
                                                 211, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win_watch, "watch_hour_single", text_num_array[0],
                                      211 + text_w, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win_watch, "colon", text_num_array[10],
                                      211 + text_w * 2 + 5, 88 + 5, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win_watch, "watch_minute_decimal", text_num_array[0],
                                      211 + text_w * 2 + 17, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        img = gui_img_create_from_mem(win_watch, "watch_minute_single", text_num_array[0],
                                      211 + text_w * 3 + 17, 88, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    refreash_time();

    // activity icon
    {
        int image_h = 100,
            image_w = 100,
            pixel_bytes = 4;
        size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        if (!img_data_activity)
        {
            img_data_activity = gui_lower_malloc(buffer_size);
        }
        memset(img_data_activity, 0, buffer_size);
        // gui_canvas_output_buffer(GUI_CANVAS_OUTPUT_RGBA, 0, 100, 100, arc_activity_cb, img_data_activity);
        gui_img_t *img = gui_img_create_from_mem(win_watch, 0, (void *)img_data_activity, 37,
                                                 68, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_img_set_animate(img, 1000, -1, canvas_activity_animation, (void *)buffer_size);
    }
    // compass icon
    gui_img_t *img = gui_img_create_from_mem(win_watch, "CLOCK_COMPASS_DIAL",
                                             UI_CLOCK_COMPASS_DIAL_ICON_BIN, 155, 348, 0, 0);
    compass_pointer = gui_img_create_from_mem(img, "CLOCK_COMPASS_POINTER",
                                              UI_CLOCK_COMPASS_POINTER_ICON_BIN, 42, 10, 0, 0);
    compass_degree = gui_text_create(img, "compass_degree",  5, 23, 0, 0);
    gui_text_set(compass_degree, (void *)degree_content, GUI_FONT_SRC_TTF,  gui_rgba(254, 106, 26,
                 UINT8_MAX), //orange color
                 strlen(degree_content),
                 32);
    gui_text_type_set(compass_degree, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(compass_degree, CENTER);
    gui_text_rendermode_set(compass_degree, 2);

    compass_orien = gui_text_create(img, "compass_orien",  0, 47, 0, 0);
    gui_text_set(compass_orien, (void *)orien_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(orien_content),
                 32);
    gui_text_type_set(compass_orien, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(compass_orien, CENTER);
    gui_text_rendermode_set(compass_orien, 2);
    gui_img_set_animate(img, 1000, -1, compass_cb, NULL);

    img_heart_rate = gui_img_create_from_mem(win_watch, "CLOCK_HEARTRATE_ICON",
                                             UI_CLOCK_HEARTRATE_ICON_BIN, 272, 348, 0,
                                             0);

    gui_obj_add_event_cb(img_heart_rate, (gui_event_cb_t)switch_heartrate, GUI_EVENT_1, NULL);
    gui_win_set_animate(win_watch, 2000, -1, (gui_animate_callback_t)win_clock_cb, win_watch);
}
