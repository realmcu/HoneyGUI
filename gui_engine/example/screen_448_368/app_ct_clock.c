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

// gui_win_t *win_watch;
// gui_img_t *img;
// gui_watch_gradient_spot_t *watch;
// gui_text_t *rate;
// static gui_watchface_gradient_t *canvas;
// gui_tabview_t *tablist_tab;
// struct tm *timeinfo;
// /* watchface_watch_gradient_spot demo end*/

// static void canvas_cb(gui_canvas_t *canvas)
// {
//     nvgRect(canvas->vg, 0, 0, 368, 448);
//     nvgFillColor(canvas->vg, nvgRGBA(30, 30, 30, 255));
//     nvgFill(canvas->vg);
// }

// static void callback_time()
// {
//     int idx = tablist_tab->cur_id.x;
//     gui_log("idx:%d\n", idx);
//     gui_app_t *app = (gui_app_t *)get_app_hongkong();
//     gui_obj_t *screen = &(app->screen);
//     gui_obj_tree_free(screen);
//     app->ui_design(app);
//     switch (idx)
//     {
//     case 0:
//         {
//             GET_BASE(img)->not_show = false;
//             GET_BASE(watch)->not_show = true;
//             GET_BASE(canvas)->not_show = true;
//         }
//         break;
//     case 1:
//         {
//             GET_BASE(img)->not_show = true;
//             GET_BASE(watch)->not_show = false;
//             GET_BASE(canvas)->not_show = true;
//         }
//         break;
//     case 2:
//         {
//             GET_BASE(img)->not_show = true;
//             GET_BASE(watch)->not_show = true;
//             GET_BASE(canvas)->not_show = false;
//         }
//         break;
//     default:
//         break;
//     }

// }

// /* callback_touch_long start*/
// static void callback_touch_long(void *obj, gui_event_t e)
// {
//     gui_log("win widget long touch enter cb\n");
//     gui_app_t *app = (gui_app_t *)get_app_hongkong();
//     gui_obj_t *screen = &(app->screen);
//     int idx = 0;
//     if (!GET_BASE(img)->not_show)
//     {
//         idx = 0;
//     }
//     if (!GET_BASE(watch)->not_show)
//     {
//         idx = 1;
//     }
//     if (!GET_BASE(canvas)->not_show)
//     {
//         idx = 2;
//     }

//     gui_obj_tree_free(screen);
//     gui_win_t *win = gui_win_create(screen, "win", 0, 0, 320, 320);
// #if ENABLE_RTK_GUI_WATCHFACE_UPDATE
//     extern void create_tree_nest(char *xml, void *obj);
//     create_tree_nest("gui_engine\\example\\screen_448_368\\root_image_hongkong\\watch_face_update\\app\\wf\\wf.xml",
//                      win);
//     return;
// #endif
//     gui_obj_add_event_cb(win, (gui_event_cb_t)callback_time, GUI_EVENT_TOUCH_CLICKED, NULL);

//     tablist_tab = gui_tabview_create(win, "tabview", 59, 84, 250, 300);
//     gui_tabview_set_style(tablist_tab, CLASSIC);
//     gui_tab_t *tb_watchface = gui_tab_create(tablist_tab, "tb_watchface",   0, 0, 250, 0, 2, 0);
//     gui_tab_t *tb_watch = gui_tab_create(tablist_tab, "tb_watch",           0, 0, 250, 0, 1, 0);

//     gui_tab_t *tb_clock = gui_tab_create(tablist_tab, "tb_clock",           0, 0, 250, 0, 0, 0);
//     gui_tabview_jump_tab(tablist_tab, idx, 0);
//     extern void tablist_clock(void *parent);
//     tablist_clock(tb_clock);
//     extern void tablist_watch(void *parent);
//     tablist_watch(tb_watch);
//     extern void tablist_watchface(void *parent);
//     tablist_watchface(tb_watchface);
// }
// /* callback_touch_long end*/

// static void callback_watchface()
// {
//     static char clock;
//     if (clock == 1)
//     {
//         GET_BASE(img)->not_show = false;
//         GET_BASE(watch)->not_show = true;
//         GET_BASE(canvas)->not_show = true;
//         clock = 2;
//     }
//     else if (clock == 2)
//     {
//         GET_BASE(img)->not_show = true;
//         GET_BASE(watch)->not_show = false;
//         GET_BASE(canvas)->not_show = true;
//         clock = 0;
//     }
//     else if (clock == 0)
//     {
//         GET_BASE(img)->not_show = true;
//         GET_BASE(watch)->not_show = true;
//         GET_BASE(canvas)->not_show = false;
//         clock = 1;
//     }
// }


// /* page_ct_clock start*/
// void page_ct_clock(void *parent)
// {
//     win_watch = gui_win_create(parent, "win", 0, 0, 368, 448);

//     gui_obj_add_event_cb(win_watch, (gui_event_cb_t)callback_touch_long, GUI_EVENT_TOUCH_LONG, NULL);

//     img = gui_img_create_from_mem(parent, "page0", CLOCKN_BIN, 0, 0, 0, 0);
//     gui_img_set_mode(img, IMG_SRC_OVER_MODE);
//     watch = gui_watch_gradient_spot_create(win_watch, "watchface", 0, 0, 0, 0);
//     gui_watch_gradient_spot_set_center(watch, 368 / 2, 448 / 2);
//     canvas = gui_watchface_gradient_create(parent, "watchface_gradient", (368 - 368) / 2,
//                                            (448 - 448) / 2, 368, 448);
//     GET_BASE(watch)->not_show = true;
//     GET_BASE(canvas)->not_show = true;
// }
// /* page_ct_clock end*/

#if 1
#include "time.h"
#include "gui_curtainview.h"
#include "tp_algo.h"
#include <math.h>
#include "cJSON.h"
#include "gui_canvas_img.h"
#include "gui_curtain.h"

#define SCREEN_WIDTH 368
#define SCREEN_HEIGHT 448
#define SCREEN_X_OFF 0
#define SCREEN_Y_OFF 0
#define COLOR_RED gui_rgb(255,0,0)
#define COLOR_SILVER gui_rgb(192,192,192)
#define COLOR_SILVER_OPACITY(opacity) gui_rgba(192,192,192, opacity)
static void *font_size_96_bin_addr = SOURCEHANSANSSC_SIZE96_BITS4_FONT_BIN;
static void *font_size_48_bin_addr = SOURCEHANSANSSC_SIZE48_BITS4_FONT_BIN;
static void *font_size_40_bin_addr = SOURCEHANSANSSC_SIZE40_BITS4_FONT_BIN;
static void *font_size_32_bin_addr = SOURCEHANSANSSC_SIZE32_BITS4_FONT_BIN;
static void *font_size_24_bin_addr = SOURCEHANSANSSC_SIZE24_BITS4_FONT_BIN;

static uint8_t curtain_index = CURTAIN_MIDDLE;
static gui_win_t *win_watch, *win_hb;
static gui_text_t *time_text, *date_text;
static char time_text_content[10], date_text_content[10];
static char weekday_content[30];
static struct tm watch_time;
struct tm *timeinfo;

static gui_img_t *watchface_1, *watchface_2;
static gui_tabview_t *tablist_tab;
bool return_to_watchface_flag; //true: return to watchface; flases: return to app_menu

static uint8_t canvas_update_flag = 0;
static gui_canvas_t *canvas_temperature; //gui_canvas_img_t
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

char *cjson_content = NULL;

static uint16_t seed = 12345;
uint16_t xorshift16()
{
    seed ^= seed << 6;
    seed ^= seed >> 9;
    seed ^= seed << 2;
    return seed;
}

static void heart_rate_app(gui_app_t *app);
static gui_app_t _app_APP_HEART_RATE =
{
    .screen = {
        .name = "APP_HEART_RATE",
        .type = SCREEN,
        .magic = GUI_MAGIC_NUMBER,
        .parent = NULL,
        .matrix = NULL,
    },
    .ui_design = heart_rate_app,
    .active_ms = 100000,
};
static char hr_content[4];

#if defined __WIN32
const char *filename =
    "./gui_engine/example/web/peripheral_simulation/json/simulation_data.json";
/* read CJSON to string */
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
#else
// #include "tuya_ble_feature_weather.h"
// #include "watch_clock.h"

void json_refreash()
{
    uint16_t degree = xorshift16() % 359;
    uint16_t move = xorshift16() % 20000;
    uint16_t ex = xorshift16() % 60;
    uint16_t stand = xorshift16() % 30;
    move = move < 1 ? 1 : move;
    ex = ex < 1 ? 1 : ex;
    stand = stand < 1 ? 1 : stand;
    uint16_t AM12 = xorshift16() % 120;
    uint16_t AM6 = xorshift16() % 120;
    uint16_t PM12 = xorshift16() % 120;
    uint16_t PM6 = xorshift16() % 120;
    // gui_log("!cjson_content: %x %x %x %x %x\n", cjson_content[0], cjson_content[1], cjson_content[2], cjson_content[3], cjson_content[4]);
    cjson_content[0] = 0x7b;
    cJSON *root = cJSON_Parse(cjson_content);
    if (!root)
    {
        gui_log("json_refreash Error parsing JSON!\r\n");
        return;
    }
    cJSON *compass_array = cJSON_GetObjectItem(root, "compass");
    if (compass_array != NULL && cJSON_GetArraySize(compass_array) > 0)
    {
        cJSON *compass_item = cJSON_GetArrayItem(compass_array, 0);
        cJSON_ReplaceItemInObject(compass_item, "degree", cJSON_CreateNumber(degree));
    }

    cJSON *activity_array = cJSON_GetObjectItem(root, "activity");
    if (activity_array != NULL && cJSON_GetArraySize(activity_array) > 0)
    {
        cJSON *activity_item = cJSON_GetArrayItem(activity_array, 0);
        cJSON_ReplaceItemInObject(activity_item, "move", cJSON_CreateNumber(move));
        cJSON_ReplaceItemInObject(activity_item, "ex", cJSON_CreateNumber(ex));
        cJSON_ReplaceItemInObject(activity_item, "stand", cJSON_CreateNumber(stand));
    }

    cJSON *heart_rate_array = cJSON_GetObjectItem(root, "heart_rate");
    if (heart_rate_array != NULL && cJSON_GetArraySize(heart_rate_array) > 0)
    {
        cJSON *heart_rate_item = cJSON_GetArrayItem(heart_rate_array, 0);
        {
            AM12 = AM12 > 60 ? AM12 : 68;
            cJSON_ReplaceItemInObject(heart_rate_item, "AM12", cJSON_CreateNumber(AM12));
        }
        {
            AM6 = AM6 > 60 ? AM6 : 73;
            cJSON_ReplaceItemInObject(heart_rate_item, "AM6", cJSON_CreateNumber(AM6));
        }
        {
            PM12 = PM12 > 60 ? PM12 : 82;
            cJSON_ReplaceItemInObject(heart_rate_item, "PM12", cJSON_CreateNumber(PM12));
        }
        {
            PM6 = PM6 > 60 ? PM6 : 94;
            cJSON_ReplaceItemInObject(heart_rate_item, "PM6", cJSON_CreateNumber(PM6));
        }
    }
    char *temp = cJSON_PrintUnformatted(root);
    sprintf(cjson_content, "%s", temp);
    gui_free(temp);
    cJSON_Delete(root);
    canvas_update_flag = 0b0111;
    gui_log("cjson_content: %s\n", cjson_content);
}
#endif

extern char *day[];
static void refreash_time()
{
    sprintf(time_text_content, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    gui_text_content_set(time_text, time_text_content, strlen(time_text_content));
    // gui_text_convert_to_img(time_text, ARGB8888);

    sprintf(date_text_content, "%s %d",  day[timeinfo->tm_wday], timeinfo->tm_mday);
    gui_text_content_set(date_text, date_text_content, strlen(date_text_content));
    // gui_text_convert_to_img(date_text, ARGB8888);
}

static void win_clock_cb(gui_win_t *win)
{
    bool slide_card_enable;
    if (win->animate->Beginning_frame)
    {
#if defined __WIN32
        time_t rawtime;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        char *temp = cjson_content;
        cjson_content = read_file(filename);
        if (!cjson_content)
        {
            cjson_content = temp;
            perror("fopen");
        }
        else
        {
            free(temp);
        }
#else
        // watch_time = watch_clock_get();
        timeinfo = &watch_time;
        gui_log("time %d:%d\r\n", timeinfo->tm_hour, timeinfo->tm_min);
        gui_log("date %d:%d\r\n", timeinfo->tm_mon + 1, timeinfo->tm_mday);
        // json_refreash();
        // tuya_ble_feature_weather_data_request(WKT_TEMP | WKT_THIHG | WKT_TLOW | WKT_CONDITION, 5);
#endif
        refreash_time();
    }

    // blur this curtain
    gui_curtainview_t *cv = 0;
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "ct_clock",
                                    (void *)&cv); //  can find "ct_clock" in "app_tb_clock.c"
    if (cv)
    {
        if (cv->cur_curtain == CURTAIN_MIDDLE)
        {
            // GUI_BASE(win_watch)->opacity_value = 255;
            GUI_BASE(img_heart_rate)->event_dsc_cnt = 1;
            slide_card_enable = true;
            curtain_index = CURTAIN_MIDDLE;
        }
        else
        {
            // GUI_BASE(win_watch)->opacity_value = 50;
            GUI_BASE(img_heart_rate)->event_dsc_cnt = 0; //block img_heart_rate cb func
            slide_card_enable = false;
            curtain_index = 0U;
        }
    }
    // gui_log("curtain_index = %d", curtain_index);

    touch_info_t *tp = tp_get_info();
    static bool hold;
    if (tp->pressed)
    {
        hold = 1;
    }
    if (tp->released)
    {
        hold = 0;
    }
    if (hold)
    {
        // enable slide card in ct_clock
        if (slide_card_enable && tp->x > 50 && tp->x < 300 && tp->y > 220 && tp->y < 330)
        {
            gui_tabview_t *tv = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "clock_tv", (void *)&tv);
            if (tv)
            {
                GUI_BASE(tv)->gesture = 0;
            }
            gui_tabview_t *clock = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "hongkong_tabview",
                                            (void *)&clock); //  can find "hongkong_tabview" in "app_hongkong.c"
            if (clock)
            {
                GUI_BASE(clock)->gesture = 1;
            }
            if (cv)
            {
                GUI_BASE(cv)->gesture = 1;
            }
        }
        else
        {
            gui_tabview_t *tv = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "clock_tv", (void *)&tv);
            if (tv)
            {
                GUI_BASE(tv)->gesture = 1;
            }
            gui_tabview_t *clock = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "hongkong_tabview",
                                            (void *)&clock); //  can find "hongkong_tabview" in "app_hongkong.c"
            if (clock)
            {
                GUI_BASE(clock)->gesture = 0;
            }
            if (cv)
            {
                GUI_BASE(cv)->gesture = 0;
            }
        }
    }
}
static void arc_activity_cb(gui_canvas_t *canvas)
{
    cJSON *root;
    NVGcontext *vg = canvas->vg;
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
        cjson_content[0] = 0x7b;
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
            cJSON *ex = cJSON_GetObjectItemCaseSensitive(act, "ex");
            cJSON *stand = cJSON_GetObjectItemCaseSensitive(act, "stand");
            nvgBeginPath(vg);
            nvgArc(vg, 16 + 100 / 2, 100 / 2, 50 - 8, 3 * M_PI / 2,
                   M_PI * (1.5f + 2.0f * move->valuedouble / 20000.0f), NVG_CW);  // cap 20000 steps
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(230, 67, 79));
            nvgStroke(vg);

            nvgBeginPath(vg);
            nvgArc(vg, 16 + 100 / 2, 100 / 2, 50 - 21, 3 * M_PI / 2,
                   M_PI * (1.5f + 2.0f * ex->valueint / 60.0f), NVG_CW);  // cap 60 min.
            nvgStrokeWidth(vg, 8);
            nvgStrokeColor(vg, nvgRGB(186, 253, 79));
            nvgStroke(vg);

            nvgBeginPath(vg);
            nvgArc(vg, 16 + 100 / 2, 100 / 2, 50 - 33, 3 * M_PI / 2,
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
            cjson_content[0] = 0x7b;
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
                cJSON *cur = cJSON_GetObjectItemCaseSensitive(weather, "cur");

                sprintf(content_cur, "%d°", cur->valueint);
                gui_text_content_set(weather_cur, content_cur, strlen(content_cur));
                sprintf(content_range, "H:%d° L:%d°", high->valueint, low->valueint);
                gui_text_content_set(weather_range, content_range, strlen(content_range));

                for (uint8_t i = 1; i <= 5; i++)
                {
                    char key[15];
                    sprintf(key, "condition_%d", i);
                    // gui_log("%s\r\n", key);
                    GUI_WIDGET_POINTER_BY_NAME(obj, key);
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

        // refreash date
        GUI_WIDGET_POINTER_BY_NAME(obj, "weather_text");
        uint8_t index = timeinfo->tm_wday;

        sprintf(weekday_content, "%s.  %s.  %s.  %s.", day[(index + 1) % 7], day[(index + 2) % 7],
                day[(index + 3) % 7],
                day[(index + 4) % 7]);
        // gui_log("%s\r\n", weekday_content);
        gui_text_content_set((gui_text_t *)obj, weekday_content, strlen(weekday_content));
    }
}

static void arc_temperature_cb(gui_canvas_t *canvas)
{
    NVGcontext *vg = canvas->vg;
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
        cjson_content[0] = 0x7b;
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
            cJSON *cur = cJSON_GetObjectItemCaseSensitive(weather, "cur");
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
        cjson_content[0] = 0x7b;
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

// heart_rate
static void canvas_cb_heartbreak_bg(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                   50); // the X/Y-axis coordinate relative to parent widget
    nvgFillColor(canvas->vg, nvgRGBA(0, 0, 0, 255));
    nvgFill(canvas->vg);
}

static void win_hb_cb(gui_win_t *win)
{
#ifdef __WIN32
    if (win->animate->Beginning_frame)
    {
        char *temp = cjson_content;
        cjson_content = read_file(filename);
        if (!cjson_content)
        {
            cjson_content = temp;
            perror("fopen");
        }
        else
        {
            free(temp);
        }
    }
#endif
}

static void canvas_cb_heartbreak_graph(gui_canvas_t *canvas)
{
    NVGcontext *vg = canvas->vg;

    // draw split line
    {
        nvgBeginPath(vg);
        nvgRect(vg, 10 + SCREEN_X_OFF + 86 * 0, 60 + SCREEN_Y_OFF, 5,
                210); // the X/Y-axis coordinate relative to parent widget
        nvgFillColor(vg, nvgRGBA(192, 192, 192, 100)); //silver
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgRect(vg, 10 + SCREEN_X_OFF + 86 * 1, 60 + SCREEN_Y_OFF, 5,
                210); // the X/Y-axis coordinate relative to parent widget
        nvgFillColor(vg, nvgRGBA(192, 192, 192, 100)); //silver
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgRect(vg, 10 + SCREEN_X_OFF + 86 * 2, 60 + SCREEN_Y_OFF, 5,
                210); // the X/Y-axis coordinate relative to parent widget
        nvgFillColor(vg, nvgRGBA(192, 192, 192, 100)); //silver
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgRect(vg, 10 + SCREEN_X_OFF + 86 * 3, 60 + SCREEN_Y_OFF, 5,
                210); // the X/Y-axis coordinate relative to parent widget
        nvgFillColor(vg, nvgRGBA(192, 192, 192, 100)); //silver
        nvgFill(vg);

        nvgBeginPath(vg);
        nvgRect(vg, 10 + SCREEN_X_OFF + 86 * 4, 60 + SCREEN_Y_OFF, 5,
                210); // the X/Y-axis coordinate relative to parent widget
        nvgFillColor(vg, nvgRGBA(192, 192, 192, 100)); //silver
        nvgFill(vg);
    }

    float x = 15.0f + SCREEN_X_OFF;
    float y = 60.0f + SCREEN_Y_OFF;
    float w = 344.0f;
    float h = 210.0f;

    float samples[4] = {0};
    float sx[4], sy[4];
    float dx = w / 4.0f;
    int i;

    cJSON *root;
    if (!cjson_content)
    {
        return;
    }
    else
    {
        cjson_content[0] = 0x7b;
        root = cJSON_Parse(cjson_content);
        if (!root)
        {
            gui_log("Error parsing JSON!\r\n");
            return;
        }
    }
    // parse heartbreak array
    cJSON *hb_array = cJSON_GetObjectItemCaseSensitive(root, "heart_rate");
    if (cJSON_IsArray(hb_array))
    {
        cJSON *hb = cJSON_GetArrayItem(hb_array, 0);
        if (!hb)
        {
            gui_log("get hb_array unsuccessful\n");
        }
        else
        {
            cJSON *AM12 = cJSON_GetObjectItemCaseSensitive(hb, "AM12");
            cJSON *AM6 = cJSON_GetObjectItemCaseSensitive(hb, "AM6");
            cJSON *PM12 = cJSON_GetObjectItemCaseSensitive(hb, "PM12");
            cJSON *PM6 = cJSON_GetObjectItemCaseSensitive(hb, "PM6");
            samples[0] = AM12->valueint;
            samples[1] = AM6->valueint;
            samples[2] = PM12->valueint;
            samples[3] = PM6->valueint;
        }
    }
    // clear
    cJSON_Delete(root);

    sprintf(hr_content, "%d", (uint8_t)samples[2]);
    GUI_WIDGET_POINTER_BY_NAME(t, "HR_TXT");
    gui_text_content_set((gui_text_t *)t, hr_content, strlen(hr_content));

    for (i = 0; i < 4; i++)
    {
        sx[i] = x + i * dx;
        // sy[i] = y + h * samples[i] * 0.7f;
        sy[i] = y + h * (1 - (samples[i] - 40.0f) / 120.0f);
    }

    // Graph background
    NVGpaint bg = nvgLinearGradient(vg, x, y, x, y + h, nvgRGBA(255, 0, 0, 0), nvgRGBA(255, 0, 0, 64));
    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0]);
    for (i = 1; i < 4; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1], sx[i] - dx * 0.5f, sy[i], sx[i], sy[i]);
    }

    nvgLineTo(vg, x + w - dx, y + h);
    nvgLineTo(vg, x, y + h);
    nvgFillPaint(vg, bg);
    nvgFill(vg);

    // Graph line
    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0] + 2);
    for (i = 1; i < 4; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1] + 2, sx[i] - dx * 0.5f, sy[i] + 2, sx[i],
                    sy[i] + 2);
    }
    nvgStrokeColor(vg, nvgRGBA(255, 0, 0, 255));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0]);
    for (i = 1; i < 4; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1], sx[i] - dx * 0.5f, sy[i], sx[i], sy[i]);
    }

    nvgStrokeColor(vg, nvgRGBA(255, 0, 0, 255));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);

    // Graph sample pos
    nvgBeginPath(vg);
    for (i = 0; i < 4; i++)
    {
        nvgCircle(vg, sx[i], sy[i], 4.0f);
    }

    nvgFillColor(vg, nvgRGBA(255, 0, 0, 255));
    nvgFill(vg);
    nvgBeginPath(vg);
    for (i = 0; i < 4; i++)
    {
        nvgCircle(vg, sx[i], sy[i], 2.0f);
    }

    nvgFillColor(vg, nvgRGBA(220, 220, 220, 255));
    nvgFill(vg);

    nvgStrokeWidth(vg, 1.0f);
}
extern void *get_app_hongkong(void);
static void win_hb_exit(void)
{
    if (return_to_watchface_flag)
    {
        gui_switch_app(gui_current_app(), get_app_hongkong());
    }
    else
    {
        extern void *get_app_menu();
        gui_switch_app(gui_current_app(), get_app_menu());
    }
}

static bool canvas_hb_update(gui_canvas_img_t *canvas) //hb temperature activity
{
    if ((canvas_update_flag & 0x01) && curtain_index)
    {
        canvas_update_flag &= 0b1110;
        // gui_log("canvas_hb_update %x\n", canvas_update_flag);
        return true;
    }
    return false;
}

static bool canvas_temperature_update(gui_canvas_img_t *canvas) //hb temperature activity
{
    if ((canvas_update_flag & 0x02) && curtain_index)
    {
        canvas_update_flag &= 0b1101;
        // gui_log("canvas_temperature_update %x\n", canvas_update_flag);
        return true;
    }
    return false;
}

static bool canvas_activity_update(gui_canvas_img_t *canvas) //hb temperature activity
{
    if ((canvas_update_flag & 0x04) && curtain_index)
    {
        canvas_update_flag &= 0b1011;
        // gui_log("canvas_activity_update %x\n", canvas_update_flag);
        return true;
    }
    return false;
}

/*Define gui_app_return_array*/
const uint32_t *gui_app_return_array[] =
{
    PATH04_BIN,
    PATH05_BIN,
    PATH07_BIN,
    PATH08_BIN,
    PATH09_BIN,
    PATH11_BIN,
    PATH12_BIN,
    PATH14_BIN,
    PATH15_BIN,
    PATH16_BIN,
    PATH18_BIN,
    PATH19_BIN,
    PATH20_BIN,
    PATH22_BIN,
    PATH23_BIN,
    PATH24_BIN,
    PATH25_BIN,
};

#include "gui_return.h"
static void heart_rate_app(gui_app_t *app)
{
    gui_log("current app:%s\n", gui_current_app()->screen.name);
    win_hb = gui_win_create(&app->screen, "hb_win", 0, 0, SCREEN_WIDTH,
                            SCREEN_HEIGHT);
    gui_canvas_t *canvas_bg = gui_canvas_create(win_hb, "hb_background", 0, 0, 0, SCREEN_WIDTH,
                                                SCREEN_HEIGHT);
    gui_canvas_set_canvas_cb(canvas_bg, canvas_cb_heartbreak_bg);
    // gui_canvas_img_t *canvas_bg = gui_canvas_img_create(win_hb, "hb_background", 0, 0, 0, SCREEN_WIDTH,
    //                                             SCREEN_HEIGHT);
    // gui_canvas_img_set_canvas_cb(canvas_bg, canvas_cb_heartbreak_bg);
    // gui_canvas_img_set_update_cb(canvas_bg, NULL);

    gui_canvas_t *canvas_graph = gui_canvas_create(win_hb, "hb_graph", 0, 0, 0, SCREEN_WIDTH,
                                                   SCREEN_HEIGHT);
    gui_canvas_set_canvas_cb(canvas_graph, canvas_cb_heartbreak_graph);
    // gui_canvas_img_t *canvas_graph = gui_canvas_img_create(win_hb, "hb_graph", 0, 0, 0, SCREEN_WIDTH,
    //                                                SCREEN_HEIGHT);
    // gui_canvas_img_set_canvas_cb(canvas_graph, canvas_cb_heartbreak_graph);
    // gui_canvas_img_set_update_cb(canvas_graph, canvas_hb_update);

    gui_win_set_animate(win_hb, 2000, -1, (gui_animate_callback_t)win_hb_cb, win_hb);

    {
        char *text = "160";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", -(14 + SCREEN_X_OFF), 60 + SCREEN_Y_OFF + 70 * 0, 0,
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_RED, strlen(text), font_size);
        gui_text_mode_set(t, RIGHT);
        gui_text_type_set(t, font_size_24_bin_addr, FONT_SRC_MEMADDR);
    }
    {
        char *text = "120";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", -(14 + SCREEN_X_OFF), 60 + SCREEN_Y_OFF + 70 * 1, 0,
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_RED, strlen(text), font_size);
        gui_text_mode_set(t, RIGHT);
        gui_text_type_set(t, font_size_24_bin_addr, FONT_SRC_MEMADDR);
    }
    {
        char *text = "80";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", -(14 + SCREEN_X_OFF), 60 + SCREEN_Y_OFF + 70 * 2, 0,
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_RED, strlen(text), font_size);
        gui_text_mode_set(t, RIGHT);
        gui_text_type_set(t, font_size_24_bin_addr, FONT_SRC_MEMADDR);
    }
    {
        char *text = "12AM";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", 15 + SCREEN_X_OFF + 87 * 0,
                                        270 + SCREEN_Y_OFF - font_size, 0, font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER, strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, font_size_24_bin_addr, FONT_SRC_MEMADDR);
    }
    {
        char *text = "6AM";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", 15 + SCREEN_X_OFF + 87 * 1,
                                        270 + SCREEN_Y_OFF - font_size, 0, font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER, strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, font_size_24_bin_addr, FONT_SRC_MEMADDR);
    }
    {
        char *text = "12PM";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", 15 + SCREEN_X_OFF + 87 * 2,
                                        270 + SCREEN_Y_OFF - font_size, 0, font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER, strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, font_size_24_bin_addr, FONT_SRC_MEMADDR);
    }
    {
        char *text = "6PM";
        int font_size = 24;
        gui_text_t *t = gui_text_create(win_hb, "txt", 15 + SCREEN_X_OFF + 87 * 3,
                                        270 + SCREEN_Y_OFF - font_size, 0, font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_SILVER, strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, font_size_24_bin_addr, FONT_SRC_MEMADDR);
    }
    {
        char *text = "Current heartrate";
        int font_size = 32;
        gui_text_t *t = gui_text_create(win_hb, "txt", 30 + SCREEN_X_OFF, 270 + 30 + SCREEN_Y_OFF, 0,
                                        font_size);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(255, 255, 255), strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    }
    {
        sprintf(hr_content, "69");
        int font_size = 96;
        gui_text_t *t = gui_text_create(win_hb, "HR_TXT", 30 + SCREEN_X_OFF, 270 + 60 + SCREEN_Y_OFF, 0,
                                        font_size);
        gui_text_set(t, hr_content, GUI_FONT_SRC_BMP, gui_rgb(255, 255, 255), strlen(hr_content),
                     font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, font_size_96_bin_addr, FONT_SRC_MEMADDR);
    }
    {
        char *text = "times/min";
        int font_size = 48;
        gui_text_t *t = gui_text_create(win_hb, "txt", 30 + SCREEN_X_OFF + 100, 270 + 100 + SCREEN_Y_OFF, 0,
                                        96);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, COLOR_RED, strlen(text), font_size);
        gui_text_mode_set(t, LEFT);
        gui_text_type_set(t, font_size_48_bin_addr, FONT_SRC_MEMADDR);
    }
    gui_return_create(&app->screen, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_hb_exit, NULL);
}

void switch_heart_rate()
{
    gui_switch_app(gui_current_app(), &_app_APP_HEART_RATE);
}

static void switch_app_menu()
{
    extern void *get_app_menu();
    gui_switch_app(gui_current_app(), get_app_menu());
}

static void callback_time()
{
    int idx = tablist_tab->cur_id.x;
    gui_log("idx:%d\n", idx);
    gui_app_t *app = get_app_hongkong();
    gui_obj_t *screen = &(app->screen);
    gui_obj_tree_free(screen);
    app->ui_design(app);
    switch (idx)
    {
    case 0:
        {
            GUI_BASE(win_watch)->not_show = false;
            GUI_BASE(watchface_1)->not_show = true;
            GUI_BASE(watchface_2)->not_show = true;
        }
        break;
    case 1:
        {
            GUI_BASE(win_watch)->not_show = true;
            GUI_BASE(win_watch)->gesture = true;
            GUI_BASE(watchface_1)->not_show = false;
            GUI_BASE(watchface_2)->not_show = true;
        }
        break;
    case 2:
        {
            GUI_BASE(win_watch)->not_show = true;
            GUI_BASE(win_watch)->gesture = true;
            GUI_BASE(watchface_1)->not_show = true;
            GUI_BASE(watchface_2)->not_show = false;
        }
        break;
    default:
        break;
    }
}

/* callback_touch_long start*/
static void callback_touch_long(void *obj, gui_event_t e)
{
    gui_log("win widget long touch enter cb\n");
    gui_app_t *app = (gui_app_t *)get_app_hongkong();
    gui_obj_t *screen = &(app->screen);
    int idx = 0;
    if (!GUI_BASE(win_watch)->not_show)
    {
        idx = 0;
    }
    if (!GUI_BASE(watchface_1)->not_show)
    {
        idx = 1;
    }
    if (!GUI_BASE(watchface_2)->not_show)
    {
        idx = 2;
    }

    gui_obj_tree_free(screen);
    gui_win_t *win = gui_win_create(screen, "win", 0, 0, 320, 320);
    gui_obj_add_event_cb(win, (gui_event_cb_t)callback_time, GUI_EVENT_TOUCH_CLICKED, NULL);

    tablist_tab = gui_tabview_create(win, "tabview", 59, 84, 250, 300);
    gui_tabview_set_style(tablist_tab, CLASSIC);
    gui_tab_t *tb_watchface_2 = gui_tab_create(tablist_tab, "tb_watchface_2",   0, 0, 250, 0, 2, 0);
    gui_tab_t *tb_watchface_1 = gui_tab_create(tablist_tab, "tb_watchface_1",   0, 0, 250, 0, 1, 0);
    gui_tab_t *tb_watchface_0 = gui_tab_create(tablist_tab, "tb_watchface_0",   0, 0, 250, 0, 0, 0);
    gui_tabview_jump_tab(tablist_tab, idx, 0);
    gui_img_t *tablist_img_0 = gui_img_create_from_mem(tb_watchface_0, "img", UI_CLOCK_FACE_MAIN_BIN, 0,
                                                       0, 0, 0);
    gui_img_scale(tablist_img_0, 0.6, 0.6);
    gui_img_t *tablist_img_1 = gui_img_create_from_mem(tb_watchface_1, "img", UI_CLOCK_FACE_SNOOBY_BIN,
                                                       0, 0, 0, 0);
    gui_img_scale(tablist_img_1, 0.6, 0.6);
    gui_img_t *tablist_img_2 = gui_img_create_from_mem(tb_watchface_2, "img", UI_CLOCK_FACE_PERSON_BIN,
                                                       0, 0, 0, 0);
    gui_img_scale(tablist_img_2, 0.6, 0.6);
}
/* callback_touch_long end*/

void page_ct_clock(void *parent)
{
#if defined __WIN32
    cjson_content = read_file(filename);
    if (!cjson_content)
    {
        perror("fopen");
    }
#else
    cjson_content = gui_malloc(600);
    memcpy(cjson_content, TUYA_CJSON_BIN, 0x240);
#endif
    win_watch = gui_win_create(parent, "win_clock", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    return_to_watchface_flag = true;
    // card
    gui_tabview_t *tv = gui_tabview_create(win_watch, "clock_tv", 20 + SCREEN_X_OFF, 160 + SCREEN_Y_OFF,
                                           328, 150);
    gui_tab_t *tb_0 = gui_tab_create(tv, "tb_0", 0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_1 = gui_tab_create(tv, "tb_1", 0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_2 = gui_tab_create(tv, "tb_2", 0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_3 = gui_tab_create(tv, "tb_3", 0, 0, 0, 0, 3, 0);
    gui_tab_t *tb_4 = gui_tab_create(tv, "tb_4", 0, 0, 0, 0, 4, 0);

    gui_img_t *img_weather = gui_img_create_from_mem(tb_0, "CLOCK_CARD_WEATHER",
                                                     UI_CLOCK_CARD_WEATHER_BIN, 0, 0, 0, 0);
    gui_obj_add_event_cb(img_weather, (gui_event_cb_t)switch_app_menu, GUI_EVENT_1, NULL);
    gui_img_create_from_mem(tb_1, "CLOCK_CARD_COMPASS", UI_CLOCK_CARD_COMPASS_BIN, 0, 0, 0, 0);
    gui_img_create_from_mem(tb_2, "CLOCK_CARD_MUSIC", UI_CLOCK_CARD_MUSIC_BIN, 0, 0, 0, 0);
    gui_img_create_from_mem(tb_3, "CLOCK_CARD_ALARM", UI_CLOCK_CARD_ALARM_BIN, 0, 0, 0, 0);
    gui_img_create_from_mem(tb_4, "CLOCK_CARD_WORKOUT", UI_CLOCK_CARD_WORKOUT_BIN, 0, 0, 0, 0);
    {
        // weather condition
        gui_img_create_from_mem(img_weather, "condition_1", UI_WEATHER_CLOUDY_BIN, 28, 73, 0, 0);
        gui_img_create_from_mem(img_weather, "condition_2", UI_WEATHER_RAIN_L_BIN, 89, 73, 0, 0);
        gui_img_create_from_mem(img_weather, "condition_3", UI_WEATHER_RAIN_M_BIN, 150, 73, 0, 0);
        gui_img_create_from_mem(img_weather, "condition_4", UI_WEATHER_RAIN_S_BIN, 208, 73, 0, 0);
        gui_img_create_from_mem(img_weather, "condition_5", UI_WEATHER_SUNNY_BIN, 272, 73, 0, 0);
        char *weather_content = "TODAY";
        gui_text_t *weather_text = gui_text_create(img_weather, "",  16, 110, 0, 0);
        gui_text_set(weather_text, (void *)weather_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(weather_content),
                     24);
        gui_text_type_set(weather_text, font_size_24_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(weather_text, LEFT);

        sprintf(weekday_content, "MON.  TUE.  WED.  THU.");
        weather_text = gui_text_create(img_weather, "weather_text",  88, 110, 0, 0);
        gui_text_set(weather_text, (void *)weekday_content, GUI_FONT_SRC_BMP, gui_rgba(242, 242, 242, 255),
                     strlen(weekday_content),
                     24);
        gui_text_type_set(weather_text, font_size_24_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(weather_text, LEFT);

        sprintf(content_cur, "22°");
        weather_cur = gui_text_create(img_weather, "weather_cur",  37, 5, 0, 0);
        gui_text_set(weather_cur, (void *)content_cur, GUI_FONT_SRC_BMP, gui_rgba(124, 199, 243, 255),
                     strlen(content_cur),
                     32);
        gui_text_type_set(weather_cur, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(weather_cur, LEFT);

        sprintf(content_range, "H:37° L:18°");
        weather_range = gui_text_create(img_weather, "weather_range",  80, 5, 0, 0);
        gui_text_set(weather_range, (void *)content_range, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content_range),
                     32);
        gui_text_type_set(weather_range, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(weather_range, LEFT);
    }
    gui_img_set_animate(img_weather, 3000, -1, weather_cb, img_weather);

    // temperature
    canvas_temperature = gui_canvas_create(win_watch, "CLOCK_TEMPERATURE", 0, 16 + SCREEN_X_OFF,
                                           330 + SCREEN_Y_OFF,
                                           100, 100);
    gui_canvas_set_canvas_cb(canvas_temperature, arc_temperature_cb);
    // canvas_temperature = gui_canvas_img_create(win_watch, "CLOCK_TEMPERATURE", 0, 16 + SCREEN_X_OFF,
    //                                        330 + SCREEN_Y_OFF,
    //                                        100, 100);
    // gui_canvas_img_set_canvas_cb(canvas_temperature, arc_temperature_cb);
    // gui_canvas_img_set_update_cb(canvas_temperature, canvas_temperature_update);


    sprintf(tempera_cur_content, "22");
    temperature_cur = gui_text_create(canvas_temperature, "temperature_cur",  0, 16, 0, 0); //32
    gui_text_set(temperature_cur, (void *)tempera_cur_content, GUI_FONT_SRC_BMP,  APP_COLOR_WHITE,
                 strlen(tempera_cur_content),
                 48);
    gui_text_type_set(temperature_cur, font_size_48_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(temperature_cur, CENTER);

    sprintf(tempera_low_content, "18");
    temperature_low = gui_text_create(canvas_temperature, "temperature_low",  19, 70, 0, 0);
    gui_text_set(temperature_low, (void *)tempera_low_content, GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE, //gui_rgba(191, 220, 48, UINT8_MAX),
                 strlen(tempera_low_content),
                 32);
    gui_text_type_set(temperature_low, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(temperature_low, LEFT);

    sprintf(tempera_high_content, "37");
    temperature_high = gui_text_create(canvas_temperature, "temperature_high",  56, 70, 0, 0);
    gui_text_set(temperature_high, (void *)tempera_high_content, GUI_FONT_SRC_BMP,
                 APP_COLOR_WHITE, //gui_rgba(250, 17, 79, UINT8_MAX),
                 strlen(tempera_high_content),
                 32);
    gui_text_type_set(temperature_high, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(temperature_high, LEFT);
// #endif

    // date & time text
    sprintf(date_text_content, "SUN 0");
    date_text = gui_text_create(win_watch, "date_text",  -35, 15 + SCREEN_Y_OFF, 0, 0);
    gui_text_set(date_text, (void *)date_text_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(date_text_content),
                 40);
    gui_text_type_set(date_text, font_size_40_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(date_text, RIGHT);
    sprintf(time_text_content, "00:00");
    time_text = gui_text_create(win_watch, "time_text",  -20, 50 + SCREEN_Y_OFF, 0, 0);
    gui_text_set(time_text, (void *)time_text_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(time_text_content),
                 96);
    gui_text_type_set(time_text, font_size_96_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(time_text, RIGHT);

    // activity icon
    gui_canvas_t *canvas_move_ring = gui_canvas_create(win_watch, "CLOCK_ACTIVITY_ICON", 0,
                                                       0 + SCREEN_X_OFF, 50 + SCREEN_Y_OFF, 368,
                                                       100);
    gui_canvas_set_canvas_cb(canvas_move_ring, arc_activity_cb);
    // gui_canvas_img_t *canvas_move_ring = gui_canvas_img_create(win_watch, "CLOCK_ACTIVITY_ICON", 0, 0 + SCREEN_X_OFF, 50 + SCREEN_Y_OFF, 368,
    //                                                    100);
    // gui_canvas_img_set_canvas_cb(canvas_move_ring, arc_activity_cb);
    // gui_canvas_img_set_update_cb(canvas_move_ring, canvas_activity_update);

    // compass icon
    gui_img_t *img = gui_img_create_from_mem(win_watch, "CLOCK_COMPASS_DIAL",
                                             UI_CLOCK_COMPASS_DIAL_ICON_BIN, 134 + SCREEN_X_OFF, 330 + SCREEN_Y_OFF, 0, 0);
    compass_pointer = gui_img_create_from_mem(img, "CLOCK_COMPASS_POINTER",
                                              UI_CLOCK_COMPASS_POINTER_ICON_BIN, 42, 10, 0, 0);
    compass_degree = gui_text_create(img, "compass_degree",  5, 23, 0, 0);
    gui_text_set(compass_degree, (void *)degree_content, GUI_FONT_SRC_BMP,  gui_rgba(254, 106, 26,
                 UINT8_MAX), //orange color
                 strlen(degree_content),
                 32);
    gui_text_type_set(compass_degree, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(compass_degree, CENTER);
    compass_orien = gui_text_create(img, "compass_orien",  0, 47, 0, 0);
    gui_text_set(compass_orien, (void *)orien_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(orien_content),
                 32);
    gui_text_type_set(compass_orien, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(compass_orien, CENTER);
    gui_img_set_animate(img, 1000, -1, compass_cb, NULL);

    img_heart_rate = gui_img_create_from_mem(win_watch, "CLOCK_HEARTRATE_ICON",
                                             UI_CLOCK_HEARTRATE_ICON_BIN, 251 + SCREEN_X_OFF, 330 + SCREEN_Y_OFF, 0,
                                             0);
    gui_obj_add_event_cb(img_heart_rate, (gui_event_cb_t)switch_heart_rate, GUI_EVENT_1, NULL);

    // set other watchface
    {
        gui_win_t *win_touch = gui_win_create(parent, "win_touch", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        gui_obj_add_event_cb(win_touch, (gui_event_cb_t)callback_touch_long, GUI_EVENT_TOUCH_LONG, NULL);

        watchface_1 = gui_img_create_from_mem(parent, "watchface", UI_CLOCK_FACE_SNOOBY_BIN, 0, 0, 0, 0);
        gui_img_scale(watchface_1, 0.95, 0.95);
        watchface_2 = gui_img_create_from_mem(parent, "watchface", UI_CLOCK_FACE_PERSON_BIN, 0, 0, 0, 0);
        gui_img_scale(watchface_2, 0.95, 0.95);
        GUI_BASE(watchface_1)->not_show = true;
        GUI_BASE(watchface_2)->not_show = true;
    }
    gui_win_set_animate(win_watch, 2000, -1, (gui_animate_callback_t)win_clock_cb, win_watch);
}
#endif
