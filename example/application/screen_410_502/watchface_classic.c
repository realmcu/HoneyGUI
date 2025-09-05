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
#include "app_main_watch.h"
#include "watch_adapt.h"

/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define COLOR_RED gui_rgb(255,0,0)
#define COLOR_SILVER gui_rgb(192,192,192)
#define COLOR_SILVER_OPACITY(opacity) gui_rgba(192,192,192, opacity)

/*============================================================================*
 *                            Variables
 *============================================================================*/
const static gui_view_descriptor_t *heartrate_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;

static gui_win_t *win_watch = NULL;
static char date_text_content[10] = {0};
extern struct tm *timeinfo;

bool return_to_watchface_flag = true; //true: return to watchface; false: return to app_menu

static gui_text_t *weather_date_text[4] = {0};
static bool weather_syn_flag = true; // sychronize data update of weather card and temp arc
static char content_cur[5] = {0};
static char content_range[15] = {0};
static char temp_cur_content[3] = {0};
static char temp_low_content[3] = {0};
static char temp_high_content[3] = {0};

static gui_img_t *compass_pointer = NULL;
static char degree_content[5] = "0°";
static char orien_content[3] = "N";

static uint8_t *img_data_temperature = NULL;
static uint8_t *img_data_activity = NULL;
static size_t buffer_size = 0;

extern char *cjson_content;
extern uint8_t json_refeash_flag;

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

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    heartrate_view = gui_view_descriptor_get("heartrate_view");
    menu_view = gui_view_descriptor_get("menu_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void time_update_cb()
{
    if (!timeinfo)
    {
        return;
    }
    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_hour_decimal, "watch_hour_decimal", win_watch);
    gui_img_set_image_data((gui_img_t *)img_hour_decimal, text_num_array[timeinfo->tm_hour / 10]);
    gui_img_refresh_size((gui_img_t *)img_hour_decimal);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_hour_single, "watch_hour_single", win_watch);
    gui_img_set_image_data((gui_img_t *)img_hour_single, text_num_array[timeinfo->tm_hour % 10]);
    gui_img_refresh_size((gui_img_t *)img_hour_single);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_minute_decimal, "watch_minute_decimal", win_watch);
    gui_img_set_image_data((gui_img_t *)img_minute_decimal, text_num_array[timeinfo->tm_min / 10]);
    gui_img_refresh_size((gui_img_t *)img_minute_decimal);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_minute_single, "watch_minute_single", win_watch);
    gui_img_set_image_data((gui_img_t *)img_minute_single, text_num_array[timeinfo->tm_min % 10]);
    gui_img_refresh_size((gui_img_t *)img_minute_single);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(date_text, "date_text", win_watch);
    sprintf(date_text_content, "%s %d",  day[timeinfo->tm_wday], timeinfo->tm_mday);
    gui_text_content_set((gui_text_t *)date_text, date_text_content, strlen(date_text_content));

    // refreash weather date
    uint8_t index = timeinfo->tm_wday + 1;

    for (uint8_t i = 0; i < 4; i++)
    {
        gui_text_content_set(weather_date_text[i], day[(index + i) % 7], 3);
    }
}

static void arc_activity_cb(NVGcontext *vg)
{
    cJSON *root;
    if (!cjson_content)
    {
        /* automatic update data if no json data*/
        // static float progress_activity = 0;
        // progress_activity += 0.001;
        // if (progress_activity >= 2)
        // {
        //     progress_activity = 0;
        // }
        // nvgBeginPath(canvas->vg);
        // nvgArc(canvas->vg, 16 + 100 / 2, 100 / 2, 50 - 8, 3 * M_PI_F / 2, M_PI_F * (progress_activity + 3 / 2),
        //     NVG_CW);
        // nvgStrokeWidth(canvas->vg, 8);
        // nvgStrokeColor(canvas->vg, nvgRGB(230, 67, 79));
        // nvgStroke(canvas->vg);

        // nvgBeginPath(canvas->vg);
        // nvgArc(canvas->vg, 16 + 100 / 2, 100 / 2, 50 - 21, 3 * M_PI_F / 2, M_PI_F * (progress_activity + 3 / 2),
        //     NVG_CW);
        // nvgStrokeWidth(canvas->vg, 8);
        // nvgStrokeColor(canvas->vg, nvgRGB(186, 253, 79));
        // nvgStroke(canvas->vg);

        // nvgBeginPath(canvas->vg);
        // nvgArc(canvas->vg, 16 + 100 / 2, 100 / 2, 50 - 33, 3 * M_PI_F / 2, M_PI_F * (progress_activity + 3 / 2),
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
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 8, 3 * M_PI_F / 2,
                   M_PI_F * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(58, 23, 29));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 8, 3 * M_PI_F / 2,
                   M_PI_F * (1.5f + 2.0f * move->valueint / 20000.0f), NVG_CW);  // cap 20000 steps
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(230, 67, 79));
            nvgStroke(vg);

            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 21, 3 * M_PI_F / 2,
                   M_PI_F * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(30, 55, 25));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 21, 3 * M_PI_F / 2,
                   M_PI_F * (1.5f + 2.0f * ex->valueint / 60.0f), NVG_CW);  // cap 60 min.
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(186, 253, 79));
            nvgStroke(vg);

            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 33, 3 * M_PI_F / 2,
                   M_PI_F * 3.5f, NVG_CW);
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(22, 50, 47));
            nvgStroke(vg);
            nvgBeginPath(vg);
            nvgArc(vg, 100 / 2, 100 / 2, 50 - 33, 3 * M_PI_F / 2,
                   M_PI_F * (1.5f + 2.0f * stand->valueint / 30.0f), NVG_CW); // cap 30 times
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(117, 230, 229));
            nvgStroke(vg);
        }
    }
    // clear
    cJSON_Delete(root);
}

static void update_weather_image(cJSON *weather, uint8_t i)
{
    char key[15];
    sprintf(key, "condition_%d", i);
    // gui_log("%s\r\n", key);
    // GUI_WIDGET_POINTER_BY_NAME(obj, key);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, key, win_watch);
    cJSON *condition = cJSON_GetObjectItemCaseSensitive(weather, key);
    if (strcmp(condition->valuestring, "Sunny") == 0)
    {
        gui_img_set_image_data((gui_img_t *)obj, UI_WEATHER_SUNNY_BIN);
    }
    else if (strcmp(condition->valuestring, "Light rain") == 0)
    {
        gui_img_set_image_data((gui_img_t *)obj, UI_WEATHER_RAIN_S_BIN);
    }
    else if (strcmp(condition->valuestring, "Showers") == 0)
    {
        gui_img_set_image_data((gui_img_t *)obj, UI_WEATHER_RAIN_S_BIN);
    }
    else if (strcmp(condition->valuestring, "Moderate rain") == 0)
    {
        gui_img_set_image_data((gui_img_t *)obj, UI_WEATHER_RAIN_M_BIN);
    }
    else if (strcmp(condition->valuestring, "Heavy rain") == 0)
    {
        gui_img_set_image_data((gui_img_t *)obj, UI_WEATHER_RAIN_L_BIN);
    }
    else if (strcmp(condition->valuestring, "Cloudy") == 0)
    {
        gui_img_set_image_data((gui_img_t *)obj, UI_WEATHER_CLOUDY_BIN);
    }
    else // need to add more weather icon
    {
        gui_img_set_image_data((gui_img_t *)obj, UI_WEATHER_CLOUDY_BIN);
    }
    gui_img_refresh_size((gui_img_t *)obj);
}

static void weather_cb()
{
    if (!(json_refeash_flag & 0x01))
    {
        return;
    }

    cJSON *root;
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
    cJSON *weather = cJSON_GetArrayItem(weather_array, 0);
    cJSON *low = cJSON_GetObjectItemCaseSensitive(weather, "low");
    cJSON *high = cJSON_GetObjectItemCaseSensitive(weather, "high");
    cJSON *cur = cJSON_GetObjectItemCaseSensitive(weather, "current");

    GUI_WIDGET_POINTER_BY_NAME_ROOT(weather_cur, "weather_cur", win_watch);
    sprintf(content_cur, "%d°", cur->valueint);
    gui_text_content_set((gui_text_t *)weather_cur, content_cur, strlen(content_cur));
    GUI_WIDGET_POINTER_BY_NAME_ROOT(weather_range, "weather_range", win_watch);
    sprintf(content_range, "H:%d° L:%d°", high->valueint, low->valueint);
    gui_text_content_set((gui_text_t *)weather_range, content_range, strlen(content_range));
    for (uint8_t i = 1; i <= 5; i++)
    {
        update_weather_image(weather, i);
    }
    cJSON_Delete(root);
    json_refeash_flag &= 0b1110;
}

static void arc_temperature_cb(NVGcontext *vg)
{
    cJSON *root;
    if (!cjson_content)
    {
        /* automatic update data if no json data*/
        // float x = 100 / 2;
        // float y = 100 / 2;
        // float r = 50 - 8; // radius
        // float r0 = M_PI_F * 5 / 6, r1 = M_PI_F * 13 / 6; // 5/6pi ~ 13/6pi CW
        // NVGpaint paint = nvgLinearGradient(vg, x + cosf(r0) * r, y + sinf(r0) * r, x + cosf(r1) * r,
        //                                    y + sinf(r1) * r, nvgRGB(191, 220, 48), nvgRGB(250, 17, 79));
        // nvgMoveTo(vg, x, y);
        // nvgLineTo(vg, x + cosf(r0) * r, y + sinf(r0) * r);
        // nvgArc(vg, x, y, r, r0, r1, NVG_CW);
        // nvgLineTo(vg, x, y);
        // nvgFillPaint(vg, paint);
        // nvgFill(vg);

        // nvgBeginPath(vg);
        // nvgArc(vg, x, y, r - 8.0f, 0, M_PI_F * 2.0f, NVG_CW);
        // nvgFillColor(vg, nvgRGB(0, 0, 0));
        // nvgFill(vg);

        // r -= 4;
        // nvgBeginPath(vg);
        // nvgArc(vg, x + cosf(r0 + 0.02f) * r, y + sinf(r0 + 0.02f) * r, 4.0f, r0, r0 + M_PI_F, NVG_CCW);
        // nvgFillColor(vg, nvgRGB(191, 220, 48));
        // nvgFill(vg);

        // nvgBeginPath(vg);
        // nvgArc(vg, x + cosf(r1 - 0.02f) * r, y + sinf(r1 - 0.02f) * r, 4.0f, r1, r1 - M_PI_F, NVG_CW);
        // nvgFillColor(vg, nvgRGB(250, 17, 79));
        // nvgFill(vg);

        // // moving dot
        // static float a0 = M_PI_F * 5 / 6; // 5/6pi ~ 13/6pi CW
        // float temp = (a0 - M_PI_F * 5 / 6) / (M_PI_F * 4 / 3);
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
        // GUI_WIDGET_POINTER_BY_NAME_ROOT(temp_cur, "temp_cur", win_watch);
        // sprintf(temp_cur_content, "%d", (int)(temp * (37 - 18) + 18 + 0.5));
        // gui_text_content_set((gui_text_t *)temp_cur, (void *)temp_cur_content, strlen(temp_cur_content));
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
            float r0 = M_PI_F * 5.0f / 6.0f;
            float r1 = M_PI_F * 13.0f / 6.0f;

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
            nvgArc(vg, x, y, r - 8.0f, 0, M_PI_F * 2.0f, NVG_CW);
            nvgFillColor(vg, nvgRGB(0, 0, 0));
            nvgFill(vg);

            // draw edge semi_circle
            r -= 4;
            nvgBeginPath(vg);
            nvgArc(vg, x + cosf(r0 + 0.02f) * r, y + sinf(r0 + 0.02f) * r, 4.0f, r0, r0 + M_PI_F, NVG_CCW);
            nvgFillColor(vg, nvgRGB(start_r, start_g, start_b));
            nvgFill(vg);

            nvgBeginPath(vg);
            nvgArc(vg, x + cosf(r1 - 0.02f) * r, y + sinf(r1 - 0.02f) * r, 4.0f, r1, r1 - M_PI_F, NVG_CW);
            nvgFillColor(vg, nvgRGB(stop_r, stop_g, stop_b));
            nvgFill(vg);

            // draw dot
            float temp = ((float)cur_val - (float)low_val) / ((float)high_val - (float)low_val);
            float a0 = M_PI_F * 5.0f / 6.0f + temp * 4.0f / 3.0f * M_PI_F; // 5/6pi ~ 13/6pi CW
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
                GUI_WIDGET_POINTER_BY_NAME_ROOT(temp_cur, "temp_cur", win_watch);
                sprintf(temp_cur_content, "%d", cur_val);
                gui_text_content_set((gui_text_t *)temp_cur, temp_cur_content, strlen(temp_cur_content));
                GUI_WIDGET_POINTER_BY_NAME_ROOT(temp_low, "temp_low", win_watch);
                sprintf(temp_low_content, "%d", low_val);
                gui_text_content_set((gui_text_t *)temp_low, temp_low_content, strlen(temp_low_content));
                GUI_WIDGET_POINTER_BY_NAME_ROOT(temp_high, "temp_high", win_watch);
                sprintf(temp_high_content, "%d", high_val);
                gui_text_content_set((gui_text_t *)temp_high, temp_high_content, strlen(temp_high_content));
                weather_syn_flag = false;
            }
        }
    }
    // clear
    cJSON_Delete(root);
}

static void compass_cb()
{
    if (!(json_refeash_flag & 0x08))
    {
        return;
    }
    cJSON *root;
    // if (!cjson_content)
    // {
    /* automatic update data if no json data*/
    // float x = 100 / 2;
    // float y = 100 / 2;
    // float r = 35; // radius
    // // moving triangle
    // static uint16_t progress_compass = 0;
    // float a0 = M_PI_F * (float)progress_compass / 360 * 2; // 0 ~ 2pi CW
    // float ax, ay;
    // ax = x + cosf(a0 + M_PI_F * 3 / 2) * r;
    // ay = y + sinf(a0 + M_PI_F * 3 / 2) * r;

    // compass_pointer->base.x = (uint16_t)ax;
    // compass_pointer->base.y = (uint16_t)ay;
    // gui_img_rotation(compass_pointer, (float)progress_compass);
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
    //     return;
    // }
    // else
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
    cJSON *comp = cJSON_GetArrayItem(compass_array, 0);
    cJSON *degree = cJSON_GetObjectItemCaseSensitive(comp, "degree");
    uint16_t degree_val = degree->valueint;
    float x = 100 / 2;
    float y = 100 / 2;
    float r = 35; // radius
    // moving triangle
    float a0 = M_PI_F * degree_val / 360 * 2; // 0 ~ 2pi CW
    float ax, ay;
    ax = x + cosf(a0 + M_PI_F * 3 / 2) * r + sinf(a0 + M_PI_F * 3 / 2) * compass_pointer->base.w / 2;
    ay = y + sinf(a0 + M_PI_F * 3 / 2) * r + cosf(a0 + M_PI_F * 1 / 2) * compass_pointer->base.w / 2;

    compass_pointer->base.x = (uint16_t)ax;
    compass_pointer->base.y = (uint16_t)ay;
    // gui_img_translate (compass_pointer, ax, ay);
    gui_img_rotation(compass_pointer, (float)degree_val);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(compass_degree, "compass_degree", win_watch);
    sprintf(degree_content, "%d°", degree_val);
    gui_text_content_set((gui_text_t *)compass_degree, degree_content, strlen(degree_content));
    uint16_t progress_compass = degree_val;
    if (progress_compass == 0)        {sprintf(orien_content, "N");}
    else if (progress_compass < 90)   {sprintf(orien_content, "NE");}
    else if (progress_compass == 90)  {sprintf(orien_content, "E");}
    else if (progress_compass < 180)  {sprintf(orien_content, "SE");}
    else if (progress_compass == 180) {sprintf(orien_content, "S");}
    else if (progress_compass < 270)  {sprintf(orien_content, "SW");}
    else if (progress_compass == 270) {sprintf(orien_content, "W");}
    else if (progress_compass < 360)  {sprintf(orien_content, "NW");}
    GUI_WIDGET_POINTER_BY_NAME_ROOT(compass_orien, "compass_orien", win_watch);
    gui_text_content_set((gui_text_t *)compass_orien, orien_content, strlen(orien_content));
    // clear
    cJSON_Delete(root);
    json_refeash_flag &= 0b0111;
}

static void activity_timer_cb(void *obj)
{
    gui_img_t *img = (gui_img_t *)obj;
    if (json_refeash_flag & 0x04)
    {
        uint8_t *img_data = (void *)gui_img_get_image_data(img);
        memset(img_data, 0, buffer_size);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, 100, 100, arc_activity_cb, img_data);
        gui_img_set_image_data(img, img_data);

        json_refeash_flag &= 0b1011;
    }
}

static void temp_timer_cb(void *obj)
{
    gui_img_t *img = (gui_img_t *)obj;
    if (json_refeash_flag & 0x02)
    {
        uint8_t *img_data = (void *)gui_img_get_image_data(img);
        memset(img_data, 0, buffer_size);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, 100, 100, arc_temperature_cb,
                                          img_data);
        gui_img_set_image_data(img, img_data);
        json_refeash_flag &= 0b1101;
    }
}

static void switch_heartrate()
{
    gui_view_switch_direct(gui_view_get_current(), heartrate_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_call_incoming()
{
    *gui_call_incoming_flag_get() = true;
}

static void switch_menu()
{
    gui_view_switch_direct(gui_view_get_current(), menu_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void create_watchface_classic(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);

    gui_canvas_create(parent, NULL, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); //fb_change
    win_watch = gui_win_create(parent, "win_clock", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    return_to_watchface_flag = true;

    // temperature
    {
        int image_h = 100;
        int image_w = 100;
        int pixel_bytes = 2;
        buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        if (!img_data_temperature)
        {
            img_data_temperature = gui_lower_malloc(buffer_size);
        }
        memset(img_data_temperature, 0, buffer_size);
        gui_img_t *img = gui_img_create_from_mem(win_watch, 0, (void *)img_data_temperature,
                                                 37,
                                                 348, 100, 100);
        // gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_img_set_quality(img, true);
        gui_obj_create_timer(GUI_BASE(img), 2000, true, temp_timer_cb);
        //text
        // sprintf(temp_cur_content, "22");
        gui_text_t *temp_cur = gui_text_create(img, "temp_cur",  32, 16, 0, 0); //32
        gui_text_set(temp_cur, (void *)temp_cur_content, GUI_FONT_SRC_TTF,  APP_COLOR_WHITE,
                     strlen(temp_cur_content),
                     48);
        gui_text_type_set(temp_cur, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(temp_cur, LEFT);
        gui_text_rendermode_set(temp_cur, 2);

        // sprintf(temp_low_content, "18");
        gui_text_t *temp_low = gui_text_create(img, "temp_low",  19, 70, 0, 0);
        gui_text_set(temp_low, (void *)temp_low_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                     strlen(temp_low_content), 32);
        gui_text_type_set(temp_low, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(temp_low, LEFT);
        gui_text_rendermode_set(temp_low, 2);

        // sprintf(temp_high_content, "37");
        gui_text_t *temp_high = gui_text_create(img, "temp_high",  56, 70, 0, 0);
        gui_text_set(temp_high, (void *)temp_high_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                     strlen(temp_high_content), 32);
        gui_text_type_set(temp_high, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(temp_high, LEFT);
        gui_text_rendermode_set(temp_high, 2);

        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, 100, 100, arc_temperature_cb,
                                          img_data_temperature);
        gui_img_refresh_size(img);
    }

    // weather condition
    {
        gui_img_t *img_weather = gui_img_create_from_mem(win_watch, "CLOCK_CARD_WEATHER",
                                                         UI_CLOCK_CARD_WEATHER_BIN, 41, 178, 0, 0);
        gui_img_set_quality(img_weather, true);
        gui_obj_add_event_cb(img_weather, (gui_event_cb_t)switch_menu, GUI_EVENT_TOUCH_CLICKED,
                             NULL);
        gui_img_t *img = gui_img_create_from_mem(img_weather, "condition_1", UI_WEATHER_CLOUDY_BIN, 28, 73,
                                                 0, 0);
        gui_img_set_quality(img, true);
        img = gui_img_create_from_mem(img_weather, "condition_2", UI_WEATHER_RAIN_L_BIN, 89, 73, 0, 0);
        gui_img_set_quality(img, true);
        img = gui_img_create_from_mem(img_weather, "condition_3", UI_WEATHER_RAIN_M_BIN, 150, 73, 0, 0);
        gui_img_set_quality(img, true);
        img = gui_img_create_from_mem(img_weather, "condition_4", UI_WEATHER_RAIN_S_BIN, 208, 73, 0, 0);
        gui_img_set_quality(img, true);
        img = gui_img_create_from_mem(img_weather, "condition_5", UI_WEATHER_SUNNY_BIN, 272, 73, 0, 0);
        gui_img_set_quality(img, true);
        char *weather_content = "Today";
        gui_text_t *weather_text = gui_text_create(img_weather, "txt", 20, 110, 0, 0);
        gui_text_set(weather_text, (void *)weather_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                     strlen(weather_content),
                     24);
        gui_text_type_set(weather_text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(weather_text, LEFT);
        gui_text_rendermode_set(weather_text, 2);

        for (uint8_t i = 0; i < 4; i++)
        {
            weather_date_text[i] = gui_text_create(img_weather, "weather_date_text",
                                                   88 + i * 60, 110, 0, 0);
            gui_text_set(weather_date_text[i], (void *)day[i], GUI_FONT_SRC_TTF, gui_rgba(242, 242, 242, 255),
                         3, 24);
            gui_text_type_set(weather_date_text[i], SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(weather_date_text[i], LEFT);
        }

        sprintf(content_cur, "22°");
        gui_text_t *weather_cur = gui_text_create(img_weather, "weather_cur",  37, 5, 0, 0);
        gui_text_set(weather_cur, (void *)content_cur, GUI_FONT_SRC_TTF, gui_rgba(124, 199, 243, 255),
                     strlen(content_cur),
                     32);
        gui_text_type_set(weather_cur, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(weather_cur, LEFT);
        gui_text_rendermode_set(weather_cur, 2);

        sprintf(content_range, "H:37° L:18°");
        gui_text_t *weather_range = gui_text_create(img_weather, "weather_range",  80, 5, 0, 0);
        gui_text_set(weather_range, (void *)content_range, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                     strlen(content_range),
                     32);
        gui_text_type_set(weather_range, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(weather_range, LEFT);
        gui_text_rendermode_set(weather_range, 2);
        gui_obj_create_timer(GUI_BASE(img_weather), 2000, true, weather_cb);
    }

    // date & time text
    sprintf(date_text_content, "%s", day[0]);
    gui_text_t *date_text = gui_text_create(win_watch, "date_text",  -35, 33, 0, 0);
    gui_text_set(date_text, (void *)date_text_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(date_text_content),
                 48);
    gui_text_type_set(date_text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(date_text, RIGHT);
    gui_text_rendermode_set(date_text, 2);
    {
        int text_w = 35;
        gui_img_t *img = gui_img_create_from_mem(win_watch, "watch_hour_decimal", text_num_array[0],
                                                 211, 88, 0, 0);
        gui_img_set_quality(img, true);
        img = gui_img_create_from_mem(win_watch, "watch_hour_single", text_num_array[0],
                                      211 + text_w, 88, 0, 0);
        gui_img_set_quality(img, true);
        img = gui_img_create_from_mem(win_watch, "colon", text_num_array[10],
                                      211 + text_w * 2 + 5, 88 + 5, 0, 0);
        gui_img_set_quality(img, true);
        img = gui_img_create_from_mem(win_watch, "watch_minute_decimal", text_num_array[0],
                                      211 + text_w * 2 + 17, 88, 0, 0);
        gui_img_set_quality(img, true);
        img = gui_img_create_from_mem(win_watch, "watch_minute_single", text_num_array[0],
                                      211 + text_w * 3 + 17, 88, 0, 0);
        gui_img_set_quality(img, true);
    }

    // activity icon
    {
        if (!img_data_activity)
        {
            img_data_activity = gui_lower_malloc(buffer_size);
        }
        memset(img_data_activity, 0, buffer_size);
        gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, 100, 100, arc_activity_cb,
                                          img_data_activity);
        gui_img_t *img = gui_img_create_from_mem(win_watch, 0, (void *)img_data_activity, 37,
                                                 68, 0, 0);
        // gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_img_set_quality(img, true);
        gui_obj_create_timer(GUI_BASE(img), 2000, true, activity_timer_cb);
    }
    // compass icon
    gui_img_t *img = gui_img_create_from_mem(win_watch, "CLOCK_COMPASS_DIAL",
                                             UI_CLOCK_COMPASS_DIAL_ICON_BIN, 155, 348, 0, 0);
    gui_img_set_quality(img, true);
    gui_obj_add_event_cb(img, (gui_event_cb_t)switch_call_incoming, GUI_EVENT_TOUCH_CLICKED,
                         NULL);

    compass_pointer = gui_img_create_from_mem(img, "CLOCK_COMPASS_POINTER",
                                              UI_CLOCK_COMPASS_POINTER_ICON_BIN, 42, 10, 0, 0);
    gui_img_set_quality(compass_pointer, true);

    gui_text_t *compass_degree = gui_text_create(img, "compass_degree", 0, 23, 100, 100);
    gui_text_set(compass_degree, (void *)degree_content, GUI_FONT_SRC_TTF,  gui_rgba(254, 106, 26,
                 UINT8_MAX), //orange color
                 strlen(degree_content),
                 32);
    gui_text_type_set(compass_degree, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(compass_degree, CENTER);
    gui_text_rendermode_set(compass_degree, 2);

    gui_text_t *compass_orien = gui_text_create(img, "compass_orien", 0, 47, 100, 100);
    gui_text_set(compass_orien, (void *)orien_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(orien_content),
                 32);
    gui_text_type_set(compass_orien, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(compass_orien, CENTER);
    gui_text_rendermode_set(compass_orien, 2);
    gui_obj_create_timer(GUI_BASE(img), 2000, true, compass_cb);
    compass_cb();

    gui_img_t *img_heart_rate = gui_img_create_from_mem(win_watch, "CLOCK_HEARTRATE_ICON",
                                                        UI_CLOCK_HEARTRATE_ICON_BIN, 272, 348, 0,
                                                        0);
    gui_img_set_quality(img_heart_rate, true);
    gui_obj_add_event_cb(img_heart_rate, (gui_event_cb_t)switch_heartrate, GUI_EVENT_TOUCH_CLICKED,
                         NULL);
    gui_obj_create_timer(GUI_BASE(win_watch), 30000, true, time_update_cb);
}

void clear_watchface_classic(gui_view_t *view)
{
    (void)view;
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
