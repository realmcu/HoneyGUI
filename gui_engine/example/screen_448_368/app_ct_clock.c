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
/* page_ct_clock end*/


#include "time.h"
#include "gui_curtainview.h"
#include "tp_algo.h"
#include <math.h>
#include "cJSON.h"

#define SCREEN_WIDTH 368
#define SCREEN_HEIGHT 448

static void *font_size_96_bin_addr = SOURCEHANSANSSC_SIZE96_BITS4_FONT_BIN;
static void *font_size_64_bin_addr = SOURCEHANSANSSC_SIZE64_BITS4_FONT_BIN;
static void *font_size_48_bin_addr = SOURCEHANSANSSC_SIZE48_BITS4_FONT_BIN;
static void *font_size_40_bin_addr = SOURCEHANSANSSC_SIZE40_BITS4_FONT_BIN;
static void *font_size_32_bin_addr = SOURCEHANSANSSC_SIZE32_BITS4_FONT_BIN;
static void *font_size_24_bin_addr = SOURCEHANSANSSC_SIZE24_BITS4_FONT_BIN;

static gui_win_t *win_watch;
static gui_text_t *time_text, *date_text;
// static char time_content[10] = "00:00", date_content[10] = "Thurs 15";
static struct tm *timeinfo;

static gui_text_t *tempare_cur, *tempare_low, *tempare_high;

static gui_img_t *compass_pointer;
static gui_text_t *compass_degree, *compass_orien;
static char degree_content[5] = "0°", *orien_content = "N";

#if defined __WIN32
const char *filename =
    "./gui_engine/example/web/peripheral_simulation/json/simulation_data.json"; //gui_engine\example\web\peripheral_simulation\json\simulation_data.json
char *json_content;
bool jason_mutex = 0;
#endif
/* read CJSON to string */
char *read_file(const char *path)
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

extern char *day[];
static void refreash_time()
{
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char temp[10];
    sprintf(temp, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    gui_text_content_set(time_text, temp, strlen(temp));
    gui_text_convert_to_img(time_text, ARGB8888);

    sprintf(temp, "%s %d",  day[timeinfo->tm_wday], timeinfo->tm_mday);
    gui_text_content_set(date_text, temp, strlen(temp));
    gui_text_convert_to_img(date_text, ARGB8888);
}

static void win_clock_cb(void)
{
#if defined _WIN32
    refreash_time();
    if (!jason_mutex)
    {
        jason_mutex = 1;
        char *temp = json_content;
        json_content = read_file(filename);
        free(temp);
        if (!json_content)
        {
            perror("fopen");
        }
        jason_mutex = 0;
    }
#endif
    // blur this curtain
    gui_curtainview_t *cv = 0;
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "ct_clock",
                                    (void *)&cv); //  can find "ct_clock" in "app_tb_clock.c"
    if (cv)
    {
        if (cv->cur_curtain == CURTAIN_UP || cv->cur_curtain == CURTAIN_DOWN)
        {
            GUI_BASE(win_watch)->opacity_value = 50;
        }
        else
        {
            GUI_BASE(win_watch)->opacity_value = 255;
        }
    }

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
        if (tp->x > 50 && tp->x < 300 && tp->y > 220 && tp->y < 330)
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
            // gui_curtainview_t *cv = 0;
            // gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "ct_clock",
            //                                 (void *)&cv); //  can find "ct_clock" in "app_tb_clock.c"
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
            // gui_curtainview_t *cv = 0;
            // gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "ct_clock",
            //                                 (void *)&cv); //  can find "ct_clock" in "app_tb_clock.c"
            if (cv)
            {
                GUI_BASE(cv)->gesture = 0;
            }
        }
    }
}
static void arc_activity_move_cb(gui_canvas_t *canvas)
{
#if defined __WIN32
    if (jason_mutex)
    {
        return;
    }
    else
    {
        // imitate external inputs
        jason_mutex = 1;
        cJSON *root = cJSON_Parse(json_content);
        if (!root)
        {
            fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
            jason_mutex = 0;
            return;
        }

        // parse activity array
        cJSON *activity = cJSON_GetObjectItemCaseSensitive(root, "activity");
        if (cJSON_IsArray(activity))
        {
            cJSON *act = NULL;
            cJSON_ArrayForEach(act, activity)
            {
                cJSON *move = cJSON_GetObjectItemCaseSensitive(act, "move");
                cJSON *ex = cJSON_GetObjectItemCaseSensitive(act, "ex");
                cJSON *stand = cJSON_GetObjectItemCaseSensitive(act, "stand");
                nvgBeginPath(canvas->vg);
                nvgArc(canvas->vg, 16 + 100 / 2, 100 / 2, 50 - 8, 3 * M_PI / 2,
                       M_PI * (1.5f + 2.0f * move->valuedouble / 5000.0f), NVG_CW);  // cap 5000 steps
                nvgStrokeWidth(canvas->vg, 8);
                nvgStrokeColor(canvas->vg, nvgRGB(230, 67, 79));
                nvgStroke(canvas->vg);

                nvgBeginPath(canvas->vg);
                nvgArc(canvas->vg, 16 + 100 / 2, 100 / 2, 50 - 21, 3 * M_PI / 2,
                       M_PI * (1.5f + 2.0f * ex->valueint / 30.0f), NVG_CW);  // cap 30 min.
                nvgStrokeWidth(canvas->vg, 8);
                nvgStrokeColor(canvas->vg, nvgRGB(186, 253, 79));
                nvgStroke(canvas->vg);

                nvgBeginPath(canvas->vg);
                nvgArc(canvas->vg, 16 + 100 / 2, 100 / 2, 50 - 33, 3 * M_PI / 2,
                       M_PI * (1.5f + 2.0f * stand->valueint / 12.0f), NVG_CW); // cap 6 times
                nvgStrokeWidth(canvas->vg, 8);
                nvgStrokeColor(canvas->vg, nvgRGB(117, 230, 229));
                nvgStroke(canvas->vg);
            }
        }
        // clear
        cJSON_Delete(root);
        jason_mutex = 0;
    }
#else
    // automation
    static float progress = 0;
    progress += 0.001;
    if (progress >= 2)
    {
        progress = 0;
    }
    nvgBeginPath(canvas->vg);
    nvgArc(canvas->vg, 16 + 100 / 2, 100 / 2, 50 - 8, 3 * M_PI / 2, M_PI * (progress + 3 / 2), NVG_CW);
    nvgStrokeWidth(canvas->vg, 8);
    nvgStrokeColor(canvas->vg, nvgRGB(230, 67, 79));
    nvgStroke(canvas->vg);

    nvgBeginPath(canvas->vg);
    nvgArc(canvas->vg, 16 + 100 / 2, 100 / 2, 50 - 21, 3 * M_PI / 2, M_PI * (progress + 3 / 2), NVG_CW);
    nvgStrokeWidth(canvas->vg, 8);
    nvgStrokeColor(canvas->vg, nvgRGB(186, 253, 79));
    nvgStroke(canvas->vg);

    nvgBeginPath(canvas->vg);
    nvgArc(canvas->vg, 16 + 100 / 2, 100 / 2, 50 - 33, 3 * M_PI / 2, M_PI * (progress + 3 / 2), NVG_CW);
    nvgStrokeWidth(canvas->vg, 8);
    nvgStrokeColor(canvas->vg, nvgRGB(117, 230, 229));
    nvgStroke(canvas->vg);
#endif

}

static void arc_temperature_cb(gui_canvas_t *canvas)
{
    NVGcontext *vg = canvas->vg;
#if defined __WIN32
    if (jason_mutex)
    {
        return;
    }
    else
    {
        jason_mutex = 1;
        cJSON *root = cJSON_Parse(json_content);
        if (!root)
        {
            fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
            jason_mutex = 0;
            return;
        }
        // parse temperature array
        cJSON *temperature = cJSON_GetObjectItemCaseSensitive(root, "temperature");
        if (cJSON_IsArray(temperature))
        {
            cJSON *temp = NULL;
            cJSON_ArrayForEach(temp, temperature)
            {
                cJSON *low = cJSON_GetObjectItemCaseSensitive(temp, "low");
                cJSON *high = cJSON_GetObjectItemCaseSensitive(temp, "high");
                cJSON *cur = cJSON_GetObjectItemCaseSensitive(temp, "cur");
                uint8_t low_val = low->valueint < 18 ? 18 : low->valueint;
                uint8_t high_val = high->valueint > 37 ? 37 : high->valueint;
                uint8_t cur_val = cur->valueint;

                // draw arc
                float x = 100 / 2; // x coordinate
                float y = 100 / 2; // y coordinate
                float r = 50 - 8; // radius
                // 5/6pi ~ 13/6pi CW -->temperature range: 18 ~ 37
                float r0 = M_PI * 5 / 6;
                float r1 = M_PI * 13 / 6;

                nvgArc(canvas->vg, x, y, r, r0, r1, NVG_CW);
                nvgStrokeWidth(canvas->vg, 8);
                // start:nvgRGB(191, 220, 48), stop:nvgRGB(250, 17, 79))
                uint8_t start_r = (int)(191 + (250 - 191) * (low_val - 18) / (37 - 18));
                uint8_t start_g = (int)(220 + (17 - 220) * (low_val - 18) / (37 - 18));
                uint8_t start_b = (int)(48 + (79 - 48) * (low_val - 18) / (37 - 18));
                uint8_t stop_r = (int)(191 + (250 - 191) * (high_val - 18) / (37 - 18));
                uint8_t stop_g = (int)(220 + (17 - 220) * (high_val - 18) / (37 - 18));
                uint8_t stop_b = (int)(48 + (79 - 48) * (high_val - 18) / (37 - 18));
                NVGpaint paint = nvgLinearGradient(canvas->vg, x + cosf(r0) * r, y + sinf(r0) * r, x + cosf(r1) * r,
                                                   y + sinf(r1) * r, nvgRGB(start_r, start_g, start_b), nvgRGB(stop_r, stop_g, stop_b));
                nvgStrokePaint(canvas->vg, paint);
                nvgStroke(canvas->vg);

                // draw dot
                float temp = ((float)cur_val - (float)low_val) / ((float)high_val - (float)low_val);
                float a0 = M_PI * 5 / 6 + temp * 4 / 3 * M_PI; // 5/6pi ~ 13/6pi CW
                float ax, ay;
                nvgBeginPath(vg);
                ax = x + cosf(a0) * r;
                ay = y + sinf(a0) * r;
                nvgCircle(vg, ax, ay, 6.5f);
                nvgFillColor(vg, nvgRGB(0, 0, 0));
                nvgFill(vg);

                static char cur_content[2];
                static char low_content[2];
                static char high_content[2];
                sprintf(cur_content, "%d", cur_val);
                gui_text_content_set(tempare_cur, cur_content, strlen(cur_content));
                sprintf(low_content, "%d", low_val);
                gui_text_content_set(tempare_low, low_content, strlen(low_content));
                sprintf(high_content, "%d", high_val);
                // gui_log("%s %d\r\n", tempare_content, strlen(tempare_content));
                gui_text_content_set(tempare_high, high_content, strlen(high_content));

                uint8_t dot_r = (int)(temp * (stop_r - start_r) + start_r + 0.5);
                uint8_t dot_g = (int)(temp * (stop_g - start_g) + start_g + 0.5);
                uint8_t dot_b = (int)(temp * (stop_b - start_b) + start_b + 0.5);
                nvgBeginPath(vg);
                nvgCircle(vg, ax, ay, 5.0f);
                nvgFillColor(vg, nvgRGB(dot_r, dot_g, dot_b));
                nvgFill(vg);
            }
        }
        // clear
        cJSON_Delete(root);
        jason_mutex = 0;
    }
#else
    // automation
    float x = 100 / 2;
    float y = 100 / 2;
    float r = 50 - 8; // radius
    float r0 = M_PI * 5 / 6, r1 = M_PI * 13 / 6; // 5/6pi ~ 13/6pi CW
    nvgArc(canvas->vg, x, y, r, r0, r1, NVG_CW);
    nvgStrokeWidth(canvas->vg, 8);
    NVGpaint paint = nvgLinearGradient(canvas->vg, x + cosf(r0) * r, y + sinf(r0) * r, x + cosf(r1) * r,
                                       y + sinf(r1) * r, nvgRGB(191, 220, 48), nvgRGB(250, 17, 79));
    nvgStrokePaint(canvas->vg, paint);
    nvgStroke(canvas->vg);

    // moving dot
    static bool increasing = true;
    static float a0 = M_PI * 5 / 6; // 5/6pi ~ 13/6pi CW
    float ax, ay;
    nvgBeginPath(vg);
    ax = x + cosf(a0) * r;
    ay = y + sinf(a0) * r;
    nvgCircle(vg, ax, ay, 6.5f);
    nvgFillColor(vg, nvgRGB(0, 0, 0));
    nvgFill(vg);

    static char tempare_content[5];
    float temp = (a0 - M_PI * 5 / 6) / (M_PI * 4 / 3);
    sprintf(tempare_content, "%d", (int)(temp * (37 - 18) + 18 + 0.5));
    gui_log("%s %d\r\n", tempare_content, strlen(tempare_content));
    gui_text_content_set(tempare_cur, (void *)tempare_content, strlen(tempare_content));
    uint8_t dot_r = (int)(temp * (250 - 191) + 191 + 0.5);
    uint8_t dot_g = (int)(temp * (17 - 220) + 220 + 0.5);
    uint8_t dot_b = (int)(temp * (79 - 48) + 48 + 0.5);
    nvgBeginPath(vg);
    nvgCircle(vg, ax, ay, 5.0f);
    nvgFillColor(vg, nvgRGB(dot_r, dot_g, dot_b));
    nvgFill(vg);
    if (increasing)
    {
        a0 += 0.001;
        if (a0 >= M_PI * 13 / 6)
        {
            increasing = false;
        }
    }
    else
    {
        a0 -= 0.001;
        if (a0 <= M_PI * 5 / 6)
        {
            increasing = true;
        }
    }
#endif
}

static void compass_cb(void)
{
#if defined __WIN32
    if (jason_mutex)
    {
        return;
    }
    else
    {
        jason_mutex = 1;
        cJSON *root = cJSON_Parse(json_content);
        if (!root)
        {
            fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
            jason_mutex = 0;
            return;
        }
        // parse compass array
        cJSON *compass = cJSON_GetObjectItemCaseSensitive(root, "compass");
        if (cJSON_IsArray(compass))
        {
            cJSON *comp = NULL;
            cJSON_ArrayForEach(comp, compass)
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
                sprintf(degree_content, "%d", degree_val);
                strcat(degree_content, "°");
                gui_text_content_set(compass_degree, degree_content, strlen(degree_content));
                if (degree_val == 0)                     {orien_content = "N"; }
                else if (degree_val > 0 && degree_val < 90)    {orien_content = "NE";}
                else if (degree_val == 90)               {orien_content = "E";}
                else if (degree_val > 90 && degree_val < 180)  {orien_content = "SE";}
                else if (degree_val == 180)               {orien_content = "S";}
                else if (degree_val > 180 && degree_val < 270) {orien_content = "SW";}
                else if (degree_val == 270)              {orien_content = "W";}
                else if (degree_val > 270 && degree_val < 360) {orien_content = "NW";}
                gui_text_content_set(compass_orien, orien_content, strlen(orien_content));
            }
        }
        // clear
        cJSON_Delete(root);
        jason_mutex = 0;
    }
#else
    // automation
    float x = 100 / 2;
    float y = 100 / 2;
    float r = 35; // radius
    // moving triangle
    static uint16_t progress = 0;
    float a0 = M_PI * (float)progress / 360 * 2; // 0 ~ 2pi CW
    float ax, ay;
    ax = x + cosf(a0 + M_PI * 3 / 2) * r;
    ay = y + sinf(a0 + M_PI * 3 / 2) * r;

    gui_img_set_location(compass_pointer, (uint16_t)ax, (uint16_t)ay);
    gui_img_rotation(compass_pointer, (float)progress, gui_img_get_width(compass_pointer) / 2,
                     gui_img_get_height(compass_pointer) / 2);
    sprintf(degree_content, "%d", progress);
    strcat(degree_content, "°");
    gui_text_content_set(compass_degree, degree_content, strlen(degree_content));
    if (progress == 0)                     {orien_content = "N"; }
    else if (progress > 0 && progress < 90)    {orien_content = "NE";}
    else if (progress == 90)               {orien_content = "E";}
    else if (progress > 90 && progress < 180)  {orien_content = "SE";}
    else if (progress == 180)               {orien_content = "S";}
    else if (progress > 180 && progress < 270) {orien_content = "SW";}
    else if (progress == 270)              {orien_content = "W";}
    else if (progress > 270 && progress < 360) {orien_content = "NW";}
    gui_text_content_set(compass_orien, orien_content, strlen(orien_content));
    progress += 1;
    if (progress >= 360)
    {
        progress = 0;
    }
#endif
}

void page_ct_clock(void *parent)
{
#if defined __WIN32
    json_content = read_file(filename);
    if (!json_content)
    {
        perror("fopen");
        return;
    }
#endif
    win_watch = gui_win_create(parent, "win_clock", 0, 0, 368, 448);

    // temperature
    gui_canvas_t *canvas_temperature = gui_canvas_create(win_watch, "CLOCK_ACTIVITY_ICON", 0, 16, 330,
                                                         100, 100);
    gui_canvas_set_canvas_cb(canvas_temperature, arc_temperature_cb);
    char *content = "22";
    tempare_cur = gui_text_create(canvas_temperature, "tempare_cur",  0, 16, 0, 0); //32
    gui_text_set(tempare_cur, (void *)content, GUI_FONT_SRC_BMP,  APP_COLOR_WHITE,
                 strlen(content),
                 48);
    gui_text_type_set(tempare_cur, font_size_48_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(tempare_cur, CENTER);
    // get range number's font color
#if defined __WIN32
    cJSON *root = cJSON_Parse(json_content);
    if (!root)
    {
        fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return;
    }
    cJSON *temperature = cJSON_GetObjectItemCaseSensitive(root, "temperature");
    if (cJSON_IsArray(temperature))
    {
        cJSON *temp = NULL;
        cJSON_ArrayForEach(temp, temperature)
        {
            cJSON *low = cJSON_GetObjectItemCaseSensitive(temp, "low");
            cJSON *high = cJSON_GetObjectItemCaseSensitive(temp, "high");
            uint8_t low_val = low->valueint < 18 ? 18 : low->valueint;
            uint8_t high_val = high->valueint > 37 ? 37 : high->valueint;

            uint8_t start_r = (int)(191 + (250 - 191) * (low_val - 18) / (37 - 18));
            uint8_t start_g = (int)(220 + (17 - 220) * (low_val - 18) / (37 - 18));
            uint8_t start_b = (int)(48 + (79 - 48) * (low_val - 18) / (37 - 18));
            uint8_t stop_r = (int)(191 + (250 - 191) * (high_val - 18) / (37 - 18));
            uint8_t stop_g = (int)(220 + (17 - 220) * (high_val - 18) / (37 - 18));
            uint8_t stop_b = (int)(48 + (79 - 48) * (high_val - 18) / (37 - 18));
            content = "18";
            tempare_low = gui_text_create(canvas_temperature, "tempare_left",  19, 70, 0, 0);
            gui_text_set(tempare_low, (void *)content, GUI_FONT_SRC_BMP,  gui_rgba(start_r, start_g, start_b,
                                                                                   UINT8_MAX),
                         strlen(content),
                         32);
            gui_text_type_set(tempare_low, font_size_32_bin_addr, FONT_SRC_MEMADDR);
            gui_text_mode_set(tempare_low, LEFT);

            content = "37";
            tempare_high = gui_text_create(canvas_temperature, "tempare_high",  56, 70, 0, 0);
            gui_text_set(tempare_high, (void *)content, GUI_FONT_SRC_BMP,  gui_rgba(stop_r, stop_g, stop_b,
                                                                                    UINT8_MAX),
                         strlen(content),
                         32);
            gui_text_type_set(tempare_high, font_size_32_bin_addr, FONT_SRC_MEMADDR);
            gui_text_mode_set(tempare_high, LEFT);
        }
    }
#else
    content = "18";
    tempare_low = gui_text_create(canvas_temperature, "tempare_left",  19, 70, 0, 0);
    gui_text_set(tempare_low, (void *)content, GUI_FONT_SRC_BMP,  gui_rgba(191, 220, 48,
                                                                           UINT8_MAX),
                 strlen(content),
                 32);
    gui_text_type_set(tempare_low, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(tempare_low, LEFT);

    content = "37";
    tempare_high = gui_text_create(canvas_temperature, "tempare_high",  56, 70, 0, 0);
    gui_text_set(tempare_high, (void *)content, GUI_FONT_SRC_BMP,  gui_rgba(250, 17, 79,
                                                                            UINT8_MAX),
                 strlen(content),
                 32);
    gui_text_type_set(tempare_high, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(tempare_high, LEFT);
#endif
    // gui_img_create_from_mem(win_watch, "CLOCK_TEMPERATURE_ICON", UI_CLOCK_TEMPERATURE_ICON_BIN, 17, 330,
    //                         0, 0);

    // date & time text
    char *time_content = "00:00", *date_content = "Thurs 15";
    date_text = gui_text_create(win_watch, "date_text",  -30, 15, 0, 0);
    gui_text_set(date_text, (void *)date_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(date_content),
                 40);
    gui_text_type_set(date_text, font_size_40_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(date_text, RIGHT);

    time_text = gui_text_create(win_watch, "time_text",  -20, 50, 0, 0);
    gui_text_set(time_text, (void *)time_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(time_content),
                 96);
    gui_text_type_set(time_text, font_size_96_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(time_text, RIGHT);

    // card
    gui_tabview_t *tv = gui_tabview_create(win_watch, "clock_tv", 20, 160, 328, 150);
    gui_tab_t *tb_0 = gui_tab_create(tv, "tb_0", 0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_1 = gui_tab_create(tv, "tb_1", 0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_2 = gui_tab_create(tv, "tb_2", 0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_3 = gui_tab_create(tv, "tb_3", 0, 0, 0, 0, 3, 0);

    gui_img_create_from_mem(tb_0, "CLOCK_CARD_COMPASS", UI_CLOCK_CARD_COMPASS_BIN, 0, 0, 0, 0);
    gui_img_create_from_mem(tb_1, "CLOCK_CARD_MUSIC", UI_CLOCK_CARD_MUSIC_BIN, 0, 0, 0, 0);
    gui_img_create_from_mem(tb_2, "CLOCK_CARD_ALARM", UI_CLOCK_CARD_ALARM_BIN, 0, 0, 0, 0);
    gui_img_create_from_mem(tb_3, "CLOCK_CARD_WORKOUT", UI_CLOCK_CARD_WORKOUT_BIN, 0, 0, 0, 0);

    // activity icon
    gui_canvas_t *canvas_move_ring = gui_canvas_create(win_watch, "CLOCK_ACTIVITY_ICON", 0, 0, 50, 368,
                                                       100);
    gui_canvas_set_canvas_cb(canvas_move_ring, arc_activity_move_cb);
    // gui_img_create_from_mem(win_watch, "CLOCK_ACTIVITY_ICON", UI_CLOCK_ACTIVITY_ICON_BIN, 16, 50, 0, 0);

    // compass icon
    gui_img_t *img = gui_img_create_from_mem(win_watch, "CLOCK_COMPASS_DIAL",
                                             UI_CLOCK_COMPASS_DIAL_ICON_BIN, 134, 330, 0, 0);
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
    // gui_img_create_from_mem(win_watch, "CLOCK_COMPASS_ICON", UI_CLOCK_COMPASS_ICON_BIN, 134 + 100, 330, 0, 0);

    gui_img_create_from_mem(win_watch, "CLOCK_HEARTRATE_ICON", UI_CLOCK_HEARTRATE_ICON_BIN, 251, 330, 0,
                            0);

    gui_win_set_animate(win_watch, 1000, -1, win_clock_cb, NULL);
}
